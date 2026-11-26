#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>

// InstNeedleEdgewise - Edgewise horizontal meter
// Attribution: XFrame.
class InstNeedleEdgewise {
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
    bool pressed_ = false;
    bool hover_ = false;
    double value_ = 0.0;
    double min_ = 0.0;
    double max_ = 100.0;
    static constexpr double kPi = 3.141592653589793;

public:
    std::function<void()> on_click;

    InstNeedleEdgewise(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstNeedleEdgewise"; text_ = "InstNeedleEdgewise";
        value_ = xframe::inst_demo_value(min_, max_, 42);
    }
    ~InstNeedleEdgewise() {}
    void set_bounds(int x, int y, int width, int height) { x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height; }
    int width() const { return x2_ - x1_; }
    int height() const { return y2_ - y1_; }
    bool contains(int x, int y) { return visible_ && x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_; }
    void set_visible(bool v) { visible_ = v; }
    bool get_visible() { return visible_; }
    void set_enabled(bool e) { enabled_ = e; }
    bool get_enabled() { return enabled_; }
    bool get_focused() { return focused_; }
    void set_style_name(std::string s) { style_name_ = s; }
    void set_text(std::string t) { text_ = t; }
    std::string get_text() { return text_; }
    SStyle style() { return styles_->resolve(style_name_); }
    void set_range(double lo, double hi) {
        if (hi <= lo) return;
        min_ = lo; max_ = hi;
        set_value(value_);
    }
    double get_min() const { return min_; }
    double get_max() const { return max_; }
    void set_value(double v) { value_ = std::max(min_, std::min(max_, v)); }
    double get_value() const { return value_; }
    bool on_button_press(int x, int y, unsigned int button) {
        (void)button;
        if (!contains(x, y) || !enabled_) return false;
        focused_ = true; pressed_ = true;
        if (on_click) on_click();
        return true;
    }
    bool on_button_release(int x, int y, unsigned int button) {
        (void)x; (void)y; (void)button;
        if (!pressed_) return false;
        pressed_ = false;
        return true;
    }
    bool on_motion(int x, int y) {
        hover_ = contains(x, y);
        return hover_;
    }
    bool on_key(KeySym key, const char* str) {
        (void)str;
        if (!focused_ || !enabled_) return false;
        double step = (max_ - min_) / 20.0;
        switch (key) {
        case XK_Up: case XK_Right: set_value(get_value() + step); return true;
        case XK_Down: case XK_Left: set_value(get_value() - step); return true;
        case XK_Home: set_value(min_); return true;
        case XK_Return: case XK_space:
            if (on_click) on_click();
            return true;
        }
        return false;
    }

    double fraction() const { return (max_ > min_) ? (value_ - min_) / (max_ - min_) : 0.0; }
    // Needle angle in degrees from vertical, negative = left: -50 .. +50.
    double needle_angle_deg() const { return -50.0 + 100.0 * fraction(); }
    int pivot_x() const { return x1_ + width() / 2; }
    int pivot_y() const { return y2_ - 16; }
    int scale_y() const { return y1_ + (int)(height() * 0.45); }
    int scale_half() const { return std::max(10, width() / 2 - 24); }
    int needle_len() const { return pivot_y() - scale_y() + 6; }
    int needle_tip_x() const {
        double a = needle_angle_deg() * kPi / 180.0;
        return pivot_x() + (int)(needle_len() * std::sin(a));
    }
    int needle_tip_y() const {
        double a = needle_angle_deg() * kPi / 180.0;
        return pivot_y() - (int)(needle_len() * std::cos(a));
    }

    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long panel = styles_->get_color_rgb(232, 232, 232);
        unsigned long face = styles_->get_color_rgb(250, 250, 248);
        unsigned long bezel = styles_->get_color_rgb(50, 50, 50);
        unsigned long tick_col = styles_->get_color_rgb(20, 20, 20);
        unsigned long red = styles_->get_color_rgb(200, 30, 30);
        unsigned long needle_col = styles_->get_color_rgb(15, 15, 15);
        XSetForeground(display_, gc_, panel);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        int px = pivot_x(), py = pivot_y(), sy = scale_y(), half = scale_half();
        XSetForeground(display_, gc_, face);
        XFillRectangle(display_, window_, gc_, x1_ + 8, y1_ + 8, width() - 16, sy - y1_ + 22);
        XSetForeground(display_, gc_, bezel);
        XDrawRectangle(display_, window_, gc_, x1_ + 8, y1_ + 8, width() - 17, sy - y1_ + 22);
        // Flat horizontal scale with ticks; rightmost 10% in red.
        XSetForeground(display_, gc_, tick_col);
        XDrawLine(display_, window_, gc_, px - half, sy, px + half, sy);
        XSetForeground(display_, gc_, red);
        XDrawLine(display_, window_, gc_, px + (int)(half * 0.8), sy, px + half, sy);
        for (int i = 0; i <= 10; ++i) {
            double f = i / 10.0;
            int tx = px - half + (int)(2 * half * f);
            bool major = (i % 2 == 0);
            int th = major ? 12 : 6;
            XSetForeground(display_, gc_, (f >= 0.9) ? red : tick_col);
            XDrawLine(display_, window_, gc_, tx, sy, tx, sy - th);
            if (major) {
                std::string lab = std::to_string((int)std::llround(min_ + f * (max_ - min_)));
                int tw = xf::text_width(st, lab);
                xf::draw_text_at(display_, window_, gc_, st, tx - tw / 2, sy - th - 12, lab);
            }
        }
        // Needle sweeps left-right from the pivot below the scale.
        XSetForeground(display_, gc_, needle_col);
        XDrawLine(display_, window_, gc_, px, py, needle_tip_x(), needle_tip_y());
        XDrawLine(display_, window_, gc_, px + 1, py, needle_tip_x() + 1, needle_tip_y());
        XSetForeground(display_, gc_, bezel);
        XFillArc(display_, window_, gc_, px - 7, py - 7, 14, 14, 0, 360 * 64);
        XSetForeground(display_, gc_, tick_col);
        int tw = xf::text_width(st, text_);
        xf::draw_text_at(display_, window_, gc_, st, px - tw / 2, py + 24, text_);
        if (focused_) {
            XSetForeground(display_, gc_, st.lineColor_);
            XDrawRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 3, height() - 3);
        }
        XFlush(display_);
    }
};
