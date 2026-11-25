#pragma once

#include "core/xframe_core.h"

//region TDateTimePicker

class TDateTimePicker

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

    int month_ = 11;

    int day_ = 25;

    int hour_ = 13;

    int minute_ = 30;

    int second_ = 0;

    bool time_mode_ = false;   // false = date picker, true = time picker

    bool calendar_open_ = false;

    int field_ = 0;            // date: 0 day, 1 month, 2 year / time: 0 h, 1 m, 2 s

    //region Calendar month math

    static bool is_leap(int y)

    {

        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);

    }

    static int days_in_month(int y, int m)

    {

        static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        if (m == 2 && is_leap(y))

        {

            return 29;

        }

        return days[m - 1];

    }

    // Days since 1970-01-01 for a civil date (Hinnant's algorithm).

    static long days_from_civil(int y, int m, int d)

    {

        y -= (m <= 2) ? 1 : 0;

        long era = (y >= 0 ? y : y - 399) / 400;

        long yoe = y - era * 400;

        long doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;

        long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;

        return era * 146097 + doe - 719468;

    }

    // Weekday of the 1st of the month, 0 = Sunday .. 6 = Saturday.

    static int first_weekday(int y, int m)

    {

        long z = days_from_civil(y, m, 1);

        long w = (z + 4) % 7;   // 1970-01-01 was a Thursday

        if (w < 0)

        {

            w += 7;

        }

        return (int)w;

    }

    static const char* month_name(int m)

    {

        static const char* names[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

        return names[m - 1];

    }

    static const char* month_short(int m)

    {

        static const char* names[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

        return names[m - 1];

    }

    //endregion

    std::string pad2(int v)

    {

        std::string s = std::to_string(v);

        return (v < 10) ? "0" + s : s;

    }

    // The three editable segments of the display text, in order.

    void segments(std::string parts[3], std::string seps[2])

    {

        if (time_mode_)

        {

            parts[0] = pad2(hour_);

            parts[1] = pad2(minute_);

            parts[2] = pad2(second_);

            seps[0] = ":";

            seps[1] = ":";

        }

        else

        {

            parts[0] = std::to_string(day_);

            parts[1] = month_short(month_);

            parts[2] = std::to_string(year_);

            seps[0] = " ";

            seps[1] = " ";

        }

    }

    std::string display_text()

    {

        std::string parts[3], seps[2];

        segments(parts, seps);

        return parts[0] + seps[0] + parts[1] + seps[1] + parts[2];

    }

    // x offset where segment i starts, for highlight + hit testing.

    int segment_x(int index, SStyle& st)

    {

        std::string parts[3], seps[2];

        segments(parts, seps);

        int sx = x1_ + 6;

        for (int i = 0; i < index; ++i)

        {

            sx += xf::text_width(st, parts[i]) + xf::text_width(st, seps[i]);

        }

        return sx;

    }

    int segment_at(int x, SStyle& st)

    {

        std::string parts[3], seps[2];

        segments(parts, seps);

        for (int i = 0; i < 3; ++i)

        {

            int sx = segment_x(i, st);

            if (x >= sx - 2 && x <= sx + xf::text_width(st, parts[i]) + 2)

            {

                return i;

            }

        }

        return -1;

    }

    // Right-hand zones: spin up/down (14 px) then dropdown (18 px).

    int spin_x1()

    {

        return x2_ - 32;

    }

    int drop_x1()

    {

        return x2_ - 18;

    }

    // The dropdown calendar popup rectangle (drawn below the field).

    void calendar_rect(int& cx1, int& cy1, int& cx2, int& cy2)

    {

        cx1 = x1_;

        cy1 = y2_ + 2;

        cx2 = x1_ + 7 * 24 + 8;

        cy2 = cy1 + 22 + 14 + 6 * 20 + 6;

    }

    void clamp_day()

    {

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

    void adjust_field(int delta)

    {

        if (time_mode_)

        {

            if (field_ == 0)

            {

                hour_ = (hour_ + delta + 24) % 24;

            }

            else if (field_ == 1)

            {

                minute_ = (minute_ + delta + 60) % 60;

            }

            else

            {

                second_ = (second_ + delta + 60) % 60;

            }

        }

        else

        {

            if (field_ == 0)

            {

                day_ += delta;

                clamp_day();

            }

            else if (field_ == 1)

            {

                month_ += delta;

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

                clamp_day();

            }

            else

            {

                year_ += delta;

                clamp_day();

            }

        }

        text_ = display_text();

        if (on_change)

        {

            on_change();

        }

    }

public:

    std::function<void()> on_change;

    TDateTimePicker(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TDateTimePicker";

        text_ = display_text();

    }

    ~TDateTimePicker() {}

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

        if (!visible_)

        {

            return false;

        }

        if (x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_)

        {

            return true;

        }

        if (calendar_open_)

        {

            int cx1, cy1, cx2, cy2;

            calendar_rect(cx1, cy1, cx2, cy2);

            return x >= cx1 && x <= cx2 && y >= cy1 && y <= cy2;

        }

        return false;

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

        return display_text();

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    void set_date(int y, int m, int d)

    {

        year_ = y;

        month_ = m < 1 ? 1 : (m > 12 ? 12 : m);

        day_ = d;

        clamp_day();

        text_ = display_text();

    }

    void get_date(int& y, int& m, int& d)

    {

        y = year_;

        m = month_;

        d = day_;

    }

    void set_time(int h, int m, int s)

    {

        hour_ = h % 24;

        minute_ = m % 60;

        second_ = s % 60;

        text_ = display_text();

    }

    void get_time(int& h, int& m, int& s)

    {

        h = hour_;

        m = minute_;

        s = second_;

    }

    void set_time_mode(bool t)

    {

        time_mode_ = t;

        field_ = 0;

        calendar_open_ = false;

        text_ = display_text();

    }

    bool get_time_mode()

    {

        return time_mode_;

    }

    bool is_calendar_open()

    {

        return calendar_open_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        SStyle ts = st;

        if (!enabled_)

        {

            ts.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        // The field: white well with an outline.

        unsigned long white = styles_->get_color_rgb(255, 255, 255);

        XSetForeground(display_, gc_, white);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, (int)st.lineWidth_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        // Highlight the selected field segment.

        if (focused_)

        {

            std::string parts[3], seps[2];

            segments(parts, seps);

            int sx = segment_x(field_, st);

            int sw = xf::text_width(st, parts[field_]);

            XSetForeground(display_, gc_, styles_->get_color_rgb(180, 205, 240));

            XFillRectangle(display_, window_, gc_, sx - 2, y1_ + 3, sw + 4, height() - 6);

        }

        xf::draw_text_left(display_, window_, gc_, ts, x1_ + 6, y1_, y2_, display_text());

        // Spin up / down zone.

        int sx1 = spin_x1();

        int mid = (y1_ + y2_) / 2;

        xf::draw_polished(display_, window_, gc_, st, sx1, y1_ + 1, drop_x1(), mid);

        xf::draw_polished(display_, window_, gc_, st, sx1, mid, drop_x1(), y2_ - 1);

        xf::draw_triangle_up(display_, window_, gc_, ts.text_color_, (sx1 + drop_x1()) / 2, y1_ + height() / 4 + 2, 3);

        xf::draw_triangle_down(display_, window_, gc_, ts.text_color_, (sx1 + drop_x1()) / 2, y2_ - height() / 4 - 2, 3);

        // Dropdown button.

        xf::draw_polished(display_, window_, gc_, st, drop_x1(), y1_ + 1, x2_ - 1, y2_ - 1);

        if (time_mode_)

        {

            // A small clock face instead of a calendar arrow.

            int cx = (drop_x1() + x2_) / 2;

            int cy = (y1_ + y2_) / 2;

            XSetForeground(display_, gc_, ts.text_color_);

            XDrawArc(display_, window_, gc_, cx - 5, cy - 5, 10, 10, 0, 360 * 64);

            XDrawLine(display_, window_, gc_, cx, cy, cx, cy - 3);

            XDrawLine(display_, window_, gc_, cx, cy, cx + 3, cy);

        }

        else

        {

            xf::draw_triangle_down(display_, window_, gc_, ts.text_color_, (drop_x1() + x2_) / 2, (y1_ + y2_) / 2 - 2, 4);

        }

        if (calendar_open_ && !time_mode_)

        {

            draw_calendar(st, ts);

        }

        XFlush(display_);

    }

    void draw_calendar(SStyle& st, SStyle& ts)

    {

        int cx1, cy1, cx2, cy2;

        calendar_rect(cx1, cy1, cx2, cy2);

        unsigned long white = styles_->get_color_rgb(255, 255, 255);

        XSetForeground(display_, gc_, white);

        XFillRectangle(display_, window_, gc_, cx1, cy1, cx2 - cx1, cy2 - cy1);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, cx1, cy1, cx2 - cx1 - 1, cy2 - cy1 - 1);

        // Header: < Month Year >.

        std::string header = std::string(month_name(month_)) + " " + std::to_string(year_);

        xf::draw_text_centered(display_, window_, gc_, ts, cx1, cy1, cx2, cy1 + 22, header);

        xf::draw_text_left(display_, window_, gc_, ts, cx1 + 5, cy1, cy1 + 22, "<");

        int gt_w = xf::text_width(ts, ">");

        xf::draw_text_at(display_, window_, gc_, ts, cx2 - 5 - gt_w, cy1 + 16, ">");

        // Weekday initials.

        static const char* dow[] = {"S", "M", "T", "W", "T", "F", "S"};

        for (int c = 0; c < 7; ++c)

        {

            int cell_x = cx1 + 4 + c * 24;

            xf::draw_text_centered(display_, window_, gc_, ts, cell_x, cy1 + 22, cell_x + 24, cy1 + 36, dow[c]);

        }

        // Day cells.

        int first = first_weekday(year_, month_);

        int dim = days_in_month(year_, month_);

        unsigned long highlight = styles_->get_color_rgb(60, 120, 210);

        for (int d = 1; d <= dim; ++d)

        {

            int cell = first + d - 1;

            int col = cell % 7;

            int row = cell / 7;

            int cell_x = cx1 + 4 + col * 24;

            int cell_y = cy1 + 36 + row * 20;

            if (d == day_)

            {

                XSetForeground(display_, gc_, highlight);

                XFillRectangle(display_, window_, gc_, cell_x + 1, cell_y + 1, 22, 18);

            }

            SStyle cell_style = ts;

            if (d == day_)

            {

                cell_style.text_color_ = white;

            }

            xf::draw_text_centered(display_, window_, gc_, cell_style, cell_x, cell_y, cell_x + 24, cell_y + 20, std::to_string(d));

        }

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!enabled_ || button != 1)

        {

            return false;

        }

        // Calendar popup gets first claim on the click.

        if (calendar_open_)

        {

            int cx1, cy1, cx2, cy2;

            calendar_rect(cx1, cy1, cx2, cy2);

            if (x >= cx1 && x <= cx2 && y >= cy1 && y <= cy2)

            {

                focused_ = true;

                if (y < cy1 + 22)

                {

                    // Month steppers in the header.

                    if (x < cx1 + 24)

                    {

                        --month_;

                        if (month_ < 1)

                        {

                            month_ = 12;

                            --year_;

                        }

                        clamp_day();

                    }

                    else if (x > cx2 - 24)

                    {

                        ++month_;

                        if (month_ > 12)

                        {

                            month_ = 1;

                            ++year_;

                        }

                        clamp_day();

                    }

                    text_ = display_text();

                    return true;

                }

                int col = (x - cx1 - 4) / 24;

                int row = (y - cy1 - 36) / 20;

                if (col >= 0 && col < 7 && row >= 0 && row < 6)

                {

                    int d = row * 7 + col - first_weekday(year_, month_) + 1;

                    if (d >= 1 && d <= days_in_month(year_, month_))

                    {

                        day_ = d;

                        field_ = 0;

                        calendar_open_ = false;

                        text_ = display_text();

                        if (on_change)

                        {

                            on_change();

                        }

                    }

                }

                return true;

            }

            calendar_open_ = false;

            return true;

        }

        if (!contains(x, y))

        {

            return false;

        }

        focused_ = true;

        if (x >= drop_x1())

        {

            if (!time_mode_)

            {

                calendar_open_ = true;

            }

            return true;

        }

        if (x >= spin_x1())

        {

            int mid = (y1_ + y2_) / 2;

            adjust_field(y < mid ? 1 : -1);

            return true;

        }

        SStyle st = style();

        int seg = segment_at(x, st);

        if (seg >= 0)

        {

            field_ = seg;

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

            field_ = (field_ + 2) % 3;

            return true;

        }

        if (key == XK_Right)

        {

            field_ = (field_ + 1) % 3;

            return true;

        }

        if (key == XK_Up)

        {

            adjust_field(1);

            return true;

        }

        if (key == XK_Down)

        {

            adjust_field(-1);

            return true;

        }

        if (key == XK_Escape)

        {

            calendar_open_ = false;

            return true;

        }

        if ((key == XK_Return || key == XK_space) && !time_mode_)

        {

            calendar_open_ = !calendar_open_;

            return true;

        }

        return false;

    }

};

//endregion
