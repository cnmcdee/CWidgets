#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <cmath>
#include <functional>
#include <string>

// InstKeySwitch - Key-operated selector: click inserts the key, further clicks move through the positions, K removes the key (position resets to 0).
// Attribution: XFrame.
class InstKeySwitch {
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
    bool key_in_ = false;
    int positions_ = 3;
    int position_ = 0;

public:
    std::function<void()> on_click;
    InstKeySwitch(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstKeySwitch"; text_ = "Key Switch";
    }
    ~InstKeySwitch() {}
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
    void cycle(int x, int y) { (void)x; (void)y; if (!key_in_) { set_key_in(true); return; } step(+1); }
    void step(int d) {
        if (!key_in_) return;
        int next = position_ + d;
        if (next >= positions_) next = 0;
        if (next < 0) next = positions_ - 1;
        if (next != position_) { position_ = next; if (on_click) on_click(); }
    }
    bool get_key_in() { return key_in_; }
    void set_key_in(bool v) {
        if (v == key_in_) return;
        key_in_ = v;
        if (!key_in_) position_ = 0;
        if (on_click) on_click();
    }
    int get_position() { return position_; }
    int get_positions() { return positions_; }
    void set_positions(int n) { if (n < 2) n = 2; if (n > 4) n = 4; positions_ = n; if (position_ >= positions_) position_ = positions_ - 1; }
    bool is_on() { return key_in_ && position_ > 0; }
    xframe::InstSwitchState get_state() { return is_on() ? xframe::InstSwitchState::On : xframe::InstSwitchState::Off; }

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
        int r = 95;
        XSetForeground(display_, gc_, bezel);
        XFillArc(display_, window_, gc_, cx - r - 14, cy - r - 14, 2 * r + 28, 2 * r + 28, 0, 360 * 64);
        XSetForeground(display_, gc_, panel);
        XFillArc(display_, window_, gc_, cx - r, cy - r, 2 * r, 2 * r, 0, 360 * 64);
        for (int i = 0; i < positions_; ++i) {
            double a = -2.094 + i * (4.188 / (positions_ > 1 ? (positions_ - 1) : 1));
            int tx = cx + (int)((r + 30) * std::cos(a));
            int ty = cy + (int)((r + 30) * std::sin(a)) + 5;
            xf::draw_text_at(display_, window_, gc_, st, tx - 4, ty, std::to_string(i));
        }
        double pa = -2.094 + position_ * (4.188 / (positions_ > 1 ? (positions_ - 1) : 1));
        int px = cx + (int)((r - 20) * std::cos(pa));
        int py = cy + (int)((r - 20) * std::sin(pa));
        XSetForeground(display_, gc_, dark);
        XFillArc(display_, window_, gc_, cx - 22, cy - 22, 44, 44, 0, 360 * 64);
        if (key_in_) {
            xf::set_line_width(display_, gc_, 10);
            XSetForeground(display_, gc_, amber);
            XDrawLine(display_, window_, gc_, cx, cy, px, py);
            xf::set_line_width(display_, gc_, 1);
            XSetForeground(display_, gc_, plate);
            XFillArc(display_, window_, gc_, px - 12, py - 12, 24, 24, 0, 360 * 64);
            XSetForeground(display_, gc_, dark);
            XDrawArc(display_, window_, gc_, px - 12, py - 12, 24, 24, 0, 360 * 64);
        } else {
            xf::draw_text_centered(display_, window_, gc_, st, cx - 60, cy - 10, cx + 60, cy + 10, "NO KEY");
        }
        std::string pos = key_in_ ? ("KEY IN  POS " + std::to_string(position_)) : "KEY OUT";
        xf::draw_text_centered(display_, window_, gc_, st, cx - 110, cy + r + 40, cx + 110, cy + r + 64, pos);
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
        if (key == XK_k || key == XK_K) { set_key_in(!key_in_); return true; }
        if (key == XK_space || key == XK_Return || key == XK_KP_Enter) { cycle(-1, -1); return true; }
        if (key == XK_Up || key == XK_Right) { step(+1); return true; }
        if (key == XK_Down || key == XK_Left) { step(-1); return true; }
        return false;
    }

};
