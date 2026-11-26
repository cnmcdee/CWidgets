#include "InstSlideSwitch.h"
#include <cstdio>
#include <cstdlib>
static Display* g_d; static Window g_w; static int g_W = 400, g_H = 300;
static int fail(const char* m) { std::printf("FAIL: %s\n", m); return 1; }
static long pixels(InstSlideSwitch& w) {
    (void)w; XSync(g_d, False);
    XImage* im = XGetImage(g_d, g_w, 0, 0, (unsigned)g_W, (unsigned)g_H, AllPlanes, ZPixmap);
    if (!im) return -1;
    long s = 0;
    for (int y = 0; y < g_H; y += 2) for (int x = 0; x < g_W; x += 2) s += (long)XGetPixel(im, x, y);
    XDestroyImage(im);
    return s;
}
int main() {
    g_d = XOpenDisplay(nullptr);
    if (!g_d) return fail("no display");
    int screen = DefaultScreen(g_d);
    g_w = XCreateSimpleWindow(g_d, RootWindow(g_d, screen), 50, 50, (unsigned)g_W, (unsigned)g_H, 1,
                              BlackPixel(g_d, screen), WhitePixel(g_d, screen));
    XMapWindow(g_d, g_w); XSync(g_d, False);
    GC gc = DefaultGC(g_d, screen);
    SStyle_Manager styles(g_d, g_w, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    InstSlideSwitch w(g_d, screen, g_w, gc, &styles, 20, 20, 360, 240);
    int clicks = 0;
    w.on_click = [&]() { ++clicks; };
    w.draw();
    if (!w.contains(200, 150)) return fail("contains() false inside bounds");
    if (w.contains(5, 5)) return fail("contains() true outside bounds");
    if (w.is_on()) return fail("starts on");
    if (w.get_state() != xframe::InstSwitchState::Off) return fail("starts not Off");
    long b = pixels(w);
    w.on_button_press(200, 150, 1); w.on_button_release(200, 150, 1);
    if (!w.is_on()) return fail("click did not turn on");
    if (w.get_state() != xframe::InstSwitchState::On) return fail("state not On");
    w.draw(); long a = pixels(w);
    if (a == b) return fail("pixels unchanged after toggle on");
    w.on_button_press(200, 150, 1); w.on_button_release(200, 150, 1);
    if (w.is_on()) return fail("second click did not turn off");
    w.draw(); long c = pixels(w);
    if (c == a) return fail("pixels unchanged after toggle off");
    if (clicks != 2) return fail("on_click fired wrong count");
    std::printf("toggle on/off, pixels change both ways, on_click x2\n");
    XCloseDisplay(g_d);
    std::printf("PASS InstSlideSwitch\n");
    return 0;
}
