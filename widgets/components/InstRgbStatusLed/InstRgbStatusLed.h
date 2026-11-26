#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
#include <utility>
#include <vector>

// // InstRgbStatusLed - ONE status LED with INDEPENDENT red, green and blue channels: set_red()/set_green()/set_blue() each on their own, any of the 8 colours + off. Click or Space toggles all channels; keys r/g/b toggle one channel.
// Attribution: XFrame.
class InstRgbStatusLed {
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
    xframe::InstRgb rgb_;
    int brightness_ = 100;
public:
    std::function<void()> on_click;

    InstRgbStatusLed(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstRgbStatusLed"; text_ = "RGB";

    }
    ~InstRgbStatusLed() {}
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
    void set_red(bool v) { rgb_.red = v; }
    void set_green(bool v) { rgb_.green = v; }
    void set_blue(bool v) { rgb_.blue = v; }
    bool get_red() const { return rgb_.red; }
    bool get_green() const { return rgb_.green; }
    bool get_blue() const { return rgb_.blue; }
    xframe::InstRgb rgb_state() const { return rgb_; }
    void set_rgb(const xframe::InstRgb& s) { rgb_ = s; }
    void set_brightness(int b) { brightness_ = (int)xframe::inst_clamp_level((double)b); }
    int get_brightness() const { return brightness_; }
    bool is_on() const { return rgb_.red || rgb_.green || rgb_.blue; }
    void set_all(bool v) { rgb_.red = v; rgb_.green = v; rgb_.blue = v; }
    void toggle_all() { if (is_on()) set_all(false); else set_all(true); }
    void colour(int& r, int& g, int& b) const {
        double f = is_on() ? (0.08 + 0.92 * brightness_ / 100.0) : 0.0;
        r = rgb_.red ? (int)(255 * f) : 0;
        g = rgb_.green ? (int)(255 * f) : 0;
        b = rgb_.blue ? (int)(255 * f) : 0;
    }

    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long white = styles_->get_color_rgb(255, 255, 255);
        unsigned long dark = styles_->get_color_rgb(40, 40, 40);
        XSetForeground(display_, gc_, white);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        int cx = (x1_ + x2_) / 2;
        int cy = (y1_ + y2_) / 2 - 20;
        int r = std::max(8, std::min(width(), height()) / 4);
        int cr = 0, cg = 0, cb = 0;
        colour(cr, cg, cb);
        XSetForeground(display_, gc_, dark);
        XFillRectangle(display_, window_, gc_, cx - r - 8, cy - r - 8, 2 * r + 16, 2 * r + 16);
        XSetForeground(display_, gc_, styles_->get_color_rgb((unsigned char)cr, (unsigned char)cg, (unsigned char)cb));
        XFillArc(display_, window_, gc_, cx - r, cy - r, 2 * r, 2 * r, 0, 360 * 64);
        if (is_on() && brightness_ >= 50) {
            XSetForeground(display_, gc_, white);
            int hr = std::max(2, r / 4);
            XFillArc(display_, window_, gc_, cx - r / 2 - hr, cy - r / 2 - hr, 2 * hr, 2 * hr, 0, 360 * 64);
        }
        const char* labs[3] = {"R", "G", "B"};
        bool ch[3] = {rgb_.red, rgb_.green, rgb_.blue};
        unsigned long cols[3] = {styles_->get_color_rgb(220, 30, 30),
                                 styles_->get_color_rgb(30, 200, 60),
                                 styles_->get_color_rgb(40, 120, 230)};
        int sq = 20;
        int sy = y2_ - 40;
        for (int i = 0; i < 3; ++i) {
            int sx = cx - 33 + i * 24;
            XSetForeground(display_, gc_, ch[i] ? cols[i] : dark);
            XFillRectangle(display_, window_, gc_, sx, sy, sq, sq);
            XSetForeground(display_, gc_, styles_->get_color_rgb(60, 60, 60));
            XDrawRectangle(display_, window_, gc_, sx, sy, sq, sq);
            int twl = xf::text_width(st, labs[i]);
            xf::draw_text_at(display_, window_, gc_, st, sx + (sq - twl) / 2, sy + 15, labs[i]);
        }
        int tw = xf::text_width(st, text_);
        xf::draw_text_at(display_, window_, gc_, st, cx - tw / 2, y2_ - 6, text_);
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
        if (button == 1) { pressed_ = true; toggle_all(); if (on_click) on_click(); return true; }
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
        if (key == XK_space) { toggle_all(); if (on_click) on_click(); return true; }
        if (key == XK_r || key == XK_R) { rgb_.red = !rgb_.red; return true; }
        if (key == XK_g || key == XK_G) { rgb_.green = !rgb_.green; return true; }
        if (key == XK_b || key == XK_B) { rgb_.blue = !rgb_.blue; return true; }
        return false;
    }
};
