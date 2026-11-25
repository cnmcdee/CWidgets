#pragma once

#include "core/xframe_core.h"

#include <cstdio>

#include <ctime>

//region TCalendar

class TCalendar

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

    int month_ = 11;   // 1..12, the displayed month

    int sel_year_ = 2026;

    int sel_month_ = 11;

    int sel_day_ = 26;

    int today_year_ = 2026;

    int today_month_ = 11;

    int today_day_ = 26;

    static long days_from_civil(int y, int m, int d)

    {

        y -= (m <= 2) ? 1 : 0;

        long era = (y >= 0 ? y : y - 399) / 400;

        long yoe = y - era * 400;

        long doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;

        long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;

        return era * 146097 + doe - 719468;

    }

    static bool is_leap(int y)

    {

        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);

    }

    static int days_in_month(int y, int m)

    {

        static const int dm[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        if (m == 2 && is_leap(y))

        {

            return 29;

        }

        return dm[m - 1];

    }

    // 0 = Sunday .. 6 = Saturday (1970-01-01 was a Thursday).

    static int weekday_of(int y, int m, int d)

    {

        long days = days_from_civil(y, m, d);

        int w = (int)((days % 7 + 7) % 7);

        return (w + 4) % 7;

    }

    int header_h()

    {

        return 24;

    }

    int week_h()

    {

        return 16;

    }

    int cell_w()

    {

        return width() / 7;

    }

    int cell_h()

    {

        return (height() - header_h() - week_h()) / 6;

    }

    void step_month(int dir)

    {

        month_ += dir;

        if (month_ < 1)

        {

            month_ = 12;

            --year_;

        }

        if (month_ > 12)

        {

            month_ = 1;

            ++year_;

        }

    }

    void select(int y, int m, int d)

    {

        sel_year_ = y;

        sel_month_ = m;

        sel_day_ = d;

        char buf[32];

        std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d", y, m, d);

        text_ = buf;

        if (on_change)

        {

            on_change();

        }

    }

public:

    std::function<void()> on_change;

    std::function<void()> on_click;

    TCalendar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TCalendar";

        // Preload with today's real date.

        std::time_t now = std::time(nullptr);

        std::tm* lt = std::localtime(&now);

        if (lt)

        {

            today_year_ = sel_year_ = year_ = lt->tm_year + 1900;

            today_month_ = sel_month_ = month_ = lt->tm_mon + 1;

            today_day_ = sel_day_ = lt->tm_mday;

        }

        char buf[32];

        std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d", sel_year_, sel_month_, sel_day_);

        text_ = buf;

    }

    ~TCalendar() {}

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

        int y = 0, m = 0, d = 0;

        if (std::sscanf(t.c_str(), "%d-%d-%d", &y, &m, &d) == 3 && m >= 1 && m <= 12 && d >= 1 && d <= days_in_month(y, m))

        {

            year_ = y;

            month_ = m;

            select(y, m, d);

        }

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    std::string get_date()

    {

        return text_;

    }

    int get_year()

    {

        return sel_year_;

    }

    int get_month()

    {

        return sel_month_;

    }

    int get_day()

    {

        return sel_day_;

    }

    void set_date(int y, int m, int d)

    {

        year_ = y;

        month_ = m;

        select(y, m, d);

    }

    void go_today()

    {

        year_ = today_year_;

        month_ = today_month_;

        select(today_year_, today_month_, today_day_);

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        if (!enabled_)

        {

            st.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        static const char* month_names[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

        static const char* week_names[7] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

        // Panel body.

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        // Header band with prev / next zones and the month title.

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 2, header_h() - 1);

        char title[64];

        std::snprintf(title, sizeof(title), "%s %d", month_names[month_ - 1], year_);

        xf::draw_text_centered(display_, window_, gc_, st, x1_, y1_, x2_, y1_ + header_h(), title);

        xf::draw_triangle_right(display_, window_, gc_, st.text_color_, x2_ - 14, y1_ + header_h() / 2, 4);

        // Left-pointing triangle drawn by hand (core has right/up/down).

        XSetForeground(display_, gc_, st.text_color_);

        for (int i = 0; i < 5; ++i)

        {

            XDrawLine(display_, window_, gc_, x1_ + 8 + i, y1_ + header_h() / 2 - (4 - i), x1_ + 8 + i, y1_ + header_h() / 2 + (4 - i));

        }

        // Weekday header row.

        int cw = cell_w();

        int ch = cell_h();

        int grid_y = y1_ + header_h() + week_h();

        for (int c = 0; c < 7; ++c)

        {

            xf::draw_text_centered(display_, window_, gc_, st, x1_ + c * cw, y1_ + header_h(), x1_ + (c + 1) * cw, grid_y, week_names[c]);

        }

        XSetForeground(display_, gc_, styles_->get_color_rgb(200, 200, 200));

        XDrawLine(display_, window_, gc_, x1_ + 2, grid_y, x2_ - 2, grid_y);

        // Day grid: real month math, leading blanks from the weekday
        // of the 1st.

        int first = weekday_of(year_, month_, 1);

        int ndays = days_in_month(year_, month_);

        for (int d = 1; d <= ndays; ++d)

        {

            int slot = first + d - 1;

            int col = slot % 7;

            int row = slot / 7;

            int cx1 = x1_ + col * cw;

            int cy1 = grid_y + row * ch;

            bool is_sel = (year_ == sel_year_ && month_ == sel_month_ && d == sel_day_);

            bool is_today = (year_ == today_year_ && month_ == today_month_ && d == today_day_);

            if (is_sel)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, cx1 + 1, cy1 + 1, cw - 2, ch - 2);

            }

            else if (is_today)

            {

                XSetForeground(display_, gc_, styles_->get_color_rgb(255, 220, 220));

                XFillRectangle(display_, window_, gc_, cx1 + 1, cy1 + 1, cw - 2, ch - 2);

            }

            char num[16];

            std::snprintf(num, sizeof(num), "%d", d);

            SStyle cell_style = st;

            if (is_sel)

            {

                cell_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            xf::draw_text_centered(display_, window_, gc_, cell_style, cx1, cy1, cx1 + cw, cy1 + ch, num);

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

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        if (y < y1_ + header_h())

        {

            if (x < x1_ + 24)

            {

                step_month(-1);

                return true;

            }

            if (x > x2_ - 24)

            {

                step_month(1);

                return true;

            }

            return true;

        }

        int grid_y = y1_ + header_h() + week_h();

        if (y >= grid_y)

        {

            int col = (x - x1_) / std::max(1, cell_w());

            int row = (y - grid_y) / std::max(1, cell_h());

            if (col >= 0 && col < 7 && row >= 0 && row < 6)

            {

                int d = row * 7 + col - weekday_of(year_, month_, 1) + 1;

                if (d >= 1 && d <= days_in_month(year_, month_))

                {

                    select(year_, month_, d);

                    if (on_click)

                    {

                        on_click();

                    }

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

        int y = sel_year_, m = sel_month_, d = sel_day_;

        if (key == XK_Left)

        {

            --d;

        }

        else if (key == XK_Right)

        {

            ++d;

        }

        else if (key == XK_Up)

        {

            d -= 7;

        }

        else if (key == XK_Down)

        {

            d += 7;

        }

        else if (key == XK_Page_Up)

        {

            step_month(-1);

            return true;

        }

        else if (key == XK_Page_Down)

        {

            step_month(1);

            return true;

        }

        else

        {

            return false;

        }

        while (d < 1)

        {

            --m;

            if (m < 1)

            {

                m = 12;

                --y;

            }

            d += days_in_month(y, m);

        }

        while (d > days_in_month(y, m))

        {

            d -= days_in_month(y, m);

            ++m;

            if (m > 12)

            {

                m = 1;

                ++y;

            }

        }

        year_ = y;

        month_ = m;

        select(y, m, d);

        return true;

    }

};

//endregion
