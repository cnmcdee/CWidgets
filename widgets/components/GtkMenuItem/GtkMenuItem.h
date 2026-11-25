#pragma once

#include "core/xframe_core.h"

//region GtkMenuItem

class GtkMenuItem

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

    bool hovered_ = false;

    bool has_submenu_ = false;

    std::string accel_;

public:

    std::function<void()> on_click;

    GtkMenuItem(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkMenuItem";

        text_ = "Menu Item";

        accel_ = "";

    }

    ~GtkMenuItem() {}

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

    //region Menu item specific API

    void set_has_submenu(bool has)

    {

        has_submenu_ = has;

    }

    bool get_has_submenu()

    {

        return has_submenu_;

    }

    void set_accel(std::string accel)

    {

        accel_ = accel;

    }

    std::string get_accel()

    {

        return accel_;

    }

    bool get_hovered()

    {

        return hovered_;

    }

    void set_hovered(bool h)

    {

        hovered_ = h;

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        SStyle row_style = st;

        if (hovered_ && enabled_)

        {

            unsigned long hot_fill = styles_->get_color_rgb(70, 110, 180);

            XSetForeground(display_, gc_, hot_fill);

            XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

            row_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

        }

        else

        {

            XSetForeground(display_, gc_, st.fillColor_);

            XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        }

        if (!enabled_)

        {

            row_style.text_color_ = styles_->get_color_rgb(150, 150, 150);

        }

        xf::draw_text_left(display_, window_, gc_, row_style, x1_ + 12, y1_, y2_, text_);

        if (!accel_.empty())

        {

            int aw = xf::text_width(row_style, accel_);

            xf::draw_text_left(display_, window_, gc_, row_style, x2_ - aw - 24, y1_, y2_, accel_);

        }

        if (has_submenu_)

        {

            xf::draw_triangle_right(display_, window_, gc_, row_style.text_color_, x2_ - 14, (y1_ + y2_) / 2, 4);

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

        hovered_ = true;

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

        (void)button;

        return false;

    }

    bool on_motion(int x, int y)

    {

        bool inside = contains(x, y);

        if (inside != hovered_)

        {

            hovered_ = inside;

            return true;

        }

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
