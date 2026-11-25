#pragma once

#include "core/xframe_core.h"

//region TListView

class TListView

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

    // View styles, matching Lazarus TViewStyle values.

    static const int vsIcon = 0;

    static const int vsSmallIcon = 1;

    static const int vsList = 2;

    static const int vsReport = 3;

    int view_style_ = vsReport;

    struct TListColumn

    {

        std::string caption;

        int width;

    };

    std::vector<TListColumn> columns_;

    std::vector<std::vector<std::string>> items_;   // item 0 cell = caption

    int selected_ = 0;

    int top_index_ = 0;        // first visible item (report / list modes)

    int header_h_ = 20;

    int row_h_ = 18;

    std::string fit_text(const std::string& s, int maxw, SStyle& st)

    {

        if (maxw < 8)

        {

            return "";

        }

        if (xf::text_width(st, s) <= maxw)

        {

            return s;

        }

        std::string t = s;

        while (!t.empty() && xf::text_width(st, t + "..") > maxw)

        {

            t.pop_back();

        }

        return t + "..";

    }

    int column_x(int col)

    {

        int cx = x1_ + 2;

        for (int i = 0; i < col && i < (int)columns_.size(); ++i)

        {

            cx += columns_[i].width;

        }

        return cx;

    }

    int icon_columns()

    {

        int cols = width() / 78;

        return cols < 1 ? 1 : cols;

    }

    void keep_selection_visible()

    {

        if (view_style_ == vsReport || view_style_ == vsList || view_style_ == vsSmallIcon)

        {

            int rh = (view_style_ == vsReport) ? row_h_ : (view_style_ == vsList ? 30 : 20);

            int body_h = height() - (view_style_ == vsReport ? header_h_ : 0) - 4;

            int count = body_h / rh;

            if (selected_ < top_index_)

            {

                top_index_ = selected_;

            }

            if (selected_ >= top_index_ + count)

            {

                top_index_ = selected_ - count + 1;

            }

        }

    }

