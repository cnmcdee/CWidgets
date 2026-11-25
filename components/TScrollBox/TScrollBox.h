#pragma once

#include "core/xframe_core.h"

//region TScrollBox

class TScrollBox

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

    std::string style_name_;

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    int scroll_x_ = 0;

    int scroll_y_ = 0;

    int content_width_ = 600;

    int content_height_ = 400;

    bool auto_scroll_ = true;

    int bar_size_ = 14;

    bool dragging_h_ = false;

    bool dragging_v_ = false;

    int drag_start_x_ = 0;

    int drag_start_y_ = 0;

    int drag_start_scroll_x_ = 0;

    int drag_start_scroll_y_ = 0;

    bool need_horz()

    {

        return auto_scroll_ && content_width_ > client_width();

    }

    bool need_vert()

    {

        return auto_scroll_ && content_height_ > client_height();

    }

    int client_width()

    {

        int w = width() - 2;

        // Reserve vert bar first based on full height estimate.
        if (auto_scroll_ && content_height_ > height() - 2)

        {

            w -= bar_size_;

        }

        if (w < 10) w = 10;

        return w;

    }

    int client_height()

    {

        int h = height() - 2;

        if (auto_scroll_ && content_width_ > width() - 2)

        {

            h -= bar_size_;

        }

        if (h < 10) h = 10;

        return h;

    }

    void clamp_scroll()

    {

        int max_x = content_width_ - client_width();

        int max_y = content_height_ - client_height();

        if (max_x < 0) max_x = 0;

        if (max_y < 0) max_y = 0;

        if (scroll_x_ < 0) scroll_x_ = 0;

        if (scroll_y_ < 0) scroll_y_ = 0;

        if (scroll_x_ > max_x) scroll_x_ = max_x;

        if (scroll_y_ > max_y) scroll_y_ = max_y;

    }

    void vert_thumb(int& ty, int& th)

    {

        int track_h = client_height();

        ty = y1_ + 1;

        th = track_h;

        if (content_height_ > 0)

        {

            th = track_h * client_height() / content_height_;

            if (th < 20) th = 20;

            if (th > track_h) th = track_h;

            int range = track_h - th;

            int max_y = content_height_ - client_height();

            if (max_y > 0 && range > 0)

            {

                ty = y1_ + 1 + range * scroll_y_ / max_y;

            }

        }

    }

    void horz_thumb(int& tx, int& tw)

    {

        int track_w = client_width();

        tx = x1_ + 1;

        tw = track_w;

        if (content_width_ > 0)

        {

            tw = track_w * client_width() / content_width_;

            if (tw < 20) tw = 20;

            if (tw > track_w) tw = track_w;

            int range = track_w - tw;

            int max_x = content_width_ - client_width();

            if (max_x > 0 && range > 0)

            {

                tx = x1_ + 1 + range * scroll_x_ / max_x;

            }

        }

    }

