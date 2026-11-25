#pragma once

#include "core/xframe_core.h"

//region TPanel

class TPanel

{

public:

    // Bevel kinds, with the Lazarus TBevelCut values.

    enum BevelKind

    {

        bvNone = 0,

        bvLowered = 1,

        bvRaised = 2,

        bvSpace = 3

    };

    // Caption alignment: 0 = left, 1 = centre, 2 = right.

    enum AlignmentKind

    {

        taLeftJustify = 0,

        taCenter = 1,

        taRightJustify = 2

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

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    int bevel_outer_ = bvRaised;

    int bevel_inner_ = bvNone;

    int bevel_width_ = 1;

    int alignment_ = taCenter;

    void draw_bevel(int bx1, int by1, int bx2, int by2, int bevel)

    {

        if (bevel == bvNone)

        {

            return;

        }

        SStyle st = style();

        if (bevel == bvSpace)

        {

            // Just a spacing frame line, in the style line colour.

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, 1);

            XDrawRectangle(display_, window_, gc_, bx1, by1, bx2 - bx1, by2 - by1);

            return;

        }

        unsigned long light = styles_->get_color_rgb(235, 235, 235);

        unsigned long dark = styles_->get_color_rgb(90, 90, 90);

        unsigned long top_left = light;

        unsigned long bottom_right = dark;

        if (bevel == bvLowered)

        {

            top_left = dark;

            bottom_right = light;

        }

        int w = bevel_width_;

        if (w < 1)

        {

            w = 1;

        }

        xf::set_line_width(display_, gc_, 1);

        for (int i = 0; i < w; ++i)

        {

            XSetForeground(display_, gc_, top_left);

            XDrawLine(display_, window_, gc_, bx1 + i, by1 + i, bx2 - i, by1 + i);

            XDrawLine(display_, window_, gc_, bx1 + i, by1 + i, bx1 + i, by2 - i);

            XSetForeground(display_, gc_, bottom_right);

            XDrawLine(display_, window_, gc_, bx1 + i, by2 - i, bx2 - i, by2 - i);

            XDrawLine(display_, window_, gc_, bx2 - i, by1 + i, bx2 - i, by2 - i);

        }

    }

    int bevel_inset(int bevel)

    {

        if (bevel == bvNone)

        {

            return 0;

        }

        if (bevel == bvSpace)

        {

            return 1;

        }

        int w = bevel_width_;

        if (w < 1)

        {

            w = 1;

        }

        return w;

    }

public:

    std::function<void()> on_click;

    TPanel(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TPanel";

        text_ = "Panel";

    }

    ~TPanel() {}

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

    //region Bevel and alignment

    void set_bevel_outer(int b)

    {

        bevel_outer_ = b;

    }

    int get_bevel_outer()

    {

        return bevel_outer_;

    }

    void set_bevel_inner(int b)

    {

        bevel_inner_ = b;

    }

    int get_bevel_inner()

    {

        return bevel_inner_;

    }

    void set_bevel_width(int w)

    {

        if (w < 1)

        {

            w = 1;

        }

        bevel_width_ = w;

    }

    int get_bevel_width()

    {

        return bevel_width_;

    }

    void set_alignment(int a)

    {

        alignment_ = a;

    }

    int get_alignment()

    {

        return alignment_;

    }

    // Inner area left for child content, inside both bevels.

    void client_rect(int &cx, int &cy, int &cw, int &ch)

    {

        int inset = bevel_inset(bevel_outer_);

        if (bevel_inner_ != bvNone)

        {

            inset += 2 + bevel_inset(bevel_inner_);

        }

        cx = x1_ + inset;

        cy = y1_ + inset;

        cw = width() - 2 * inset;

        ch = height() - 2 * inset;

        if (cw < 0)

        {

            cw = 0;

        }

        if (ch < 0)

        {

            ch = 0;

        }

    }

    //endregion

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

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        draw_bevel(x1_, y1_, x2_ - 1, y2_ - 1, bevel_outer_);

        if (bevel_inner_ != bvNone)

        {

            int gap = bevel_inset(bevel_outer_) + 2;

            draw_bevel(x1_ + gap, y1_ + gap, x2_ - 1 - gap, y2_ - 1 - gap, bevel_inner_);

        }

        if (!text_.empty())

        {

            if (alignment_ == taLeftJustify)

            {

                int cx = 0;

                int cy = 0;

                int cw = 0;

                int ch = 0;

                client_rect(cx, cy, cw, ch);

                xf::draw_text_left(display_, window_, gc_, st, cx + 4, y1_, y2_, text_);

            }

            else if (alignment_ == taRightJustify)

            {

                int cx = 0;

                int cy = 0;

                int cw = 0;

                int ch = 0;

                client_rect(cx, cy, cw, ch);

                int tw = xf::text_width(st, text_);

                xf::draw_text_left(display_, window_, gc_, st, cx + cw - tw - 4, y1_, y2_, text_);

            }

            else

            {

                xf::draw_text_centered(display_, window_, gc_, st, x1_, y1_, x2_, y2_, text_);

            }

        }

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, 1);

            int cx = 0;

            int cy = 0;

            int cw = 0;

            int ch = 0;

            client_rect(cx, cy, cw, ch);

            if (cw > 4 && ch > 4)

            {

                XDrawRectangle(display_, window_, gc_, cx + 1, cy + 1, cw - 3, ch - 3);

            }

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

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (button != 1)

        {

            return false;

        }

        if (contains(x, y) && enabled_ && on_click)

        {

            on_click();

            return true;

        }

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
