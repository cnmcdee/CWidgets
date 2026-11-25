#pragma once

#include "core/xframe_core.h"

//region GtkDropDown

class GtkDropDown

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;   // mirrors the selected item

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    std::vector<std::string> items_;

    int selected_ = -1;

    bool open_ = false;

    int hover_row_ = -1;      // row within the filtered list

    bool enable_search_ = true;

    std::string filter_;      // the search row's text while open

    bool pressed_ = false;

    int row_height()

    {

        return 20;

    }

    int max_rows()

    {

        return 8;

    }

    int search_height()

    {

        return enable_search_ ? 26 : 0;

    }

    int list_y1()

    {

        return y2_ + 2;

    }

    int rows_y1()

    {

        return list_y1() + search_height();

    }

    // Indices of the items passing the current filter.

    std::vector<int> filtered()

    {

        std::vector<int> out;

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            if (filter_.empty())

            {

                out.push_back(i);

                continue;

            }

            std::string hay = items_[(size_t)i];

            std::string needle = filter_;

            for (auto& c : hay)

            {

                if (c >= 'A' && c <= 'Z')

                {

                    c = (char)(c + 32);

                }

            }

            for (auto& c : needle)

            {

                if (c >= 'A' && c <= 'Z')

                {

                    c = (char)(c + 32);

                }

            }

            if (hay.find(needle) != std::string::npos)

            {

                out.push_back(i);

            }

        }

        return out;

    }

    int shown_rows()

    {

        return std::min(max_rows(), (int)filtered().size());

    }

    bool in_list(int x, int y)

    {

        return open_ && x >= x1_ && x <= x2_ && y >= list_y1() && y <= rows_y1() + shown_rows() * row_height();

    }

    void choose_filtered(int row)

    {

        std::vector<int> f = filtered();

        if (row < 0 || row >= (int)f.size())

        {

            return;

        }

        int idx = f[(size_t)row];

        bool was = (selected_ != idx);

        selected_ = idx;

        text_ = items_[(size_t)idx];

        open_ = false;

        filter_.clear();

        if (was && on_changed)

        {

            on_changed();

        }

    }

