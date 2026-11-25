#pragma once

#include "core/xframe_core.h"

//region TStringGrid

class TStringGrid

{

public:

    struct GridRect

    {

        int x1;

        int y1;

        int x2;

        int y2;

    };

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

    int col_count_ = 5;

    int row_count_ = 6;

    int fixed_cols_ = 1;

    int fixed_rows_ = 1;

    std::vector<int> col_widths_;

    int default_row_height_ = 20;

    std::vector<std::vector<std::string>> cells_;   // [col][row], like LCL Cells[col][row]

    int sel_col_ = 1;

    int sel_row_ = 1;

    int top_left_col_ = 1;

    int top_left_row_ = 1;

    bool go_editing_ = true;

    bool go_row_select_ = false;

    bool editing_ = false;

    std::string edit_buffer_;

    int edit_cursor_ = 0;

    void resize_cells()

    {

        cells_.assign(col_count_, std::vector<std::string>(row_count_));

        col_widths_.assign(col_count_, 64);

    }

    void preload()

    {

        if (col_count_ > 0 && row_count_ > 0)

        {

            cells_[0][0] = "";

        }

        for (int c = 1; c < col_count_; ++c)

        {

            std::string label;

            label += (char)('A' + c - 1);

            cells_[c][0] = label;

        }

        for (int r = 1; r < row_count_; ++r)

        {

            cells_[0][r] = std::to_string(r);

        }

        const char *items[5] = { "Apples", "Pears", "Oranges", "Grapes", "Melons" };

        const char *qty[5] = { "42", "17", "28", "9", "13" };

        const char *price[5] = { "1.20", "1.50", "0.95", "2.40", "3.10" };

        const char *total[5] = { "50.40", "25.50", "26.60", "21.60", "40.30" };

        for (int r = 1; r < row_count_ && r <= 5; ++r)

        {

            if (col_count_ > 1) cells_[1][r] = items[r - 1];

            if (col_count_ > 2) cells_[2][r] = qty[r - 1];

            if (col_count_ > 3) cells_[3][r] = price[r - 1];

            if (col_count_ > 4) cells_[4][r] = total[r - 1];

        }

    }

    void ensure_visible()

    {

        if (sel_col_ < fixed_cols_)

        {

            sel_col_ = fixed_cols_;

        }

        if (sel_row_ < fixed_rows_)

        {

            sel_row_ = fixed_rows_;

        }

        if (sel_col_ < top_left_col_)

        {

            top_left_col_ = sel_col_;

        }

        if (sel_row_ < top_left_row_)

        {

            top_left_row_ = sel_row_;

        }

        int avail = width() - fixed_width();

        int used = 0;

        int c = sel_col_;

        while (c >= top_left_col_)

        {

            used += col_widths_[c];

            if (used > avail && c > top_left_col_)

            {

                ++top_left_col_;

                used = 0;

                c = sel_col_;

            }

            else

            {

                break;

            }

        }

        if (sel_row_ >= top_left_row_)

        {

            int rows_fit = (height() - fixed_rows_ * default_row_height_) / default_row_height_;

            if (rows_fit < 1) rows_fit = 1;

            if (sel_row_ >= top_left_row_ + rows_fit)

            {

                top_left_row_ = sel_row_ - rows_fit + 1;

            }

        }

    }

    void commit_edit()

    {

        if (!editing_)

        {

            return;

        }

        editing_ = false;

        if (sel_col_ >= 0 && sel_col_ < col_count_ && sel_row_ >= 0 && sel_row_ < row_count_)

        {

            cells_[sel_col_][sel_row_] = edit_buffer_;

            if (on_edit_done) on_edit_done(sel_col_, sel_row_);

        }

    }

    void start_edit(bool clear_with_char, char ch)

    {

        if (!go_editing_ || sel_col_ < fixed_cols_ || sel_row_ < fixed_rows_)

        {

            return;

        }

        editing_ = true;

        edit_buffer_ = clear_with_char ? std::string(1, ch) : cells_[sel_col_][sel_row_];

        edit_cursor_ = (int)edit_buffer_.size();

    }

