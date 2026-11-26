#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
#include <utility>
#include <vector>

// // InstBeacon - rotating/flashing beacon: on/off + flash rate (0.5-10 Hz). tick() advances one flash step; key f steps the flash, Space toggles on/off.
// Attribution: XFrame.
class InstBeacon {
private:
    Display* display_;
    int screen_;
    Window window_;
    GC gc_;
    SStyle_Manager* styles_;
    int x1_, y1_, x2_, y2_;
    std::string name_;
    std::string text_;
    std::string style_name_;
    bool visible_ = true;
    bool enabled_ = true;
    bool focused_ = false;
    bool hovered_ = false;
    bool pressed_ = false;
    xframe::InstLamp lamp_;
    double flash_rate_hz_ = 2.0;
    bool phase_ = true;
public:
    std::function<void()> on_click;

    InstBeacon(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstBeacon"; text_ = "BEACON";

    }
    ~InstBeacon() {}
    void set_bounds(int x, int y, int width, int height) { x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height; }
    int width() { return x2_ - x1_; }
    int height() { return y2_ - y1_; }
    bool contains(int x, int y) { return visible_ && x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_; }
    void set_visible(bool v) { visible_ = v; }
    bool get_visible() { return visible_; }
    void set_enabled(bool e) { enabled_ = e; }
    bool get_enabled() { return enabled_; }
    void set_style_name(std::string s) { style_name_ = s; }
    void set_text(std::string t) { text_ = t; }
    std::string get_text() { return text_; }
    SStyle style() { return styles_->resolve(style_name_); }
    bool is_pressed() const { return pressed_; }
    bool is_hovered() const { return hovered_; }
    void set_on(bool on) { lamp_.on = on; }
    bool is_on() const { return lamp_.on; }
    void toggle() { lamp_.on = !lamp_.on; }
    void set_brightness(int b) { lamp_.brightness = (int)xframe::inst_clamp_level((double)b); }
    int get_brightness() const { return lamp_.brightness; }
    xframe::InstLamp lamp_state() const { return lamp_; }
    void set_flash_rate(double hz) {
        if (hz < 0.5) hz = 0.5;
        if (hz > 10.0) hz = 10.0;
        flash_rate_hz_ = hz;
    }
    double get_flash_rate() const { return flash_rate_hz_; }
    void tick() { phase_ = !phase_; }
    void set_phase(bool p) { phase_ = p; }
    bool get_phase() const { return phase_; }
    bool is_lit() const { return lamp_.on && phase_; }

    unsigned long beacon_colour(int r, int g, int b) {
        double f = is_lit() ? (0.08 + 0.92 * lamp_.brightness / 100.0) : 0.10;
        return styles_->get_color_rgb((unsigned char)(r * f), (unsigned char)(g * f), (unsigned char)(b * f));
    }

    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long white = styles_->get_color_rgb(255, 255, 255);
        unsigned long dark = styles_->get_color_rgb(40, 40, 40);
        XSetForeground(display_, gc_, white);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        int cx = (x1_ + x2_) / 2;
        int base_y = y2_ - 44;
        int r = std::max(10, std::min(width(), height()) / 3);
        int cy = base_y - r + 10;
        XSetForeground(display_, gc_, dark);
        XFillRectangle(display_, window_, gc_, cx - 70, base_y, 140, 22);
        XSetForeground(display_, gc_, beacon_colour(230, 160, 20));
        XFillArc(display_, window_, gc_, cx - r, cy - r, 2 * r, 2 * r, 0, 360 * 64);
        if (is_lit()) {
            XSetForeground(display_, gc_, white);
            int hr = std::max(2, r / 4);
            XFillArc(display_, window_, gc_, cx - r / 2 - hr, cy - r / 2 - hr, 2 * hr, 2 * hr, 0, 360 * 64);
            XSetForeground(display_, gc_, beacon_colour(230, 160, 20));
            XDrawLine(display_, window_, gc_, cx + r / 2, cy - r / 2, cx + r + 40, cy - r - 30);
            XDrawLine(display_, window_, gc_, cx + r / 2, cy - r / 2 + 6, cx + r + 40, cy - r - 24);
            XDrawLine(display_, window_, gc_, cx + r / 2, cy - r / 2 - 6, cx + r + 40, cy - r - 36);
        }
        std::string rate = "FLASH " + std::to_string((int)flash_rate_hz_) + " Hz";
        int twr = xf::text_width(st, rate);
        xf::draw_text_at(display_, window_, gc_, st, cx - twr / 2, base_y + 38, rate);
        int tw = xf::text_width(st, text_);
        xf::draw_text_at(display_, window_, gc_, st, cx - tw / 2, y2_ - 4, text_);
        if (hovered_ || focused_) {
            XSetForeground(display_, gc_, styles_->get_color_rgb(0, 120, 200));
            XDrawRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 2, height() - 2);
        }
        XSetForeground(display_, gc_, st.lineColor_);
        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        XFlush(display_);
    }
    bool on_button_press(int x, int y, unsigned int button) {
        if (!contains(x, y) || !enabled_) return false;
        focused_ = true;
        if (button == 1) { pressed_ = true; toggle(); if (on_click) on_click(); return true; }
        return false;
    }
    bool on_button_release(int x, int y, unsigned int button) {
        (void)x; (void)y;
        if (button == 1 && pressed_) { pressed_ = false; return true; }
        return false;
    }
    bool on_motion(int x, int y) {
        hovered_ = contains(x, y);
        return hovered_;
    }
    bool on_key(KeySym key, const char* str) {
        (void)str;
        if (!focused_ || !enabled_) return false;
        if (key == XK_space) { toggle(); if (on_click) on_click(); return true; }
        if (key == XK_f || key == XK_F) { tick(); return true; }
        return false;
    }
};
