#pragma once

#include "core/xframe_core.h"

//region TPrintDialog

class TPrintDialog

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

    std::vector<std::string> printers_;

    int sel_printer_ = 0;

    int copies_val_ = 1;

    std::string copies_text_;

    int copies_cursor_ = 0;

    int range_mode_ = 0;   // 0 = All, 1 = Pages, 2 = Selection

    std::string from_text_;

    std::string to_text_;

    int from_cursor_ = 0;

    int to_cursor_ = 0;

    int focus_field_ = 0;   // 0 none, 1 copies, 2 from, 3 to

    int result_ = 0;

    bool in_rect(int px, int py, int rx1, int ry1, int rx2, int ry2)

    {

        return px >= rx1 && px <= rx2 && py >= ry1 && py <= ry2;

    }

    void sync_copies()

    {

        copies_text_ = std::to_string(copies_val_);

        copies_cursor_ = (int)copies_text_.size();

    }

    int parse_int(const std::string& s, int fallback)

    {

        int v = 0;

        bool any = false;

        for (size_t i = 0; i < s.size(); ++i)

        {

            if (s[i] >= '0' && s[i] <= '9')

            {

                v = v * 10 + (s[i] - '0');

                any = true;

            }

        }

        return any ? v : fallback;

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

    void draw_edit(int ex1, int ey1, int ex2, int ey2, const std::string& s, int cursor, bool foc, bool active)

    {

        SStyle st = style();

        XSetForeground(display_, gc_, styles_->get_color_rgb(active ? 255 : 225, active ? 255 : 225, active ? 255 : 225));

        XFillRectangle(display_, window_, gc_, ex1, ey1, ex2 - ex1, ey2 - ey1);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, ex1, ey1, ex2 - ex1, ey2 - ey1);

        SStyle est = st;

        if (!active)

        {

            est.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        xf::draw_text_left(display_, window_, gc_, est, ex1 + 5, ey1, ey2, s);

        if (foc && active)

        {

            int cx = ex1 + 5 + xf::text_width(st, s.substr(0, (size_t)cursor));

            XSetForeground(display_, gc_, st.text_color_);

            XDrawLine(display_, window_, gc_, cx, ey1 + 3, cx, ey2 - 3);

        }

    }

    bool edit_key_digits(std::string& buf, int& cur, KeySym key, const char* str)

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

        if (str && str[0] >= '0' && str[0] <= '9' && (int)buf.size() < 3)

        {

            buf.insert((size_t)cur, 1, str[0]);

            cur++;

            return true;

        }

        return false;

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

    TPrintDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TPrintDialog";

        text_ = "Print";

        printers_ = {"Office LaserJet Pro", "PDF Printer", "Photo Inkjet 2600", "Network Plotter"};

        from_text_ = "1";

        from_cursor_ = 1;

        to_text_ = "10";

        to_cursor_ = 2;

        sync_copies();

    }

    ~TPrintDialog() {}

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

    std::string get_printer_name()

    {

        return printers_[(size_t)sel_printer_];

    }

    int get_copies()

    {

        int v = parse_int(copies_text_, 1);

        if (v < 1)

        {

            v = 1;

        }

        if (v > 99)

        {

            v = 99;

        }

        return v;

    }

    void set_copies(int n)

    {

        copies_val_ = std::max(1, std::min(99, n));

        sync_copies();

    }

    int get_range_mode()

    {

        return range_mode_;

    }

    int get_page_from()

    {

        return parse_int(from_text_, 1);

    }

    int get_page_to()

    {

        return parse_int(to_text_, 1);

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

        //region Printer list

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y1_ + 28, y1_ + 48, "Printer:");

        int lx1 = x1_ + 12;

        int ly1 = y1_ + 50;

        int lx2 = x2_ - 12;

        int ly2 = y1_ + 122;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, lx1, ly1, lx2 - lx1, ly2 - ly1);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, lx1, ly1, lx2 - lx1, ly2 - ly1);

        for (size_t i = 0; i < printers_.size(); ++i)

        {

            int ry = ly1 + (int)i * 18;

            SStyle rst = st;

            if ((int)i == sel_printer_)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, lx1 + 1, ry, lx2 - lx1 - 2, 18);

                rst.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            // little printer glyph: body + paper

            XSetForeground(display_, gc_, styles_->get_color_rgb(250, 250, 250));

            XFillRectangle(display_, window_, gc_, lx1 + 5, ry + 3, 8, 5);

            XFillRectangle(display_, window_, gc_, lx1 + 3, ry + 8, 12, 6);

            XSetForeground(display_, gc_, rst.text_color_);

            XDrawRectangle(display_, window_, gc_, lx1 + 3, ry + 8, 12, 6);

            xf::draw_text_left(display_, window_, gc_, rst, lx1 + 22, ry, ry + 18, printers_[i]);

        }

        //endregion

        //region Copies spin edit

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y1_ + 132, y1_ + 156, "Copies:");

        draw_edit(x1_ + 70, y1_ + 132, x1_ + 114, y1_ + 156, copies_text_, copies_cursor_, focus_field_ == 1, true);

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 116, y1_ + 132, 16, 11);

        XFillRectangle(display_, window_, gc_, x1_ + 116, y1_ + 145, 16, 11);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_ + 116, y1_ + 132, 16, 24);

        XDrawLine(display_, window_, gc_, x1_ + 116, y1_ + 144, x1_ + 132, y1_ + 144);

        xf::draw_triangle_up(display_, window_, gc_, st.text_color_, x1_ + 124, y1_ + 140, 3);

        xf::draw_triangle_down(display_, window_, gc_, st.text_color_, x1_ + 124, y1_ + 148, 3);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 150, y1_ + 132, y1_ + 156, "Status: Ready");

        //endregion

        //region Page range group

        int gx1 = x1_ + 12;

        int gy1 = y1_ + 168;

        int gx2 = x2_ - 12;

        int gy2 = y2_ - 48;

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, gx1, gy1, gx2 - gx1, gy2 - gy1);

        xf::draw_text_left(display_, window_, gc_, st, gx1 + 8, gy1 - 16, gy1 + 2, "Page range");

        draw_radio(gx1 + 12, gy1 + 12, range_mode_ == 0, "All");

        draw_radio(gx1 + 12, gy1 + 36, range_mode_ == 1, "Pages:");

        draw_edit(gx1 + 92, gy1 + 32, gx1 + 136, gy1 + 56, from_text_, from_cursor_, focus_field_ == 2, range_mode_ == 1);

        xf::draw_text_left(display_, window_, gc_, st, gx1 + 142, gy1 + 32, gy1 + 56, "to");

        draw_edit(gx1 + 164, gy1 + 32, gx1 + 208, gy1 + 56, to_text_, to_cursor_, focus_field_ == 3, range_mode_ == 1);

        draw_radio(gx1 + 12, gy1 + 64, range_mode_ == 2, "Selection");

        //endregion

        draw_mini_button(x2_ - 188, y2_ - 34, x2_ - 100, y2_ - 10, "Print", false);

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

        if (in_rect(x, y, x1_ + 12, y1_ + 50, x2_ - 12, y1_ + 122))

        {

            int idx = (y - (y1_ + 50)) / 18;

            if (idx >= 0 && idx < (int)printers_.size())

            {

                sel_printer_ = idx;

            }

            focus_field_ = 0;

            return true;

        }

        if (in_rect(x, y, x1_ + 70, y1_ + 132, x1_ + 114, y1_ + 156))

        {

            focus_field_ = 1;

            copies_cursor_ = (int)copies_text_.size();

            return true;

        }

        if (in_rect(x, y, x1_ + 116, y1_ + 132, x1_ + 132, y1_ + 144))

        {

            copies_val_ = std::min(99, get_copies() + 1);

            sync_copies();

            return true;

        }

        if (in_rect(x, y, x1_ + 116, y1_ + 145, x1_ + 132, y1_ + 156))

        {

            copies_val_ = std::max(1, get_copies() - 1);

            sync_copies();

            return true;

        }

        int gx1 = x1_ + 12;

        int gy1 = y1_ + 168;

        if (in_rect(x, y, gx1 + 12, gy1 + 10, gx1 + 120, gy1 + 26))

        {

            range_mode_ = 0;

            return true;

        }

        if (in_rect(x, y, gx1 + 12, gy1 + 34, gx1 + 90, gy1 + 50))

        {

            range_mode_ = 1;

            return true;

        }

        if (in_rect(x, y, gx1 + 12, gy1 + 62, gx1 + 140, gy1 + 78))

        {

            range_mode_ = 2;

            return true;

        }

        if (range_mode_ == 1 && in_rect(x, y, gx1 + 92, gy1 + 32, gx1 + 136, gy1 + 56))

        {

            focus_field_ = 2;

            from_cursor_ = (int)from_text_.size();

            return true;

        }

        if (range_mode_ == 1 && in_rect(x, y, gx1 + 164, gy1 + 32, gx1 + 208, gy1 + 56))

        {

            focus_field_ = 3;

            to_cursor_ = (int)to_text_.size();

            return true;

        }

        if (in_rect(x, y, x2_ - 188, y2_ - 34, x2_ - 100, y2_ - 10))

        {

            result_ = mrOk;

            if (on_click)

            {

                on_click();

            }

            return true;

        }

        if (in_rect(x, y, x2_ - 92, y2_ - 34, x2_ - 12, y2_ - 10))

        {

            result_ = mrCancel;

            return true;

        }

        focus_field_ = 0;

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

        if (focus_field_ == 1)

        {

            bool handled = edit_key_digits(copies_text_, copies_cursor_, key, str);

            if (handled)

            {

                copies_val_ = get_copies();

            }

            return handled;

        }

        if (focus_field_ == 2)

        {

            return edit_key_digits(from_text_, from_cursor_, key, str);

        }

        if (focus_field_ == 3)

        {

            return edit_key_digits(to_text_, to_cursor_, key, str);

        }

        return false;

    }

};

//endregion