public:

    std::function<void()> on_select;

    TListView(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TListView";

        text_ = "TListView";

        add_column("Name", 130);

        add_column("Size", 70);

        add_column("Type", 110);

        add_item({"alpha.txt", "12 KB", "Text file"});

        add_item({"beta.png", "340 KB", "PNG image"});

        add_item({"gamma.cpp", "8 KB", "C++ source"});

        add_item({"delta.zip", "1.2 MB", "Archive"});

        add_item({"epsilon", "-", "Folder"});

        add_item({"zeta.dat", "96 KB", "Data file"});

    }

    ~TListView() {}

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

    void set_view_style(int vs)

    {

        view_style_ = vs;

        top_index_ = 0;

    }

    int get_view_style()

    {

        return view_style_;

    }

    void add_column(std::string caption, int w)

    {

        TListColumn col;

        col.caption = caption;

        col.width = w;

        columns_.push_back(col);

    }

    int column_count()

    {

        return (int)columns_.size();

    }

    void add_item(std::vector<std::string> cells)

    {

        items_.push_back(cells);

    }

    void clear_items()

    {

        items_.clear();

        selected_ = 0;

        top_index_ = 0;

    }

    int item_count()

    {

        return (int)items_.size();

    }

    int get_selected()

    {

        return selected_;

    }

    void set_selected(int i)

    {

        if (i >= 0 && i < (int)items_.size())

        {

            selected_ = i;

            keep_selection_visible();

        }

    }

    std::string get_selected_caption()

    {

        if (selected_ >= 0 && selected_ < (int)items_.size() && !items_[selected_].empty())

        {

            return items_[selected_][0];

        }

        return "";

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        unsigned long white = styles_->get_color_rgb(255, 255, 255);

        unsigned long highlight = styles_->get_color_rgb(180, 205, 240);

        unsigned long grid = styles_->get_color_rgb(215, 215, 215);

        XSetForeground(display_, gc_, white);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, (int)st.lineWidth_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        SStyle ts = st;

        if (!enabled_)

        {

            ts.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        if (view_style_ == vsReport)

        {

            draw_report(st, ts, highlight, grid);

        }

        else if (view_style_ == vsIcon)

        {

            draw_icons(st, ts, highlight, true);

        }

        else

        {

            draw_rows(st, ts, highlight, view_style_ == vsList);

        }

        XFlush(display_);

    }

    void draw_report(SStyle& st, SStyle& ts, unsigned long highlight, unsigned long grid)

    {

        // Column header band.

        for (size_t c = 0; c < columns_.size(); ++c)

        {

            int cx = column_x((int)c);

            xf::draw_polished(display_, window_, gc_, st, cx, y1_ + 1, cx + columns_[c].width, y1_ + header_h_);

            xf::draw_text_left(display_, window_, gc_, ts, cx + 5, y1_ + 1, y1_ + header_h_, columns_[c].caption);

        }

        int count = (height() - header_h_ - 4) / row_h_;

        for (int i = 0; i < count; ++i)

        {

            int idx = top_index_ + i;

            if (idx < 0 || idx >= (int)items_.size())

            {

                break;

            }

            int top = y1_ + header_h_ + 2 + i * row_h_;

            if (idx == selected_)

            {

                XSetForeground(display_, gc_, highlight);

                XFillRectangle(display_, window_, gc_, x1_ + 2, top, width() - 4, row_h_);

            }

            XSetForeground(display_, gc_, grid);

            XDrawLine(display_, window_, gc_, x1_ + 2, top + row_h_, x2_ - 2, top + row_h_);

            for (size_t c = 0; c < columns_.size() && c < items_[idx].size(); ++c)

            {

                int cx = column_x((int)c);

                std::string cell = fit_text(items_[idx][c], columns_[c].width - 8, ts);

                xf::draw_text_left(display_, window_, gc_, ts, cx + 5, top, top + row_h_, cell);

            }

        }

    }

    void draw_icons(SStyle& st, SStyle& ts, unsigned long highlight, bool large)

    {

        int cols = icon_columns();

        int tile_w = 78;

        int tile_h = 66;

        for (size_t i = 0; i < items_.size(); ++i)

        {

            int col = (int)i % cols;

            int row = (int)i / cols;

            int tx = x1_ + 4 + col * tile_w;

            int ty = y1_ + 4 + row * tile_h;

            if (ty + tile_h > y2_)

            {

                break;

            }

            if ((int)i == selected_)

            {

                XSetForeground(display_, gc_, highlight);

                XFillRectangle(display_, window_, gc_, tx, ty, tile_w - 4, tile_h - 4);

            }

            // The icon tile itself.

            int iw = large ? 34 : 20;

            int ix = tx + (tile_w - 4 - iw) / 2;

            xf::draw_polished(display_, window_, gc_, st, ix, ty + 3, ix + iw, ty + 3 + iw);

            std::string cap = items_[i].empty() ? "" : fit_text(items_[i][0], tile_w - 10, ts);

            xf::draw_text_centered(display_, window_, gc_, ts, tx, ty + 6 + iw, tx + tile_w - 4, ty + tile_h - 4, cap);

        }

    }

    void draw_rows(SStyle& st, SStyle& ts, unsigned long highlight, bool big_icons)

    {

        int rh = big_icons ? 30 : 20;

        int icon = big_icons ? 22 : 14;

        int count = (height() - 4) / rh;

        for (int i = 0; i < count; ++i)

        {

            int idx = top_index_ + i;

            if (idx < 0 || idx >= (int)items_.size())

            {

                break;

            }

            int top = y1_ + 2 + i * rh;

            if (idx == selected_)

            {

                XSetForeground(display_, gc_, highlight);

                XFillRectangle(display_, window_, gc_, x1_ + 2, top, width() - 4, rh);

            }

            int iy = top + (rh - icon) / 2;

            xf::draw_polished(display_, window_, gc_, st, x1_ + 6, iy, x1_ + 6 + icon, iy + icon);

            std::string cap = items_[idx].empty() ? "" : items_[idx][0];

            xf::draw_text_left(display_, window_, gc_, ts, x1_ + 12 + icon, top, top + rh, cap);

        }

    }

    int item_at(int x, int y)

    {

        if (view_style_ == vsReport)

        {

            if (y < y1_ + header_h_)

            {

                return -1;

            }

            int idx = top_index_ + (y - y1_ - header_h_ - 2) / row_h_;

            return (idx >= 0 && idx < (int)items_.size()) ? idx : -1;

        }

        if (view_style_ == vsIcon)

        {

            int cols = icon_columns();

            int col = (x - x1_ - 4) / 78;

            int row = (y - y1_ - 4) / 66;

            if (col < 0 || row < 0 || col >= cols)

            {

                return -1;

            }

            int idx = row * cols + col;

            return (idx >= 0 && idx < (int)items_.size()) ? idx : -1;

        }

        int rh = (view_style_ == vsList) ? 30 : 20;

        int idx = top_index_ + (y - y1_ - 2) / rh;

        return (idx >= 0 && idx < (int)items_.size()) ? idx : -1;

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        int idx = item_at(x, y);

        if (idx >= 0)

        {

            selected_ = idx;

            if (on_select)

            {

                on_select();

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

        if (!focused_ || !enabled_ || items_.empty())

        {

            return false;

        }

        int step = 1;

        if (view_style_ == vsIcon && (key == XK_Up || key == XK_Down))

        {

            step = icon_columns();

        }

        if (key == XK_Up || key == XK_Left)

        {

            selected_ = std::max(0, selected_ - step);

            keep_selection_visible();

            if (on_select)

            {

                on_select();

            }

            return true;

        }

        if (key == XK_Down || key == XK_Right)

        {

            selected_ = std::min((int)items_.size() - 1, selected_ + step);

            keep_selection_visible();

            if (on_select)

            {

                on_select();

            }

            return true;

        }

        return false;

    }

};

//endregion
