#pragma once

#include "core/xframe_core.h"

//region TColorDialog

class TColorDialog

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

    std::vector<Srgb> colors_;          // the 48 standard colours

    std::vector<Srgb> custom_colors_;   // 16 custom slots

    int sel_index_ = 9;                 // index into colors_, -1 = custom

    Srgb selected_color_;

    std::string r_text_;

    std::string g_text_;

    std::string b_text_;

    int r_cursor_ = 0;

    int g_cursor_ = 0;

    int b_cursor_ = 0;

    int edit_focus_ = -1;               // 0 = R, 1 = G, 2 = B, -1 = none

    int result_ = 0;

    bool in_rect(int px, int py, int rx1, int ry1, int rx2, int ry2)

    {

        return px >= rx1 && px <= rx2 && py >= ry1 && py <= ry2;

    }

    void sync_rgb_text()

    {

        r_text_ = std::to_string(selected_color_.r);

        g_text_ = std::to_string(selected_color_.g);

        b_text_ = std::to_string(selected_color_.b);

        r_cursor_ = (int)r_text_.size();

        g_cursor_ = (int)g_text_.size();

        b_cursor_ = (int)b_text_.size();

    }

    void apply_rgb_text()

    {

        auto clamp_parse = [](const std::string& s) {

            int v = 0;

            for (size_t i = 0; i < s.size(); ++i)

            {

                if (s[i] >= '0' && s[i] <= '9')

                {

                    v = v * 10 + (s[i] - '0');

                }

            }

            if (v > 255)

            {

                v = 255;

            }

            return v;

        };

        selected_color_.r = clamp_parse(r_text_);

        selected_color_.g = clamp_parse(g_text_);

        selected_color_.b = clamp_parse(b_text_);

        sel_index_ = -1;

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

    bool edit_key_digits(std::string& buf, int& cur, KeySym key, const char* str)

    {

        if (key == XK_BackSpace)

        {

            if (cur > 0)

            {

                buf.erase((size_t)cur - 1, 1);

                cur--;

            }

            apply_rgb_text();

            return true;

        }

        if (key == XK_Delete)

        {

            if (cur < (int)buf.size())

            {

                buf.erase((size_t)cur, 1);

            }

            apply_rgb_text();

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

        if (str && str[0] >= '0' && str[0] <= '9' && (int)buf.size() < 3)

        {

            buf.insert((size_t)cur, 1, str[0]);

            cur++;

            apply_rgb_text();

            return true;

        }

        return false;

    }

public:

    std::function<void()> on_click;

    TColorDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TColorDialog";

        text_ = "Select a Color";

        colors_ = {

            {0, 0, 0},       {128, 0, 0},   {0, 128, 0},   {128, 128, 0}, {0, 0, 128},   {128, 0, 128}, {0, 128, 128}, {192, 192, 192},

            {128, 128, 128}, {255, 0, 0},   {0, 255, 0},   {255, 255, 0}, {0, 0, 255},   {255, 0, 255}, {0, 255, 255}, {255, 255, 255},

            {255, 128, 0},   {255, 165, 0}, {255, 215, 0}, {255, 255, 153}, {204, 255, 153}, {153, 255, 204}, {153, 204, 255}, {204, 153, 255},

            {255, 153, 204}, {165, 42, 42}, {210, 105, 30}, {244, 164, 96}, {189, 183, 107}, {107, 142, 35}, {32, 178, 170}, {70, 130, 180},

            {100, 149, 237}, {147, 112, 219}, {219, 112, 147}, {240, 128, 128}, {144, 238, 144}, {135, 206, 250}, {238, 232, 170}, {221, 160, 221},

            {64, 64, 64},    {96, 96, 96},  {160, 160, 160}, {208, 208, 208}, {232, 232, 232}, {245, 245, 245}, {30, 30, 30}, {255, 250, 240},

        };

        custom_colors_.assign(16, Srgb{255, 255, 255});

        selected_color_ = colors_[9];

        sync_rgb_text();

    }

    ~TColorDialog() {}

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

    Srgb get_color()

    {

        return selected_color_;

    }

    void set_color(Srgb c)

    {

        selected_color_ = c;

        sel_index_ = -1;

        sync_rgb_text();

    }

    unsigned long get_color_pixel()

    {

        return styles_->get_color_Srgb(selected_color_);

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

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y1_ + 28, y1_ + 48, "Basic colors:");

        //region Palette grid, 8 x 6

        int gx = x1_ + 12;

        int gy = y1_ + 50;

        int cell_w = 26;

        int cell_h = 22;

        int gap = 4;

        for (int i = 0; i < 48; ++i)

        {

            int cx = gx + (i % 8) * (cell_w + gap);

            int cy = gy + (i / 8) * (cell_h + gap);

            XSetForeground(display_, gc_, styles_->get_color_Srgb(colors_[(size_t)i]));

            XFillRectangle(display_, window_, gc_, cx, cy, cell_w, cell_h);

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, 1);

            XDrawRectangle(display_, window_, gc_, cx, cy, cell_w, cell_h);

            if (i == sel_index_)

            {

                XSetForeground(display_, gc_, styles_->get_color_rgb(0, 0, 0));

                xf::set_line_width(display_, gc_, 2);

                XDrawRectangle(display_, window_, gc_, cx - 2, cy - 2, cell_w + 4, cell_h + 4);

                xf::set_line_width(display_, gc_, 1);

            }

        }

        //endregion

        //region Custom colours row

        int cust_y = gy + 6 * (cell_h + gap) + 8;

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, cust_y, cust_y + 18, "Custom colors:");

        for (int i = 0; i < 16; ++i)

        {

            int cx = gx + i * (cell_w + gap);

            int cy = cust_y + 20;

            XSetForeground(display_, gc_, styles_->get_color_Srgb(custom_colors_[(size_t)i]));

            XFillRectangle(display_, window_, gc_, cx, cy, cell_w, cell_h);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, cx, cy, cell_w, cell_h);

        }

        //endregion

        //region Preview + RGB fields on the right

        int px = x1_ + 268;

        xf::draw_text_left(display_, window_, gc_, st, px, y1_ + 28, y1_ + 48, "Color:");

        XSetForeground(display_, gc_, styles_->get_color_Srgb(selected_color_));

        XFillRectangle(display_, window_, gc_, px, y1_ + 50, 76, 52);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, px, y1_ + 50, 76, 52);

        const char* labels[3] = {"Red:", "Green:", "Blue:"};

        const std::string* texts[3] = {&r_text_, &g_text_, &b_text_};

        const int* curs[3] = {&r_cursor_, &g_cursor_, &b_cursor_};

        for (int i = 0; i < 3; ++i)

        {

            int ey = y1_ + 116 + i * 30;

            xf::draw_text_left(display_, window_, gc_, st, px, ey, ey + 22, labels[i]);

            draw_edit(px + 48, ey, px + 96, ey + 22, *texts[i], *curs[i], edit_focus_ == i);

        }

        char hexbuf[16];

        snprintf(hexbuf, sizeof(hexbuf), "#%02X%02X%02X", selected_color_.r, selected_color_.g, selected_color_.b);

        xf::draw_text_left(display_, window_, gc_, st, px, y1_ + 212, y1_ + 234, hexbuf);

        //endregion

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

        int gx = x1_ + 12;

        int gy = y1_ + 50;

        int cell_w = 26;

        int cell_h = 22;

        int gap = 4;

        if (in_rect(x, y, gx, gy, gx + 8 * (cell_w + gap), gy + 6 * (cell_h + gap)))

        {

            int col = (x - gx) / (cell_w + gap);

            int row = (y - gy) / (cell_h + gap);

            if (col >= 0 && col < 8 && row >= 0 && row < 6)

            {

                if ((x - gx) % (cell_w + gap) < cell_w && (y - gy) % (cell_h + gap) < cell_h)

                {

                    sel_index_ = row * 8 + col;

                    selected_color_ = colors_[(size_t)sel_index_];

                    sync_rgb_text();

                    edit_focus_ = -1;

                }

            }

            return true;

        }

        int cust_y = gy + 6 * (cell_h + gap) + 8;

        if (in_rect(x, y, gx, cust_y + 20, gx + 16 * (cell_w + gap), cust_y + 20 + cell_h))

        {

            int col = (x - gx) / (cell_w + gap);

            if (col >= 0 && col < 16 && (x - gx) % (cell_w + gap) < cell_w)

            {

                // Store the current colour into the slot and select it.

                custom_colors_[(size_t)col] = selected_color_;

                sel_index_ = -1;

            }

            return true;

        }

        int px = x1_ + 268;

        for (int i = 0; i < 3; ++i)

        {

            int ey = y1_ + 116 + i * 30;

            if (in_rect(x, y, px + 48, ey, px + 96, ey + 22))

            {

                edit_focus_ = i;

                return true;

            }

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

        edit_focus_ = -1;

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

        if (key == XK_Tab)

        {

            edit_focus_ = (edit_focus_ + 1) % 3;

            return true;

        }

        if (edit_focus_ == 0)

        {

            return edit_key_digits(r_text_, r_cursor_, key, str);

        }

        if (edit_focus_ == 1)

        {

            return edit_key_digits(g_text_, g_cursor_, key, str);

        }

        if (edit_focus_ == 2)

        {

            return edit_key_digits(b_text_, b_cursor_, key, str);

        }

        return false;

    }

};

//endregion
