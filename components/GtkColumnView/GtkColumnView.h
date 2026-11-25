#pragma once

#include "core/xframe_core.h"

//region GtkColumnView

class GtkColumnView

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

    struct Column

    {

        std::string title;

        int width;

    };

    std::vector<Column> columns_;

    std::vector<std::vector<std::string>> rows_;

    int selected_row_ = 0;

    int scroll_row_ = 0;

    int sort_column_ = -1;

    bool sort_ascending_ = true;

    int header_height_ = 22;

    int row_height_ = 20;

    int visible_rows()

    {

        return std::max(1, (height() - header_height_ - 4) / row_height_);

    }

    void sort_rows()

    {

        if (sort_column_ < 0)

        {

            return;

        }

        int col = sort_column_;

        bool asc = sort_ascending_;

        std::sort(rows_.begin(), rows_.end(), [col, asc](const std::vector<std::string>& a, const std::vector<std::string>& b) {

            std::string av = col < (int)a.size() ? a[col] : "";

            std::string bv = col < (int)b.size() ? b[col] : "";

            return asc ? av < bv : av > bv;

        });

    }

public:

    std::function<void()> on_selection_changed;

    GtkColumnView(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkColumnView";

        text_ = "ColumnView";

        add_column("Name", 130);

        add_column("Artist", 130);

        add_column("Year", 60);

        add_row({"Blue in Green", "Miles Davis", "1959"});

        add_row({"So What", "Miles Davis", "1959"});

        add_row({"Take Five", "Dave Brubeck", "1959"});

        add_row({"Giant Steps", "John Coltrane", "1960"});

        add_row({"My Favorite Things", "John Coltrane", "1961"});

        add_row({"Time Out", "Dave Brubeck", "1959"});

        add_row({"Kind of Blue", "Miles Davis", "1959"});

    }

    ~GtkColumnView() {}

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

    //region Column view specific API

    void add_column(std::string title, int col_width)

    {

        Column col;

        col.title = title;

        col.width = col_width;

        columns_.push_back(col);

    }

    void add_row(std::vector<std::string> cells)

    {

        rows_.push_back(cells);

    }

    void clear_rows()

    {

        rows_.clear();

        selected_row_ = -1;

        scroll_row_ = 0;

    }

    int row_count()

    {

        return (int)rows_.size();

    }

    int get_selected_row()

    {

        return selected_row_;

    }

    std::string get_cell(int row, int col)

    {

        if (row >= 0 && row < (int)rows_.size() && col >= 0 && col < (int)rows_[row].size())

        {

            return rows_[row][col];

        }

        return "";

    }

    int get_sort_column()

    {

        return sort_column_;

    }

    bool get_sort_ascending()

    {

        return sort_ascending_;

    }

    void sort_by(int col)

    {

        if (col == sort_column_)

        {

            sort_ascending_ = !sort_ascending_;

        }

        else

        {

            sort_column_ = col;

            sort_ascending_ = true;

        }

        sort_rows();

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

        // Header band with sort arrow on the active column.

        unsigned long header_fill = styles_->get_color_rgb(225, 225, 225);

        XSetForeground(display_, gc_, header_fill);

        XFillRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 2, header_height_);

        int col_x = x1_ + 4;

        for (size_t c = 0; c < columns_.size(); ++c)

        {

            xf::draw_text_left(display_, window_, gc_, st, col_x, y1_ + 1, y1_ + 1 + header_height_, columns_[c].title);

            if ((int)c == sort_column_)

            {

                int tw = xf::text_width(st, columns_[c].title);

                int cx = col_x + tw + 8;

                int cy = y1_ + 1 + header_height_ / 2;

                if (sort_ascending_)

                {

                    xf::draw_triangle_up(display_, window_, gc_, st.text_color_, cx, cy + 3, 4);

                }

                else

                {

                    xf::draw_triangle_down(display_, window_, gc_, st.text_color_, cx, cy - 3, 4);

                }

            }

            col_x += columns_[c].width;

            if (c + 1 < columns_.size())

            {

                XSetForeground(display_, gc_, st.lineColor_);

                XDrawLine(display_, window_, gc_, col_x - 4, y1_ + 3, col_x - 4, y1_ + header_height_ - 3);

            }

        }

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x1_ + 1, y1_ + header_height_ + 1, x2_ - 1, y1_ + header_height_ + 1);

        // Rows.

        unsigned long sel_fill = styles_->get_color_rgb(70, 110, 180);

        unsigned long alt_fill = styles_->get_color_rgb(242, 242, 242);

        int rows_top = y1_ + header_height_ + 3;

        for (int i = scroll_row_; i < (int)rows_.size(); ++i)

        {

            int ry = rows_top + (i - scroll_row_) * row_height_;

            if (ry + row_height_ > y2_ - 2)

            {

                break;

            }

            SStyle row_style = st;

            if (i == selected_row_)

            {

                XSetForeground(display_, gc_, sel_fill);

                XFillRectangle(display_, window_, gc_, x1_ + 2, ry, width() - 4, row_height_);

                row_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            else if (i % 2 == 1)

            {

                XSetForeground(display_, gc_, alt_fill);

                XFillRectangle(display_, window_, gc_, x1_ + 2, ry, width() - 4, row_height_);

            }

            int cell_x = x1_ + 4;

            for (size_t c = 0; c < columns_.size(); ++c)

            {

                std::string value = c < rows_[i].size() ? rows_[i][c] : "";

                xf::draw_text_left(display_, window_, gc_, row_style, cell_x, ry, ry + row_height_, value);

                cell_x += columns_[c].width;

            }

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

        if (button == 4)

        {

            scroll_row_ = std::max(0, scroll_row_ - 3);

            return true;

        }

        if (button == 5)

        {

            scroll_row_ += 3;

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        // Header click: choose / toggle the sort column.

        if (y <= y1_ + header_height_ + 1)

        {

            int col_x = x1_ + 4;

            for (size_t c = 0; c < columns_.size(); ++c)

            {

                if (x >= col_x - 4 && x < col_x - 4 + columns_[c].width)

                {

                    sort_by((int)c);

                    return true;

                }

                col_x += columns_[c].width;

            }

            return true;

        }

        int rows_top = y1_ + header_height_ + 3;

        int index = scroll_row_ + (y - rows_top) / row_height_;

        if (index >= 0 && index < (int)rows_.size() && index != selected_row_)

        {

            selected_row_ = index;

            if (on_selection_changed)

            {

                on_selection_changed();

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

        if (key == XK_Down && selected_row_ < (int)rows_.size() - 1)

        {

            selected_row_++;

        }

        else if (key == XK_Up && selected_row_ > 0)

        {

            selected_row_--;

        }

        else

        {

            return false;

        }

        if (selected_row_ < scroll_row_)

        {

            scroll_row_ = selected_row_;

        }

        if (selected_row_ >= scroll_row_ + visible_rows())

        {

            scroll_row_ = selected_row_ - visible_rows() + 1;

        }

        if (on_selection_changed)

        {

            on_selection_changed();

        }

        return true;

    }

};

//endregion
