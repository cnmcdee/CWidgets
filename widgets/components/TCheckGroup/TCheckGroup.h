#pragma once

#include "core/xframe_core.h"

//region TCheckGroup

class TCheckGroup

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

    std::vector<bool> checked_;

    int columns_ = 2;

    int item_height_ = 24;

    int selected_index_ = 0;

    bool item_rect(int index, int &rx, int &ry, int &rw, int &rh)

    {

        if (index < 0 || index >= (int)items_.size())

        {

            return false;

        }

        int cols = columns_;

        if (cols < 1)

        {

            cols = 1;

        }

        int col_w = (width() - 20) / cols;

        if (col_w < 20)

        {

            col_w = 20;

        }

        int col = index % cols;

        int row = index / cols;

        rx = x1_ + 10 + col * col_w;

        ry = y1_ + 26 + row * item_height_;

        rw = col_w - 6;

        rh = item_height_;

        return true;

    }

public:

    std::function<void(int)> on_item_change;

    TCheckGroup(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TCheckGroup";

        text_ = "Check Group";

        items_.push_back("Apples");

        checked_.push_back(true);

        items_.push_back("Oranges");

        checked_.push_back(false);

        items_.push_back("Pears");

        checked_.push_back(true);

        items_.push_back("Grapes");

        checked_.push_back(false);

        items_.push_back("Melons");

        checked_.push_back(false);

        items_.push_back("Berries");

        checked_.push_back(true);

    }

    ~TCheckGroup() {}

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

    //region Items

    void add_item(std::string caption, bool checked = false)

    {

        items_.push_back(caption);

        checked_.push_back(checked);

    }

    void clear_items()

    {

        items_.clear();

        checked_.clear();

        selected_index_ = 0;

    }

    int item_count()

    {

        return (int)items_.size();

    }

    std::string get_item(int index)

    {

        if (index < 0 || index >= (int)items_.size())

        {

            return "";

        }

        return items_[index];

    }

    void set_item(int index, std::string caption)

    {

        if (index < 0 || index >= (int)items_.size())

        {

            return;

        }

        items_[index] = caption;

    }

    bool get_checked(int index)

    {

        if (index < 0 || index >= (int)checked_.size())

        {

            return false;

        }

        return checked_[index];

    }

    void set_checked(int index, bool value)

    {

        if (index < 0 || index >= (int)checked_.size())

        {

            return;

        }

        checked_[index] = value;

    }

    int checked_count()

    {

        int n = 0;

        for (size_t i = 0; i < checked_.size(); ++i)

        {

            if (checked_[i])

            {

                ++n;

            }

        }

        return n;

    }

    void set_columns(int c)

    {

        if (c < 1)

        {

            c = 1;

        }

        columns_ = c;

    }

    int get_columns()

    {

        return columns_;

    }

    void set_item_height(int h)

    {

        if (h >= 16)

        {

            item_height_ = h;

        }

    }

    int get_item_height()

    {

        return item_height_;

    }

    int index_at(int x, int y)

    {

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            int rx = 0;

            int ry = 0;

            int rw = 0;

            int rh = 0;

            if (!item_rect(i, rx, ry, rw, rh))

            {

                continue;

            }

            if (x >= rx && x <= rx + rw && y >= ry && y <= ry + rh)

            {

                return i;

            }

        }

        return -1;

    }

    bool item_bounds(int index, int &rx, int &ry, int &rw, int &rh)

    {

        return item_rect(index, rx, ry, rw, rh);

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

        // Group box body: fill, then a square-ish frame whose top
        // edge sits below the caption band.

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        xf::set_line_width(display_, gc_, (int)st.lineWidth_);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_ + 8, width() - 1, height() - 9);

        // Caption: paint over the top frame line, then draw the text.

        if (!text_.empty() && st.font_)

        {

            int tw = xf::text_width(st, text_);

            XSetForeground(display_, gc_, st.fillColor_);

            XFillRectangle(display_, window_, gc_, x1_ + 8, y1_, tw + 8, 16);

            xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y1_, y1_ + 16, text_);

        }

        // Items, laid out in columns.

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            int rx = 0;

            int ry = 0;

            int rw = 0;

            int rh = 0;

            if (!item_rect(i, rx, ry, rw, rh))

            {

                continue;

            }

            if (ry + rh > y2_)

            {

                continue;

            }

            int box = 14;

            int bx = rx + 2;

            int by = ry + (rh - box) / 2;

            XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

            XFillRectangle(display_, window_, gc_, bx, by, box, box);

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, 1);

            XDrawRectangle(display_, window_, gc_, bx, by, box, box);

            if (checked_[i])

            {

                unsigned long mark = enabled_ ? st.text_color_ : styles_->get_color_rgb(140, 140, 140);

                xf::draw_checkmark(display_, window_, gc_, mark, bx + box / 2, by + box / 2 - 1, box - 2);

            }

            if (focused_ && enabled_ && i == selected_index_)

            {

                XSetForeground(display_, gc_, st.lineColor_);

                XDrawRectangle(display_, window_, gc_, rx, ry + 2, rw - 1, rh - 5);

            }

            xf::draw_text_left(display_, window_, gc_, st, bx + box + 6, ry, ry + rh, items_[i]);

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

        if (index < 0)

        {

            return true;

        }

        selected_index_ = index;

        checked_[index] = !checked_[index];

        if (on_item_change)

        {

            on_item_change(index);

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

        int cols = columns_;

        if (cols < 1)

        {

            cols = 1;

        }

        if (key == XK_Down)

        {

            selected_index_ += cols;

            if (selected_index_ >= (int)items_.size())

            {

                selected_index_ = (int)items_.size() - 1;

            }

            return true;

        }

        if (key == XK_Up)

        {

            selected_index_ -= cols;

            if (selected_index_ < 0)

            {

                selected_index_ = 0;

            }

            return true;

        }

        if (key == XK_Right)

        {

            if (selected_index_ + 1 < (int)items_.size())

            {

                ++selected_index_;

            }

            return true;

        }

        if (key == XK_Left)

        {

            if (selected_index_ > 0)

            {

                --selected_index_;

            }

            return true;

        }

        if (key == XK_space || key == XK_Return)

        {

            if (selected_index_ >= 0 && selected_index_ < (int)checked_.size())

            {

                checked_[selected_index_] = !checked_[selected_index_];

                if (on_item_change)

                {

                    on_item_change(selected_index_);

                }

            }

            return true;

        }

        return false;

    }

};

//endregion
