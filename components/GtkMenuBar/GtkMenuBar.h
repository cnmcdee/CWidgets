#pragma once

#include "core/xframe_core.h"

//region GtkMenuBar

class GtkMenuBar

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

    std::vector<std::vector<std::string>> dropdowns_;

    int open_ = -1;             // top-level index whose menu is dropped

    int hover_top_ = -1;

    int hover_drop_ = -1;

    int drop_row_height_ = 24;

    int item_x(int index)

    {

        SStyle st = style();

        int x = x1_ + 4;

        for (int i = 0; i < index && i < (int)items_.size(); ++i)

        {

            x += xf::text_width(st, items_[i]) + 20;

        }

        return x;

    }

    int item_w(int index)

    {

        SStyle st = style();

        return xf::text_width(st, items_[index]) + 20;

    }

    int top_at(int x)

    {

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            if (x >= item_x(i) && x < item_x(i) + item_w(i))

            {

                return i;

            }

        }

        return -1;

    }

    int drop_width()

    {

        if (open_ < 0 || open_ >= (int)dropdowns_.size())

        {

            return 0;

        }

        SStyle st = style();

        int w = 120;

        for (size_t i = 0; i < dropdowns_[open_].size(); ++i)

        {

            w = std::max(w, xf::text_width(st, dropdowns_[open_][i]) + 28);

        }

        return w;

    }

    int drop_height()

    {

        if (open_ < 0 || open_ >= (int)dropdowns_.size())

        {

            return 0;

        }

        return (int)dropdowns_[open_].size() * drop_row_height_ + 4;

    }

    bool in_dropdown(int x, int y)

    {

        if (open_ < 0)

        {

            return false;

        }

        return x >= item_x(open_) && x < item_x(open_) + drop_width() && y >= y2_ && y < y2_ + drop_height();

    }

public:

    std::function<void(int, int)> on_activate;

    GtkMenuBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkMenuBar";

        text_ = "MenuBar";

        add_item("File", {"New", "Open...", "Save", "Quit"});

        add_item("Edit", {"Undo", "Redo", "Cut", "Copy", "Paste"});

        add_item("View", {"Zoom In", "Zoom Out", "Fullscreen"});

        add_item("Help", {"About"});

    }

    ~GtkMenuBar() {}

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

        if (!visible_)

        {

            return false;

        }

        if (x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_)

        {

            return true;

        }

        // The dropped panel hangs below the bar but still belongs to it.

        return in_dropdown(x, y);

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

    //region Menu bar specific API

    void add_item(std::string label, std::vector<std::string> dropdown)

    {

        items_.push_back(label);

        dropdowns_.push_back(dropdown);

    }

    int item_count()

    {

        return (int)items_.size();

    }

    int get_open()

    {

        return open_;

    }

    void open_menu(int index)

    {

        if (index >= 0 && index < (int)items_.size())

        {

            open_ = index;

        }

    }

    void close_menu()

    {

        open_ = -1;

        hover_drop_ = -1;

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

        unsigned long hot_fill = styles_->get_color_rgb(70, 110, 180);

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            int ix = item_x(i);

            int iw = item_w(i);

            SStyle item_style = st;

            if (i == open_ || i == hover_top_)

            {

                XSetForeground(display_, gc_, hot_fill);

                XFillRectangle(display_, window_, gc_, ix, y1_ + 2, iw, height() - 4);

                item_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            xf::draw_text_centered(display_, window_, gc_, item_style, ix, y1_, ix + iw, y2_, items_[i]);

        }

        // Dropped panel for the open menu.

        if (open_ >= 0)

        {

            int dx = item_x(open_);

            int dy = y2_;

            int dw = drop_width();

            int dh = drop_height();

            xf::draw_polished(display_, window_, gc_, st, dx, dy, dx + dw, dy + dh);

            for (int i = 0; i < (int)dropdowns_[open_].size(); ++i)

            {

                int ry = dy + 2 + i * drop_row_height_;

                SStyle row_style = st;

                if (i == hover_drop_)

                {

                    XSetForeground(display_, gc_, hot_fill);

                    XFillRectangle(display_, window_, gc_, dx + 2, ry, dw - 4, drop_row_height_);

                    row_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

                }

                xf::draw_text_left(display_, window_, gc_, row_style, dx + 12, ry, ry + drop_row_height_, dropdowns_[open_][i]);

            }

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        // A click inside the dropped panel activates its row.

        if (in_dropdown(x, y))

        {

            int row = (y - y2_ - 2) / drop_row_height_;

            if (row >= 0 && row < (int)dropdowns_[open_].size())

            {

                int bar_index = open_;

                close_menu();

                if (on_activate)

                {

                    on_activate(bar_index, row);

                }

            }

            else

            {

                close_menu();

            }

            return true;

        }

        if (!contains(x, y))

        {

            return false;

        }

        int top = top_at(x);

        if (top >= 0)

        {

            open_ = (open_ == top) ? -1 : top;

            hover_drop_ = -1;

            return true;

        }

        close_menu();

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

        bool changed = false;

        if (y >= y1_ && y <= y2_)

        {

            int top = top_at(x);

            if (top != hover_top_)

            {

                hover_top_ = top;

                changed = true;

            }

            // Sliding along the bar with a menu open switches menus.

            if (open_ >= 0 && top >= 0 && top != open_)

            {

                open_ = top;

                hover_drop_ = -1;

                changed = true;

            }

        }

        if (in_dropdown(x, y))

        {

            int row = (y - y2_ - 2) / drop_row_height_;

            if (row != hover_drop_)

            {

                hover_drop_ = row;

                changed = true;

            }

        }

        else if (hover_drop_ != -1)

        {

            hover_drop_ = -1;

            changed = true;

        }

        return changed;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Escape && open_ >= 0)

        {

            close_menu();

            return true;

        }

        if (key == XK_Right && open_ >= 0)

        {

            open_ = (open_ + 1) % (int)items_.size();

            return true;

        }

        if (key == XK_Left && open_ >= 0)

        {

            open_ = (open_ + (int)items_.size() - 1) % (int)items_.size();

            return true;

        }

        return false;

    }

};

//endregion
