// test_inst_de.cpp - in-process tests for Instrumentation groups D + E.
// Verifies set_value clamps, get_value round-trips, click fires on_click,
// and the needle/marker pixels actually move when the value changes
// (centroid of the marker colour shifts along the expected axis).
// Attribution: XFrame.
#include "widgets/components/InstAnalogGauge/InstAnalogGauge.h"
#include "widgets/components/InstPressureGauge/InstPressureGauge.h"
#include "widgets/components/InstTemperatureGauge/InstTemperatureGauge.h"
#include "widgets/components/InstBarMeter/InstBarMeter.h"
#include "widgets/components/InstVuMeter/InstVuMeter.h"
#include "widgets/components/InstDigitalReadout/InstDigitalReadout.h"
#include "widgets/components/InstManometer/InstManometer.h"
#include "widgets/components/InstFlowMeter/InstFlowMeter.h"
#include "widgets/components/InstPanelPlate/InstPanelPlate.h"
#include "widgets/components/InstNameplate/InstNameplate.h"
#include "widgets/components/InstBezel/InstBezel.h"
#include "widgets/components/InstControlPanel/InstControlPanel.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

static Display* dpy;
static Window win;
static GC gc;
static SStyle_Manager* styles;
static int fails = 0;
static int passes = 0;

static void check(bool ok, const char* what) {
    if (ok) { ++passes; std::printf("  ok: %s\n", what); }
    else { ++fails; std::printf("  FAIL: %s\n", what); }
}

struct Centroid { long n = 0; double sx = 0, sy = 0; };

static Centroid centroid(unsigned long pix, int x, int y, int w, int h) {
    XImage* img = XGetImage(dpy, win, x, y, (unsigned)w, (unsigned)h, AllPlanes, ZPixmap);
    Centroid c;
    if (!img) return c;
    for (int yy = 0; yy < h; ++yy)
        for (int xx = 0; xx < w; ++xx)
            if (XGetPixel(img, xx, yy) == pix) { ++c.n; c.sx += xx; c.sy += yy; }
    XDestroyImage(img);
    return c;
}

static void settle() { XFlush(dpy); usleep(120000); }

template <typename W>
static void gauge_checks(const char* name, W& w, double mn, double mx) {
    std::printf("[%s]\n", name);
    w.set_value(mn - 1000.0);
    check(w.get_value() == mn, "clamps below min");
    w.set_value(mx + 1000.0);
    check(w.get_value() == mx, "clamps above max");
    double mid = (mn + mx) / 2.0;
    w.set_value(mid);
    check(w.get_value() == mid, "get returns set value");
    check(w.contains(100, 100), "contains centre");
    check(!w.contains(2000, 2000), "rejects outside point");
    bool clicked = false;
    w.on_click = [&]() { clicked = true; };
    check(w.on_button_press(100, 100, 1), "click accepted");
    check(clicked, "on_click fired");
    check(w.on_button_release(100, 100, 1), "release accepted");
    w.set_value(mid);
    w.on_key(XK_Right, "");
    check(w.get_value() > mid, "Right key raises value");
    w.set_visible(false);
    w.draw();
    w.set_visible(true);
    w.set_bounds(20, 20, 400, 300);
}

template <typename W>
static void marker_shift(const char* name, W& w, unsigned long pix,
                         double lo_v, double hi_v, char axis, double min_shift) {
    XClearWindow(dpy, win); settle();
    w.set_value(lo_v); w.draw(); settle();
    Centroid a = centroid(pix, 20, 20, 400, 300);
    XClearWindow(dpy, win); settle();
    w.set_value(hi_v); w.draw(); settle();
    Centroid b = centroid(pix, 20, 20, 400, 300);
    char buf[256];
    double da = (axis == 'x') ? (a.n ? a.sx / a.n : 0) : (a.n ? a.sy / a.n : 0);
    double db = (axis == 'x') ? (b.n ? b.sx / b.n : 0) : (b.n ? b.sy / b.n : 0);
    double shift = db - da;
    std::snprintf(buf, sizeof(buf), "marker moves along %c (%.1f -> %.1f, n %ld -> %ld)", axis, da, db, a.n, b.n);
    bool ok = a.n > 20 && b.n > 20 && ((axis == 'x' && shift > min_shift) || (axis == 'y' && shift < -min_shift));
    if (!ok) std::printf("  note: %s shift=%.1f\n", name, shift);
    check(ok, buf);
}

static unsigned long rgb(int r, int g, int b) { return styles->get_color_rgb((unsigned char)r, (unsigned char)g, (unsigned char)b); }

