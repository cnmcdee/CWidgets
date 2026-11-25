#pragma once

#include "core/xframe_core.h"

//region GtkGrid

class GtkGrid

{

public:

    struct Rect
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
    std::string style_name_;   // "" = manager's active style
    bool visible_ = true;
    bool enabled_ = true;
    bool focused_ = false;

    int column_spacing_ = 6;
    int row_spacing_ = 6;
    int sample_columns_ = 3;
    int sample_rows_ = 3;
    int selected_col_ = 0;
    int selected_row_ = 0;

public:

    GtkGrid(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)
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
        name_ = "GtkGrid";
        text_ = "Grid";
    }

    ~GtkGrid() {}

    void set_bounds(int x, int y, int width, int height) { x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height; }

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

    void set_column_spacing(int spacing) { column_spacing_ = std::max(0, spacing); }

    int get_column_spacing() { return column_spacing_; }

    void set_row_spacing(int spacing) { row_spacing_ = std::max(0, spacing); }

    int get_row_spacing() { return row_spacing_; }

    int get_selected_col() { return selected_col_; }

    int get_selected_row() { return selected_row_; }

    // Even split of the padded area, spacing taken out first, with
    // the rounding remainder handed to the earliest cells.

    Rect cell_rect(int col, int row, int col_count, int row_count)
    {
        Rect r { x1_, y1_, x2_, y2_ };

        if (col_count <= 0 || row_count <= 0 || col < 0 || row < 0 || col >= col_count || row >= row_count)
        {
            return r;
        }

        int pad = 4;
        int total_w = std::max(0, width() - 2 * pad - column_spacing_ * (col_count - 1));
        int total_h = std::max(0, height() - 2 * pad - row_spacing_ * (row_count - 1));
        int cell_w = total_w / col_count;
        int cell_h = total_h / row_count;

        r.x1 = x1_ + pad + col * (cell_w + column_spacing_) + std::min(col, total_w % col_count);
        r.x2 = r.x1 + cell_w + ((col < total_w % col_count) ? 1 : 0);
        r.y1 = y1_ + pad + row * (cell_h + row_spacing_) + std::min(row, total_h % row_count);
        r.y2 = r.y1 + cell_h + ((row < total_h % row_count) ? 1 : 0);

        return r;
    }

    void draw()
    {
        if (!visible_)
        {
            return;
        }

        SStyle st = style();
        SStyle frame = st;
        frame.fillColor_ = styles_->get_color_rgb(238, 238, 240);
        xf::draw_polished(display_, window_, gc_, frame, x1_, y1_, x2_, y2_);

        for (int row = 0; row < sample_rows_; ++row)
        {
            for (int col = 0; col < sample_columns_; ++col)
            {
                Rect r = cell_rect(col, row, sample_columns_, sample_rows_);
                SStyle cell = st;

                if (col == selected_col_ && row == selected_row_ && focused_)
                {
                    cell.fillColor_ = styles_->get_color_rgb(197, 220, 248);
                }

                xf::fill_rounded_rect(display_, window_, gc_, cell, r.x1, r.y1, r.x2, r.y2);
                xf::draw_rounded_outline(display_, window_, gc_, cell, r.x1, r.y1, r.x2, r.y2);

                std::string label = std::to_string(col) + "," + std::to_string(row);
                xf::draw_text_centered(display_, window_, gc_, cell, r.x1, r.y1, r.x2, r.y2, label);
            }
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

        for (int row = 0; row < sample_rows_; ++row)
        {
            for (int col = 0; col < sample_columns_; ++col)
            {
                Rect r = cell_rect(col, row, sample_columns_, sample_rows_);

                if (x >= r.x1 && x <= r.x2 && y >= r.y1 && y <= r.y2)
                {
                    selected_col_ = col;
                    selected_row_ = row;
                    return true;
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

        if (key == XK_Left && selected_col_ > 0)
        {
            selected_col_ -= 1;
            return true;
        }

        if (key == XK_Right && selected_col_ < sample_columns_ - 1)
        {
            selected_col_ += 1;
            return true;
        }

        if (key == XK_Up && selected_row_ > 0)
        {
            selected_row_ -= 1;
            return true;
        }

        if (key == XK_Down && selected_row_ < sample_rows_ - 1)
        {
            selected_row_ += 1;
            return true;
        }

        return false;
    }

};

//endregion
