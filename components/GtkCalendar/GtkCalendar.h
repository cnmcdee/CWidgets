#pragma once

#include "core/xframe_core.h"

#include <cstdio>

//region GtkCalendar

// A full month calendar: month/year header with prev/next buttons,
// a day-of-week header row, and a six-row day grid computed with
// real month math. Days can be marked; today is outlined and the
// selected day is filled.

class GtkCalendar

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

    int year_ = 2026;

    int month_ = 11;           // 1..12

    int day_ = 25;             // selected day

    int today_year_ = 2026;

    int today_month_ = 11;

    int today_day_ = 25;

    unsigned int marked_ = 0;  // bit (day-1) set = day marked

    bool show_heading_ = true;

    bool show_day_names_ = true;

public:

    std::function<void()> on_day_selected;

    std::function<void()> on_month_changed;

    GtkCalendar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkCalendar";

        text_ = "GtkCalendar";

        mark_day(1);

        mark_day(14);

    }

    ~GtkCalendar() {}

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

    //region Month math

    static bool is_leap(int y)

    {

        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);

    }

    static int days_in_month(int y, int m)

    {

        static const int days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        if (m == 2 && is_leap(y))

        {

            return 29;

        }

        return days[m - 1];

    }

    // Sakamoto's algorithm: 0 = Sunday .. 6 = Saturday.

    static int weekday_of(int y, int m, int d)

    {

        static const int t[12] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

        if (m < 3)

        {

            y -= 1;

        }

        return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;

    }

    static const char* month_name(int m)

    {

        static const char* names[12] = {

            "January", "February", "March", "April", "May", "June",

            "July", "August", "September", "October", "November", "December"

        };

        return names[(m - 1 + 12) % 12];

    }

    //endregion

    //region GtkCalendar API

    void set_date(int y, int m, int d)

    {

        month_ = std::max(1, std::min(12, m));

        year_ = y;

        day_ = std::max(1, std::min(days_in_month(year_, month_), d));

    }

    void get_date(int& y, int& m, int& d)

    {

        y = year_;

        m = month_;

        d = day_;

    }

    int get_year()

    {

        return year_;

    }

    int get_month()

    {

        return month_;

    }

    int get_day()

    {

        return day_;

    }

    void set_today(int y, int m, int d)

    {

        today_year_ = y;

        today_month_ = m;

        today_day_ = d;

    }

    void select_day(int d)

    {

        day_ = std::max(1, std::min(days_in_month(year_, month_), d));

        if (on_day_selected)

        {

            on_day_selected();

        }

    }

    void prev_month()

    {

        if (--month_ < 1)

        {

            month_ = 12;

            --year_;

        }

        day_ = std::min(day_, days_in_month(year_, month_));

        if (on_month_changed)

        {

            on_month_changed();

        }

    }

    void next_month()

    {

        if (++month_ > 12)

        {

            month_ = 1;

            ++year_;

        }

        day_ = std::min(day_, days_in_month(year_, month_));

        if (on_month_changed)

        {

            on_month_changed();

        }

    }

    void mark_day(int d)

    {

        if (d >= 1 && d <= 31)

        {

            marked_ |= (1u << (d - 1));

        }

    }

    void unmark_day(int d)

    {

        if (d >= 1 && d <= 31)

        {

            marked_ &= ~(1u << (d - 1));

        }

    }

    void clear_marks()

    {

        marked_ = 0;

    }

    bool get_marked(int d)

    {

        return d >= 1 && d <= 31 && (marked_ & (1u << (d - 1))) != 0;

    }

    void set_show_heading(bool s)

    {

        show_heading_ = s;

    }

    void set_show_day_names(bool s)

    {

        show_day_names_ = s;

    }

    std::string date_string()

    {

        char buf[40];

        std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year_, month_, day_);

        return std::string(buf);

    }

    //endregion

    //region Geometry

    int header_h()

    {

        return show_heading_ ? 24 : 0;

    }

    int names_h()

    {

        return show_day_names_ ? 16 : 0;

    }

    int grid_top()

    {

        return y1_ + header_h() + names_h();

    }

    int cell_w()

    {

        return std::max(1, width() / 7);

    }

    int cell_h()

    {

        return std::max(1, (height() - header_h() - names_h()) / 6);

    }

    void prev_rect(int& rx, int& ry, int& rw, int& rh)

    {

        rx = x1_ + 3;

        ry = y1_ + 3;

        rw = 18;

        rh = std::max(10, header_h() - 6);

    }

    void next_rect(int& rx, int& ry, int& rw, int& rh)

    {

        rw = 18;

        rh = std::max(10, header_h() - 6);

        rx = x2_ - rw - 3;

        ry = y1_ + 3;

    }

    // Day number at a grid position, or 0 for an empty cell.

    int day_at_cell(int row, int col)

    {

        int first = weekday_of(year_, month_, 1);

        int d = row * 7 + col - first + 1;

        if (d < 1 || d > days_in_month(year_, month_))

        {

            return 0;

        }

        return d;

    }

    bool cell_at(int x, int y, int& row, int& col)

    {

        if (y < grid_top())

        {

            return false;

        }

        col = (x - x1_) / cell_w();

        row = (y - grid_top()) / cell_h();

        return col >= 0 && col < 7 && row >= 0 && row < 6;

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

        int cw = cell_w();

        int ch = cell_h();

        if (show_heading_)

        {

            char title[48];

            std::snprintf(title, sizeof(title), "%s %d", month_name(month_), year_);

            xf::draw_text_centered(display_, window_, gc_, st, x1_, y1_, x2_, y1_ + header_h(), title);

            int px, py, pw, ph;

            prev_rect(px, py, pw, ph);

            xf::draw_polished(display_, window_, gc_, st, px, py, px + pw, py + ph);

            xf::draw_text_centered(display_, window_, gc_, st, px, py, px + pw, py + ph, "<");

            next_rect(px, py, pw, ph);

            xf::draw_polished(display_, window_, gc_, st, px, py, px + pw, py + ph);

            xf::draw_text_centered(display_, window_, gc_, st, px, py, px + pw, py + ph, ">");

        }

        if (show_day_names_)

        {

            static const char* dow[7] = { "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa" };

            SStyle dim = st;

            dim.text_color_ = styles_->get_color_rgb(110, 110, 110);

            for (int c = 0; c < 7; ++c)

            {

                xf::draw_text_centered(display_, window_, gc_, dim, x1_ + c * cw, y1_ + header_h(), x1_ + (c + 1) * cw, y1_ + header_h() + names_h(), dow[c]);

            }

        }

        int top = grid_top();

        // Faint grid lines.

        XSetForeground(display_, gc_, styles_->get_color_rgb(205, 205, 205));

        for (int c = 1; c < 7; ++c)

        {

            XDrawLine(display_, window_, gc_, x1_ + c * cw, top, x1_ + c * cw, top + 6 * ch);

        }

        for (int r = 1; r < 6; ++r)

        {

            XDrawLine(display_, window_, gc_, x1_, top + r * ch, x2_ - 1, top + r * ch);

        }

        for (int r = 0; r < 6; ++r)

        {

            for (int c = 0; c < 7; ++c)

            {

                int d = day_at_cell(r, c);

                if (d == 0)

                {

                    continue;

                }

                int cx1 = x1_ + c * cw;

                int cy1 = top + r * ch;

                bool is_today = (year_ == today_year_ && month_ == today_month_ && d == today_day_);

                bool is_selected = (d == day_);

                if (is_selected)

                {

                    XSetForeground(display_, gc_, enabled_ ? styles_->get_color_rgb(52, 101, 164) : styles_->get_color_rgb(150, 150, 150));

                    XFillRectangle(display_, window_, gc_, cx1 + 1, cy1 + 1, cw - 2, ch - 2);

                }

                SStyle cell = st;

                if (is_selected)

                {

                    cell.text_color_ = styles_->get_color_rgb(255, 255, 255);

                }

                else if (!enabled_)

                {

                    cell.text_color_ = styles_->get_color_rgb(150, 150, 150);

                }

                else if (c == 0 || c == 6)

                {

                    cell.text_color_ = styles_->get_color_rgb(170, 60, 60);   // weekends

                }

                char num[8];

                std::snprintf(num, sizeof(num), "%d", d);

                xf::draw_text_centered(display_, window_, gc_, cell, cx1, cy1, cx1 + cw, cy1 + ch, num);

                if (is_today && !is_selected)

                {

                    XSetForeground(display_, gc_, styles_->get_color_rgb(200, 50, 50));

                    XDrawRectangle(display_, window_, gc_, cx1 + 1, cy1 + 1, cw - 3, ch - 3);

                }

                if (get_marked(d))

                {

                    // A dot under the number marks the day.

                    XSetForeground(display_, gc_, is_selected ? styles_->get_color_rgb(255, 255, 255) : styles_->get_color_rgb(200, 50, 50));

                    XFillArc(display_, window_, gc_, cx1 + cw / 2 - 2, cy1 + ch - 6, 4, 4, 0, 360 * 64);

                }

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

        if (show_heading_)

        {

            int px, py, pw, ph;

            prev_rect(px, py, pw, ph);

            if (x >= px && x <= px + pw && y >= py && y <= py + ph)

            {

                prev_month();

                return true;

            }

            next_rect(px, py, pw, ph);

            if (x >= px && x <= px + pw && y >= py && y <= py + ph)

            {

                next_month();

                return true;

            }

        }

        int row, col;

        if (cell_at(x, y, row, col))

        {

            int d = day_at_cell(row, col);

            if (d > 0)

            {

                select_day(d);

            }

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

        if (key == XK_Left)

        {

            select_day(day_ - 1 < 1 ? 1 : day_ - 1);

            return true;

        }

        if (key == XK_Right)

        {

            select_day(day_ + 1);

            return true;

        }

        if (key == XK_Up)

        {

            select_day(day_ - 7 < 1 ? 1 : day_ - 7);

            return true;

        }

        if (key == XK_Down)

        {

            select_day(day_ + 7);

            return true;

        }

        if (key == XK_Page_Up)

        {

            prev_month();

            return true;

        }

        if (key == XK_Page_Down)

        {

            next_month();

            return true;

        }

        return false;

    }

};

//endregion
