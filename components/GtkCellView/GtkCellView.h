#pragma once

#include "core/xframe_core.h"

//region GtkCellView

class GtkCellView

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;   // the row's text cell

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    bool has_pixmap_ = true;      // draw the pixmap cell

    Srgb pixmap_color_ = {90, 140, 200};

    bool draw_sensitive_ = true;  // false = greyed, insensitive look

    bool bg_set_ = false;         // a background colour is set

    Srgb bg_color_ = {255, 255, 255};

    int spacing_ = 6;

public:

    std::function<void()> on_click;

    GtkCellView(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkCellView";

        text_ = "Cell row: icon and text";

    }

    ~GtkCellView() {}

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

    //region GtkCellView specific API

    void set_pixmap_visible(bool v)

    {

        has_pixmap_ = v;

    }

    bool get_pixmap_visible()

    {

        return has_pixmap_;

    }

    void set_pixmap_color(int r, int g, int b)

    {

        pixmap_color_ = {r, g, b};

    }

    void set_draw_sensitive(bool sensitive)

    {

        draw_sensitive_ = sensitive;

    }

    bool get_draw_sensitive()

    {

        return draw_sensitive_;

    }

    void set_background_color(int r, int g, int b)

    {

        bg_set_ = true;

        bg_color_ = {r, g, b};

    }

    void clear_background_color()

    {

        bg_set_ = false;

    }

    bool get_background_set()

    {

        return bg_set_;

    }

    void set_spacing(int px)

    {

        spacing_ = px;

    }

    int get_spacing()

    {

        return spacing_;

    }

    // X of the text cell, after the pixmap cell and spacing.

    int text_left()

    {

        int tx = x1_ + 4;

        if (has_pixmap_)

        {

            tx += 18 + spacing_;

        }

        return tx;

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // Background: the set colour wins, otherwise the style fill.

        unsigned long bg = bg_set_ ? styles_->get_color_Srgb(bg_color_) : st.fillColor_;

        XSetForeground(display_, gc_, bg);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        }

        int cy = (y1_ + y2_) / 2;

        // Pixmap cell: a small two-tone picture box.

        if (has_pixmap_)

        {

            int bx = x1_ + 4;

            int by = cy - 8;

            unsigned long pc = styles_->get_color_Srgb(pixmap_color_);

            if (!draw_sensitive_ || !enabled_)

            {

                pc = styles_->get_color_rgb(170, 170, 170);

            }

            XSetForeground(display_, gc_, pc);

            XFillRectangle(display_, window_, gc_, bx, by, 16, 16);

            // A lighter "sky" band and a sun dot, like a tiny photo.

            XSetForeground(display_, gc_, styles_->get_color_rgb(225, 235, 245));

            XFillRectangle(display_, window_, gc_, bx, by, 16, 6);

            XSetForeground(display_, gc_, styles_->get_color_rgb(240, 200, 60));

            XFillArc(display_, window_, gc_, bx + 10, by + 2, 4, 4, 0, 360 * 64);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, bx, by, 16, 16);

        }

        // Text cell.

        SStyle ink = st;

        if (!draw_sensitive_ || !enabled_)

        {

            ink.text_color_ = styles_->get_color_rgb(150, 150, 150);

        }

        xf::draw_text_left(display_, window_, gc_, ink, text_left(), y1_, y2_, text_);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

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
