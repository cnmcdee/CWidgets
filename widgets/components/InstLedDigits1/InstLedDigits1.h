#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>

// InstLedDigits1 - old 7-segment LED digit display, 1 digit(s), vintage red
// LED style: dark bezel, lit segments bright red, unlit segments faint.
// Shows the value with 0 decimal place(s); range 0 .. 9.
// Attribution: XFrame.
class InstLedDigits1 {
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
    int decimal_places_ = 0;

    void draw_digit(int x, int y, int w, int h, int t, int digit,
                    unsigned long lit, unsigned long dim) {
        struct Seg { char id; int sx, sy, sw, sh; };
        Seg segs[7] = {
            {'a', x + t, y, w - 2 * t, t},
            {'b', x + w - t, y + t, t, h / 2 - t},
            {'c', x + w - t, y + h / 2, t, h / 2 - t},
            {'d', x + t, y + h - t, w - 2 * t, t},
            {'e', x, y + h / 2, t, h / 2 - t},
            {'f', x, y + t, t, h / 2 - t},
            {'g', x + t, y + h / 2 - t / 2, w - 2 * t, t},
        };
        for (int i = 0; i < 7; ++i) {
            XSetForeground(display_, gc_, segment_on(digit, segs[i].id) ? lit : dim);
            XFillRectangle(display_, window_, gc_, segs[i].sx, segs[i].sy,
                           std::max(1, segs[i].sw), std::max(1, segs[i].sh));
        }
    }

public:
    static constexpr int kDigits = 1;
    std::function<void()> on_click;

    InstLedDigits1(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstLedDigits1"; text_ = "InstLedDigits1";
        value_ = xframe::inst_demo_value(0.0, max_value(), 42);
    }
    ~InstLedDigits1() {}
    void set_bounds(int x, int y, int width, int height) { x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height; }
    int width() const { return x2_ - x1_; }
    int height() const { return y2_ - y1_; }
    bool contains(int x, int y) { return visible_ && x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_; }
    void set_visible(bool v) { visible_ = v; }
    bool get_visible() { return visible_; }
    void set_enabled(bool e) { enabled_ = e; }
    bool get_enabled() { return enabled_; }
    bool get_focused() { return focused_; }
    bool get_pressed() { return pressed_; }
    void set_style_name(std::string s) { style_name_ = s; }
    void set_text(std::string t) { text_ = t; }
    std::string get_text() { return text_; }
    SStyle style() { SStyle s = styles_->resolve(style_name_); XFontStruct* bf = xframe::inst_bold_font(display_); if (bf) s.font_ = bf; return s; }

    double max_value() const {
        double m = 1.0;
        for (int i = 0; i < kDigits; ++i) m *= 10.0;
        m -= 1.0;
        double scale = 1.0;
        for (int i = 0; i < decimal_places_; ++i) scale *= 10.0;
        return m / scale;
    }
    void set_value(double v) { value_ = std::max(0.0, std::min(max_value(), v)); }
    double get_value() const { return value_; }
    void set_decimal_places(int dp) {
        decimal_places_ = std::max(0, std::min(2, dp));
        set_value(value_);
    }
    int get_decimal_places() const { return decimal_places_; }
    double step() const {
        double s = 1.0;
        for (int i = 0; i < decimal_places_; ++i) s /= 10.0;
        return s;
    }
    // Digits actually shown, zero-padded to kDigits (decimal point not included).
    std::string shown_digits() const {
        double scale = 1.0;
        for (int i = 0; i < decimal_places_; ++i) scale *= 10.0;
        long long scaled = (long long)std::llround(value_ * scale);
        long long cap = 1;
        for (int i = 0; i < kDigits; ++i) cap *= 10;
        cap -= 1;
        if (scaled > cap) scaled = cap;
        if (scaled < 0) scaled = 0;
        std::string s = std::to_string(scaled);
        if ((int)s.size() < kDigits) s = std::string(kDigits - s.size(), '0') + s;
        return s;
    }
    int digit_at(int i) const {
        std::string s = shown_digits();
        if (i < 0 || i >= (int)s.size()) return 0;
        return s[i] - '0';
    }
    static bool segment_on(int digit, char seg) {
        static const char* kSegs[10] = {"abcdef", "bc", "abdeg", "abcdg", "bcfg",
                                        "acdfg", "acdefg", "abc", "abcdefg", "abcdfg"};
        if (digit < 0 || digit > 9) return false;
        return std::string(kSegs[digit]).find(seg) != std::string::npos;
    }
    // Decimal point sits after this digit index (before the last
    // decimal_places_ digits); -1 when there is no decimal point.
    int decimal_point_after() const {
        if (decimal_places_ <= 0) return -1;
        return kDigits - decimal_places_ - 1;
    }

    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long bg = styles_->get_color_rgb(12, 12, 14);
        unsigned long bezel = styles_->get_color_rgb(58, 58, 62);
        unsigned long lit = styles_->get_color_rgb(255, 60, 35);
        unsigned long dim = styles_->get_color_rgb(52, 14, 11);
        unsigned long label_col = styles_->get_color_rgb(170, 170, 170);
        XSetForeground(display_, gc_, bg);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        XSetForeground(display_, gc_, bezel);
        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);
        XDrawRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 5, height() - 5);
        XSetForeground(display_, gc_, label_col);
        xf::draw_text_at(display_, window_, gc_, st, x1_ + 6, y1_ + 12, text_);
        int label_h = 16;
        int ax = x1_ + 6, ay = y1_ + label_h + 4, aw = width() - 12, ah = height() - label_h - 10;
        if (aw >= 10 && ah >= 10) {
            int gap = 4;
            int dp_w = (decimal_places_ > 0) ? 8 : 0;
            int cell_w = (aw - gap * (kDigits - 1) - dp_w) / kDigits;
            int cell_h = ah;
            int t = std::max(2, cell_w / 6);
            std::string s = shown_digits();
            int cx = ax;
            for (int i = 0; i < kDigits; ++i) {
                draw_digit(cx, ay, cell_w, cell_h, t, s[i] - '0', lit, dim);
                cx += cell_w + gap;
                if (i == decimal_point_after()) {
                    XSetForeground(display_, gc_, lit);
                    XFillRectangle(display_, window_, gc_, cx - gap + 1, ay + cell_h - t - 1, t + 1, t + 1);
                    cx += dp_w;
                }
            }
        }
        if (focused_) {
            XSetForeground(display_, gc_, st.lineColor_);
            XDrawRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 3, height() - 3);
        }
        XFlush(display_);
    }
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
        switch (key) {
        case XK_Up: set_value(get_value() + step()); return true;
        case XK_Down: set_value(get_value() - step()); return true;
        case XK_Home: set_value(0.0); return true;
        case XK_Return: case XK_space:
            if (on_click) on_click();
            return true;
        }
        return false;
    }
};
