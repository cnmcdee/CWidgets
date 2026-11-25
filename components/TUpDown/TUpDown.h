#pragma once

#include "core/xframe_core.h"

//region TUpDown

class TUpDown

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

    int orientation_ = 0;      // 0 = vertical, 1 = horizontal

    int position_ = 25;

    int min_ = 0;

    int max_ = 100;

    int increment_ = 1;

    int pressed_half_ = -1;    // -1 none, 0 = up / left, 1 = down / right

    // The two arrow halves: half 0 is the top (or left) half.

    void half_rect(int half, int& hx1, int& hy1, int& hx2, int& hy2)

    {

        if (orientation_ == 0)

        {

            hx1 = x1_;

            hx2 = x2_;

            hy1 = (half == 0) ? y1_ : (y1_ + y2_) / 2;

            hy2 = (half == 0) ? (y1_ + y2_) / 2 : y2_;

        }

        else

        {

            hy1 = y1_;

            hy2 = y2_;

            hx1 = (half == 0) ? x1_ : (x1_ + x2_) / 2;

            hx2 = (half == 0) ? (x1_ + x2_) / 2 : x2_;

        }

    }

    int half_at(int x, int y)

    {

        for (int h = 0; h < 2; ++h)

        {

            int hx1, hy1, hx2, hy2;

            half_rect(h, hx1, hy1, hx2, hy2);

            if (x >= hx1 && x <= hx2 && y >= hy1 && y <= hy2)

            {

                return h;

            }

        }

        return -1;

    }

    void draw_left_triangle(int cx, int cy, int size, unsigned long color)

    {

        XSetForeground(display_, gc_, color);

        for (int i = 0; i < size; ++i)

        {

            XDrawLine(display_, window_, gc_, cx - i, cy - (size - i), cx - i, cy + (size - i));

        }

    }

    void change_by(int delta)

    {

        int p = position_ + delta;

        if (p < min_)

        {

            p = min_;

        }

        if (p > max_)

        {

            p = max_;

        }

        if (p != position_)

        {

            position_ = p;

            if (on_change)

            {

                on_change();

            }

        }

    }

public:

    std::function<void()> on_change;

    std::function<void()> on_click;

    TUpDown(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TUpDown";

        text_ = "TUpDown";

    }

    ~TUpDown() {}

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

    void set_orientation(int o)

    {

        orientation_ = o;

    }

    int get_orientation()

    {

        return orientation_;

    }

    void set_min(int m)

    {

        min_ = m;

    }

    void set_max(int m)

    {

        max_ = m;

    }

    void set_increment(int i)

    {

        increment_ = i;

    }

    int get_increment()

    {

        return increment_;

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

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        unsigned long arrow = st.text_color_;

        if (!enabled_)

        {

            arrow = styles_->get_color_rgb(140, 140, 140);

            st.text_color_ = arrow;

        }

        for (int h = 0; h < 2; ++h)

        {

            int hx1, hy1, hx2, hy2;

            half_rect(h, hx1, hy1, hx2, hy2);

            SStyle hs = st;

            if (pressed_half_ == h)

            {

                unsigned long swap = hs.fillColor_;

                hs.fillColor_ = hs.outlineColor_;

                hs.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, hs, hx1, hy1, hx2, hy2);

            int cx = (hx1 + hx2) / 2;

            int cy = (hy1 + hy2) / 2;

            if (orientation_ == 0)

            {

                if (h == 0)

                {

                    xf::draw_triangle_up(display_, window_, gc_, arrow, cx, cy + 3, 5);

                }

                else

                {

                    xf::draw_triangle_down(display_, window_, gc_, arrow, cx, cy - 3, 5);

                }

            }

            else

            {

                if (h == 0)

                {

                    draw_left_triangle(cx + 3, cy, 5, arrow);

                }

                else

                {

                    xf::draw_triangle_right(display_, window_, gc_, arrow, cx - 3, cy, 5);

                }

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

        pressed_half_ = half_at(x, y);

        if (pressed_half_ == 0)

        {

            change_by(orientation_ == 0 ? increment_ : -increment_);

        }

        else if (pressed_half_ == 1)

        {

            change_by(orientation_ == 0 ? -increment_ : increment_);

        }

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

        if (button != 1 || pressed_half_ < 0)

        {

            return false;

        }

        pressed_half_ = -1;

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

        if (key == XK_Up || key == XK_Right)

        {

            change_by(increment_);

            return true;

        }

        if (key == XK_Down || key == XK_Left)

        {

            change_by(-increment_);

            return true;

        }

        return false;

    }

};

//endregion
