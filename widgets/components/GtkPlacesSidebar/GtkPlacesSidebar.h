#pragma once

#include "core/xframe_core.h"

//region GtkPlacesSidebar

class GtkPlacesSidebar

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

    struct Place

    {

        std::string name;

        std::string glyph;

    };

    std::vector<Place> places_;

    int selected_ = 0;

    int hover_ = -1;

    int row_height_ = 28;

    int row_at(int y)

    {

        if (y < y1_ + 2)

        {

            return -1;

        }

        int index = (y - y1_ - 2) / row_height_;

        if (index < 0 || index >= (int)places_.size())

        {

            return -1;

        }

        return index;

    }

public:

    std::function<void(std::string)> on_place_selected;

    GtkPlacesSidebar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkPlacesSidebar";

        text_ = "Places";

        add_place("Home", "H");

        add_place("Desktop", "D");

        add_place("Documents", "Dc");

        add_place("Downloads", "Dn");

        add_place("Music", "M");

        add_place("Pictures", "P");

        add_place("Videos", "V");

        add_place("Trash", "T");

    }

    ~GtkPlacesSidebar() {}

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

    //region Places specific API

    void add_place(std::string place_name, std::string glyph)

    {

        Place p;

        p.name = place_name;

        p.glyph = glyph;

        places_.push_back(p);

    }

    int place_count()

    {

        return (int)places_.size();

    }

    int get_selected()

    {

        return selected_;

    }

    std::string get_selected_place()

    {

        if (selected_ >= 0 && selected_ < (int)places_.size())

        {

            return places_[selected_].name;

        }

        return "";

    }

    void select_place(std::string place_name)

    {

        for (size_t i = 0; i < places_.size(); ++i)

        {

            if (places_[i].name == place_name)

            {

                selected_ = (int)i;

                return;

            }

        }

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

        unsigned long sel_fill = styles_->get_color_rgb(70, 110, 180);

        unsigned long hov_fill = styles_->get_color_rgb(225, 232, 245);

        unsigned long icon_fill = styles_->get_color_rgb(120, 160, 210);

        for (int i = 0; i < (int)places_.size(); ++i)

        {

            int ry = y1_ + 2 + i * row_height_;

            if (ry + row_height_ > y2_ - 2)

            {

                break;

            }

            SStyle row_style = st;

            if (i == selected_)

            {

                XSetForeground(display_, gc_, sel_fill);

                XFillRectangle(display_, window_, gc_, x1_ + 2, ry, width() - 4, row_height_);

                row_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            else if (i == hover_)

            {

                XSetForeground(display_, gc_, hov_fill);

                XFillRectangle(display_, window_, gc_, x1_ + 2, ry, width() - 4, row_height_);

            }

            // Icon box with the place glyph.

            int ix = x1_ + 8;

            int iy = ry + (row_height_ - 18) / 2;

            SStyle icon_style = st;

            icon_style.fillColor_ = icon_fill;

            icon_style.radius_ = 3;

            xf::fill_rounded_rect(display_, window_, gc_, icon_style, ix, iy, ix + 18, iy + 18);

            xf::draw_rounded_outline(display_, window_, gc_, icon_style, ix, iy, ix + 18, iy + 18);

            SStyle glyph_style = st;

            glyph_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

            xf::draw_text_centered(display_, window_, gc_, glyph_style, ix, iy, ix + 18, iy + 18, places_[i].glyph);

            xf::draw_text_left(display_, window_, gc_, row_style, x1_ + 34, ry, ry + row_height_, places_[i].name);

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

        int row = row_at(y);

        if (row >= 0 && row != selected_)

        {

            selected_ = row;

            if (on_place_selected)

            {

                on_place_selected(places_[selected_].name);

            }

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

        int row = contains(x, y) ? row_at(y) : -1;

        if (row != hover_)

        {

            hover_ = row;

            return true;

        }

        return false;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_ || places_.empty())

        {

            return false;

        }

        if (key == XK_Down && selected_ < (int)places_.size() - 1)

        {

            selected_++;

        }

        else if (key == XK_Up && selected_ > 0)

        {

            selected_--;

        }

        else

        {

            return false;

        }

        if (on_place_selected)

        {

            on_place_selected(places_[selected_].name);

        }

        return true;

    }

};

//endregion
