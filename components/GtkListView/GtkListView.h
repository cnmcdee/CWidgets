#pragma once

#include "core/xframe_core.h"

//region GtkListView

class GtkListView

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

    int anchor_row_ = -1;

    int scroll_row_ = 0;

    int row_height_ = 22;

    bool dragging_ = false;

    int band_x0_ = 0;

    int band_y0_ = 0;

    int band_x1_ = 0;

    int band_y1_ = 0;

    int visible_rows()

    {

        return std::max(1, (height() - 4) / row_height_);

    }

    int row_at(int y)

    {

        if (y < y1_ + 2)

        {

            return -1;

        }

        int index = scroll_row_ + (y - y1_ - 2) / row_height_;

        if (index < 0 || index >= (int)items_.size())

        {

            return -1;

        }

        return index;

    }

public:

    std::function<void()> on_selection_changed;

    GtkListView(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkListView";

        text_ = "ListView";

        add_item("Alpha");

        add_item("Bravo");

        add_item("Charlie");

        add_item("Delta");

        add_item("Echo");

        add_item("Foxtrot");

        add_item("Golf");

        add_item("Hotel");

        if (!selected_.empty())

        {

            selected_[0] = true;

            anchor_row_ = 0;

        }

    }

    ~GtkListView() {}

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

    //region List specific API

    void add_item(std::string item)

    {

        items_.push_back(item);

        selected_.push_back(false);

    }

    void clear_items()

    {

        items_.clear();

        selected_.clear();

        anchor_row_ = -1;

        scroll_row_ = 0;

    }

    int item_count()

    {

        return (int)items_.size();

    }

    std::string get_item(int index)

    {

        if (index >= 0 && index < (int)items_.size())

        {

            return items_[index];

        }

        return "";

    }

    void select_row(int index)

    {

        for (size_t i = 0; i < selected_.size(); ++i)

        {

            selected_[i] = false;

        }

        if (index >= 0 && index < (int)selected_.size())

        {

            selected_[index] = true;

            anchor_row_ = index;

        }

    }

    std::vector<int> get_selected_rows()

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

    int get_selected_row()

    {

        std::vector<int> rows = get_selected_rows();

        return rows.empty() ? -1 : rows[0];

    }

    void set_scroll_row(int row)

    {

        scroll_row_ = std::max(0, row);

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

        unsigned long band_fill = styles_->get_color_rgb(150, 180, 230);

        for (int i = scroll_row_; i < (int)items_.size(); ++i)

        {

            int ry = y1_ + 2 + (i - scroll_row_) * row_height_;

            if (ry + row_height_ > y2_ - 2)

            {

                break;

            }

            SStyle row_style = st;

            if (selected_[i])

            {

                XSetForeground(display_, gc_, sel_fill);

                XFillRectangle(display_, window_, gc_, x1_ + 2, ry, width() - 4, row_height_);

                row_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            xf::draw_text_left(display_, window_, gc_, row_style, x1_ + 8, ry, ry + row_height_, items_[i]);

        }

        // Rubber band while dragging.

        if (dragging_)

        {

            int bx = std::min(band_x0_, band_x1_);

            int by = std::min(band_y0_, band_y1_);

            int bw = std::abs(band_x1_ - band_x0_);

            int bh = std::abs(band_y1_ - band_y0_);

            XSetForeground(display_, gc_, band_fill);

            XDrawRectangle(display_, window_, gc_, bx, by, bw, bh);

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

        if (!contains(x, y) || !enabled_)

        {

            return false;

        }

        focused_ = true;

        if (button == 4)

        {

            scroll_row_ = std::max(0, scroll_row_ - 3);

            return true;

        }

        if (button == 5)

        {

            scroll_row_ += 3;

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        int row = row_at(y);

        for (size_t i = 0; i < selected_.size(); ++i)

        {

            selected_[i] = false;

        }

        if (row >= 0)

        {

            selected_[row] = true;

            anchor_row_ = row;

        }

        dragging_ = true;

        band_x0_ = x;

        band_y0_ = y;

        band_x1_ = x;

        band_y1_ = y;

        if (on_selection_changed)

        {

            on_selection_changed();

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button != 1 || !dragging_)

        {

            return false;

        }

        dragging_ = false;

        return true;

    }

    bool on_motion(int x, int y)

    {

        if (!dragging_)

        {

            return false;

        }

        band_x1_ = x;

        band_y1_ = y;

        // Select every row the band touches.

        int top = std::min(band_y0_, band_y1_);

        int bottom = std::max(band_y0_, band_y1_);

        for (size_t i = 0; i < selected_.size(); ++i)

        {

            selected_[i] = false;

        }

        for (int ry = top; ry <= bottom; ry += 4)

        {

            int row = row_at(ry);

            if (row >= 0)

            {

                selected_[row] = true;

            }

        }

        int last = row_at(bottom);

        if (last >= 0)

        {

            selected_[last] = true;

        }

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_ || items_.empty())

        {

            return false;

        }

        int current = anchor_row_ < 0 ? 0 : anchor_row_;

        if (key == XK_Down && current < (int)items_.size() - 1)

        {

            select_row(current + 1);

        }

        else if (key == XK_Up && current > 0)

        {

            select_row(current - 1);

        }

        else

        {

            return false;

        }

        if (anchor_row_ < scroll_row_)

        {

            scroll_row_ = anchor_row_;

        }

        if (anchor_row_ >= scroll_row_ + visible_rows())

        {

            scroll_row_ = anchor_row_ - visible_rows() + 1;

        }

        if (on_selection_changed)

        {

            on_selection_changed();

        }

        return true;

    }

};

//endregion
