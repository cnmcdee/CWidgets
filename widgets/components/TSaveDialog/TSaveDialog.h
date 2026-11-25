#pragma once

#include "core/xframe_core.h"

//region TSaveDialog

class TSaveDialog

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

    static const int mrYes = 6;

    static const int mrNo = 7;

    std::vector<std::string> entries_;

    std::vector<bool> entry_is_dir_;

    int selected_ = 0;

    int top_index_ = 0;

    std::string directory_;

    std::string filename_;

    int filename_cursor_ = 0;

    bool edit_focus_ = true;

    std::string filter_;

    int result_ = 0;

    bool confirm_overwrite_ = false;   // overwrite prompt is showing

    bool overwrite_confirmed_ = false;

    bool in_rect(int px, int py, int rx1, int ry1, int rx2, int ry2)

    {

        return px >= rx1 && px <= rx2 && py >= ry1 && py <= ry2;

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

    void draw_edit(int ex1, int ey1, int ex2, int ey2, const std::string& s, int cursor, bool foc)

    {

        SStyle st = style();

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, ex1, ey1, ex2 - ex1, ey2 - ey1);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, ex1, ey1, ex2 - ex1, ey2 - ey1);

        xf::draw_text_left(display_, window_, gc_, st, ex1 + 5, ey1, ey2, s);

        if (foc)

        {

            int cx = ex1 + 5 + xf::text_width(st, s.substr(0, (size_t)cursor));

            XSetForeground(display_, gc_, st.text_color_);

            XDrawLine(display_, window_, gc_, cx, ey1 + 3, cx, ey2 - 3);

        }

    }

    bool edit_key(std::string& buf, int& cur, KeySym key, const char* str)

    {

        if (key == XK_BackSpace)

        {

            if (cur > 0)

            {

                buf.erase((size_t)cur - 1, 1);

                cur--;

            }

            return true;

        }

        if (key == XK_Delete)

        {

            if (cur < (int)buf.size())

            {

                buf.erase((size_t)cur, 1);

            }

            return true;

        }

        if (key == XK_Left)

        {

            if (cur > 0)

            {

                cur--;

            }

            return true;

        }

        if (key == XK_Right)

        {

            if (cur < (int)buf.size())

            {

                cur++;

            }

            return true;

        }

        if (key == XK_Home)

        {

            cur = 0;

            return true;

        }

        if (key == XK_End)

        {

            cur = (int)buf.size();

            return true;

        }

        if (str && str[0] >= 32 && str[0] < 127)

        {

            buf.insert((size_t)cur, 1, str[0]);

            cur++;

            return true;

        }

        return false;

    }

    int visible_rows()

    {

        return (y2_ - 104 - (y1_ + 56)) / 18;

    }

    std::string current_name()

    {

        if (!filename_.empty())

        {

            return filename_;

        }

        if (selected_ >= 0 && selected_ < (int)entries_.size() && !entry_is_dir_[(size_t)selected_])

        {

            return entries_[(size_t)selected_];

        }

        return "";

    }

    bool name_exists(const std::string& n)

    {

        for (size_t i = 0; i < entries_.size(); ++i)

        {

            if (!entry_is_dir_[i] && entries_[i] == n)

            {

                return true;

            }

        }

        return false;

    }

    void do_save()

    {

        std::string n = current_name();

        if (n.empty())

        {

            return;

        }

        if (name_exists(n))

        {

            // Ask before overwriting, like the real dialog.

            confirm_overwrite_ = true;

            return;

        }

        result_ = mrOk;

    }

