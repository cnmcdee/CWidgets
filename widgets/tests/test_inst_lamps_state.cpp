// State harness for Group C (illuminators / status LEDs): proves on/off and
// brightness actually change the drawn pixels, the RGB LED's 8 channel
// combinations are all distinct, and the stack light / beacon / lamp-test
// button behave. Draws each widget in turn into one window and hashes the
// pixels with XGetImage. Attribution: XFrame.
#include "InstPanelLampRed/InstPanelLampRed.h"
#include "InstPanelLampGreen/InstPanelLampGreen.h"
#include "InstPanelLampAmber/InstPanelLampAmber.h"
#include "InstPanelLampBlue/InstPanelLampBlue.h"
#include "InstPanelLampWhite/InstPanelLampWhite.h"
#include "InstLedRed/InstLedRed.h"
#include "InstLedGreen/InstLedGreen.h"
#include "InstLedBlue/InstLedBlue.h"
#include "InstRgbStatusLed/InstRgbStatusLed.h"
#include "InstStackLight/InstStackLight.h"
#include "InstBeacon/InstBeacon.h"
#include "InstLampTestButton/InstLampTestButton.h"
#include "InstData.h"

#include <cstdio>
#include <string>
#include <unistd.h>
#include <vector>

static Display* d = nullptr;
static Window win = 0;
static GC gc_ = 0;
static SStyle_Manager* styles = nullptr;
static int failures = 0;
static int checks = 0;

static void check(bool ok, const std::string& msg) {
    ++checks;
    if (!ok) { ++failures; std::printf("FAIL: %s\n", msg.c_str()); }
}

struct Sig { unsigned long long hash; int nonwhite; };

static Sig shot(int x, int y, int w, int h) {
    XSync(d, False);
    XImage* im = XGetImage(d, win, x, y, (unsigned)w, (unsigned)h, AllPlanes, ZPixmap);
    Sig s{1469598103934665603ULL, 0};
    if (!im) return s;
    for (int yy = 0; yy < h; ++yy) {
        for (int xx = 0; xx < w; ++xx) {
            unsigned long p = XGetPixel(im, xx, yy) & 0xffffffUL;
            int r = (int)((p >> 16) & 255), g = (int)((p >> 8) & 255), b = (int)(p & 255);
            if (r < 250 || g < 250 || b < 250) ++s.nonwhite;
            unsigned long long v = (unsigned long long)p;
            s.hash ^= v + 0x9e3779b97f4a7c15ULL + (s.hash << 6) + (s.hash >> 2);
        }
    }
    XDestroyImage(im);
    return s;
}

static void test_instdata() {
    int f0 = failures;
    check(xframe::inst_clamp_level(-3.0) == 0.0, "InstData clamp low");
    check(xframe::inst_clamp_level(150.0) == 100.0, "InstData clamp high");
    check(xframe::inst_clamp_level(42.5) == 42.5, "InstData clamp mid");
    check(xframe::inst_demo_level(42) == xframe::inst_demo_level(42), "InstData demo deterministic");
    check(xframe::inst_demo_level(42) != xframe::inst_demo_level(7), "InstData demo seed differs");
    check(xframe::inst_demo_value(0.0, 10.0, 42) >= 0.0 && xframe::inst_demo_value(0.0, 10.0, 42) <= 10.0, "InstData demo value in range");
    xframe::InstLamp l;
    check(!l.on && l.brightness == 100, "InstData InstLamp defaults");
    xframe::InstRgb rgb;
    check(!rgb.red && !rgb.green && !rgb.blue, "InstData InstRgb defaults");
    std::printf("STATE InstData %s\n", failures == f0 ? "PASS" : "FAIL");
}