    void move_sel(int dc, int dr)

    {

        int oc = sel_col_;

        int orow = sel_row_;

        sel_col_ = std::max(fixed_cols_, std::min(col_count_ - 1, sel_col_ + dc));

        sel_row_ = std::max(fixed_rows_, std::min(row_count_ - 1, sel_row_ + dr));

        ensure_visible();

        if ((sel_col_ != oc || sel_row_ != orow) && on_select_cell) on_select_cell(sel_col_, sel_row_);

    }

public:

    std::function<void(int, int)> on_select_cell;

    std::function<void(int, int)> on_edit_done;

    std::function<void()> on_click;

    TStringGrid(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TStringGrid";

        text_ = "TStringGrid";

        resize_cells();

        preload();

    }

    ~TStringGrid() {}

    void set_bounds(int x, int y, int width, int height)

    {

        x1_ = x;

        y1_ = y;

        x2_ = x + width;

        y2_ = y + height;

    }

    int width() { return x2_ - x1_; }

    int height() { return y2_ - y1_; }

    bool contains(int x, int y) { return visible_ && x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_; }

    void set_visible(bool v) { visible_ = v; }

    bool get_visible() { return visible_; }

    void set_enabled(bool e) { enabled_ = e; }

    bool get_enabled() { return enabled_; }

    void set_style_name(std::string s) { style_name_ = s; }

    void set_text(std::string t) { text_ = t; }

    std::string get_text() { return text_; }

    SStyle style() { return styles_->resolve(style_name_); }

    int fixed_width()

    {

        int w = 0;

        for (int c = 0; c < fixed_cols_ && c < col_count_; ++c) w += col_widths_[c];

        return w;

    }

    int col_left(int c)

    {

        if (c < fixed_cols_)

        {

            int x = x1_;

            for (int i = 0; i < c; ++i) x += col_widths_[i];

            return x;

        }

        int x = x1_ + fixed_width();

        for (int i = top_left_col_; i < c; ++i) x += col_widths_[i];

        return x;

    }

    int row_top(int r)

    {

        if (r < fixed_rows_) return y1_ + r * default_row_height_;

        return y1_ + fixed_rows_ * default_row_height_ + (r - top_left_row_) * default_row_height_;

    }

    GridRect cell_rect(int c, int r)

    {

        GridRect rc;

        rc.x1 = col_left(c);

        rc.y1 = row_top(r);

        rc.x2 = rc.x1 + col_widths_[c];

        rc.y2 = rc.y1 + default_row_height_;

        return rc;

    }

    void cell_at(int x, int y, int& out_col, int& out_row)

    {

        out_col = -1;

        out_row = -1;

        for (int c = 0; c < col_count_; ++c)

        {

            bool vis = (c < fixed_cols_) || (c >= top_left_col_);

            if (!vis) continue;

            int lx = col_left(c);

            if (x >= lx && x < lx + col_widths_[c] && lx < x2_) out_col = c;

        }

        for (int r = 0; r < row_count_; ++r)

        {

            bool vis = (r < fixed_rows_) || (r >= top_left_row_);

            if (!vis) continue;

            int ty = row_top(r);

            if (y >= ty && y < ty + default_row_height_ && ty < y2_) out_row = r;

        }

        if (out_col < 0 || out_row < 0) { out_col = -1; out_row = -1; }

    }

    void draw()

