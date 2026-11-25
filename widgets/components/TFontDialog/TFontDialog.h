#pragma once

#include "core/xframe_core.h"

//region TFontDialog

class TFontDialog

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

    std::vector<std::string> families_;

    std::vector<std::string> styles_list_;

    std::vector<std::string> sizes_;

    int sel_family_ = 0;

    int sel_style_ = 0;

    int sel_size_ = 4;

    int top_family_ = 0;

    int top_style_ = 0;

    int top_size_ = 0;

    bool strikeout_ = false;

    bool underline_ = false;

    std::string size_text_;

    int size_cursor_ = 0;

    bool size_edit_focus_ = false;

    int result_ = 0;

    bool in_rect(int px, int py, int rx1, int ry1, int rx2, int ry2)

    {

        return px >= rx1 && px <= rx2 && py >= ry1 && py <= ry2;

    }

    // Column geometry: family / style / size list boxes.

    void column_rects(int& fx1, int& fx2, int& sx1, int& sx2, int& zx1, int& zx2)

    {

        int inner_w = width() - 24;

        fx1 = x1_ + 12;

        fx2 = fx1 + inner_w * 45 / 100;

        sx1 = fx2 + 8;

        sx2 = sx1 + inner_w * 30 / 100;

        zx1 = sx2 + 8;

        zx2 = x2_ - 12;

    }

    int list_y1()

    {

        return y1_ + 72;

    }

    int list_y2()

    {

        return y2_ - 104;

    }

    int list_rows()

    {

        return (list_y2() - list_y1()) / 17;

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

    void draw_list(int bx1, int bx2, const std::vector<std::string>& items, int sel, int top)

    {

        SStyle st = style();

        int by1 = list_y1();

        int by2 = list_y2();

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, bx1, by1, bx2 - bx1, by2 - by1);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, bx1, by1, bx2 - bx1, by2 - by1);

        int rows = list_rows();

        for (int i = top; i < (int)items.size() && i < top + rows; ++i)

        {

            int ry = by1 + (i - top) * 17;

            SStyle rst = st;

            if (i == sel)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, bx1 + 1, ry, bx2 - bx1 - 2, 17);

                rst.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            xf::draw_text_left(display_, window_, gc_, rst, bx1 + 5, ry, ry + 17, items[(size_t)i]);

        }

    }

    void draw_display_edit(int ex1, int ex2, const std::string& s, bool foc, int cursor)

    {

        SStyle st = style();

        int ey1 = y1_ + 46;

        int ey2 = y1_ + 68;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, ex1, ey1, ex2 - ex1, ey2 - ey1);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, ex1, ey1, ex2 - ex1, ey2 - ey1);

        xf::draw_text_left(display_, window_, gc_, st, ex1 + 5, ey1, ey2, s);

        if (foc)

        {

            int cx = ex1 + 5 + xf::text_width(st, s.substr(0, (size_t)cursor));

            XSetForeground(display_, gc_, st.text_color_);

            XDrawLine(display_, window_, gc_, cx, ey1 + 3, cx, ey2 - 3);

        }

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

