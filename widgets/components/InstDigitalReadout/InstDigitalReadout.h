#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

// InstDigitalReadout - 7-segment style numeric display, 0 to 999.9.
// Attribution: XFrame.
class InstDigitalReadout {
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
    bool hover_ = false;
    bool pressed_ = false;
    double min_ = 0;
    double max_ = 999.9;
    double value_ = 123.4;

public:
    InstDigitalReadout(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height, double value = 123.4) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstDigitalReadout"; text_ = "Readout";
        value_ = value;

    }
    ~InstDigitalReadout() {}
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
    std::function<void()> on_click;
    void set_value(double v) { value_ = std::max(min_, std::min(max_, v)); }
    double get_value() const { return value_; }
    void set_range(double lo, double hi) { if (hi > lo) { min_ = lo; max_ = hi; set_value(value_); } }
    double min_value() const { return min_; }
    double max_value() const { return max_; }
    double fraction() const { return (max_ > min_) ? (value_ - min_) / (max_ - min_) : 0.0; }
    void draw_segment(int x, int y, int w, int h, bool on, unsigned long col, unsigned long off) {
        XSetForeground(display_, gc_, on ? col : off);
        XFillRectangle(display_, window_, gc_, x, y, w, h);
    }
    void draw_digit(int x, int y, int w, int h, int d) {
        static const bool seg[10][7] = {{1,1,1,1,1,1,0},{0,1,1,0,0,0,0},{1,1,0,1,1,0,1},{1,1,1,1,0,0,1},{0,1,1,0,0,1,1},{1,0,1,1,0,1,1},{1,0,1,1,1,1,1},{1,1,1,0,0,0,0},{1,1,1,1,1,1,1},{1,1,1,1,0,1,1}};
        unsigned long on = styles_->get_color_rgb(255, 176, 0);
        unsigned long off = styles_->get_color_rgb(50, 45, 20);
        int t = 6;
        draw_segment(x + t, y, w - 2 * t, t, seg[d][0], on, off);
        draw_segment(x + w - t, y + t, t, h / 2 - 2 * t, seg[d][1], on, off);
        draw_segment(x + w - t, y + h / 2 + t, t, h / 2 - 2 * t, seg[d][2], on, off);
        draw_segment(x + t, y + h - t, w - 2 * t, t, seg[d][3], on, off);
        draw_segment(x, y + h / 2 + t, t, h / 2 - 2 * t, seg[d][4], on, off);
        draw_segment(x, y + t, t, h / 2 - 2 * t, seg[d][5], on, off);
        draw_segment(x + t, y + h / 2 - t / 2, w - 2 * t, t, seg[d][6], on, off);
    }

    static std::string fmt(double v, int prec = 0) {
        std::ostringstream oss; oss.precision(prec); oss << std::fixed << v;
        return oss.str();
    }
    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long white = styles_->get_color_rgb(255, 255, 255);
        unsigned long face = styles_->get_color_rgb(245, 245, 240);
        unsigned long ink = styles_->get_color_rgb(40, 40, 40);
        unsigned long grey = styles_->get_color_rgb(150, 150, 150);
        unsigned long red = styles_->get_color_rgb(200, 30, 30);
        unsigned long amber = styles_->get_color_rgb(255, 176, 0);
        unsigned long green = styles_->get_color_rgb(0, 150, 0);
        unsigned long blue = styles_->get_color_rgb(30, 120, 200);
        unsigned long dark = styles_->get_color_rgb(30, 30, 30);
        unsigned long panel = styles_->get_color_rgb(210, 210, 205);
        (void)white; (void)face; (void)ink; (void)grey; (void)red; (void)amber;
        (void)green; (void)blue; (void)dark; (void)panel;
        XSetForeground(display_, gc_, white);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        int tw0 = xf::text_width(st, text_);
        xf::draw_text_at(display_, window_, gc_, st, x1_ + (width() - tw0) / 2, y1_ + 18, text_);
        int dx = x1_ + 12, dy = y1_ + 30, dw = width() - 24, dh = height() - 42;
        XSetForeground(display_, gc_, dark);
        XFillRectangle(display_, window_, gc_, dx, dy, dw, dh);
        XSetForeground(display_, gc_, ink);
        XDrawRectangle(display_, window_, gc_, dx, dy, dw, dh);
        std::string s = fmt(value_, 1);
        int digw = 34, digh = 64, gap = 8;
        int totalw = 0;
        for (char c : s) totalw += (c == '.') ? 12 : (c == '-' ? 20 : digw + gap);
        totalw -= gap;
        int tx = dx + (dw - totalw) / 2;
        int ty = dy + (dh - digh) / 2;
        for (char c : s) {
            if (c == '.') {
                XSetForeground(display_, gc_, amber);
                XFillRectangle(display_, window_, gc_, tx, ty + digh - 8, 8, 8);
                tx += 12;
            } else if (c == '-') {
                XSetForeground(display_, gc_, amber);
                XFillRectangle(display_, window_, gc_, tx, ty + digh / 2 - 3, 16, 6);
                tx += 20;
            } else if (c >= '0' && c <= '9') {
                draw_digit(tx, ty, digw, digh, c - '0');
                tx += digw + gap;
            }
        }

        XSetForeground(display_, gc_, st.lineColor_);
        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        XFlush(display_);
    }
    bool on_button_press(int x, int y, unsigned int button) {
        if (!contains(x, y) || !enabled_) return false;
        (void)button;
        focused_ = true; pressed_ = true;

        if (on_click) on_click();
        return true;
    }
    bool on_button_release(int x, int y, unsigned int button) {
        (void)x; (void)y;
        if (button == 1 && pressed_) { pressed_ = false; return true; }
        return false;
    }
    bool on_motion(int x, int y) {
        hover_ = contains(x, y);
        return hover_;
    }
    bool on_key(KeySym key, const char* str) {
        (void)str;
        (void)key;
        if (!focused_ || !enabled_) return false;
        double step = (max_ - min_) * 0.05;
        switch (key) {
        case XK_Left: case XK_Down: set_value(value_ - step); return true;
        case XK_Right: case XK_Up: set_value(value_ + step); return true;
        case XK_Home: set_value((min_ + max_) / 2.0); return true;
        }

        return false;
    }
};