static void test_InstPanelLampRed() {
    int f0 = failures;
    InstPanelLampRed w(d, 0, win, gc_, styles, 10, 10, 180, 180);
    w.set_on(false); w.set_brightness(100); w.draw();
    Sig off = shot(10, 10, 180, 180);
    check(off.nonwhite > 20, "InstPanelLampRed draws non-blank off");
    w.set_on(true); w.draw();
    Sig on = shot(10, 10, 180, 180);
    check(on.hash != off.hash, "InstPanelLampRed on differs from off");
    w.set_brightness(30); w.draw();
    Sig dim = shot(10, 10, 180, 180);
    check(dim.hash != on.hash && dim.hash != off.hash, "InstPanelLampRed brightness 30 distinct");
    w.set_brightness(100); w.draw();
    bool before = w.is_on();
    w.on_button_press(100, 100, 1);
    w.on_button_release(100, 100, 1);
    check(w.is_on() != before, "InstPanelLampRed click toggles");
    w.on_key(XK_space, " ");
    check(w.is_on() == before, "InstPanelLampRed space toggles back");
    w.set_brightness(250);
    check(w.get_brightness() == 100, "InstPanelLampRed brightness clamps high");
    w.set_brightness(-5);
    check(w.get_brightness() == 0, "InstPanelLampRed brightness clamps low");
    xframe::InstLamp st = w.lamp_state();
    check(st.on == w.is_on() && st.brightness == w.get_brightness(), "InstPanelLampRed lamp_state matches");
    std::printf("STATE InstPanelLampRed %s\n", failures == f0 ? "PASS" : "FAIL");
}

static void test_InstPanelLampGreen() {
    int f0 = failures;
    InstPanelLampGreen w(d, 0, win, gc_, styles, 10, 10, 180, 180);
    w.set_on(false); w.set_brightness(100); w.draw();
    Sig off = shot(10, 10, 180, 180);
    check(off.nonwhite > 20, "InstPanelLampGreen draws non-blank off");
    w.set_on(true); w.draw();
    Sig on = shot(10, 10, 180, 180);
    check(on.hash != off.hash, "InstPanelLampGreen on differs from off");
    w.set_brightness(30); w.draw();
    Sig dim = shot(10, 10, 180, 180);
    check(dim.hash != on.hash && dim.hash != off.hash, "InstPanelLampGreen brightness 30 distinct");
    w.set_brightness(100); w.draw();
    bool before = w.is_on();
    w.on_button_press(100, 100, 1);
    w.on_button_release(100, 100, 1);
    check(w.is_on() != before, "InstPanelLampGreen click toggles");
    w.on_key(XK_space, " ");
    check(w.is_on() == before, "InstPanelLampGreen space toggles back");
    w.set_brightness(250);
    check(w.get_brightness() == 100, "InstPanelLampGreen brightness clamps high");
    w.set_brightness(-5);
    check(w.get_brightness() == 0, "InstPanelLampGreen brightness clamps low");
    xframe::InstLamp st = w.lamp_state();
    check(st.on == w.is_on() && st.brightness == w.get_brightness(), "InstPanelLampGreen lamp_state matches");
    std::printf("STATE InstPanelLampGreen %s\n", failures == f0 ? "PASS" : "FAIL");
}

static void test_InstPanelLampAmber() {
    int f0 = failures;
    InstPanelLampAmber w(d, 0, win, gc_, styles, 10, 10, 180, 180);
    w.set_on(false); w.set_brightness(100); w.draw();
    Sig off = shot(10, 10, 180, 180);
    check(off.nonwhite > 20, "InstPanelLampAmber draws non-blank off");
    w.set_on(true); w.draw();
    Sig on = shot(10, 10, 180, 180);
    check(on.hash != off.hash, "InstPanelLampAmber on differs from off");
    w.set_brightness(30); w.draw();
    Sig dim = shot(10, 10, 180, 180);
    check(dim.hash != on.hash && dim.hash != off.hash, "InstPanelLampAmber brightness 30 distinct");
    w.set_brightness(100); w.draw();
    bool before = w.is_on();
    w.on_button_press(100, 100, 1);
    w.on_button_release(100, 100, 1);
    check(w.is_on() != before, "InstPanelLampAmber click toggles");
    w.on_key(XK_space, " ");
    check(w.is_on() == before, "InstPanelLampAmber space toggles back");
    w.set_brightness(250);
    check(w.get_brightness() == 100, "InstPanelLampAmber brightness clamps high");
    w.set_brightness(-5);
    check(w.get_brightness() == 0, "InstPanelLampAmber brightness clamps low");
    xframe::InstLamp st = w.lamp_state();
    check(st.on == w.is_on() && st.brightness == w.get_brightness(), "InstPanelLampAmber lamp_state matches");
    std::printf("STATE InstPanelLampAmber %s\n", failures == f0 ? "PASS" : "FAIL");
}

