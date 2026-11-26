#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <string>
#include <vector>

// InstSeg14Display3 - old 14-segment LED alphanumeric display, 3 digit(s),
// vintage amber LED style: dark bezel, lit segments bright amber, unlit
// segments faint. Shows a string value (set_value/get_value); characters
// outside the 14-segment repertoire render as their closest
// approximation, or blank when no approximation exists.
// Repertoire (14-segment):
//   Clean: 0123456789ABCDEFGHIJLOPSTUX
//   Approximate (renders, but only roughly): KMNQRVWYZ'":
//   Cannot show (renders blank): ;,?!@#$%&*()[]{}<>\|^`~
//   Known look-alikes: '0'/'O', '5'/'S', '6'/'G', 'J'/'U' render identically.
//   Lowercase is shown as uppercase.
// Attribution: XFrame.
class InstSeg14Display3 {
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
    std::string value_ = "ABC";

    static const std::map<char, std::string>& segment_map() {
        static const std::map<char, std::string> kMap = {
        {' ', ""},
        {'\"', "h j"},
        {'\'', "j"},
        {'+', "g1 g2 i l"},
        {'-', "g1 g2"},
        {'/', "j k"},
        {'0', "a b c d e f"},
        {'1', "b c"},
        {'2', "a b d e g1 g2"},
        {'3', "a b c d g1 g2"},
        {'4', "b c f g1 g2"},
        {'5', "a c d f g1 g2"},
        {'6', "a c d e f g1 g2"},
        {'7', "a b c"},
        {'8', "a b c d e f g1 g2"},
        {'9', "a b c d f g1 g2"},
        {'=', "d g1 g2"},
        {'A', "a b c e f g1 g2"},
        {'B', "a b c d g1 g2 i l"},
        {'C', "a d e f"},
        {'D', "a b c d i l"},
        {'E', "a d e f g1 g2"},
        {'F', "a e f g1 g2"},
        {'G', "a c d e f g1 g2"},
        {'H', "b c e f g1 g2"},
        {'I', "i l"},
        {'J', "b c d e"},
        {'K', "e f g1 j k"},
        {'L', "d e f"},
        {'M', "b c e f h j"},
        {'N', "b c e f h m"},
        {'O', "a b c d e f"},
        {'P', "a b e f g1 g2"},
        {'Q', "a b c d e f m"},
        {'R', "a b e f g1 g2 m"},
        {'S', "a c d f g1 g2"},
        {'T', "a i l"},
        {'U', "b c d e"},
        {'V', "c e j k"},
        {'W', "b c e f k m"},
        {'X', "h j k m"},
        {'Y', "b c f g1 g2 l"},
        {'Z', "a d j k"},
        {'_', "d"},
        };
        return kMap;
    }

