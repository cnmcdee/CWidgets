#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

// InstBezel - mounting bezel frame with an open window and corner screws.
// Attribution: XFrame.
class InstBezel {
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

public:
    InstBezel(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstBezel"; text_ = "GAUGE";

    }
    ~InstBezel() {}
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
    SStyle style() { SStyle s = styles_->resolve(style_name_); XFontStruct* bf = xframe::inst_bold_font(display_); if (bf) s.font_ = bf; return s; }
    std::function<void()> on_click;
    void draw_screw(int cx, int cy) {
        unsigned long g = styles_->get_color_rgb(150, 150, 150);
        unsigned long ik = styles_->get_color_rgb(40, 40, 40);
        XSetForeground(display_, gc_, g);
        XFillArc(display_, window_, gc_, cx - 6, cy - 6, 12, 12, 0, 360 * 64);
        XSetForeground(display_, gc_, ik);
        XDrawArc(display_, window_, gc_, cx - 6, cy - 6, 12, 12, 0, 360 * 64);
        XDrawLine(display_, window_, gc_, cx - 4, cy, cx + 4, cy);
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
        XSetForeground(display_, gc_, panel);
        XFillRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, height() - 4);
        XSetForeground(display_, gc_, ink);
        XDrawRectangle(display_, window_, gc_, x1_ + 4, y1_ + 4, width() - 8, height() - 8);
        int ix = x1_ + 26, iy = y1_ + 26, iw = width() - 52, ih = height() - 52;
        XSetForeground(display_, gc_, white);
        XFillRectangle(display_, window_, gc_, ix, iy, iw, ih);
        XSetForeground(display_, gc_, ink);
        XDrawRectangle(display_, window_, gc_, ix, iy, iw, ih);
        XDrawRectangle(display_, window_, gc_, ix + 3, iy + 3, iw - 6, ih - 6);
        int tw = xf::text_width(st, text_);
        xf::draw_text_at(display_, window_, gc_, st, x1_ + (width() - tw) / 2, y2_ - 12, text_);
        draw_screw(x1_ + 16, y1_ + 16);
        draw_screw(x2_ - 16, y1_ + 16);
        draw_screw(x1_ + 16, y2_ - 16);
        draw_screw(x2_ - 16, y2_ - 16);

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

        return false;
    }
};
