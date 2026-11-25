// layout/main_layout.cpp — CWidgets layout/design window.
//
// A pure-X11 window built with XFrame's own primitives. It instantiates
// the REAL XFrame widget classes (via cw::create_widget in
// common/registry.h) — no placeholders. The widget type selected in the
// palette (shared selection file, see common/ipc.h) is placed with a
// click, then can be selected, dragged to move, and deleted. Double-click
// a placed widget to show resize handles (corners + edges); dragging a
// handle resizes the widget live; clicking elsewhere or Escape dismisses.
//
// Interactive keys: Delete/BackSpace delete, Escape dismiss handles /
// clear selection, s save layout, q quit.
//
// Script mode (for automated testing):
//   cwidgets_layout --script cmds.txt --log out.log
// Commands (one per line, # comments):
//   PLACE <Type> [x y w h]   place a widget (defaults: centred, default size)
//   PLACE_ALL                place all registered types in a grid
//   MOVE <id> <dx> <dy>      move a placed widget
//   RESIZE <id> <w> <h>      resize a placed widget
//   DELETE <id>              delete a placed widget
//   SELECT <id>              select a placed widget
//   DBLCLICK <id>            show resize handles on a placed widget
//   HANDLE <nw|n|ne|e|se|s|sw|w> <dx> <dy>
//                            drag that resize handle by (dx,dy)
//   CHECKDRAW <id>           count non-white pixels in the widget rect
//                            (GtkInvisible is expected BLANK by design)
//   SAVE <file> / LOAD <file>
//   LIST / COUNT / QUIT
// Every mutation rewrites the layout state file (common/ipc.h) so a test
// driver can verify bounds without guessing from pixels.
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <sys/select.h>
#include <sys/time.h>
#include <time.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "common/ico.h"
#include "common/ipc.h"
#include "common/registry.h"

static Display* dpy;
static int screen;
static Window win;
static GC gc;
static XFontStruct* font;
static int win_w = 1024, win_h = 720;
static const int kStatusH = 22;
static SStyle_Manager* styles = nullptr;

struct Placed {
    int id;
    int type;  // index into cw::kWidgetNames
    int x, y, w, h;
    std::unique_ptr<cw::IWidget> wdg;
};
static std::vector<Placed> placed;
static int next_id = 1;
static int last_placed_id = -1;
static int selected_id = -1;   // -1 = none
static bool handles_on = false;
static int sel_type = -1;      // type selected in the palette
static std::string sel_name;
static long sel_mtime = -1;

static unsigned long col_bg, col_grid, col_sel, col_handle, col_handle_bdr,
    col_status, col_text;

static long now_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

// --- handles ------------------------------------------------------------
enum { H_NW = 0, H_N, H_NE, H_E, H_SE, H_S, H_SW, H_W };
static const int kHandleSize = 9;

static Placed* find_placed(int id) {
    for (auto& p : placed)
        if (p.id == id) return &p;
    return nullptr;
}

static void handle_rect(const Placed& p, int h, int& hx, int& hy) {
    int cx = p.x + p.w / 2, cy = p.y + p.h / 2;
    int xs[8] = {p.x, cx, p.x + p.w, p.x + p.w,
                 p.x + p.w, cx, p.x, p.x};
    int ys[8] = {p.y, p.y, p.y, cy,
                 p.y + p.h, p.y + p.h, p.y + p.h, cy};
    hx = xs[h] - kHandleSize / 2;
    hy = ys[h] - kHandleSize / 2;
}

static int handle_at(int mx, int my) {
    if (!handles_on || selected_id < 0) return -1;
    Placed* p = find_placed(selected_id);
    if (!p) return -1;
    for (int h = 0; h < 8; ++h) {
        int hx, hy;
        handle_rect(*p, h, hx, hy);
        if (mx >= hx - 2 && mx <= hx + kHandleSize + 2 && my >= hy - 2 &&
            my <= hy + kHandleSize + 2)
            return h;
    }
    return -1;
}

static void apply_handle_drag(Placed& p, int h, int dx, int dy) {
    int x = p.x, y = p.y, w = p.w, hh = p.h;
    switch (h) {
        case H_NW: x += dx; y += dy; w -= dx; hh -= dy; break;
        case H_N: y += dy; hh -= dy; break;
        case H_NE: y += dy; w += dx; hh -= dy; break;
        case H_E: w += dx; break;
        case H_SE: w += dx; hh += dy; break;
        case H_S: hh += dy; break;
        case H_SW: x += dx; w -= dx; hh += dy; break;
        case H_W: x += dx; w -= dx; break;
    }
    if (w < 24) w = 24;
    if (hh < 16) hh = 16;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    p.x = x;
    p.y = y;
    p.w = w;
    p.h = hh;
    p.wdg->set_bounds(x, y, w, hh);
}

