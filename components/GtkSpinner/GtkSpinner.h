#pragma once

#include "core/xframe_core.h"

#include <cmath>

//region GtkSpinner

// The "busy" throbber: twelve spokes arranged in a circle, lit in
// sequence. step() advances the animation one frame — call it from
// a timer while active_ is set.

class GtkSpinner

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

    bool active_ = true;

    int frame_ = 0;            // 0..11, which spoke is lit

public:

    GtkSpinner(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkSpinner";

        text_ = "GtkSpinner";

    }

    ~GtkSpinner() {}

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

    //region GtkSpinner API

    void start()

    {

        active_ = true;

    }

    void stop()

    {

        active_ = false;

    }

    void set_active(bool a)

    {

        active_ = a;

    }

    bool get_active()

    {

        return active_;

    }

    int get_frame()

    {

        return frame_;

    }

    // Advance one frame; a stopped spinner stays frozen.

    void step()

    {

        if (active_)

        {

            frame_ = (frame_ + 1) % 12;

        }

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        int cx = (x1_ + x2_) / 2;

        int cy = (y1_ + y2_) / 2;

        int radius = std::min(width(), height()) / 2 - 2;

        if (radius < 6)

        {

            radius = 6;

        }

        xf::set_line_width(display_, gc_, 2);

        for (int i = 0; i < 12; ++i)

        {

            double angle = i * (3.14159265 / 6.0) - 3.14159265 / 2.0;

            int ix = cx + (int)(std::cos(angle) * radius * 0.55);

            int iy = cy + (int)(std::sin(angle) * radius * 0.55);

            int ox = cx + (int)(std::cos(angle) * radius);

            int oy = cy + (int)(std::sin(angle) * radius);

            // Spokes trail behind the lit one, fading as they age.

            int age = (frame_ - i + 12) % 12;

            int shade = active_ ? std::max(45, 215 - age * 16) : 120;

            if (!enabled_)

            {

                shade = 170;

            }

            XSetForeground(display_, gc_, styles_->get_color_rgb((unsigned char)shade, (unsigned char)shade, (unsigned char)shade));

            XDrawLine(display_, window_, gc_, ix, iy, ox, oy);

        }

        xf::set_line_width(display_, gc_, 1);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || button != 1)

        {

            return false;

        }

        focused_ = true;

        active_ = !active_;   // click toggles the animation

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
