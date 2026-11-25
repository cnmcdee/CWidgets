#pragma once

#include "core/xframe_core.h"

//region TColorListBox

class TColorListBox

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

    int top_index_ = 0;

    int row_height_ = 16;

    int visible_rows()

    {

        int n = (height() - 4) / row_height_;

        return n < 1 ? 1 : n;

    }

    void ensure_visible()

    {

        if (selected_ < top_index_) top_index_ = selected_;

        if (selected_ >= top_index_ + visible_rows()) top_index_ = selected_ - visible_rows() + 1;

        if (top_index_ < 0) top_index_ = 0;

    }

    void select_index(int idx, bool fire)

    {

        if (idx < 0 || idx >= (int)items_.size()) return;

        if (idx != selected_)

        {

            selected_ = idx;

            ensure_visible();

            if (fire && on_change) on_change();

        }

        else

        {

            ensure_visible();

        }

    }

public:

    std::function<void()> on_change;

    std::function<void()> on_click;

    TColorListBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TColorListBox";

        text_ = "TColorListBox";

        items_ = {
            {"clBlack", {0, 0, 0}}, {"clMaroon", {128, 0, 0}}, {"clGreen", {0, 128, 0}},
            {"clOlive", {128, 128, 0}}, {"clNavy", {0, 0, 128}}, {"clPurple", {128, 0, 128}},
            {"clTeal", {0, 128, 128}}, {"clGray", {128, 128, 128}}, {"clSilver", {192, 192, 192}},
            {"clRed", {255, 0, 0}}, {"clLime", {0, 255, 0}}, {"clYellow", {255, 255, 0}},
            {"clBlue", {0, 0, 255}}, {"clFuchsia", {255, 0, 255}}, {"clAqua", {0, 255, 255}},
            {"clWhite", {255, 255, 255}}
        };

        selected_ = 12;

        ensure_visible();

    }

    ~TColorListBox() {}

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

    void draw()

    {

        if (!visible_) return;

        SStyle st = style();

        // White inset list area with a plain frame.

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        for (int i = 0; i < visible_rows(); ++i)

        {

            int idx = top_index_ + i;

            if (idx >= (int)items_.size()) break;

            int ry1 = y1_ + 2 + i * row_height_;

            int ry2 = ry1 + row_height_;

            if (idx == selected_)

            {

                XSetForeground(display_, gc_, styles_->get_color_rgb(51, 153, 255));

                XFillRectangle(display_, window_, gc_, x1_ + 2, ry1, width() - 4, row_height_);

            }

            XSetForeground(display_, gc_, styles_->get_color_Srgb(items_[idx].rgb));

            XFillRectangle(display_, window_, gc_, x1_ + 6, ry1 + 3, 18, row_height_ - 6);

            XSetForeground(display_, gc_, styles_->get_color_rgb(60, 60, 60));

            XDrawRectangle(display_, window_, gc_, x1_ + 6, ry1 + 3, 18, row_height_ - 6);

            SStyle tst = st;

            if (idx == selected_) tst.text_color_ = styles_->get_color_rgb(255, 255, 255);

            if (!enabled_) tst.text_color_ = styles_->get_color_rgb(140, 140, 140);

            xf::draw_text_left(display_, window_, gc_, tst, x1_ + 30, ry1, ry2, items_[idx].name);

        }

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, (int)st.lineWidth_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        if (focused_)

        {

            XDrawRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 5, height() - 5);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_) return false;

        focused_ = true;

        if (button == 4)

        {

            if (top_index_ > 0) --top_index_;

            return true;

        }

        if (button == 5)

        {

            if (top_index_ + visible_rows() < (int)items_.size()) ++top_index_;

            return true;

        }

        if (button != 1) return false;

        int idx = top_index_ + (y - y1_ - 2) / row_height_;

        if (idx >= 0 && idx < (int)items_.size()) select_index(idx, true);

        if (on_click) on_click();

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x; (void)y; (void)button;

        return false;

    }

    bool on_motion(int x, int y)

    {

        (void)x; (void)y;

        return false;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_) return false;

        if (key == XK_Down) { select_index(std::min((int)items_.size() - 1, selected_ + 1), true); return true; }

        if (key == XK_Up) { select_index(std::max(0, selected_ - 1), true); return true; }

        if (key == XK_Page_Down) { select_index(std::min((int)items_.size() - 1, selected_ + visible_rows()), true); return true; }

        if (key == XK_Page_Up) { select_index(std::max(0, selected_ - visible_rows()), true); return true; }

        if (key == XK_Home) { select_index(0, true); return true; }

        if (key == XK_End) { select_index((int)items_.size() - 1, true); return true; }

        return false;

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

        select_index(idx, false);

    }

    void set_selected_color_by_name(std::string n)

    {

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            if (items_[i].name == n) { select_index(i, false); return; }

        }

    }

    int get_selected_index() { return selected_; }

    int color_count() { return (int)items_.size(); }

    int get_top_index() { return top_index_; }

};

//endregion