// --- state ---------------------------------------------------------------
static void write_state() {
    FILE* f = fopen(cw::layout_state_path().c_str(), "w");
    if (!f) return;
    for (const auto& p : placed) {
        fprintf(f, "%d %s %d %d %d %d selected=%d handles=%d\n", p.id,
                cw::kWidgetNames[p.type], p.x, p.y, p.w, p.h,
                p.id == selected_id ? 1 : 0, handles_on && p.id == selected_id ? 1 : 0);
    }
    fclose(f);
}

static void redraw() {
    XSetForeground(dpy, gc, col_bg);
    XFillRectangle(dpy, win, gc, 0, 0, win_w, win_h);
    // Dotted canvas grid.
    XSetForeground(dpy, gc, col_grid);
    for (int gx = 24; gx < win_w; gx += 24)
        for (int gy = 24; gy < win_h - kStatusH; gy += 24)
            XDrawPoint(dpy, win, gc, gx, gy);
    for (auto& p : placed) p.wdg->draw();
    // Selection outline + handles.
    if (selected_id >= 0) {
        Placed* p = find_placed(selected_id);
        if (p) {
            XSetForeground(dpy, gc, col_sel);
            XSetLineAttributes(dpy, gc, 1, LineOnOffDash, CapButt, JoinMiter);
            XDrawRectangle(dpy, win, gc, p->x - 2, p->y - 2, p->w + 4, p->h + 4);
            XSetLineAttributes(dpy, gc, 1, LineSolid, CapButt, JoinMiter);
            if (handles_on) {
                for (int h = 0; h < 8; ++h) {
                    int hx, hy;
                    handle_rect(*p, h, hx, hy);
                    XSetForeground(dpy, gc, col_handle);
                    XFillRectangle(dpy, win, gc, hx, hy, kHandleSize, kHandleSize);
                    XSetForeground(dpy, gc, col_handle_bdr);
                    XDrawRectangle(dpy, win, gc, hx, hy, kHandleSize - 1,
                                   kHandleSize - 1);
                }
            }
        }
    }
    // Status bar.
    XSetForeground(dpy, gc, col_status);
    XFillRectangle(dpy, win, gc, 0, win_h - kStatusH, win_w, kStatusH);
    XSetForeground(dpy, gc, col_text);
    XSetFont(dpy, gc, font->fid);
    char buf[256];
    const char* sel = (sel_type >= 0) ? cw::kWidgetNames[sel_type] : "(none — pick in palette)";
    snprintf(buf, sizeof(buf),
             "layout: %d widget(s)  |  selected to place: %s  |  "
             "click=place  drag=move  double-click=resize handles  "
             "Del=delete  s=save  Esc=dismiss",
             (int)placed.size(), sel);
    XDrawString(dpy, win, gc, 8, win_h - 7, buf, (int)strlen(buf));
    XFlush(dpy);
}

static int place_widget(int type, int x, int y, int w, int h) {
    if (type < 0 || type >= cw::kWidgetCount) return -1;
    if (w <= 0) w = cw::kWidgetDefaultW[type];
    if (h <= 0) h = cw::kWidgetDefaultH[type];
    if (x < 0) x = (win_w - w) / 2;
    if (y < 0) y = (win_h - kStatusH - h) / 2;
    auto wdg = cw::create_widget(type, dpy, screen, win, gc, styles, x, y, w, h);
    if (!wdg) return -1;
    int id = next_id++;
    last_placed_id = id;
    placed.push_back({id, type, x, y, w, h, std::move(wdg)});
    selected_id = id;
    handles_on = false;
    write_state();
    return id;
}

static bool delete_widget(int id) {
    for (size_t i = 0; i < placed.size(); ++i) {
        if (placed[i].id == id) {
            placed.erase(placed.begin() + (long)i);
            if (selected_id == id) {
                selected_id = -1;
                handles_on = false;
            }
            // LAST tracks the most recently placed *surviving* widget.
            if (last_placed_id == id)
                last_placed_id = placed.empty() ? -1 : placed.back().id;
            write_state();
            return true;
        }
    }
    return false;
}

