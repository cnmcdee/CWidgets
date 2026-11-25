#pragma once

#include "core/xframe_core.h"

//region GtkColorButton

class GtkColorButton

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

    Srgb color_ = {52, 101, 164};   // a GTK-ish blue, so the swatch shows at once

public:

    std::function<void()> on_click;

    std::function<void(Srgb)> on_color_set;

    GtkColorButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkColorButton";

        text_ = "";

    }

    ~GtkColorButton() {}

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

    //region Colour state

    void set_color(Srgb color)

    {

        color_ = color;

    }

    void set_color_rgb(int r, int g, int b)

    {

        color_ = Srgb{r, g, b};

    }

    Srgb get_color()

    {

        return color_;

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

        if (pressed_)

        {

            unsigned long swap = st.fillColor_;

            st.fillColor_ = st.outlineColor_;

            st.outlineColor_ = swap;

        }

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        // The swatch: an inset rounded rect filled with the colour.
        // With a caption set, the swatch shrinks to a chip at the
        // left and the caption sits beside it.

        int sx1 = x1_ + 5;

        int sy1 = y1_ + 5;

        int sx2 = x2_ - 5;

        int sy2 = y2_ - 5;

        if (!text_.empty())

        {

            sx2 = sx1 + 26;

        }

        SStyle swatch = st;

        swatch.fillColor_ = styles_->get_color_Srgb(color_);

        swatch.radius_ = 3;

        xf::fill_rounded_rect(display_, window_, gc_, swatch, sx1, sy1, sx2, sy2);

        xf::draw_rounded_outline(display_, window_, gc_, swatch, sx1, sy1, sx2, sy2);

        if (!text_.empty())

        {

            xf::draw_text_left(display_, window_, gc_, st, sx2 + 6, y1_, y2_, text_);

        }

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

            // The real button opens a colour chooser here; in this
            // framework the click simply reports the current colour.

            if (on_color_set)

            {

                on_color_set(color_);

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

            if (on_color_set)

            {

                on_color_set(color_);

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
