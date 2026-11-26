#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
#include <utility>
#include <vector>

// // InstStackLight - 3-tier tower light: red (top), amber, green tiers, each an independent illuminator with its own on/off + brightness. Click a tier to toggle it; keys 1/2/3 toggle red/amber/green.
// Attribution: XFrame.
class InstStackLight {
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
    bool hovered_ = false;
    bool pressed_ = false;
    xframe::InstLamp red_;
    xframe::InstLamp amber_;
    xframe::InstLamp green_;
public:
    std::function<void()> on_click;

    InstStackLight(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstStackLight"; text_ = "STACK";

    }
    ~InstStackLight() {}
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
    bool is_pressed() const { return pressed_; }
    bool is_hovered() const { return hovered_; }
    void set_red(bool v) { red_.on = v; }
    void set_amber(bool v) { amber_.on = v; }
    void set_green(bool v) { green_.on = v; }
    bool is_red_on() const { return red_.on; }
    bool is_amber_on() const { return amber_.on; }
    bool is_green_on() const { return green_.on; }
    void set_red_brightness(int b) { red_.brightness = (int)xframe::inst_clamp_level((double)b); }
    void set_amber_brightness(int b) { amber_.brightness = (int)xframe::inst_clamp_level((double)b); }
    void set_green_brightness(int b) { green_.brightness = (int)xframe::inst_clamp_level((double)b); }
    int get_red_brightness() const { return red_.brightness; }
    int get_amber_brightness() const { return amber_.brightness; }
    int get_green_brightness() const { return green_.brightness; }
    xframe::InstLamp red_lamp() const { return red_; }
    xframe::InstLamp amber_lamp() const { return amber_; }
    xframe::InstLamp green_lamp() const { return green_; }
    void set_all(bool v) { red_.on = v; amber_.on = v; green_.on = v; }
    bool is_on() const { return red_.on || amber_.on || green_.on; }

    int tier_h() { return std::max(20, (height() - 80) / 3); }
    int tier_y(int i) { return y1_ + 8 + i * (tier_h() + 4); }
    void draw_tier(int x, int y, int w, int h, const xframe::InstLamp& t, int r, int g, int b, const char* lab) {
        double f = t.on ? (0.08 + 0.92 * t.brightness / 100.0) : 0.10;
        unsigned long c = styles_->get_color_rgb((unsigned char)(r * f), (unsigned char)(g * f), (unsigned char)(b * f));
        XSetForeground(display_, gc_, c);
        XFillRectangle(display_, window_, gc_, x, y, w, h);
        XSetForeground(display_, gc_, styles_->get_color_rgb(60, 60, 60));
        XDrawRectangle(display_, window_, gc_, x, y, w, h);
        SStyle st = style();
        int tw = xf::text_width(st, lab);
        xf::draw_text_at(display_, window_, gc_, st, x + (w - tw) / 2, y + h / 2 + 5, lab);
    }

    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long white = styles_->get_color_rgb(255, 255, 255);
        unsigned long dark = styles_->get_color_rgb(40, 40, 40);
        XSetForeground(display_, gc_, white);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        int cx = (x1_ + x2_) / 2;
        int tw2 = 70;
        int th = tier_h();
        draw_tier(cx - tw2 / 2, tier_y(0), tw2, th, red_, 220, 30, 30, "R");
        draw_tier(cx - tw2 / 2, tier_y(1), tw2, th, amber_, 230, 160, 20, "A");
        draw_tier(cx - tw2 / 2, tier_y(2), tw2, th, green_, 30, 200, 60, "G");
        int pole_top = tier_y(2) + th + 4;
        int pole_bot = std::max(pole_top + 1, y2_ - 30);
        XSetForeground(display_, gc_, dark);
        XFillRectangle(display_, window_, gc_, cx - 4, pole_top, 8, pole_bot - pole_top);
        XFillRectangle(display_, window_, gc_, cx - 50, y2_ - 30, 100, 12);
        int tw = xf::text_width(st, text_);
        xf::draw_text_at(display_, window_, gc_, st, cx - tw / 2, y2_ - 6, text_);
        if (hovered_ || focused_) {
            XSetForeground(display_, gc_, styles_->get_color_rgb(0, 120, 200));
            XDrawRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 2, height() - 2);
        }
        XSetForeground(display_, gc_, st.lineColor_);
        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        XFlush(display_);
    }
    bool on_button_press(int x, int y, unsigned int button) {
        if (!contains(x, y) || !enabled_) return false;
        focused_ = true;
        if (button == 1) {
            pressed_ = true;
            int th = tier_h();
            for (int i = 0; i < 3; ++i) {
                if (y >= tier_y(i) && y <= tier_y(i) + th) {
                    if (i == 0) red_.on = !red_.on;
                    else if (i == 1) amber_.on = !amber_.on;
                    else green_.on = !green_.on;
                    break;
                }
            }
            if (on_click) on_click();
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
        hovered_ = contains(x, y);
        return hovered_;
    }
    bool on_key(KeySym key, const char* str) {
        (void)str;
        if (!focused_ || !enabled_) return false;
        if (key == XK_1) { red_.on = !red_.on; return true; }
        if (key == XK_2) { amber_.on = !amber_.on; return true; }
        if (key == XK_3) { green_.on = !green_.on; return true; }
        if (key == XK_space) { if (is_on()) set_all(false); else set_all(true); return true; }
        return false;
    }
};