public:

    std::function<void()> on_click;

    TFontDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TFontDialog";

        text_ = "Font";

        families_ = {"DejaVu Sans", "DejaVu Serif", "DejaVu Sans Mono", "Liberation Sans", "Liberation Serif", "Liberation Mono", "FreeSans", "FreeSerif", "Noto Sans", "Ubuntu"};

        styles_list_ = {"Regular", "Bold", "Italic", "Bold Italic"};

        sizes_ = {"8", "9", "10", "11", "12", "14", "16", "18", "20", "24", "28", "36", "48", "72"};

        size_text_ = sizes_[(size_t)sel_size_];

        size_cursor_ = (int)size_text_.size();

    }

    ~TFontDialog() {}

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

    std::string get_font_name()

    {

        return families_[(size_t)sel_family_];

    }

    std::string get_style_name()

    {

        return styles_list_[(size_t)sel_style_];

    }

    int get_size()

    {

        int v = 0;

        for (size_t i = 0; i < size_text_.size(); ++i)

        {

            if (size_text_[i] >= '0' && size_text_[i] <= '9')

            {

                v = v * 10 + (size_text_[i] - '0');

            }

        }

        return v > 0 ? v : 12;

    }

    bool get_strikeout()

    {

        return strikeout_;

    }

    bool get_underline()

    {

        return underline_;

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

        int fx1, fx2, sx1, sx2, zx1, zx2;

        column_rects(fx1, fx2, sx1, sx2, zx1, zx2);

        xf::draw_text_left(display_, window_, gc_, st, fx1, y1_ + 26, y1_ + 44, "Font:");

        xf::draw_text_left(display_, window_, gc_, st, sx1, y1_ + 26, y1_ + 44, "Style:");

        xf::draw_text_left(display_, window_, gc_, st, zx1, y1_ + 26, y1_ + 44, "Size:");

        draw_display_edit(fx1, fx2, families_[(size_t)sel_family_], false, 0);

        draw_display_edit(sx1, sx2, styles_list_[(size_t)sel_style_], false, 0);

        draw_display_edit(zx1, zx2, size_text_, size_edit_focus_, size_cursor_);

        draw_list(fx1, fx2, families_, sel_family_, top_family_);

        draw_list(sx1, sx2, styles_list_, sel_style_, top_style_);

        draw_list(zx1, zx2, sizes_, sel_size_, top_size_);

        //region Effects + preview

        draw_checkbox(x1_ + 12, y2_ - 92, strikeout_, "Strikeout");

        draw_checkbox(x1_ + 12, y2_ - 70, underline_, "Underline");

        int pvx1 = x1_ + 130;

        int pvy1 = y2_ - 98;

        int pvx2 = x2_ - 12;

        int pvy2 = y2_ - 44;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, pvx1, pvy1, pvx2 - pvx1, pvy2 - pvy1);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, pvx1, pvy1, pvx2 - pvx1, pvy2 - pvy1);

        xf::draw_text_left(display_, window_, gc_, st, pvx1 + 4, pvy1 - 18, pvy1, "Preview:");

        std::string sample = "AaBbYyZz 123";

        if (st.font_)

        {

            int tw = xf::text_width(st, sample);

            int tx = pvx1 + ((pvx2 - pvx1) - tw) / 2;

            int ty = pvy1 + ((pvy2 - pvy1) + st.font_->ascent - st.font_->descent) / 2;

            xf::draw_text_at(display_, window_, gc_, st, tx, ty, sample);

            XSetForeground(display_, gc_, st.text_color_);

            xf::set_line_width(display_, gc_, 1);

            if (underline_)

            {

                XDrawLine(display_, window_, gc_, tx, ty + 2, tx + tw, ty + 2);

            }

            if (strikeout_)

            {

                XDrawLine(display_, window_, gc_, tx, ty - st.font_->ascent / 3, tx + tw, ty - st.font_->ascent / 3);

            }

        }

        std::string desc = get_font_name() + ", " + get_style_name() + ", " + std::to_string(get_size()) + " pt";

        xf::draw_text_left(display_, window_, gc_, st, pvx1 + 4, pvy2 + 2, pvy2 + 18, desc);

        //endregion

        draw_mini_button(x2_ - 188, y2_ - 34, x2_ - 100, y2_ - 10, "OK", false);

        draw_mini_button(x2_ - 92, y2_ - 34, x2_ - 12, y2_ - 10, "Cancel", false);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_)

        {

            return false;

        }

        focused_ = true;

        if (button == 1 && in_rect(x, y, x2_ - 24, y1_ + 4, x2_ - 6, y1_ + 22))

        {

            result_ = mrCancel;

            return true;

        }

        int fx1, fx2, sx1, sx2, zx1, zx2;

        column_rects(fx1, fx2, sx1, sx2, zx1, zx2);

        int by1 = list_y1();

        int by2 = list_y2();

        // Wheel scrolling over whichever list is under the cursor.

        if ((button == 4 || button == 5) && in_rect(x, y, fx1, by1, zx2, by2))

        {

            int delta = (button == 4) ? -1 : 1;

            int* top = &top_family_;

            int count = (int)families_.size();

            if (x >= sx1 && x < zx1)

            {

                top = &top_style_;

                count = (int)styles_list_.size();

            }

            else if (x >= zx1)

            {

                top = &top_size_;

                count = (int)sizes_.size();

            }

            *top += delta;

            if (*top < 0)

            {

                *top = 0;

            }

            if (*top > count - list_rows())

            {

                *top = std::max(0, count - list_rows());

            }

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        if (in_rect(x, y, fx1, by1, fx2, by2))

        {

            int idx = top_family_ + (y - by1) / 17;

            if (idx >= 0 && idx < (int)families_.size())

            {

                sel_family_ = idx;

            }

            size_edit_focus_ = false;

            return true;

        }

        if (in_rect(x, y, sx1, by1, sx2, by2))

        {

            int idx = top_style_ + (y - by1) / 17;

            if (idx >= 0 && idx < (int)styles_list_.size())

            {

                sel_style_ = idx;

            }

            size_edit_focus_ = false;

            return true;

        }

        if (in_rect(x, y, zx1, by1, zx2, by2))

        {

            int idx = top_size_ + (y - by1) / 17;

            if (idx >= 0 && idx < (int)sizes_.size())

            {

                sel_size_ = idx;

                size_text_ = sizes_[(size_t)idx];

                size_cursor_ = (int)size_text_.size();

            }

            size_edit_focus_ = false;

            return true;

        }

        if (in_rect(x, y, zx1, y1_ + 46, zx2, y1_ + 68))

        {

            size_edit_focus_ = true;

            size_cursor_ = (int)size_text_.size();

            return true;

        }

        if (in_rect(x, y, x1_ + 12, y2_ - 92, x1_ + 110, y2_ - 79))

        {

            strikeout_ = !strikeout_;

            return true;

        }

        if (in_rect(x, y, x1_ + 12, y2_ - 70, x1_ + 110, y2_ - 57))

        {

            underline_ = !underline_;

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

        size_edit_focus_ = false;

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

        if (size_edit_focus_)

        {

            if (key == XK_BackSpace)

            {

                if (size_cursor_ > 0)

                {

                    size_text_.erase((size_t)size_cursor_ - 1, 1);

                    size_cursor_--;

                }

                return true;

            }

            if (str && str[0] >= '0' && str[0] <= '9' && (int)size_text_.size() < 3)

            {

                size_text_.insert((size_t)size_cursor_, 1, str[0]);

                size_cursor_++;

                return true;

            }

            if (key == XK_Left && size_cursor_ > 0)

            {

                size_cursor_--;

                return true;

            }

            if (key == XK_Right && size_cursor_ < (int)size_text_.size())

            {

                size_cursor_++;

                return true;

            }

            return false;

        }

        if (key == XK_Up || key == XK_Down)

        {

            int d = (key == XK_Up) ? -1 : 1;

            sel_size_ = std::max(0, std::min((int)sizes_.size() - 1, sel_size_ + d));

            size_text_ = sizes_[(size_t)sel_size_];

            size_cursor_ = (int)size_text_.size();

            return true;

        }

        return false;

    }

};

//endregion
