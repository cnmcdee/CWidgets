#pragma once

#include "core/xframe_core.h"

//region TSelectDirectoryDialog

class TSelectDirectoryDialog

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

    // Lazarus modal results

    static const int mrNone = 0;

    static const int mrOk = 1;

    static const int mrCancel = 2;

    struct TDirEntry

    {

        std::string name;

        std::string path;

        int level;

        bool expanded;

        bool has_children;

        bool visible;

    };

    std::vector<TDirEntry> entries_;

    int selected_ = 0;

    int top_index_ = 0;

    int result_ = 0;

    bool in_rect(int px, int py, int rx1, int ry1, int rx2, int ry2)

    {

        return px >= rx1 && px <= rx2 && py >= ry1 && py <= ry2;

    }

    void update_visibility()

    {

        int hide_below = -1;

        for (size_t i = 0; i < entries_.size(); ++i)

        {

            TDirEntry& e = entries_[i];

            if (hide_below >= 0 && e.level > hide_below)

            {

                e.visible = false;

                continue;

            }

            hide_below = -1;

            e.visible = true;

            if (e.has_children && !e.expanded)

            {

                hide_below = e.level;

            }

        }

    }

    int visible_count()

    {

        int n = 0;

        for (size_t i = 0; i < entries_.size(); ++i)

        {

            if (entries_[i].visible)

            {

                n++;

            }

        }

        return n;

    }

    // Entry index of the row-th visible entry, or -1.

    int entry_at_row(int row)

    {

        int n = 0;

        for (size_t i = 0; i < entries_.size(); ++i)

        {

            if (!entries_[i].visible)

            {

                continue;

            }

            if (n == row)

            {

                return (int)i;

            }

            n++;

        }

        return -1;

    }

    int row_of_entry(int idx)

    {

        int n = 0;

        for (int i = 0; i < idx && i < (int)entries_.size(); ++i)

        {

            if (entries_[(size_t)i].visible)

            {

                n++;

            }

        }

        return n;

    }

    void draw_chrome()

    {

        SStyle st = style();

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        XSetForeground(display_, gc_, st.outlineColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, 22);

        SStyle tst = st;

        tst.text_color_ = styles_->get_color_rgb(255, 255, 255);

        xf::draw_text_left(display_, window_, gc_, tst, x1_ + 8, y1_ + 2, y1_ + 24, text_);

        XSetForeground(display_, gc_, styles_->get_color_rgb(200, 60, 50));

        XFillRectangle(display_, window_, gc_, x2_ - 24, y1_ + 4, 18, 18);

        xf::draw_text_centered(display_, window_, gc_, tst, x2_ - 24, y1_ + 4, x2_ - 6, y1_ + 22, "X");

    }

    void draw_mini_button(int bx1, int by1, int bx2, int by2, const std::string& cap, bool down)

    {

        SStyle st = style();

        if (down)

        {

            unsigned long swap = st.fillColor_;

            st.fillColor_ = st.outlineColor_;

            st.outlineColor_ = swap;

        }

        xf::draw_polished(display_, window_, gc_, st, bx1, by1, bx2, by2);

        xf::draw_text_centered(display_, window_, gc_, st, bx1, by1, bx2, by2, cap);

    }

