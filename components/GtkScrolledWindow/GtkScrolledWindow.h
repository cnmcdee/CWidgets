#pragma once

#include "core/xframe_core.h"

#include <cstdio>

//region GtkScrolledWindow

class GtkScrolledWindow

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

    // Policy values: 0 = always, 1 = automatic, 2 = never.

    int hpolicy_ = 1;

    int vpolicy_ = 1;

    int content_width_ = 1200;

    int content_height_ = 900;

    int hscroll_ = 0;

    int vscroll_ = 0;

    int bar_ = 14;   // scrollbar thickness

    int dragging_ = 0;   // 0 none, 1 vertical thumb, 2 horizontal thumb

    int grab_offset_ = 0;

    bool vbar_shown()

    {

        if (vpolicy_ == 2)

        {

            return false;

        }

        if (vpolicy_ == 0)

        {

            return true;

        }

        return content_height_ > height() - (hbar_shown_raw() ? bar_ : 0);

    }

    bool hbar_shown_raw()

    {

        if (hpolicy_ == 2)

        {

            return false;

        }

        if (hpolicy_ == 0)

        {

            return true;

        }

        return content_width_ > width();

    }

    bool hbar_shown()

    {

        if (hpolicy_ == 2)

        {

            return false;

        }

        if (hpolicy_ == 0)

        {

            return true;

        }

        return content_width_ > width() - (vpolicy_ != 2 ? bar_ : 0);

    }

    int view_w()

    {

        int w = width() - (vbar_shown() ? bar_ : 0);

        return w < 1 ? 1 : w;

    }

    int view_h()

    {

        int h = height() - (hbar_shown() ? bar_ : 0);

        return h < 1 ? 1 : h;

    }

    int max_hscroll()

    {

        int m = content_width_ - view_w();

        return m < 0 ? 0 : m;

    }

    int max_vscroll()

    {

        int m = content_height_ - view_h();

        return m < 0 ? 0 : m;

    }

    void clamp_scroll()

    {

        if (hscroll_ < 0)

        {

            hscroll_ = 0;

        }

        if (hscroll_ > max_hscroll())

        {

            hscroll_ = max_hscroll();

        }

        if (vscroll_ < 0)

        {

            vscroll_ = 0;

        }

        if (vscroll_ > max_vscroll())

        {

            vscroll_ = max_vscroll();

        }

    }

    // Vertical thumb geometry inside its track (the viewport band).

    void vthumb(int& ty1, int& ty2)

    {

        int track_h = view_h();

        int th = track_h * view_h() / (content_height_ > 0 ? content_height_ : 1);

        if (th < 16)

        {

            th = 16;

        }

        if (th > track_h)

        {

            th = track_h;

        }

        int maxs = max_vscroll();

        int off = maxs > 0 ? vscroll_ * (track_h - th) / maxs : 0;

        ty1 = y1_ + off;

        ty2 = ty1 + th;

    }

    void hthumb(int& tx1, int& tx2)

    {

        int track_w = view_w();

        int tw = track_w * view_w() / (content_width_ > 0 ? content_width_ : 1);

        if (tw < 16)

        {

            tw = 16;

        }

        if (tw > track_w)

        {

            tw = track_w;

        }

        int maxs = max_hscroll();

        int off = maxs > 0 ? hscroll_ * (track_w - tw) / maxs : 0;

        tx1 = x1_ + off;

        tx2 = tx1 + tw;

    }