static void test_InstPanelLampBlue() {
    int f0 = failures;
    InstPanelLampBlue w(d, 0, win, gc_, styles, 10, 10, 180, 180);
    w.set_on(false); w.set_brightness(100); w.draw();
    Sig off = shot(10, 10, 180, 180);
    check(off.nonwhite > 20, "InstPanelLampBlue draws non-blank off");
    w.set_on(true); w.draw();
    Sig on = shot(10, 10, 180, 180);
    check(on.hash != off.hash, "InstPanelLampBlue on differs from off");
    w.set_brightness(30); w.draw();
    Sig dim = shot(10, 10, 180, 180);
    check(dim.hash != on.hash && dim.hash != off.hash, "InstPanelLampBlue brightness 30 distinct");
    w.set_brightness(100); w.draw();
    bool before = w.is_on();
    w.on_button_press(100, 100, 1);
    w.on_button_release(100, 100, 1);
    check(w.is_on() != before, "InstPanelLampBlue click toggles");
    w.on_key(XK_space, " ");
    check(w.is_on() == before, "InstPanelLampBlue space toggles back");
    w.set_brightness(250);
    check(w.get_brightness() == 100, "InstPanelLampBlue brightness clamps high");
    w.set_brightness(-5);
    check(w.get_brightness() == 0, "InstPanelLampBlue brightness clamps low");
    xframe::InstLamp st = w.lamp_state();
    check(st.on == w.is_on() && st.brightness == w.get_brightness(), "InstPanelLampBlue lamp_state matches");
    std::printf("STATE InstPanelLampBlue %s\n", failures == f0 ? "PASS" : "FAIL");
}

static void test_InstPanelLampWhite() {
    int f0 = failures;
    InstPanelLampWhite w(d, 0, win, gc_, styles, 10, 10, 180, 180);
    w.set_on(false); w.set_brightness(100); w.draw();
    Sig off = shot(10, 10, 180, 180);
    check(off.nonwhite > 20, "InstPanelLampWhite draws non-blank off");
    w.set_on(true); w.draw();
    Sig on = shot(10, 10, 180, 180);
    check(on.hash != off.hash, "InstPanelLampWhite on differs from off");
    w.set_brightness(30); w.draw();
    Sig dim = shot(10, 10, 180, 180);
    check(dim.hash != on.hash && dim.hash != off.hash, "InstPanelLampWhite brightness 30 distinct");
    w.set_brightness(100); w.draw();
    bool before = w.is_on();
    w.on_button_press(100, 100, 1);
    w.on_button_release(100, 100, 1);
    check(w.is_on() != before, "InstPanelLampWhite click toggles");
    w.on_key(XK_space, " ");
    check(w.is_on() == before, "InstPanelLampWhite space toggles back");
    w.set_brightness(250);
    check(w.get_brightness() == 100, "InstPanelLampWhite brightness clamps high");
    w.set_brightness(-5);
    check(w.get_brightness() == 0, "InstPanelLampWhite brightness clamps low");
    xframe::InstLamp st = w.lamp_state();
    check(st.on == w.is_on() && st.brightness == w.get_brightness(), "InstPanelLampWhite lamp_state matches");
    std::printf("STATE InstPanelLampWhite %s\n", failures == f0 ? "PASS" : "FAIL");
}

