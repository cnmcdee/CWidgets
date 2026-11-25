#pragma once

#include "core/xframe_core.h"

#include <cstdio>

//region GtkScaleButton

class GtkScaleButton

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

    bool pressed_ = false;

    double value_ = 50;

    double min_ = 0;

    double max_ = 100;

    double step_ = 1;

    // Which end zone the current press landed in: -1 minus, +1 plus.

    int pressed_zone_ = 0;

    int zone_at(int x)

    {

        int zone = 20;

        if (x < x1_ + zone)

        {

            return -1;

        }

        if (x > x2_ - zone)

        {

            return 1;

        }

        return 0;

    }

    void apply_step(int dir)

    {

        double v = value_ + dir * step_;

        if (v < min_)

        {

            v = min_;

        }

        if (v > max_)

        {

            v = max_;

        }

        if (v != value_)

        {

            value_ = v;

            if (on_value_changed)

            {

                on_value_changed(value_);

            }

        }

    }

public:

    std::function<void()> on_click;

    std::function<void(double)> on_value_changed;

    GtkScaleButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkScaleButton";

        text_ = "50";

    }

    ~GtkScaleButton() {}

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

    //region Value and range

    void set_value(double v)

    {

        if (v < min_)

        {

            v = min_;

        }

        if (v > max_)

        {

            v = max_;

        }

        value_ = v;

    }

    double get_value()

    {

        return value_;

    }

    void set_range(double min, double max)

    {

        min_ = min;

        max_ = max;

        set_value(value_);

    }

    double get_min()

    {

        return min_;

    }

    double get_max()

    {

        return max_;

    }

    void set_step(double s)

    {

        step_ = s;

    }

    double get_step()

    {

        return step_;

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

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        int cy = y1_ + height() / 2;

        // Minus glyph in the left zone.

        XSetForeground(display_, gc_, st.text_color_);

        xf::set_line_width(display_, gc_, 2);

        XDrawLine(display_, window_, gc_, x1_ + 6, cy, x1_ + 14, cy);

        // Plus glyph in the right zone.

        XDrawLine(display_, window_, gc_, x2_ - 14, cy, x2_ - 6, cy);

        XDrawLine(display_, window_, gc_, x2_ - 10, cy - 4, x2_ - 10, cy + 4);

        xf::set_line_width(display_, gc_, 1);

        // The current value, centred between the zones.

        char buf[32];

        if (step_ >= 1)

        {

            std::snprintf(buf, sizeof(buf), "%d", (int)(value_ + 0.5));

        }

        else

        {

            std::snprintf(buf, sizeof(buf), "%.1f", value_);

        }

        xf::draw_text_centered(display_, window_, gc_, st, x1_ + 20, y1_, x2_ - 20, y2_, buf);

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 3, y1_ + 3, width() - 6, height() - 6);

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

        pressed_zone_ = zone_at(x);

        if (pressed_zone_ != 0)

        {

            apply_step(pressed_zone_);

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (!pressed_ || button != 1)

        {

            return false;

        }

        pressed_ = false;

        pressed_zone_ = 0;

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

        if (key == XK_Left || key == XK_Down)

        {

            apply_step(-1);

            return true;

        }

        if (key == XK_Right || key == XK_Up)

        {

            apply_step(1);

            return true;

        }

        return false;

    }

};

//endregion
