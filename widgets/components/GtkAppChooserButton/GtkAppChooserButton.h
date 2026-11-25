#pragma once

#include "core/xframe_core.h"

//region GtkAppChooserButton

class GtkAppChooserButton

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

    std::string app_name_ = "Text Editor";

    std::string app_id_ = "org.gnome.TextEditor";

public:

    std::function<void()> on_click;

    std::function<void(std::string)> on_changed;   // hands back the app id

    GtkAppChooserButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkAppChooserButton";

        text_ = app_name_;

    }

    ~GtkAppChooserButton() {}

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

        set_app_name(t);

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region Application state

    void set_selected_app(std::string name, std::string id)

    {

        app_name_ = name;

        app_id_ = id;

        text_ = app_name_;

    }

    void set_app_name(std::string name)

    {

        app_name_ = name;

        text_ = app_name_;

    }

    std::string get_app_name()

    {

        return app_name_;

    }

    void set_app_id(std::string id)

    {

        app_id_ = id;

    }

    std::string get_app_id()

    {

        return app_id_;

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

        int cy = y1_ + height() / 2;

        //region App icon glyph

        // A little window: outline, a filled title bar, two lines of
        // content standing in for the application's own icon.

        int ix = x1_ + 7;

        int iy = cy - 8;

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, ix, iy, 16, 16);

        XSetForeground(display_, gc_, st.outlineColor_);

        XFillRectangle(display_, window_, gc_, ix + 1, iy + 1, 14, 4);

        XSetForeground(display_, gc_, st.text_color_);

        XDrawLine(display_, window_, gc_, ix + 3, iy + 9, ix + 13, iy + 9);

        XDrawLine(display_, window_, gc_, ix + 3, iy + 12, ix + 10, iy + 12);

        //endregion

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 29, y1_, y2_, text_);

        xf::draw_triangle_down(display_, window_, gc_, st.text_color_, x2_ - 11, cy - 3, 4);

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

            if (on_changed)

            {

                on_changed(app_id_);

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

        if (key == XK_Return || key == XK_space || key == XK_Down)

        {

            if (on_changed)

            {

                on_changed(app_id_);

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
