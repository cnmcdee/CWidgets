#pragma once

#include "core/xframe_core.h"

//region GtkWindow

class GtkWindow

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;          // the window title

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    // Window-manager state, kept by the widget itself because in
    // XFrame the "window" is drawn inside a host window.

    bool closed_ = false;

    bool iconified_ = false;

    bool maximized_ = false;

    bool resizable_ = true;

    bool decorated_ = true;

    // Title-bar button the mouse is currently over: 0 none,
    // 1 minimise, 2 maximise, 3 close.

    int hover_button_ = 0;

    // Title-bar dragging: while drag_ is set, on_motion moves the
    // whole widget by the mouse delta.

    bool drag_ = false;

    int drag_dx_ = 0;

    int drag_dy_ = 0;

    // Bounds saved when maximising, restored on un-maximise.

    int saved_x1_ = 0;

    int saved_y1_ = 0;

    int saved_x2_ = 0;

    int saved_y2_ = 0;

    int title_bar_height()

    {

        return 28;

    }

    // Title-bar button rectangles: three 26x20 buttons at the
    // right of the title bar. which: 1 min, 2 max, 3 close.

    void button_rect(int which, int &bx1, int &by1, int &bx2, int &by2)

    {

        by1 = y1_ + 4;

        by2 = y1_ + title_bar_height() - 4;

        bx2 = x2_ - 6 - (3 - which) * 30;

        bx1 = bx2 - 26;

    }

    bool in_rect(int x, int y, int rx1, int ry1, int rx2, int ry2)

    {

        return x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2;

    }

public:

    std::function<void()> on_close;

    std::function<void()> on_minimize;

    std::function<void()> on_maximize;

    GtkWindow(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkWindow";

        text_ = "Window";

    }

    ~GtkWindow() {}

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

        return visible_ && !closed_ && !iconified_ && x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_;

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

    //region GtkWindow specific API

    void set_title(std::string t)

    {

        text_ = t;

    }

    std::string get_title()

    {

        return text_;

    }

    void set_resizable(bool r)

    {

        resizable_ = r;

    }

    bool get_resizable()

    {

        return resizable_;

    }

    void set_decorated(bool d)

    {

        decorated_ = d;

    }

    bool get_decorated()

    {

        return decorated_;

    }

    bool is_closed()

    {

        return closed_;

    }

    bool is_iconified()

    {

        return iconified_;

    }

    bool is_maximized()

    {

        return maximized_;

    }

    void close()

    {

        closed_ = true;

        if (on_close)

        {

            on_close();

        }

    }

    void iconify()

    {

        iconified_ = true;

        if (on_minimize)

        {

            on_minimize();

        }

    }

    void deiconify()

    {

        iconified_ = false;

    }

    void maximize()

    {

        if (maximized_ || !resizable_)

        {

            return;

        }

        saved_x1_ = x1_;

        saved_y1_ = y1_;

        saved_x2_ = x2_;

        saved_y2_ = y2_;

        // Grow to fill the host window.

        Window root;

        int rx, ry;

        unsigned int rw, rh, rb, rd;

        if (XGetGeometry(display_, window_, &root, &rx, &ry, &rw, &rh, &rb, &rd))

        {

            x1_ = 0;

            y1_ = 0;

            x2_ = (int)rw;

            y2_ = (int)rh;

        }

        maximized_ = true;

        if (on_maximize)

        {

            on_maximize();

        }

    }

    void unmaximize()

    {

        if (!maximized_)

        {

            return;

        }

        x1_ = saved_x1_;

        y1_ = saved_y1_;

        x2_ = saved_x2_;

        y2_ = saved_y2_;

        maximized_ = false;

    }

    // The client area below the title bar: where child widgets go.

    int content_x()

    {

        return x1_ + 4;

    }

    int content_y()

    {

        return y1_ + (decorated_ ? title_bar_height() : 0) + 4;

    }

    int content_width()

    {

        return width() - 8;

    }

    int content_height()

    {

        return y2_ - content_y() - 4;

    }

    //endregion

    void draw()

    {

        if (!visible_ || closed_ || iconified_)

        {

            return;

        }

        SStyle st = style();

        // The window body.

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        if (decorated_)

        {

            int tb = title_bar_height();

            // Title bar: a band in the outline colour across the top.

            XSetForeground(display_, gc_, st.outlineColor_);

            XFillRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, tb - 2);

            // Title text, left aligned in the bar.

            SStyle tst = st;

            tst.text_color_ = st.fillColor_;

            xf::draw_text_left(display_, window_, gc_, tst, x1_ + 10, y1_, y1_ + tb, text_);

            // The three window buttons.

            const char* glyphs[3] = { "_", "+", "X" };

            for (int i = 1; i <= 3; ++i)

            {

                int bx1, by1, bx2, by2;

                button_rect(i, bx1, by1, bx2, by2);

                SStyle bst = st;

                if (hover_button_ == i)

                {

                    bst.fillColor_ = (i == 3) ? styles_->get_color_rgb(205, 35, 35) : st.fillColor_;

                }

                xf::draw_polished(display_, window_, gc_, bst, bx1, by1, bx2, by2);

                xf::draw_text_centered(display_, window_, gc_, bst, bx1, by1, bx2, by2, glyphs[i - 1]);

            }

        }

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 2, height() - 2);

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

        if (decorated_ && y <= y1_ + title_bar_height())

        {

            for (int i = 1; i <= 3; ++i)

            {

                int bx1, by1, bx2, by2;

                button_rect(i, bx1, by1, bx2, by2);

                if (in_rect(x, y, bx1, by1, bx2, by2))

                {

                    if (i == 1)

                    {

                        iconify();

                    }

                    else if (i == 2)

                    {

                        if (maximized_)

                        {

                            unmaximize();

                        }

                        else

                        {

                            maximize();

                        }

                    }

                    else

                    {

                        close();

                    }

                    return true;

                }

            }

            // Pressed the title bar itself: start a drag move.

            drag_ = true;

            drag_dx_ = x - x1_;

            drag_dy_ = y - y1_;

            return true;

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button == 1 && drag_)

        {

            drag_ = false;

            return true;

        }

        return false;

    }

    bool on_motion(int x, int y)

    {

        if (drag_)

        {

            int w = width();

            int h = height();

            x1_ = x - drag_dx_;

            y1_ = y - drag_dy_;

            x2_ = x1_ + w;

            y2_ = y1_ + h;

            return true;

        }

        int old = hover_button_;

        hover_button_ = 0;

        if (decorated_ && contains(x, y))

        {

            for (int i = 1; i <= 3; ++i)

            {

                int bx1, by1, bx2, by2;

                button_rect(i, bx1, by1, bx2, by2);

                if (in_rect(x, y, bx1, by1, bx2, by2))

                {

                    hover_button_ = i;

                }

            }

        }

        return hover_button_ != old;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_F4)

        {

            close();

            return true;

        }

        return false;

    }

};

//endregion
