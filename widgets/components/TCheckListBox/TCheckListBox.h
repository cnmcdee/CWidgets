#pragma once

#include "core/xframe_core.h"

//region TCheckListBox

class TCheckListBox

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

    std::string style_name_;

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    std::vector<std::string> items_;

    std::vector<bool> checked_;

    int item_index_ = -1;

    int top_index_ = 0;

    int row_height_ = 18;

    int visible_rows()

    {

        int h = height() - 4;

        if (h < row_height_)

        {

            return 1;

        }

        return h / row_height_;

    }

    void ensure_visible(int idx)

    {

        int vr = visible_rows();

        if (idx < top_index_)

        {

            top_index_ = idx;

        }

        if (idx >= top_index_ + vr)

        {

            top_index_ = idx - vr + 1;

        }

        if (top_index_ < 0)

        {

            top_index_ = 0;

        }

    }

public:

    std::function<void()> on_click;

    std::function<void(int)> on_click_check;

    TCheckListBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TCheckListBox";

        text_ = "TCheckListBox";

        add_item("Apples", true);

        add_item("Bread", false);

        add_item("Cheese", true);

        add_item("Eggs", false);

        add_item("Milk", false);

        add_item("Coffee", false);

        item_index_ = 0;

    }

    ~TCheckListBox() {}

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

        if (item_index_ >= 0 && item_index_ < (int)items_.size())

        {

            return items_[item_index_];

        }

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region Specific API

    void add_item(std::string text, bool checked = false)

    {

        items_.push_back(text);

        checked_.push_back(checked);

    }

    void set_checked(int i, bool v)

    {

        if (i >= 0 && i < (int)checked_.size())

        {

            checked_[i] = v;

        }

    }

    bool get_checked(int i)

    {

        if (i >= 0 && i < (int)checked_.size())

        {

            return checked_[i];

        }

        return false;

    }

    int get_item_index()

    {

        return item_index_;

    }

    void set_item_index(int i)

    {

        if (i >= -1 && i < (int)items_.size())

        {

            item_index_ = i;

            if (i >= 0)

            {

                ensure_visible(i);

            }

        }

    }

    int item_count()

    {

        return (int)items_.size();

    }

    std::string get_item(int i)

    {

        if (i >= 0 && i < (int)items_.size())

        {

            return items_[i];

        }

        return "";

    }

    void clear_items()

    {

        items_.clear();

        checked_.clear();

        item_index_ = -1;

        top_index_ = 0;

    }

    int get_top_index()

    {

        return top_index_;

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        unsigned long white = styles_->get_color_rgb(255, 255, 255);

        XSetForeground(display_, gc_, white);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        int vr = visible_rows();

        for (int row = 0; row < vr; ++row)

        {

            int idx = top_index_ + row;

            if (idx < 0 || idx >= (int)items_.size())

            {

                break;

            }

            int ry1 = y1_ + 2 + row * row_height_;

            int ry2 = ry1 + row_height_;

            bool selected = (idx == item_index_);

            if (selected)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, x1_ + 1, ry1, width() - 2, row_height_);

            }

            int bx = x1_ + 4;

            int by = ry1 + 2;

            int bs = row_height_ - 5;

            if (bs < 10)

            {

                bs = 10;

            }

            XSetForeground(display_, gc_, white);

            XFillRectangle(display_, window_, gc_, bx, by, bs, bs);

            XSetForeground(display_, gc_, styles_->get_color_rgb(60, 60, 60));

            XDrawRectangle(display_, window_, gc_, bx, by, bs, bs);

            if (idx < (int)checked_.size() && checked_[idx])

            {

                xf::draw_checkmark(display_, window_, gc_, st.text_color_, bx + bs / 2, by + bs / 2 - 1, bs - 2);

            }

            SStyle lst = st;

            if (selected)

            {

                lst.text_color_ = white;

            }

            if (!enabled_)

            {

                lst.text_color_ = styles_->get_color_rgb(140, 140, 140);

            }

            xf::draw_text_left(display_, window_, gc_, lst, x1_ + 22, ry1, ry2, items_[idx]);

        }

        if (focused_)

        {

            XSetForeground(display_, gc_, st.outlineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 2, height() - 2);

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

            if (top_index_ > 0)

            {

                --top_index_;

            }

            return true;

        }

        if (button == 5)

        {

            int vr = visible_rows();

            if (top_index_ + vr < (int)items_.size())

            {

                ++top_index_;

            }

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        int row = (y - (y1_ + 2)) / row_height_;

        if (row < 0)

        {

            return true;

        }

        int idx = top_index_ + row;

        if (idx < 0 || idx >= (int)items_.size())

        {

            return true;

        }

        item_index_ = idx;

        if (x >= x1_ + 2 && x <= x1_ + 16)

        {

            checked_[idx] = !checked_[idx];

            if (on_click_check)

            {

                on_click_check(idx);

            }

        }

        else if (on_click)

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

        if (!focused_ || !enabled_ || items_.empty())

        {

            return false;

        }

        if (key == XK_Down)

        {

            if (item_index_ < (int)items_.size() - 1)

            {

                ++item_index_;

                ensure_visible(item_index_);

            }

            return true;

        }

        if (key == XK_Up)

        {

            if (item_index_ > 0)

            {

                --item_index_;

                ensure_visible(item_index_);

            }

            return true;

        }

        if (key == XK_space)

        {

            if (item_index_ >= 0 && item_index_ < (int)checked_.size())

            {

                checked_[item_index_] = !checked_[item_index_];

                if (on_click_check)

                {

                    on_click_check(item_index_);

                }

            }

            return true;

        }

        if (key == XK_Return && on_click)

        {

            on_click();

            return true;

        }

        return false;

    }

};

//endregion
