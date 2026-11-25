#pragma once

#include "core/xframe_core.h"

//region GtkFlowBox

class GtkFlowBox

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

    int min_child_width_ = 90;

    int min_child_height_ = 64;

    int column_spacing_ = 6;

    int row_spacing_ = 6;

    int selected_index_ = 0;

    int pressed_index_ = -1;

public:

    std::function<void(int)> on_child_activated;

    GtkFlowBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkFlowBox";

        text_ = "FlowBox";

        items_ = { "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight" };

    }

    ~GtkFlowBox() {}

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

    void set_min_child_size(int w, int h)

    {

        min_child_width_ = std::max(16, w);

        min_child_height_ = std::max(16, h);

    }

    int get_min_child_width()

    {

        return min_child_width_;

    }

    int get_min_child_height()

    {

        return min_child_height_;

    }

    void set_spacing(int column_spacing, int row_spacing)

    {

        column_spacing_ = std::max(0, column_spacing);

        row_spacing_ = std::max(0, row_spacing);

    }

    void add_item(std::string label)

    {

        items_.push_back(label);

    }

    int item_count()

    {

        return (int)items_.size();

    }

    int get_selected_index()

    {

        return selected_index_;

    }

    void set_selected_index(int index)

    {

        if (index >= -1 && index < (int)items_.size())

        {

            selected_index_ = index;

        }

    }

    int columns()

    {

        int inner = width() - 12;

        int cols = (inner + column_spacing_) / (min_child_width_ + column_spacing_);

        return std::max(1, cols);

    }

    int rows()

    {

        if (items_.empty())

        {

            return 0;

        }

        return ((int)items_.size() + columns() - 1) / columns();

    }

    // Geometry of tile i: returns false when i is out of range.

    bool child_rect(int i, int& rx1, int& ry1, int& rx2, int& ry2)

    {

        if (i < 0 || i >= (int)items_.size())

        {

            return false;

        }

        int cols = columns();

        int col = i % cols;

        int row = i / cols;

        int tile_w = (width() - 12 - (cols - 1) * column_spacing_) / cols;

        rx1 = x1_ + 6 + col * (tile_w + column_spacing_);

        ry1 = y1_ + 6 + row * (min_child_height_ + row_spacing_);

        rx2 = rx1 + tile_w;

        ry2 = ry1 + min_child_height_;

        return true;

    }

    int index_at(int x, int y)

    {

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            int rx1, ry1, rx2, ry2;

            if (child_rect(i, rx1, ry1, rx2, ry2) && x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2)

            {

                return i;

            }

        }

        return -1;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        unsigned long highlight = styles_->get_color_rgb(70, 120, 200);

        unsigned long dim_text = styles_->get_color_rgb(140, 140, 140);

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            int rx1, ry1, rx2, ry2;

            if (!child_rect(i, rx1, ry1, rx2, ry2))

            {

                continue;

            }

            if (ry1 > y2_ - 6)

            {

                break;

            }

            SStyle tile = st;

            if (!enabled_)

            {

                tile.text_color_ = dim_text;

            }

            if (i == selected_index_)

            {

                tile.fillColor_ = highlight;

                tile.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            if (i == pressed_index_)

            {

                unsigned long swap = tile.fillColor_;

                tile.fillColor_ = tile.outlineColor_;

                tile.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, tile, rx1, ry1, rx2, std::min(ry2, y2_ - 6));

            // Small icon-ish square above the label.

            int cx = (rx1 + rx2) / 2;

            XSetForeground(display_, gc_, tile.lineColor_);

            XDrawRectangle(display_, window_, gc_, cx - 9, ry1 + 8, 18, 14);

            xf::draw_text_centered(display_, window_, gc_, tile, rx1, ry1 + 22, rx2, std::min(ry2, y2_ - 6), items_[i]);

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

        focused_ = true;

        int index = index_at(x, y);

        if (index >= 0)

        {

            selected_index_ = index;

            pressed_index_ = index;

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (pressed_index_ < 0 || button != 1)

        {

            return false;

        }

        int released = pressed_index_;

        pressed_index_ = -1;

        if (index_at(x, y) == released && on_child_activated)

        {

            on_child_activated(released);

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

        if (!focused_ || !enabled_ || items_.empty())

        {

            return false;

        }

        int cols = columns();

        int count = (int)items_.size();

        int next = selected_index_;

        if (key == XK_Right)

        {

            next = std::min(count - 1, selected_index_ + 1);

        }

        else if (key == XK_Left)

        {

            next = std::max(0, selected_index_ - 1);

        }

        else if (key == XK_Down)

        {

            next = std::min(count - 1, selected_index_ + cols);

        }

        else if (key == XK_Up)

        {

            next = std::max(0, selected_index_ - cols);

        }

        else if (key == XK_Return && selected_index_ >= 0)

        {

            if (on_child_activated)

            {

                on_child_activated(selected_index_);

            }

            return true;

        }

        else

        {

            return false;

        }

        selected_index_ = next;

        return true;

    }

};

//endregion