static int count_nonwhite(int x, int y, int w, int h) {
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > win_w) w = win_w - x;
    if (y + h > win_h) h = win_h - y;
    if (w <= 0 || h <= 0) return -1;
    XImage* img = XGetImage(dpy, win, x, y, (unsigned)w, (unsigned)h,
                            AllPlanes, ZPixmap);
    if (!img) return -1;
    int n = 0;
    for (int yy = 0; yy < h; ++yy) {
        for (int xx = 0; xx < w; ++xx) {
            unsigned long px = XGetPixel(img, xx, yy);
            int r = (int)((px >> 16) & 0xff), g = (int)((px >> 8) & 0xff),
                b = (int)(px & 0xff);
            if (r < 250 || g < 250 || b < 250) ++n;
        }
    }
    XDestroyImage(img);
    return n;
}

static bool save_layout(const std::string& path) {
    FILE* f = fopen(path.c_str(), "w");
    if (!f) return false;
    for (const auto& p : placed)
        fprintf(f, "%s %d %d %d %d\n", cw::kWidgetNames[p.type], p.x, p.y, p.w, p.h);
    fclose(f);
    return true;
}

static int load_layout(const std::string& path) {
    std::ifstream in(path);
    if (!in) return -1;
    placed.clear();
    selected_id = -1;
    handles_on = false;
    int n = 0;
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        std::string name;
        int x, y, w, h;
        if (!(ss >> name >> x >> y >> w >> h)) continue;
        int t = cw::index_of(name);
        if (t < 0) continue;
        if (place_widget(t, x, y, w, h) >= 0) ++n;
    }
    write_state();
    return n;
}

static void poll_selection() {
    long mt = cw::file_mtime(cw::selection_path());
    if (mt < 0 || mt == sel_mtime) return;
    sel_mtime = mt;
    std::string name;
    int idx = -1;
    if (cw::read_selection(name, idx) && idx >= 0) {
        sel_type = idx;
        sel_name = name;
        std::string title = std::string("CWidgets Layout — place: ") + name;
        XStoreName(dpy, win, title.c_str());
        redraw();
    }
}

// --- script mode ---------------------------------------------------------
static int handle_name_to_id(const std::string& s) {
    if (s == "nw") return H_NW;
    if (s == "n") return H_N;
    if (s == "ne") return H_NE;
    if (s == "e") return H_E;
    if (s == "se") return H_SE;
    if (s == "s") return H_S;
    if (s == "sw") return H_SW;
    if (s == "w") return H_W;
    return -1;
}

static int resolve_id(const std::string& tok, int& id) {
    // Script ids may be a number or LAST (most recently placed widget).
    if (tok == "LAST") {
        if (last_placed_id < 0) return -1;
        id = last_placed_id;
        return 0;
    }
    try {
        id = std::stoi(tok);
    } catch (...) {
        return -1;
    }
    return 0;
}

