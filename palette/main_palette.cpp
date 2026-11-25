// palette/main_palette.cpp — CWidgets selector palette.
//
// A pure-X11 window built with XFrame's own primitives (raw Xlib, same as
// every widget in the repo). Lists all 194 widgets grouped into 9 sections
// with headers (click a header to collapse/expand), each entry showing its
// 16x16 .ico icon + name. Clicking an entry selects it: the entry is
// highlighted and the selection is written to the shared selection file
// (see common/ipc.h) so the layout window can place it.
//
// Modes:
//   cwidgets_palette                 interactive palette
//   cwidgets_palette --selftest       select every entry in turn through
//                                      the same select_entry() path a mouse
//                                      click uses, log SELECT lines, exit 0
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
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
static int win_w = 380, win_h = 880;

static const int kTopBar = 30;
static const int kHeaderH = 26;
static const int kEntryH = 22;

static bool collapsed[cw::kSectionCount] = {false};
static int selected = -1;
static int scroll_y = 0;
static std::vector<XImage*> icons;  // one per widget, or nullptr
static int icons_loaded = 0;
static unsigned long col_bg, col_bar, col_text, col_sel, col_sel_text,
    col_head_text, col_border;

struct Row {
    bool is_header;
    int section;  // section index
    int widget;   // widget index, -1 for headers
    int y;        // top y in content coords
    int h;
};
static std::vector<Row> rows;
static int content_h = 0;

static void rebuild_rows() {
    rows.clear();
    int y = kTopBar;
    for (int s = 0; s < cw::kSectionCount; ++s) {
        rows.push_back({true, s, -1, y, kHeaderH});
        y += kHeaderH;
        if (collapsed[s]) continue;
        for (int i = 0; i < cw::kWidgetCount; ++i) {
            if (cw::kWidgetSection[i] != s) continue;
            rows.push_back({false, s, i, y, kEntryH});
            y += kEntryH;
        }
    }
    content_h = y;
    int max_scroll = content_h - win_h;
    if (max_scroll < 0) max_scroll = 0;
    if (scroll_y > max_scroll) scroll_y = max_scroll;
    if (scroll_y < 0) scroll_y = 0;
}

static void select_entry(int idx) {
    if (idx < 0 || idx >= cw::kWidgetCount) return;
    selected = idx;
    cw::write_selection(cw::kWidgetNames[idx], idx);
    std::string title = std::string("CWidgets Palette - selected: ") +
                        cw::kWidgetNames[idx];
    XStoreName(dpy, win, title.c_str());
    XClearWindow(dpy, win);
}

static void draw_triangle(int x, int y, bool open) {
    // Small ▸ / ▾ marker for collapsible headers.
    if (open) {
        XDrawLine(dpy, win, gc, x, y, x + 8, y);
        XDrawLine(dpy, win, gc, x + 1, y + 1, x + 7, y + 1);
        XDrawLine(dpy, win, gc, x + 2, y + 2, x + 6, y + 2);
        XDrawLine(dpy, win, gc, x + 3, y + 3, x + 5, y + 3);
        XDrawLine(dpy, win, gc, x + 4, y + 4, x + 4, y + 4);
    } else {
        XDrawLine(dpy, win, gc, x, y, x, y + 8);
        XDrawLine(dpy, win, gc, x + 1, y + 1, x + 1, y + 7);
        XDrawLine(dpy, win, gc, x + 2, y + 2, x + 2, y + 6);
        XDrawLine(dpy, win, gc, x + 3, y + 3, x + 3, y + 5);
        XDrawLine(dpy, win, gc, x + 4, y + 4, x + 4, y + 4);
    }
}

