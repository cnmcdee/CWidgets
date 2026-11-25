#pragma once

#include "core/xframe_core.h"

//region GtkAccelLabel

class GtkAccelLabel

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

    std::string accel_text_ = "Ctrl+S";   // shown right-aligned

    std::string accel_widget_;            // name of the widget it labels

    double xalign_ = 0.0;

public:

    GtkAccelLabel(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkAccelLabel";

        text_ = "Save";

    }

    ~GtkAccelLabel() {}

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

    //region GtkAccelLabel API

    void set_accel_text(std::string t)

    {

        accel_text_ = t;

    }

    std::string get_accel_text()

    {

        return accel_text_;

    }

    void set_accel_widget(std::string widget_name)

    {

        accel_widget_ = widget_name;

    }

    std::string get_accel_widget()

    {

        return accel_widget_;

    }

    void set_xalign(double a)

    {

        xalign_ = std::max(0.0, std::min(1.0, a));

    }

    double get_xalign()

    {

        return xalign_;

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

        // Accelerator first (right aligned), so the label can use the
        // remaining space on the left.

        int aw = xf::text_width(st, accel_text_);

        int ax = x2_ - 2 - aw;

        SStyle dim = st;

        dim.text_color_ = styles_->get_color_rgb(110, 110, 110);

        if (!enabled_)

        {

            dim.text_color_ = styles_->get_color_rgb(160, 160, 160);

        }

        xf::draw_text_at(display_, window_, gc_, dim, ax, y1_ + (height() + (st.font_ ? st.font_->ascent - st.font_->descent : 10)) / 2, accel_text_);

        int label_space = (ax - 6) - (x1_ + 2);

        std::string shown = text_;

        while (!shown.empty() && xf::text_width(st, shown) > label_space)

        {

            shown.pop_back();

        }

        int lx = x1_ + 2 + (int)((label_space - xf::text_width(st, shown)) * xalign_);

        xf::draw_text_left(display_, window_, gc_, st, lx, y1_, y2_, shown);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || button != 1)

        {

            return false;

        }

        focused_ = true;

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