public:

    std::function<void()> on_click;

    TSaveDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TSaveDialog";

        text_ = "Save As";

        directory_ = "/home/user/Documents";

        filter_ = "All files (*.*)";

        entries_ = {"Documents", "Downloads", "notes.txt", "budget.ods", "main.cpp", "report.pdf", "data.csv", "todo.txt"};

        entry_is_dir_ = {true, true, false, false, false, false, false, false};

        selected_ = 2;

        filename_ = "notes.txt";

        filename_cursor_ = (int)filename_.size();

    }

    ~TSaveDialog() {}

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

    void set_directory(std::string d)

    {

        directory_ = d;

    }

    std::string get_directory()

    {

        return directory_;

    }

    void set_filter(std::string f)

    {

        filter_ = f;

    }

    std::string get_filter()

    {

        return filter_;

    }

    void set_filename(std::string f)

    {

        filename_ = f;

        filename_cursor_ = (int)filename_.size();

    }

    std::string get_filename()

    {

        std::string n = current_name();

        if (n.empty())

        {

            return directory_;

        }

        if (n[0] == '/')

        {

            return n;

        }

        return directory_ + "/" + n;

    }

    int get_result()

    {

        return result_;

    }

    bool get_overwrite_confirmed()

    {

        return overwrite_confirmed_;

    }

    bool get_confirm_showing()

    {

        return confirm_overwrite_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        draw_chrome();

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y1_ + 28, y1_ + 52, "Save in:  " + directory_);

        //region File list

        int lx1 = x1_ + 12;

        int ly1 = y1_ + 56;

        int lx2 = x2_ - 12;

        int ly2 = y2_ - 104;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, lx1, ly1, lx2 - lx1, ly2 - ly1);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, lx1, ly1, lx2 - lx1, ly2 - ly1);

        int rows = visible_rows();

        for (int i = top_index_; i < (int)entries_.size() && i < top_index_ + rows; ++i)

        {

            int ry = ly1 + (i - top_index_) * 18;

            SStyle rst = st;

            if (i == selected_)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, lx1 + 1, ry, lx2 - lx1 - 2, 18);

                rst.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            if (entry_is_dir_[(size_t)i])

            {

                XSetForeground(display_, gc_, styles_->get_color_rgb(240, 200, 90));

                XFillRectangle(display_, window_, gc_, lx1 + 5, ry + 6, 12, 8);

                XFillRectangle(display_, window_, gc_, lx1 + 5, ry + 4, 6, 3);

            }

            else

            {

                XSetForeground(display_, gc_, styles_->get_color_rgb(250, 250, 250));

                XFillRectangle(display_, window_, gc_, lx1 + 6, ry + 3, 9, 12);

                XSetForeground(display_, gc_, st.lineColor_);

                XDrawRectangle(display_, window_, gc_, lx1 + 6, ry + 3, 9, 12);

            }

            xf::draw_text_left(display_, window_, gc_, rst, lx1 + 22, ry, ry + 18, entries_[(size_t)i]);

        }

        //endregion

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y2_ - 96, y2_ - 74, "File name:");

        draw_edit(x1_ + 96, y2_ - 96, x2_ - 12, y2_ - 74, filename_, filename_cursor_, edit_focus_);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y2_ - 68, y2_ - 46, "Save as type:");

        draw_edit(x1_ + 96, y2_ - 68, x2_ - 12, y2_ - 46, filter_, 0, false);

        draw_mini_button(x2_ - 188, y2_ - 34, x2_ - 100, y2_ - 10, "Save", false);

        draw_mini_button(x2_ - 92, y2_ - 34, x2_ - 12, y2_ - 10, "Cancel", false);

        if (confirm_overwrite_)

        {

            //region Overwrite prompt

            int cx1 = (x1_ + x2_) / 2 - 150;

            int cy1 = (y1_ + y2_) / 2 - 55;

            int cx2 = cx1 + 300;

            int cy2 = cy1 + 110;

            xf::draw_polished(display_, window_, gc_, st, cx1, cy1, cx2, cy2);

            xf::draw_text_centered(display_, window_, gc_, st, cx1 + 10, cy1 + 10, cx2 - 10, cy1 + 34, current_name());

            xf::draw_text_centered(display_, window_, gc_, st, cx1 + 10, cy1 + 32, cx2 - 10, cy1 + 56, "already exists. Overwrite it?");

            draw_mini_button(cx1 + 40, cy2 - 38, cx1 + 130, cy2 - 12, "Yes", false);

            draw_mini_button(cx2 - 130, cy2 - 38, cx2 - 40, cy2 - 12, "No", false);

            //endregion

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

        if (confirm_overwrite_)

        {

            if (button != 1)

            {

                return false;

            }

            int cx1 = (x1_ + x2_) / 2 - 150;

            int cy1 = (y1_ + y2_) / 2 - 55;

            int cx2 = cx1 + 300;

            int cy2 = cy1 + 110;

            if (in_rect(x, y, cx1 + 40, cy2 - 38, cx1 + 130, cy2 - 12))

            {

                confirm_overwrite_ = false;

                overwrite_confirmed_ = true;

                result_ = mrOk;

                return true;

            }

            if (in_rect(x, y, cx2 - 130, cy2 - 38, cx2 - 40, cy2 - 12))

            {

                confirm_overwrite_ = false;

                return true;

            }

            return true;   // prompt swallows other clicks

        }

        if (in_rect(x, y, x2_ - 24, y1_ + 4, x2_ - 6, y1_ + 22))

        {

            result_ = mrCancel;

            return true;

        }

        int lx1 = x1_ + 12;

        int ly1 = y1_ + 56;

        int lx2 = x2_ - 12;

        int ly2 = y2_ - 104;

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

                if (top_index_ + visible_rows() < (int)entries_.size())

                {

                    top_index_++;

                }

                return true;

            }

            if (button == 1)

            {

                int idx = top_index_ + (y - ly1) / 18;

                if (idx >= 0 && idx < (int)entries_.size())

                {

                    selected_ = idx;

                    if (!entry_is_dir_[(size_t)idx])

                    {

                        filename_ = entries_[(size_t)idx];

                        filename_cursor_ = (int)filename_.size();

                        edit_focus_ = true;

                    }

                }

                return true;

            }

            return false;

        }

        if (button != 1)

        {

            return false;

        }

        if (in_rect(x, y, x1_ + 96, y2_ - 96, x2_ - 12, y2_ - 74))

        {

            edit_focus_ = true;

            filename_cursor_ = (int)filename_.size();

            return true;

        }

        if (in_rect(x, y, x2_ - 188, y2_ - 34, x2_ - 100, y2_ - 10))

        {

            do_save();

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (confirm_overwrite_)

        {

            if (key == XK_Return || key == XK_y)

            {

                confirm_overwrite_ = false;

                overwrite_confirmed_ = true;

                result_ = mrOk;

                return true;

            }

            if (key == XK_Escape || key == XK_n)

            {

                confirm_overwrite_ = false;

                return true;

            }

            return false;

        }

        if (edit_focus_)

        {

            if (key == XK_Return)

            {

                do_save();

                return true;

            }

            if (key == XK_Escape)

            {

                result_ = mrCancel;

                return true;

            }

            return edit_key(filename_, filename_cursor_, key, str);

        }

        if (key == XK_Up)

        {

            if (selected_ > 0)

            {

                selected_--;

            }

            return true;

        }

        if (key == XK_Down)

        {

            if (selected_ < (int)entries_.size() - 1)

            {

                selected_++;

            }

            return true;

        }

        if (key == XK_Return)

        {

            do_save();

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
