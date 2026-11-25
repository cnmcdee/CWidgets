#pragma once

#include "core/xframe_core.h"

//region TEdit

class TEdit

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;   // the real edit buffer

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    int cursor_pos_ = 0;   // insertion point, 0..text_.size()

    int scroll_offset_ = 0;   // first visible character

    int padding_ = 4;

    void ensure_cursor_visible()

    {

        if (cursor_pos_ < scroll_offset_)

        {

            scroll_offset_ = cursor_pos_;

        }

        SStyle st = style();

        int inner = width() - 2 * padding_ - 2;

        while (scroll_offset_ < cursor_pos_)

        {

            std::string shown = text_.substr(scroll_offset_, cursor_pos_ - scroll_offset_);

            if (xf::text_width(st, shown) <= inner)

            {

                break;

            }

            scroll_offset_++;

        }

        if (scroll_offset_ > (int)text_.size())

        {

            scroll_offset_ = (int)text_.size();

        }

    }

    void fire_change()

    {

        ensure_cursor_visible();

        if (on_change)

        {

            on_change();

        }

    }

public:

    std::function<void()> on_change;

    std::function<void()> on_click;

    TEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TEdit";

        text_ = "Edit me";

        cursor_pos_ = (int)text_.size();

        scroll_offset_ = 0;

    }

    ~TEdit() {}

    void set_bounds(int x, int y, int width, int height)

    {

        x1_ = x;

        y1_ = y;

        x2_ = x + width;

        y2_ = y + height;

        ensure_cursor_visible();

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

        cursor_pos_ = (int)text_.size();

        scroll_offset_ = 0;

        ensure_cursor_visible();

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    int get_cursor_pos()

    {

        return cursor_pos_;

    }

    void set_cursor_pos(int pos)

    {

        if (pos < 0)

        {

            pos = 0;

        }

        if (pos > (int)text_.size())

        {

            pos = (int)text_.size();

        }

        cursor_pos_ = pos;

        ensure_cursor_visible();

    }

    int get_scroll_offset()

    {

        return scroll_offset_;

    }

    void clear()

    {

        text_.clear();

        cursor_pos_ = 0;

        scroll_offset_ = 0;

        fire_change();

    }

    void select_all()

    {

        cursor_pos_ = (int)text_.size();

        ensure_cursor_visible();

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // White inset field.

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        // Sunken inset edge: dark top/left, light bottom/right.

        XSetForeground(display_, gc_, styles_->get_color_rgb(120, 120, 120));

        XDrawLine(display_, window_, gc_, x1_, y1_, x2_ - 1, y1_);

        XDrawLine(display_, window_, gc_, x1_, y1_, x1_, y2_ - 1);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        if (!enabled_)

        {

            st.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        // Clip the text to the field so a long buffer never spills out.

        XRectangle clip;

        clip.x = (short)(x1_ + 1);

        clip.y = (short)(y1_ + 1);

        clip.width = (unsigned short)(width() - 2);

        clip.height = (unsigned short)(height() - 2);

        XSetClipRectangles(display_, gc_, 0, 0, &clip, 1, Unsorted);

        std::string shown = "";

        if (scroll_offset_ <= (int)text_.size())

        {

            shown = text_.substr(scroll_offset_);

        }

        xf::draw_text_left(display_, window_, gc_, st, x1_ + padding_, y1_, y2_, shown);

        if (focused_ && enabled_)

        {

            std::string before = text_.substr(scroll_offset_, cursor_pos_ - scroll_offset_);

            int cursor_x = x1_ + padding_ + xf::text_width(st, before);

            XSetForeground(display_, gc_, st.text_color_);

            xf::set_line_width(display_, gc_, 1);

            XDrawLine(display_, window_, gc_, cursor_x, y1_ + 3, cursor_x, y2_ - 4);

        }

        XSetClipMask(display_, gc_, None);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        // Place the cursor on the character nearest the click.

        SStyle st = style();

        int target_x = x - x1_ - padding_;

        int pos = scroll_offset_;

        while (pos < (int)text_.size())

        {

            std::string before = text_.substr(scroll_offset_, pos - scroll_offset_ + 1);

            if (xf::text_width(st, before) > target_x)

            {

                break;

            }

            pos++;

        }

        cursor_pos_ = pos;

        ensure_cursor_visible();

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_BackSpace)

        {

            if (cursor_pos_ > 0)

            {

                text_.erase(cursor_pos_ - 1, 1);

                cursor_pos_--;

                fire_change();

            }

            return true;

        }

        if (key == XK_Delete)

        {

            if (cursor_pos_ < (int)text_.size())

            {

                text_.erase(cursor_pos_, 1);

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

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Right)

        {

            if (cursor_pos_ < (int)text_.size())

            {

                cursor_pos_++;

            }

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Home)

        {

            cursor_pos_ = 0;

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_End)

        {

            cursor_pos_ = (int)text_.size();

            ensure_cursor_visible();

            return true;

        }

        if (str && str[0] >= 32 && str[0] != 127)

        {

            std::string insert;

            for (int i = 0; str[i] != '\0'; i++)

            {

                if (str[i] >= 32 && str[i] != 127)

                {

                    insert += str[i];

                }

            }

            if (!insert.empty())

            {

                text_.insert(cursor_pos_, insert);

                cursor_pos_ += (int)insert.size();

                fire_change();

            }

            return true;

        }

        return false;

    }

};

//endregion