    {

        if (!visible_) return;

        SStyle st = style();

        unsigned long white = styles_->get_color_rgb(255, 255, 255);

        unsigned long grey = styles_->get_color_rgb(200, 200, 200);

        unsigned long highlight = styles_->get_color_rgb(51, 153, 255);

        XSetForeground(display_, gc_, white);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        for (int r = 0; r < row_count_; ++r)

        {

            for (int c = 0; c < col_count_; ++c)

            {

                if (c >= fixed_cols_ && c < top_left_col_) continue;

                if (r >= fixed_rows_ && r < top_left_row_) continue;

                GridRect rc = cell_rect(c, r);

                if (rc.x1 >= x2_ || rc.y1 >= y2_) continue;

                int cx2 = std::min(rc.x2, x2_);

                int cy2 = std::min(rc.y2, y2_);

                bool fixed = (c < fixed_cols_ || r < fixed_rows_);

                bool selected = (c == sel_col_ && r == sel_row_) || (go_row_select_ && r == sel_row_ && c >= fixed_cols_);

                if (fixed)

                {

                    XSetForeground(display_, gc_, st.fillColor_);

                    XFillRectangle(display_, window_, gc_, rc.x1, rc.y1, cx2 - rc.x1, cy2 - rc.y1);

                }

                else if (selected && !editing_)

                {

                    XSetForeground(display_, gc_, highlight);

                    XFillRectangle(display_, window_, gc_, rc.x1, rc.y1, cx2 - rc.x1, cy2 - rc.y1);

                }

                else

                {

                    XSetForeground(display_, gc_, white);

                    XFillRectangle(display_, window_, gc_, rc.x1, rc.y1, cx2 - rc.x1, cy2 - rc.y1);

                }

                XSetForeground(display_, gc_, grey);

                xf::set_line_width(display_, gc_, 1);

                XDrawRectangle(display_, window_, gc_, rc.x1, rc.y1, cx2 - rc.x1 - 1, cy2 - rc.y1 - 1);

                SStyle tst = st;

                if (selected && !fixed && !editing_) tst.text_color_ = white;

                std::string val = (editing_ && c == sel_col_ && r == sel_row_) ? edit_buffer_ : cells_[c][r];

                xf::draw_text_left(display_, window_, gc_, tst, rc.x1 + 4, rc.y1, cy2, val);

                if (editing_ && c == sel_col_ && r == sel_row_ && focused_)

                {

                    int tw = xf::text_width(tst, edit_buffer_.substr(0, edit_cursor_));

                    XSetForeground(display_, gc_, st.text_color_);

                    XDrawLine(display_, window_, gc_, rc.x1 + 4 + tw, rc.y1 + 3, rc.x1 + 4 + tw, cy2 - 3);

                }

            }

        }

        if (!editing_)

        {

            GridRect rc = cell_rect(sel_col_, sel_row_);

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, 2);

            XDrawRectangle(display_, window_, gc_, rc.x1 + 1, rc.y1 + 1, std::min(rc.x2, x2_) - rc.x1 - 2, std::min(rc.y2, y2_) - rc.y1 - 2);

            xf::set_line_width(display_, gc_, 1);

        }

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_) return false;

        focused_ = true;

        if (button == 4)

        {

            if (top_left_row_ > fixed_rows_) --top_left_row_;

            return true;

        }

        if (button == 5)

        {

            if (top_left_row_ < row_count_ - 1) ++top_left_row_;

            return true;

        }

        if (button != 1) return false;

        int c = -1;

        int r = -1;

        cell_at(x, y, c, r);

        if (c < 0) return true;

        commit_edit();

        if (c != sel_col_ || r != sel_row_)

        {

            sel_col_ = c;

            sel_row_ = r;

            ensure_visible();

            if (on_select_cell) on_select_cell(sel_col_, sel_row_);

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x; (void)y; (void)button;

        return false;

    }

    bool on_motion(int x, int y)

    {

        (void)x; (void)y;

        return false;

    }

    bool on_key(KeySym key, const char* str)

    {

        if (!focused_ || !enabled_) return false;

        if (editing_)

        {

            if (key == XK_Return || key == XK_KP_Enter)

            {

                commit_edit();

                move_sel(0, 1);

                return true;

            }

            if (key == XK_Escape)

            {

                editing_ = false;

                return true;

            }

            if (key == XK_Tab)

            {

                commit_edit();

                move_sel(1, 0);

                return true;

            }

            if (key == XK_BackSpace)

            {

                if (edit_cursor_ > 0) { edit_buffer_.erase(edit_cursor_ - 1, 1); --edit_cursor_; }

                return true;

            }

            if (key == XK_Delete)

            {

                if (edit_cursor_ < (int)edit_buffer_.size()) edit_buffer_.erase(edit_cursor_, 1);

                return true;

            }

            if (key == XK_Left) { if (edit_cursor_ > 0) --edit_cursor_; return true; }

            if (key == XK_Right) { if (edit_cursor_ < (int)edit_buffer_.size()) ++edit_cursor_; return true; }

            if (key == XK_Home) { edit_cursor_ = 0; return true; }

            if (key == XK_End) { edit_cursor_ = (int)edit_buffer_.size(); return true; }

            if (key == XK_Up) { commit_edit(); move_sel(0, -1); return true; }

            if (key == XK_Down) { commit_edit(); move_sel(0, 1); return true; }

            if (str && str[0] >= 32 && str[0] < 127)

            {

                edit_buffer_.insert(edit_cursor_, 1, str[0]);

                ++edit_cursor_;

                return true;

            }

            return false;

        }

        if (key == XK_Left) { move_sel(-1, 0); return true; }

        if (key == XK_Right) { move_sel(1, 0); return true; }

        if (key == XK_Up) { move_sel(0, -1); return true; }

        if (key == XK_Down) { move_sel(0, 1); return true; }

        if (key == XK_F2) { start_edit(false, 0); return editing_; }

        if (key == XK_Return && go_editing_) { start_edit(false, 0); return editing_; }

        if (str && str[0] >= 32 && str[0] < 127 && go_editing_)

        {

            start_edit(true, str[0]);

            return editing_;

        }

        return false;

    }

    void set_cell(int c, int r, std::string v)

    {

        if (c >= 0 && c < col_count_ && r >= 0 && r < row_count_) cells_[c][r] = v;

    }

    std::string get_cell(int c, int r)

    {

        if (c >= 0 && c < col_count_ && r >= 0 && r < row_count_) return cells_[c][r];

        return "";

    }

    void set_col_count(int n)

    {

        if (n < 1) n = 1;

        std::vector<std::vector<std::string>> old = cells_;

        std::vector<int> oldw = col_widths_;

        col_count_ = n;

        resize_cells();

        for (int c = 0; c < col_count_ && c < (int)old.size(); ++c)

        {

            col_widths_[c] = oldw[c];

            for (int r = 0; r < row_count_ && r < (int)old[c].size(); ++r) cells_[c][r] = old[c][r];

        }

    }

    void set_row_count(int n)

    {

        if (n < 1) n = 1;

        std::vector<std::vector<std::string>> old = cells_;

        row_count_ = n;

        cells_.assign(col_count_, std::vector<std::string>(row_count_));

        for (int c = 0; c < col_count_; ++c)

            for (int r = 0; r < row_count_ && r < (int)old[c].size(); ++r) cells_[c][r] = old[c][r];

    }

    int get_col_count() { return col_count_; }

    int get_row_count() { return row_count_; }

    void set_col_width(int c, int w)

    {

        if (c >= 0 && c < col_count_ && w >= 8) col_widths_[c] = w;

    }

    int get_col_width(int c)

    {

        if (c >= 0 && c < col_count_) return col_widths_[c];

        return 0;

    }

    int get_col() { return sel_col_; }

    int get_row() { return sel_row_; }

    void set_selection(int c, int r)

    {

        sel_col_ = std::max(0, std::min(col_count_ - 1, c));

        sel_row_ = std::max(0, std::min(row_count_ - 1, r));

        ensure_visible();

        if (on_select_cell) on_select_cell(sel_col_, sel_row_);

    }

    void set_go_editing(bool v) { go_editing_ = v; }

    bool get_go_editing() { return go_editing_; }

    void set_go_row_select(bool v) { go_row_select_ = v; }

    bool is_editing() { return editing_; }

};

//endregion