public:

    GtkScrolledWindow(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkScrolledWindow";

        text_ = "Scrolled Window";

    }

    ~GtkScrolledWindow() {}

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

    void set_hpolicy(int p)

    {

        hpolicy_ = p;

        clamp_scroll();

    }

    void set_vpolicy(int p)

    {

        vpolicy_ = p;

        clamp_scroll();

    }

    int get_hpolicy()

    {

        return hpolicy_;

    }

    int get_vpolicy()

    {

        return vpolicy_;

    }

    void set_content_size(int w, int h)

    {

        content_width_ = w;

        content_height_ = h;

        clamp_scroll();

    }

    int get_hscroll()

    {

        return hscroll_;

    }

    int get_vscroll()

    {

        return vscroll_;

    }

    void set_hscroll(int v)

    {

        hscroll_ = v;

        clamp_scroll();

    }

    void set_vscroll(int v)

    {

        vscroll_ = v;

        clamp_scroll();

    }

    // The visible content area, shrunk by any visible scrollbars.

    void viewport_rect(int& vx1, int& vy1, int& vx2, int& vy2)

    {

        vx1 = x1_;

        vy1 = y1_;

        vx2 = x1_ + view_w();

        vy2 = y1_ + view_h();

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        clamp_scroll();

        int vx1, vy1, vx2, vy2;

        viewport_rect(vx1, vy1, vx2, vy2);

        // Viewport background.

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, vx1, vy1, vx2 - vx1, vy2 - vy1);

        // Sample content: a grid offset by the scroll position,
        // clipped by hand to the viewport band.

        XSetForeground(display_, gc_, styles_->get_color_rgb(210, 210, 210));

        xf::set_line_width(display_, gc_, 1);

        int cell = 40;

        int gx0 = vx1 - (hscroll_ % cell);

        for (int gx = gx0; gx <= vx2; gx += cell)

        {

            XDrawLine(display_, window_, gc_, gx, vy1, gx, vy2);

        }

        int gy0 = vy1 - (vscroll_ % cell);

        for (int gy = gy0; gy <= vy2; gy += cell)

        {

            XDrawLine(display_, window_, gc_, vx1, gy, vx2, gy);

        }

        char buf[64];

        snprintf(buf, sizeof(buf), "content %d x %d @ %d,%d", content_width_, content_height_, hscroll_, vscroll_);

        xf::draw_text_at(display_, window_, gc_, st, vx1 + 8, vy1 + 18, std::string(buf));

        // Viewport frame.

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, vx1, vy1, vx2 - vx1, vy2 - vy1);

        // Vertical scrollbar: track on the right, thumb inside it.

        if (vbar_shown())

        {

            int tx = vx2;

            XSetForeground(display_, gc_, styles_->get_color_rgb(235, 235, 235));

            XFillRectangle(display_, window_, gc_, tx, vy1, bar_, vy2 - vy1);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, tx, vy1, bar_, vy2 - vy1);

            int ty1, ty2;

            vthumb(ty1, ty2);

            XSetForeground(display_, gc_, styles_->get_color_rgb(170, 170, 170));

            XFillRectangle(display_, window_, gc_, tx + 2, ty1, bar_ - 4, ty2 - ty1);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, tx + 2, ty1, bar_ - 4, ty2 - ty1);

        }

        // Horizontal scrollbar: track on the bottom, thumb inside it.

        if (hbar_shown())

        {

            int ty = vy2;

            XSetForeground(display_, gc_, styles_->get_color_rgb(235, 235, 235));

            XFillRectangle(display_, window_, gc_, vx1, ty, vx2 - vx1, bar_);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, vx1, ty, vx2 - vx1, bar_);

            int tx1, tx2;

            hthumb(tx1, tx2);

            XSetForeground(display_, gc_, styles_->get_color_rgb(170, 170, 170));

            XFillRectangle(display_, window_, gc_, tx1, ty + 2, tx2 - tx1, bar_ - 4);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, tx1, ty + 2, tx2 - tx1, bar_ - 4);

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

        // Mouse wheel: 4/5 vertical, 6/7 horizontal.

        if (button == 4)

        {

            vscroll_ -= 24;

            clamp_scroll();

            return true;

        }

        if (button == 5)

        {

            vscroll_ += 24;

            clamp_scroll();

            return true;

        }

        if (button == 6)

        {

            hscroll_ -= 24;

            clamp_scroll();

            return true;

        }

        if (button == 7)

        {

            hscroll_ += 24;

            clamp_scroll();

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        int vx1, vy1, vx2, vy2;

        viewport_rect(vx1, vy1, vx2, vy2);

        // Vertical thumb hit starts a vertical drag.

        if (vbar_shown() && x >= vx2 && x <= vx2 + bar_ && y >= vy1 && y <= vy2)

        {

            int ty1, ty2;

            vthumb(ty1, ty2);

            if (y >= ty1 && y <= ty2)

            {

                dragging_ = 1;

                grab_offset_ = y - ty1;

            }

            else if (y < ty1)

            {

                vscroll_ -= view_h();

                clamp_scroll();

            }

            else

            {

                vscroll_ += view_h();

                clamp_scroll();

            }

            return true;

        }

        // Horizontal thumb hit starts a horizontal drag.

        if (hbar_shown() && y >= vy2 && y <= vy2 + bar_ && x >= vx1 && x <= vx2)

        {

            int tx1, tx2;

            hthumb(tx1, tx2);

            if (x >= tx1 && x <= tx2)

            {

                dragging_ = 2;

                grab_offset_ = x - tx1;

            }

            else if (x < tx1)

            {

                hscroll_ -= view_w();

                clamp_scroll();

            }

            else

            {

                hscroll_ += view_w();

                clamp_scroll();

            }

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

        if (dragging_ == 1)

        {

            int ty1, ty2;

            vthumb(ty1, ty2);

            int thumb_h = ty2 - ty1;

            int track_h = view_h();

            int maxs = max_vscroll();

            int range = track_h - thumb_h;

            if (range > 0 && maxs > 0)

            {

                int new_top = y - grab_offset_ - y1_;

                vscroll_ = new_top * maxs / range;

                clamp_scroll();

            }

            return true;

        }

        if (dragging_ == 2)

        {

            int tx1, tx2;

            hthumb(tx1, tx2);

            int thumb_w = tx2 - tx1;

            int track_w = view_w();

            int maxs = max_hscroll();

            int range = track_w - thumb_w;

            if (range > 0 && maxs > 0)

            {

                int new_left = x - grab_offset_ - x1_;

                hscroll_ = new_left * maxs / range;

                clamp_scroll();

            }

            return true;

        }

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

        if (key == XK_Down)

        {

            vscroll_ += 24;

            clamp_scroll();

            return true;

        }

        if (key == XK_Up)

        {

            vscroll_ -= 24;

            clamp_scroll();

            return true;

        }

        if (key == XK_Right)

        {

            hscroll_ += 24;

            clamp_scroll();

            return true;

        }

        if (key == XK_Left)

        {

            hscroll_ -= 24;

            clamp_scroll();

            return true;

        }

        if (key == XK_Page_Down)

        {

            vscroll_ += view_h();

            clamp_scroll();

            return true;

        }

        if (key == XK_Page_Up)

        {

            vscroll_ -= view_h();

            clamp_scroll();

            return true;

        }

        return false;

    }

};

//endregion
