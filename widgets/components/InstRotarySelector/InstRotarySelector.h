#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <cmath>
#include <functional>
#include <string>

// InstRotarySelector - Rotary selector switch with 2, 3 or 4 positions and a pointer. Click (or Space) advances to the next position.
// Attribution: XFrame.
class InstRotarySelector {
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
    int positions_ = 3;
    int position_ = 0;

public:
    std::function<void()> on_click;
    InstRotarySelector(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstRotarySelector"; text_ = "Rotary";
    }
    ~InstRotarySelector() {}
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
    void cycle(int x, int y) { (void)x; (void)y; step(+1); }
    void step(int d) {
        int next = position_ + d;
        if (next >= positions_) next = 0;
        if (next < 0) next = positions_ - 1;
        if (next != position_) { position_ = next; if (on_click) on_click(); }
    }
    int get_position() { return position_; }
    int get_positions() { return positions_; }
    void set_positions(int n) { if (n < 2) n = 2; if (n > 4) n = 4; positions_ = n; if (position_ >= positions_) position_ = positions_ - 1; }
    void set_position(int p) { if (p < 0) p = 0; if (p >= positions_) p = positions_ - 1; if (p != position_) { position_ = p; if (on_click) on_click(); } }
    bool is_on() { return position_ > 0; }
    xframe::InstSwitchState get_state() { return position_ > 0 ? xframe::InstSwitchState::On : xframe::InstSwitchState::Off; }

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
        int r = 100;
        XSetForeground(display_, gc_, bezel);
        XFillArc(display_, window_, gc_, cx - r - 14, cy - r - 14, 2 * r + 28, 2 * r + 28, 0, 360 * 64);
        XSetForeground(display_, gc_, panel);
        XFillArc(display_, window_, gc_, cx - r, cy - r, 2 * r, 2 * r, 0, 360 * 64);
        for (int i = 0; i < positions_; ++i) {
            double a = -2.094 + i * (4.188 / (positions_ > 1 ? (positions_ - 1) : 1));
            int tx = cx + (int)((r + 34) * std::cos(a));
            int ty = cy + (int)((r + 34) * std::sin(a)) + 5;
            std::string lab = std::to_string(i);
            xf::draw_text_at(display_, window_, gc_, st, tx - 4, ty, lab);
            int mx = cx + (int)((r - 6) * std::cos(a));
            int my = cy + (int)((r - 6) * std::sin(a));
            XSetForeground(display_, gc_, dark);
            XFillArc(display_, window_, gc_, mx - 4, my - 4, 8, 8, 0, 360 * 64);
        }
        double pa = -2.094 + position_ * (4.188 / (positions_ > 1 ? (positions_ - 1) : 1));
        int px = cx + (int)((r - 14) * std::cos(pa));
        int py = cy + (int)((r - 14) * std::sin(pa));
        xf::set_line_width(display_, gc_, 8);
        XSetForeground(display_, gc_, amber);
        XDrawLine(display_, window_, gc_, cx, cy, px, py);
        xf::set_line_width(display_, gc_, 1);
        XSetForeground(display_, gc_, plate);
        XFillArc(display_, window_, gc_, cx - 26, cy - 26, 52, 52, 0, 360 * 64);
        XSetForeground(display_, gc_, dark);
        XDrawArc(display_, window_, gc_, cx - 26, cy - 26, 52, 52, 0, 360 * 64);
        std::string pos = "POS " + std::to_string(position_) + " / " + std::to_string(positions_);
        xf::draw_text_centered(display_, window_, gc_, st, cx - 110, cy + r + 44, cx + 110, cy + r + 68, pos);
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
        if (button == 1) { pressed_ = true; cycle(x, y); return true; }
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
        if (key == XK_space || key == XK_Return || key == XK_KP_Enter) { cycle(-1, -1); return true; }
        if (key == XK_Up || key == XK_Right) { step(+1); return true; }
        if (key == XK_Down || key == XK_Left) { step(-1); return true; }
        return false;
    }

};
