#pragma once

#include "core/xframe_core.h"

//region TCalendarDialog

class TCalendarDialog

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

    int year_ = 2026;

    int month_ = 11;   // 1..12

    int day_ = 25;

    int result_ = 0;

    bool in_rect(int px, int py, int rx1, int ry1, int rx2, int ry2)

    {

        return px >= rx1 && px <= rx2 && py >= ry1 && py <= ry2;

    }

    bool is_leap(int y)

    {

        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);

    }

    int days_in_month(int y, int m)

    {

        static const int dm[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        if (m == 2 && is_leap(y))

        {

            return 29;

        }

        return dm[m - 1];

    }

    // Sakamoto's algorithm: 0 = Sunday .. 6 = Saturday.

    int day_of_week(int y, int m, int d)

    {

        static const int t[12] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

        if (m < 3)

        {

            y -= 1;

        }

        return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;

    }

    std::string month_name(int m)

    {

        static const char* names[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

        return names[m - 1];

    }

    std::string pad2(int v)

    {

        if (v < 10)

        {

            return "0" + std::to_string(v);

        }

        return std::to_string(v);

    }

    void step_month(int delta)

    {

        month_ += delta;

        if (month_ < 1)

        {

            month_ = 12;

            year_--;

        }

        if (month_ > 12)

        {

            month_ = 1;

            year_++;

        }

        int dim = days_in_month(year_, month_);

        if (day_ > dim)

        {

            day_ = dim;

        }

    }

    // Grid geometry.

    int grid_x1()

    {

        return x1_ + 12;

    }

    int grid_y1()

    {

        return y1_ + 84;   // below the header + weekday row

    }

    int cell_w()

    {

        return (width() - 24) / 7;

    }

    int cell_h()

    {

        return 26;

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

    TCalendarDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TCalendarDialog";

        text_ = "Select a Date";

        year_ = 2026;

        month_ = 11;

        day_ = 25;

    }

    ~TCalendarDialog() {}

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

    void set_date(int y, int m, int d)

    {

        year_ = y;

        month_ = std::max(1, std::min(12, m));

        day_ = std::max(1, std::min(days_in_month(year_, month_), d));

    }

    std::string get_date()

    {

        return std::to_string(year_) + "-" + pad2(month_) + "-" + pad2(day_);

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

        //region Month header with prev / next buttons

        draw_mini_button(x1_ + 12, y1_ + 30, x1_ + 44, y1_ + 54, "<", false);

        draw_mini_button(x2_ - 44, y1_ + 30, x2_ - 12, y1_ + 54, ">", false);

        xf::draw_text_centered(display_, window_, gc_, st, x1_ + 48, y1_ + 30, x2_ - 48, y1_ + 54, month_name(month_) + " " + std::to_string(year_));

        //endregion

        //region Weekday header row

        static const char* wd[7] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

        int cw = cell_w();

        for (int c = 0; c < 7; ++c)

        {

            int cx = grid_x1() + c * cw;

            SStyle wst = st;

            if (c == 0 || c == 6)

            {

                wst.text_color_ = styles_->get_color_rgb(190, 60, 50);

            }

            xf::draw_text_centered(display_, window_, gc_, wst, cx, y1_ + 58, cx + cw, y1_ + 80, wd[c]);

        }

        //endregion

        //region Day grid

        int gx1 = grid_x1();

        int gy1 = grid_y1();

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, gx1, gy1, cw * 7, cell_h() * 6);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, gx1, gy1, cw * 7, cell_h() * 6);

        int first = day_of_week(year_, month_, 1);

        int dim = days_in_month(year_, month_);

        for (int d = 1; d <= dim; ++d)

        {

            int cell = first + d - 1;

            int cx = gx1 + (cell % 7) * cw;

            int cy = gy1 + (cell / 7) * cell_h();

            SStyle dst = st;

            int dow = (first + d - 1) % 7;

            if (dow == 0 || dow == 6)

            {

                dst.text_color_ = styles_->get_color_rgb(190, 60, 50);

            }

            if (d == day_)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, cx + 1, cy + 1, cw - 2, cell_h() - 2);

                dst.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            xf::draw_text_centered(display_, window_, gc_, dst, cx, cy, cx + cw, cy + cell_h(), std::to_string(d));

        }

        //endregion

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y2_ - 66, y2_ - 44, "Selected: " + get_date());

        draw_mini_button(x2_ - 188, y2_ - 34, x2_ - 100, y2_ - 10, "OK", false);

        draw_mini_button(x2_ - 92, y2_ - 34, x2_ - 12, y2_ - 10, "Cancel", false);

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

        if (in_rect(x, y, x1_ + 12, y1_ + 30, x1_ + 44, y1_ + 54))

        {

            step_month(-1);

            return true;

        }

        if (in_rect(x, y, x2_ - 44, y1_ + 30, x2_ - 12, y1_ + 54))

        {

            step_month(1);

            return true;

        }

        int gx1 = grid_x1();

        int gy1 = grid_y1();

        int cw = cell_w();

        if (in_rect(x, y, gx1, gy1, gx1 + cw * 7, gy1 + cell_h() * 6))

        {

            int col = (x - gx1) / cw;

            int row = (y - gy1) / cell_h();

            int first = day_of_week(year_, month_, 1);

            int d = row * 7 + col - first + 1;

            if (d >= 1 && d <= days_in_month(year_, month_))

            {

                day_ = d;

            }

            return true;

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

        int dim = days_in_month(year_, month_);

        if (key == XK_Left)

        {

            if (day_ > 1)

            {

                day_--;

            }

            return true;

        }

        if (key == XK_Right)

        {

            if (day_ < dim)

            {

                day_++;

            }

            return true;

        }

        if (key == XK_Up)

        {

            if (day_ > 7)

            {

                day_ -= 7;

            }

            return true;

        }

        if (key == XK_Down)

        {

            if (day_ + 7 <= dim)

            {

                day_ += 7;

            }

            return true;

        }

        if (key == XK_Page_Up)

        {

            step_month(-1);

            return true;

        }

        if (key == XK_Page_Down)

        {

            step_month(1);

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
