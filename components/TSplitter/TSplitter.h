#pragma once

#include "core/xframe_core.h"

//region TSplitter

class TSplitter

{

public:

    enum EAligned { alLeft, alRight, alTop, alBottom };

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

    EAligned aligned_ = alLeft;

    int min_size_ = 30;

    int resize_style_ = 1;

    int position_ = 120;

    bool dragging_ = false;

    int drag_start_x_ = 0;

    int drag_start_y_ = 0;

    int drag_start_pos_ = 0;

    int drag_start_position_ = 0;

    int drag_grab_offset_ = 0;

public:

    std::function<void(int)> on_moved;

    std::function<void()> on_click;

    TSplitter(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TSplitter";

        text_ = "TSplitter";

        position_ = x;

        drag_start_position_ = position_;

    }

    ~TSplitter() {}

    void set_bounds(int x, int y, int width, int height)

    {

        x1_ = x;

        y1_ = y;

        x2_ = x + width;

        y2_ = y + height;

    }

    int width()

    {

        return x2_ - x1_;

    }

    int height()

    {

        return y2_ - y1_;

    }

    bool contains(int x, int y)

    {

        return visible_ && x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_;

    }

    void set_visible(bool v)

    {

        visible_ = v;

    }

    bool get_visible()

    {

        return visible_;

    }

    void set_enabled(bool e)

    {

        enabled_ = e;

    }

    bool get_enabled()

    {

        return enabled_;

    }

    void set_style_name(std::string s)

    {

        style_name_ = s;

    }

    void set_text(std::string t)

    {

        text_ = t;

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region Specific API

    void set_aligned(EAligned a)

    {

        aligned_ = a;

    }

    EAligned get_aligned()

    {

        return aligned_;

    }

    int get_position()

    {

        return position_;

    }

    void set_position(int p)

    {

        if (p < min_size_)

        {

            p = min_size_;

        }

        position_ = p;

    }

    void set_min_size(int m)

    {

        min_size_ = m;

        if (position_ < min_size_)

        {

            position_ = min_size_;

        }

    }

    int get_min_size()

    {

        return min_size_;

    }

    void set_resize_style(int s)

    {

        resize_style_ = s;

    }

    int get_resize_style()

    {

        return resize_style_;

    }

    bool is_vertical()

    {

        return aligned_ == alLeft || aligned_ == alRight;

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        unsigned long light = styles_->get_color_rgb(245, 245, 245);

        unsigned long dark = styles_->get_color_rgb(105, 105, 105);

        unsigned long dot = enabled_ ? st.lineColor_ : styles_->get_color_rgb(150, 150, 150);

        xf::set_line_width(display_, gc_, 1);

        if (is_vertical())

        {

            XSetForeground(display_, gc_, light);

            XDrawLine(display_, window_, gc_, x1_, y1_, x1_, y2_);

            XSetForeground(display_, gc_, dark);

            XDrawLine(display_, window_, gc_, x2_ - 1, y1_, x2_ - 1, y2_);

            int cx = (x1_ + x2_) / 2;

            int cy = (y1_ + y2_) / 2;

            XSetForeground(display_, gc_, dot);

            for (int i = -2; i <= 2; ++i)

            {

                XFillRectangle(display_, window_, gc_, cx - 1, cy + i * 6 - 1, 3, 3);

            }

        }

        else

        {

            XSetForeground(display_, gc_, light);

            XDrawLine(display_, window_, gc_, x1_, y1_, x2_, y1_);

            XSetForeground(display_, gc_, dark);

            XDrawLine(display_, window_, gc_, x1_, y2_ - 1, x2_, y2_ - 1);

            int cx = (x1_ + x2_) / 2;

            int cy = (y1_ + y2_) / 2;

            XSetForeground(display_, gc_, dot);

            for (int i = -2; i <= 2; ++i)

            {

                XFillRectangle(display_, window_, gc_, cx + i * 6 - 1, cy - 1, 3, 3);

            }

        }

        if (dragging_)

        {

            XSetForeground(display_, gc_, st.outlineColor_);

            XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        dragging_ = true;

        drag_start_x_ = x;

        drag_start_y_ = y;

        drag_start_pos_ = is_vertical() ? x : y;

        drag_start_position_ = position_;

        drag_grab_offset_ = is_vertical() ? (x - x1_) : (y - y1_);

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (!dragging_ || button != 1)

        {

            return false;

        }

        dragging_ = false;

        if (position_ < min_size_)

        {

            position_ = min_size_;

        }

        if (on_moved)

        {

            on_moved(position_);

        }

        return true;

    }

    bool on_motion(int x, int y)

    {

        if (!dragging_)

        {

            return false;

        }

        if (is_vertical())

        {

            int delta = x - drag_start_x_;

            position_ = drag_start_position_ + delta;

            (void)drag_start_pos_;

            (void)drag_grab_offset_;

        }

        else

        {

            int delta = y - drag_start_y_;

            position_ = drag_start_position_ + delta;

        }

        if (position_ < min_size_)

        {

            position_ = min_size_;

        }

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        int step = 4;

        bool changed = false;

        if (is_vertical() && key == XK_Left)

        {

            position_ -= step;

            changed = true;

        }

        else if (is_vertical() && key == XK_Right)

        {

            position_ += step;

            changed = true;

        }

        else if (!is_vertical() && key == XK_Up)

        {

            position_ -= step;

            changed = true;

        }

        else if (!is_vertical() && key == XK_Down)

        {

            position_ += step;

            changed = true;

        }

        if (changed)

        {

            if (position_ < min_size_)

            {

                position_ = min_size_;

            }

            if (on_moved)

            {

                on_moved(position_);

            }

            return true;

        }

        return false;

    }

};

//endregion
