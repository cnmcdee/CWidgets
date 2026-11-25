#pragma once

#include "core/xframe_core.h"

//region GtkPaned

class GtkPaned

{

public:

    struct Rect

    {

        int x1;

        int y1;

        int x2;

        int y2;

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

    std::string style_name_;

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    int orientation_ = 0;

    int position_ = 160;

    bool dragging_ = false;

    int handle_size_ = 10;

public:

    std::function<void(int)> on_position_changed;

    GtkPaned(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkPaned";

        text_ = "Paned";

        position_ = width / 2;

        if (position_ < 40)

        {

            position_ = 40;

        }

    }

    ~GtkPaned() {}

    void set_bounds(int x, int y, int width, int height)

    {

        x1_ = x;

        y1_ = y;

        x2_ = x + width;

        y2_ = y + height;

        clamp_position();

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

    void clamp_position()

    {

        int span = (orientation_ == 0) ? width() : height();

        if (position_ < 40)

        {

            position_ = 40;

        }

        if (position_ > span - 40 && span - 40 >= 40)

        {

            position_ = span - 40;

        }

    }

    void set_position(int p)

    {

        position_ = p;

        clamp_position();

    }

    int get_position()

    {

        return position_;

    }

    void set_orientation(int o)

    {

        orientation_ = (o == 1) ? 1 : 0;

        clamp_position();

    }

    int get_orientation()

    {

        return orientation_;

    }

    bool is_horizontal()

    {

        return orientation_ == 0;

    }

    Rect first_pane_rect()

    {

        if (orientation_ == 0)

        {

            return Rect{x1_, y1_, x1_ + position_ - handle_size_ / 2, y2_};

        }

        return Rect{x1_, y1_, x2_, y1_ + position_ - handle_size_ / 2};

    }

    Rect second_pane_rect()

    {

        if (orientation_ == 0)

        {

            return Rect{x1_ + position_ + handle_size_ / 2, y1_, x2_, y2_};

        }

        return Rect{x1_, y1_ + position_ + handle_size_ / 2, x2_, y2_};

    }

    Rect handle_rect()

    {

        if (orientation_ == 0)

        {

            return Rect{x1_ + position_ - handle_size_ / 2, y1_, x1_ + position_ + handle_size_ / 2, y2_};

        }

        return Rect{x1_, y1_ + position_ - handle_size_ / 2, x2_, y1_ + position_ + handle_size_ / 2};

    }

    bool near_handle(int x, int y)

    {

        Rect h = handle_rect();

        return x >= h.x1 - 3 && x <= h.x2 + 3 && y >= h.y1 - 3 && y <= h.y2 + 3;

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

        Rect a = first_pane_rect();

        Rect b = second_pane_rect();

        Rect h = handle_rect();

        xf::draw_polished(display_, window_, gc_, st, a.x1, a.y1, a.x2, a.y2);

        xf::draw_polished(display_, window_, gc_, st, b.x1, b.y1, b.x2, b.y2);

        xf::draw_text_centered(display_, window_, gc_, st, a.x1, a.y1, a.x2, a.y2, "First");

        xf::draw_text_centered(display_, window_, gc_, st, b.x1, b.y1, b.x2, b.y2, "Second");

        unsigned long grip = dragging_ ? st.outlineColor_ : styles_->get_color_rgb(120, 120, 120);

        XSetForeground(display_, gc_, grip);

        XFillRectangle(display_, window_, gc_, h.x1, h.y1, h.x2 - h.x1, h.y2 - h.y1);

        unsigned long dot = styles_->get_color_rgb(235, 235, 235);

        XSetForeground(display_, gc_, dot);

        int cx = (h.x1 + h.x2) / 2;

        int cy = (h.y1 + h.y2) / 2;

        for (int i = -1; i <= 1; ++i)

        {

            if (orientation_ == 0)

            {

                XFillArc(display_, window_, gc_, cx - 2, cy + i * 10 - 2, 4, 4, 0, 360 * 64);

            }

            else

            {

                XFillArc(display_, window_, gc_, cx + i * 10 - 2, cy - 2, 4, 4, 0, 360 * 64);

            }

        }

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, height() - 4);

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

        if (near_handle(x, y))

        {

            dragging_ = true;

            return true;

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (!dragging_ || button != 1)

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

        if (orientation_ == 0)

        {

            position_ = x - x1_;

        }

        else

        {

            position_ = y - y1_;

        }

        clamp_position();

        if (on_position_changed)

        {

            on_position_changed(position_);

        }

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        int step = 8;

        if (orientation_ == 0 && (key == XK_Left || key == XK_Right))

        {

            position_ += (key == XK_Right) ? step : -step;

        }

        else if (orientation_ == 1 && (key == XK_Up || key == XK_Down))

        {

            position_ += (key == XK_Down) ? step : -step;

        }

        else

        {

            return false;

        }

        clamp_position();

        if (on_position_changed)

        {

            on_position_changed(position_);

        }

        return true;

    }

};

//endregion
