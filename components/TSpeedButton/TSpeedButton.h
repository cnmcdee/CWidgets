#pragma once

#include "core/xframe_core.h"

//region TSpeedButton

class TSpeedButton

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

    bool pressed_ = false;

    bool hover_ = false;

    bool down_ = false;

    int group_index_ = 0;

    bool allow_all_up_ = false;

    // OnClick timing (LCL behaviour, simplified): the click fires on
    // mouse-up over the button, after the down_ state has settled, so
    // a handler always reads the final get_down() value. Grouped
    // buttons toggle on release; momentary buttons never latch.

public:

    std::function<void()> on_click;

    TSpeedButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TSpeedButton";

        text_ = "Speed";

    }

    ~TSpeedButton() {}

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

    void set_group_index(int g)

    {

        group_index_ = g;

        if (g == 0)

        {

            down_ = false;

        }

    }

    int get_group_index()

    {

        return group_index_;

    }

    void set_allow_all_up(bool a)

    {

        allow_all_up_ = a;

    }

    bool get_allow_all_up()

    {

        return allow_all_up_;

    }

    bool get_down()

    {

        return down_;

    }

    void set_down(bool d)

    {

        if (group_index_ == 0)

        {

            down_ = false;

            return;

        }

        if (d == false && down_ && !allow_all_up_)

        {

            return;

        }

        down_ = d;

    }

    bool get_hover()

    {

        return hover_;

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

        bool show_body = hover_ || down_ || pressed_ || focused_;

        if (down_ || pressed_)

        {

            unsigned long swap = st.fillColor_;

            st.fillColor_ = st.outlineColor_;

            st.outlineColor_ = swap;

            xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        }

        else if (show_body)

        {

            xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        }

        else

        {

            // Flat idle state: no body, no border — just the caption
            // sitting on the parent surface, like an LCL toolbar.

            XSetForeground(display_, gc_, st.fillColor_);

            XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        }

        int off = (pressed_ || down_) ? 1 : 0;

        xf::draw_text_centered(display_, window_, gc_, st, x1_ + off, y1_ + off, x2_ + off, y2_ + off, text_);

        if (down_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, 2);

            XDrawLine(display_, window_, gc_, x1_ + 3, y2_ - 4, x2_ - 3, y2_ - 4);

            xf::set_line_width(display_, gc_, 1);

        }

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, height() - 4);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        pressed_ = true;

        focused_ = true;

        hover_ = true;

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (!pressed_ || button != 1)

        {

            return false;

        }

        pressed_ = false;

        bool inside = contains(x, y);

        if (inside)

        {

            if (group_index_ != 0)

            {

                if (!down_)

                {

                    down_ = true;

                }

                else if (allow_all_up_)

                {

                    down_ = false;

                }

            }

            if (on_click)

            {

                on_click();

            }

        }

        return true;

    }

    bool on_motion(int x, int y)

    {

        bool now_hover = contains(x, y);

        if (now_hover != hover_)

        {

            hover_ = now_hover;

            return true;

        }

        return hover_;

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

            if (group_index_ != 0)

            {

                if (!down_)

                {

                    down_ = true;

                }

                else if (allow_all_up_)

                {

                    down_ = false;

                }

            }

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
