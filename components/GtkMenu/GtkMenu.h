#pragma once

#include "core/xframe_core.h"

//region GtkMenu

class GtkMenu

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

    // Item kinds.

    static const int kNormal = 0;

    static const int kSeparator = 1;

    static const int kDisabled = 2;

    static const int kSubmenu = 3;

    struct Entry

    {

        std::string label;

        int kind;

    };

    std::vector<Entry> items_;

    int hover_ = -1;

    int row_height_ = 24;

    int item_at(int y)

    {

        if (y < y1_ + 2)

        {

            return -1;

        }

        int index = (y - y1_ - 2) / row_height_;

        if (index < 0 || index >= (int)items_.size())

        {

            return -1;

        }

        return index;

    }

public:

    std::function<void(int)> on_activate;

    GtkMenu(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkMenu";

        text_ = "Menu";

        add_item("New Window");

        add_item("Open...");

        add_item("Save");

        add_separator();

        add_submenu_item("Recent Files");

        add_disabled_item("Print...");

        add_separator();

        add_item("Quit");

    }

    ~GtkMenu() {}

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

    //region Menu specific API

    void add_item(std::string label)

    {

        Entry e;

        e.label = label;

        e.kind = kNormal;

        items_.push_back(e);

    }

    void add_submenu_item(std::string label)

    {

        Entry e;

        e.label = label;

        e.kind = kSubmenu;

        items_.push_back(e);

    }

    void add_disabled_item(std::string label)

    {

        Entry e;

        e.label = label;

        e.kind = kDisabled;

        items_.push_back(e);

    }

    void add_separator()

    {

        Entry e;

        e.label = "";

        e.kind = kSeparator;

        items_.push_back(e);

    }

    void clear_items()

    {

        items_.clear();

        hover_ = -1;

    }

    int item_count()

    {

        return (int)items_.size();

    }

    std::string get_item_label(int index)

    {

        if (index >= 0 && index < (int)items_.size())

        {

            return items_[index].label;

        }

        return "";

    }

    int get_hover()

    {

        return hover_;

    }

    int content_height()

    {

        return (int)items_.size() * row_height_ + 4;

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

        unsigned long hover_fill = styles_->get_color_rgb(70, 110, 180);

        unsigned long grey = styles_->get_color_rgb(150, 150, 150);

        unsigned long sep = styles_->get_color_rgb(190, 190, 190);

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            int ry = y1_ + 2 + i * row_height_;

            if (ry + row_height_ > y2_)

            {

                break;

            }

            Entry& e = items_[i];

            if (e.kind == kSeparator)

            {

                XSetForeground(display_, gc_, sep);

                XDrawLine(display_, window_, gc_, x1_ + 8, ry + row_height_ / 2, x2_ - 8, ry + row_height_ / 2);

                continue;

            }

            SStyle row_style = st;

            if (i == hover_ && e.kind != kDisabled)

            {

                XSetForeground(display_, gc_, hover_fill);

                XFillRectangle(display_, window_, gc_, x1_ + 2, ry, width() - 4, row_height_);

                row_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            if (e.kind == kDisabled)

            {

                row_style.text_color_ = grey;

            }

            xf::draw_text_left(display_, window_, gc_, row_style, x1_ + 12, ry, ry + row_height_, e.label);

            if (e.kind == kSubmenu)

            {

                xf::draw_triangle_right(display_, window_, gc_, row_style.text_color_, x2_ - 16, ry + row_height_ / 2, 4);

            }

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

        int index = item_at(y);

        if (index < 0)

        {

            return true;

        }

        Entry& e = items_[index];

        if (e.kind == kNormal || e.kind == kSubmenu)

        {

            hover_ = index;

            if (on_activate)

            {

                on_activate(index);

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

        if (!contains(x, y))

        {

            if (hover_ != -1)

            {

                hover_ = -1;

                return true;

            }

            return false;

        }

        int index = item_at(y);

        if (index != hover_)

        {

            hover_ = index;

            return true;

        }

        return false;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_ || items_.empty())

        {

            return false;

        }

        if (key == XK_Down || key == XK_Up)

        {

            int next = hover_;

            for (int step = 0; step < (int)items_.size(); ++step)

            {

                next += (key == XK_Down ? 1 : -1);

                if (next < 0)

                {

                    next = (int)items_.size() - 1;

                }

                if (next >= (int)items_.size())

                {

                    next = 0;

                }

                if (items_[next].kind == kNormal || items_[next].kind == kSubmenu)

                {

                    break;

                }

            }

            hover_ = next;

            return true;

        }

        if (key == XK_Return && hover_ >= 0)

        {

            if (on_activate)

            {

                on_activate(hover_);

            }

            return true;

        }

        return false;

    }

};

//endregion
