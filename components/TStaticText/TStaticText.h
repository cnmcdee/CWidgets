#pragma once

#include "core/xframe_core.h"

//region TStaticBorderStyle

enum TStaticBorderStyle

{

    sbsNone,

    sbsSingle,

    sbsSunken

};

//endregion

//region TStaticText

class TStaticText

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

    TStaticBorderStyle border_style_ = sbsSunken;

    int alignment_ = 0;

    std::string focus_control_;

    bool show_accel_char_ = true;

public:

    std::function<void()> on_click;

    TStaticText(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TStaticText";

        text_ = "Static Text";

        focus_control_ = "";

    }

    ~TStaticText() {}

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

    void set_border_style(TStaticBorderStyle b)

    {

        border_style_ = b;

    }

    TStaticBorderStyle get_border_style()

    {

        return border_style_;

    }

    void set_alignment(int a)

    {

        alignment_ = a;

    }

    int get_alignment()

    {

        return alignment_;

    }

    void set_focus_control(std::string name)

    {

        focus_control_ = name;

    }

    std::string get_focus_control()

    {

        return focus_control_;

    }

    void set_show_accel_char(bool s)

    {

        show_accel_char_ = s;

    }

    bool get_show_accel_char()

    {

        return show_accel_char_;

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

        int tx1 = x1_;

        int ty1 = y1_;

        int tx2 = x2_;

        int ty2 = y2_;

        if (border_style_ == sbsSingle)

        {

            XSetForeground(display_, gc_, st.fillColor_);

            XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, (int)st.lineWidth_);

            XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

            tx1 = x1_ + 4;

            tx2 = x2_ - 4;

        }

        else if (border_style_ == sbsSunken)

        {

            XSetForeground(display_, gc_, st.fillColor_);

            XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

            unsigned long light = styles_->get_color_rgb(255, 255, 255);

            unsigned long dark = styles_->get_color_rgb(90, 90, 90);

            unsigned long mid_light = styles_->get_color_rgb(210, 210, 210);

            unsigned long mid_dark = styles_->get_color_rgb(150, 150, 150);

            // Sunken: dark outside top/left, light outside
            // bottom/right, reversed on the inner line.

            XSetForeground(display_, gc_, dark);

            XDrawLine(display_, window_, gc_, x1_, y1_, x2_ - 1, y1_);

            XDrawLine(display_, window_, gc_, x1_, y1_, x1_, y2_ - 1);

            XSetForeground(display_, gc_, light);

            XDrawLine(display_, window_, gc_, x1_, y2_ - 1, x2_ - 1, y2_ - 1);

            XDrawLine(display_, window_, gc_, x2_ - 1, y1_, x2_ - 1, y2_ - 1);

            XSetForeground(display_, gc_, mid_dark);

            XDrawLine(display_, window_, gc_, x1_ + 1, y1_ + 1, x2_ - 2, y1_ + 1);

            XDrawLine(display_, window_, gc_, x1_ + 1, y1_ + 1, x1_ + 1, y2_ - 2);

            XSetForeground(display_, gc_, mid_light);

            XDrawLine(display_, window_, gc_, x1_ + 1, y2_ - 2, x2_ - 2, y2_ - 2);

            XDrawLine(display_, window_, gc_, x2_ - 2, y1_ + 1, x2_ - 2, y2_ - 2);

            tx1 = x1_ + 5;

            tx2 = x2_ - 5;

            ty1 = y1_ + 2;

            ty2 = y2_ - 2;

        }

        if (alignment_ == 1)

        {

            xf::draw_text_centered(display_, window_, gc_, st, tx1, ty1, tx2, ty2, text_);

        }

        else if (alignment_ == 2)

        {

            if (st.font_)

            {

                int tw = xf::text_width(st, text_);

                xf::draw_text_left(display_, window_, gc_, st, tx2 - tw, ty1, ty2, text_);

            }

        }

        else

        {

            xf::draw_text_left(display_, window_, gc_, st, tx1, ty1, ty2, text_);

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

        if (!contains(x, y) || button != 1)

        {

            return false;

        }

        pressed_ = true;

        focused_ = true;

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (!pressed_ || button != 1)

        {

            return false;

        }

        pressed_ = false;

        if (contains(x, y) && enabled_ && on_click)

        {

            on_click();

        }

        return true;

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