static int run_script(const std::string& path, FILE* log) {
    std::ifstream in(path);
    if (!in) {
        fprintf(log, "FAIL script file not found: %s\n", path.c_str());
        return 1;
    }
    int fails = 0, cmds = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line[0] == '#') continue;
        std::istringstream ss(line);
        std::string cmd;
        if (!(ss >> cmd)) continue;
        ++cmds;
        auto fail = [&](const std::string& why) {
            fprintf(log, "FAIL %s : %s\n", line.c_str(), why.c_str());
            ++fails;
        };
        if (cmd == "PLACE") {
            std::string name;
            int x = -1, y = -1, w = 0, h = 0;
            ss >> name >> x >> y >> w >> h;
            int t = cw::index_of(name);
            if (t < 0) { fail("unknown widget"); continue; }
            int id = place_widget(t, x, y, w, h);
            if (id < 0) { fail("create_widget returned null"); continue; }
            redraw();
            XSync(dpy, False);
            fprintf(log, "OK PLACE %s id=%d %d %d %d %d\n", name.c_str(), id,
                    placed.back().x, placed.back().y, placed.back().w,
                    placed.back().h);
        } else if (cmd == "PLACE_ALL") {
            int n = 0;
            for (int t = 0; t < cw::kWidgetCount; ++t) {
                int x = (t % 4) * 260 + 10, y = (t / 4) * 130 + 10;
                if (place_widget(t, x, y, 240, 110) >= 0) ++n;
            }
            redraw();
            XSync(dpy, False);
            fprintf(log, "OK PLACE_ALL %d/%d\n", n, cw::kWidgetCount);
            if (n != cw::kWidgetCount) { fail("not all placed"); }
        } else if (cmd == "MOVE") {
            std::string tok; int dx, dy; int id;
            ss >> tok >> dx >> dy;
            if (resolve_id(tok, id) != 0) { fail("bad id"); continue; }
            Placed* p = find_placed(id);
            if (!p) { fail("no such id"); continue; }
            p->x += dx;
            p->y += dy;
            p->wdg->set_bounds(p->x, p->y, p->w, p->h);
            write_state();
            redraw();
            XSync(dpy, False);
            fprintf(log, "OK MOVE %d -> %d %d\n", id, p->x, p->y);
        } else if (cmd == "RESIZE") {
            std::string tok; int w, h; int id;
            ss >> tok >> w >> h;
            if (resolve_id(tok, id) != 0) { fail("bad id"); continue; }
            Placed* p = find_placed(id);
            if (!p) { fail("no such id"); continue; }
            p->w = w;
            p->h = h;
            p->wdg->set_bounds(p->x, p->y, w, h);
            write_state();
            redraw();
            XSync(dpy, False);
            fprintf(log, "OK RESIZE %d -> %d %d\n", id, w, h);
        } else if (cmd == "DELETE") {
            std::string tok; int id;
            ss >> tok;
            if (resolve_id(tok, id) != 0) { fail("bad id"); continue; }
            if (!delete_widget(id)) { fail("no such id"); continue; }
            redraw();
            XSync(dpy, False);
            fprintf(log, "OK DELETE %d count=%d\n", id, (int)placed.size());
        } else if (cmd == "SELECT") {
            std::string tok; int id;
            ss >> tok;
            if (resolve_id(tok, id) != 0) { fail("bad id"); continue; }
            if (!find_placed(id)) { fail("no such id"); continue; }
            selected_id = id;
            handles_on = false;
            write_state();
            redraw();
            fprintf(log, "OK SELECT %d\n", id);
        } else if (cmd == "DBLCLICK") {
            std::string tok; int id;
            ss >> tok;
            if (resolve_id(tok, id) != 0) { fail("bad id"); continue; }
            Placed* p = find_placed(id);
            if (!p) { fail("no such id"); continue; }
            selected_id = id;
            handles_on = true;
            write_state();
            redraw();
            XSync(dpy, False);
            fprintf(log, "OK DBLCLICK %d handles=on\n", id);
        } else if (cmd == "HANDLE") {
            std::string hn;
            int dx, dy;
            ss >> hn >> dx >> dy;
            int h = handle_name_to_id(hn);
            if (h < 0) { fail("bad handle name"); continue; }
            if (!handles_on || selected_id < 0) { fail("handles not shown"); continue; }
            Placed* p = find_placed(selected_id);
            if (!p) { fail("no selected widget"); continue; }
            int ox = p->x, oy = p->y, ow = p->w, oh = p->h;
            apply_handle_drag(*p, h, dx, dy);
            write_state();
            redraw();
            XSync(dpy, False);
            fprintf(log, "OK HANDLE %s %d %d : %d %d %d %d -> %d %d %d %d\n",
                    hn.c_str(), dx, dy, ox, oy, ow, oh, p->x, p->y, p->w, p->h);
        } else if (cmd == "CHECKDRAW") {
            std::string tok; int id;
            ss >> tok;
            if (resolve_id(tok, id) != 0) { fail("bad id"); continue; }
            Placed* p = find_placed(id);
            if (!p) { fail("no such id"); continue; }
            redraw();
            XSync(dpy, False);
            int n = count_nonwhite(p->x, p->y, p->w, p->h);
            if (n < 0) { fail("XGetImage failed"); continue; }
            const char* verdict = "DRAWN";
            if (n == 0) {
                // GtkInvisible is blank by design (phase 1 result).
                if (std::string(cw::kWidgetNames[p->type]) == "GtkInvisible")
                    verdict = "BLANK-EXPECTED";
                else {
                    fail("widget drew nothing (0 non-white pixels)");
                    continue;
                }
            }
            fprintf(log, "OK CHECKDRAW %d %s nonwhite=%d %s\n", id,
                    cw::kWidgetNames[p->type], n, verdict);
        } else if (cmd == "SAVE") {
            std::string f;
            ss >> f;
            if (!save_layout(f)) { fail("save failed"); continue; }
            fprintf(log, "OK SAVE %s count=%d\n", f.c_str(), (int)placed.size());
        } else if (cmd == "LOAD") {
            std::string f;
            ss >> f;
            int n = load_layout(f);
            if (n < 0) { fail("load failed"); continue; }
            redraw();
            XSync(dpy, False);
            fprintf(log, "OK LOAD %s count=%d\n", f.c_str(), n);
        } else if (cmd == "COUNT") {
            fprintf(log, "OK COUNT %d\n", (int)placed.size());
        } else if (cmd == "HOLD") {
            // Keep the window alive N seconds (for screenshots of a
            // scripted state); redraws once a second.
            int secs = 5;
            ss >> secs;
            for (int i = 0; i < secs * 10; ++i) {
                redraw();
                XSync(dpy, False);
                struct timespec ts = {0, 100000000};
                nanosleep(&ts, nullptr);
            }
            fprintf(log, "OK HOLD %d\n", secs);
        } else if (cmd == "LIST") {
            for (const auto& p : placed)
                fprintf(log, "LIST %d %s %d %d %d %d\n", p.id,
                        cw::kWidgetNames[p.type], p.x, p.y, p.w, p.h);
            fprintf(log, "OK LIST %d\n", (int)placed.size());
        } else if (cmd == "QUIT") {
            fprintf(log, "OK QUIT\n");
            break;
        } else {
            fail("unknown command");
        }
        fflush(log);
    }
    fprintf(log, "SCRIPT %d commands, %d failures\n", cmds, fails);
    fflush(log);
    return fails ? 1 : 0;
}

