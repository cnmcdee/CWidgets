#pragma once

#include "core/xframe_core.h"
#include "../InstData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

// InstControlPanel - grid container: title strip, screws, and labelled cells other widgets sit on. Click a cell to select it.
// Attribution: XFrame.
class InstControlPanel {
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
    int rows_ = 3;
    int cols_ = 2;
    std::vector<std::string> labels_;
    int sel_row_ = -1;
    int sel_col_ = -1;

public:
    InstControlPanel(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "InstControlPanel"; text_ = "MAIN PANEL";

    }
    ~InstControlPanel() {}
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
    void set_grid(int rows, int cols) {
        if (rows < 1 || cols < 1) return;
        rows_ = rows; cols_ = cols;
        labels_.assign((size_t)(rows * cols), "");
    }
    int rows() const { return rows_; }
    int cols() const { return cols_; }
    void place_cell(int r, int c, const std::string& label) {
        if (r < 0 || r >= rows_ || c < 0 || c >= cols_) return;
        if (labels_.size() != (size_t)(rows_ * cols_)) labels_.assign((size_t)(rows_ * cols_), "");
        labels_[(size_t)(r * cols_ + c)] = label;
    }
    std::string cell_label(int r, int c) const {
        if (r < 0 || r >= rows_ || c < 0 || c >= cols_) return "";
        if (labels_.size() != (size_t)(rows_ * cols_)) return "";
        return labels_[(size_t)(r * cols_ + c)];
    }
    int selected_row() const { return sel_row_; }
    int selected_col() const { return sel_col_; }
    std::string selected_label() const { return cell_label(sel_row_, sel_col_); }
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
        int sx = x1_ + 10, sy = y1_ + 10, sw = width() - 20, sh = 30;
        XSetForeground(display_, gc_, amber);
        XFillRectangle(display_, window_, gc_, sx, sy, sw, sh);
        XSetForeground(display_, gc_, ink);
        XDrawRectangle(display_, window_, gc_, sx, sy, sw, sh);
        int tw = xf::text_width(st, text_);
        xf::draw_text_at(display_, window_, gc_, st, sx + (sw - tw) / 2, sy + 20, text_);
        if (labels_.size() != (size_t)(rows_ * cols_)) labels_.assign((size_t)(rows_ * cols_), "");
        int gx = x1_ + 16, gy = y1_ + 52, gw = width() - 32, gh = height() - 68;
        int cw = gw / cols_, ch = gh / rows_;
        for (int r = 0; r < rows_; ++r) {
            for (int c = 0; c < cols_; ++c) {
                int cx0 = gx + c * cw, cy0 = gy + r * ch;
                bool sel = (r == sel_row_ && c == sel_col_);
                XSetForeground(display_, gc_, sel ? amber : face);
                XFillRectangle(display_, window_, gc_, cx0 + 3, cy0 + 3, cw - 6, ch - 6);
                XSetForeground(display_, gc_, ink);
                XDrawRectangle(display_, window_, gc_, cx0 + 3, cy0 + 3, cw - 6, ch - 6);
                std::string lab = labels_[(size_t)(r * cols_ + c)];
                if (!lab.empty()) {
                    int lw = xf::text_width(st, lab);
                    xf::draw_text_at(display_, window_, gc_, st, cx0 + (cw - lw) / 2, cy0 + ch / 2 + 4, lab);
                }
            }
        }
        draw_screw(x1_ + 20, y1_ + 20);
        draw_screw(x2_ - 20, y1_ + 20);
        draw_screw(x1_ + 20, y2_ - 20);
        draw_screw(x2_ - 20, y2_ - 20);

        XSetForeground(display_, gc_, st.lineColor_);
        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        XFlush(display_);
    }
    bool on_button_press(int x, int y, unsigned int button) {
        if (!contains(x, y) || !enabled_) return false;
        (void)button;
        focused_ = true; pressed_ = true;
        int gx = x1_ + 16, gy = y1_ + 52, gw = width() - 32, gh = height() - 68;
        int cw = gw / cols_, ch = gh / rows_;
        if (x >= gx && y >= gy && cw > 0 && ch > 0) {
            sel_col_ = std::min(cols_ - 1, (x - gx) / cw);
            sel_row_ = std::min(rows_ - 1, (y - gy) / ch);
        }

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
