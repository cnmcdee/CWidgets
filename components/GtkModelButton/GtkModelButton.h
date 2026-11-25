#pragma once

#include "core/xframe_core.h"

//region GtkModelButton

class GtkModelButton

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

    bool hover_ = false;

    // The GtkModelButton role: 0 = normal, 1 = check, 2 = radio,
    // matching GTK_BUTTON_ROLE_NORMAL / CHECK / RADIO.

    int role_ = 1;

    bool active_ = true;   // preloaded ticked, matching the check role

    bool icon_visible_ = true;

public:

    std::function<void()> on_click;

    GtkModelButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkModelButton";

        text_ = "Menu Item";

    }

    ~GtkModelButton() {}

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

    //region Role and state

    void set_role(int role)

    {

        role_ = role;

    }

    int get_role()

    {

        return role_;

    }

    void set_active(bool a)

    {

        active_ = a;

    }

    bool get_active()

    {

        return active_;

    }

    void set_icon_visible(bool v)

    {

        icon_visible_ = v;

    }

    bool get_icon_visible()

    {

        return icon_visible_;

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

        // A menu row is flat; it only paints a background while the
        // pointer is over it or it is being pressed.

        if (hover_ || pressed_ || focused_)

        {

            SStyle bg = st;

            bg.fillColor_ = st.outlineColor_;

            bg.radius_ = 4;

            xf::fill_rounded_rect(display_, window_, gc_, bg, x1_, y1_, x2_, y2_);

        }

        int cy = y1_ + height() / 2;

        // The indicator slot at the left: a check mark, a radio dot,
        // or empty space for a normal item.

        if (role_ == 1 && active_)

        {

            xf::draw_checkmark(display_, window_, gc_, st.text_color_, x1_ + 12, cy, 11);

        }

        else if (role_ == 2)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, 1);

            XDrawArc(display_, window_, gc_, x1_ + 7, cy - 5, 10, 10, 0, 360 * 64);

            if (active_)

            {

                XSetForeground(display_, gc_, st.text_color_);

                XFillArc(display_, window_, gc_, x1_ + 10, cy - 2, 4, 4, 0, 360 * 64);

            }

        }

        int text_x = x1_ + 24;

        // The optional icon: a small framed box with a picture glyph.

        if (icon_visible_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 24, cy - 7, 14, 14);

            XDrawLine(display_, window_, gc_, x1_ + 26, cy + 5, x1_ + 31, cy);

            XDrawLine(display_, window_, gc_, x1_ + 31, cy, x1_ + 34, cy + 3);

            XDrawLine(display_, window_, gc_, x1_ + 31, cy, x1_ + 36, cy + 5);

            text_x = x1_ + 44;

        }

        xf::draw_text_left(display_, window_, gc_, st, text_x, y1_, y2_, text_);

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

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (!pressed_ || button != 1)

        {

            return false;

        }

        pressed_ = false;

        if (contains(x, y))

        {

            if (role_ == 1)

            {

                active_ = !active_;

            }

            else if (role_ == 2)

            {

                active_ = true;

            }

            if (on_click)

            {

                on_click();

            }

        }

        return true;

    }

    bool on_motion(int x, int y)

    {

        bool inside = contains(x, y);

        if (inside != hover_)

        {

            hover_ = inside;

            return true;   // caller redraws, the highlight changes

        }

        return inside;

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

            if (role_ == 1)

            {

                active_ = !active_;

            }

            else if (role_ == 2)

            {

                active_ = true;

            }

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
