#pragma once

#include "core/xframe_core.h"

//region TRadioButton

class TRadioButton

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

    bool checked_ = false;

    std::vector<TRadioButton*> *group_ = nullptr;   // shared group, not owned

    int circle_size_ = 14;

    void uncheck_others()

    {

        if (!group_)

        {

            return;

        }

        for (size_t i = 0; i < group_->size(); i++)

        {

            TRadioButton *other = (*group_)[i];

            if (other && other != this)

            {

                other->checked_ = false;

            }

        }

    }

public:

    std::function<void()> on_change;

    std::function<void()> on_click;

    TRadioButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TRadioButton";

        text_ = "Option";

        checked_ = true;   // a bare demo shows the selected look

    }

    ~TRadioButton() {}

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

    bool get_checked()

    {

        return checked_;

    }

    void set_checked(bool checked)

    {

        if (checked)

        {

            uncheck_others();

        }

        checked_ = checked;

    }

    // Join a shared group: checking any one member unchecks the rest.
    // The caller owns the vector and keeps it alive.

    void set_group(std::vector<TRadioButton*> *group)

    {

        group_ = group;

        if (group_)

        {

            bool found = false;

            for (size_t i = 0; i < group_->size(); i++)

            {

                if ((*group_)[i] == this)

                {

                    found = true;

                }

            }

            if (!found)

            {

                group_->push_back(this);

            }

            if (checked_)

            {

                uncheck_others();

            }

        }

    }

    std::vector<TRadioButton*> *get_group()

    {

        return group_;

    }

    // A radio button turns on by click, never off.

    void check()

    {

        if (checked_)

        {

            return;

        }

        uncheck_others();

        checked_ = true;

        if (on_change)

        {

            on_change();

        }

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        int cy = (y1_ + y2_) / 2;

        int circle_x = x1_ + 2;

        int circle_y = cy - circle_size_ / 2;

        // White disc with the style outline.

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillArc(display_, window_, gc_, circle_x, circle_y, circle_size_, circle_size_, 0, 360 * 64);

        XSetForeground(display_, gc_, enabled_ ? st.lineColor_ : styles_->get_color_rgb(150, 150, 150));

        xf::set_line_width(display_, gc_, 1);

        XDrawArc(display_, window_, gc_, circle_x, circle_y, circle_size_, circle_size_, 0, 360 * 64);

        if (checked_)

        {

            int dot = 6;

            int dot_x = circle_x + (circle_size_ - dot) / 2;

            int dot_y = circle_y + (circle_size_ - dot) / 2;

            XSetForeground(display_, gc_, enabled_ ? st.text_color_ : styles_->get_color_rgb(150, 150, 150));

            XFillArc(display_, window_, gc_, dot_x, dot_y, dot, dot, 0, 360 * 64);

        }

        if (!enabled_)

        {

            st.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        xf::draw_text_left(display_, window_, gc_, st, circle_x + circle_size_ + 6, y1_, y2_, text_);

        if (focused_)

        {

            int text_w = xf::text_width(st, text_);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, circle_x + circle_size_ + 4, y1_ + 2, text_w + 4, height() - 4);

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

        check();

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

            check();

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