int main(int argc, char** argv) {
    std::string script, logpath;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--script" && i + 1 < argc) script = argv[++i];
        if (std::string(argv[i]) == "--log" && i + 1 < argc) logpath = argv[++i];
    }

    dpy = XOpenDisplay(nullptr);
    if (!dpy) {
        fprintf(stderr, "layout: cannot open display\n");
        return 1;
    }
    screen = DefaultScreen(dpy);
    Window root = RootWindow(dpy, screen);
    win = XCreateSimpleWindow(dpy, root, 460, 40, win_w, win_h, 0,
                              BlackPixel(dpy, screen), WhitePixel(dpy, screen));
    XStoreName(dpy, win, "CWidgets Layout");
    XSelectInput(dpy, win,
                 ExposureMask | KeyPressMask | StructureNotifyMask |
                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom del = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, win, &del, 1);
    XMapWindow(dpy, win);
    gc = XCreateGC(dpy, win, 0, nullptr);
    font = XLoadQueryFont(dpy, "-misc-fixed-medium-r-normal--13-*");
    if (!font) font = XLoadQueryFont(dpy, "fixed");
    if (!font) {
        fprintf(stderr, "layout: no font\n");
        return 1;
    }
    col_bg = WhitePixel(dpy, screen);
    col_grid = cw::alloc_color(dpy, screen, 225, 225, 228);
    col_sel = cw::alloc_color(dpy, screen, 37, 99, 235);
    col_handle = cw::alloc_color(dpy, screen, 255, 127, 0);
    col_handle_bdr = BlackPixel(dpy, screen);
    col_status = cw::alloc_color(dpy, screen, 238, 238, 240);
    col_text = BlackPixel(dpy, screen);

    styles = new SStyle_Manager(dpy, win, screen, gc);
    styles->add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120},
                         Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1,
                         "-misc-fixed-medium-r-normal--13-*");
    styles->add_style_ex("dark", Srgb{45, 45, 48}, Srgb{20, 20, 22},
                         Srgb{200, 200, 200}, Srgb{240, 240, 240}, 6, 1,
                         "-misc-fixed-medium-r-normal--13-*");
    styles->set_active_style_byName("default");
    redraw();

    if (!script.empty()) {
        FILE* log = stdout;
        if (!logpath.empty()) {
            log = fopen(logpath.c_str(), "w");
            if (!log) log = stdout;
        }
        int rc = run_script(script, log);
        if (log != stdout) fclose(log);
        XCloseDisplay(dpy);
        return rc;
    }

    // Interactive mode.
    poll_selection();
    bool running = true;
    bool dragging = false, handle_drag = false;
    int drag_handle = -1, grab_dx = 0, grab_dy = 0;
    int drag_start_x = 0, drag_start_y = 0;
    long last_press_ms = 0;
    int last_press_id = -1;
    int fd = ConnectionNumber(dpy);
    while (running) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        struct timeval tv = {0, 100000};  // 100 ms poll for palette selection
        int n = select(fd + 1, &rfds, nullptr, nullptr, &tv);
        if (n == 0) {
            poll_selection();
            continue;
        }
        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);
            switch (ev.type) {
                case Expose:
                    redraw();
                    break;
                case ConfigureNotify:
                    win_w = ev.xconfigure.width;
                    win_h = ev.xconfigure.height;
                    redraw();
                    break;
                case ButtonPress: {
                    int mx = ev.xbutton.x, my = ev.xbutton.y;
                    if (ev.xbutton.button != 1) break;
                    int h = handle_at(mx, my);
                    if (h >= 0) {
                        handle_drag = true;
                        drag_handle = h;
                        drag_start_x = mx;
                        drag_start_y = my;
                        break;
                    }
                    // Topmost placed widget containing the point.
                    Placed* hit = nullptr;
                    for (auto it = placed.rbegin(); it != placed.rend(); ++it) {
                        if (mx >= it->x && mx <= it->x + it->w && my >= it->y &&
                            my <= it->y + it->h) {
                            hit = &(*it);
                            break;
                        }
                    }
                    long t = now_ms();
                    if (hit) {
                        bool dbl = (hit->id == last_press_id &&
                                    t - last_press_ms < 450);
                        last_press_ms = t;
                        last_press_id = hit->id;
                        if (dbl) {
                            selected_id = hit->id;
                            handles_on = true;
                            write_state();
                            redraw();
                            break;
                        }
                        selected_id = hit->id;
                        handles_on = false;
                        dragging = true;
                        grab_dx = mx - hit->x;
                        grab_dy = my - hit->y;
                        write_state();
                        redraw();
                    } else {
                        last_press_ms = t;
                        last_press_id = -1;
                        if (handles_on || selected_id >= 0) {
                            handles_on = false;
                            selected_id = -1;
                            write_state();
                            redraw();
                        }
                        if (sel_type >= 0 && my < win_h - kStatusH) {
                            int w = cw::kWidgetDefaultW[sel_type];
                            int hh = cw::kWidgetDefaultH[sel_type];
                            int id = place_widget(sel_type, mx - w / 2,
                                                  my - hh / 2, w, hh);
                            if (id >= 0) redraw();
                        }
                    }
                    break;
                }
                case MotionNotify: {
                    int mx = ev.xmotion.x, my = ev.xmotion.y;
                    if (handle_drag && selected_id >= 0) {
                        Placed* p = find_placed(selected_id);
                        if (p) {
                            apply_handle_drag(*p, drag_handle, mx - drag_start_x,
                                              my - drag_start_y);
                            drag_start_x = mx;
                            drag_start_y = my;
                            write_state();
                            redraw();
                        }
                    } else if (dragging && selected_id >= 0) {
                        Placed* p = find_placed(selected_id);
                        if (p) {
                            p->x = mx - grab_dx;
                            p->y = my - grab_dy;
                            if (p->x < 0) p->x = 0;
                            if (p->y < 0) p->y = 0;
                            p->wdg->set_bounds(p->x, p->y, p->w, p->h);
                            write_state();
                            redraw();
                        }
                    } else {
                        // Forward motion to the hovered widget (hover states).
                        for (auto& p : placed) {
                            if (p.wdg->contains(mx, my)) p.wdg->on_motion(mx, my);
                        }
                    }
                    break;
                }
                case ButtonRelease: {
                    if (handle_drag || dragging) {
                        write_state();
                        redraw();
                    }
                    // Forward release to the selected widget (click states).
                    if (selected_id >= 0) {
                        Placed* p = find_placed(selected_id);
                        if (p)
                            p->wdg->on_button_release(ev.xbutton.x,
                                                      ev.xbutton.y, 1);
                    }
                    dragging = false;
                    handle_drag = false;
                    drag_handle = -1;
                    break;
                }
                case KeyPress: {
                    char buf[32];
                    KeySym key = 0;
                    XLookupString(&ev.xkey, buf, sizeof(buf) - 1, &key, nullptr);
                    if (key == XK_Delete || key == XK_BackSpace) {
                        if (selected_id >= 0) {
                            delete_widget(selected_id);
                            redraw();
                        }
                    } else if (key == XK_Escape) {
                        handles_on = false;
                        selected_id = -1;
                        write_state();
                        redraw();
                    } else if (key == XK_s) {
                        save_layout("/tmp/cwidgets-layout.txt");
                        redraw();
                    } else if (key == XK_q) {
                        running = false;
                    }
                    break;
                }
                case ClientMessage:
                    if ((Atom)ev.xclient.data.l[0] == del) running = false;
                    break;
            }
        }
        poll_selection();
    }
    XCloseDisplay(dpy);
    return 0;
}
