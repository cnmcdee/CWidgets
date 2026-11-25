#pragma once

#include "core/xframe_core.h"

//region TValueListEditor

class TValueListEditor

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

    struct Row

    {

        std::string key;

        std::string value;

    };

    std::vector<Row> rows_;

    int key_width_ = 110;

    int row_height_ = 18;

    int sel_row_ = 0;

    bool editing_ = false;

    std::string edit_buffer_;

    int edit_cursor_ = 0;

    void commit_edit()

    {

        if (!editing_)

        {

            return;

        }

        editing_ = false;

        if (sel_row_ >= 0 && sel_row_ < (int)rows_.size())

        {

            rows_[sel_row_].value = edit_buffer_;

            if (on_change)

            {

                on_change(sel_row_);

            }

        }

    }

    void cancel_edit()

    {

        editing_ = false;

    }

    void start_edit()

    {

        if (sel_row_ < 0 || sel_row_ >= (int)rows_.size())

        {

            return;

        }

        editing_ = true;

        edit_buffer_ = rows_[sel_row_].value;

        edit_cursor_ = (int)edit_buffer_.size();

    }

    int row_at(int y)

    {

        int top = y1_ + row_height_;

        if (y < top)

        {

            return -1;

        }

        int row = (y - top) / row_height_;

        if (row < 0 || row >= (int)rows_.size())

        {

            return -1;

        }

        return row;

    }

