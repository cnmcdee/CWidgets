// Headless logic + pixel + repertoire checks for Instrumentation Group G.
// Builds with -Wall -Wextra, runs under Xvfb. Attribution: XFrame.
#include "InstSeg14Display1/InstSeg14Display1.h"
#include "InstSeg14Display2/InstSeg14Display2.h"
#include "InstSeg14Display3/InstSeg14Display3.h"
#include "InstSeg14Display4/InstSeg14Display4.h"
#include "InstSeg14Display5/InstSeg14Display5.h"
#include "InstSeg14Display6/InstSeg14Display6.h"
#include "InstSeg16Display1/InstSeg16Display1.h"
#include "InstSeg16Display2/InstSeg16Display2.h"
#include "InstSeg16Display3/InstSeg16Display3.h"
#include "InstSeg16Display4/InstSeg16Display4.h"
#include "InstSeg16Display5/InstSeg16Display5.h"
#include "InstSeg16Display6/InstSeg16Display6.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>

static Display* g_d = nullptr;
static Window g_w = 0;
static int g_fail = 0;

static void check(bool ok, const char* what) {
    std::printf("  %s %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) ++g_fail;
}

static int bright_amber(XImage* im) {
    int n = 0;
    for (int y = 0; y < im->height; ++y)
        for (int x = 0; x < im->width; ++x) {
            unsigned long p = XGetPixel(im, x, y);
            int r = (p >> 16) & 0xff, g = (p >> 8) & 0xff, b = p & 0xff;
            if (r > 200 && g > 120 && g < 220 && b < 80) ++n;
        }
    return n;
}

static int img_diff(XImage* a, XImage* b) {
    int n = 0;
    int w = a->width < b->width ? a->width : b->width;
    int h = a->height < b->height ? a->height : b->height;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            unsigned long pa = XGetPixel(a, x, y), pb = XGetPixel(b, x, y);
            int dr = abs((int)((pa >> 16) & 0xff) - (int)((pb >> 16) & 0xff));
            int dg = abs((int)((pa >> 8) & 0xff) - (int)((pb >> 8) & 0xff));
            int db = abs((int)(pa & 0xff) - (int)(pb & 0xff));
            if (dr > 30 || dg > 30 || db > 30) ++n;
        }
    return n;
}

static XImage* snap() {
    XSync(g_d, False);
    return XGetImage(g_d, g_w, 0, 0, 900, 700, AllPlanes, ZPixmap);
}

template <typename W>
static void check_seg(const char* name, W& w, int n, int seg, const char* demo) {
    std::printf("[%s]\n", name);
    check(W::kDigits == n, "digit count matches");
    check(W::kSegments == seg, "segment count matches");
    check(std::string(W::default_value()) == demo, "default demo string");
    w.set_value(demo);
    check(w.get_value() == demo, "demo value round-trips");
    w.set_value("ABCDEFGH");
    check(w.cell_count() == n, "long value truncates to N cells");
    w.set_value("hello");
    std::string got = w.get_value();
    check((int)got.size() == (n < 5 ? n : 5) && got == std::string("HELLO").substr(0, n < 5 ? n : 5),
          "lowercase shown as uppercase");
    w.set_value("???");
    check(w.cell_count() == 0, "unshowable characters dropped");
    w.set_value("");
    check(w.cell_count() == 0, "empty value is empty");
    if (n >= 3) {
        w.set_value("42.5");
        check(w.cell_count() == 3 && w.char_at(1) == '2', "decimal point does not use a cell");
    }
    const char* top = (seg == 14) ? "a" : "a1";
    check(W::segment_on('8', top) && W::segment_on('8', "b") && W::segment_on('8', "g1"),
          "digit 8 lights top/right/middle segments");
    check(W::segment_on('1', "b") && W::segment_on('1', "c") && !W::segment_on('1', top),
          "digit 1 lights only b,c");
    check(W::segment_on('A', "a1") == (seg == 16), "16-seg splits the top bar, 14-seg does not");
    check(W::segment_on('M', "i") == (seg == 16), "16-seg M has a centre stem, 14-seg does not");
    // Pixel checks: all-8s lights amber, blank is dark, renders differ.
    XClearWindow(g_d, g_w);
    std::string eights(n, '8');
    w.set_value(eights);
    w.draw();
    XImage* im1 = snap();
    int amb1 = bright_amber(im1);
    XClearWindow(g_d, g_w);
    w.set_value(std::string(n, ' '));
    w.draw();
    XImage* im0 = snap();
    int amb0 = bright_amber(im0);
    int d = img_diff(im0, im1);
    std::printf("  amber8=%d amberBlank=%d diff=%d\n", amb1, amb0, d);
    check(amb1 > 50, "all-8s renders lit amber");
    check(amb0 == 0, "blank renders no lit amber");
    check(d > 100, "screenshot differs between 8s and blank");
    XDestroyImage(im0);
    XDestroyImage(im1);
    // Click focuses, Up steps the last character, Home restores the demo.
    bool clicked = false;
    w.on_click = [&clicked]() { clicked = true; };
    w.set_value(demo);
    int cx = 20 + w.width() / 2, cy = 20 + w.height() / 2;
    check(w.on_button_press(cx, cy, 1) && clicked && w.get_focused(), "click focuses + fires on_click");
    std::string before = w.get_value();
    check(w.on_key(XK_Up, "") && w.get_value() != before, "Up key steps the last character");
    check(w.on_key(XK_Home, "") && w.get_value() == demo, "Home restores the demo string");
    check(w.on_button_release(cx, cy, 1), "button release handled");
    XClearWindow(g_d, g_w);
}

