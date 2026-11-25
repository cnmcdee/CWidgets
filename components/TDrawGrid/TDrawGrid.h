#pragma once

#include "core/xframe_core.h"

//region TDrawGrid

class TDrawGrid

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

    int default_row_height_ = 24;

    int sel_col_ = 1;

    int sel_row_ = 1;

    int top_left_col_ = 1;

    int top_left_row_ = 1;

    int fixed_width()

    {

        int w = 0;

        for (int c = 0; c < fixed_cols_ && c < col_count_; ++c) w += col_widths_[c];

        return w;

    }

    void ensure_visible()

    {

        sel_col_ = std::max(fixed_cols_, std::min(col_count_ - 1, sel_col_));

        sel_row_ = std::max(fixed_rows_, std::min(row_count_ - 1, sel_row_));

        if (sel_col_ < top_left_col_) top_left_col_ = sel_col_;

        if (sel_row_ < top_left_row_) top_left_row_ = sel_row_;

        int rows_fit = (height() - fixed_rows_ * default_row_height_) / default_row_height_;

        if (rows_fit < 1) rows_fit = 1;

        if (sel_row_ >= top_left_row_ + rows_fit) top_left_row_ = sel_row_ - rows_fit + 1;

        while (sel_col_ >= top_left_col_)

        {

            int used = 0;

            for (int c = top_left_col_; c <= sel_col_; ++c) used += col_widths_[c];

            if (used <= width() - fixed_width() || top_left_col_ >= sel_col_) break;

            ++top_left_col_;

        }

    }

    void move_sel(int dc, int dr)

    {

        int oc = sel_col_;

        int orow = sel_row_;

        sel_col_ += dc;

        sel_row_ += dr;

        ensure_visible();

        if ((sel_col_ != oc || sel_row_ != orow) && on_select_cell) on_select_cell(sel_col_, sel_row_);

    }

public:

    std::function<void(TDrawGrid*, int, int, int, int, int, int)> on_draw_cell;

    std::function<void(int, int)> on_select_cell;

    std::function<void()> on_click;

    TDrawGrid(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TDrawGrid";

        text_ = "TDrawGrid";

        col_widths_.assign(col_count_, 56);

    }

    ~TDrawGrid() {}

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

            if (c >= fixed_cols_ && c < top_left_col_) continue;

            int lx = col_left(c);

            if (x >= lx && x < lx + col_widths_[c] && lx < x2_) out_col = c;

        }

        for (int r = 0; r < row_count_; ++r)

        {

            if (r >= fixed_rows_ && r < top_left_row_) continue;

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

                if (on_draw_cell && !fixed)

                {

                    on_draw_cell(this, c, r, rc.x1, rc.y1, cx2, cy2);

                }

                else if (fixed)

                {

                    XSetForeground(display_, gc_, st.fillColor_);

                    XFillRectangle(display_, window_, gc_, rc.x1, rc.y1, cx2 - rc.x1, cy2 - rc.y1);

                }

                else

                {

                    // Default drawing: checkerboard tint plus "c,r" label.

                    bool alt = ((c + r) % 2 == 0);

                    XSetForeground(display_, gc_, alt ? white : styles_->get_color_rgb(240, 240, 240));

                    XFillRectangle(display_, window_, gc_, rc.x1, rc.y1, cx2 - rc.x1, cy2 - rc.y1);

                    std::string label = std::to_string(c) + "," + std::to_string(r);

                    xf::draw_text_centered(display_, window_, gc_, st, rc.x1, rc.y1, cx2, cy2, label);

                }

                XSetForeground(display_, gc_, grey);

                xf::set_line_width(display_, gc_, 1);

                XDrawRectangle(display_, window_, gc_, rc.x1, rc.y1, cx2 - rc.x1 - 1, cy2 - rc.y1 - 1);

            }

        }

        GridRect rc = cell_rect(sel_col_, sel_row_);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 2);

        XDrawRectangle(display_, window_, gc_, rc.x1 + 1, rc.y1 + 1, std::min(rc.x2, x2_) - rc.x1 - 2, std::min(rc.y2, y2_) - rc.y1 - 2);

        xf::set_line_width(display_, gc_, 1);

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

        if (c >= 0 && (c != sel_col_ || r != sel_row_))

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

        (void)str;

        if (!focused_ || !enabled_) return false;

        if (key == XK_Left) { move_sel(-1, 0); return true; }

        if (key == XK_Right) { move_sel(1, 0); return true; }

        if (key == XK_Up) { move_sel(0, -1); return true; }

        if (key == XK_Down) { move_sel(0, 1); return true; }

        return false;

    }

    void set_col_count(int n)

    {

        if (n < 1) n = 1;

        col_count_ = n;

        col_widths_.assign(col_count_, 56);

    }

    void set_row_count(int n)

    {

        if (n >= 1) row_count_ = n;

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

    void set_default_row_height(int h)

    {

        if (h >= 8) default_row_height_ = h;

    }

    int get_col() { return sel_col_; }

    int get_row() { return sel_row_; }

    void set_selection(int c, int r)

    {

        sel_col_ = c;

        sel_row_ = r;

        ensure_visible();

        if (on_select_cell) on_select_cell(sel_col_, sel_row_);

    }

};

//endregion