public:

    std::function<void()> on_click;

    TSelectDirectoryDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TSelectDirectoryDialog";

        text_ = "Select Directory";

        entries_ = {

            {"/", "/", 0, true, true, true},

            {"home", "/home", 1, true, true, true},

            {"user", "/home/user", 2, true, true, true},

            {"Documents", "/home/user/Documents", 3, false, false, true},

            {"Downloads", "/home/user/Downloads", 3, false, false, true},

            {"Pictures", "/home/user/Pictures", 3, false, false, true},

            {"Projects", "/home/user/Projects", 3, false, true, true},

            {"xframe-widgets", "/home/user/Projects/xframe-widgets", 4, false, false, true},

            {"etc", "/etc", 1, false, true, true},

            {"usr", "/usr", 1, false, true, true},

            {"var", "/var", 1, false, false, true},

        };

        selected_ = 3;

        update_visibility();

    }

    ~TSelectDirectoryDialog() {}

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

    std::string get_directory()

    {

        if (selected_ >= 0 && selected_ < (int)entries_.size())

        {

            return entries_[(size_t)selected_].path;

        }

        return "/";

    }

    void set_directory(std::string path)

    {

        for (size_t i = 0; i < entries_.size(); ++i)

        {

            if (entries_[i].path == path)

            {

                selected_ = (int)i;

                return;

            }

        }

    }

    int get_result()

    {

        return result_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        draw_chrome();

        //region Directory tree

        int lx1 = x1_ + 12;

        int ly1 = y1_ + 34;

        int lx2 = x2_ - 12;

        int ly2 = y2_ - 48;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, lx1, ly1, lx2 - lx1, ly2 - ly1);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, lx1, ly1, lx2 - lx1, ly2 - ly1);

        int rows = (ly2 - ly1) / 18;

        int row = 0;

        for (size_t i = 0; i < entries_.size(); ++i)

        {

            const TDirEntry& e = entries_[i];

            if (!e.visible)

            {

                continue;

            }

            if (row < top_index_)

            {

                row++;

                continue;

            }

            if (row >= top_index_ + rows)

            {

                break;

            }

            int ry = ly1 + (row - top_index_) * 18;

            SStyle rst = st;

            if ((int)i == selected_)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, lx1 + 1, ry, lx2 - lx1 - 2, 18);

                rst.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            int indent = lx1 + 6 + e.level * 16;

            if (e.has_children)

            {

                if (e.expanded)

                {

                    xf::draw_triangle_down(display_, window_, gc_, rst.text_color_, indent + 5, ry + 6, 4);

                }

                else

                {

                    xf::draw_triangle_right(display_, window_, gc_, rst.text_color_, indent + 3, ry + 9, 4);

                }

            }

            XSetForeground(display_, gc_, styles_->get_color_rgb(240, 200, 90));

            XFillRectangle(display_, window_, gc_, indent + 14, ry + 6, 12, 8);

            XFillRectangle(display_, window_, gc_, indent + 14, ry + 4, 6, 3);

            xf::draw_text_left(display_, window_, gc_, rst, indent + 30, ry, ry + 18, e.name);

            row++;

        }

        //endregion

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y2_ - 42, y2_ - 20, get_directory());

        draw_mini_button(x2_ - 188, y2_ - 34, x2_ - 100, y2_ - 10, "Select", false);

        draw_mini_button(x2_ - 92, y2_ - 34, x2_ - 12, y2_ - 10, "Cancel", false);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_)

        {

            return false;

        }

        focused_ = true;

        if (in_rect(x, y, x2_ - 24, y1_ + 4, x2_ - 6, y1_ + 22))

        {

            result_ = mrCancel;

            return true;

        }

        int lx1 = x1_ + 12;

        int ly1 = y1_ + 34;

        int lx2 = x2_ - 12;

        int ly2 = y2_ - 48;

        if (in_rect(x, y, lx1, ly1, lx2, ly2))

        {

            if (button == 4)

            {

                if (top_index_ > 0)

                {

                    top_index_--;

                }

                return true;

            }

            if (button == 5)

            {

                int rows = (ly2 - ly1) / 18;

                if (top_index_ + rows < visible_count())

                {

                    top_index_++;

                }

                return true;

            }

            if (button == 1)

            {

                int row = top_index_ + (y - ly1) / 18;

                int idx = entry_at_row(row);

                if (idx >= 0)

                {

                    TDirEntry& e = entries_[(size_t)idx];

                    int indent = lx1 + 6 + e.level * 16;

                    if (e.has_children && x >= indent && x <= indent + 12)

                    {

                        e.expanded = !e.expanded;

                        update_visibility();

                    }

                    selected_ = idx;

                }

                return true;

            }

            return false;

        }

        if (button != 1)

        {

            return false;

        }

        if (in_rect(x, y, x2_ - 188, y2_ - 34, x2_ - 100, y2_ - 10))

        {

            result_ = mrOk;

            return true;

        }

        if (in_rect(x, y, x2_ - 92, y2_ - 34, x2_ - 12, y2_ - 10))

        {

            result_ = mrCancel;

            return true;

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

        if (key == XK_Up || key == XK_Down)

        {

            int row = row_of_entry(selected_);

            int nrow = (key == XK_Up) ? row - 1 : row + 1;

            int idx = entry_at_row(nrow);

            if (idx >= 0)

            {

                selected_ = idx;

            }

            return true;

        }

        if (key == XK_Right || key == XK_Left)

        {

            TDirEntry& e = entries_[(size_t)selected_];

            if (e.has_children)

            {

                e.expanded = (key == XK_Right);

                update_visibility();

            }

            return true;

        }

        if (key == XK_Return)

        {

            result_ = mrOk;

            return true;

        }

        if (key == XK_Escape)

        {

            result_ = mrCancel;

            return true;

        }

        return false;

    }

};

//endregion
