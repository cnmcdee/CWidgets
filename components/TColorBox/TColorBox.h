#pragma once

#include "core/xframe_core.h"

//region TColorBox

class TColorBox

{

public:

    struct ColorItem

    {

        std::string name;

        Srgb rgb;

    };

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;

    std::string style_name_;

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    std::vector<ColorItem> items_;

    int selected_ = 12;   // clBlue

    bool dropped_ = false;

    int hover_ = -1;

    int row_height_ = 16;

    int list_rows()

    {

        return std::min((int)items_.size(), 8);

    }

    int list_y1() { return y2_ + 2; }

    int list_y2() { return list_y1() + list_rows() * row_height_ + 4; }

    bool in_list(int x, int y)

    {

        return dropped_ && x >= x1_ && x <= x2_ && y >= list_y1() && y <= list_y2();

    }

    void select_index(int idx, bool fire)

    {

        if (idx < 0 || idx >= (int)items_.size()) return;

        if (idx != selected_)

        {

            selected_ = idx;

            if (fire && on_change) on_change();

        }

    }

public:

    std::function<void()> on_change;

    std::function<void()> on_click;

    TColorBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

    {

        display_ = display;

        screen_ = screen;

        window_ = window;

        gc_ = gc;

        styles_ = styles;

        x1_ = x;

        y1_ = y;

        x2_ = x + width;

        y2_ = y + height;

        name_ = "TColorBox";

        text_ = "TColorBox";

        items_ = {
            {"clBlack", {0, 0, 0}}, {"clMaroon", {128, 0, 0}}, {"clGreen", {0, 128, 0}},
            {"clOlive", {128, 128, 0}}, {"clNavy", {0, 0, 128}}, {"clPurple", {128, 0, 128}},
            {"clTeal", {0, 128, 128}}, {"clGray", {128, 128, 128}}, {"clSilver", {192, 192, 192}},
            {"clRed", {255, 0, 0}}, {"clLime", {0, 255, 0}}, {"clYellow", {255, 255, 0}},
            {"clBlue", {0, 0, 255}}, {"clFuchsia", {255, 0, 255}}, {"clAqua", {0, 255, 255}},
            {"clWhite", {255, 255, 255}}
        };

        selected_ = 12;

        hover_ = selected_;

    }

    ~TColorBox() {}

    void set_bounds(int x, int y, int width, int height)

    {

        x1_ = x;

        y1_ = y;

        x2_ = x + width;

        y2_ = y + height;

    }

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

    bool is_dropped() { return dropped_; }

    void draw()