template <typename W>
static void check_repertoire(const char* name, W& w) {
    std::printf("[repertoire %s]\n", name);
    std::string clean = W::clean_repertoire();
    std::string approx = W::approx_repertoire();
    std::string cannot = W::cannot_repertoire();
    std::vector<XImage*> imgs;
    std::vector<int> amber;
    for (char c : clean) {
        XClearWindow(g_d, g_w);
        w.set_value(std::string(1, c));
        w.draw();
        XImage* im = snap();
        int a = bright_amber(im);
        std::printf("  clean '%c' amber=%d\n", c, a);
        check(a > 20, "clean character renders lit segments");
        imgs.push_back(im);
        amber.push_back(a);
    }
    // Clean characters render distinctly, except the documented look-alikes.
    for (size_t i = 0; i < clean.size(); ++i) {
        for (size_t j = i + 1; j < clean.size(); ++j) {
            char a = clean[i], b = clean[j];
            if ((a == '0' && b == 'O') || (a == '5' && b == 'S') || (a == '6' && b == 'G') || (a == 'J' && b == 'U')) continue;
            int d = img_diff(imgs[i], imgs[j]);
            if (d <= 50) {
                std::printf("  FAIL clean pair '%c'/'%c' diff=%d\n", a, b, d);
                ++g_fail;
            }
        }
    }
    std::printf("  clean pairwise distinctness checked (%d chars)\n", (int)clean.size());
    for (XImage* im : imgs) XDestroyImage(im);
    for (char c : approx) {
        XClearWindow(g_d, g_w);
        // ':' has no cell of its own; probe it as a dot after '1'.
        w.set_value(c == ':' ? std::string("1:") : std::string(1, c));
        w.draw();
        XImage* im = snap();
        int a = bright_amber(im);
        std::printf("  approx '%c' amber=%d\n", c, a);
        check(a > 5, "approximate character renders something");
        XDestroyImage(im);
    }
    for (char c : cannot) {
        XClearWindow(g_d, g_w);
        w.set_value(std::string(1, c));
        w.draw();
        XImage* im = snap();
        int a = bright_amber(im);
        check(a == 0, "unshowable character renders blank");
        XDestroyImage(im);
    }
    // Backslash (char 92) is also unshowable.
    XClearWindow(g_d, g_w);
    w.set_value(std::string(1, (char)92));
    w.draw();
    XImage* imb = snap();
    check(bright_amber(imb) == 0, "backslash renders blank");
    XDestroyImage(imb);
    XClearWindow(g_d, g_w);
}

