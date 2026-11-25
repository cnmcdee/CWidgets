#pragma once

#include "core/xframe_core.h"

#include <cstdio>

//region GtkSpinButton

class GtkSpinButton

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;   // the editable numeric buffer

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    double value_ = 0.0;

    double lower_ = 0.0;

    double upper_ = 100.0;

    double step_increment_ = 1.0;

    double page_increment_ = 10.0;

    int digits_ = 0;

    bool wrap_ = false;

    int cursor_ = 0;

    bool blink_on_ = true;

    std::string format_value(double v)

    {

        char buf[64];

        std::snprintf(buf, sizeof(buf), "%.*f", digits_, v);

        return std::string(buf);

    }

    void sync_buffer()

    {

        text_ = format_value(value_);

        cursor_ = (int)text_.size();

    }

    double clamp_value(double v)

    {

        if (wrap_)

        {

            if (v > upper_)

            {

                v = lower_ + (v - upper_);

            }

            if (v < lower_)

            {

                v = upper_ - (lower_ - v);

            }

            return v;

        }

        if (v < lower_)

        {

            v = lower_;

        }

        if (v > upper_)

        {

            v = upper_;

        }

        return v;

    }

    void step_by(double delta)

    {

        double old = value_;

        value_ = clamp_value(value_ + delta);

        sync_buffer();

        if (value_ != old && on_value_changed)

        {

            on_value_changed(value_);

        }

    }

    // Arrow zones: a strip at the right edge, split top / bottom.

    int arrow_left()

    {

        return x2_ - 22;

    }

    bool in_up_zone(int x, int y)

    {

        return x >= arrow_left() && y >= y1_ && y < (y1_ + y2_) / 2;

    }

    bool in_down_zone(int x, int y)

    {

        return x >= arrow_left() && y >= (y1_ + y2_) / 2 && y <= y2_;

    }