    // Normalise a raw string to the displayable value: uppercase, drop
    // characters with no approximation, keep at most kDigits display cells;
    // '.' and ':' become a decimal-point dot after the previous cell.
    std::string normalize(const std::string& raw) const {
        std::string out;
        int cells = 0;
        for (char rc : raw) {
            char c = rc;
            if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
            if (c == '.' || c == ':') {
                if (cells > 0 && (out.empty() || out.back() != '.')) out += '.';
                continue;
            }
            if (cells >= kDigits) break;
            auto it = segment_map().find(c);
            if (it == segment_map().end()) continue;
            out += c;
            ++cells;
        }
        return out;
    }
    // Display cells (characters excluding '.' dots).
    std::vector<char> cells() const {
        std::vector<char> v;
        for (char c : value_) if (c != '.') v.push_back(c);
        return v;
    }
    // Decimal-point dot after cell i?
    bool dp_after(int i) const {
        int cell = -1;
        for (char c : value_) {
            if (c == '.') {
                if (cell == i) return true;
            } else {
                ++cell;
            }
        }
        return false;
    }
    void draw_diag(int x1, int y1, int x2, int y2, int t, unsigned long col) {
        double dx = (double)(x2 - x1), dy = (double)(y2 - y1);
        double len = std::sqrt(dx * dx + dy * dy);
        if (len < 1.0) return;
        double px = -dy / len * t / 2.0, py = dx / len * t / 2.0;
        XPoint pts[4] = {
            {(short)std::lround(x1 + px), (short)std::lround(y1 + py)},
            {(short)std::lround(x2 + px), (short)std::lround(y2 + py)},
            {(short)std::lround(x2 - px), (short)std::lround(y2 - py)},
            {(short)std::lround(x1 - px), (short)std::lround(y1 - py)}};
        XSetForeground(display_, gc_, col);
        XFillPolygon(display_, window_, gc_, pts, 4, Convex, CoordModeOrigin);
    }
    void draw_segment(int x, int y, int w, int h, int t, const std::string& seg,
                      unsigned long lit, unsigned long dim) {
        unsigned long col = segment_on(cur_, seg) ? lit : dim;
        int midx = x + w / 2, midy = y + h / 2;
        int hh = std::max(1, h / 2 - t);
        int hw = std::max(1, w / 2 - t);
        if (seg == "a") { XSetForeground(display_, gc_, col); XFillRectangle(display_, window_, gc_, x + t, y, std::max(1, w - 2 * t), t); }
        else if (seg == "a1") { XSetForeground(display_, gc_, col); XFillRectangle(display_, window_, gc_, x + t, y, hw, t); }
        else if (seg == "a2") { XSetForeground(display_, gc_, col); XFillRectangle(display_, window_, gc_, midx, y, hw, t); }
        else if (seg == "b") { XSetForeground(display_, gc_, col); XFillRectangle(display_, window_, gc_, x + w - t, y + t, t, hh); }
        else if (seg == "c") { XSetForeground(display_, gc_, col); XFillRectangle(display_, window_, gc_, x + w - t, midy, t, hh); }
        else if (seg == "d") { XSetForeground(display_, gc_, col); XFillRectangle(display_, window_, gc_, x + t, y + h - t, std::max(1, w - 2 * t), t); }
        else if (seg == "d1") { XSetForeground(display_, gc_, col); XFillRectangle(display_, window_, gc_, x + t, y + h - t, hw, t); }
        else if (seg == "d2") { XSetForeground(display_, gc_, col); XFillRectangle(display_, window_, gc_, midx, y + h - t, hw, t); }
        else if (seg == "e") { XSetForeground(display_, gc_, col); XFillRectangle(display_, window_, gc_, x, midy, t, hh); }
        else if (seg == "f") { XSetForeground(display_, gc_, col); XFillRectangle(display_, window_, gc_, x, y + t, t, hh); }
        else if (seg == "g1") { XSetForeground(display_, gc_, col); XFillRectangle(display_, window_, gc_, x + t, midy - t / 2, hw, t); }
        else if (seg == "g2") { XSetForeground(display_, gc_, col); XFillRectangle(display_, window_, gc_, midx, midy - t / 2, hw, t); }
        else if (seg == "i") { XSetForeground(display_, gc_, col); XFillRectangle(display_, window_, gc_, midx - t / 2, y + t, t, std::max(1, h / 2 - 2 * t)); }
        else if (seg == "l") { XSetForeground(display_, gc_, col); XFillRectangle(display_, window_, gc_, midx - t / 2, midy + t, t, std::max(1, h / 2 - 2 * t)); }
        else if (seg == "h") draw_diag(x + t, y + t, midx, midy - t, t, col);
        else if (seg == "j") draw_diag(x + w - t, y + t, midx, midy - t, t, col);
        else if (seg == "k") draw_diag(x + t, y + h - t, midx, midy + t, t, col);
        else if (seg == "m") draw_diag(x + w - t, y + h - t, midx, midy + t, t, col);
    }
    void draw_digit(int x, int y, int w, int h, int t, char ch,
                    unsigned long lit, unsigned long dim) {
        cur_ = ch;
        for (const std::string& seg : kSegIds) draw_segment(x, y, w, h, t, seg, lit, dim);
    }
    char cur_ = ' ';

public:
    static constexpr int kDigits = 3;
    static constexpr int kSegments = 14;
    static const std::vector<std::string> kSegIds;
    std::function<void()> on_click;

    InstSeg14Display3(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstSeg14Display3"; text_ = "InstSeg14Display3";
    }
    ~InstSeg14Display3() {}
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
    SStyle style() { return styles_->resolve(style_name_); }