static void draw_all() {
    XSetForeground(dpy, gc, col_bg);
    XFillRectangle(dpy, win, gc, 0, 0, win_w, win_h);
    // Top bar.
    XSetForeground(dpy, gc, col_bar);
    XFillRectangle(dpy, win, gc, 0, 0, win_w, kTopBar);
    XSetForeground(dpy, gc, col_text);
    XSetFont(dpy, gc, font->fid);
    char top[128];
    snprintf(top, sizeof(top), "CWidgets Palette - %d widgets, %d sections",
             cw::kWidgetCount, cw::kSectionCount);
    XDrawString(dpy, win, gc, 10, 20, top, (int)strlen(top));

    // Clip rows to the content area so a half-scrolled row can never
    // paint over the top bar.
    XRectangle clip = {0, (short)kTopBar, (unsigned short)win_w,
                       (unsigned short)(win_h - kTopBar)};
    XSetClipRectangles(dpy, gc, 0, 0, &clip, 1, Unsorted);
    for (const Row& r : rows) {
        int y = r.y - scroll_y;
        if (y + r.h < kTopBar || y > win_h) continue;
        if (r.is_header) {
            int s = r.section;
            unsigned long acc = 0;
            // Section accent colours, same values as tools/sections.py.
            static const int kAcc[9][3] = {
                {37, 99, 235}, {8, 145, 178}, {22, 163, 74}, {147, 51, 234},
                {234, 88, 12}, {219, 39, 119}, {77, 124, 15}, {180, 83, 9},
                {220, 38, 38}};
            acc = cw::alloc_color(dpy, screen, kAcc[s][0], kAcc[s][1], kAcc[s][2]);
            XSetForeground(dpy, gc, acc);
            XFillRectangle(dpy, win, gc, 0, y, win_w, r.h);
            XSetForeground(dpy, gc, col_head_text);
            draw_triangle(10, y + 9, !collapsed[s]);
            int count = 0;
            for (int i = 0; i < cw::kWidgetCount; ++i)
                if (cw::kWidgetSection[i] == s) ++count;
            char buf[160];
            snprintf(buf, sizeof(buf), "%s  (%d)", cw::kSectionNames[s], count);
            XDrawString(dpy, win, gc, 26, y + 18, buf, (int)strlen(buf));
        } else {
            int i = r.widget;
            bool sel = (i == selected);
            if (sel) {
                XSetForeground(dpy, gc, col_sel);
                XFillRectangle(dpy, win, gc, 0, y, win_w, r.h);
            }
            XImage* img = icons[(size_t)i];
            if (img) {
                XPutImage(dpy, win, gc, img, 0, 0, 8, y + 3, 16, 16);
            } else {
                // Missing icon: red box so a failure is visible, not silent.
                XSetForeground(dpy, gc, cw::alloc_color(dpy, screen, 220, 38, 38));
                XFillRectangle(dpy, win, gc, 8, y + 3, 16, 16);
            }
            XSetForeground(dpy, gc, sel ? col_sel_text : col_text);
            const char* name = cw::kWidgetNames[i];
            XDrawString(dpy, win, gc, 32, y + 16, name, (int)strlen(name));
        }
    }
    XSetClipMask(dpy, gc, None);
    // Bottom border line + scroll hint.
    XSetForeground(dpy, gc, col_border);
    XDrawLine(dpy, win, gc, 0, kTopBar, win_w, kTopBar);
    XFlush(dpy);
}

static int row_at(int my) {
    int cy = my + scroll_y;
    for (size_t k = 0; k < rows.size(); ++k) {
        if (cy >= rows[k].y && cy < rows[k].y + rows[k].h) return (int)k;
    }
    return -1;
}

static void scroll_to_widget(int idx) {
    // Make sure the row for widget idx is visible (expand its section).
    int s = cw::kWidgetSection[idx];
    collapsed[s] = false;
    rebuild_rows();
    for (const Row& r : rows) {
        if (!r.is_header && r.widget == idx) {
            int y = r.y;
            if (y - scroll_y < kTopBar) scroll_y = y - kTopBar;
            if (y + r.h - scroll_y > win_h) scroll_y = y + r.h - win_h;
            if (scroll_y < 0) scroll_y = 0;
            break;
        }
    }
}

static std::string find_icons_dir() {
    const char* env = getenv("CWIDGETS_ICONS");
    if (env && *env) return env;
    const char* cands[] = {"icons", "../icons", "/opt/cwidgets/icons"};
    for (const char* c : cands) {
        std::string p = std::string(c) + "/GtkButton.ico";
        FILE* f = fopen(p.c_str(), "rb");
        if (f) {
            fclose(f);
            return c;
        }
    }
    return "/opt/cwidgets/icons";
}

