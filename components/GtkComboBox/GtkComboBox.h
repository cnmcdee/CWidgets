#pragma once

#include "core/xframe_core.h"

//region GtkComboBox

class GtkComboBox

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;   // mirrors the active item's text

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    std::vector<std::string> items_;

    int active_ = -1;

    bool open_ = false;       // dropdown list is showing

    int hover_ = -1;          // row under the mouse while open

    int top_row_ = 0;         // scroll offset of the open list

    bool pressed_ = false;

    int row_height()

    {

        return 20;

    }

    int max_rows()

    {

        return 8;

    }

    int list_y1()

    {

        return y2_ + 2;

    }

    int shown_rows()

    {

        return std::min(max_rows(), (int)items_.size());

    }

    void keep_active_visible()

    {

        if (active_ < top_row_)

        {

            top_row_ = std::max(0, active_);

        }

        if (active_ >= top_row_ + max_rows())

        {

            top_row_ = active_ - max_rows() + 1;

        }

    }

    void select_index(int i)

    {

        if (i < 0 || i >= (int)items_.size())

        {

            return;

        }

        bool was = (active_ != i);

        active_ = i;

        text_ = items_[(size_t)i];

        open_ = false;

        if (was && on_changed)

        {

            on_changed();

        }

    }

public:

    std::function<void()> on_changed;

    GtkComboBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkComboBox";

        items_ = {"Red", "Green", "Blue", "Orange", "Purple", "Teal", "Magenta", "Brown", "Black", "Slate"};

        active_ = 2;

        text_ = items_[(size_t)active_];

    }

    ~GtkComboBox() {}

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

        // While open, the dropdown list is part of the widget.

        if (open_ && x >= x1_ && x <= x2_ && y >= list_y1() && y <= list_y1() + shown_rows() * row_height())

        {

            return true;

        }

        return false;

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

        if (!e)

        {

            open_ = false;

        }

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

        set_active_text(t);

    }

    std::string get_text()

    {

        return get_active_text();

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region GtkComboBox specific API

    void add_item(std::string item)

    {

        items_.push_back(item);

        if (active_ < 0)

        {

            active_ = 0;

            text_ = item;

        }

    }

    void remove_item(int index)

    {

        if (index < 0 || index >= (int)items_.size())

        {

            return;

        }

        items_.erase(items_.begin() + index);

        if (active_ >= (int)items_.size())

        {

            active_ = (int)items_.size() - 1;

        }

        text_ = active_ >= 0 ? items_[(size_t)active_] : "";

    }

    void clear_items()

    {

        items_.clear();

        active_ = -1;

        text_ = "";

        top_row_ = 0;

    }

    int get_item_count()

    {

        return (int)items_.size();

    }

    std::string get_item_text(int index)

    {

        if (index < 0 || index >= (int)items_.size())

        {

            return "";

        }

        return items_[(size_t)index];

    }

    void set_active(int index)

    {

        if (index >= -1 && index < (int)items_.size())

        {

            active_ = index;

            text_ = index >= 0 ? items_[(size_t)index] : "";

        }

    }

    int get_active()

    {

        return active_;

    }

    std::string get_active_text()

    {

        if (active_ < 0 || active_ >= (int)items_.size())

        {

            return "";

        }

        return items_[(size_t)active_];

    }

    void set_active_text(std::string t)

    {

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            if (items_[(size_t)i] == t)

            {

                active_ = i;

                text_ = t;

                return;

            }

        }

    }

    bool get_popup_open()

    {

        return open_;

    }

    void popup()

    {

        if (enabled_ && !items_.empty())

        {

            open_ = true;

            keep_active_visible();

        }

    }

    void popdown()

    {

        open_ = false;

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

        if (pressed_ || open_)

        {

            unsigned long swap = st.fillColor_;

            st.fillColor_ = st.outlineColor_;

            st.outlineColor_ = swap;

        }

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        // Active item text, left aligned.

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 8, y1_, y2_, get_active_text());

        // Separator + down triangle zone.

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x2_ - 24, y1_ + 3, x2_ - 24, y2_ - 3);

        xf::draw_triangle_down(display_, window_, gc_, st.text_color_, x2_ - 12, (y1_ + y2_) / 2 - 3, 6);

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 3, y1_ + 3, width() - 6, height() - 6);

        }

        // The open dropdown list, drawn below the button.

        if (open_)

        {

            int ly = list_y1();

            int lh = shown_rows() * row_height();

            XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

            XFillRectangle(display_, window_, gc_, x1_, ly, width(), lh);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_, ly, width(), lh);

            for (int row = 0; row < shown_rows(); ++row)

            {

                int idx = top_row_ + row;

                if (idx >= (int)items_.size())

                {

                    break;

                }

                int ry = ly + row * row_height();

                if (idx == hover_ || idx == active_)

                {

                    XSetForeground(display_, gc_, styles_->get_color_rgb(195, 215, 245));

                    XFillRectangle(display_, window_, gc_, x1_ + 1, ry + 1, width() - 2, row_height() - 1);

                }

                xf::draw_text_left(display_, window_, gc_, st, x1_ + 8, ry, ry + row_height(), items_[(size_t)idx]);

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

        // A row of the open list?

        if (open_ && x >= x1_ && x <= x2_ && y >= list_y1() && y <= list_y1() + shown_rows() * row_height())

        {

            int idx = top_row_ + (y - list_y1()) / row_height();

            select_index(idx);

            return true;

        }

        if (x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_)

        {

            focused_ = true;

            pressed_ = true;

            open_ = !open_;

            if (open_)

            {

                keep_active_visible();

            }

            return true;

        }

        if (open_)

        {

            open_ = false;   // click-away closes the popup

        }

        return false;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button != 1 || !pressed_)

        {

            return false;

        }

        pressed_ = false;

        return true;

    }

    bool on_motion(int x, int y)

    {

        if (!open_)

        {

            return false;

        }

        if (x >= x1_ && x <= x2_ && y >= list_y1() && y <= list_y1() + shown_rows() * row_height())

        {

            int idx = top_row_ + (y - list_y1()) / row_height();

            if (idx != hover_)

            {

                hover_ = idx;

                return true;

            }

        }

        return false;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Down)

        {

            if (!open_)

            {

                popup();

            }

            else if (active_ + 1 < (int)items_.size())

            {

                select_index(active_ + 1);

                open_ = true;

            }

            return true;

        }

        if (key == XK_Up)

        {

            if (open_ && active_ > 0)

            {

                select_index(active_ - 1);

                open_ = true;

            }

            return true;

        }

        if (key == XK_Return || key == XK_KP_Enter || key == XK_space)

        {

            open_ = !open_;

            if (open_)

            {

                keep_active_visible();

            }

            return true;

        }

        if (key == XK_Escape)

        {

            open_ = false;

            return true;

        }

        return false;

    }

};

//endregion
