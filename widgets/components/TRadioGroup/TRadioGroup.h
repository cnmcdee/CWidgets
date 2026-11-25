#pragma once

#include "core/xframe_core.h"

//region TRadioGroup

class TRadioGroup

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

    int item_index_ = 0;

    int columns_ = 1;

    int row_height_ = 22;

    int rows_per_column()

    {

        if (items_.empty())

        {

            return 0;

        }

        int cols = columns_ < 1 ? 1 : columns_;

        return ((int)items_.size() + cols - 1) / cols;

    }

    int column_width()

    {

        int cols = columns_ < 1 ? 1 : columns_;

        int w = (x2_ - 8 - (x1_ + 8)) / cols;

        if (w < 20)

        {

            w = 20;

        }

        return w;

    }

    void item_rect(int index, int& ix1, int& iy1, int& ix2, int& iy2)

    {

        int rows = rows_per_column();

        if (rows < 1)

        {

            rows = 1;

        }

        int col = index / rows;

        int row = index % rows;

        ix1 = x1_ + 8 + col * column_width();

        iy1 = y1_ + 18 + row * row_height_;

        ix2 = ix1 + column_width() - 4;

        iy2 = iy1 + row_height_;

    }

    void fire_change()

    {

        if (on_change)

        {

            on_change();

        }

    }

public:

    std::function<void()> on_change;

    std::function<void()> on_click;

    TRadioGroup(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TRadioGroup";

        text_ = "RadioGroup";

        add_item("Option One");

        add_item("Option Two");

        add_item("Option Three");

        item_index_ = 0;

    }

    ~TRadioGroup() {}

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

    void set_caption(std::string t)

    {

        text_ = t;

    }

    std::string get_caption()

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

        items_.push_back(item);

    }

    void clear()

    {

        items_.clear();

        item_index_ = -1;

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

        if (index < 0)

        {

            index = 0;

        }

        if (index >= (int)items_.size())

        {

            index = (int)items_.size() - 1;

        }

        item_index_ = index;

    }

    int get_item_index()

    {

        return item_index_;

    }

    void set_columns(int cols)

    {

        if (cols < 1)

        {

            cols = 1;

        }

        columns_ = cols;

    }

    int get_columns()

    {

        return columns_;

    }

    void client_rect(int& cx1, int& cy1, int& cx2, int& cy2)

    {

        cx1 = x1_ + 8;

        cy1 = y1_ + 18;

        cx2 = x2_ - 8;

        cy2 = y2_ - 8;

    }

    int item_at(int x, int y)

    {

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            int ix1 = 0;

            int iy1 = 0;

            int ix2 = 0;

            int iy2 = 0;

            item_rect(i, ix1, iy1, ix2, iy2);

            if (x >= ix1 && x <= ix2 && y >= iy1 && y <= iy2)

            {

                return i;

            }

        }

        return -1;

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

        xf::fill_rounded_rect(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        xf::draw_rounded_outline(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        if (!text_.empty() && st.font_)

        {

            int tw = xf::text_width(st, text_);

            int cap_x = x1_ + 10;

            XSetForeground(display_, gc_, st.fillColor_);

            XFillRectangle(display_, window_, gc_, cap_x - 3, y1_ - 1, tw + 6, 3);

            xf::draw_text_left(display_, window_, gc_, st, cap_x, y1_ - 6, y1_ + 10, text_);

        }

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            int ix1 = 0;

            int iy1 = 0;

            int ix2 = 0;

            int iy2 = 0;

            item_rect(i, ix1, iy1, ix2, iy2);

            int cx = ix1 + 8;

            int cy = (iy1 + iy2) / 2;

            int r = 6;

            if (i == item_index_ && focused_)

            {

                XSetForeground(display_, gc_, styles_->get_color_rgb(210, 225, 245));

                XFillRectangle(display_, window_, gc_, ix1, iy1 + 1, ix2 - ix1, row_height_ - 2);

            }

            XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

            XFillArc(display_, window_, gc_, cx - r, cy - r, 2 * r, 2 * r, 0, 360 * 64);

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, 1);

            XDrawArc(display_, window_, gc_, cx - r, cy - r, 2 * r, 2 * r, 0, 360 * 64);

            if (i == item_index_)

            {

                XSetForeground(display_, gc_, st.text_color_);

                XFillArc(display_, window_, gc_, cx - 3, cy - 3, 6, 6, 0, 360 * 64);

            }

            xf::draw_text_left(display_, window_, gc_, st, ix1 + 20, iy1, iy2, items_[i]);

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

        int index = item_at(x, y);

        if (index >= 0 && index != item_index_)

        {

            item_index_ = index;

            fire_change();

        }

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

        else if (key == XK_Left)

        {

            next = item_index_ - rows_per_column();

        }

        else if (key == XK_Right)

        {

            next = item_index_ + rows_per_column();

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

            item_index_ = next;

            fire_change();

        }

        return true;

    }

};

//endregion
