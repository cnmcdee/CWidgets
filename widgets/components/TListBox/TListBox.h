#pragma once

#include "core/xframe_core.h"

#include <set>

//region TListBox

class TListBox

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

    std::set<int> selected_;

    int item_index_ = -1;

    int top_index_ = 0;

    bool multi_select_ = false;

    bool sorted_ = false;

    int item_height_ = 18;

    int visible_rows()

    {

        int rows = height() / item_height_;

        if (rows < 1)

        {

            rows = 1;

        }

        return rows;

    }

    void clamp_top()

    {

        if (top_index_ < 0)

        {

            top_index_ = 0;

        }

        int max_top = (int)items_.size() - visible_rows();

        if (max_top < 0)

        {

            max_top = 0;

        }

        if (top_index_ > max_top)

        {

            top_index_ = max_top;

        }

    }

    void ensure_visible(int index)

    {

        if (index < 0)

        {

            return;

        }

        if (index < top_index_)

        {

            top_index_ = index;

        }

        if (index >= top_index_ + visible_rows())

        {

            top_index_ = index - visible_rows() + 1;

        }

        clamp_top();

    }

    void fire_selection_change()

    {

        if (on_selection_change)

        {

            on_selection_change();

        }

    }

public:

    std::function<void()> on_selection_change;

    std::function<void()> on_click;

    TListBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TListBox";

        text_ = "ListBox";

        add_item("Alpha");

        add_item("Bravo");

        add_item("Charlie");

        add_item("Delta");

        add_item("Echo");

        add_item("Foxtrot");

        add_item("Golf");

        add_item("Hotel");

        item_index_ = 1;

        selected_.insert(1);

        top_index_ = 0;

    }

    ~TListBox() {}

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

    //region Items API

    void add_item(std::string item)

    {

        if (sorted_)

        {

            auto pos = std::lower_bound(items_.begin(), items_.end(), item);

            items_.insert(pos, item);

        }

        else

        {

            items_.push_back(item);

        }

    }

    void insert_item(int index, std::string item)

    {

        if (sorted_)

        {

            add_item(item);

            return;

        }

        if (index < 0)

        {

            index = 0;

        }

        if (index > (int)items_.size())

        {

            index = (int)items_.size();

        }

        items_.insert(items_.begin() + index, item);

    }

    void delete_item(int index)

    {

        if (index < 0 || index >= (int)items_.size())

        {

            return;

        }

        items_.erase(items_.begin() + index);

        selected_.erase(index);

        if (item_index_ >= (int)items_.size())

        {

            item_index_ = (int)items_.size() - 1;

        }

        clamp_top();

    }

    void clear()

    {

        items_.clear();

        selected_.clear();

        item_index_ = -1;

        top_index_ = 0;

    }

    int count()

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

    std::vector<std::string>& items()

    {

        return items_;

    }

    void set_item_index(int index)

    {

        if (items_.empty())

        {

            item_index_ = -1;

            return;

        }

        if (index < -1)

        {

            index = -1;

        }

        if (index >= (int)items_.size())

        {

            index = (int)items_.size() - 1;

        }

        item_index_ = index;

        if (!multi_select_)

        {

            selected_.clear();

            if (index >= 0)

            {

                selected_.insert(index);

            }

        }

        ensure_visible(index);

    }

    int get_item_index()

    {

        return item_index_;

    }

    void set_top_index(int index)

    {

        top_index_ = index;

        clamp_top();

    }

    int get_top_index()

    {

        return top_index_;

    }

    void set_multi_select(bool m)

    {

        multi_select_ = m;

    }

    bool get_multi_select()

    {

        return multi_select_;

    }

    void set_sorted(bool s)

    {

        sorted_ = s;

        if (sorted_)

        {

            std::sort(items_.begin(), items_.end());

        }

    }

    bool get_sorted()

    {

        return sorted_;

    }

    bool is_selected(int index)

    {

        if (multi_select_)

        {

            return selected_.count(index) > 0;

        }

        return index == item_index_;

    }

    void set_selected(int index, bool sel)

    {

        if (index < 0 || index >= (int)items_.size())

        {

            return;

        }

        if (sel)

        {

            selected_.insert(index);

        }

        else

        {

            selected_.erase(index);

        }

    }

    int sel_count()

    {

        if (multi_select_)

        {

            return (int)selected_.size();

        }

        return item_index_ >= 0 ? 1 : 0;

    }

    int row_at(int y)

    {

        if (y < y1_ || y > y2_)

        {

            return -1;

        }

        int row = top_index_ + (y - y1_ - 2) / item_height_;

        if (row < 0 || row >= (int)items_.size())

        {

            return -1;

        }

        return row;

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

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        xf::draw_rounded_outline(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        int rows = visible_rows();

        for (int r = 0; r < rows; ++r)

        {

            int index = top_index_ + r;

            if (index < 0 || index >= (int)items_.size())

            {

                break;

            }

            int ry1 = y1_ + 2 + r * item_height_;

            int ry2 = ry1 + item_height_;

            if (is_selected(index))

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, x1_ + 2, ry1, width() - 4, item_height_);

                SStyle sel = st;

                sel.text_color_ = styles_->get_color_rgb(255, 255, 255);

                xf::draw_text_left(display_, window_, gc_, sel, x1_ + 6, ry1, ry2, items_[index]);

            }

            else

            {

                xf::draw_text_left(display_, window_, gc_, st, x1_ + 6, ry1, ry2, items_[index]);

            }

        }

        if ((int)items_.size() > rows)

        {

            int track_h = height() - 4;

            int thumb_h = track_h * rows / (int)items_.size();

            if (thumb_h < 12)

            {

                thumb_h = 12;

            }

            int max_top = (int)items_.size() - rows;

            int thumb_y = y1_ + 2 + (max_top > 0 ? (track_h - thumb_h) * top_index_ / max_top : 0);

            XSetForeground(display_, gc_, styles_->get_color_rgb(190, 190, 190));

            XFillRectangle(display_, window_, gc_, x2_ - 8, thumb_y, 5, thumb_h);

        }

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

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

            top_index_ -= 3;

            clamp_top();

            return true;

        }

        if (button == 5)

        {

            top_index_ += 3;

            clamp_top();

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        int row = row_at(y);

        if (row < 0)

        {

            return true;

        }

        if (multi_select_)

        {

            item_index_ = row;

            if (selected_.count(row))

            {

                selected_.erase(row);

            }

            else

            {

                selected_.insert(row);

            }

        }

        else

        {

            item_index_ = row;

            selected_.clear();

            selected_.insert(row);

        }

        ensure_visible(row);

        fire_selection_change();

        if (on_click)

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

        int next = item_index_;

        if (key == XK_Up)

        {

            next = item_index_ - 1;

        }

        else if (key == XK_Down)

        {

            next = item_index_ + 1;

        }

        else if (key == XK_Page_Up)

        {

            next = item_index_ - visible_rows();

        }

        else if (key == XK_Page_Down)

        {

            next = item_index_ + visible_rows();

        }

        else if (key == XK_Home)

        {

            next = 0;

        }

        else if (key == XK_End)

        {

            next = (int)items_.size() - 1;

        }

        else

        {

            return false;

        }

        if (next < 0)

        {

            next = 0;

        }

        if (next >= (int)items_.size())

        {

            next = (int)items_.size() - 1;

        }

        if (next != item_index_)

        {

            set_item_index(next);

            fire_selection_change();

        }

        return true;

    }

};

//endregion
