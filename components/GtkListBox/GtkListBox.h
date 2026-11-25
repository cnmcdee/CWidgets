#pragma once

#include "core/xframe_core.h"

//region GtkListBox

class GtkListBox

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

    std::vector<std::string> rows_;

    int row_height_ = 24;

    int selected_row_ = 0;

    int scroll_offset_ = 0;   // first visible row index

    int inset_ = 2;

    void clamp_scroll()

    {

        int vis = visible_row_count();

        if (scroll_offset_ < 0)

        {

            scroll_offset_ = 0;

        }

        int max_off = (int)rows_.size() - vis;

        if (max_off < 0)

        {

            max_off = 0;

        }

        if (scroll_offset_ > max_off)

        {

            scroll_offset_ = max_off;

        }

    }

    void keep_selection_visible()

    {

        if (selected_row_ < 0)

        {

            return;

        }

        int vis = visible_row_count();

        if (selected_row_ < scroll_offset_)

        {

            scroll_offset_ = selected_row_;

        }

        if (selected_row_ >= scroll_offset_ + vis)

        {

            scroll_offset_ = selected_row_ - vis + 1;

        }

        clamp_scroll();

    }

public:

    std::function<void(int)> on_row_selected;

    GtkListBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkListBox";

        text_ = "List Box";

        rows_.push_back("Apple");

        rows_.push_back("Banana");

        rows_.push_back("Cherry");

        rows_.push_back("Dragonfruit");

        rows_.push_back("Elderberry");

        rows_.push_back("Fig");

        rows_.push_back("Grape");

        rows_.push_back("Honeydew");

        rows_.push_back("Kiwi");

        rows_.push_back("Lemon");

        rows_.push_back("Mango");

        rows_.push_back("Nectarine");

        rows_.push_back("Orange");

        rows_.push_back("Papaya");

    }

    ~GtkListBox() {}

    void set_bounds(int x, int y, int width, int height)

    {

        x1_ = x;

        y1_ = y;

        x2_ = x + width;

        y2_ = y + height;

        clamp_scroll();

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

    int visible_row_count()

    {

        int n = (height() - 2 * inset_) / row_height_;

        return n < 1 ? 1 : n;

    }

    // Rect of the row at visible slot v (0 = top visible row).

    void row_rect(int v, int& rx1, int& ry1, int& rx2, int& ry2)

    {

        rx1 = x1_ + inset_;

        ry1 = y1_ + inset_ + v * row_height_;

        rx2 = x2_ - inset_;

        ry2 = ry1 + row_height_;

    }

    void add_row(std::string label)

    {

        rows_.push_back(label);

    }

    void remove_row(int i)

    {

        if (i < 0 || i >= (int)rows_.size())

        {

            return;

        }

        rows_.erase(rows_.begin() + i);

        if (selected_row_ >= (int)rows_.size())

        {

            selected_row_ = (int)rows_.size() - 1;

        }

        clamp_scroll();

    }

    int row_count()

    {

        return (int)rows_.size();

    }

    int get_selected_row()

    {

        return selected_row_;

    }

    void set_selected_row(int i)

    {

        if (rows_.empty())

        {

            selected_row_ = -1;

            return;

        }

        if (i < -1)

        {

            i = -1;

        }

        if (i >= (int)rows_.size())

        {

            i = (int)rows_.size() - 1;

        }

        selected_row_ = i;

        keep_selection_visible();

    }

    std::string get_row(int i)

    {

        if (i < 0 || i >= (int)rows_.size())

        {

            return "";

        }

        return rows_[i];

    }

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

        clamp_scroll();

        // Frame + background.

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, (int)st.lineWidth_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        int vis = visible_row_count();

        for (int v = 0; v < vis; ++v)

        {

            int idx = scroll_offset_ + v;

            if (idx >= (int)rows_.size())

            {

                break;

            }

            int rx1, ry1, rx2, ry2;

            row_rect(v, rx1, ry1, rx2, ry2);

            if (idx == selected_row_)

            {

                // Selected row: highlighted fill, swapped text colour.

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, rx1, ry1, rx2 - rx1, ry2 - ry1);

                SStyle sel = st;

                sel.text_color_ = styles_->get_color_rgb(255, 255, 255);

                xf::draw_text_left(display_, window_, gc_, sel, rx1 + 8, ry1, ry2, rows_[idx]);

            }

            else

            {

                xf::draw_text_left(display_, window_, gc_, st, rx1 + 8, ry1, ry2, rows_[idx]);

            }

            // Separator line under the row.

            XSetForeground(display_, gc_, styles_->get_color_rgb(225, 225, 225));

            XDrawLine(display_, window_, gc_, rx1, ry2 - 1, rx2, ry2 - 1);

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

        // Mouse wheel scrolls the row window.

        if (button == 4)

        {

            scroll_offset_ -= 1;

            clamp_scroll();

            return true;

        }

        if (button == 5)

        {

            scroll_offset_ += 1;

            clamp_scroll();

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        if (y < y1_ + inset_)

        {

            return true;

        }

        int v = (y - y1_ - inset_) / row_height_;

        int idx = scroll_offset_ + v;

        if (idx >= 0 && idx < (int)rows_.size())

        {

            selected_row_ = idx;

            if (on_row_selected)

            {

                on_row_selected(idx);

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

        (void)x;

        (void)y;

        return false;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_ || rows_.empty())

        {

            return false;

        }

        if (key == XK_Down)

        {

            if (selected_row_ < (int)rows_.size() - 1)

            {

                selected_row_ += 1;

            }

            keep_selection_visible();

            if (on_row_selected && selected_row_ >= 0)

            {

                on_row_selected(selected_row_);

            }

            return true;

        }

        if (key == XK_Up)

        {

            if (selected_row_ > 0)

            {

                selected_row_ -= 1;

            }

            keep_selection_visible();

            if (on_row_selected && selected_row_ >= 0)

            {

                on_row_selected(selected_row_);

            }

            return true;

        }

        if (key == XK_Page_Down)

        {

            selected_row_ += visible_row_count();

            if (selected_row_ >= (int)rows_.size())

            {

                selected_row_ = (int)rows_.size() - 1;

            }

            keep_selection_visible();

            return true;

        }

        if (key == XK_Page_Up)

        {

            selected_row_ -= visible_row_count();

            if (selected_row_ < 0)

            {

                selected_row_ = 0;

            }

            keep_selection_visible();

            return true;

        }

        return false;

    }

};

//endregion
