#pragma once

#include "core/xframe_core.h"

//region GtkColorDialogButton

class GtkColorDialogButton

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

    Srgb color_ = {214, 96, 44};   // a warm orange, distinct from GtkColorButton's blue

public:

    std::function<void()> on_click;

    std::function<void(Srgb)> on_color_set;

    GtkColorDialogButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkColorDialogButton";

        text_ = "";

    }

    ~GtkColorDialogButton() {}

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

        // The GTK4 dialog button is all swatch: the whole face is
        // filled with the current colour, drawn with the polished
        // construction but the colour standing in for the fill.

        SStyle face = st;

        face.fillColor_ = styles_->get_color_Srgb(color_);

        if (!enabled_)

        {

            face.fillColor_ = styles_->get_color_rgb(170, 170, 170);

        }

        if (pressed_)

        {

            unsigned long swap = face.fillColor_;

            face.fillColor_ = face.outlineColor_;

            face.outlineColor_ = swap;

        }

        xf::draw_polished(display_, window_, gc_, face, x1_, y1_, x2_, y2_);

        // A small dropdown triangle at the right, dark on a light
        // colour and white on a dark one, by rough luminance.

        int lum = (color_.r * 299 + color_.g * 587 + color_.b * 114) / 1000;

        unsigned long tri = lum > 140 ? styles_->get_color_rgb(40, 40, 40)

                                      : styles_->get_color_rgb(255, 255, 255);

        int cy = y1_ + height() / 2;

        xf::draw_triangle_down(display_, window_, gc_, tri, x2_ - 12, cy - 3, 4);

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

            // The real button opens a GtkColorDialog here; in this
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
