#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>

// InstNeedleVu - VU-style needle on a VU meter scale
// Attribution: XFrame.
class InstNeedleVu {
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
    double min_ = -20.0;
    double max_ = 3.0;
    static constexpr double kPi = 3.141592653589793;

public:
    std::function<void()> on_click;

    InstNeedleVu(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstNeedleVu"; text_ = "InstNeedleVu";
        value_ = xframe::inst_demo_value(min_, max_, 42);
    }
    ~InstNeedleVu() {}
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

    // Standard VU scale: nonlinear dB spacing, red above 0 VU.
    static double vu_fraction(double v) {
        static const double kDb[10] = {-20, -10, -7, -5, -3, -1, 0, 1, 2, 3};
        static const double kFr[10] = {0.00, 0.28, 0.38, 0.47, 0.56, 0.68, 0.78, 0.86, 0.93, 1.00};
        if (v <= kDb[0]) return kFr[0];
        if (v >= kDb[9]) return kFr[9];
        for (int i = 0; i < 9; ++i) {
            if (v >= kDb[i] && v <= kDb[i + 1]) {
                double f = (v - kDb[i]) / (kDb[i + 1] - kDb[i]);
                return kFr[i] + f * (kFr[i + 1] - kFr[i]);
            }
        }
        return 0.0;
    }
    double fraction() const { return vu_fraction(value_); }
    // Needle angle in degrees from vertical, negative = left: -45 .. +45.
    double needle_angle_deg() const { return -45.0 + 90.0 * fraction(); }
    int pivot_x() const { return x1_ + width() / 2; }
    int pivot_y() const { return y2_ - 14; }
    int arc_radius() const { return std::max(10, (int)(std::min(width(), height()) * 0.72)); }

    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long panel = styles_->get_color_rgb(226, 226, 226);
        unsigned long face = styles_->get_color_rgb(248, 238, 205);
        unsigned long bezel = styles_->get_color_rgb(50, 50, 50);
        unsigned long tick_col = styles_->get_color_rgb(20, 20, 20);
        unsigned long red = styles_->get_color_rgb(200, 30, 30);
        unsigned long needle_col = styles_->get_color_rgb(15, 15, 15);
        XSetForeground(display_, gc_, panel);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        int px = pivot_x(), py = pivot_y(), R = arc_radius();
        XSetForeground(display_, gc_, face);
        XFillRectangle(display_, window_, gc_, x1_ + 6, y1_ + 6, width() - 12, height() - 12);
        XSetForeground(display_, gc_, bezel);
        XDrawRectangle(display_, window_, gc_, x1_ + 6, y1_ + 6, width() - 13, height() - 13);
        static const double kDb[10] = {-20, -10, -7, -5, -3, -1, 0, 1, 2, 3};
        // Scale arc (black to 0 VU, red above) + ticks + labels.
        for (int i = 0; i < 10; ++i) {
            double a0 = (-45.0 + 90.0 * vu_fraction(kDb[i])) * kPi / 180.0;
            double a1 = (-45.0 + 90.0 * vu_fraction(kDb[i + 1 < 10 ? i + 1 : i])) * kPi / 180.0;
            // Arc segment between adjacent scale points, in 8 steps.
            double f0 = vu_fraction(kDb[i]);
            double f1 = vu_fraction(kDb[i + 1 < 10 ? i + 1 : i]);
            if (i == 9) { f1 = f0; }
            unsigned long col = (kDb[i] >= 0) ? red : tick_col;
            double prev_x = 0, prev_y = 0;
            bool first = true;
            for (int s = 0; s <= 8; ++s) {
                double f = f0 + (f1 - f0) * s / 8.0;
                double a = (-45.0 + 90.0 * f) * kPi / 180.0;
                double x = px + R * std::sin(a);
                double y = py - R * std::cos(a);
                if (!first) {
                    XSetForeground(display_, gc_, col);
                    XDrawLine(display_, window_, gc_, (int)prev_x, (int)prev_y, (int)x, (int)y);
                }
                prev_x = x; prev_y = y; first = false;
            }
            (void)a0; (void)a1;
            double a = (-45.0 + 90.0 * vu_fraction(kDb[i])) * kPi / 180.0;
            double sx = std::sin(a), sy = -std::cos(a);
            int tx0 = px + (int)((R - 10) * sx), ty0 = py + (int)((R - 10) * sy);
            int tx1 = px + (int)(R * sx), ty1 = py + (int)(R * sy);
            XSetForeground(display_, gc_, (kDb[i] >= 0) ? red : tick_col);
            XDrawLine(display_, window_, gc_, tx0, ty0, tx1, ty1);
            std::string lab = (kDb[i] > 0) ? ("+" + std::to_string((int)kDb[i])) : std::to_string((int)kDb[i]);
            int tw = xf::text_width(st, lab);
            int lx = px + (int)((R - 26) * sx) - tw / 2;
            int ly = py + (int)((R - 26) * sy) + 4;
            xf::draw_text_at(display_, window_, gc_, st, lx, ly, lab);
        }
        // VU needle: thin black pointer with a short tail.
        double a = needle_angle_deg() * kPi / 180.0;
        double dx = std::sin(a), dy = -std::cos(a);
        double pxx = std::cos(a), pyy = std::sin(a);
        int tip_x = px + (int)((R - 4) * dx), tip_y = py + (int)((R - 4) * dy);
        int tail_x = px - (int)(12 * dx), tail_y = py - (int)(12 * dy);
        XPoint pts[4];
        pts[0].x = (short)tip_x; pts[0].y = (short)tip_y;
        pts[1].x = (short)(px + 2 * pxx); pts[1].y = (short)(py + 2 * pyy);
        pts[2].x = (short)tail_x; pts[2].y = (short)tail_y;
        pts[3].x = (short)(px - 2 * pxx); pts[3].y = (short)(py - 2 * pyy);
        XSetForeground(display_, gc_, needle_col);
        XFillPolygon(display_, window_, gc_, pts, 4, Convex, CoordModeOrigin);
        XSetForeground(display_, gc_, bezel);
        XFillArc(display_, window_, gc_, px - 7, py - 7, 14, 14, 0, 360 * 64);
        XSetForeground(display_, gc_, tick_col);
        int tw = xf::text_width(st, "VU");
        xf::draw_text_at(display_, window_, gc_, st, px - tw / 2, py - R + 34, "VU");
        int tw2 = xf::text_width(st, text_);
        xf::draw_text_at(display_, window_, gc_, st, px - tw2 / 2, py - R + 50, text_);
        if (focused_) {
            XSetForeground(display_, gc_, st.lineColor_);
            XDrawRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 3, height() - 3);
        }
        XFlush(display_);
    }
};