public:

    std::function<void(double)> on_value_changed;

    std::function<void()> on_activate;

    GtkSpinButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkSpinButton";

        lower_ = 0.0;

        upper_ = 100.0;

        step_increment_ = 1.0;

        value_ = 42.0;

        sync_buffer();

    }

    ~GtkSpinButton() {}

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

        set_value(std::atof(t.c_str()));

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region GtkSpinButton specific API

    void set_range(double min, double max)

    {

        lower_ = min;

        upper_ = max;

        value_ = clamp_value(value_);

        sync_buffer();

    }

    void get_range(double& min, double& max)

    {

        min = lower_;

        max = upper_;

    }

    void set_increments(double step, double page)

    {

        step_increment_ = step;

        page_increment_ = page;

    }

    void set_digits(int digits)

    {

        digits_ = std::max(0, std::min(20, digits));

        sync_buffer();

    }

    int get_digits()

    {

        return digits_;

    }

    void set_wrap(bool w)

    {

        wrap_ = w;

    }

    bool get_wrap()

    {

        return wrap_;

    }

    void set_value(double v)

    {

        double old = value_;

        value_ = clamp_value(v);

        sync_buffer();

        if (value_ != old && on_value_changed)

        {

            on_value_changed(value_);

        }

    }

    double get_value()

    {

        return value_;

    }

    int get_value_as_int()

    {

        return (int)(value_ >= 0 ? value_ + 0.5 : value_ - 0.5);

    }

    // Parse the edited buffer back into value_ (Enter / focus commit).

    void commit()

    {

        set_value(std::atof(text_.c_str()));

    }

    void spin(int direction)   // +1 up one step, -1 down one step

    {

        step_by(direction >= 0 ? step_increment_ : -step_increment_);

    }

    void tick()

    {

        if (focused_)

        {

            blink_on_ = !blink_on_;

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

        SStyle body = st;

        body.fillColor_ = styles_->get_color_rgb(255, 255, 255);

        if (!enabled_)

        {

            body.fillColor_ = styles_->get_color_rgb(235, 235, 235);

        }

        xf::fill_rounded_rect(display_, window_, gc_, body, x1_, y1_, x2_, y2_);

        if (focused_)

        {

            SStyle ring = st;

            ring.lineColor_ = styles_->get_color_rgb(60, 120, 200);

            ring.lineWidth_ = 2;

            xf::draw_rounded_outline(display_, window_, gc_, ring, x1_, y1_, x2_, y2_);

        }

        else

        {

            xf::draw_rounded_outline(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        }

        // Value text, right-aligned before the arrow strip.

        SStyle ink = st;

        if (!enabled_)

        {

            ink.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        int tw = xf::text_width(ink, text_);

        int baseline = y1_ + (height() + (st.font_ ? st.font_->ascent - st.font_->descent : 10)) / 2;

        xf::draw_text_at(display_, window_, gc_, ink, arrow_left() - 6 - tw, baseline, text_);

        // Arrow strip.

        XSetForeground(display_, gc_, st.outlineColor_);

        XFillRectangle(display_, window_, gc_, arrow_left(), y1_ + 2, 20, height() - 4);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, arrow_left(), y1_ + 2, arrow_left(), y2_ - 2);

        XDrawLine(display_, window_, gc_, arrow_left(), (y1_ + y2_) / 2, x2_ - 2, (y1_ + y2_) / 2);

        unsigned long tri = enabled_ ? st.text_color_ : styles_->get_color_rgb(150, 150, 150);

        xf::draw_triangle_up(display_, window_, gc_, tri, arrow_left() + 10, y1_ + height() / 4 + 3, 5);

        xf::draw_triangle_down(display_, window_, gc_, tri, arrow_left() + 10, y1_ + (3 * height()) / 4 - 3, 5);

        // Cursor bar inside the number.

        if (focused_ && enabled_ && blink_on_)

        {

            int cx = arrow_left() - 6 - tw + xf::text_width(st, text_.substr(0, (size_t)cursor_));

            XSetForeground(display_, gc_, styles_->get_color_rgb(20, 20, 20));

            XDrawLine(display_, window_, gc_, cx, y1_ + 5, cx, y2_ - 5);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_)

        {

            return false;

        }

        focused_ = true;

        blink_on_ = true;

        if (button == 4)   // wheel up

        {

            step_by(step_increment_);

            return true;

        }

        if (button == 5)   // wheel down

        {

            step_by(-step_increment_);

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        if (in_up_zone(x, y))

        {

            step_by(step_increment_);

            return true;

        }

        if (in_down_zone(x, y))

        {

            step_by(-step_increment_);

            return true;

        }

        // Click in the number: place the cursor.

        int tw = xf::text_width(style(), text_);

        int start = arrow_left() - 6 - tw;

        int px = x - start;

        int best = (int)text_.size();

        int best_dist = 1 << 30;

        for (int i = 0; i <= (int)text_.size(); ++i)

        {

            int d = xf::text_width(style(), text_.substr(0, (size_t)i)) - px;

            if (d < 0)

            {

                d = -d;

            }

            if (d < best_dist)

            {

                best_dist = d;

                best = i;

            }

        }

        cursor_ = best;

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        blink_on_ = true;

        if (key == XK_Up)

        {

            step_by(step_increment_);

            return true;

        }

        if (key == XK_Down)

        {

            step_by(-step_increment_);

            return true;

        }

        if (key == XK_Page_Up)

        {

            step_by(page_increment_);

            return true;

        }

        if (key == XK_Page_Down)

        {

            step_by(-page_increment_);

            return true;

        }

        if (key == XK_Return || key == XK_KP_Enter)

        {

            commit();

            if (on_activate)

            {

                on_activate();

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

            if (cursor_ < (int)text_.size())

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

            cursor_ = (int)text_.size();

            return true;

        }

        if (key == XK_BackSpace)

        {

            if (cursor_ > 0)

            {

                text_.erase((size_t)(cursor_ - 1), 1);

                --cursor_;

            }

            return true;

        }

        if (key == XK_Delete)

        {

            if (cursor_ < (int)text_.size())

            {

                text_.erase((size_t)cursor_, 1);

            }

            return true;

        }

        // Only numeric characters may be typed.

        if (str && ((str[0] >= '0' && str[0] <= '9') || str[0] == '.' || str[0] == '-' || str[0] == '+'))

        {

            text_.insert((size_t)cursor_, 1, str[0]);

            ++cursor_;

            return true;

        }

        return false;

    }

};

//endregion
