#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <cmath>
#include <functional>
#include <string>

// InstToggleSwitch - Bat-lever toggle switch: lever points up when on, down when off.
// Attribution: XFrame.
class InstToggleSwitch {
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
    bool on_ = false;

public:
    std::function<void()> on_click;
    InstToggleSwitch(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstToggleSwitch"; text_ = "Toggle";
    }
    ~InstToggleSwitch() {}
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
    void toggle() { on_ = !on_; if (on_click) on_click(); }
    void set_on(bool v) { if (on_ != v) { on_ = v; if (on_click) on_click(); } }
    bool is_on() { return on_; }
    xframe::InstSwitchState get_state() { return on_ ? xframe::InstSwitchState::On : xframe::InstSwitchState::Off; }

    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long white = styles_->get_color_rgb(255, 255, 255);
        unsigned long panel = styles_->get_color_rgb(58, 58, 62);
        unsigned long bezel = styles_->get_color_rgb(38, 38, 42);
        unsigned long plate = styles_->get_color_rgb(210, 210, 210);
        unsigned long rocker = styles_->get_color_rgb(190, 190, 195);
        unsigned long dark = styles_->get_color_rgb(40, 40, 40);
        unsigned long amber = styles_->get_color_rgb(255, 176, 0);
        unsigned long green = styles_->get_color_rgb(0, 180, 0);
        unsigned long greenlit = styles_->get_color_rgb(60, 255, 90);
        unsigned long red = styles_->get_color_rgb(200, 30, 30);
        unsigned long redlit = styles_->get_color_rgb(255, 70, 60);
        unsigned long dimred = styles_->get_color_rgb(120, 30, 30);
        unsigned long outline = styles_->get_color_rgb(120, 120, 120);
        (void)white; (void)panel; (void)bezel; (void)plate; (void)rocker; (void)dark;
        (void)amber; (void)green; (void)greenlit; (void)red; (void)redlit; (void)dimred; (void)outline;
        XSetForeground(display_, gc_, white);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        int cx = (x1_ + x2_) / 2;
        int cy = (y1_ + y2_) / 2;
        XSetForeground(display_, gc_, bezel);
        XFillRectangle(display_, window_, gc_, cx - 110, cy - 90, 220, 180);
        XSetForeground(display_, gc_, panel);
        XFillRectangle(display_, window_, gc_, cx - 96, cy - 76, 192, 152);
        int lx = on_ ? cx + 52 : cx - 52;
        int ly = on_ ? cy - 78 : cy + 78;
        XSetForeground(display_, gc_, dark);
        XDrawLine(display_, window_, gc_, cx, cy, lx, ly);
        xf::set_line_width(display_, gc_, 10);
        XSetForeground(display_, gc_, plate);
        XDrawLine(display_, window_, gc_, cx, cy, lx, ly);
        xf::set_line_width(display_, gc_, 1);
        XSetForeground(display_, gc_, on_ ? greenlit : redlit);
        XFillArc(display_, window_, gc_, lx - 16, ly - 16, 32, 32, 0, 360 * 64);
        XSetForeground(display_, gc_, dark);
        XDrawArc(display_, window_, gc_, lx - 16, ly - 16, 32, 32, 0, 360 * 64);
        XSetForeground(display_, gc_, dark);
        XFillArc(display_, window_, gc_, cx - 26, cy - 26, 52, 52, 0, 360 * 64);
        XSetForeground(display_, gc_, plate);
        XFillArc(display_, window_, gc_, cx - 18, cy - 18, 36, 36, 0, 360 * 64);
        XSetForeground(display_, gc_, dark);
        XDrawArc(display_, window_, gc_, cx - 26, cy - 26, 52, 52, 0, 360 * 64);
        xf::draw_text_centered(display_, window_, gc_, st, cx - 110, cy - 130, cx + 110, cy - 106, on_ ? "ON" : "OFF");
        xf::draw_text_centered(display_, window_, gc_, st, x1_, y2_ - 28, x2_, y2_ - 6, text_);
        XSetForeground(display_, gc_, st.lineColor_);
        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        if (focused_) {
            XSetForeground(display_, gc_, amber);
            XDrawRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 5, height() - 5);
        }
        XFlush(display_);
    }
    bool on_button_press(int x, int y, unsigned int button) {
        if (!contains(x, y) || !enabled_) return false;
        focused_ = true;
        if (button == 1) { pressed_ = true; toggle(); return true; }
        return false;
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
        if (key == XK_space || key == XK_Return || key == XK_KP_Enter) { toggle(); return true; }
        return false;
    }

};