public:

    std::function<void(int)> on_change;

    TValueListEditor(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TValueListEditor";

        text_ = "ValueListEditor";

        rows_.push_back(Row{"Name", "Widget"});

        rows_.push_back(Row{"Width", "320"});

        rows_.push_back(Row{"Height", "240"});

        rows_.push_back(Row{"Caption", "Hello"});

        rows_.push_back(Row{"Color", "Default"});

        rows_.push_back(Row{"Enabled", "True"});

    }

    ~TValueListEditor() {}

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

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        unsigned long white = styles_->get_color_rgb(255, 255, 255);

        unsigned long header_fill = styles_->get_color_rgb(210, 210, 210);

        unsigned long sel_fill = styles_->get_color_rgb(180, 205, 240);

        unsigned long grid = styles_->get_color_rgb(170, 170, 170);

        if (!enabled_)

        {

            st.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        XSetForeground(display_, gc_, white);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        // Header row, shaded like fixed cells.

        XSetForeground(display_, gc_, header_fill);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), row_height_);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 4, y1_, y1_ + row_height_, "Key");

        xf::draw_text_left(display_, window_, gc_, st, x1_ + key_width_ + 4, y1_, y1_ + row_height_, "Value");

        XSetForeground(display_, gc_, grid);

        XDrawLine(display_, window_, gc_, x1_, y1_ + row_height_, x2_, y1_ + row_height_);

        XDrawLine(display_, window_, gc_, x1_ + key_width_, y1_, x2_ < x1_ + key_width_ ? x1_ : x1_ + key_width_, y2_);

        for (int i = 0; i < (int)rows_.size(); ++i)

        {

            int ry1 = y1_ + row_height_ + i * row_height_;

            int ry2 = ry1 + row_height_;

            if (ry1 > y2_)

            {

                break;

            }

            if (i == sel_row_)

            {

                XSetForeground(display_, gc_, sel_fill);

                XFillRectangle(display_, window_, gc_, x1_ + 1, ry1 + 1, width() - 2, row_height_ - 1);

            }

            xf::draw_text_left(display_, window_, gc_, st, x1_ + 4, ry1, ry2, rows_[i].key);

            if (i == sel_row_ && editing_)

            {

                XSetForeground(display_, gc_, white);

                XFillRectangle(display_, window_, gc_, x1_ + key_width_ + 1, ry1 + 1, width() - key_width_ - 2, row_height_ - 1);

                xf::draw_text_left(display_, window_, gc_, st, x1_ + key_width_ + 4, ry1, ry2, edit_buffer_);

                if (focused_)

                {

                    std::string before = edit_buffer_.substr(0, edit_cursor_);

                    int cx = x1_ + key_width_ + 4 + xf::text_width(st, before);

                    XSetForeground(display_, gc_, styles_->get_color_rgb(0, 0, 0));

                    XDrawLine(display_, window_, gc_, cx, ry1 + 3, cx, ry2 - 3);

                }

            }

            else

            {

                xf::draw_text_left(display_, window_, gc_, st, x1_ + key_width_ + 4, ry1, ry2, rows_[i].value);

                if (i == sel_row_ && focused_)

                {

                    XSetForeground(display_, gc_, st.lineColor_);

                    XDrawRectangle(display_, window_, gc_, x1_ + key_width_ + 1, ry1 + 1, width() - key_width_ - 3, row_height_ - 3);

                }

            }

            XSetForeground(display_, gc_, grid);

            XDrawLine(display_, window_, gc_, x1_, ry2, x2_, ry2);

        }

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        int row = row_at(y);

        if (row >= 0)

        {

            bool was_editing_same = editing_ && row == sel_row_;

            if (editing_ && row != sel_row_)

            {

                commit_edit();

            }

            sel_row_ = row;

            if (was_editing_same && x > x1_ + key_width_)

            {

                // Position the edit cursor from the click.

                SStyle st = style();

                int best = (int)edit_buffer_.size();

                for (int i = 0; i <= (int)edit_buffer_.size(); ++i)

                {

                    int cx = x1_ + key_width_ + 4 + xf::text_width(st, edit_buffer_.substr(0, i));

                    if (cx >= x)

                    {

                        best = i;

                        break;

                    }

                }

                edit_cursor_ = best;

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Up)

        {

            commit_edit();

            if (sel_row_ > 0)

            {

                sel_row_--;

            }

            return true;

        }

        if (key == XK_Down)

        {

            commit_edit();

            if (sel_row_ + 1 < (int)rows_.size())

            {

                sel_row_++;

            }

            return true;

        }

        if (key == XK_Insert)

        {

            commit_edit();

            int at = sel_row_ + 1;

            if (at < 0 || at > (int)rows_.size())

            {

                at = (int)rows_.size();

            }

            rows_.insert(rows_.begin() + at, Row{"", ""});

            sel_row_ = at;

            return true;

        }

        if (editing_)

        {

            if (key == XK_Return || key == XK_KP_Enter || key == XK_Tab || key == XK_ISO_Left_Tab)

            {

                commit_edit();

                if (sel_row_ + 1 < (int)rows_.size())

                {

                    sel_row_++;

                }

                return true;

            }

            if (key == XK_Escape)

            {

                cancel_edit();

                return true;

            }

            if (key == XK_BackSpace)

            {

                if (edit_cursor_ > 0)

                {

                    edit_buffer_.erase(edit_cursor_ - 1, 1);

                    edit_cursor_--;

                }

                return true;

            }

            if (key == XK_Delete)

            {

                if (edit_cursor_ < (int)edit_buffer_.size())

                {

                    edit_buffer_.erase(edit_cursor_, 1);

                }

                return true;

            }

            if (key == XK_Left)

            {

                if (edit_cursor_ > 0)

                {

                    edit_cursor_--;

                }

                return true;

            }

            if (key == XK_Right)

            {

                if (edit_cursor_ < (int)edit_buffer_.size())

                {

                    edit_cursor_++;

                }

                return true;

            }

            if (key == XK_Home)

            {

                edit_cursor_ = 0;

                return true;

            }

            if (key == XK_End)

            {

                edit_cursor_ = (int)edit_buffer_.size();

                return true;

            }

            if (str && str[0] >= 32 && str[0] < 127)

            {

                edit_buffer_.insert(edit_cursor_, 1, str[0]);

                edit_cursor_++;

                return true;

            }

            return false;

        }

        if (key == XK_F2 || key == XK_Return || key == XK_KP_Enter)

        {

            start_edit();

            return true;

        }

        if (key == XK_Delete)

        {

            // Not editing: Delete does not remove the row (LCL behaviour).

            return true;

        }

        if (str && str[0] >= 32 && str[0] < 127)

        {

            start_edit();

            edit_buffer_.clear();

            edit_buffer_.push_back(str[0]);

            edit_cursor_ = 1;

            return true;

        }

        return false;

    }

    //region TValueListEditor API

    void set_key_width(int w)

    {

        key_width_ = w < 20 ? 20 : w;

    }

    int get_key_width()

    {

        return key_width_;

    }

    int get_count()

    {

        return (int)rows_.size();

    }

    int get_selected_row()

    {

        return sel_row_;

    }

    void set_selected_row(int i)

    {

        if (i >= 0 && i < (int)rows_.size())

        {

            sel_row_ = i;

        }

    }

    bool get_editing()

    {

        return editing_;

    }

    void insert_row(std::string key, std::string value)

    {

        rows_.push_back(Row{key, value});

    }

    void delete_row(int i)

    {

        if (i < 0 || i >= (int)rows_.size())

        {

            return;

        }

        rows_.erase(rows_.begin() + i);

        if (sel_row_ >= (int)rows_.size())

        {

            sel_row_ = (int)rows_.size() - 1;

        }

        if (sel_row_ < 0)

        {

            sel_row_ = 0;

        }

    }

    std::string get_key(int i)

    {

        if (i < 0 || i >= (int)rows_.size())

        {

            return "";

        }

        return rows_[i].key;

    }

    std::string get_value(int i)

    {

        if (i < 0 || i >= (int)rows_.size())

        {

            return "";

        }

        return rows_[i].value;

    }

    void set_value(int i, std::string v)

    {

        if (i < 0 || i >= (int)rows_.size())

        {

            return;

        }

        rows_[i].value = v;

        if (on_change)

        {

            on_change(i);

        }

    }

    int find_key(std::string name)

    {

        for (int i = 0; i < (int)rows_.size(); ++i)

        {

            if (rows_[i].key == name)

            {

                return i;

            }

        }

        return -1;

    }

    std::string get_value_by_key(std::string name)

    {

        int i = find_key(name);

        return i >= 0 ? rows_[i].value : std::string("");

    }

    //endregion

};

//endregion
