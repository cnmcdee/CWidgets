#pragma once

#include "core/xframe_core.h"

//region TToggleBox

class TToggleBox

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

    bool checked_ = false;

    int state_ = 0;   // 0 = unchecked, 1 = checked, 2 = grayed

    bool allow_grayed_ = false;

    void sync_state()

    {

        checked_ = (state_ == 1);

    }

    void fire_change()

    {

        sync_state();

        if (on_change)

        {

            on_change();

        }

    }

public:

    std::function<void()> on_change;

    std::function<void()> on_click;

    TToggleBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TToggleBox";

        text_ = "Toggle";

        checked_ = true;

        state_ = 1;

    }

    ~TToggleBox() {}

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

    bool get_checked()

    {

        return checked_;

    }

    void set_checked(bool checked)

    {

        state_ = checked ? 1 : 0;

        sync_state();

    }

    int get_state()

    {

        return state_;

    }

    void set_state(int state)

    {

        if (state < 0)

        {

            state = 0;

        }

        if (state > 2)

        {

            state = 2;

        }

        state_ = state;

        sync_state();

    }

    void set_allow_grayed(bool allow)

    {

        allow_grayed_ = allow;

    }

    bool get_allow_grayed()

    {

        return allow_grayed_;

    }

    void toggle()

    {

        if (allow_grayed_)

        {

            state_ = (state_ + 1) % 3;

        }

        else

        {

            state_ = (state_ == 1) ? 0 : 1;

        }

        fire_change();

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        if (!enabled_ || state_ == 2)

        {

            st.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        if (pressed_ || state_ == 1 || state_ == 2)

        {

            // Sunken / pressed look while the box is on.

            unsigned long swap = st.fillColor_;

            st.fillColor_ = st.outlineColor_;

            st.outlineColor_ = swap;

        }

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        if (state_ == 1)

        {

            int cy = (y1_ + y2_) / 2;

            xf::draw_checkmark(display_, window_, gc_, st.text_color_, x1_ + 14, cy, 14);

        }

        if (state_ == 2)

        {

            int cy = (y1_ + y2_) / 2;

            xf::draw_checkmark(display_, window_, gc_, styles_->get_color_rgb(150, 150, 150), x1_ + 14, cy, 14);

        }

        // Caption sits right of the checkmark space.

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 26, y1_, y2_, text_);

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

            toggle();

            if (on_click)

            {

                on_click();

            }

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

        if (key == XK_Return || key == XK_space)

        {

            toggle();

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
