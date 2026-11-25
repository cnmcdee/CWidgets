#pragma once

#include "core/xframe_core.h"

//region TPairSplitter

class TPairSplitter

{

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

    std::string text2_;

    int position_ = 0;

    bool position_set_ = false;

    bool vertical_ = true;

    int min_size_ = 20;

    int bar_thickness_ = 6;

    bool dragging_ = false;

    int drag_start_position_ = 0;

    int effective_position()

    {

        if (!position_set_)

        {

            return vertical_ ? width() / 2 : height() / 2;

        }

        return position_;

    }

    int clamp_position(int pos)

    {

        int total = vertical_ ? width() : height();

        int max_pos = total - bar_thickness_ - min_size_;

        if (max_pos < min_size_)

        {

            max_pos = min_size_;

        }

        if (pos < min_size_)

        {

            return min_size_;

        }

        if (pos > max_pos)

        {

            return max_pos;

        }

        return pos;

    }

    bool in_bar(int x, int y)

    {

        int pos = effective_position();

        if (vertical_)

        {

            return x >= x1_ + pos && x <= x1_ + pos + bar_thickness_ && y >= y1_ && y <= y2_;

        }

        return y >= y1_ + pos && y <= y1_ + pos + bar_thickness_ && x >= x1_ && x <= x2_;

    }

public:

    std::function<void()> on_moved;

    TPairSplitter(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TPairSplitter";

        text_ = "Side 1";

        text2_ = "Side 2";

        position_ = width / 2;

        position_set_ = true;

    }

    ~TPairSplitter() {}

    void set_bounds(int x, int y, int width, int height)

    {

        x1_ = x;

        y1_ = y;

        x2_ = x + width;

        y2_ = y + height;

        position_ = clamp_position(position_);

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

    bool side_rect(int side, int& sx1, int& sy1, int& sx2, int& sy2)

    {

        int pos = effective_position();

        if (side == 1)

        {

            sx1 = x1_;

            sy1 = y1_;

            sx2 = vertical_ ? x1_ + pos : x2_;

            sy2 = vertical_ ? y2_ : y1_ + pos;

            return true;

        }

        if (side == 2)

        {

            sx1 = vertical_ ? x1_ + pos + bar_thickness_ : x1_;

            sy1 = vertical_ ? y1_ : y1_ + pos + bar_thickness_;

            sx2 = x2_;

            sy2 = y2_;

            return true;

        }

        return false;

    }

    void draw_pane(int px1, int py1, int px2, int py2, std::string caption, const SStyle& st)

    {

        unsigned long fill = styles_->get_color_rgb(250, 250, 250);

        XSetForeground(display_, gc_, fill);

        XFillRectangle(display_, window_, gc_, px1, py1, px2 - px1, py2 - py1);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, px1, py1, px2 - px1, py2 - py1);

        xf::draw_text_left(display_, window_, gc_, st, px1 + 6, py1, py1 + 20, caption);

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        if (!enabled_)

        {

            st.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        int ax1 = 0;

        int ay1 = 0;

        int ax2 = 0;

        int ay2 = 0;

        side_rect(1, ax1, ay1, ax2, ay2);

        draw_pane(ax1, ay1, ax2, ay2, text_, st);

        side_rect(2, ax1, ay1, ax2, ay2);

        draw_pane(ax1, ay1, ax2, ay2, text2_, st);

        // Splitter bar between the panes, with grip dots.

        int pos = effective_position();

        int bx1 = vertical_ ? x1_ + pos : x1_;

        int by1 = vertical_ ? y1_ : y1_ + pos;

        int bx2 = vertical_ ? bx1 + bar_thickness_ : x2_;

        int by2 = vertical_ ? y2_ : by1 + bar_thickness_;

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, bx1, by1, bx2 - bx1, by2 - by1);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, bx1, by1, bx2 - bx1, by2 - by1);

        unsigned long dot = styles_->get_color_rgb(110, 110, 110);

        XSetForeground(display_, gc_, dot);

        if (vertical_)

        {

            int cy = (by1 + by2) / 2;

            for (int i = -2; i <= 2; ++i)

            {

                XFillRectangle(display_, window_, gc_, bx1 + 2, cy + i * 5 - 1, 2, 2);

            }

        }

        else

        {

            int cx = (bx1 + bx2) / 2;

            for (int i = -2; i <= 2; ++i)

            {

                XFillRectangle(display_, window_, gc_, cx + i * 5 - 1, by1 + 2, 2, 2);

            }

        }

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 2, height() - 2);

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

        if (in_bar(x, y))

        {

            dragging_ = true;

            drag_start_position_ = effective_position();

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button != 1 || !dragging_)

        {

            return false;

        }

        dragging_ = false;

        if (effective_position() != drag_start_position_ && on_moved)

        {

            on_moved();

        }

        return true;

    }

    bool on_motion(int x, int y)

    {

        if (!dragging_)

        {

            return false;

        }

        int pos = vertical_ ? x - x1_ - bar_thickness_ / 2 : y - y1_ - bar_thickness_ / 2;

        position_ = clamp_position(pos);

        position_set_ = true;

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        int delta = 0;

        if (vertical_ && key == XK_Left)

        {

            delta = -4;

        }

        else if (vertical_ && key == XK_Right)

        {

            delta = 4;

        }

        else if (!vertical_ && key == XK_Up)

        {

            delta = -4;

        }

        else if (!vertical_ && key == XK_Down)

        {

            delta = 4;

        }

        if (delta != 0)

        {

            int old = effective_position();

            position_ = clamp_position(old + delta);

            position_set_ = true;

            if (position_ != old && on_moved)

            {

                on_moved();

            }

            return true;

        }

        return false;

    }

    //region TPairSplitter API

    void set_position(int pos)

    {

        position_ = clamp_position(pos);

        position_set_ = true;

    }

    int get_position()

    {

        return effective_position();

    }

    void set_vertical(bool v)

    {

        vertical_ = v;

        position_ = clamp_position(position_);

    }

    bool get_vertical()

    {

        return vertical_;

    }

    void set_min_size(int m)

    {

        min_size_ = m < 0 ? 0 : m;

        position_ = clamp_position(position_);

    }

    int get_min_size()

    {

        return min_size_;

    }

    void set_pane_text(int side, std::string t)

    {

        if (side == 1)

        {

            text_ = t;

        }

        else if (side == 2)

        {

            text2_ = t;

        }

    }

    std::string get_pane_text(int side)

    {

        if (side == 1)

        {

            return text_;

        }

        if (side == 2)

        {

            return text2_;

        }

        return "";

    }

    void set_text2(std::string t)

    {

        text2_ = t;

    }

    std::string get_text2()

    {

        return text2_;

    }

    //endregion

};

//endregion
