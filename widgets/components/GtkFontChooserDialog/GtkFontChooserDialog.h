#pragma once

#include "core/xframe_core.h"

#include <cctype>

//region GtkFontChooserDialog

class GtkFontChooserDialog

{

public:

    // Real GTK response ids.

    static const int RESPONSE_NONE = -1;

    static const int RESPONSE_DELETE_EVENT = -4;

    static const int RESPONSE_ACCEPT = -3;

    static const int RESPONSE_OK = -5;

    static const int RESPONSE_CANCEL = -6;

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;          // the dialog title

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    std::vector<std::string> families_;

    std::vector<std::string> styles_list_;

    std::vector<int> sizes_;

    int family_sel_ = 0;

    int style_sel_ = 0;

    int size_sel_ = 5;          // 12 pt in the default size list

    std::string filter_;        // the search entry text

    bool filter_focused_ = false;

    std::string preview_text_ = "The quick brown fox jumps over the lazy dog.";

    int response_ = RESPONSE_NONE;

    int pressed_button_ = -1;   // 0 cancel, 1 ok

    int title_bar_height()

    {

        return 26;

    }

    int action_bar_height()

    {

        return 40;

    }

    int row_height()

    {

        return 18;

    }

    // Column geometry: family | style | size.

    int col_x(int col)

    {

        if (col == 0)

        {

            return x1_ + 12;

        }

        if (col == 1)

        {

            return x1_ + 190;

        }

        return x1_ + 330;

    }

    int col_w(int col)

    {

        if (col == 0)

        {

            return 170;

        }

        if (col == 1)

        {

            return 132;

        }

        return 70;

    }

    int list_top()

    {

        return y1_ + title_bar_height() + 62;

    }

    int list_bottom()

    {

        return y2_ - action_bar_height() - 64;

    }

    // Families passing the search filter, in order.

    std::vector<int> visible_families()

    {

        std::vector<int> out;

        std::string needle = filter_;

        for (size_t i = 0; i < needle.size(); ++i)

        {

            needle[i] = std::tolower((unsigned char)needle[i]);

        }

        for (size_t i = 0; i < families_.size(); ++i)

        {

            std::string hay = families_[i];

            for (size_t j = 0; j < hay.size(); ++j)

            {

                hay[j] = std::tolower((unsigned char)hay[j]);

            }

            if (needle.empty() || hay.find(needle) != std::string::npos)

            {

                out.push_back((int)i);

            }

        }

        return out;

    }

    void draw_list(const SStyle& st, int col, const std::vector<std::string>& items, int selected)

    {

        int lx = col_x(col);

        int lw = col_w(col);

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, lx, list_top(), lw, list_bottom() - list_top());

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, lx, list_top(), lw, list_bottom() - list_top());

        int rows = (list_bottom() - list_top()) / row_height();

        for (int i = 0; i < (int)items.size() && i < rows; ++i)

        {

            int ry1 = list_top() + 1 + i * row_height();

            int ry2 = ry1 + row_height();

            if (i == selected)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, lx + 1, ry1, lw - 2, row_height() - 1);

                SStyle rst = st;

                rst.text_color_ = st.fillColor_;

                xf::draw_text_left(display_, window_, gc_, rst, lx + 6, ry1, ry2, items[i]);

            }

            else

            {

                xf::draw_text_left(display_, window_, gc_, st, lx + 6, ry1, ry2, items[i]);

            }

        }

    }

    void button_rect(int which, int &bx1, int &by1, int &bx2, int &by2)

    {

        by2 = y2_ - 7;

        by1 = by2 - 26;

        bx2 = x2_ - 10 - (1 - which) * 96;

        bx1 = bx2 - 88;

    }

    bool in_rect(int x, int y, int rx1, int ry1, int rx2, int ry2)

    {

        return x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2;

    }