int main() {
    dpy = XOpenDisplay(nullptr);
    if (!dpy) { std::printf("FAIL: no display\n"); return 1; }
    int screen = DefaultScreen(dpy);
    win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 100, 100, 800, 600, 1,
                              BlackPixel(dpy, screen), WhitePixel(dpy, screen));
    XMapWindow(dpy, win);
    gc = DefaultGC(dpy, screen);
    SStyle_Manager st(dpy, win, screen, gc);
    st.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    st.set_active_style_byName("default");
    styles = &st;
    settle();
    unsigned long red = rgb(200, 30, 30), green = rgb(0, 150, 0);
    unsigned long blue = rgb(30, 120, 200), amber = rgb(255, 176, 0);

    { InstAnalogGauge w(dpy, screen, win, gc, styles, 20, 20, 400, 300);
      gauge_checks("InstAnalogGauge", w, 0, 100);
      marker_shift("InstAnalogGauge", w, red, 5, 95, 'x', 15); }
    { InstPressureGauge w(dpy, screen, win, gc, styles, 20, 20, 400, 300);
      gauge_checks("InstPressureGauge", w, 0, 300);
      marker_shift("InstPressureGauge", w, red, 15, 285, 'x', 15); }
    { InstTemperatureGauge w(dpy, screen, win, gc, styles, 20, 20, 400, 300);
      gauge_checks("InstTemperatureGauge", w, -20, 200);
      marker_shift("InstTemperatureGauge", w, red, -9, 189, 'x', 15); }
    { InstVuMeter w(dpy, screen, win, gc, styles, 20, 20, 400, 300);
      gauge_checks("InstVuMeter", w, -20, 3);
      marker_shift("InstVuMeter", w, red, -18.85, 1.85, 'x', 15); }
    { InstBarMeter w(dpy, screen, win, gc, styles, 20, 20, 400, 300);
      gauge_checks("InstBarMeter", w, 0, 100);
      marker_shift("InstBarMeter vertical", w, green, 10, 60, 'y', 15);
      w.set_horizontal(true);
      marker_shift("InstBarMeter horizontal", w, green, 10, 60, 'x', 15);
      w.set_horizontal(false);
      w.set_value(50); w.draw();
      check(w.on_button_press(30, 280, 1) && w.get_value() < 20, "click near bottom sets low value"); }
    { InstDigitalReadout w(dpy, screen, win, gc, styles, 20, 20, 400, 300);
      gauge_checks("InstDigitalReadout", w, 0, 999.9);
      XClearWindow(dpy, win); settle();
      w.set_value(0.0); w.draw(); settle();
      Centroid a = centroid(amber, 20, 20, 400, 300);
      XClearWindow(dpy, win); settle();
      w.set_value(999.9); w.draw(); settle();
      Centroid b = centroid(amber, 20, 20, 400, 300);
      char buf[128];
      std::snprintf(buf, sizeof(buf), "lit segments change with value (amber %ld -> %ld px)", a.n, b.n);
      check(a.n > 100 && b.n > 100 && llabs(b.n - a.n) > 50, buf); }
    { InstManometer w(dpy, screen, win, gc, styles, 20, 20, 400, 300);
      gauge_checks("InstManometer", w, -50, 50);
      marker_shift("InstManometer", w, blue, -40, 40, 'x', 10); }
    { InstFlowMeter w(dpy, screen, win, gc, styles, 20, 20, 400, 300);
      gauge_checks("InstFlowMeter", w, 0, 100);
      marker_shift("InstFlowMeter", w, red, 10, 90, 'y', 15); }
    { InstPanelPlate w(dpy, screen, win, gc, styles, 20, 20, 400, 300);
      std::printf("[InstPanelPlate]\n");
      w.draw(); settle();
      bool clicked = false; w.on_click = [&]() { clicked = true; };
      check(w.on_button_press(100, 100, 1) && clicked, "click fires on_click");
      check(w.contains(100, 100) && !w.contains(2000, 2000), "contains");
      w.set_text("POWER"); check(w.get_text() == "POWER", "title set/get"); w.draw(); }
    { InstNameplate w(dpy, screen, win, gc, styles, 20, 20, 400, 300);
      std::printf("[InstNameplate]\n");
      w.draw(); settle();
      bool clicked = false; w.on_click = [&]() { clicked = true; };
      check(w.on_button_press(100, 100, 1) && clicked, "click fires on_click");
      w.set_text("PUMP 2"); check(w.get_text() == "PUMP 2", "label set/get"); w.draw(); }
    { InstBezel w(dpy, screen, win, gc, styles, 20, 20, 400, 300);
      std::printf("[InstBezel]\n");
      w.draw(); settle();
      bool clicked = false; w.on_click = [&]() { clicked = true; };
      check(w.on_button_press(100, 100, 1) && clicked, "click fires on_click"); w.draw(); }
    { InstControlPanel w(dpy, screen, win, gc, styles, 20, 20, 400, 300);
      std::printf("[InstControlPanel]\n");
      w.set_grid(2, 2);
      w.place_cell(0, 0, "PUMP"); w.place_cell(0, 1, "VALVE");
      check(w.cell_label(0, 1) == "VALVE", "cell label stored");
      check(w.rows() == 2 && w.cols() == 2, "grid size");
      w.draw(); settle();
      bool clicked = false; w.on_click = [&]() { clicked = true; };
      // cell (0,1): grid starts x=36,y=72, cell w=184,h=116 -> centre ~ (312,130)
      check(w.on_button_press(312, 130, 1) && clicked, "click fires on_click");
      check(w.selected_label() == "VALVE", "click selects VALVE cell");
      w.draw(); }

    std::printf("RESULT: %d passed, %d failed\n", passes, fails);
    XCloseDisplay(dpy);
    return fails ? 1 : 0;
}