    {

        if (!visible_) return;

        SStyle st = style();

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        int btn_w = 20;

        // Colour swatch.

        if (selected_ >= 0 && selected_ < (int)items_.size())

        {

            XSetForeground(display_, gc_, styles_->get_color_Srgb(items_[selected_].rgb));

            XFillRectangle(display_, window_, gc_, x1_ + 6, y1_ + 5, 22, height() - 10);

            XSetForeground(display_, gc_, styles_->get_color_rgb(60, 60, 60));

            XDrawRectangle(display_, window_, gc_, x1_ + 6, y1_ + 5, 22, height() - 10);

            xf::draw_text_left(display_, window_, gc_, st, x1_ + 34, y1_, y2_, items_[selected_].name);

        }

        // Dropdown button zone.

        XSetForeground(display_, gc_, st.outlineColor_);

        XDrawLine(display_, window_, gc_, x2_ - btn_w, y1_ + 3, x2_ - btn_w, y2_ - 3);

        unsigned long tri = enabled_ ? st.text_color_ : styles_->get_color_rgb(140, 140, 140);

        xf::draw_triangle_down(display_, window_, gc_, tri, (x2_ - btn_w + x2_) / 2, (y1_ + y2_) / 2 - 3, 5);

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 3, y1_ + 3, width() - 6, height() - 6);

        }

        if (dropped_)

        {

            int ly1 = list_y1();

            int ly2 = list_y2();

            XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

            XFillRectangle(display_, window_, gc_, x1_, ly1, width(), ly2 - ly1);

            // Scroll so the selected row is visible in the popup.

            int first = std::max(0, std::min(selected_ - list_rows() + 1, (int)items_.size() - list_rows()));

            if (hover_ >= 0 && hover_ < first) first = hover_;

            if (hover_ >= first + list_rows()) first = hover_ - list_rows() + 1;

            for (int i = 0; i < list_rows(); ++i)

            {

                int idx = first + i;

                if (idx >= (int)items_.size()) break;

                int ry1 = ly1 + 2 + i * row_height_;

                int ry2 = ry1 + row_height_;

                if (idx == hover_ || idx == selected_)

                {

                    XSetForeground(display_, gc_, styles_->get_color_rgb(51, 153, 255));

                    XFillRectangle(display_, window_, gc_, x1_ + 2, ry1, width() - 4, row_height_);

                }

                XSetForeground(display_, gc_, styles_->get_color_Srgb(items_[idx].rgb));

                XFillRectangle(display_, window_, gc_, x1_ + 6, ry1 + 3, 18, row_height_ - 6);

                XSetForeground(display_, gc_, styles_->get_color_rgb(60, 60, 60));

                XDrawRectangle(display_, window_, gc_, x1_ + 6, ry1 + 3, 18, row_height_ - 6);

                SStyle tst = st;

                if (idx == hover_ || idx == selected_) tst.text_color_ = styles_->get_color_rgb(255, 255, 255);

                xf::draw_text_left(display_, window_, gc_, tst, x1_ + 30, ry1, ry2, items_[idx].name);

            }

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_, ly1, width() - 1, ly2 - ly1 - 1);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!enabled_ || button != 1) return false;

        if (dropped_ && in_list(x, y))

        {

            int first = std::max(0, std::min(selected_ - list_rows() + 1, (int)items_.size() - list_rows()));

            if (hover_ >= 0 && hover_ < first) first = hover_;

            if (hover_ >= first + list_rows()) first = hover_ - list_rows() + 1;

            int idx = first + (y - list_y1() - 2) / row_height_;

            if (idx >= 0 && idx < (int)items_.size())

            {

                select_index(idx, true);

            }

            dropped_ = false;

            focused_ = true;

            return true;

        }

        if (contains(x, y))

        {

            focused_ = true;

            dropped_ = !dropped_;

            hover_ = selected_;

            return true;

        }

        if (dropped_)

        {

            dropped_ = false;

            return true;

        }

        return false;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x; (void)y; (void)button;

        return false;

    }

    bool on_motion(int x, int y)

    {

        if (!dropped_ || !in_list(x, y)) return false;

        int first = std::max(0, std::min(selected_ - list_rows() + 1, (int)items_.size() - list_rows()));

        if (hover_ >= 0 && hover_ < first) first = hover_;

        if (hover_ >= first + list_rows()) first = hover_ - list_rows() + 1;

        int idx = first + (y - list_y1() - 2) / row_height_;

        if (idx >= 0 && idx < (int)items_.size() && idx != hover_)

        {

            hover_ = idx;

            return true;

        }

        return false;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_) return false;

        if (key == XK_Down)

        {

            if (dropped_) { hover_ = std::min((int)items_.size() - 1, hover_ + 1); }

            else { select_index(std::min((int)items_.size() - 1, selected_ + 1), true); hover_ = selected_; }

            return true;

        }

        if (key == XK_Up)

        {

            if (dropped_) { hover_ = std::max(0, hover_ - 1); }

            else { select_index(std::max(0, selected_ - 1), true); hover_ = selected_; }

            return true;

        }

        if (key == XK_Return || key == XK_KP_Enter || key == XK_space)

        {

            if (dropped_) { select_index(hover_, true); dropped_ = false; }

            else { dropped_ = true; hover_ = selected_; }

            return true;

        }

        if (key == XK_Escape)

        {

            dropped_ = false;

            hover_ = selected_;

            return true;

        }

        return false;

    }

    void set_selected_color_by_name(std::string n)

    {

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            if (items_[i].name == n) { selected_ = i; hover_ = i; return; }

        }

    }

    std::string get_selected_name()

    {

        if (selected_ >= 0 && selected_ < (int)items_.size()) return items_[selected_].name;

        return "";

    }

    Srgb get_selected_rgb()

    {

        if (selected_ >= 0 && selected_ < (int)items_.size()) return items_[selected_].rgb;

        return Srgb{0, 0, 0};

    }

    unsigned long get_selected_color()

    {

        return styles_->get_color_Srgb(get_selected_rgb());

    }

    void set_selected_index(int idx)

    {

        if (idx >= 0 && idx < (int)items_.size()) { selected_ = idx; hover_ = idx; }

    }

    int get_selected_index() { return selected_; }

    int color_count() { return (int)items_.size(); }

};

//endregion
