#pragma once

#include "core/xframe_core.h"

//region GtkIconView

class GtkIconView

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

    std::vector<bool> selected_;

    int scroll_row_ = 0;

    int tile_w_ = 92;

    int tile_h_ = 84;

    int columns()

    {

        return std::max(1, (width() - 4) / tile_w_);

    }

    int tile_rows_visible()

    {

        return std::max(1, (height() - 4) / tile_h_);

    }

public:

    std::function<void()> on_selection_changed;

    std::function<void()> on_item_activated;

    GtkIconView(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkIconView";

        text_ = "IconView";

        const char *sample[] = {"beach.png", "city.jpg", "forest.png", "lake.jpg", "mountain.png", "river.jpg", "sunset.png", "valley.jpg", "desert.png", "ocean.jpg"};

        for (int i = 0; i < 10; ++i)

        {

            add_item(sample[i]);

        }

        if (!selected_.empty())

        {

            selected_[0] = true;

        }

    }

    ~GtkIconView() {}

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

    //region Icon view specific API

    void add_item(std::string item)

    {

        items_.push_back(item);

        selected_.push_back(false);

    }

    void clear_items()

    {

        items_.clear();

        selected_.clear();

        scroll_row_ = 0;

    }

    int item_count()

    {

        return (int)items_.size();

    }

    std::vector<int> get_selected_items()

    {

        std::vector<int> out;

        for (size_t i = 0; i < selected_.size(); ++i)

        {

            if (selected_[i])

            {

                out.push_back((int)i);

            }

        }

        return out;

    }

    std::string get_selected_text()

    {

        std::vector<int> sel = get_selected_items();

        if (!sel.empty())

        {

            return items_[sel[0]];

        }

        return "";

    }

    void select_item(int index)

    {

        for (size_t i = 0; i < selected_.size(); ++i)

        {

            selected_[i] = false;

        }

        if (index >= 0 && index < (int)selected_.size())

        {

            selected_[index] = true;

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

        int cols = columns();

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            int row = i / cols;

            int col = i % cols;

            if (row < scroll_row_)

            {

                continue;

            }

            int tx = x1_ + 2 + col * tile_w_;

            int ty = y1_ + 2 + (row - scroll_row_) * tile_h_;

            if (ty + tile_h_ > y2_ - 2)

            {

                break;

            }

            if (selected_[i])

            {

                XSetForeground(display_, gc_, sel_fill);

                XFillRectangle(display_, window_, gc_, tx + 2, ty + 2, tile_w_ - 4, tile_h_ - 4);

            }

            // Picture glyph: a framed box with a sun dot and hills.

            int ix = tx + (tile_w_ - 48) / 2;

            int iy = ty + 6;

            unsigned long sky = styles_->get_color_rgb(170, 205, 240);

            unsigned long hill = styles_->get_color_rgb(110, 170, 90);

            unsigned long sun = styles_->get_color_rgb(245, 200, 60);

            XSetForeground(display_, gc_, sky);

            XFillRectangle(display_, window_, gc_, ix, iy, 48, 40);

            XSetForeground(display_, gc_, sun);

            XFillArc(display_, window_, gc_, ix + 34, iy + 5, 9, 9, 0, 360 * 64);

            XSetForeground(display_, gc_, hill);

            XFillArc(display_, window_, gc_, ix - 6, iy + 24, 34, 22, 0, 180 * 64);

            XFillArc(display_, window_, gc_, ix + 20, iy + 26, 34, 20, 0, 180 * 64);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, ix, iy, 48, 40);

            SStyle label_style = st;

            if (selected_[i])

            {

                label_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            xf::draw_text_centered(display_, window_, gc_, label_style, tx + 2, ty + 50, tx + tile_w_ - 2, ty + tile_h_ - 4, items_[i]);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_)

        {

            return false;

        }

        focused_ = true;

        if (button == 4)

        {

            scroll_row_ = std::max(0, scroll_row_ - 1);

            return true;

        }

        if (button == 5)

        {

            scroll_row_ += 1;

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        int col = (x - x1_ - 2) / tile_w_;

        int row = scroll_row_ + (y - y1_ - 2) / tile_h_;

        int index = row * columns() + col;

        if (col >= 0 && col < columns() && index >= 0 && index < (int)items_.size())

        {

            bool was_selected = selected_[index];

            select_item(index);

            if (was_selected && on_item_activated)

            {

                // A second click on the same tile activates it.

                on_item_activated();

            }

            if (on_selection_changed)

            {

                on_selection_changed();

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

        std::vector<int> sel = get_selected_items();

        int current = sel.empty() ? 0 : sel[0];

        int cols = columns();

        int next = current;

        if (key == XK_Right)

        {

            next = std::min((int)items_.size() - 1, current + 1);

        }

        else if (key == XK_Left)

        {

            next = std::max(0, current - 1);

        }

        else if (key == XK_Down)

        {

            next = std::min((int)items_.size() - 1, current + cols);

        }

        else if (key == XK_Up)

        {

            next = std::max(0, current - cols);

        }

        else if (key == XK_Return)

        {

            if (on_item_activated)

            {

                on_item_activated();

            }

            return true;

        }

        else

        {

            return false;

        }

        select_item(next);

        int row = next / cols;

        if (row < scroll_row_)

        {

            scroll_row_ = row;

        }

        if (row >= scroll_row_ + tile_rows_visible())

        {

            scroll_row_ = row - tile_rows_visible() + 1;

        }

        if (on_selection_changed)

        {

            on_selection_changed();

        }

        return true;

    }

};

//endregion
