#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <sstream>
#include <string>

// InstHorizontalTankLevel - Horizontal bullet tank, fill by cross-section width, percent readout, hi/lo alarm markers.
// Attribution: XFrame.
class InstHorizontalTankLevel {
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
    bool hovered_ = false;
    double level_;
    xframe::InstAlarmBands bands_;
    std::function<void()> on_click_;

public:
    InstHorizontalTankLevel(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height, double level = xframe::inst_demo_level(43)) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstHorizontalTankLevel"; text_ = "InstHorizontalTankLevel";
        level_ = xframe::inst_clamp_level(level);
    }
    ~InstHorizontalTankLevel() {}
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
    void set_level(double pct) { level_ = xframe::inst_clamp_level(pct); }
    double get_level() { return level_; }
    void set_alarm_bands(const xframe::InstAlarmBands& b) { bands_ = b; }
    const xframe::InstAlarmBands& alarm_bands() { return bands_; }
    void set_on_click(std::function<void()> f) { on_click_ = f; }
    static std::string fmt(double v) {
        std::ostringstream oss; oss.precision(2); oss << std::fixed << v;
        std::string s = oss.str();
        while (!s.empty() && s.back() == '0') s.pop_back();
        if (!s.empty() && s.back() == '.') s.pop_back();
        return s;
    }
    double level_at_point(int x, int y) {
        (void)y;
        int tx = x1_ + 40;
        int tw = width() - 180;
        double f = (double)(x - tx) / (double)tw;
        return xframe::inst_clamp_level(f * 100.0);
    }
    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long white = styles_->get_color_rgb(255, 255, 255);
        unsigned long ink = styles_->get_color_rgb(60, 60, 60);
        unsigned long pale = styles_->get_color_rgb(235, 242, 248);
        unsigned long fillc = styles_->get_color_rgb(40, 120, 200);
        unsigned long grid = styles_->get_color_rgb(210, 210, 210);
        unsigned long amber = styles_->get_color_rgb(220, 150, 0);
        unsigned long red = styles_->get_color_rgb(200, 30, 30);
        XSetForeground(display_, gc_, white);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        int tx = x1_ + 40;
        int ty = y1_ + 90;
        int tw = width() - 180;
        int th = 150;
        XSetForeground(display_, gc_, pale);
        XFillRectangle(display_, window_, gc_, tx, ty, tw, th);
        int fw = (int)(tw * level_ / 100.0);
        if (fw > 0) {
            XSetForeground(display_, gc_, fillc);
            XFillRectangle(display_, window_, gc_, tx, ty, fw, th);
        }
        XSetForeground(display_, gc_, ink);
        XDrawLine(display_, window_, gc_, tx, ty, tx + tw, ty);
        XDrawLine(display_, window_, gc_, tx, ty + th, tx + tw, ty + th);
        XDrawArc(display_, window_, gc_, tx - th / 2, ty, th, th, 90 * 64, 180 * 64);
        XDrawArc(display_, window_, gc_, tx + tw - th / 2, ty, th, th, 270 * 64, 180 * 64);
        for (int v = 0; v <= 100; v += 25) {
            int gx = tx + (int)(tw * v / 100.0);
            XSetForeground(display_, gc_, grid);
            XDrawLine(display_, window_, gc_, gx, ty + th + 4, gx, ty + th + 14);
            std::string lab = std::to_string(v);
            int lw = xf::text_width(st, lab);
            xf::draw_text_at(display_, window_, gc_, st, gx - lw / 2, ty + th + 28, lab);
        }
        auto mark = [&](double pct, unsigned long c, const char* lab) {
            int gx = tx + (int)(tw * pct / 100.0);
            XSetForeground(display_, gc_, c);
            XDrawLine(display_, window_, gc_, gx, ty, gx, ty + th);
            xf::draw_text_at(display_, window_, gc_, st, gx + 4, ty - 8, lab);
        };
        mark(bands_.hi_hi, red, "HI-HI");
        mark(bands_.hi, amber, "HI");
        mark(bands_.lo, amber, "LO");
        mark(bands_.lo_lo, red, "LO-LO");
        std::string pct = fmt(level_) + "%";
        xf::draw_text_at(display_, window_, gc_, st, tx + tw + 12, ty + th / 2 + 4, pct);
        xf::draw_text_at(display_, window_, gc_, st, x1_ + 8, y1_ + 16, text_);
        XSetForeground(display_, gc_, hovered_ ? styles_->get_color_rgb(255, 170, 0) : st.lineColor_);
        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        XFlush(display_);
    }
    bool on_button_press(int x, int y, unsigned int button) {
        if (!contains(x, y) || !enabled_) return false;
        focused_ = true;
        if (button == 4) { set_level(level_ + 5.0); return true; }
        if (button == 5) { set_level(level_ - 5.0); return true; }
        if (button == 1) {
            pressed_ = true;
            set_level(level_at_point(x, y));
            if (on_click_) on_click_();
            return true;
        }
        return false;
    }
    bool on_button_release(int x, int y, unsigned int button) {
        (void)x; (void)y;
        if (button == 1 && pressed_) { pressed_ = false; return true; }
        return false;
    }
    bool on_motion(int x, int y) {
        bool h = contains(x, y);
        if (h == hovered_) return false;
        hovered_ = h;
        return true;
    }
    bool on_key(KeySym key, const char* str) {
        (void)str;
        (void)key;
        if (!focused_ || !enabled_) return false;
        switch (key) {
        case XK_Up: case XK_Right: set_level(level_ + 5.0); return true;
        case XK_Down: case XK_Left: set_level(level_ - 5.0); return true;
        case XK_Home: set_level(0.0); return true;
        case XK_End: set_level(100.0); return true;
        case XK_r: case XK_R: set_level(xframe::inst_demo_level(42)); return true;
        }
        return false;
    }
};
