#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>

// InstNeedleTapered - Standard tapered pointer on a round dial
// Attribution: XFrame.
class InstNeedleTapered {
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

    InstNeedleTapered(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstNeedleTapered"; text_ = "InstNeedleTapered";
        value_ = xframe::inst_demo_value(min_, max_, 42);
    }
    ~InstNeedleTapered() {}
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
    SStyle style() { SStyle s = styles_->resolve(style_name_); XFontStruct* bf = xframe::inst_bold_font(display_); if (bf) s.font_ = bf; return s; }
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
    // Needle angle in degrees from 12 o'clock, clockwise positive: -135 .. +135.
    double needle_angle_deg() const { return -135.0 + 270.0 * fraction(); }
    int centre_x() const { return x1_ + width() / 2; }
    int centre_y() const { return y1_ + height() / 2 + 6; }
    int dial_radius() const { return std::max(10, std::min(width(), height()) / 2 - 14); }

    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long panel = styles_->get_color_rgb(228, 228, 228);
        unsigned long face = styles_->get_color_rgb(250, 246, 230);
        unsigned long bezel = styles_->get_color_rgb(40, 40, 40);
        unsigned long tick_col = styles_->get_color_rgb(30, 30, 30);
        unsigned long red = styles_->get_color_rgb(200, 30, 30);
        unsigned long needle_col = styles_->get_color_rgb(30, 30, 30);
        XSetForeground(display_, gc_, panel);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        int cx = centre_x(), cy = centre_y(), R = dial_radius();
        XSetForeground(display_, gc_, bezel);
        XFillArc(display_, window_, gc_, cx - R - 4, cy - R - 4, 2 * R + 8, 2 * R + 8, 0, 360 * 64);
        XSetForeground(display_, gc_, face);
        XFillArc(display_, window_, gc_, cx - R, cy - R, 2 * R, 2 * R, 0, 360 * 64);
        // Red zone: top 10% of scale, drawn as short radial ticks.
        for (int i = 0; i <= 10; ++i) {
            double f = 0.9 + 0.01 * i;
            double a = (-135.0 + 270.0 * f) * kPi / 180.0;
            double sx = std::sin(a), sy = -std::cos(a);
            XSetForeground(display_, gc_, red);
            XDrawLine(display_, window_, gc_,
                      cx + (int)((R - 4) * sx), cy + (int)((R - 4) * sy),
                      cx + (int)(R * sx), cy + (int)(R * sy));
        }
        // Ticks + labels every 20%.
        for (int i = 0; i <= 10; ++i) {
            double f = i / 10.0;
            double a = (-135.0 + 270.0 * f) * kPi / 180.0;
            double sx = std::sin(a), sy = -std::cos(a);
            bool major = (i % 2 == 0);
            int inner = major ? R - 12 : R - 7;
            XSetForeground(display_, gc_, tick_col);
            XDrawLine(display_, window_, gc_,
                      cx + (int)(inner * sx), cy + (int)(inner * sy),
                      cx + (int)(R * sx), cy + (int)(R * sy));
            if (major) {
                std::string lab = std::to_string((int)std::llround(min_ + f * (max_ - min_)));
                int tw = xf::text_width(st, lab);
                int lx = cx + (int)((R - 26) * sx) - tw / 2;
                int ly = cy + (int)((R - 26) * sy) + 4;
                xf::draw_text_at(display_, window_, gc_, st, lx, ly, lab);
            }
        }
        // Tapered needle: wide at the hub, narrow at the tip.
        double a = needle_angle_deg() * kPi / 180.0;
        double dx = std::sin(a), dy = -std::cos(a);
        double px = std::cos(a), py = std::sin(a);
        int tip_x = cx + (int)((R - 14) * dx), tip_y = cy + (int)((R - 14) * dy);
        XPoint pts[3];
        pts[0].x = (short)tip_x; pts[0].y = (short)tip_y;
        pts[1].x = (short)(cx + 6 * px); pts[1].y = (short)(cy + 6 * py);
        pts[2].x = (short)(cx - 6 * px); pts[2].y = (short)(cy - 6 * py);
        XSetForeground(display_, gc_, needle_col);
        XFillPolygon(display_, window_, gc_, pts, 3, Convex, CoordModeOrigin);
        XSetForeground(display_, gc_, bezel);
        XFillArc(display_, window_, gc_, cx - 8, cy - 8, 16, 16, 0, 360 * 64);
        XSetForeground(display_, gc_, tick_col);
        int tw = xf::text_width(st, text_);
        xf::draw_text_at(display_, window_, gc_, st, cx - tw / 2, cy + R - 26, text_);
        if (focused_) {
            XSetForeground(display_, gc_, st.lineColor_);
            XDrawRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 3, height() - 3);
        }
        XFlush(display_);
    }
};
