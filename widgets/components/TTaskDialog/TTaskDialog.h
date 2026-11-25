#pragma once

#include "core/xframe_core.h"

//region TTaskDialog

class TTaskDialog

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;          // the dialog caption (title bar)

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    // Lazarus modal results

    static const int mrNone = 0;

    static const int mrOk = 1;

    static const int mrCancel = 2;

    static const int mrAbort = 3;

    static const int mrRetry = 4;

    static const int mrIgnore = 5;

    static const int mrYes = 6;

    static const int mrNo = 7;

    // Icon kinds

    static const int tdiNone = 0;

    static const int tdiInformation = 1;

    static const int tdiWarning = 2;

    static const int tdiError = 3;

    static const int tdiQuestion = 4;

    int icon_ = tdiInformation;

    std::string instruction_;

    std::string content_;

    std::vector<std::string> buttons_;

    std::vector<std::string> radios_;

    int radio_index_ = 0;

    int button_index_ = -1;

    int modal_result_ = 0;

    bool in_rect(int px, int py, int rx1, int ry1, int rx2, int ry2)

    {

        return px >= rx1 && px <= rx2 && py >= ry1 && py <= ry2;

    }

    int result_for_caption(const std::string& cap, int index)

    {

        if (cap == "OK")

        {

            return mrOk;

        }

        if (cap == "Cancel")

        {

            return mrCancel;

        }

        if (cap == "Abort")

        {

            return mrAbort;

        }

        if (cap == "Retry")

        {

            return mrRetry;

        }

        if (cap == "Ignore")

        {

            return mrIgnore;

        }

        if (cap == "Yes")

        {

            return mrYes;

        }

        if (cap == "No")

        {

            return mrNo;

        }

        return 100 + index;   // custom button

    }

    std::vector<std::string> wrap_text(const std::string& s, int maxw)

    {

        SStyle st = style();

        std::vector<std::string> lines;

        std::string cur;

        size_t i = 0;

        while (i < s.size())

        {

            size_t j = s.find(' ', i);

            std::string word = s.substr(i, j == std::string::npos ? std::string::npos : j - i);

            std::string trial = cur.empty() ? word : cur + " " + word;

            if (xf::text_width(st, trial) > maxw && !cur.empty())

            {

                lines.push_back(cur);

                cur = word;

            }

            else

            {

                cur = trial;

            }

            if (j == std::string::npos)

            {

                break;

            }

            i = j + 1;

        }

        if (!cur.empty())

        {

            lines.push_back(cur);

        }

        return lines;

    }

    int content_top()

    {

        return y1_ + 76;

    }

    int content_lines_height()

    {

        return (int)wrap_text(content_, width() - 96).size() * 16;

    }

    int radios_top()

    {

        return content_top() + content_lines_height() + 10;

    }

    // Buttons are laid out right-aligned along the bottom.

    void button_rect(int i, int& bx1, int& by1, int& bx2, int& by2)

    {

        SStyle st = style();

        int total = 0;

        for (size_t k = 0; k < buttons_.size(); ++k)

        {

            total += xf::text_width(st, buttons_[k]) + 26 + (k ? 8 : 0);

        }

        int cx = x2_ - 12 - total;

        for (int k = 0; k < i; ++k)

        {

            cx += xf::text_width(st, buttons_[(size_t)k]) + 26 + 8;

        }

        bx1 = cx;

        bx2 = cx + xf::text_width(st, buttons_[(size_t)i]) + 26;

        by1 = y2_ - 38;

        by2 = y2_ - 12;

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

    void draw_icon(int ix, int iy, int size)

    {

        SStyle st = style();

        SStyle wst = st;

        wst.text_color_ = styles_->get_color_rgb(255, 255, 255);

        if (icon_ == tdiInformation || icon_ == tdiQuestion || icon_ == tdiError)

        {

            unsigned long fill = styles_->get_color_rgb(50, 110, 200);

            if (icon_ == tdiError)

            {

                fill = styles_->get_color_rgb(205, 45, 45);

            }

            XSetForeground(display_, gc_, fill);

            XFillArc(display_, window_, gc_, ix, iy, size, size, 0, 360 * 64);

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, 1);

            XDrawArc(display_, window_, gc_, ix, iy, size, size, 0, 360 * 64);

            if (icon_ == tdiInformation)

            {

                xf::draw_text_centered(display_, window_, gc_, wst, ix, iy, ix + size, iy + size, "i");

            }

            else if (icon_ == tdiQuestion)

            {

                xf::draw_text_centered(display_, window_, gc_, wst, ix, iy, ix + size, iy + size, "?");

            }

            else

            {

                int m = size / 4;

                XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

                xf::set_line_width(display_, gc_, 3);

                XDrawLine(display_, window_, gc_, ix + m, iy + m, ix + size - m, iy + size - m);

                XDrawLine(display_, window_, gc_, ix + size - m, iy + m, ix + m, iy + size - m);

                xf::set_line_width(display_, gc_, 1);

            }

        }

        else if (icon_ == tdiWarning)

        {

            // Yellow warning triangle built from shrinking scan lines.

            int cx = ix + size / 2;

            XSetForeground(display_, gc_, styles_->get_color_rgb(245, 200, 40));

            for (int r = 0; r < size; ++r)

            {

                int half = (r * size / 2) / std::max(1, size);

                XDrawLine(display_, window_, gc_, cx - half, iy + r, cx + half, iy + r);

            }

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawLine(display_, window_, gc_, cx, iy, ix, iy + size);

            XDrawLine(display_, window_, gc_, cx, iy, ix + size, iy + size);

            XDrawLine(display_, window_, gc_, ix, iy + size, ix + size, iy + size);

            SStyle bst = st;

            bst.text_color_ = styles_->get_color_rgb(0, 0, 0);

            xf::draw_text_centered(display_, window_, gc_, bst, ix, iy + size / 3, ix + size, iy + size, "!");

        }

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

    std::function<void()> on_click;

    TTaskDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TTaskDialog";

        text_ = "Task Dialog";

        instruction_ = "The widget library has been rebuilt.";

        content_ = "All GTK and Lazarus components are now available as standalone X11 classes. Do you want to open the demo project now, or keep browsing the component palette first?";

        buttons_ = {"Yes", "No", "Cancel"};

        radios_ = {"Open the demo", "Browse the palette"};

    }

    ~TTaskDialog() {}

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

    void set_icon(int icon)

    {

        icon_ = icon;

    }

    int get_icon()

    {

        return icon_;

    }

    void set_instruction(std::string s)

    {

        instruction_ = s;

    }

    std::string get_instruction()

    {

        return instruction_;

    }

    void set_content(std::string s)

    {

        content_ = s;

    }

    std::string get_content()

    {

        return content_;

    }

    void clear_buttons()

    {

        buttons_.clear();

    }

    void add_button(std::string caption)

    {

        buttons_.push_back(caption);

    }

    int button_count()

    {

        return (int)buttons_.size();

    }

    void clear_radios()

    {

        radios_.clear();

        radio_index_ = 0;

    }

    void add_radio(std::string caption)

    {

        radios_.push_back(caption);

    }

    int get_radio_index()

    {

        return radios_.empty() ? -1 : radio_index_;

    }

    int get_modal_result()

    {

        return modal_result_;

    }

    int get_result()

    {

        return modal_result_;

    }

    int get_button_index()

    {

        return button_index_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        draw_chrome();

        draw_icon(x1_ + 16, y1_ + 36, 36);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 64, y1_ + 34, y1_ + 58, instruction_);

        std::vector<std::string> lines = wrap_text(content_, width() - 96);

        for (size_t i = 0; i < lines.size(); ++i)

        {

            int ly = content_top() + (int)i * 16;

            xf::draw_text_at(display_, window_, gc_, st, x1_ + 64, ly + 12, lines[i]);

        }

        for (size_t i = 0; i < radios_.size(); ++i)

        {

            draw_radio(x1_ + 64, radios_top() + (int)i * 20, radio_index_ == (int)i, radios_[i]);

        }

        for (size_t i = 0; i < buttons_.size(); ++i)

        {

            int bx1, by1, bx2, by2;

            button_rect((int)i, bx1, by1, bx2, by2);

            draw_mini_button(bx1, by1, bx2, by2, buttons_[i], button_index_ == (int)i && modal_result_ != 0);

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

        if (in_rect(x, y, x2_ - 24, y1_ + 4, x2_ - 6, y1_ + 22))

        {

            modal_result_ = mrCancel;

            return true;

        }

        for (size_t i = 0; i < radios_.size(); ++i)

        {

            if (in_rect(x, y, x1_ + 64, radios_top() + (int)i * 20, x1_ + 64 + 18 + xf::text_width(style(), radios_[i]), radios_top() + (int)i * 20 + 14))

            {

                radio_index_ = (int)i;

                return true;

            }

        }

        for (size_t i = 0; i < buttons_.size(); ++i)

        {

            int bx1, by1, bx2, by2;

            button_rect((int)i, bx1, by1, bx2, by2);

            if (in_rect(x, y, bx1, by1, bx2, by2))

            {

                button_index_ = (int)i;

                modal_result_ = result_for_caption(buttons_[i], (int)i);

                if (on_click)

                {

                    on_click();

                }

                return true;

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

        if (key == XK_Escape)

        {

            modal_result_ = mrCancel;

            return true;

        }

        if (key == XK_Up || key == XK_Down)

        {

            if (!radios_.empty())

            {

                int d = (key == XK_Up) ? -1 : 1;

                radio_index_ = std::max(0, std::min((int)radios_.size() - 1, radio_index_ + d));

            }

            return true;

        }

        if (key == XK_Return)

        {

            if (!buttons_.empty())

            {

                button_index_ = 0;

                modal_result_ = result_for_caption(buttons_[0], 0);

                if (on_click)

                {

                    on_click();

                }

            }

            return true;

        }

        return false;

    }

};

//endregion
