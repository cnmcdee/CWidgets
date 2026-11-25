#pragma once

#include "core/xframe_core.h"

//region TImage

class TImage

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

    bool stretch_ = false;

    bool proportional_ = false;

    bool center_ = true;

    bool transparent_ = false;

    int picture_width_ = 64;

    int picture_height_ = 48;

public:

    std::function<void()> on_click;

    TImage(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TImage";

        text_ = "Image";

    }

    ~TImage() {}

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

    void set_stretch(bool s)

    {

        stretch_ = s;

    }

    bool get_stretch()

    {

        return stretch_;

    }

    void set_proportional(bool p)

    {

        proportional_ = p;

    }

    bool get_proportional()

    {

        return proportional_;

    }

    void set_center(bool c)

    {

        center_ = c;

    }

    bool get_center()

    {

        return center_;

    }

    void set_transparent(bool t)

    {

        transparent_ = t;

    }

    bool get_transparent()

    {

        return transparent_;

    }

    void set_picture_size(int w, int h)

    {

        picture_width_ = w;

        picture_height_ = h;

    }

    int get_picture_width()

    {

        return picture_width_;

    }

    int get_picture_height()

    {

        return picture_height_;

    }

    // Compute where the (pretend) picture lands inside the client
    // area, honouring stretch / proportional / center like LCL.

    void glyph_rect(int& gx1, int& gy1, int& gx2, int& gy2)

    {

        int cw = width();

        int ch = height();

        int pw = picture_width_;

        int ph = picture_height_;

        if (stretch_)

        {

            if (proportional_ && pw > 0 && ph > 0)

            {

                double scale_w = (double)cw / (double)pw;

                double scale_h = (double)ch / (double)ph;

                double scale = scale_w < scale_h ? scale_w : scale_h;

                int nw = (int)(pw * scale);

                int nh = (int)(ph * scale);

                gx1 = x1_ + (cw - nw) / 2;

                gy1 = y1_ + (ch - nh) / 2;

                gx2 = gx1 + nw;

                gy2 = gy1 + nh;

            }

            else

            {

                gx1 = x1_;

                gy1 = y1_;

                gx2 = x2_;

                gy2 = y2_;

            }

            return;

        }

        if (center_)

        {

            gx1 = x1_ + (cw - pw) / 2;

            gy1 = y1_ + (ch - ph) / 2;

            gx2 = gx1 + pw;

            gy2 = gy1 + ph;

        }

        else

        {

            gx1 = x1_;

            gy1 = y1_;

            gx2 = x1_ + pw;

            gy2 = y1_ + ph;

        }

        if (gx1 < x1_)

        {

            gx1 = x1_;

        }

        if (gy1 < y1_)

        {

            gy1 = y1_;

        }

        if (gx2 > x2_)

        {

            gx2 = x2_;

        }

        if (gy2 > y2_)

        {

            gy2 = y2_;

        }

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

        if (!transparent_)

        {

            XSetForeground(display_, gc_, st.fillColor_);

            XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        }

        int gx1 = 0;

        int gy1 = 0;

        int gx2 = 0;

        int gy2 = 0;

        glyph_rect(gx1, gy1, gx2, gy2);

        int gw = gx2 - gx1;

        int gh = gy2 - gy1;

        if (gw < 8 || gh < 8)

        {

            return;

        }

        unsigned long sky = styles_->get_color_rgb(210, 230, 250);

        unsigned long sun_col = styles_->get_color_rgb(245, 200, 40);

        unsigned long hill_col = styles_->get_color_rgb(70, 150, 70);

        unsigned long ink = enabled_ ? st.lineColor_ : styles_->get_color_rgb(140, 140, 140);

        if (!transparent_)

        {

            XSetForeground(display_, gc_, sky);

            XFillRectangle(display_, window_, gc_, gx1, gy1, gw, gh);

        }

        XSetForeground(display_, gc_, ink);

        XDrawRectangle(display_, window_, gc_, gx1, gy1, gw - 1, gh - 1);

        // Sun: filled circle, top-right of the frame.

        int sun_d = gw / 5;

        if (sun_d < 6)

        {

            sun_d = 6;

        }

        if (sun_d > 18)

        {

            sun_d = 18;

        }

        int sun_x = gx2 - sun_d - 6;

        int sun_y = gy1 + 5;

        XSetForeground(display_, gc_, sun_col);

        XFillArc(display_, window_, gc_, sun_x, sun_y, sun_d, sun_d, 0, 360 * 64);

        XSetForeground(display_, gc_, ink);

        XDrawArc(display_, window_, gc_, sun_x, sun_y, sun_d, sun_d, 0, 360 * 64);

        // Mountains: two peaks along the bottom edge.

        int base_y = gy2 - 1;

        int peak1_x = gx1 + gw / 4;

        int peak1_y = gy1 + gh / 2;

        int mid_x = gx1 + gw / 2;

        int mid_y = gy1 + gh * 3 / 4;

        int peak2_x = gx1 + gw * 3 / 4;

        int peak2_y = gy1 + gh * 2 / 5;

        XSetForeground(display_, gc_, hill_col);

        for (int i = 0; i < gh / 3; ++i)

        {

            int y = base_y - i;

            int spread1 = i * 2;

            XDrawLine(display_, window_, gc_, peak1_x - spread1 / 2, y, peak1_x + spread1 / 2, y);

        }

        XSetForeground(display_, gc_, ink);

        xf::set_line_width(display_, gc_, 1);

        XDrawLine(display_, window_, gc_, gx1, base_y, peak1_x, peak1_y);

        XDrawLine(display_, window_, gc_, peak1_x, peak1_y, mid_x, mid_y);

        XDrawLine(display_, window_, gc_, mid_x, mid_y, peak2_x, peak2_y);

        XDrawLine(display_, window_, gc_, peak2_x, peak2_y, gx2 - 1, base_y);

        if (!text_.empty())

        {

            int label_y1 = gy2 + 2;

            if (label_y1 + 12 < y2_)

            {

                xf::draw_text_centered(display_, window_, gc_, st, x1_, label_y1, x2_, y2_, text_);

            }

            else

            {

                xf::draw_text_centered(display_, window_, gc_, st, gx1, gy1, gx2, gy2, text_);

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

        if (contains(x, y) && on_click)

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
