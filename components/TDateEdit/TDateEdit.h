#pragma once

#include "core/xframe_core.h"

#include <cstdio>

#include <ctime>

//region TDateEdit

class TDateEdit

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

    int day_ = 26;

    int month_ = 11;

    int year_ = 2026;

    int segment_ = 0;   // 0 = day, 1 = month, 2 = year

    int digits_ = 0;    // digits typed into the current segment

    int button_w_ = 20;

    bool calendar_open_ = false;

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

    static long days_from_civil(int y, int m, int d)

    {

        y -= (m <= 2) ? 1 : 0;

        long era = (y >= 0 ? y : y - 399) / 400;

        long yoe = y - era * 400;

        long doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;

        long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;

        return era * 146097 + doe - 719468;

    }

    static int weekday_of(int y, int m, int d)

    {

        long days = days_from_civil(y, m, d);

        int w = (int)((days % 7 + 7) % 7);

        return (w + 4) % 7;

    }

    void clamp_date()

    {

        if (month_ < 1)

        {

            month_ = 1;

        }

        if (month_ > 12)

        {

            month_ = 12;

        }

        int dim = days_in_month(year_, month_);

        if (day_ > dim)

        {

            day_ = dim;

        }

        if (day_ < 1)

        {

            day_ = 1;

        }

    }

    void sync_text()

    {

        char buf[32];

        std::snprintf(buf, sizeof(buf), "%02d/%02d/%04d", day_, month_, year_);

        text_ = buf;

    }

    int segment_value(int seg)

    {

        return seg == 0 ? day_ : (seg == 1 ? month_ : year_);

    }

    void set_segment_value(int seg, int v)

    {

        if (seg == 0)

        {

            day_ = v;

        }

        else if (seg == 1)

        {

            month_ = v;

        }

        else

        {

            year_ = v;

        }

        clamp_date();

        sync_text();

    }

    void step_segment(int dir)

    {

        int v = segment_value(segment_) + dir;

        if (segment_ == 1)

        {

            if (v < 1)

            {

                v = 12;

            }

            if (v > 12)

            {

                v = 1;

            }

        }

        set_segment_value(segment_, v);

        digits_ = 0;

    }

    int grid_y2()

    {

        return y2_ + 2 + 18 + 6 * 16 + 4;

    }

    bool in_grid(int x, int y)

    {

        return calendar_open_ && x >= x1_ && x <= x1_ + 7 * 22 && y >= y2_ + 2 && y <= grid_y2();

    }