static void test_InstLedRed() {
    int f0 = failures;
    InstLedRed w(d, 0, win, gc_, styles, 10, 10, 180, 180);
    w.set_on(false); w.set_brightness(100); w.draw();
    Sig off = shot(10, 10, 180, 180);
    check(off.nonwhite > 20, "InstLedRed draws non-blank off");
    w.set_on(true); w.draw();
    Sig on = shot(10, 10, 180, 180);
    check(on.hash != off.hash, "InstLedRed on differs from off");
    w.set_brightness(30); w.draw();
    Sig dim = shot(10, 10, 180, 180);
    check(dim.hash != on.hash && dim.hash != off.hash, "InstLedRed brightness 30 distinct");
    w.set_brightness(100); w.draw();
    bool before = w.is_on();
    w.on_button_press(100, 100, 1);
    w.on_button_release(100, 100, 1);
    check(w.is_on() != before, "InstLedRed click toggles");
    w.on_key(XK_space, " ");
    check(w.is_on() == before, "InstLedRed space toggles back");
    w.set_brightness(250);
    check(w.get_brightness() == 100, "InstLedRed brightness clamps high");
    w.set_brightness(-5);
    check(w.get_brightness() == 0, "InstLedRed brightness clamps low");
    xframe::InstLamp st = w.lamp_state();
    check(st.on == w.is_on() && st.brightness == w.get_brightness(), "InstLedRed lamp_state matches");
    std::printf("STATE InstLedRed %s\n", failures == f0 ? "PASS" : "FAIL");
}

static void test_InstLedGreen() {
    int f0 = failures;
    InstLedGreen w(d, 0, win, gc_, styles, 10, 10, 180, 180);
    w.set_on(false); w.set_brightness(100); w.draw();
    Sig off = shot(10, 10, 180, 180);
    check(off.nonwhite > 20, "InstLedGreen draws non-blank off");
    w.set_on(true); w.draw();
    Sig on = shot(10, 10, 180, 180);
    check(on.hash != off.hash, "InstLedGreen on differs from off");
    w.set_brightness(30); w.draw();
    Sig dim = shot(10, 10, 180, 180);
    check(dim.hash != on.hash && dim.hash != off.hash, "InstLedGreen brightness 30 distinct");
    w.set_brightness(100); w.draw();
    bool before = w.is_on();
    w.on_button_press(100, 100, 1);
    w.on_button_release(100, 100, 1);
    check(w.is_on() != before, "InstLedGreen click toggles");
    w.on_key(XK_space, " ");
    check(w.is_on() == before, "InstLedGreen space toggles back");
    w.set_brightness(250);
    check(w.get_brightness() == 100, "InstLedGreen brightness clamps high");
    w.set_brightness(-5);
    check(w.get_brightness() == 0, "InstLedGreen brightness clamps low");
    xframe::InstLamp st = w.lamp_state();
    check(st.on == w.is_on() && st.brightness == w.get_brightness(), "InstLedGreen lamp_state matches");
    std::printf("STATE InstLedGreen %s\n", failures == f0 ? "PASS" : "FAIL");
}

static void test_InstLedBlue() {
    int f0 = failures;
    InstLedBlue w(d, 0, win, gc_, styles, 10, 10, 180, 180);
    w.set_on(false); w.set_brightness(100); w.draw();
    Sig off = shot(10, 10, 180, 180);
    check(off.nonwhite > 20, "InstLedBlue draws non-blank off");
    w.set_on(true); w.draw();
    Sig on = shot(10, 10, 180, 180);
    check(on.hash != off.hash, "InstLedBlue on differs from off");
    w.set_brightness(30); w.draw();
    Sig dim = shot(10, 10, 180, 180);
    check(dim.hash != on.hash && dim.hash != off.hash, "InstLedBlue brightness 30 distinct");
    w.set_brightness(100); w.draw();
    bool before = w.is_on();
    w.on_button_press(100, 100, 1);
    w.on_button_release(100, 100, 1);
    check(w.is_on() != before, "InstLedBlue click toggles");
    w.on_key(XK_space, " ");
    check(w.is_on() == before, "InstLedBlue space toggles back");
    w.set_brightness(250);
    check(w.get_brightness() == 100, "InstLedBlue brightness clamps high");
    w.set_brightness(-5);
    check(w.get_brightness() == 0, "InstLedBlue brightness clamps low");
    xframe::InstLamp st = w.lamp_state();
    check(st.on == w.is_on() && st.brightness == w.get_brightness(), "InstLedBlue lamp_state matches");
    std::printf("STATE InstLedBlue %s\n", failures == f0 ? "PASS" : "FAIL");
}


