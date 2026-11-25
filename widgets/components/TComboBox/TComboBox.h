#pragma once

#include "core/xframe_core.h"

//region TComboBox

class TComboBox

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

    int item_index_ = -1;

    int style_ = 0;

    bool dropdown_open_ = false;

    int cursor_pos_ = 0;

    int item_height_ = 18;

    int max_dropdown_rows_ = 6;

    int edit_height()

    {

        return height();

    }

    int button_width()

    {

        return 20;

    }

    int dropdown_rows()

    {

        int rows = (int)items_.size();

        if (rows > max_dropdown_rows_)

        {

            rows = max_dropdown_rows_;

        }

        if (rows < 1)

        {

            rows = 1;

        }

        return rows;

    }

    int dropdown_y2()

    {

        return y2_ + dropdown_rows() * item_height_ + 4;

    }

    bool in_dropdown(int x, int y)

    {

        return dropdown_open_ && x >= x1_ && x <= x2_ && y > y2_ && y <= dropdown_y2();

    }

    void fire_change()

    {

        if (on_change)

        {

            on_change();

        }

    }

    void select_index(int index)

    {

        if (index < 0 || index >= (int)items_.size())

        {

            return;

        }

        item_index_ = index;

        text_ = items_[index];

        cursor_pos_ = (int)text_.size();

    }

public:

    std::function<void()> on_change;

    std::function<void()> on_click;

    TComboBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TComboBox";

        text_ = "ComboBox";

        add_item("Apple");

        add_item("Banana");

        add_item("Cherry");

        add_item("Date");

        add_item("Elderberry");

        add_item("Fig");

        select_index(0);

    }

    ~TComboBox() {}

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

        cursor_pos_ = (int)text_.size();

        item_index_ = -1;

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            if (items_[i] == t)

            {

                item_index_ = i;

            }

        }

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region Items and style API

    void add_item(std::string item)

    {

        items_.push_back(item);

    }

    void clear()

    {

        items_.clear();

        item_index_ = -1;

        text_ = "";

        cursor_pos_ = 0;

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

        select_index(index);

    }

    int get_item_index()

    {

        return item_index_;

    }

    void set_style(int s)

    {

        style_ = s;

        if (style_ == 1 && item_index_ >= 0)

        {

            select_index(item_index_);

        }

    }

    int get_style()

    {

        return style_;

    }

    void set_dropdown_open(bool open)

    {

        dropdown_open_ = open;

    }

    bool get_dropdown_open()

    {

        return dropdown_open_;

    }

    int get_cursor_pos()

    {

        return cursor_pos_;

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

        int eh = edit_height();

        int bw = button_width();

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), eh);

        xf::draw_rounded_outline(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 6, y1_, y2_, text_);

        if (focused_ && style_ == 0 && st.font_)

        {

            std::string before = text_.substr(0, cursor_pos_);

            int cx = x1_ + 6 + xf::text_width(st, before);

            XSetForeground(display_, gc_, st.text_color_);

            XDrawLine(display_, window_, gc_, cx, y1_ + 4, cx, y2_ - 4);

        }

        xf::draw_polished(display_, window_, gc_, st, x2_ - bw, y1_ + 1, x2_ - 1, y2_ - 1);

        xf::draw_triangle_down(display_, window_, gc_, st.text_color_, (x2_ - bw + x2_) / 2, y1_ + eh / 2 - 3, 5);

        if (dropdown_open_)

        {

            int dy1 = y2_ + 1;

            int dy2 = dropdown_y2();

            XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

            XFillRectangle(display_, window_, gc_, x1_, dy1, width(), dy2 - dy1);

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, 1);

            XDrawRectangle(display_, window_, gc_, x1_, dy1, width(), dy2 - dy1);

            for (int i = 0; i < dropdown_rows(); ++i)

            {

                int ry1 = dy1 + 2 + i * item_height_;

                int ry2 = ry1 + item_height_;

                if (i == item_index_)

                {

                    XSetForeground(display_, gc_, st.outlineColor_);

                    XFillRectangle(display_, window_, gc_, x1_ + 2, ry1, width() - 4, item_height_);

                    SStyle sel = st;

                    sel.text_color_ = styles_->get_color_rgb(255, 255, 255);

                    xf::draw_text_left(display_, window_, gc_, sel, x1_ + 6, ry1, ry2, items_[i]);

                }

                else

                {

                    xf::draw_text_left(display_, window_, gc_, st, x1_ + 6, ry1, ry2, items_[i]);

                }

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

        if (in_dropdown(x, y))

        {

            int index = (y - y2_ - 3) / item_height_;

            if (index >= 0 && index < (int)items_.size() && index < dropdown_rows())

            {

                select_index(index);

                fire_change();

            }

            dropdown_open_ = false;

            focused_ = true;

            return true;

        }

        if (!contains(x, y))

        {

            dropdown_open_ = false;

            return false;

        }

        focused_ = true;

        if (x >= x2_ - button_width())

        {

            dropdown_open_ = !dropdown_open_;

            return true;

        }

        if (style_ == 0)

        {

            cursor_pos_ = (int)text_.size();

        }

        else

        {

            dropdown_open_ = !dropdown_open_;

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Down || key == XK_Up)

        {

            if (!items_.empty())

            {

                int next = item_index_ + (key == XK_Down ? 1 : -1);

                if (next < 0)

                {

                    next = 0;

                }

                if (next >= (int)items_.size())

                {

                    next = (int)items_.size() - 1;

                }

                select_index(next);

                fire_change();

            }

            return true;

        }

        if (key == XK_Escape)

        {

            dropdown_open_ = false;

            return true;

        }

        if (key == XK_Return)

        {

            dropdown_open_ = false;

            fire_change();

            return true;

        }

        if (key == XK_F4)

        {

            dropdown_open_ = !dropdown_open_;

            return true;

        }

        if (style_ != 0)

        {

            return false;

        }

        if (key == XK_BackSpace)

        {

            if (cursor_pos_ > 0)

            {

                text_.erase(cursor_pos_ - 1, 1);

                cursor_pos_--;

                item_index_ = -1;

                fire_change();

            }

            return true;

        }

        if (key == XK_Delete)

        {

            if (cursor_pos_ < (int)text_.size())

            {

                text_.erase(cursor_pos_, 1);

                item_index_ = -1;

                fire_change();

            }

            return true;

        }

        if (key == XK_Left)

        {

            if (cursor_pos_ > 0)

            {

                cursor_pos_--;

            }

            return true;

        }

        if (key == XK_Right)

        {

            if (cursor_pos_ < (int)text_.size())

            {

                cursor_pos_++;

            }

            return true;

        }

        if (key == XK_Home)

        {

            cursor_pos_ = 0;

            return true;

        }

        if (key == XK_End)

        {

            cursor_pos_ = (int)text_.size();

            return true;

        }

        if (str && str[0] >= 32 && str[0] < 127)

        {

            text_.insert(cursor_pos_, 1, str[0]);

            cursor_pos_++;

            item_index_ = -1;

            fire_change();

            return true;

        }

        return false;

    }

};

//endregion
