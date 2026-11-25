#pragma once

#include "core/xframe_core.h"

//region GtkScale

class GtkScale

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

    bool vertical_ = false;

    double min_ = 0.0;

    double max_ = 100.0;

    double value_ = 40.0;

    double step_ = 1.0;

    double page_step_ = 10.0;

    bool draw_value_ = true;

    bool dragging_ = false;

    std::vector<double> marks_;

    int pad_ = 12;              // room for the handle at each end

    double clamp_value(double v)

    {

        return std::max(min_, std::min(max_, v));

    }

    // Centre position of the handle along the trough.

    int handle_pos()

    {

        double frac = (max_ > min_) ? (value_ - min_) / (max_ - min_) : 0.0;

        if (vertical_)

        {

            // Vertical scales run max at the top.

            return y2_ - pad_ - (int)(frac * (height() - 2 * pad_));

        }

        return x1_ + pad_ + (int)(frac * (width() - 2 * pad_));

    }

    double value_at(int x, int y)

    {

        double frac;

        if (vertical_)

        {

            frac = (double)(y2_ - pad_ - y) / (double)std::max(1, height() - 2 * pad_);

        }

        else

        {

            frac = (double)(x - x1_ - pad_) / (double)std::max(1, width() - 2 * pad_);

        }

        frac = std::max(0.0, std::min(1.0, frac));

        return clamp_value(min_ + frac * (max_ - min_));

    }

    void set_value_internal(double v)

    {

        double nv = clamp_value(v);

        if (nv != value_)

        {

            value_ = nv;

            if (on_value_changed)

            {

                on_value_changed();

            }

        }

    }

    std::string value_string()

    {

        char buf[32];

        if (step_ >= 1.0)

        {

            snprintf(buf, sizeof(buf), "%d", (int)(value_ + 0.5));

        }

        else

        {

            snprintf(buf, sizeof(buf), "%.1f", value_);

        }

        return std::string(buf);

    }

public:

    std::function<void()> on_value_changed;

    GtkScale(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkScale";

        text_ = "Scale";

        add_mark(0.0);

        add_mark(50.0);

        add_mark(100.0);

    }

    ~GtkScale() {}

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

    //region Scale specific API

    void set_range(double min, double max)

    {

        min_ = min;

        max_ = max;

        value_ = clamp_value(value_);

    }

    void set_value(double v)

    {

        set_value_internal(v);

    }

    double get_value()

    {

        return value_;

    }

    double get_min()

    {

        return min_;

    }

    double get_max()

    {

        return max_;

    }

    void set_step(double step)

    {

        step_ = step;

    }

    void set_orientation_vertical(bool vertical)

    {

        vertical_ = vertical;

    }

    bool get_orientation_vertical()

    {

        return vertical_;

    }

    void set_draw_value(bool draw)

    {

        draw_value_ = draw;

    }

    void add_mark(double at_value)

    {

        marks_.push_back(at_value);

    }

    void clear_marks()

    {

        marks_.clear();

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

        unsigned long trough_fill = styles_->get_color_rgb(200, 200, 200);

        unsigned long fill_part = styles_->get_color_rgb(70, 110, 180);

        int hp = handle_pos();

        if (!vertical_)

        {

            int cy = (y1_ + y2_) / 2 - 4;

            // Trough, with the filled portion up to the handle.

            XSetForeground(display_, gc_, trough_fill);

            XFillRectangle(display_, window_, gc_, x1_ + pad_, cy - 2, width() - 2 * pad_, 5);

            XSetForeground(display_, gc_, fill_part);

            XFillRectangle(display_, window_, gc_, x1_ + pad_, cy - 2, hp - x1_ - pad_, 5);

            // Marks under the trough.

            XSetForeground(display_, gc_, st.lineColor_);

            for (size_t i = 0; i < marks_.size(); ++i)

            {

                double frac = (max_ > min_) ? (marks_[i] - min_) / (max_ - min_) : 0.0;

                int mx = x1_ + pad_ + (int)(frac * (width() - 2 * pad_));

                XDrawLine(display_, window_, gc_, mx, cy + 5, mx, cy + 10);

            }

            // Handle.

            SStyle handle_style = st;

            handle_style.radius_ = 5;

            xf::draw_polished(display_, window_, gc_, handle_style, hp - 7, cy - 11, hp + 7, cy + 12);

            if (draw_value_)

            {

                xf::draw_text_centered(display_, window_, gc_, st, hp - 24, cy + 12, hp + 24, y2_, value_string());

            }

        }

        else

        {

            int cx = (x1_ + x2_) / 2;

            XSetForeground(display_, gc_, trough_fill);

            XFillRectangle(display_, window_, gc_, cx - 2, y1_ + pad_, 5, height() - 2 * pad_);

            XSetForeground(display_, gc_, fill_part);

            XFillRectangle(display_, window_, gc_, cx - 2, hp, 5, y2_ - pad_ - hp);

            XSetForeground(display_, gc_, st.lineColor_);

            for (size_t i = 0; i < marks_.size(); ++i)

            {

                double frac = (max_ > min_) ? (marks_[i] - min_) / (max_ - min_) : 0.0;

                int my = y2_ - pad_ - (int)(frac * (height() - 2 * pad_));

                XDrawLine(display_, window_, gc_, cx + 5, my, cx + 10, my);

            }

            SStyle handle_style = st;

            handle_style.radius_ = 5;

            xf::draw_polished(display_, window_, gc_, handle_style, cx - 11, hp - 7, cx + 12, hp + 7);

            if (draw_value_)

            {

                xf::draw_text_centered(display_, window_, gc_, st, cx + 12, hp - 10, x2_, hp + 10, value_string());

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

        if (!contains(x, y) || !enabled_)

        {

            return false;

        }

        focused_ = true;

        if (button == 4)

        {

            set_value_internal(value_ + step_);

            return true;

        }

        if (button == 5)

        {

            set_value_internal(value_ - step_);

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        dragging_ = true;

        set_value_internal(value_at(x, y));

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button != 1 || !dragging_)

        {

            return false;

        }

        dragging_ = false;

        return true;

    }

    bool on_motion(int x, int y)

    {

        if (!dragging_)

        {

            return false;

        }

        set_value_internal(value_at(x, y));

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Right || key == XK_Up)

        {

            set_value_internal(value_ + step_);

            return true;

        }

        if (key == XK_Left || key == XK_Down)

        {

            set_value_internal(value_ - step_);

            return true;

        }

        if (key == XK_Page_Up)

        {

            set_value_internal(value_ + page_step_);

            return true;

        }

        if (key == XK_Page_Down)

        {

            set_value_internal(value_ - page_step_);

            return true;

        }

        if (key == XK_Home)

        {

            set_value_internal(min_);

            return true;

        }

        if (key == XK_End)

        {

            set_value_internal(max_);

            return true;

        }

        return false;

    }

};

//endregion
