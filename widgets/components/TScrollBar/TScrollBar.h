#pragma once

#include "core/xframe_core.h"

//region TScrollBar

class TScrollBar

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

    int kind_ = 0;

    int min_ = 0;

    int max_ = 100;

    int position_ = 0;

    int page_size_ = 20;

    int small_change_ = 1;

    int large_change_ = 20;

    bool dragging_ = false;

    int drag_offset_ = 0;

    int arrow_size()

    {

        return 16;

    }

    int range()

    {

        int r = max_ - min_;

        if (r < 0)

        {

            r = 0;

        }

        return r;

    }

    int track_length()

    {

        int len = (kind_ == 1 ? width() : height()) - 2 * arrow_size();

        if (len < 8)

        {

            len = 8;

        }

        return len;

    }

    int thumb_size()

    {

        int len = track_length();

        int total = range() + page_size_;

        if (total <= 0)

        {

            return len;

        }

        int size = len * page_size_ / total;

        if (size < 12)

        {

            size = 12;

        }

        if (size > len)

        {

            size = len;

        }

        return size;

    }

    int thumb_pos()

    {

        int len = track_length();

        int travel = len - thumb_size();

        if (range() <= 0 || travel <= 0)

        {

            return 0;

        }

        return travel * (position_ - min_) / range();

    }

    void thumb_rect(int& tx1, int& ty1, int& tx2, int& ty2)

    {

        if (kind_ == 1)

        {

            tx1 = x1_ + arrow_size() + thumb_pos();

            tx2 = tx1 + thumb_size();

            ty1 = y1_ + 1;

            ty2 = y2_ - 1;

        }

        else

        {

            tx1 = x1_ + 1;

            tx2 = x2_ - 1;

            ty1 = y1_ + arrow_size() + thumb_pos();

            ty2 = ty1 + thumb_size();

        }

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

    TScrollBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TScrollBar";

        text_ = "ScrollBar";

        kind_ = 0;

    }

    ~TScrollBar() {}

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

    //region Range API

    void set_kind(int k)

    {

        kind_ = k;

    }

    int get_kind()

    {

        return kind_;

    }

    void set_min(int v)

    {

        min_ = v;

        set_position(position_);

    }

    int get_min()

    {

        return min_;

    }

    void set_max(int v)

    {

        max_ = v;

        set_position(position_);

    }

    int get_max()

    {

        return max_;

    }

    void set_range(int lo, int hi)

    {

        min_ = lo;

        max_ = hi;

        set_position(position_);

    }

    void set_position(int v)

    {

        if (v < min_)

        {

            v = min_;

        }

        if (v > max_)

        {

            v = max_;

        }

        if (v != position_)

        {

            position_ = v;

            fire_change();

        }

        else

        {

            position_ = v;

        }

    }

    int get_position()

    {

        return position_;

    }

    void set_page_size(int v)

    {

        if (v < 1)

        {

            v = 1;

        }

        page_size_ = v;

        large_change_ = v;

    }

    int get_page_size()

    {

        return page_size_;

    }

    void set_small_change(int v)

    {

        small_change_ = v;

    }

    int get_small_change()

    {

        return small_change_;

    }

    void set_large_change(int v)

    {

        large_change_ = v;

    }

    int get_large_change()

    {

        return large_change_;

    }

    bool is_dragging()

    {

        return dragging_;

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

        XSetForeground(display_, gc_, styles_->get_color_rgb(225, 225, 225));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        xf::draw_rounded_outline(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        if (kind_ == 1)

        {

            xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x1_ + arrow_size(), y2_);

            xf::draw_polished(display_, window_, gc_, st, x2_ - arrow_size(), y1_, x2_, y2_);

            xf::draw_triangle_right(display_, window_, gc_, st.text_color_, x2_ - arrow_size() / 2 - 3, (y1_ + y2_) / 2, 4);

            XSetForeground(display_, gc_, st.text_color_);

            for (int i = 0; i < 5; ++i)

            {

                XDrawLine(display_, window_, gc_, x1_ + arrow_size() / 2 + (5 - i), (y1_ + y2_) / 2 - i, x1_ + arrow_size() / 2 + (5 - i), (y1_ + y2_) / 2 + i);

            }

        }

        else

        {

            xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y1_ + arrow_size());

            xf::draw_polished(display_, window_, gc_, st, x1_, y2_ - arrow_size(), x2_, y2_);

            xf::draw_triangle_up(display_, window_, gc_, st.text_color_, (x1_ + x2_) / 2, y1_ + arrow_size() / 2 + 3, 5);

            xf::draw_triangle_down(display_, window_, gc_, st.text_color_, (x1_ + x2_) / 2, y2_ - arrow_size() / 2 - 3, 5);

        }

        int tx1 = 0;

        int ty1 = 0;

        int tx2 = 0;

        int ty2 = 0;

        thumb_rect(tx1, ty1, tx2, ty2);

        SStyle thumb = st;

        if (dragging_)

        {

            unsigned long swap = thumb.fillColor_;

            thumb.fillColor_ = thumb.outlineColor_;

            thumb.outlineColor_ = swap;

        }

        xf::draw_polished(display_, window_, gc_, thumb, tx1, ty1, tx2, ty2);

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

            set_position(position_ - small_change_);

            return true;

        }

        if (button == 5)

        {

            set_position(position_ + small_change_);

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        int tx1 = 0;

        int ty1 = 0;

        int tx2 = 0;

        int ty2 = 0;

        thumb_rect(tx1, ty1, tx2, ty2);

        if (x >= tx1 && x <= tx2 && y >= ty1 && y <= ty2)

        {

            dragging_ = true;

            drag_offset_ = (kind_ == 1) ? (x - tx1) : (y - ty1);

            return true;

        }

        if (kind_ == 1)

        {

            if (x < x1_ + arrow_size())

            {

                set_position(position_ - small_change_);

            }

            else if (x >= x2_ - arrow_size())

            {

                set_position(position_ + small_change_);

            }

            else if (x < tx1)

            {

                set_position(position_ - large_change_);

            }

            else

            {

                set_position(position_ + large_change_);

            }

        }

        else

        {

            if (y < y1_ + arrow_size())

            {

                set_position(position_ - small_change_);

            }

            else if (y >= y2_ - arrow_size())

            {

                set_position(position_ + small_change_);

            }

            else if (y < ty1)

            {

                set_position(position_ - large_change_);

            }

            else

            {

                set_position(position_ + large_change_);

            }

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button != 1)

        {

            return false;

        }

        if (dragging_)

        {

            dragging_ = false;

            return true;

        }

        return false;

    }

    bool on_motion(int x, int y)

    {

        if (!dragging_ || !enabled_)

        {

            return false;

        }

        int travel = track_length() - thumb_size();

        if (travel <= 0 || range() <= 0)

        {

            return true;

        }

        int coord = (kind_ == 1) ? x : y;

        int origin = (kind_ == 1) ? x1_ : y1_;

        int rel = coord - origin - arrow_size() - drag_offset_;

        int new_pos = min_ + rel * range() / travel;

        set_position(new_pos);

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Up || key == XK_Left)

        {

            set_position(position_ - small_change_);

            return true;

        }

        if (key == XK_Down || key == XK_Right)

        {

            set_position(position_ + small_change_);

            return true;

        }

        if (key == XK_Page_Up)

        {

            set_position(position_ - large_change_);

            return true;

        }

        if (key == XK_Page_Down)

        {

            set_position(position_ + large_change_);

            return true;

        }

        if (key == XK_Home)

        {

            set_position(min_);

            return true;

        }

        if (key == XK_End)

        {

            set_position(max_);

            return true;

        }

        return false;

    }

};

//endregion