    void set_value(const std::string& v) { value_ = normalize(v); }
    std::string get_value() const { return value_; }
    // The string actually shown, padded with spaces to kDigits cells.
    std::string shown_text() const {
        std::string s = value_;
        int n = 0;
        for (char c : s) if (c != '.') ++n;
        while (n < kDigits) { s += ' '; ++n; }
        return s;
    }
    int cell_count() const {
        int n = 0;
        for (char c : value_) if (c != '.') ++n;
        return n;
    }
    char char_at(int i) const {
        std::vector<char> v = cells();
        if (i < 0 || i >= (int)v.size()) return ' ';
        return v[i];
    }
    static bool segment_on(char ch, const std::string& seg) {
        if (ch >= 'a' && ch <= 'z') ch = (char)(ch - 'a' + 'A');
        auto it = segment_map().find(ch);
        if (it == segment_map().end()) return false;
        const std::string& s = it->second;
        size_t pos = 0;
        while (pos < s.size()) {
            size_t sp = s.find(' ', pos);
            std::string tok = s.substr(pos, sp == std::string::npos ? sp : sp - pos);
            if (tok == seg) return true;
            if (sp == std::string::npos) break;
            pos = sp + 1;
        }
        return false;
    }
    static bool renders_clean(char ch) {
        return std::string("0123456789ABCDEFGHIJLOPSTUX").find(ch) != std::string::npos;
    }
    static bool renders_approx(char ch) {
        return std::string("KMNQRVWYZ'\":").find(ch) != std::string::npos;
    }
    static bool renders_blank(char ch) {
        return std::string(";,?!@#$%&*()[]{}<>\\|^`~").find(ch) != std::string::npos;
    }
    static std::string clean_repertoire() { return "0123456789ABCDEFGHIJLOPSTUX"; }
    static std::string approx_repertoire() { return "KMNQRVWYZ'\":"; }
    static std::string cannot_repertoire() { return ";,?!@#$%&*()[]{}<>\\|^`~"; }
    static const char* default_value() { return "ABC"; }

    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long bg = styles_->get_color_rgb(12, 12, 14);
        unsigned long bezel = styles_->get_color_rgb(58, 58, 62);
        unsigned long lit = styles_->get_color_rgb(255, 176, 0);
        unsigned long dim = styles_->get_color_rgb(60, 42, 8);
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
            bool has_dp = value_.find('.') != std::string::npos;
            int dp_w = has_dp ? 8 : 0;
            int cell_w = (aw - gap * (kDigits - 1) - dp_w) / kDigits;
            int cell_h = ah;
            int t = std::max(2, cell_w / 6);
            std::vector<char> v = cells();
            int cx = ax;
            for (int i = 0; i < kDigits; ++i) {
                char ch = (i < (int)v.size()) ? v[i] : ' ';
                draw_digit(cx, ay, cell_w, cell_h, t, ch, lit, dim);
                cx += cell_w + gap;
                if (dp_after(i)) {
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
    // Up/Down step the last displayed character (digit wraps 9->0, letter
    // wraps Z->A); Home restores the default demo string.
    bool on_key(KeySym key, const char* str) {
        (void)str;
        if (!focused_ || !enabled_) return false;
        switch (key) {
        case XK_Up: case XK_Right: {
            std::vector<char> v = cells();
            if (v.empty()) return false;
            char c = v.back();
            if (c >= '0' && c <= '9') c = (char)('0' + (c - '0' + 1) % 10);
            else if (c >= 'A' && c <= 'Z') c = (char)('A' + (c - 'A' + 1) % 26);
            else return false;
            v.back() = c;
            std::string s;
            for (char ch : v) s += ch;
            set_value(s);
            return true;
        }
        case XK_Down: case XK_Left: {
            std::vector<char> v = cells();
            if (v.empty()) return false;
            char c = v.back();
            if (c >= '0' && c <= '9') c = (char)('0' + (c - '0' + 9) % 10);
            else if (c >= 'A' && c <= 'Z') c = (char)('A' + (c - 'A' + 25) % 26);
            else return false;
            v.back() = c;
            std::string s;
            for (char ch : v) s += ch;
            set_value(s);
            return true;
        }
        case XK_Home: set_value(default_value()); return true;
        case XK_Return: case XK_space:
            if (on_click) on_click();
            return true;
        }
        return false;
    }
};

const std::vector<std::string> InstSeg14Display3::kSegIds = {"a", "b", "c", "d", "e", "f", "g1", "g2", "h", "i", "j", "k", "l", "m"};
