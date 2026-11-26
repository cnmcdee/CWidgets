#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

// InstFlowMeter - rotameter: a float rides in a tapered tube, 0 to 100 L/min. Click sets the flow.
// Attribution: XFrame.
class InstFlowMeter {
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
    double max_ = 100;
    double value_ = 40.0;

public:
    InstFlowMeter(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height, double value = 40.0) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstFlowMeter"; text_ = "Flow";
        value_ = value;

    }
    ~InstFlowMeter() {}
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
        int cx = (x1_ + x2_) / 2;
        int top = y1_ + 34, bot = y2_ - 34;
        int topw = 90, botw = 34;
        XPoint pts[4] = {{(short)(cx - topw / 2), (short)top}, {(short)(cx + topw / 2), (short)top},
                         {(short)(cx + botw / 2), (short)bot}, {(short)(cx - botw / 2), (short)bot}};
        XSetForeground(display_, gc_, face);
        XFillPolygon(display_, window_, gc_, pts, 4, Convex, CoordModeOrigin);
        XSetForeground(display_, gc_, ink);
        XDrawLine(display_, window_, gc_, cx - topw / 2, top, cx + topw / 2, top);
        XDrawLine(display_, window_, gc_, cx + topw / 2, top, cx + botw / 2, bot);
        XDrawLine(display_, window_, gc_, cx + botw / 2, bot, cx - botw / 2, bot);
        XDrawLine(display_, window_, gc_, cx - botw / 2, bot, cx - topw / 2, top);
        for (int i = 0; i <= 10; ++i) {
            double f = i / 10.0;
            int gy = bot - (int)(f * (bot - top));
            int hw = (int)(botw / 2 + f * (topw - botw) / 2);
            XDrawLine(display_, window_, gc_, cx + hw, gy, cx + hw + 8, gy);
            if (i % 2 == 0) {
                std::string lab = fmt(f * 100.0);
                xf::draw_text_at(display_, window_, gc_, st, cx + hw + 12, gy + 4, lab);
            }
        }
        int fy = bot - (int)(fraction() * (bot - top));
        XSetForeground(display_, gc_, red);
        XFillArc(display_, window_, gc_, cx - 11, fy - 11, 22, 22, 0, 360 * 64);
        XSetForeground(display_, gc_, ink);
        XDrawArc(display_, window_, gc_, cx - 11, fy - 11, 22, 22, 0, 360 * 64);
        std::string val = text_ + "  " + fmt(value_) + " L/min";
        int vw = xf::text_width(st, val);
        xf::draw_text_at(display_, window_, gc_, st, x1_ + (width() - vw) / 2, y1_ + 18, val);

        XSetForeground(display_, gc_, st.lineColor_);
        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        XFlush(display_);
    }
    bool on_button_press(int x, int y, unsigned int button) {
        if (!contains(x, y) || !enabled_) return false;
        (void)button;
        focused_ = true; pressed_ = true;
        int top = y1_ + 34, bot = y2_ - 34;
        set_value(min_ + std::max(0.0, std::min(1.0, (double)(bot - y) / (bot - top))) * (max_ - min_));

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
