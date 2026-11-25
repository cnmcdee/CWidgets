#pragma once

#include "core/xframe_core.h"

//region GtkSeparator

// A single etched line, horizontal or vertical: a dark line with a
// light line beside it, centred in the widget's bounds.

class GtkSeparator

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

    int orientation_ = 0;      // 0 = horizontal, 1 = vertical

public:

    GtkSeparator(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkSeparator";

        text_ = "GtkSeparator";

        // A wide short box reads horizontal, a tall thin one vertical.

        orientation_ = (height > width) ? 1 : 0;

    }

    ~GtkSeparator() {}

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

    //region GtkSeparator API

    void set_orientation(int o)

    {

        orientation_ = (o == 1) ? 1 : 0;

    }

    int get_orientation()

    {

        return orientation_;

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        unsigned long dark = styles_->get_color_rgb(150, 150, 150);

        unsigned long light = styles_->get_color_rgb(248, 248, 248);

        if (orientation_ == 0)

        {

            int cy = (y1_ + y2_) / 2;

            XSetForeground(display_, gc_, dark);

            XDrawLine(display_, window_, gc_, x1_, cy, x2_ - 1, cy);

            XSetForeground(display_, gc_, light);

            XDrawLine(display_, window_, gc_, x1_, cy + 1, x2_ - 1, cy + 1);

        }

        else

        {

            int cx = (x1_ + x2_) / 2;

            XSetForeground(display_, gc_, dark);

            XDrawLine(display_, window_, gc_, cx, y1_, cx, y2_ - 1);

            XSetForeground(display_, gc_, light);

            XDrawLine(display_, window_, gc_, cx + 1, y1_, cx + 1, y2_ - 1);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        (void)button;

        return false;   // separators are purely decorative

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
