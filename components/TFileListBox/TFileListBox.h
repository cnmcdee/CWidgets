#pragma once

#include "core/xframe_core.h"

//region TFileListBox

class TFileListBox

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

    struct SFileItem

    {

        std::string name;

        bool is_dir;

    };

    std::vector<SFileItem> all_items_

    {

        {"src", true}, {"tests", true}, {"document.txt", false}, {"notes.txt", false},

        {"main.cpp", false}, {"xframe_core.h", false}, {"Makefile", false}, {"photo.png", false},

        {"archive.zip", false}, {"readme.md", false}, {"data.csv", false}, {"build.sh", false},

        {"song.mp3", false}, {"report.pdf", false}, {"todo.txt", false}, {"widget.cpp", false}

    };

    std::vector<SFileItem> items_;   // after the mask filter

    int item_index_ = 0;

    int top_index_ = 0;

    std::string mask_ = "*.*";

    // Lazarus TFileType bits, kept as state: which kinds are listed.

    int file_type_ = 1;   // bit0 files, bit1 directories, bit2 hidden

    bool show_hidden_ = false;

    int header_h_ = 18;

    int row_h_ = 16;

    static bool ends_with(const std::string& s, const std::string& suffix)

    {

        return s.size() >= suffix.size() && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;

    }

    static bool match_one(const std::string& name, std::string pattern)

    {

        if (pattern == "*" || pattern == "*.*")

        {

            return true;

        }

        if (pattern.size() > 2 && pattern[0] == '*' && pattern[1] == '.')

        {

            return ends_with(name, pattern.substr(1));

        }

        return name == pattern;

    }

    bool matches_mask(const std::string& name)

    {

        size_t start = 0;

        while (start <= mask_.size())

        {

            size_t semi = mask_.find(';', start);

            std::string part = mask_.substr(start, semi == std::string::npos ? std::string::npos : semi - start);

            if (match_one(name, part))

            {

                return true;

            }

            if (semi == std::string::npos)

            {

                break;

            }

            start = semi + 1;

        }

        return false;

    }

    void refilter()

    {

        items_.clear();

        for (auto& it : all_items_)

        {

            if (it.is_dir && !(file_type_ & 2))

            {

                continue;

            }

            if (!it.is_dir && !(file_type_ & 1))

            {

                continue;

            }

            if (it.is_dir || matches_mask(it.name))

            {

                items_.push_back(it);

            }

        }

        if (item_index_ >= (int)items_.size())

        {

            item_index_ = (int)items_.size() - 1;

        }

        if (item_index_ < 0 && !items_.empty())

        {

            item_index_ = 0;

        }

        top_index_ = 0;

    }

    int visible_rows()

    {

        return std::max(1, (height() - header_h_) / row_h_);

    }

    void keep_visible()

    {

        if (item_index_ < top_index_)

        {

            top_index_ = item_index_;

        }

        if (item_index_ >= top_index_ + visible_rows())

        {

            top_index_ = item_index_ - visible_rows() + 1;

        }

    }

    void select(int i)

    {

        if (i < 0 || i >= (int)items_.size() || i == item_index_)

        {

            item_index_ = std::max(-1, std::min(i, (int)items_.size() - 1));

            keep_visible();

            return;

        }

        item_index_ = i;

        keep_visible();

        text_ = items_[i].name;

        if (on_change)

        {

            on_change();

        }

    }

public:

    std::function<void()> on_click;

    std::function<void()> on_change;

    std::function<void()> on_dbl_click;

    TFileListBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TFileListBox";

        file_type_ = 3;   // files + directories

        refilter();

        item_index_ = 2;   // document.txt

        text_ = items_.empty() ? "" : items_[item_index_].name;

    }

    ~TFileListBox() {}

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

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            if (items_[i].name == t)

            {

                select(i);

                return;

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

    std::string get_filename()

    {

        if (item_index_ >= 0 && item_index_ < (int)items_.size())

        {

            return items_[item_index_].name;

        }

        return "";

    }

    int get_item_index()

    {

        return item_index_;

    }

    int get_count()

    {

        return (int)items_.size();

    }

    void set_mask(std::string m)

    {

        mask_ = m;

        refilter();

    }

    std::string get_mask()

    {

        return mask_;

    }

    void set_file_type(int ft)

    {

        file_type_ = ft;

        refilter();

    }

    int get_file_type()

    {

        return file_type_;

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

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        // Header bar showing the active mask.

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 2, header_h_ - 1);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 4, y1_, y1_ + header_h_, "Mask: " + mask_);

        int rows = visible_rows();

        for (int r = 0; r < rows; ++r)

        {

            int i = top_index_ + r;

            if (i >= (int)items_.size())

            {

                break;

            }

            int ry = y1_ + header_h_ + r * row_h_;

            if (i == item_index_)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, x1_ + 1, ry, width() - 2, row_h_);

            }

            SStyle rs = st;

            if (i == item_index_)

            {

                rs.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            std::string label = items_[i].is_dir ? "[" + items_[i].name + "]" : items_[i].name;

            xf::draw_text_left(display_, window_, gc_, rs, x1_ + 4, ry, ry + row_h_, label);

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

        if (button == 4)   // wheel up

        {

            if (top_index_ > 0)

            {

                --top_index_;

            }

            return true;

        }

        if (button == 5)   // wheel down

        {

            if (top_index_ + visible_rows() < (int)items_.size())

            {

                ++top_index_;

            }

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        if (y >= y1_ + header_h_)

        {

            int i = top_index_ + (y - y1_ - header_h_) / row_h_;

            if (i >= 0 && i < (int)items_.size())

            {

                bool was = (i == item_index_);

                select(i);

                if (was && on_dbl_click)

                {

                    on_dbl_click();

                }

                if (on_click)

                {

                    on_click();

                }

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Up)

        {

            select(item_index_ - 1);

            return true;

        }

        if (key == XK_Down)

        {

            select(item_index_ + 1);

            return true;

        }

        if (key == XK_Page_Up)

        {

            select(item_index_ - visible_rows());

            return true;

        }

        if (key == XK_Page_Down)

        {

            select(item_index_ + visible_rows());

            return true;

        }

        if (key == XK_Home)

        {

            select(0);

            return true;

        }

        if (key == XK_End)

        {

            select((int)items_.size() - 1);

            return true;

        }

        return false;

    }

};

//endregion
