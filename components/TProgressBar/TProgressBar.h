#pragma once

#include "core/xframe_core.h"

//region TProgressBar

class TProgressBar

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

    int min_ = 0;

    int max_ = 100;

    int position_ = 65;

    int step_ = 10;

    bool smooth_ = false;      // false = segmented blocks, true = smooth bar

    int orientation_ = 0;      // 0 = horizontal, 1 = vertical

    bool show_text_ = false;   // draw the percentage over the bar

    double fraction()

    {

        if (max_ <= min_)

        {

            return 0.0;

        }

        double f = (double)(position_ - min_) / (double)(max_ - min_);

        if (f < 0.0)

        {

            f = 0.0;

        }

        if (f > 1.0)

        {

            f = 1.0;

        }

        return f;

    }

public:

    TProgressBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TProgressBar";

        text_ = "TProgressBar";

    }

    ~TProgressBar() {}

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

    void set_min(int m)

    {

        min_ = m;

        set_position(position_);

    }

    int get_min()

    {

        return min_;

    }

    void set_max(int m)

    {

        max_ = m;

        set_position(position_);

    }

    int get_max()

    {

        return max_;

    }

    void set_position(int p)

    {

        if (p < min_)

        {

            p = min_;

        }

        if (p > max_)

        {

            p = max_;

        }

        position_ = p;

    }

    int get_position()

    {

        return position_;

    }

    void set_step(int s)

    {

        step_ = s;

    }

    int get_step()

    {

        return step_;

    }

    void step_it()

    {

        set_position(position_ + step_);

    }

    void step_by(int delta)

    {

        set_position(position_ + delta);

    }

    void set_smooth(bool s)

    {

        smooth_ = s;

    }

    bool get_smooth()

    {

        return smooth_;

    }

    void set_orientation(int o)

    {

        orientation_ = o;

    }

    void set_show_text(bool s)

    {

        show_text_ = s;

    }

    int get_percent()

    {

        return (int)(fraction() * 100.0 + 0.5);

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // Sunken frame with a white well inside.

        unsigned long white = styles_->get_color_rgb(255, 255, 255);

        unsigned long bar = styles_->get_color_rgb(60, 120, 210);

        if (!enabled_)

        {

            bar = styles_->get_color_rgb(150, 150, 150);

        }

        XSetForeground(display_, gc_, white);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, (int)st.lineWidth_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        int ix1 = x1_ + 3;

        int iy1 = y1_ + 3;

        int ix2 = x2_ - 3;

        int iy2 = y2_ - 3;

        double f = fraction();

        XSetForeground(display_, gc_, bar);

        if (orientation_ == 0)

        {

            int iw = ix2 - ix1;

            if (smooth_)

            {

                int fw = (int)(iw * f);

                if (fw > 0)

                {

                    XFillRectangle(display_, window_, gc_, ix1, iy1, fw, iy2 - iy1);

                }

            }

            else

            {

                // Segmented blocks: 10 px block, 2 px gap.

                int pitch = 12;

                int total = iw / pitch;

                int filled = (int)(total * f + 0.5);

                for (int i = 0; i < filled; ++i)

                {

                    XFillRectangle(display_, window_, gc_, ix1 + i * pitch, iy1, 10, iy2 - iy1);

                }

            }

        }

        else

        {

            int ih = iy2 - iy1;

            if (smooth_)

            {

                int fh = (int)(ih * f);

                if (fh > 0)

                {

                    XFillRectangle(display_, window_, gc_, ix1, iy2 - fh, ix2 - ix1, fh);

                }

            }

            else

            {

                int pitch = 12;

                int total = ih / pitch;

                int filled = (int)(total * f + 0.5);

                for (int i = 0; i < filled; ++i)

                {

                    XFillRectangle(display_, window_, gc_, ix1, iy2 - (i + 1) * pitch + 2, ix2 - ix1, 10);

                }

            }

        }

        if (show_text_)

        {

            std::string pct = std::to_string(get_percent()) + "%";

            xf::draw_text_centered(display_, window_, gc_, st, x1_, y1_, x2_, y2_, pct);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || button != 1)

        {

            return false;

        }

        focused_ = true;

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

        (void)key;

        (void)str;

        return false;

    }

};

//endregion
