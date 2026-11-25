#pragma once

#include "core/xframe_core.h"

//region TMemo

class TMemo

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

    std::vector<std::string> lines_;

    int cursor_line_ = 0;

    int cursor_col_ = 0;

    int top_line_ = 0;   // vertical scroll offset, in lines

    bool wordwrap_ = true;

    int padding_ = 4;

    int line_height()

    {

        SStyle st = style();

        if (st.font_)

        {

            return st.font_->ascent + st.font_->descent + 3;

        }

        return 16;

    }

    int visible_line_count()

    {

        int count = (height() - 2 * padding_) / line_height();

        if (count < 1)

        {

            count = 1;

        }

        return count;

    }

    void clamp_cursor()

    {

        if (lines_.empty())

        {

            lines_.push_back("");

        }

        if (cursor_line_ < 0)

        {

            cursor_line_ = 0;

        }

        if (cursor_line_ >= (int)lines_.size())

        {

            cursor_line_ = (int)lines_.size() - 1;

        }

        if (cursor_col_ < 0)

        {

            cursor_col_ = 0;

        }

        if (cursor_col_ > (int)lines_[cursor_line_].size())

        {

            cursor_col_ = (int)lines_[cursor_line_].size();

        }

    }

    void ensure_cursor_visible()

    {

        clamp_cursor();

        if (cursor_line_ < top_line_)

        {

            top_line_ = cursor_line_;

        }

        int last = top_line_ + visible_line_count() - 1;

        if (cursor_line_ > last)

        {

            top_line_ = cursor_line_ - visible_line_count() + 1;

        }

        if (top_line_ < 0)

        {

            top_line_ = 0;

        }

    }

    void fire_change()

    {

        ensure_cursor_visible();

        sync_text();

        if (on_change)

        {

            on_change();

        }

    }

    void sync_text()

    {

        text_.clear();

        for (size_t i = 0; i < lines_.size(); i++)

        {

            if (i > 0)

            {

                text_ += "\n";

            }

            text_ += lines_[i];

        }

    }

