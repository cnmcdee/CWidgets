#pragma once

#include "core/xframe_core.h"

//region TPageSetupDialog

class TPageSetupDialog

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

    struct TPaper

    {

        std::string name;

        int w_mm;

        int h_mm;

    };

    std::vector<TPaper> papers_;

    int sel_paper_ = 0;

    int orientation_ = 0;   // 0 = Portrait, 1 = Landscape

    // Margins in millimetres

    int margin_left_ = 20;

    int margin_right_ = 20;

    int margin_top_ = 25;

    int margin_bottom_ = 25;

    int dragging_ = 0;   // 0 none, 1 left, 2 right, 3 top, 4 bottom

    int result_ = 0;

    bool in_rect(int px, int py, int rx1, int ry1, int rx2, int ry2)

    {

        return px >= rx1 && px <= rx2 && py >= ry1 && py <= ry2;

    }

    int paper_w_mm()

    {

        const TPaper& p = papers_[(size_t)sel_paper_];

        return orientation_ == 0 ? p.w_mm : p.h_mm;

    }

    int paper_h_mm()

    {

        const TPaper& p = papers_[(size_t)sel_paper_];

        return orientation_ == 0 ? p.h_mm : p.w_mm;

    }

    // Preview geometry: the paper rect and the px-per-mm scale.

    void paper_rect(int& rx1, int& ry1, int& rx2, int& ry2, double& scale)

    {

        int area_x1 = x1_ + 12;

        int area_x2 = x1_ + width() * 56 / 100;

        int area_y1 = y1_ + 40;

        int area_y2 = y2_ - 50;

        int avail_w = area_x2 - area_x1;

        int avail_h = area_y2 - area_y1;

        double sx = avail_w / (double)paper_w_mm();

        double sy = avail_h / (double)paper_h_mm();

        scale = std::min(sx, sy);

        int pw = (int)(paper_w_mm() * scale);

        int ph = (int)(paper_h_mm() * scale);

        rx1 = area_x1 + (avail_w - pw) / 2;

        ry1 = area_y1 + (avail_h - ph) / 2;

        rx2 = rx1 + pw;

        ry2 = ry1 + ph;

    }

    int clamp_margin(int v, int limit)

    {

        if (v < 5)

        {

            v = 5;

        }

        if (v > limit)

        {

            v = limit;

        }

        return v;

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

    TPageSetupDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TPageSetupDialog";

        text_ = "Page Setup";

        papers_ = {{"A4", 210, 297}, {"Letter", 216, 279}, {"Legal", 216, 356}, {"A5", 148, 210}};

    }

    ~TPageSetupDialog() {}

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

    void get_margins(int& left, int& top, int& right, int& bottom)

    {

        left = margin_left_;

        top = margin_top_;

        right = margin_right_;

        bottom = margin_bottom_;

    }

    int get_margin_left()

    {

        return margin_left_;

    }

    int get_margin_top()

    {

        return margin_top_;

    }

    int get_margin_right()

    {

        return margin_right_;

    }

    int get_margin_bottom()

    {

        return margin_bottom_;

    }

    void set_margins(int left, int top, int right, int bottom)

    {

        margin_left_ = left;

        margin_top_ = top;

        margin_right_ = right;

        margin_bottom_ = bottom;

    }

    std::string get_paper_size()

    {

        return papers_[(size_t)sel_paper_].name;

    }

    std::string get_orientation()

    {

        return orientation_ == 0 ? "Portrait" : "Landscape";

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

        //region Paper preview with draggable margin guides

        int rx1, ry1, rx2, ry2;

        double scale;

        paper_rect(rx1, ry1, rx2, ry2, scale);

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, rx1, ry1, rx2 - rx1, ry2 - ry1);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, rx1, ry1, rx2 - rx1, ry2 - ry1);

        int gl = rx1 + (int)(margin_left_ * scale);

        int gr = rx2 - (int)(margin_right_ * scale);

        int gt = ry1 + (int)(margin_top_ * scale);

        int gb = ry2 - (int)(margin_bottom_ * scale);

        // Text area shading inside the margins

        XSetForeground(display_, gc_, styles_->get_color_rgb(240, 244, 250));

        XFillRectangle(display_, window_, gc_, gl, gt, gr - gl, gb - gt);

        // Fake text lines inside the text area

        XSetForeground(display_, gc_, styles_->get_color_rgb(170, 175, 185));

        for (int ly = gt + 8; ly < gb - 4; ly += 7)

        {

            XDrawLine(display_, window_, gc_, gl + 6, ly, gr - 6 - ((ly * 7) % 23), ly);

        }

        // Margin guides in blue, thicker while one is being dragged

        XSetForeground(display_, gc_, styles_->get_color_rgb(60, 110, 220));

        xf::set_line_width(display_, gc_, dragging_ == 1 ? 2 : 1);

        XDrawLine(display_, window_, gc_, gl, ry1, gl, ry2);

        xf::set_line_width(display_, gc_, dragging_ == 2 ? 2 : 1);

        XDrawLine(display_, window_, gc_, gr, ry1, gr, ry2);

        xf::set_line_width(display_, gc_, dragging_ == 3 ? 2 : 1);

        XDrawLine(display_, window_, gc_, rx1, gt, rx2, gt);

        xf::set_line_width(display_, gc_, dragging_ == 4 ? 2 : 1);

        XDrawLine(display_, window_, gc_, rx1, gb, rx2, gb);

        xf::set_line_width(display_, gc_, 1);

        xf::draw_text_centered(display_, window_, gc_, st, rx1, ry2 + 4, rx2, ry2 + 22, get_paper_size() + "  " + std::to_string(paper_w_mm()) + " x " + std::to_string(paper_h_mm()) + " mm");

        //endregion

        //region Controls on the right

        int cx = x1_ + width() * 56 / 100 + 14;

        xf::draw_text_left(display_, window_, gc_, st, cx, y1_ + 34, y1_ + 54, "Paper size:");

        for (size_t i = 0; i < papers_.size(); ++i)

        {

            int ry = y1_ + 58 + (int)i * 22;

            if ((int)i == sel_paper_)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, cx - 4, ry - 2, 110, 20);

                SStyle rst = st;

                rst.text_color_ = styles_->get_color_rgb(255, 255, 255);

                xf::draw_text_left(display_, window_, gc_, rst, cx, ry, ry + 18, papers_[i].name);

            }

            else

            {

                xf::draw_text_left(display_, window_, gc_, st, cx, ry, ry + 18, papers_[i].name);

            }

        }

        xf::draw_text_left(display_, window_, gc_, st, cx, y1_ + 156, y1_ + 176, "Orientation:");

        draw_radio(cx, y1_ + 180, orientation_ == 0, "Portrait");

        draw_radio(cx, y1_ + 202, orientation_ == 1, "Landscape");

        xf::draw_text_left(display_, window_, gc_, st, cx, y1_ + 232, y1_ + 252, "Margins (mm): drag the");

        xf::draw_text_left(display_, window_, gc_, st, cx, y1_ + 250, y1_ + 270, "blue guides on the paper.");

        xf::draw_text_left(display_, window_, gc_, st, cx, y1_ + 276, y1_ + 296, "Left:   " + std::to_string(margin_left_));

        xf::draw_text_left(display_, window_, gc_, st, cx, y1_ + 294, y1_ + 314, "Right:  " + std::to_string(margin_right_));

        xf::draw_text_left(display_, window_, gc_, st, cx, y1_ + 312, y1_ + 332, "Top:    " + std::to_string(margin_top_));

        xf::draw_text_left(display_, window_, gc_, st, cx, y1_ + 330, y1_ + 350, "Bottom: " + std::to_string(margin_bottom_));

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

        // Margin guide grabbing

        int rx1, ry1, rx2, ry2;

        double scale;

        paper_rect(rx1, ry1, rx2, ry2, scale);

        int gl = rx1 + (int)(margin_left_ * scale);

        int gr = rx2 - (int)(margin_right_ * scale);

        int gt = ry1 + (int)(margin_top_ * scale);

        int gb = ry2 - (int)(margin_bottom_ * scale);

        if (in_rect(x, y, rx1, ry1, rx2, ry2))

        {

            if (std::abs(x - gl) <= 4)

            {

                dragging_ = 1;

            }

            else if (std::abs(x - gr) <= 4)

            {

                dragging_ = 2;

            }

            else if (std::abs(y - gt) <= 4)

            {

                dragging_ = 3;

            }

            else if (std::abs(y - gb) <= 4)

            {

                dragging_ = 4;

            }

            return true;

        }

        int cx = x1_ + width() * 56 / 100 + 14;

        if (in_rect(x, y, cx - 4, y1_ + 56, cx + 120, y1_ + 58 + (int)papers_.size() * 22))

        {

            int idx = (y - (y1_ + 58)) / 22;

            if (idx >= 0 && idx < (int)papers_.size())

            {

                sel_paper_ = idx;

            }

            return true;

        }

        if (in_rect(x, y, cx, y1_ + 178, cx + 110, y1_ + 196))

        {

            orientation_ = 0;

            return true;

        }

        if (in_rect(x, y, cx, y1_ + 200, cx + 120, y1_ + 218))

        {

            orientation_ = 1;

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

        if (button == 1 && dragging_ != 0)

        {

            dragging_ = 0;

            return true;

        }

        return false;

    }

    bool on_motion(int x, int y)

    {

        if (dragging_ == 0)

        {

            return false;

        }

        int rx1, ry1, rx2, ry2;

        double scale;

        paper_rect(rx1, ry1, rx2, ry2, scale);

        if (scale <= 0.0)

        {

            return false;

        }

        if (dragging_ == 1)

        {

            margin_left_ = clamp_margin((int)((x - rx1) / scale + 0.5), paper_w_mm() - margin_right_ - 10);

        }

        else if (dragging_ == 2)

        {

            margin_right_ = clamp_margin((int)((rx2 - x) / scale + 0.5), paper_w_mm() - margin_left_ - 10);

        }

        else if (dragging_ == 3)

        {

            margin_top_ = clamp_margin((int)((y - ry1) / scale + 0.5), paper_h_mm() - margin_bottom_ - 10);

        }

        else if (dragging_ == 4)

        {

            margin_bottom_ = clamp_margin((int)((ry2 - y) / scale + 0.5), paper_h_mm() - margin_top_ - 10);

        }

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

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

        // Arrow keys nudge the left / top margins for keyboard users.

        if (key == XK_Left)

        {

            margin_left_ = clamp_margin(margin_left_ - 1, paper_w_mm() - margin_right_ - 10);

            return true;

        }

        if (key == XK_Right)

        {

            margin_left_ = clamp_margin(margin_left_ + 1, paper_w_mm() - margin_right_ - 10);

            return true;

        }

        if (key == XK_Up)

        {

            margin_top_ = clamp_margin(margin_top_ - 1, paper_h_mm() - margin_bottom_ - 10);

            return true;

        }

        if (key == XK_Down)

        {

            margin_top_ = clamp_margin(margin_top_ + 1, paper_h_mm() - margin_bottom_ - 10);

            return true;

        }

        return false;

    }

};

//endregion
