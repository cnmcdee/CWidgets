#pragma once

#include "core/xframe_core.h"

//region GtkToolbar

class GtkToolbar

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

    std::vector<std::string> items_;

    int pressed_ = -1;

    int hover_ = -1;

    int zone_w(int index)

    {

        SStyle st = style();

        return xf::text_width(st, items_[index]) + 34;

    }

    int zone_x(int index)

    {

        int x = x1_ + 3;

        for (int i = 0; i < index; ++i)

        {

            x += zone_w(i);

        }

        return x;

    }

    // How many leading zones fit before the overflow triangle.

    int fitting_count()

    {

        int limit = width() - 26;

        int used = 0;

        int count = 0;

        for (size_t i = 0; i < items_.size(); ++i)

        {

            if (used + zone_w((int)i) > limit && count > 0)

            {

                break;

            }

            used += zone_w((int)i);

            count++;

        }

        return count;

    }

    int zone_at(int x)

    {

        int count = fitting_count();

        for (int i = 0; i < count; ++i)

        {

            if (x >= zone_x(i) && x < zone_x(i) + zone_w(i))

            {

                return i;

            }

        }

        return -1;

    }

public:

    std::function<void(int)> on_activate;

    GtkToolbar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkToolbar";

        text_ = "Toolbar";

        add_item("New");

        add_item("Open");

        add_item("Save");

        add_item("Cut");

        add_item("Copy");

        add_item("Paste");

        add_item("Undo");

        add_item("Redo");

        add_item("Print");

        add_item("Help");

    }

    ~GtkToolbar() {}

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

    //region Toolbar specific API

    void add_item(std::string label)

    {

        items_.push_back(label);

    }

    void clear_items()

    {

        items_.clear();

        pressed_ = -1;

        hover_ = -1;

    }

    int item_count()

    {

        return (int)items_.size();

    }

    bool has_overflow()

    {

        return fitting_count() < (int)items_.size();

    }

    int visible_item_count()

    {

        return fitting_count();

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        unsigned long hot_fill = styles_->get_color_rgb(205, 218, 238);

        unsigned long icon_fill = styles_->get_color_rgb(120, 160, 210);

        int count = fitting_count();

        for (int i = 0; i < count; ++i)

        {

            int zx = zone_x(i);

            int zw = zone_w(i);

            if (i == pressed_ || i == hover_)

            {

                XSetForeground(display_, gc_, hot_fill);

                XFillRectangle(display_, window_, gc_, zx + 1, y1_ + 3, zw - 2, height() - 6);

                XSetForeground(display_, gc_, st.lineColor_);

                XDrawRectangle(display_, window_, gc_, zx + 1, y1_ + 3, zw - 2, height() - 6);

            }

            // Little icon square, then the label.

            int iy = (y1_ + y2_) / 2 - 6;

            SStyle icon_style = st;

            icon_style.fillColor_ = icon_fill;

            icon_style.radius_ = 3;

            xf::fill_rounded_rect(display_, window_, gc_, icon_style, zx + 5, iy, zx + 17, iy + 12);

            xf::draw_text_left(display_, window_, gc_, st, zx + 22, y1_, y2_, items_[i]);

        }

        // Overflow indicator when not everything fits.

        if (has_overflow())

        {

            xf::draw_triangle_right(display_, window_, gc_, st.text_color_, x2_ - 14, (y1_ + y2_) / 2, 5);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        pressed_ = zone_at(x);

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (button != 1 || pressed_ < 0)

        {

            return false;

        }

        int released_on = zone_at(x);

        int was = pressed_;

        pressed_ = -1;

        if (released_on == was && contains(x, y) && on_activate)

        {

            on_activate(was);

        }

        return true;

    }

    bool on_motion(int x, int y)

    {

        int zone = contains(x, y) ? zone_at(x) : -1;

        if (zone != hover_)

        {

            hover_ = zone;

            return true;

        }

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
