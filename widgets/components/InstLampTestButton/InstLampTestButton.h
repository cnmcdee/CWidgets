#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
#include <utility>
#include <vector>

// // InstLampTestButton - lamp-test push button: while held it lights every lamp wired up with add_lamp(), then restores each lamp's previous state on release. set_on_test() gets a callback the panel wires up.
// Attribution: XFrame.
class InstLampTestButton {
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
    std::vector<std::pair<std::function<void(bool)>, std::function<bool()>>> lamps_;
    std::vector<bool> saved_;
    std::function<void(bool)> on_test;
public:
    std::function<void()> on_click;

    InstLampTestButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstLampTestButton"; text_ = "LAMP TEST";

    }
    ~InstLampTestButton() {}
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
    void add_lamp(std::function<void(bool)> setter, std::function<bool()> getter) {
        lamps_.push_back(std::make_pair(setter, getter));
    }
    int lamp_count() const { return (int)lamps_.size(); }
    void set_on_test(std::function<void(bool)> cb) { on_test = cb; }
    bool is_testing() const { return pressed_; }
    void press() {
        if (pressed_) return;
        pressed_ = true;
        saved_.clear();
        for (auto& p : lamps_) { saved_.push_back(p.second()); p.first(true); }
        if (on_test) on_test(true);
    }
    void release() {
        if (!pressed_) return;
        pressed_ = false;
        for (size_t i = 0; i < lamps_.size(); ++i) lamps_[i].first(saved_[i]);
        saved_.clear();
        if (on_test) on_test(false);
    }

    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long white = styles_->get_color_rgb(255, 255, 255);
        unsigned long bezel = styles_->get_color_rgb(70, 70, 70);
        unsigned long face = styles_->get_color_rgb(200, 200, 200);
        unsigned long face_down = styles_->get_color_rgb(120, 120, 120);
        XSetForeground(display_, gc_, white);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        int cx = (x1_ + x2_) / 2;
        int cy = (y1_ + y2_) / 2;
        int R = std::max(8, std::min(width(), height()) / 2 - 10);
        int ir = pressed_ ? std::max(4, R - 6) : std::max(4, R - 2);
        XSetForeground(display_, gc_, bezel);
        XFillArc(display_, window_, gc_, cx - R, cy - R, 2 * R, 2 * R, 0, 360 * 64);
        XSetForeground(display_, gc_, pressed_ ? face_down : face);
        XFillArc(display_, window_, gc_, cx - ir, cy - ir, 2 * ir, 2 * ir, 0, 360 * 64);
        int tw1 = xf::text_width(st, "LAMP");
        int tw2 = xf::text_width(st, "TEST");
        xf::draw_text_at(display_, window_, gc_, st, cx - tw1 / 2, cy - 2, "LAMP");
        xf::draw_text_at(display_, window_, gc_, st, cx - tw2 / 2, cy + 16, "TEST");
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
        if (button == 1) { press(); if (on_click) on_click(); return true; }
        return false;
    }
    bool on_button_release(int x, int y, unsigned int button) {
        (void)x; (void)y;
        if (button == 1 && pressed_) { release(); return true; }
        return false;
    }
    bool on_motion(int x, int y) {
        hovered_ = contains(x, y);
        return hovered_;
    }
    bool on_key(KeySym key, const char* str) {
        (void)str;
        if (!focused_ || !enabled_) return false;
        if (key == XK_space) { press(); release(); return true; }
        return false;
    }
};