public:

    std::function<void()> on_changed;

    GtkDropDown(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkDropDown";

        items_ = {"Alpha", "Beta", "Gamma", "Delta", "Epsilon", "Zeta", "Eta", "Theta"};

        selected_ = 0;

        text_ = items_[0];

    }

    ~GtkDropDown() {}

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

        return in_list(x, y);

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

        set_selected_text(t);

    }

    std::string get_text()

    {

        return get_selected_text();

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region GtkDropDown specific API

    void add_item(std::string item)

    {

        items_.push_back(item);

        if (selected_ < 0)

        {

            selected_ = 0;

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

        if (selected_ >= (int)items_.size())

        {

            selected_ = (int)items_.size() - 1;

        }

        text_ = selected_ >= 0 ? items_[(size_t)selected_] : "";

    }

    void clear_items()

    {

        items_.clear();

        selected_ = -1;

        text_ = "";

    }

    int get_item_count()

    {

        return (int)items_.size();

    }

    void set_selected(int index)

    {

        if (index >= -1 && index < (int)items_.size())

        {

            selected_ = index;

            text_ = index >= 0 ? items_[(size_t)index] : "";

        }

    }

    int get_selected()

    {

        return selected_;

    }

    std::string get_selected_text()

    {

        if (selected_ < 0 || selected_ >= (int)items_.size())

        {

            return "";

        }

        return items_[(size_t)selected_];

    }

    void set_selected_text(std::string t)

    {

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            if (items_[(size_t)i] == t)

            {

                selected_ = i;

                text_ = t;

                return;

            }

        }

    }

    void set_enable_search(bool e)

    {

        enable_search_ = e;

    }

    bool get_enable_search()

    {

        return enable_search_;

    }

    bool get_popup_open()

    {

        return open_;

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

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 8, y1_, y2_, get_selected_text());

        xf::draw_triangle_down(display_, window_, gc_, st.text_color_, x2_ - 14, (y1_ + y2_) / 2 - 3, 6);

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 3, y1_ + 3, width() - 6, height() - 6);

        }

        if (open_)

        {

            int ly = list_y1();

            int total_h = search_height() + shown_rows() * row_height();

            XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

            XFillRectangle(display_, window_, gc_, x1_, ly, width(), total_h);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_, ly, width(), total_h);

            // Search row at the top of the popup.

            if (enable_search_)

            {

                XSetForeground(display_, gc_, styles_->get_color_rgb(245, 245, 245));

                XFillRectangle(display_, window_, gc_, x1_ + 1, ly + 1, width() - 2, search_height() - 2);

                XSetForeground(display_, gc_, st.lineColor_);

                XDrawLine(display_, window_, gc_, x1_, ly + search_height(), x2_, ly + search_height());

                int cy = ly + search_height() / 2;

                unsigned long grey = styles_->get_color_rgb(110, 110, 110);

                XSetForeground(display_, gc_, grey);

                xf::set_line_width(display_, gc_, 2);

                XDrawArc(display_, window_, gc_, x1_ + 6, cy - 5, 9, 9, 0, 360 * 64);

                XDrawLine(display_, window_, gc_, x1_ + 13, cy + 2, x1_ + 18, cy + 7);

                xf::set_line_width(display_, gc_, 1);

                if (filter_.empty())

                {

                    SStyle ph = st;

                    ph.text_color_ = styles_->get_color_rgb(150, 150, 150);

                    xf::draw_text_left(display_, window_, gc_, ph, x1_ + 24, ly, ly + search_height(), "Search...");

                }

                else

                {

                    xf::draw_text_left(display_, window_, gc_, st, x1_ + 24, ly, ly + search_height(), filter_);

                }

            }

            std::vector<int> f = filtered();

            for (int row = 0; row < shown_rows(); ++row)

            {

                int idx = f[(size_t)row];

                int ry = rows_y1() + row * row_height();

                if (row == hover_row_ || idx == selected_)

                {

                    XSetForeground(display_, gc_, styles_->get_color_rgb(195, 215, 245));

                    XFillRectangle(display_, window_, gc_, x1_ + 1, ry + 1, width() - 2, row_height() - 1);

                }

                xf::draw_text_left(display_, window_, gc_, st, x1_ + 8, ry, ry + row_height(), items_[(size_t)idx]);

                if (idx == selected_)

                {

                    xf::draw_checkmark(display_, window_, gc_, st.text_color_, x2_ - 14, ry + row_height() / 2, 10);

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

        if (open_ && y >= rows_y1() && in_list(x, y))

        {

            choose_filtered((y - rows_y1()) / row_height());

            return true;

        }

        if (open_ && in_list(x, y))

        {

            return true;   // clicks on the search row just keep focus

        }

        if (x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_)

        {

            focused_ = true;

            pressed_ = true;

            open_ = !open_;

            if (!open_)

            {

                filter_.clear();

            }

            return true;

        }

        if (open_)

        {

            open_ = false;

            filter_.clear();

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

        if (!open_ || y < rows_y1() || !in_list(x, y))

        {

            return false;

        }

        int row = (y - rows_y1()) / row_height();

        if (row == hover_row_)

        {

            return false;

        }

        hover_row_ = row;

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (!open_)

        {

            if (key == XK_Down || key == XK_Return || key == XK_KP_Enter || key == XK_space)

            {

                open_ = true;

                return true;

            }

            return false;

        }

        // While open, keys drive the search filter first.

        if (key == XK_Escape)

        {

            open_ = false;

            filter_.clear();

            return true;

        }

        if (key == XK_Return || key == XK_KP_Enter)

        {

            if (hover_row_ >= 0)

            {

                choose_filtered(hover_row_);

            }

            else

            {

                open_ = false;

                filter_.clear();

            }

            return true;

        }

        if (key == XK_Down)

        {

            int n = (int)filtered().size();

            if (n > 0)

            {

                hover_row_ = std::min(hover_row_ + 1, n - 1);

            }

            return true;

        }

        if (key == XK_Up)

        {

            hover_row_ = std::max(hover_row_ - 1, 0);

            return true;

        }

        if (enable_search_ && key == XK_BackSpace)

        {

            if (!filter_.empty())

            {

                filter_.pop_back();

                hover_row_ = -1;

            }

            return true;

        }

        if (enable_search_ && str && str[0] >= 32 && str[0] != 127)

        {

            filter_ += str[0];

            hover_row_ = -1;

            return true;

        }

        return false;

    }

};

//endregion