static void test_rgb() {
    int f0 = failures;
    InstRgbStatusLed w(d, 0, win, gc_, styles, 10, 10, 180, 180);
    std::vector<unsigned long long> hashes;
    for (int combo = 0; combo < 8; ++combo) {
        w.set_red((combo & 1) != 0);
        w.set_green((combo & 2) != 0);
        w.set_blue((combo & 4) != 0);
        w.set_brightness(100);
        check(w.get_red() == ((combo & 1) != 0), "RGB get_red combo");
        check(w.get_green() == ((combo & 2) != 0), "RGB get_green combo");
        check(w.get_blue() == ((combo & 4) != 0), "RGB get_blue combo");
        w.draw();
        Sig s = shot(10, 10, 180, 180);
        check(s.nonwhite > 20, "RGB draws non-blank combo");
        hashes.push_back(s.hash);
    }
    for (int i = 0; i < 8; ++i)
        for (int j = i + 1; j < 8; ++j)
            check(hashes[i] != hashes[j], "RGB combos distinct");
    int r = 0, g = 0, b = 0;
    w.set_red(true); w.set_green(false); w.set_blue(false);
    w.colour(r, g, b);
    check(r > 200 && g == 0 && b == 0, "RGB colour red alone");
    w.set_red(false); w.set_green(true);
    w.colour(r, g, b);
    check(r == 0 && g > 200 && b == 0, "RGB colour green alone");
    w.set_green(false); w.set_blue(true);
    w.colour(r, g, b);
    check(r == 0 && g == 0 && b > 200, "RGB colour blue alone");
    // keys toggle one channel only (widget focused by the click below)
    w.set_all(false);
    w.on_button_press(100, 100, 1);
    w.on_button_release(100, 100, 1);
    check(w.is_on(), "RGB click turns all on");
    w.on_key(XK_r, "r");
    check(!w.get_red() && w.get_green() && w.get_blue(), "RGB key r toggles red only");
    w.on_key(XK_space, " ");
    check(!w.is_on(), "RGB space turns all off");
    std::printf("STATE InstRgbStatusLed %s\n", failures == f0 ? "PASS" : "FAIL");
}

static void test_stack() {
    int f0 = failures;
    InstStackLight w(d, 0, win, gc_, styles, 10, 10, 180, 180);
    w.set_all(false);
    w.draw();
    Sig off = shot(10, 10, 180, 180);
    check(off.nonwhite > 20, "StackLight draws non-blank off");
    w.set_red(true); w.draw();
    Sig red = shot(10, 10, 180, 180);
    check(red.hash != off.hash, "StackLight red differs from off");
    w.set_red(false); w.set_amber(true); w.draw();
    Sig amb = shot(10, 10, 180, 180);
    check(amb.hash != off.hash && amb.hash != red.hash, "StackLight amber distinct");
    w.set_amber(false); w.set_green(true); w.draw();
    Sig grn = shot(10, 10, 180, 180);
    check(grn.hash != off.hash && grn.hash != red.hash && grn.hash != amb.hash, "StackLight green distinct");
    w.set_all(true); w.draw();
    Sig all = shot(10, 10, 180, 180);
    check(all.hash != red.hash && all.hash != amb.hash && all.hash != grn.hash, "StackLight all distinct");
    // click top tier toggles red off; key 2 toggles amber
    w.on_button_press(100, 34, 1);
    w.on_button_release(100, 34, 1);
    check(!w.is_red_on() && w.is_amber_on() && w.is_green_on(), "StackLight click top tier toggles red");
    w.on_key(XK_2, "2");
    check(!w.is_amber_on(), "StackLight key 2 toggles amber");
    w.on_key(XK_3, "3");
    check(!w.is_green_on(), "StackLight key 3 toggles green");
    std::printf("STATE InstStackLight %s\n", failures == f0 ? "PASS" : "FAIL");
}