int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) { std::printf("FAIL no display\n"); return 1; }
    g_d = display;
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                        0, 0, 900, 700, 1, BlackPixel(display, screen),
                                        WhitePixel(display, screen));
    g_w = window;
    XMapWindow(display, window);
    XSync(display, False);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");

    InstSeg14Display1 w_InstSeg14Display1(display, screen, window, gc, &styles, 20, 20, 90, 110);
    check_seg("InstSeg14Display1", w_InstSeg14Display1, 1, 14, "A");
    InstSeg14Display2 w_InstSeg14Display2(display, screen, window, gc, &styles, 20, 20, 140, 110);
    check_seg("InstSeg14Display2", w_InstSeg14Display2, 2, 14, "42");
    InstSeg14Display3 w_InstSeg14Display3(display, screen, window, gc, &styles, 20, 20, 190, 110);
    check_seg("InstSeg14Display3", w_InstSeg14Display3, 3, 14, "ABC");
    InstSeg14Display4 w_InstSeg14Display4(display, screen, window, gc, &styles, 20, 20, 240, 110);
    check_seg("InstSeg14Display4", w_InstSeg14Display4, 4, 14, "HELL");
    InstSeg14Display5 w_InstSeg14Display5(display, screen, window, gc, &styles, 20, 20, 290, 110);
    check_seg("InstSeg14Display5", w_InstSeg14Display5, 5, 14, "HELLO");
    InstSeg14Display6 w_InstSeg14Display6(display, screen, window, gc, &styles, 20, 20, 340, 110);
    check_seg("InstSeg14Display6", w_InstSeg14Display6, 6, 14, "ABC123");
    InstSeg16Display1 w_InstSeg16Display1(display, screen, window, gc, &styles, 20, 20, 90, 110);
    check_seg("InstSeg16Display1", w_InstSeg16Display1, 1, 16, "M");
    InstSeg16Display2 w_InstSeg16Display2(display, screen, window, gc, &styles, 20, 20, 140, 110);
    check_seg("InstSeg16Display2", w_InstSeg16Display2, 2, 16, "MW");
    InstSeg16Display3 w_InstSeg16Display3(display, screen, window, gc, &styles, 20, 20, 190, 110);
    check_seg("InstSeg16Display3", w_InstSeg16Display3, 3, 16, "MWM");
    InstSeg16Display4 w_InstSeg16Display4(display, screen, window, gc, &styles, 20, 20, 240, 110);
    check_seg("InstSeg16Display4", w_InstSeg16Display4, 4, 16, "MWMW");
    InstSeg16Display5 w_InstSeg16Display5(display, screen, window, gc, &styles, 20, 20, 290, 110);
    check_seg("InstSeg16Display5", w_InstSeg16Display5, 5, 16, "MWMWM");
    InstSeg16Display6 w_InstSeg16Display6(display, screen, window, gc, &styles, 20, 20, 340, 110);
    check_seg("InstSeg16Display6", w_InstSeg16Display6, 6, 16, "MWMWMW");
    InstSeg14Display1 r14(display, screen, window, gc, &styles, 20, 20, 90, 110);
    check_repertoire("InstSeg14Display1", r14);
    InstSeg16Display1 r16(display, screen, window, gc, &styles, 20, 20, 90, 110);
    check_repertoire("InstSeg16Display1", r16);

    // 16-segment M/W render differently (better) than 14-segment M/W.
    {
        InstSeg14Display1 s14(display, screen, window, gc, &styles, 20, 20, 90, 110);
        InstSeg16Display1 s16(display, screen, window, gc, &styles, 20, 20, 90, 110);
        XClearWindow(g_d, g_w);
        s14.set_value("M"); s14.draw();
        XImage* a = snap();
        XClearWindow(g_d, g_w);
        s16.set_value("M"); s16.draw();
        XImage* b = snap();
        int dm = img_diff(a, b);
        std::printf("  M 14-vs-16 diff=%d\n", dm);
        check(dm > 50, "16-seg M differs from 14-seg M");
        XDestroyImage(a); XDestroyImage(b);
        XClearWindow(g_d, g_w);
        s14.set_value("W"); s14.draw();
        a = snap();
        XClearWindow(g_d, g_w);
        s16.set_value("W"); s16.draw();
        b = snap();
        int dw = img_diff(a, b);
        std::printf("  W 14-vs-16 diff=%d\n", dw);
        check(dw > 50, "16-seg W differs from 14-seg W");
        XDestroyImage(a); XDestroyImage(b);
    }

    XCloseDisplay(display);
    std::printf(g_fail == 0 ? "ALL PASS\n" : "FAILURES: %d\n", g_fail);
    return g_fail == 0 ? 0 : 1;
}
