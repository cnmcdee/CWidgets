#pragma once

#include "core/xframe_core.h"

#include <cmath>

#include <cstdio>

//region TFloatSpinEdit

class TFloatSpinEdit

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

    double value_ = 3.14;

    double min_value_ = 0.0;

    double max_value_ = 100.0;

    double increment_ = 0.1;

    int decimals_ = 2;

    std::string buffer_ = "3.14";

    int cursor_ = 4;

    int button_w_ = 16;

    int pressed_zone_ = 0;   // 0 none, 1 up, 2 down

    double clamp_value(double v)

    {

        if (v < min_value_)

        {

            return min_value_;

        }

        if (v > max_value_)

        {

            return max_value_;

        }

        return v;

    }

    std::string format_value(double v)

    {

        char buf[64];

        std::snprintf(buf, sizeof(buf), "%.*f", decimals_, v);

        return buf;

    }

    void sync_buffer()

    {

        buffer_ = format_value(value_);

        text_ = buffer_;

        cursor_ = (int)buffer_.size();

    }

    void commit()

    {

        double v = value_;

        try

        {

            v = std::stod(buffer_.empty() ? "0" : buffer_);

        }

        catch (...)

        {

            v = value_;

        }

        set_value(v);

    }

    void step(int dir)

    {

        commit();

        set_value(value_ + dir * increment_);

    }

public:

    std::function<void()> on_change;

    TFloatSpinEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TFloatSpinEdit";

        text_ = "3.14";

    }

    ~TFloatSpinEdit() {}

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

        buffer_ = t;

        cursor_ = (int)buffer_.size();

        commit();

    }

    std::string get_text()

    {

        return buffer_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    double get_value()

    {

        return value_;

    }

    void set_value(double v)

    {

        double nv = clamp_value(v);

        bool changed = std::fabs(nv - value_) > 1e-12;

        value_ = nv;

        sync_buffer();

        if (changed && on_change)

        {

            on_change();

        }

    }

    void set_min_value(double v)

    {

        min_value_ = v;

        set_value(value_);

    }

    double get_min_value()

    {

        return min_value_;

    }

    void set_max_value(double v)

    {

        max_value_ = v;

        set_value(value_);

    }

    double get_max_value()

    {

        return max_value_;

    }

    void set_increment(double v)

    {

        increment_ = v;

    }

    double get_increment()

    {

        return increment_;

    }

    void set_decimals(int d)

    {

        decimals_ = d < 0 ? 0 : (d > 10 ? 10 : d);

        sync_buffer();

    }

    int get_decimals()

    {

        return decimals_;

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

        // White edit field.

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 4, y1_, y2_, buffer_);

        if (focused_ && enabled_)

        {

            std::string before = buffer_.substr(0, cursor_);

            int cx = x1_ + 4 + xf::text_width(st, before);

            XSetForeground(display_, gc_, st.text_color_);

            XDrawLine(display_, window_, gc_, cx, y1_ + 3, cx, y2_ - 3);

        }

        // Up / down button zones on the right.

        int bx = x2_ - button_w_;

        int mid = y1_ + height() / 2;

        SStyle up = st;

        SStyle dn = st;

        if (pressed_zone_ == 1)

        {

            up.fillColor_ = st.outlineColor_;

        }

        if (pressed_zone_ == 2)

        {

            dn.fillColor_ = st.outlineColor_;

        }

        xf::draw_polished(display_, window_, gc_, up, bx, y1_, x2_, mid);

        xf::draw_polished(display_, window_, gc_, dn, bx, mid, x2_, y2_);

        xf::draw_triangle_up(display_, window_, gc_, st.text_color_, bx + button_w_ / 2, y1_ + (mid - y1_) / 2 + 3, 3);

        xf::draw_triangle_down(display_, window_, gc_, st.text_color_, bx + button_w_ / 2, mid + (y2_ - mid) / 2 - 3, 3);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        int bx = x2_ - button_w_;

        if (x >= bx)

        {

            pressed_zone_ = (y < y1_ + height() / 2) ? 1 : 2;

            step(pressed_zone_ == 1 ? 1 : -1);

        }

        else

        {

            SStyle st = style();

            int best = (int)buffer_.size();

            for (int i = 0; i <= (int)buffer_.size(); ++i)

            {

                if (x1_ + 4 + xf::text_width(st, buffer_.substr(0, i)) >= x)

                {

                    best = i;

                    break;

                }

            }

            cursor_ = best;

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button != 1 || pressed_zone_ == 0)

        {

            return false;

        }

        pressed_zone_ = 0;

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_BackSpace)

        {

            if (cursor_ > 0)

            {

                buffer_.erase(cursor_ - 1, 1);

                --cursor_;

            }

            return true;

        }

        if (key == XK_Delete)

        {

            if (cursor_ < (int)buffer_.size())

            {

                buffer_.erase(cursor_, 1);

            }

            return true;

        }

        if (key == XK_Left)

        {

            if (cursor_ > 0)

            {

                --cursor_;

            }

            return true;

        }

        if (key == XK_Right)

        {

            if (cursor_ < (int)buffer_.size())

            {

                ++cursor_;

            }

            return true;

        }

        if (key == XK_Home)

        {

            cursor_ = 0;

            return true;

        }

        if (key == XK_End)

        {

            cursor_ = (int)buffer_.size();

            return true;

        }

        if (key == XK_Up)

        {

            step(1);

            return true;

        }

        if (key == XK_Down)

        {

            step(-1);

            return true;

        }

        if (key == XK_Return || key == XK_KP_Enter)

        {

            commit();

            return true;

        }

        if (str && ((str[0] >= '0' && str[0] <= '9') || str[0] == '.' || (str[0] == '-' && cursor_ == 0)))

        {

            buffer_.insert(cursor_, 1, str[0]);

            ++cursor_;

            return true;

        }

        return false;

    }

};

//endregion