static void test_beacon() {
    int f0 = failures;
    InstBeacon w(d, 0, win, gc_, styles, 10, 10, 180, 180);
    w.set_on(false); w.draw();
    Sig off = shot(10, 10, 180, 180);
    check(off.nonwhite > 20, "Beacon draws non-blank off");
    w.set_on(true); w.set_phase(true); w.draw();
    Sig lit = shot(10, 10, 180, 180);
    check(lit.hash != off.hash, "Beacon lit differs from off");
    w.tick(); w.draw();
    Sig dark = shot(10, 10, 180, 180);
    check(dark.hash != lit.hash, "Beacon tick changes flash phase");
    check(!w.get_phase(), "Beacon tick flips phase");
    w.set_flash_rate(25.0);
    check(w.get_flash_rate() == 10.0, "Beacon flash rate clamps high");
    w.set_flash_rate(0.1);
    check(w.get_flash_rate() == 0.5, "Beacon flash rate clamps low");
    w.set_flash_rate(2.0);
    check(w.get_flash_rate() == 2.0, "Beacon flash rate set");
    bool before = w.is_on();
    w.on_button_press(100, 100, 1);
    w.on_button_release(100, 100, 1);
    check(w.is_on() != before, "Beacon click toggles");
    w.on_key(XK_f, "f");
    check(w.get_phase(), "Beacon key f ticks phase");
    std::printf("STATE InstBeacon %s\n", failures == f0 ? "PASS" : "FAIL");
}

static void test_lamptest() {
    int f0 = failures;
    InstPanelLampRed lamp_r(d, 0, win, gc_, styles, 10, 10, 180, 180);
    InstPanelLampGreen lamp_g(d, 0, win, gc_, styles, 10, 10, 180, 180);
    InstLampTestButton w(d, 0, win, gc_, styles, 10, 10, 180, 180);
    check(w.lamp_count() == 0, "LampTest starts with no lamps");
    lamp_r.set_on(true);
    lamp_g.set_on(false);
    int cb_count = 0;
    bool cb_last = false;
    w.add_lamp([&](bool v) { lamp_r.set_on(v); }, [&]() { return lamp_r.is_on(); });
    w.add_lamp([&](bool v) { lamp_g.set_on(v); }, [&]() { return lamp_g.is_on(); });
    w.set_on_test([&](bool t) { ++cb_count; cb_last = t; });
    check(w.lamp_count() == 2, "LampTest lamp count 2");
    w.draw();
    Sig idle = shot(10, 10, 180, 180);
    check(idle.nonwhite > 20, "LampTest draws non-blank");
    w.press();
    check(w.is_testing(), "LampTest press reports testing");
    check(lamp_r.is_on() && lamp_g.is_on(), "LampTest press lights every lamp");
    check(cb_count == 1 && cb_last, "LampTest on_test fired true");
    w.draw();
    Sig held = shot(10, 10, 180, 180);
    check(held.hash != idle.hash, "LampTest held differs from idle");
    w.release();
    check(!w.is_testing(), "LampTest release ends testing");
    check(lamp_r.is_on() && !lamp_g.is_on(), "LampTest release restores previous states");
    check(cb_count == 2 && !cb_last, "LampTest on_test fired false");
    w.on_button_press(100, 100, 1);
    check(lamp_r.is_on() && lamp_g.is_on(), "LampTest button press lights lamps");
    w.on_button_release(100, 100, 1);
    check(lamp_r.is_on() && !lamp_g.is_on(), "LampTest button release restores");
    std::printf("STATE InstLampTestButton %s\n", failures == f0 ? "PASS" : "FAIL");
}

int main() {
    d = XOpenDisplay(nullptr);
    if (!d) { std::printf("no display\n"); return 1; }
    int screen = DefaultScreen(d);
    win = XCreateSimpleWindow(d, RootWindow(d, screen), 100, 100, 800, 600, 1,
                              BlackPixel(d, screen), WhitePixel(d, screen));
    XMapWindow(d, win);
    XSync(d, False);
    usleep(200000);
    gc_ = DefaultGC(d, screen);
    SStyle_Manager sm(d, win, screen, gc_);
    sm.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    sm.set_active_style_byName("default");
    styles = &sm;
    test_instdata();
    test_InstPanelLampRed();
    test_InstPanelLampGreen();
    test_InstPanelLampAmber();
    test_InstPanelLampBlue();
    test_InstPanelLampWhite();
    test_InstLedRed();
    test_InstLedGreen();
    test_InstLedBlue();

    test_rgb();
    test_stack();
    test_beacon();
    test_lamptest();
    std::printf("STATE HARNESS: %d checks, %d failures\n", checks, failures);
    XCloseDisplay(d);
    return failures == 0 ? 0 : 1;
}
