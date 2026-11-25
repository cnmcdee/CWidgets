#pragma once

#include "core/xframe_core.h"

//region GtkToolButton

class GtkToolButton

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

    bool hovered_ = false;

    std::string icon_label_;    // one-letter glyph drawn in the icon box

public:

    std::function<void()> on_click;

    GtkToolButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkToolButton";

        text_ = "Save";

        icon_label_ = "S";

    }

    ~GtkToolButton() {}

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

    //region Tool button specific API

    void set_icon_label(std::string glyph)

    {

        icon_label_ = glyph;

    }

    std::string get_icon_label()

    {

        return icon_label_;

    }

    void set_label(std::string label)

    {

        text_ = label;

    }

    std::string get_label()

    {

        return text_;

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

        // Flat toolbar look: chrome appears on hover / press only.

        if (pressed_ || hovered_)

        {

            if (pressed_)

            {

                unsigned long swap = st.fillColor_;

                st.fillColor_ = st.outlineColor_;

                st.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        }

        else

        {

            XSetForeground(display_, gc_, st.fillColor_);

            XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        }

        // Icon box centred over the label.

        int iw = 22;

        int ih = 22;

        int ix = x1_ + (width() - iw) / 2;

        int iy = y1_ + 4;

        if (height() < 44)

        {

            iy = y1_ + (height() - ih) / 2;

        }

        SStyle icon_style = st;

        icon_style.fillColor_ = styles_->get_color_rgb(120, 160, 210);

        icon_style.radius_ = 4;

        xf::fill_rounded_rect(display_, window_, gc_, icon_style, ix, iy, ix + iw, iy + ih);

        xf::draw_rounded_outline(display_, window_, gc_, icon_style, ix, iy, ix + iw, iy + ih);

        SStyle glyph = st;

        glyph.text_color_ = styles_->get_color_rgb(255, 255, 255);

        xf::draw_text_centered(display_, window_, gc_, glyph, ix, iy, ix + iw, iy + ih, icon_label_);

        if (height() >= 44)

        {

            xf::draw_text_centered(display_, window_, gc_, st, x1_, iy + ih + 2, x2_, y2_ - 2, text_);

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

        if (contains(x, y) && on_click)

        {

            on_click();

        }

        return true;

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