int main(int argc, char** argv) {
    bool selftest = false;
    for (int i = 1; i < argc; ++i)
        if (std::string(argv[i]) == "--selftest") selftest = true;

    dpy = XOpenDisplay(nullptr);
    if (!dpy) {
        fprintf(stderr, "palette: cannot open display\n");
        return 1;
    }
    screen = DefaultScreen(dpy);
    Window root = RootWindow(dpy, screen);
    win = XCreateSimpleWindow(dpy, root, 60, 40, win_w, win_h, 0,
                              BlackPixel(dpy, screen), WhitePixel(dpy, screen));
    XStoreName(dpy, win, "CWidgets Palette - 194 widgets");
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
        fprintf(stderr, "palette: no font\n");
        return 1;
    }
    col_bg = WhitePixel(dpy, screen);
    col_bar = cw::alloc_color(dpy, screen, 238, 238, 240);
    col_text = BlackPixel(dpy, screen);
    col_sel = cw::alloc_color(dpy, screen, 37, 99, 235);
    col_sel_text = WhitePixel(dpy, screen);
    col_head_text = WhitePixel(dpy, screen);
    col_border = cw::alloc_color(dpy, screen, 200, 200, 200);

    // Load icons.
    std::string idir = find_icons_dir();
    icons.assign(cw::kWidgetCount, nullptr);
    Visual* vis = DefaultVisual(dpy, screen);
    int depth = DefaultDepth(dpy, screen);
    for (int i = 0; i < cw::kWidgetCount; ++i) {
        std::string path = idir + "/" + cw::kWidgetNames[i] + ".ico";
        std::vector<uint8_t> rgba;
        int w = 0, h = 0;
        if (cw::load_ico_rgba(path, rgba, w, h)) {
            icons[(size_t)i] = cw::rgba_to_ximage(dpy, vis, depth, rgba, w, h);
            if (icons[(size_t)i]) ++icons_loaded;
        } else {
            fprintf(stderr, "palette: icon missing/unreadable: %s\n", path.c_str());
        }
    }
    fprintf(stderr, "palette: icons loaded %d/%d from %s\n", icons_loaded,
            cw::kWidgetCount, idir.c_str());
    if (icons_loaded != cw::kWidgetCount) {
        fprintf(stderr, "palette: FATAL: not all icons loaded\n");
        return 1;
    }

    rebuild_rows();
    draw_all();

    if (selftest) {
        // Drive every selection through the same select_entry() path a
        // mouse click uses, and verify the selection file after each one.
        int fails = 0;
        for (int i = 0; i < cw::kWidgetCount; ++i) {
            scroll_to_widget(i);
            select_entry(i);
            draw_all();
            std::string name;
            int idx = -1;
            if (!cw::read_selection(name, idx) || name != cw::kWidgetNames[i] ||
                idx != i) {
                printf("SELECT %d %s FAIL (file=%s idx=%d)\n", i,
                       cw::kWidgetNames[i], name.c_str(), idx);
                ++fails;
            } else {
                printf("SELECT %d %s OK\n", i, cw::kWidgetNames[i]);
            }
            fflush(stdout);
        }
        printf("SELFTEST %d/%d selections OK\n", cw::kWidgetCount - fails,
               cw::kWidgetCount);
        return fails ? 1 : 0;
    }

    bool running = true;
    while (running) {
        XEvent ev;
        XNextEvent(dpy, &ev);
        switch (ev.type) {
            case Expose:
                draw_all();
                break;
            case ConfigureNotify:
                win_w = ev.xconfigure.width;
                win_h = ev.xconfigure.height;
                rebuild_rows();
                draw_all();
                break;
            case ButtonPress: {
                int b = ev.xbutton.button;
                if (b == 4) {
                    scroll_y -= 44;
                    if (scroll_y < 0) scroll_y = 0;
                    draw_all();
                } else if (b == 5) {
                    scroll_y += 44;
                    rebuild_rows();
                    draw_all();
                } else if (b == 1) {
                    int k = row_at(ev.xbutton.y);
                    if (k >= 0) {
                        if (rows[(size_t)k].is_header) {
                            collapsed[rows[(size_t)k].section] =
                                !collapsed[rows[(size_t)k].section];
                            rebuild_rows();
                        } else {
                            select_entry(rows[(size_t)k].widget);
                        }
                        draw_all();
                    }
                }
                break;
            }
            case KeyPress: {
                char buf[32];
                KeySym key = 0;
                XLookupString(&ev.xkey, buf, sizeof(buf) - 1, &key, nullptr);
                if (key == XK_Up) {
                    if (selected > 0) {
                        scroll_to_widget(selected - 1);
                        select_entry(selected - 1);
                        draw_all();
                    }
                } else if (key == XK_Down) {
                    if (selected < cw::kWidgetCount - 1) {
                        scroll_to_widget(selected + 1);
                        select_entry(selected + 1);
                        draw_all();
                    }
                } else if (key == XK_Page_Down) {
                    scroll_y += 400;
                    rebuild_rows();
                    draw_all();
                } else if (key == XK_Page_Up) {
                    scroll_y -= 400;
                    if (scroll_y < 0) scroll_y = 0;
                    draw_all();
                } else if (key == XK_Home) {
                    scroll_y = 0;
                    draw_all();
                } else if (key == XK_End) {
                    rebuild_rows();
                    draw_all();
                } else if (key == XK_q || key == XK_Escape) {
                    running = false;
                }
                break;
            }
            case ClientMessage:
                if ((Atom)ev.xclient.data.l[0] == del) running = false;
                break;
        }
    }
    XCloseDisplay(dpy);
    return 0;
}
