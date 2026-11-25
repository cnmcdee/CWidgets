#pragma once

#include "core/xframe_core.h"

//region TBevel

enum TBevelStyle

{

    bsLowered,

    bsRaised

};

enum TBevelShape

{

    bsBox,

    bsFrame,

    bsTopLine,

    bsBottomLine,

    bsLeftLine,

    bsRightLine,

    bsSpacer

};

class TBevel

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

    TBevelStyle bevel_style_ = bsLowered;

    TBevelShape bevel_shape_ = bsBox;

    // One bevelled edge pair: for lowered the top / left edges take
    // the shadow and the bottom / right edges the highlight; raised
    // swaps the two colours.

    void draw_bevel_rect(int bx1, int by1, int bx2, int by2)

    {

        unsigned long highlight = styles_->get_color_rgb(240, 240, 240);

        unsigned long shadow = styles_->get_color_rgb(105, 105, 105);

        unsigned long top_left = (bevel_style_ == bsLowered) ? shadow : highlight;

        unsigned long bottom_right = (bevel_style_ == bsLowered) ? highlight : shadow;

        xf::set_line_width(display_, gc_, 1);

        XSetForeground(display_, gc_, top_left);

        XDrawLine(display_, window_, gc_, bx1, by1, bx2 - 1, by1);

        XDrawLine(display_, window_, gc_, bx1, by1, bx1, by2 - 1);

        XSetForeground(display_, gc_, bottom_right);

        XDrawLine(display_, window_, gc_, bx1, by2 - 1, bx2 - 1, by2 - 1);

        XDrawLine(display_, window_, gc_, bx2 - 1, by1, bx2 - 1, by2 - 1);

    }

    // A single 2px bevel line along one edge of the bounds.

    void draw_bevel_line(bool horizontal, bool at_start)

    {

        unsigned long highlight = styles_->get_color_rgb(240, 240, 240);

        unsigned long shadow = styles_->get_color_rgb(105, 105, 105);

        unsigned long first = (bevel_style_ == bsLowered) ? shadow : highlight;

        unsigned long second = (bevel_style_ == bsLowered) ? highlight : shadow;

        xf::set_line_width(display_, gc_, 1);

        if (horizontal)

        {

            int y = at_start ? y1_ : y2_ - 2;

            XSetForeground(display_, gc_, first);

            XDrawLine(display_, window_, gc_, x1_, y, x2_ - 1, y);

            XSetForeground(display_, gc_, second);

            XDrawLine(display_, window_, gc_, x1_, y + 1, x2_ - 1, y + 1);

        }

        else

        {

            int x = at_start ? x1_ : x2_ - 2;

            XSetForeground(display_, gc_, first);

            XDrawLine(display_, window_, gc_, x, y1_, x, y2_ - 1);

            XSetForeground(display_, gc_, second);

            XDrawLine(display_, window_, gc_, x + 1, y1_, x + 1, y2_ - 1);

        }

    }

public:

    TBevel(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TBevel";

        text_ = "TBevel";

    }

    ~TBevel() {}

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

    void set_bevel_style(TBevelStyle s)

    {

        bevel_style_ = s;

    }

    TBevelStyle get_bevel_style()

    {

        return bevel_style_;

    }

    void set_bevel_shape(TBevelShape s)

    {

        bevel_shape_ = s;

    }

    TBevelShape get_bevel_shape()

    {

        return bevel_shape_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        if (bevel_shape_ == bsSpacer)

        {

            // A spacer occupies its bounds but draws nothing.

            return;

        }

        SStyle st = style();

        if (bevel_shape_ == bsBox)

        {

            XSetForeground(display_, gc_, st.fillColor_);

            XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

            draw_bevel_rect(x1_, y1_, x2_, y2_);

        }

        else if (bevel_shape_ == bsFrame)

        {

            // Bevel only on the frame band: an outer bevel, a gap
            // filled with the style fill, then an inner bevel.

            XSetForeground(display_, gc_, st.fillColor_);

            XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

            draw_bevel_rect(x1_, y1_, x2_, y2_);

            draw_bevel_rect(x1_ + 4, y1_ + 4, x2_ - 4, y2_ - 4);

        }

        else if (bevel_shape_ == bsTopLine)

        {

            draw_bevel_line(true, true);

        }

        else if (bevel_shape_ == bsBottomLine)

        {

            draw_bevel_line(true, false);

        }

        else if (bevel_shape_ == bsLeftLine)

        {

            draw_bevel_line(false, true);

        }

        else if (bevel_shape_ == bsRightLine)

        {

            draw_bevel_line(false, false);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        (void)button;

        return false;

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

        (void)key;

        (void)str;

        return false;

    }

};

//endregion
