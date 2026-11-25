#pragma once

#include "core/xframe_core.h"

//region TReplaceDialog

class TReplaceDialog

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

    static const int mrCancel = 2;

    std::string find_;

    std::string replace_;

    int find_cursor_ = 0;

    int replace_cursor_ = 0;

    int edit_focus_ = 0;   // 0 = find edit, 1 = replace edit

    bool match_case_ = false;

    bool whole_words_ = false;

    bool search_up_ = false;

    int result_ = 0;

    int find_count_ = 0;

    int replace_count_ = 0;

    int replace_all_count_ = 0;

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

    void draw_checkbox(int bx, int by, bool on, const std::string& label)

    {

        SStyle st = style();

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, bx, by, 13, 13);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, bx, by, 13, 13);

        if (on)

        {

            xf::draw_checkmark(display_, window_, gc_, st.text_color_, bx + 6, by + 6, 9);

        }

        xf::draw_text_left(display_, window_, gc_, st, bx + 18, by - 2, by + 15, label);

    }

    void draw_radio(int bx, int by, bool on, const std::string& label)

    {

        SStyle st = style();

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillArc(display_, window_, gc_, bx, by, 12, 12, 0, 360 * 64);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawArc(display_, window_, gc_, bx, by, 12, 12, 0, 360 * 64);

        if (on)

        {

            XSetForeground(display_, gc_, st.text_color_);

            XFillArc(display_, window_, gc_, bx + 4, by + 4, 5, 5, 0, 360 * 64);

        }

        xf::draw_text_left(display_, window_, gc_, st, bx + 17, by - 2, by + 14, label);

    }

public:

    std::function<void()> on_find_next;

    std::function<void()> on_replace;

    std::function<void()> on_replace_all;

    std::function<void()> on_click;

    TReplaceDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TReplaceDialog";

        text_ = "Replace";

        find_ = "needle";

        find_cursor_ = (int)find_.size();

        replace_ = "haystack";

        replace_cursor_ = (int)replace_.size();

    }

    ~TReplaceDialog() {}

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

    std::string find_text()

    {

        return find_;

    }

    void set_find_text(std::string s)

    {

        find_ = s;

        find_cursor_ = (int)find_.size();

    }

    std::string replace_text()

    {

        return replace_;

    }

    void set_replace_text(std::string s)

    {

        replace_ = s;

        replace_cursor_ = (int)replace_.size();

    }

    bool match_case()

    {

        return match_case_;

    }

    bool whole_words()

    {

        return whole_words_;

    }

    bool search_up()

    {

        return search_up_;

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

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y1_ + 34, y1_ + 58, "Find what:");

        draw_edit(x1_ + 96, y1_ + 34, x2_ - 118, y1_ + 58, find_, find_cursor_, edit_focus_ == 0);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y1_ + 64, y1_ + 88, "Replace with:");

        draw_edit(x1_ + 96, y1_ + 64, x2_ - 118, y1_ + 88, replace_, replace_cursor_, edit_focus_ == 1);

        draw_checkbox(x1_ + 12, y1_ + 102, match_case_, "Match case");

        draw_checkbox(x1_ + 12, y1_ + 126, whole_words_, "Whole words only");

        draw_radio(x1_ + 190, y1_ + 102, search_up_, "Up");

        draw_radio(x1_ + 190, y1_ + 124, !search_up_, "Down");

        //region Button column

        draw_mini_button(x2_ - 106, y1_ + 34, x2_ - 12, y1_ + 58, "Find Next", false);

        draw_mini_button(x2_ - 106, y1_ + 64, x2_ - 12, y1_ + 88, "Replace", false);

        draw_mini_button(x2_ - 106, y1_ + 94, x2_ - 12, y1_ + 118, "Replace All", false);

        draw_mini_button(x2_ - 106, y1_ + 124, x2_ - 12, y1_ + 148, "Close", false);

        //endregion

        std::string status = "Finds: " + std::to_string(find_count_) + "   Replaced: " + std::to_string(replace_count_) + "   Replace-all runs: " + std::to_string(replace_all_count_);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y2_ - 26, y2_ - 8, status);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        if (in_rect(x, y, x2_ - 24, y1_ + 4, x2_ - 6, y1_ + 22))

        {

            result_ = mrCancel;

            return true;

        }

        if (in_rect(x, y, x1_ + 96, y1_ + 34, x2_ - 118, y1_ + 58))

        {

            edit_focus_ = 0;

            find_cursor_ = (int)find_.size();

            return true;

        }

        if (in_rect(x, y, x1_ + 96, y1_ + 64, x2_ - 118, y1_ + 88))

        {

            edit_focus_ = 1;

            replace_cursor_ = (int)replace_.size();

            return true;

        }

        if (in_rect(x, y, x1_ + 12, y1_ + 102, x1_ + 140, y1_ + 115))

        {

            match_case_ = !match_case_;

            return true;

        }

        if (in_rect(x, y, x1_ + 12, y1_ + 126, x1_ + 160, y1_ + 139))

        {

            whole_words_ = !whole_words_;

            return true;

        }

        if (in_rect(x, y, x1_ + 190, y1_ + 100, x1_ + 260, y1_ + 116))

        {

            search_up_ = true;

            return true;

        }

        if (in_rect(x, y, x1_ + 190, y1_ + 122, x1_ + 260, y1_ + 138))

        {

            search_up_ = false;

            return true;

        }

        if (in_rect(x, y, x2_ - 106, y1_ + 34, x2_ - 12, y1_ + 58))

        {

            find_count_++;

            if (on_find_next)

            {

                on_find_next();

            }

            return true;

        }

        if (in_rect(x, y, x2_ - 106, y1_ + 64, x2_ - 12, y1_ + 88))

        {

            replace_count_++;

            if (on_replace)

            {

                on_replace();

            }

            return true;

        }

        if (in_rect(x, y, x2_ - 106, y1_ + 94, x2_ - 12, y1_ + 118))

        {

            replace_all_count_++;

            if (on_replace_all)

            {

                on_replace_all();

            }

            return true;

        }

        if (in_rect(x, y, x2_ - 106, y1_ + 124, x2_ - 12, y1_ + 148))

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

        if (key == XK_Escape)

        {

            result_ = mrCancel;

            return true;

        }

        if (key == XK_Tab)

        {

            edit_focus_ = 1 - edit_focus_;

            return true;

        }

        if (key == XK_Return)

        {

            find_count_++;

            if (on_find_next)

            {

                on_find_next();

            }

            return true;

        }

        if (edit_focus_ == 0)

        {

            return edit_key(find_, find_cursor_, key, str);

        }

        return edit_key(replace_, replace_cursor_, key, str);

    }

};

//endregion