public:

    std::function<void()> on_click;

    TScrollBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TScrollBox";

        text_ = "TScrollBox";

        content_width_ = 600;

        content_height_ = 400;

        scroll_x_ = 0;

        scroll_y_ = 0;

    }

    ~TScrollBox() {}

    void set_bounds(int x, int y, int width, int height)

    {

        x1_ = x;

        y1_ = y;

        x2_ = x + width;

        y2_ = y + height;

        clamp_scroll();

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

    //region Specific API

    void set_content_size(int w, int h)

    {

        content_width_ = w;

        content_height_ = h;

        clamp_scroll();

    }

    void scroll_to(int x, int y)

    {

        scroll_x_ = x;

        scroll_y_ = y;

        clamp_scroll();

    }

    int get_scroll_x()

    {

        return scroll_x_;

    }

    int get_scroll_y()

    {

        return scroll_y_;

    }

    void set_auto_scroll(bool v)

    {

        auto_scroll_ = v;

        clamp_scroll();

    }

    bool get_auto_scroll()

    {

        return auto_scroll_;

    }

    bool horz_visible()

    {

        return need_horz();

    }

    bool vert_visible()

    {

        return need_vert();

    }

    int get_client_width()

    {

        return client_width();

    }

    int get_client_height()

    {

        return client_height();

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        clamp_scroll();

        SStyle st = style();

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        int cx1 = x1_ + 1;

        int cy1 = y1_ + 1;

        int cx2 = cx1 + client_width();

        int cy2 = cy1 + client_height();

        unsigned long white = styles_->get_color_rgb(255, 255, 255);

        XSetForeground(display_, gc_, white);

        XFillRectangle(display_, window_, gc_, cx1, cy1, client_width(), client_height());

        // Clip content drawing to the client area via simple intersection checks.

        XSetForeground(display_, gc_, styles_->get_color_rgb(210, 210, 210));

        xf::set_line_width(display_, gc_, 1);

        int start_gx = (scroll_x_ / 40) * 40;

        int start_gy = (scroll_y_ / 40) * 40;

        for (int vx = start_gx; vx <= scroll_x_ + client_width(); vx += 40)

        {

            int sx = cx1 + (vx - scroll_x_);

            if (sx >= cx1 && sx < cx2)

            {

                XDrawLine(display_, window_, gc_, sx, cy1, sx, cy2 - 1);

            }

        }

        for (int vy = start_gy; vy <= scroll_y_ + client_height(); vy += 40)

        {

            int sy = cy1 + (vy - scroll_y_);

            if (sy >= cy1 && sy < cy2)

            {

                XDrawLine(display_, window_, gc_, cx1, sy, cx2 - 1, sy);

            }

        }

        struct BoxDef { int vx; int vy; int w; int h; const char* label; };

        BoxDef boxes[3] = { {20, 20, 110, 60, "Box A"}, {180, 110, 130, 70, "Box B"}, {380, 260, 140, 80, "Box C"} };

        for (int i = 0; i < 3; ++i)

        {

            int bx = cx1 + (boxes[i].vx - scroll_x_);

            int by = cy1 + (boxes[i].vy - scroll_y_);

            int bx2 = bx + boxes[i].w;

            int by2 = by + boxes[i].h;

            if (bx2 < cx1 || bx < cx1 || by2 < cy1 || by < cy1 || bx >= cx2 || by >= cy2 || bx2 > cx2 || by2 > cy2)

            {

                // Only draw boxes fully inside the client area (simple clipping).
                if (!(bx >= cx1 && by >= cy1 && bx2 <= cx2 && by2 <= cy2))

                {

                    continue;

                }

            }

            xf::draw_polished(display_, window_, gc_, st, bx, by, bx2, by2);

            xf::draw_text_centered(display_, window_, gc_, st, bx, by, bx2, by2, boxes[i].label);

        }

        // Scrollbars.

        if (need_vert())

        {

            int tx = cx2;

            XSetForeground(display_, gc_, styles_->get_color_rgb(225, 225, 225));

            XFillRectangle(display_, window_, gc_, tx, cy1, bar_size_, client_height());

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, tx, cy1, bar_size_, client_height());

            int ty = 0;

            int th = 0;

            vert_thumb(ty, th);

            XSetForeground(display_, gc_, st.fillColor_);

            XFillRectangle(display_, window_, gc_, tx + 2, ty + 1, bar_size_ - 4, th - 2);

            XSetForeground(display_, gc_, st.outlineColor_);

            XDrawRectangle(display_, window_, gc_, tx + 2, ty + 1, bar_size_ - 4, th - 2);

        }

        if (need_horz())

        {

            int ty = cy2;

            XSetForeground(display_, gc_, styles_->get_color_rgb(225, 225, 225));

            XFillRectangle(display_, window_, gc_, cx1, ty, client_width(), bar_size_);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, cx1, ty, client_width(), bar_size_);

            int tx = 0;

            int tw = 0;

            horz_thumb(tx, tw);

            XSetForeground(display_, gc_, st.fillColor_);

            XFillRectangle(display_, window_, gc_, tx + 1, ty + 2, tw - 2, bar_size_ - 4);

            XSetForeground(display_, gc_, st.outlineColor_);

            XDrawRectangle(display_, window_, gc_, tx + 1, ty + 2, tw - 2, bar_size_ - 4);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_)

        {

            return false;

        }

        focused_ = true;

        if (button == 4)

        {

            scroll_y_ -= 16;

            clamp_scroll();

            return true;

        }

        if (button == 5)

        {

            scroll_y_ += 16;

            clamp_scroll();

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        int cx1 = x1_ + 1;

        int cy1 = y1_ + 1;

        int cx2 = cx1 + client_width();

        int cy2 = cy1 + client_height();

        if (need_vert() && x >= cx2 && x < cx2 + bar_size_ && y >= cy1 && y < cy2)

        {

            int ty = 0;

            int th = 0;

            vert_thumb(ty, th);

            if (y >= ty && y < ty + th)

            {

                dragging_v_ = true;

                drag_start_y_ = y;

                drag_start_scroll_y_ = scroll_y_;

            }

            else if (y < ty)

            {

                scroll_y_ -= client_height();

                clamp_scroll();

            }

            else

            {

                scroll_y_ += client_height();

                clamp_scroll();

            }

            return true;

        }

        if (need_horz() && y >= cy2 && y < cy2 + bar_size_ && x >= cx1 && x < cx2)

        {

            int tx = 0;

            int tw = 0;

            horz_thumb(tx, tw);

            if (x >= tx && x < tx + tw)

            {

                dragging_h_ = true;

                drag_start_x_ = x;

                drag_start_scroll_x_ = scroll_x_;

            }

            else if (x < tx)

            {

                scroll_x_ -= client_width();

                clamp_scroll();

            }

            else

            {

                scroll_x_ += client_width();

                clamp_scroll();

            }

            return true;

        }

        if (on_click)

        {

            on_click();

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button != 1)

        {

            return false;

        }

        if (dragging_h_ || dragging_v_)

        {

            dragging_h_ = false;

            dragging_v_ = false;

            return true;

        }

        return false;

    }

    bool on_motion(int x, int y)

    {

        if (dragging_v_)

        {

            int ty = 0;

            int th = 0;

            vert_thumb(ty, th);

            int track_h = client_height();

            int range = track_h - th;

            int max_y = content_height_ - client_height();

            if (range > 0 && max_y > 0)

            {

                scroll_y_ = drag_start_scroll_y_ + (y - drag_start_y_) * max_y / range;

                clamp_scroll();

            }

            return true;

        }

        if (dragging_h_)

        {

            int tx = 0;

            int tw = 0;

            horz_thumb(tx, tw);

            int track_w = client_width();

            int range = track_w - tw;

            int max_x = content_width_ - client_width();

            if (range > 0 && max_x > 0)

            {

                scroll_x_ = drag_start_scroll_x_ + (x - drag_start_x_) * max_x / range;

                clamp_scroll();

            }

            return true;

        }

        return false;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Left) { scroll_x_ -= 16; clamp_scroll(); return true; }

        if (key == XK_Right) { scroll_x_ += 16; clamp_scroll(); return true; }

        if (key == XK_Up) { scroll_y_ -= 16; clamp_scroll(); return true; }

        if (key == XK_Down) { scroll_y_ += 16; clamp_scroll(); return true; }

        if (key == XK_Page_Up) { scroll_y_ -= client_height(); clamp_scroll(); return true; }

        if (key == XK_Page_Down) { scroll_y_ += client_height(); clamp_scroll(); return true; }

        if (key == XK_Home) { scroll_x_ = 0; scroll_y_ = 0; return true; }

        if (key == XK_End) { scroll_x_ = content_width_; scroll_y_ = content_height_; clamp_scroll(); return true; }

        return false;

    }

};

//endregion