public:

    std::function<void(int)> on_response;

    GtkFontChooserDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkFontChooserDialog";

        text_ = "Select a Font";

        families_ = { "Cantarell", "DejaVu Sans", "DejaVu Sans Mono", "DejaVu Serif", "Liberation Mono", "Liberation Sans", "Liberation Serif", "Noto Sans", "Noto Serif", "Ubuntu", "Ubuntu Mono" };

        styles_list_ = { "Regular", "Italic", "Bold", "Bold Italic" };

        sizes_ = { 6, 8, 9, 10, 11, 12, 14, 16, 18, 20, 24, 28, 32, 36, 48, 72 };

        family_sel_ = 1;   // DejaVu Sans

    }

    ~GtkFontChooserDialog() {}

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

    //region GtkFontChooserDialog specific API

    // GTK font names look like "DejaVu Sans Bold 12"; the style
    // word is omitted for Regular.

    std::string get_font_name()

    {

        std::string out = families_[family_sel_];

        if (styles_list_[style_sel_] != "Regular")

        {

            out += " " + styles_list_[style_sel_];

        }

        out += " " + std::to_string(sizes_[size_sel_]);

        return out;

    }

    void set_font_name(std::string font_name)

    {

        // Pull a trailing size number off, then match the longest
        // family name, then look for a style word in what remains.

        std::string rest = font_name;

        size_t sp = rest.find_last_of(' ');

        if (sp != std::string::npos)

        {

            std::string tail = rest.substr(sp + 1);

            bool numeric = !tail.empty();

            for (size_t i = 0; i < tail.size(); ++i)

            {

                if (!std::isdigit((unsigned char)tail[i]))

                {

                    numeric = false;

                }

            }

            if (numeric)

            {

                int sz = std::atoi(tail.c_str());

                for (size_t i = 0; i < sizes_.size(); ++i)

                {

                    if (sizes_[i] == sz)

                    {

                        size_sel_ = (int)i;

                    }

                }

                rest = rest.substr(0, sp);

            }

        }

        for (size_t i = 0; i < families_.size(); ++i)

        {

            if (rest.rfind(families_[i], 0) == 0)

            {

                family_sel_ = (int)i;

                std::string extra = rest.substr(families_[i].size());

                for (size_t j = 0; j < styles_list_.size(); ++j)

                {

                    if (extra.find(styles_list_[j]) != std::string::npos)

                    {

                        style_sel_ = (int)j;

                    }

                }

                return;

            }

        }

    }

    std::string get_family()

    {

        return families_[family_sel_];

    }

    std::string get_style_name()

    {

        return styles_list_[style_sel_];

    }

    int get_size()

    {

        return sizes_[size_sel_];

    }

    void set_preview_text(std::string t)

    {

        preview_text_ = t;

    }

    std::string get_preview_text()

    {

        return preview_text_;

    }

    int get_response()

    {

        return response_;

    }

    void respond(int response_id)

    {

        response_ = response_id;

        if (on_response)

        {

            on_response(response_id);

        }

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

        int tb = title_bar_height();

        // Title bar.

        XSetForeground(display_, gc_, st.outlineColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, tb - 2);

        SStyle tst = st;

        tst.text_color_ = st.fillColor_;

        xf::draw_text_left(display_, window_, gc_, tst, x1_ + 10, y1_, y1_ + tb, text_);

        xf::draw_text_centered(display_, window_, gc_, tst, x2_ - 28, y1_, x2_ - 4, y1_ + tb, "X");

        // Search entry.

        int ey = y1_ + tb + 6;

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, ey, ey + 20, "Search:");

        int ex1 = x1_ + 62;

        int ex2 = x1_ + 400 > x2_ - 12 ? x2_ - 12 : x1_ + 400;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, ex1, ey, ex2 - ex1, 20);

        XSetForeground(display_, gc_, filter_focused_ ? st.lineColor_ : styles_->get_color_rgb(150, 150, 150));

        xf::set_line_width(display_, gc_, filter_focused_ ? 2 : 1);

        XDrawRectangle(display_, window_, gc_, ex1, ey, ex2 - ex1, 20);

        xf::set_line_width(display_, gc_, 1);

        xf::draw_text_left(display_, window_, gc_, st, ex1 + 5, ey, ey + 20, filter_);

        if (filter_focused_)

        {

            int cursor_x = ex1 + 5 + xf::text_width(st, filter_);

            XSetForeground(display_, gc_, st.text_color_);

            XDrawLine(display_, window_, gc_, cursor_x, ey + 3, cursor_x, ey + 17);

        }

        // Column headers.

        const char* headers[3] = { "Font", "Style", "Size" };

        for (int c = 0; c < 3; ++c)

        {

            xf::draw_text_left(display_, window_, gc_, st, col_x(c) + 2, list_top() - 20, list_top() - 2, headers[c]);

        }

        // Family list (filtered).

        std::vector<int> vis = visible_families();

        std::vector<std::string> fam_items;

        int fam_sel_row = -1;

        for (size_t i = 0; i < vis.size(); ++i)

        {

            fam_items.push_back(families_[vis[i]]);

            if (vis[i] == family_sel_)

            {

                fam_sel_row = (int)i;

            }

        }

        draw_list(st, 0, fam_items, fam_sel_row);

        draw_list(st, 1, styles_list_, style_sel_);

        std::vector<std::string> size_items;

        for (size_t i = 0; i < sizes_.size(); ++i)

        {

            size_items.push_back(std::to_string(sizes_[i]));

        }

        draw_list(st, 2, size_items, size_sel_);

        // Preview band: the chosen font name + the preview text.
        // (Core X fonts cannot be rescaled, so the preview shows
        // the name/size as text beside the sample line.)

        int py = list_bottom() + 10;

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_ + 12, py, width() - 24, 44);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 20, py + 2, py + 20, get_font_name());

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 20, py + 22, py + 42, preview_text_);

        // Action bar.

        int ay = y2_ - action_bar_height();

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x1_ + 2, ay, x2_ - 2, ay);

        const char* labels[2] = { "Cancel", "OK" };

        for (int i = 0; i < 2; ++i)

        {

            int bx1, by1, bx2, by2;

            button_rect(i, bx1, by1, bx2, by2);

            SStyle bst = st;

            if (pressed_button_ == i)

            {

                unsigned long swap = bst.fillColor_;

                bst.fillColor_ = bst.outlineColor_;

                bst.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, bst, bx1, by1, bx2, by2);

            xf::draw_text_centered(display_, window_, gc_, bst, bx1, by1, bx2, by2, labels[i]);

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

        filter_focused_ = false;

        if (in_rect(x, y, x2_ - 28, y1_, x2_ - 4, y1_ + title_bar_height()))

        {

            respond(RESPONSE_DELETE_EVENT);

            return true;

        }

        // Search entry.

        int ey = y1_ + title_bar_height() + 6;

        int ex2 = x1_ + 400 > x2_ - 12 ? x2_ - 12 : x1_ + 400;

        if (x >= x1_ + 62 && x <= ex2 && y >= ey && y <= ey + 20)

        {

            filter_focused_ = true;

            return true;

        }

        // The three lists.

        if (y >= list_top() && y <= list_bottom())

        {

            int row = (y - list_top() - 1) / row_height();

            if (x >= col_x(0) && x <= col_x(0) + col_w(0))

            {

                std::vector<int> vis = visible_families();

                if (row >= 0 && row < (int)vis.size())

                {

                    family_sel_ = vis[row];

                }

                return true;

            }

            if (x >= col_x(1) && x <= col_x(1) + col_w(1))

            {

                if (row >= 0 && row < (int)styles_list_.size())

                {

                    style_sel_ = row;

                }

                return true;

            }

            if (x >= col_x(2) && x <= col_x(2) + col_w(2))

            {

                if (row >= 0 && row < (int)sizes_.size())

                {

                    size_sel_ = row;

                }

                return true;

            }

        }

        for (int i = 0; i < 2; ++i)

        {

            int bx1, by1, bx2, by2;

            button_rect(i, bx1, by1, bx2, by2);

            if (in_rect(x, y, bx1, by1, bx2, by2))

            {

                pressed_button_ = i;

                return true;

            }

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (button != 1 || pressed_button_ < 0)

        {

            return false;

        }

        int which = pressed_button_;

        pressed_button_ = -1;

        int bx1, by1, bx2, by2;

        button_rect(which, bx1, by1, bx2, by2);

        if (in_rect(x, y, bx1, by1, bx2, by2))

        {

            respond(which == 1 ? RESPONSE_OK : RESPONSE_CANCEL);

        }

        return true;

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

        if (key == XK_Escape)

        {

            respond(RESPONSE_CANCEL);

            return true;

        }

        if (key == XK_Return)

        {

            respond(RESPONSE_OK);

            return true;

        }

        if (filter_focused_)

        {

            if (key == XK_BackSpace)

            {

                if (!filter_.empty())

                {

                    filter_.pop_back();

                }

                return true;

            }

            if (str && str[0] >= 32 && str[0] < 127)

            {

                filter_ += str[0];

                return true;

            }

        }

        return false;

    }

};

//endregion
