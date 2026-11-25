#pragma once

#include "core/xframe_core.h"

//region GtkScrollbar

class GtkScrollbar

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

    bool vertical_ = true;

    double value_ = 0.0;        // top / left of the visible range

    double upper_ = 100.0;      // total size of the scrolled area

    double page_size_ = 25.0;   // visible fraction

    double step_ = 4.0;

    bool dragging_ = false;

    int grab_offset_ = 0;       // pointer position inside the thumb

    double clamp_value(double v)

    {

        double hi = std::max(0.0, upper_ - page_size_);

        return std::max(0.0, std::min(hi, v));

    }

    int axis_length()

    {

        return vertical_ ? height() : width();

    }

    int axis_start()

    {

        return vertical_ ? y1_ : x1_;

    }

    int axis_pos(int x, int y)

    {

        return vertical_ ? y : x;

    }

    int thumb_length()

    {

        double frac = upper_ > 0.0 ? page_size_ / upper_ : 1.0;

        frac = std::max(0.05, std::min(1.0, frac));

        return std::max(18, (int)(frac * axis_length()));

    }

    int thumb_start()

    {

        double range = upper_ - page_size_;

        double frac = range > 0.0 ? value_ / range : 0.0;

        return axis_start() + (int)(frac * (axis_length() - thumb_length()));

    }

    bool on_thumb(int x, int y)

    {

        int p = axis_pos(x, y);

        return p >= thumb_start() && p <= thumb_start() + thumb_length();

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

public:

    std::function<void()> on_value_changed;

    GtkScrollbar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkScrollbar";

        text_ = "Scrollbar";

        // A tall scrollbar in the demo bounds reads as vertical;
        // a wide one as horizontal.

        vertical_ = height >= width;

    }

    ~GtkScrollbar() {}

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

    //region Scrollbar specific API

    void set_value(double v)

    {

        set_value_internal(v);

    }

    double get_value()

    {

        return value_;

    }

    void set_upper(double upper)

    {

        upper_ = std::max(1.0, upper);

        value_ = clamp_value(value_);

    }

    double get_upper()

    {

        return upper_;

    }

    void set_page_size(double page)

    {

        page_size_ = std::max(1.0, page);

        value_ = clamp_value(value_);

    }

    double get_page_size()

    {

        return page_size_;

    }

    void set_range(double upper, double page)

    {

        set_upper(upper);

        set_page_size(page);

    }

    void set_orientation_vertical(bool vertical)

    {

        vertical_ = vertical;

    }

    bool get_orientation_vertical()

    {

        return vertical_;

    }

    void scroll_by(double delta)

    {

        set_value_internal(value_ + delta);

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // Trough.

        unsigned long trough_fill = styles_->get_color_rgb(228, 228, 228);

        XSetForeground(display_, gc_, trough_fill);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        // Thumb.

        SStyle thumb_style = st;

        thumb_style.radius_ = 4;

        if (!enabled_)

        {

            thumb_style.fillColor_ = styles_->get_color_rgb(190, 190, 190);

        }

        int ts = thumb_start();

        int tl = thumb_length();

        if (vertical_)

        {

            xf::draw_polished(display_, window_, gc_, thumb_style, x1_ + 2, ts, x2_ - 2, ts + tl);

            // Grip lines.

            XSetForeground(display_, gc_, st.lineColor_);

            int gy = ts + tl / 2;

            XDrawLine(display_, window_, gc_, x1_ + 5, gy - 3, x2_ - 5, gy - 3);

            XDrawLine(display_, window_, gc_, x1_ + 5, gy, x2_ - 5, gy);

            XDrawLine(display_, window_, gc_, x1_ + 5, gy + 3, x2_ - 5, gy + 3);

        }

        else

        {

            xf::draw_polished(display_, window_, gc_, thumb_style, ts, y1_ + 2, ts + tl, y2_ - 2);

            XSetForeground(display_, gc_, st.lineColor_);

            int gx = ts + tl / 2;

            XDrawLine(display_, window_, gc_, gx - 3, y1_ + 5, gx - 3, y2_ - 5);

            XDrawLine(display_, window_, gc_, gx, y1_ + 5, gx, y2_ - 5);

            XDrawLine(display_, window_, gc_, gx + 3, y1_ + 5, gx + 3, y2_ - 5);

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

            set_value_internal(value_ - step_);

            return true;

        }

        if (button == 5)

        {

            set_value_internal(value_ + step_);

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        if (on_thumb(x, y))

        {

            dragging_ = true;

            grab_offset_ = axis_pos(x, y) - thumb_start();

            return true;

        }

        // Trough click pages toward the pointer.

        if (axis_pos(x, y) < thumb_start())

        {

            set_value_internal(value_ - page_size_);

        }

        else

        {

            set_value_internal(value_ + page_size_);

        }

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

        int new_start = axis_pos(x, y) - grab_offset_;

        double travel = (double)std::max(1, axis_length() - thumb_length());

        double frac = (double)(new_start - axis_start()) / travel;

        set_value_internal(frac * (upper_ - page_size_));

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Down || key == XK_Right)

        {

            set_value_internal(value_ + step_);

            return true;

        }

        if (key == XK_Up || key == XK_Left)

        {

            set_value_internal(value_ - step_);

            return true;

        }

        if (key == XK_Page_Down)

        {

            set_value_internal(value_ + page_size_);

            return true;

        }

        if (key == XK_Page_Up)

        {

            set_value_internal(value_ - page_size_);

            return true;

        }

        if (key == XK_Home)

        {

            set_value_internal(0.0);

            return true;

        }

        if (key == XK_End)

        {

            set_value_internal(upper_ - page_size_);

            return true;

        }

        return false;

    }

};

//endregion