public:

    std::function<void()> on_click;

    std::function<void()> on_button_click;

    std::function<void()> on_change;

    TDateEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TDateEdit";

        std::time_t now = std::time(nullptr);

        std::tm* lt = std::localtime(&now);

        if (lt)

        {

            year_ = lt->tm_year + 1900;

            month_ = lt->tm_mon + 1;

            day_ = lt->tm_mday;

        }

        sync_text();

    }

    ~TDateEdit() {}

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

        return visible_ && ((x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_) || in_grid(x, y));

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

        int d = 0, m = 0, y = 0;

        if (std::sscanf(t.c_str(), "%d/%d/%d", &d, &m, &y) == 3)

        {

            day_ = d;

            month_ = m;

            year_ = y;

            clamp_date();

            sync_text();

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

        char buf[32];

        std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year_, month_, day_);

        return buf;

    }

    int get_day()

    {

        return day_;

    }

    int get_month()

    {

        return month_;

    }

    int get_year()

    {

        return year_;

    }

    void set_date(int y, int m, int d)

    {

        year_ = y;

        month_ = m;

        day_ = d;

        clamp_date();

        sync_text();

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

        int bx = x2_ - button_w_;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        // Segment highlight behind the active field while editing.

        if (focused_ && enabled_)

        {

            char segbuf[16];

            std::snprintf(segbuf, sizeof(segbuf), segment_ == 2 ? "%04d" : "%02d", segment_value(segment_));

            std::string prefix = segment_ == 0 ? "" : (segment_ == 1 ? "00/" : "00/00/");

            int sx = x1_ + 4 + xf::text_width(st, prefix);

            int sw = xf::text_width(st, segbuf);

            XSetForeground(display_, gc_, styles_->get_color_rgb(200, 220, 255));

            XFillRectangle(display_, window_, gc_, sx - 1, y1_ + 2, sw + 2, height() - 4);

        }

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 4, y1_, y2_, text_);

        // Dropdown calendar button zone.

        xf::draw_polished(display_, window_, gc_, st, bx, y1_, x2_, y2_);

        xf::draw_triangle_down(display_, window_, gc_, st.text_color_, bx + button_w_ / 2, y1_ + height() / 2 - 2, 4);

        if (calendar_open_)

        {

            int gx = x1_;

            int gy = y2_ + 2;

            int gw = 7 * 22;

            int gh = 18 + 6 * 16 + 4;

            XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

            XFillRectangle(display_, window_, gc_, gx, gy, gw, gh);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, gx, gy, gw - 1, gh - 1);

            static const char* month_names[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

            char title[32];

            std::snprintf(title, sizeof(title), "%s %d", month_names[month_ - 1], year_);

            xf::draw_text_centered(display_, window_, gc_, st, gx, gy, gx + gw, gy + 18, title);

            int first = weekday_of(year_, month_, 1);

            int ndays = days_in_month(year_, month_);

            for (int d = 1; d <= ndays; ++d)

            {

                int slot = first + d - 1;

                int cx = gx + (slot % 7) * 22;

                int cy = gy + 18 + (slot / 7) * 16;

                if (d == day_)

                {

                    XSetForeground(display_, gc_, st.outlineColor_);

                    XFillRectangle(display_, window_, gc_, cx + 1, cy + 1, 20, 14);

                }

                char num[16];

                std::snprintf(num, sizeof(num), "%d", d);

                SStyle cs = st;

                if (d == day_)

                {

                    cs.text_color_ = styles_->get_color_rgb(255, 255, 255);

                }

                xf::draw_text_centered(display_, window_, gc_, cs, cx, cy, cx + 22, cy + 16, num);

            }

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!visible_ || !enabled_ || button != 1)

        {

            return false;

        }

        if (in_grid(x, y))

        {

            int col = (x - x1_) / 22;

            int row = (y - (y2_ + 2) - 18) / 16;

            if (col >= 0 && col < 7 && row >= 0 && row < 6)

            {

                int d = row * 7 + col - weekday_of(year_, month_, 1) + 1;

                if (d >= 1 && d <= days_in_month(year_, month_))

                {

                    day_ = d;

                    clamp_date();

                    sync_text();

                    calendar_open_ = false;

                    if (on_change)

                    {

                        on_change();

                    }

                }

            }

            return true;

        }

        if (x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_)

        {

            focused_ = true;

            if (x >= x2_ - button_w_)

            {

                calendar_open_ = !calendar_open_;

                if (on_button_click)

                {

                    on_button_click();

                }

            }

            else

            {

                // Pick the segment by click position in "DD/MM/YYYY".

                SStyle st = style();

                int rel = x - (x1_ + 4);

                int w_day = xf::text_width(st, "00/");

                int w_mon = xf::text_width(st, "00/");

                segment_ = rel < w_day ? 0 : (rel < w_day + w_mon ? 1 : 2);

                digits_ = 0;

            }

            return true;

        }

        calendar_open_ = false;

        return false;

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

        if (key == XK_Left)

        {

            if (segment_ > 0)

            {

                --segment_;

            }

            digits_ = 0;

            return true;

        }

        if (key == XK_Right || key == XK_Tab)

        {

            if (segment_ < 2)

            {

                ++segment_;

            }

            digits_ = 0;

            return true;

        }

        if (key == XK_Up)

        {

            step_segment(1);

            return true;

        }

        if (key == XK_Down)

        {

            step_segment(-1);

            return true;

        }

        if (str && str[0] >= '0' && str[0] <= '9')

        {

            int digit = str[0] - '0';

            int max_digits = segment_ == 2 ? 4 : 2;

            int v = digits_ == 0 ? digit : segment_value(segment_) * 10 + digit;

            set_segment_value(segment_, v);

            ++digits_;

            if (digits_ >= max_digits)

            {

                digits_ = 0;

                if (segment_ < 2)

                {

                    ++segment_;

                }

            }

            if (on_change)

            {

                on_change();

            }

            return true;

        }

        if (str && (str[0] == '/' || str[0] == '.' || str[0] == '-'))

        {

            if (segment_ < 2)

            {

                ++segment_;

            }

            digits_ = 0;

            return true;

        }

        return false;

    }

};

//endregion