public:

    std::function<void()> on_change;

    std::function<void()> on_click;

    TMemo(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TMemo";

        lines_.push_back("Memo line one.");

        lines_.push_back("Type here - Enter makes");

        lines_.push_back("a new line, BackSpace at");

        lines_.push_back("the start joins lines.");

        lines_.push_back("");

        lines_.push_back("Arrow keys move the cursor.");

        cursor_line_ = 0;

        cursor_col_ = 0;

        top_line_ = 0;

        wordwrap_ = true;

        sync_text();

    }

    ~TMemo() {}

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

        lines_.clear();

        std::string line;

        for (size_t i = 0; i <= t.size(); i++)

        {

            if (i == t.size() || t[i] == '\n')

            {

                lines_.push_back(line);

                line.clear();

            }

            else if (t[i] != '\r')

            {

                line += t[i];

            }

        }

        if (lines_.empty())

        {

            lines_.push_back("");

        }

        cursor_line_ = 0;

        cursor_col_ = 0;

        top_line_ = 0;

        sync_text();

    }

    std::string get_text()

    {

        sync_text();

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    std::vector<std::string>& lines()

    {

        return lines_;

    }

    int line_count()

    {

        return (int)lines_.size();

    }

    std::string get_line(int index)

    {

        if (index < 0 || index >= (int)lines_.size())

        {

            return "";

        }

        return lines_[index];

    }

    void add_line(std::string line)

    {

        lines_.push_back(line);

        sync_text();

    }

    void clear()

    {

        lines_.clear();

        lines_.push_back("");

        cursor_line_ = 0;

        cursor_col_ = 0;

        top_line_ = 0;

        fire_change();

    }

    int get_cursor_line()

    {

        return cursor_line_;

    }

    int get_cursor_col()

    {

        return cursor_col_;

    }

    int get_top_line()

    {

        return top_line_;

    }

    void set_wordwrap(bool wrap)

    {

        wordwrap_ = wrap;

    }

    bool get_wordwrap()

    {

        return wordwrap_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // White inset field, same as TEdit.

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, styles_->get_color_rgb(120, 120, 120));

        XDrawLine(display_, window_, gc_, x1_, y1_, x2_ - 1, y1_);

        XDrawLine(display_, window_, gc_, x1_, y1_, x1_, y2_ - 1);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        if (!enabled_)

        {

            st.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        int lh = line_height();

        int count = visible_line_count();

        XRectangle clip;

        clip.x = (short)(x1_ + 1);

        clip.y = (short)(y1_ + 1);

        clip.width = (unsigned short)(width() - 2);

        clip.height = (unsigned short)(height() - 2);

        XSetClipRectangles(display_, gc_, 0, 0, &clip, 1, Unsorted);

        for (int row = 0; row < count; row++)

        {

            int index = top_line_ + row;

            if (index >= (int)lines_.size())

            {

                break;

            }

            int baseline = y1_ + padding_ + row * lh + (st.font_ ? st.font_->ascent : 12);

            xf::draw_text_at(display_, window_, gc_, st, x1_ + padding_, baseline, lines_[index]);

        }

        if (focused_ && enabled_)

        {

            int row = cursor_line_ - top_line_;

            if (row >= 0 && row < count)

            {

                std::string before = lines_[cursor_line_].substr(0, cursor_col_);

                int cursor_x = x1_ + padding_ + xf::text_width(st, before);

                int cursor_top = y1_ + padding_ + row * lh;

                XSetForeground(display_, gc_, st.text_color_);

                xf::set_line_width(display_, gc_, 1);

                XDrawLine(display_, window_, gc_, cursor_x, cursor_top, cursor_x, cursor_top + lh - 3);

            }

        }

        XSetClipMask(display_, gc_, None);

        // Scrollbar hint on the right edge when the text overflows.

        if ((int)lines_.size() > count)

        {

            int track_x = x2_ - 9;

            XSetForeground(display_, gc_, styles_->get_color_rgb(225, 225, 225));

            XFillRectangle(display_, window_, gc_, track_x, y1_ + 2, 6, height() - 4);

            int thumb_h = (height() - 4) * count / (int)lines_.size();

            if (thumb_h < 12)

            {

                thumb_h = 12;

            }

            int thumb_y = y1_ + 2 + (height() - 4 - thumb_h) * top_line_ / std::max(1, (int)lines_.size() - count);

            XSetForeground(display_, gc_, styles_->get_color_rgb(160, 160, 160));

            XFillRectangle(display_, window_, gc_, track_x, thumb_y, 6, thumb_h);

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

        int row = (y - y1_ - padding_) / line_height();

        if (row < 0)

        {

            row = 0;

        }

        cursor_line_ = top_line_ + row;

        clamp_cursor();

        // Column nearest the click on that line.

        SStyle st = style();

        int target_x = x - x1_ - padding_;

        int col = 0;

        while (col < (int)lines_[cursor_line_].size())

        {

            std::string before = lines_[cursor_line_].substr(0, col + 1);

            if (xf::text_width(st, before) > target_x)

            {

                break;

            }

            col++;

        }

        cursor_col_ = col;

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

        clamp_cursor();

        if (key == XK_Return || key == XK_KP_Enter)

        {

            std::string tail = lines_[cursor_line_].substr(cursor_col_);

            lines_[cursor_line_] = lines_[cursor_line_].substr(0, cursor_col_);

            lines_.insert(lines_.begin() + cursor_line_ + 1, tail);

            cursor_line_++;

            cursor_col_ = 0;

            fire_change();

            return true;

        }

        if (key == XK_BackSpace)

        {

            if (cursor_col_ > 0)

            {

                lines_[cursor_line_].erase(cursor_col_ - 1, 1);

                cursor_col_--;

                fire_change();

            }

            else if (cursor_line_ > 0)

            {

                int join_at = (int)lines_[cursor_line_ - 1].size();

                lines_[cursor_line_ - 1] += lines_[cursor_line_];

                lines_.erase(lines_.begin() + cursor_line_);

                cursor_line_--;

                cursor_col_ = join_at;

                fire_change();

            }

            return true;

        }

        if (key == XK_Delete)

        {

            if (cursor_col_ < (int)lines_[cursor_line_].size())

            {

                lines_[cursor_line_].erase(cursor_col_, 1);

                fire_change();

            }

            else if (cursor_line_ + 1 < (int)lines_.size())

            {

                lines_[cursor_line_] += lines_[cursor_line_ + 1];

                lines_.erase(lines_.begin() + cursor_line_ + 1);

                fire_change();

            }

            return true;

        }

        if (key == XK_Left)

        {

            if (cursor_col_ > 0)

            {

                cursor_col_--;

            }

            else if (cursor_line_ > 0)

            {

                cursor_line_--;

                cursor_col_ = (int)lines_[cursor_line_].size();

            }

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Right)

        {

            if (cursor_col_ < (int)lines_[cursor_line_].size())

            {

                cursor_col_++;

            }

            else if (cursor_line_ + 1 < (int)lines_.size())

            {

                cursor_line_++;

                cursor_col_ = 0;

            }

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Up)

        {

            if (cursor_line_ > 0)

            {

                cursor_line_--;

            }

            clamp_cursor();

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Down)

        {

            if (cursor_line_ + 1 < (int)lines_.size())

            {

                cursor_line_++;

            }

            clamp_cursor();

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Home)

        {

            cursor_col_ = 0;

            return true;

        }

        if (key == XK_End)

        {

            cursor_col_ = (int)lines_[cursor_line_].size();

            return true;

        }

        if (key == XK_Page_Up)

        {

            cursor_line_ -= visible_line_count();

            clamp_cursor();

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Page_Down)

        {

            cursor_line_ += visible_line_count();

            clamp_cursor();

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Tab)

        {

            lines_[cursor_line_].insert(cursor_col_, "  ");

            cursor_col_ += 2;

            fire_change();

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

                lines_[cursor_line_].insert(cursor_col_, insert);

                cursor_col_ += (int)insert.size();

                fire_change();

            }

            return true;

        }

        return false;

    }

};

//endregion
