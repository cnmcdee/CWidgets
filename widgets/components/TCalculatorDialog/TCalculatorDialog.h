#pragma once

#include "core/xframe_core.h"

//region TCalculatorDialog

class TCalculatorDialog

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

    int result_ = 0;

    // Working calculator state

    std::string display_text_;

    double accumulator_ = 0.0;

    char pending_op_ = 0;      // 0, '+', '-', '*', '/'

    bool fresh_input_ = true;  // next digit starts a new entry

    bool error_ = false;

    std::string pressed_label_;   // keypad button currently held down

    bool in_rect(int px, int py, int rx1, int ry1, int rx2, int ry2)

    {

        return px >= rx1 && px <= rx2 && py >= ry1 && py <= ry2;

    }

    std::string format_value(double v)

    {

        if (error_)

        {

            return "Error";

        }

        char buf[40];

        snprintf(buf, sizeof(buf), "%.10g", v);

        return buf;

    }

    double entry_value()

    {

        return std::atof(display_text_.c_str());

    }

    bool apply_pending()

    {

        double rhs = entry_value();

        if (pending_op_ == 0)

        {

            accumulator_ = rhs;

            return true;

        }

        if (pending_op_ == '+')

        {

            accumulator_ += rhs;

        }

        else if (pending_op_ == '-')

        {

            accumulator_ -= rhs;

        }

        else if (pending_op_ == '*')

        {

            accumulator_ *= rhs;

        }

        else if (pending_op_ == '/')

        {

            if (rhs == 0.0)

            {

                error_ = true;

                display_text_ = "Error";

                return false;

            }

            accumulator_ /= rhs;

        }

        display_text_ = format_value(accumulator_);

        return true;

    }

    void press_digit(char d)

    {

        if (error_)

        {

            clear_all();

        }

        if (fresh_input_ || display_text_ == "0")

        {

            display_text_ = std::string(1, d);

            fresh_input_ = false;

        }

        else if (display_text_.size() < 14)

        {

            display_text_ += d;

        }

    }

    void press_dot()

    {

        if (error_)

        {

            clear_all();

        }

        if (fresh_input_)

        {

            display_text_ = "0.";

            fresh_input_ = false;

            return;

        }

        if (display_text_.find('.') == std::string::npos && display_text_.size() < 14)

        {

            display_text_ += '.';

        }

    }

    void press_op(char op)

    {

        if (error_)

        {

            return;

        }

        if (!fresh_input_ || pending_op_ == 0)

        {

            if (!apply_pending())

            {

                pending_op_ = 0;

                fresh_input_ = true;

                return;

            }

        }

        pending_op_ = op;

        fresh_input_ = true;

    }

    void press_equals()

    {

        if (error_)

        {

            return;

        }

        if (pending_op_ != 0)

        {

            apply_pending();

            pending_op_ = 0;

        }

        else

        {

            accumulator_ = entry_value();

            display_text_ = format_value(accumulator_);

        }

        fresh_input_ = true;

    }

    void clear_all()

    {

        display_text_ = "0";

        accumulator_ = 0.0;

        pending_op_ = 0;

        fresh_input_ = true;

        error_ = false;

    }

    void backspace_entry()

    {

        if (fresh_input_ || error_)

        {

            return;

        }

        if (display_text_.size() > 1)

        {

            display_text_.pop_back();

        }

        else

        {

            display_text_ = "0";

        }

    }

    void press_label(const std::string& label)

    {

        if (label.size() == 1 && label[0] >= '0' && label[0] <= '9')

        {

            press_digit(label[0]);

        }

        else if (label == ".")

        {

            press_dot();

        }

        else if (label == "+" || label == "-" || label == "*" || label == "/")

        {

            press_op(label[0]);

        }

        else if (label == "=")

        {

            press_equals();

        }

        else if (label == "C")

        {

            clear_all();

        }

    }

    const char* key_label(int row, int col)

    {

        static const char* grid[4][4] = {

            {"7", "8", "9", "/"},

            {"4", "5", "6", "*"},

            {"1", "2", "3", "-"},

            {"0", ".", "=", "+"},

        };

        return grid[row][col];

    }

    // Keypad geometry: rows 0..3 are the 4x4 grid, row 4 is the wide C.

    void cell_rect(int row, int col, int& bx1, int& by1, int& bx2, int& by2)

    {

        int gx1 = x1_ + 12;

        int gy1 = y1_ + 84;

        int gx2 = x2_ - 12;

        int gy2 = y2_ - 12;

        int cell_w = (gx2 - gx1 - 3 * 6) / 4;

        int cell_h = (gy2 - gy1 - 4 * 6) / 5;

        if (row == 4)

        {

            bx1 = gx1;

            bx2 = gx2;

        }

        else

        {

            bx1 = gx1 + col * (cell_w + 6);

            bx2 = bx1 + cell_w;

        }

        by1 = gy1 + row * (cell_h + 6);

        by2 = by1 + cell_h;

    }

    std::string label_at(int x, int y)

    {

        for (int r = 0; r < 4; ++r)

        {

            for (int c = 0; c < 4; ++c)

            {

                int bx1, by1, bx2, by2;

                cell_rect(r, c, bx1, by1, bx2, by2);

                if (in_rect(x, y, bx1, by1, bx2, by2))

                {

                    return key_label(r, c);

                }

            }

        }

        int bx1, by1, bx2, by2;

        cell_rect(4, 0, bx1, by1, bx2, by2);

        if (in_rect(x, y, bx1, by1, bx2, by2))

        {

            return "C";

        }

        return "";

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

    void draw_key(int bx1, int by1, int bx2, int by2, const std::string& cap)

    {

        SStyle st = style();

        if (!pressed_label_.empty() && cap.rfind(pressed_label_, 0) == 0)

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

    TCalculatorDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TCalculatorDialog";

        text_ = "Calculator";

        display_text_ = "0";

        // Preload a sum so the demo opens mid-calculation: 12 + ...

        press_digit('1');

        press_digit('2');

        press_op('+');

    }

    ~TCalculatorDialog() {}

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

    std::string get_display()

    {

        return display_text_;

    }

    double get_value()

    {

        return entry_value();

    }

    void set_value(double v)

    {

        clear_all();

        display_text_ = format_value(v);

        accumulator_ = v;

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

        //region Display

        int dx1 = x1_ + 12;

        int dy1 = y1_ + 32;

        int dx2 = x2_ - 12;

        int dy2 = y1_ + 74;

        XSetForeground(display_, gc_, styles_->get_color_rgb(250, 252, 245));

        XFillRectangle(display_, window_, gc_, dx1, dy1, dx2 - dx1, dy2 - dy1);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, dx1, dy1, dx2 - dx1, dy2 - dy1);

        std::string shown = display_text_;

        if (pending_op_ != 0 && fresh_input_ && !error_)

        {

            shown = display_text_ + " " + std::string(1, pending_op_);

        }

        int tw = xf::text_width(st, shown);

        int baseline = dy1 + ((dy2 - dy1) + (st.font_ ? st.font_->ascent - st.font_->descent : 10)) / 2;

        xf::draw_text_at(display_, window_, gc_, st, dx2 - tw - 10, baseline, shown);

        //endregion

        //region Keypad

        for (int r = 0; r < 4; ++r)

        {

            for (int c = 0; c < 4; ++c)

            {

                int bx1, by1, bx2, by2;

                cell_rect(r, c, bx1, by1, bx2, by2);

                draw_key(bx1, by1, bx2, by2, key_label(r, c));

            }

        }

        int bx1, by1, bx2, by2;

        cell_rect(4, 0, bx1, by1, bx2, by2);

        draw_key(bx1, by1, bx2, by2, "C  (Clear)");

        //endregion

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

            result_ = mrOk;

            return true;

        }

        std::string label = label_at(x, y);

        if (!label.empty())

        {

            pressed_label_ = (label == "C  (Clear)") ? "C" : label;

            press_label(pressed_label_);

            return true;

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button == 1 && !pressed_label_.empty())

        {

            pressed_label_.clear();

            return true;

        }

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

            clear_all();

            return true;

        }

        if (key == XK_Return || key == XK_KP_Enter || key == XK_equal)

        {

            press_equals();

            pressed_label_ = "=";

            return true;

        }

        if (key == XK_BackSpace)

        {

            backspace_entry();

            return true;

        }

        if (str && str[0])

        {

            char c = str[0];

            if (c >= '0' && c <= '9')

            {

                press_digit(c);

                return true;

            }

            if (c == '.' || c == '+' || c == '-' || c == '*' || c == '/')

            {

                press_label(std::string(1, c));

                return true;

            }

            if (c == 'c' || c == 'C')

            {

                clear_all();

                return true;

            }

        }

        (void)key;

        return false;

    }

};

//endregion
