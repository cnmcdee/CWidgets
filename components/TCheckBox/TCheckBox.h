#pragma once

#include "core/xframe_core.h"

//region TCheckBox

class TCheckBox

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

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    int state_ = 1;   // 0 = unchecked, 1 = checked, 2 = grayed

    bool allow_grayed_ = false;

    int box_size_ = 14;

    void fire_change()

    {

        if (on_change)

        {

            on_change();

        }

    }

public:

    std::function<void()> on_change;

    std::function<void()> on_click;

    TCheckBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TCheckBox";

        text_ = "Check me";

        state_ = 1;

    }

    ~TCheckBox() {}

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

    int get_state()

    {

        return state_;

    }

    void set_state(int state)

    {

        if (state < 0)

        {

            state = 0;

        }

        if (state > 2)

        {

            state = 2;

        }

        if (state == 2 && !allow_grayed_)

        {

            state = 1;

        }

        state_ = state;

    }

    bool get_checked()

    {

        return state_ == 1;

    }

    void set_checked(bool checked)

    {

        state_ = checked ? 1 : 0;

    }

    void set_allow_grayed(bool allow)

    {

        allow_grayed_ = allow;

        if (!allow_grayed_ && state_ == 2)

        {

            state_ = 1;

        }

    }

    bool get_allow_grayed()

    {

        return allow_grayed_;

    }

    // One click step: 0 -> 1 -> (2 if grayed is allowed) -> 0.

    void cycle()

    {

        if (state_ == 0)

        {

            state_ = 1;

        }

        else if (state_ == 1 && allow_grayed_)

        {

            state_ = 2;

        }

        else

        {

            state_ = 0;

        }

        fire_change();

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        int cy = (y1_ + y2_) / 2;

        int box_x = x1_ + 2;

        int box_y = cy - box_size_ / 2;

        // The little box itself: white, grey when grayed, outlined.

        if (state_ == 2)

        {

            XSetForeground(display_, gc_, styles_->get_color_rgb(200, 200, 200));

        }

        else

        {

            XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        }

        XFillRectangle(display_, window_, gc_, box_x, box_y, box_size_, box_size_);

        XSetForeground(display_, gc_, enabled_ ? st.lineColor_ : styles_->get_color_rgb(150, 150, 150));

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, box_x, box_y, box_size_, box_size_);

        if (state_ == 1)

        {

            xf::draw_checkmark(display_, window_, gc_, st.text_color_, box_x + box_size_ / 2, cy - 1, 12);

        }

        if (state_ == 2)

        {

            xf::draw_checkmark(display_, window_, gc_, styles_->get_color_rgb(130, 130, 130), box_x + box_size_ / 2, cy - 1, 12);

        }

        if (!enabled_ || state_ == 2)

        {

            st.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        xf::draw_text_left(display_, window_, gc_, st, box_x + box_size_ + 6, y1_, y2_, text_);

        if (focused_)

        {

            int text_w = xf::text_width(st, text_);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, box_x + box_size_ + 4, y1_ + 2, text_w + 4, height() - 4);

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

        cycle();

        if (on_click)

        {

            on_click();

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        (void)button;

        return false;

    }

    bool on_motion(int x, int y)

    {

        (void)x;

        (void)y;

        return false;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Return || key == XK_space)

        {

            cycle();

            if (on_click)

            {

                on_click();

            }

            return true;

        }

        return false;

    }

};

//endregion
