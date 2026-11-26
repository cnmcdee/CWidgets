// Headless logic + pixel checks for Instrumentation Group F.
// Builds with -Wall -Wextra, runs under Xvfb. Attribution: XFrame.
#include "InstLedDigits1/InstLedDigits1.h"
#include "InstLedDigits2/InstLedDigits2.h"
#include "InstLedDigits3/InstLedDigits3.h"
#include "InstLedDigits4/InstLedDigits4.h"
#include "InstLedDigits5/InstLedDigits5.h"
#include "InstLedDigits6/InstLedDigits6.h"
#include "InstNeedleTapered/InstNeedleTapered.h"
#include "InstNeedleKnifeEdge/InstNeedleKnifeEdge.h"
#include "InstNeedleEdgewise/InstNeedleEdgewise.h"
#include "InstNeedleVu/InstNeedleVu.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>

static Display* g_d = nullptr;
static Window g_w = 0;
static int g_fail = 0;

static void check(bool ok, const char* what) {
    std::printf("  %s %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) ++g_fail;
}

static int bright_red(XImage* im) {
    int n = 0;
    for (int y = 0; y < im->height; ++y)
        for (int x = 0; x < im->width; ++x) {
            unsigned long p = XGetPixel(im, x, y);
            int r = (p >> 16) & 0xff, g = (p >> 8) & 0xff, b = p & 0xff;
            if (r > 200 && g < 100 && b < 100) ++n;
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
static void check_digits(const char* name, W& w, int n) {
    std::printf("[%s]\n", name);
    check(W::kDigits == n, "digit count matches");
    w.set_value(-5.0);
    check(w.get_value() == 0.0, "clamps below 0 to 0");
    w.set_value(1e9);
    check(w.get_value() == w.max_value(), "clamps above max to max_value");
    double mid = w.max_value() / 2.0;
    w.set_value(mid);
    check(std::fabs(w.get_value() - mid) < 1e-9, "mid value round-trips");
    check((int)w.shown_digits().size() == n, "shown digits zero-padded to N");
    w.set_value(0.0);
    check(w.digit_at(0) == 0, "value 0 shows 0");
    check(W::segment_on(8, 'a') && W::segment_on(8, 'b') && W::segment_on(8, 'c') &&
          W::segment_on(8, 'd') && W::segment_on(8, 'e') && W::segment_on(8, 'f') &&
          W::segment_on(8, 'g'), "digit 8 lights all 7 segments");
    check(W::segment_on(1, 'b') && W::segment_on(1, 'c') && !W::segment_on(1, 'a') &&
          !W::segment_on(1, 'g'), "digit 1 lights only b,c");
    check(w.decimal_point_after() == (n >= 2 ? n - 2 : -1), "decimal point position");
    // Pixel checks: max value lights more red than 0, and the two renders differ.
    XClearWindow(g_d, g_w);
    w.set_value(0.0);
    w.draw();
    XImage* im0 = snap();
    int red0 = bright_red(im0);
    XClearWindow(g_d, g_w);
    double eights = 0.0;
    {
        long long e = 0;
        for (int i = 0; i < n; ++i) e = e * 10 + 8;
        double sc = 1.0;
        for (int i = 0; i < w.get_decimal_places(); ++i) sc *= 10.0;
        eights = (double)e / sc;
    }
    w.set_value(eights);
    w.draw();
    XImage* im1 = snap();
    int red1 = bright_red(im1);
    int d = img_diff(im0, im1);
    std::printf("  red0=%d red8=%d diff=%d\n", red0, red1, d);
    check(red0 > 50, "value 0 renders lit segments");
    check(red1 > red0, "all-8s value renders more lit red than 0");
    check(d > 100, "screenshot differs between 0 and max");
    XDestroyImage(im0);
    XDestroyImage(im1);
    // Click focuses, Up key steps the value.
    bool clicked = false;
    w.on_click = [&clicked]() { clicked = true; };
    int cx = 20 + w.width() / 2, cy = 20 + w.height() / 2;
    check(w.on_button_press(cx, cy, 1) && clicked && w.get_focused(), "click focuses + fires on_click");
    double before = w.get_value();
    w.set_value(w.max_value() / 2.0);
    before = w.get_value();
    check(w.on_key(XK_Up, "") && w.get_value() > before, "Up key increases value");
    check(w.on_button_release(cx, cy, 1), "button release handled");
    XClearWindow(g_d, g_w);
}

template <typename W>
static void check_needle(const char* name, W& w, double lo, double hi) {
    std::printf("[%s]\n", name);
    w.set_range(lo, hi);
    check(w.get_min() == lo && w.get_max() == hi, "range set");
    w.set_value(lo - 50.0);
    check(w.get_value() == lo, "clamps below min");
    w.set_value(hi + 50.0);
    check(w.get_value() == hi, "clamps above max");
    double mid = lo + (hi - lo) / 2.0;
    w.set_value(mid);
    check(std::fabs(w.get_value() - mid) < 1e-9, "mid value round-trips");
    w.set_value(lo);
    double a_lo = w.needle_angle_deg();
    w.set_value(hi);
    double a_hi = w.needle_angle_deg();
    check(a_lo < a_hi, "needle angle rises with value");
    w.set_value(mid);
    double a_mid = w.needle_angle_deg();
    check(a_mid > a_lo && a_mid < a_hi, "mid angle between extremes");
    XClearWindow(g_d, g_w);
    w.set_value(lo);
    w.draw();
    XImage* im0 = snap();
    XClearWindow(g_d, g_w);
    w.set_value(hi);
    w.draw();
    XImage* im1 = snap();
    int d = img_diff(im0, im1);
    check(d > 100, "needle pixels move between min and max");
    XDestroyImage(im0);
    XDestroyImage(im1);
    bool clicked = false;
    w.on_click = [&clicked]() { clicked = true; };
    int cx = 20 + w.width() / 2, cy = 170 + w.height() / 2;
    check(w.on_button_press(cx, cy, 1) && clicked && w.get_focused(), "click focuses + fires on_click");
    w.set_value(mid);
    double before = w.get_value();
    check(w.on_key(XK_Up, "") && w.get_value() > before, "Up key increases value");
    check(w.on_button_release(cx, cy, 1), "button release handled");
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

    InstLedDigits1 d1(display, screen, window, gc, &styles, 20, 20, 90, 110);
    check_digits("InstLedDigits1", d1, 1);
    InstLedDigits2 d2(display, screen, window, gc, &styles, 20, 20, 140, 110);
    check_digits("InstLedDigits2", d2, 2);
    InstLedDigits3 d3(display, screen, window, gc, &styles, 20, 20, 190, 110);
    check_digits("InstLedDigits3", d3, 3);
    InstLedDigits4 d4(display, screen, window, gc, &styles, 20, 20, 240, 110);
    check_digits("InstLedDigits4", d4, 4);
    InstLedDigits5 d5(display, screen, window, gc, &styles, 20, 20, 290, 110);
    check_digits("InstLedDigits5", d5, 5);
    InstLedDigits6 d6(display, screen, window, gc, &styles, 20, 20, 340, 110);
    check_digits("InstLedDigits6", d6, 6);
    InstNeedleTapered t(display, screen, window, gc, &styles, 20, 170, 420, 380);
    check_needle("InstNeedleTapered", t, 0.0, 100.0);
    InstNeedleKnifeEdge k(display, screen, window, gc, &styles, 20, 170, 420, 380);
    check_needle("InstNeedleKnifeEdge", k, 0.0, 100.0);
    InstNeedleEdgewise e(display, screen, window, gc, &styles, 20, 170, 560, 280);
    check_needle("InstNeedleEdgewise", e, 0.0, 100.0);
    InstNeedleVu v(display, screen, window, gc, &styles, 20, 170, 480, 380);
    check_needle("InstNeedleVu", v, -20.0, 3.0);

    XCloseDisplay(display);
    std::printf(g_fail == 0 ? "ALL PASS\n" : "FAILURES: %d\n", g_fail);
    return g_fail == 0 ? 0 : 1;
}
