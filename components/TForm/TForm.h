#pragma once

#include "core/xframe_core.h"

//region TForm

class TForm

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

    int title_height_ = 28;

    bool closed_ = false;

    bool minimized_ = false;

    bool maximized_ = false;

    int pressed_box_ = 0;   // 0 none, 1 close, 2 minimize, 3 maximize

    bool dragging_ = false;

    int drag_offset_x_ = 0;

    int drag_offset_y_ = 0;

    // Geometry of the three title-bar boxes, right aligned.

    int box_left(int index)

    {

        // index 0 = minimize, 1 = maximize, 2 = close

        return x2_ - 8 - (3 - index) * 26 + 2;

    }

    bool in_box(int x, int y, int index)

    {

        int bx = box_left(index);

        int by = y1_ + 4;

        return x >= bx && x <= bx + 22 && y >= by && y <= by + title_height_ - 8;

    }

    int box_at(int x, int y)

    {

        if (y < y1_ || y > y1_ + title_height_)

        {

            return 0;

        }

        if (in_box(x, y, 2))

        {

            return 1;

        }

        if (in_box(x, y, 0))

        {

            return 2;

        }

        if (in_box(x, y, 1))

        {

            return 3;

        }

        return 0;

    }

public:

    std::function<void()> on_click;

    std::function<void()> on_close;

    struct ClientRect

    {

        int x;

        int y;

        int w;

        int h;

    };

    TForm(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TForm";

        text_ = "Form1";

    }

    ~TForm() {}

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

    // The client area is everything under the title bar, inset by
    // the form border so child widgets can be placed inside it.

    ClientRect client_rect()

    {

        ClientRect r;

        r.x = x1_ + 2;

        r.y = y1_ + title_height_ + 2;

        r.w = width() - 4;

        r.h = height() - title_height_ - 4;

        if (r.w < 0)

        {

            r.w = 0;

        }

        if (r.h < 0)

        {

            r.h = 0;

        }

        return r;

    }

    int client_x()

    {

        return client_rect().x;

    }

    int client_y()

    {

        return client_rect().y;

    }

    int client_width()

    {

        return client_rect().w;

    }

    int client_height()

    {

        return client_rect().h;

    }

    bool get_closed()

    {

        return closed_;

    }

    void set_closed(bool c)

    {

        closed_ = c;

    }

    bool get_minimized()

    {

        return minimized_;

    }

    bool get_maximized()

    {

        return maximized_;

    }

    int get_title_height()

    {

        return title_height_;

    }

    void set_title_height(int h)

    {

        if (h >= 16)

        {

            title_height_ = h;

        }

    }

    void draw()

    {

        if (!visible_ || closed_)

        {

            return;

        }

        SStyle st = style();

        // Form body and border.

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        // Client area in a slightly lighter fill.

        ClientRect cr = client_rect();

        XSetForeground(display_, gc_, styles_->get_color_rgb(245, 245, 245));

        XFillRectangle(display_, window_, gc_, cr.x, cr.y, cr.w, cr.h);

        // Title bar band.

        XSetForeground(display_, gc_, st.outlineColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, title_height_ - 2);

        // Caption, left aligned in the title bar, in the fill colour
        // so it reads against the title band.

        if (st.font_)

        {

            SStyle cap = st;

            cap.text_color_ = st.fillColor_;

            xf::draw_text_left(display_, window_, gc_, cap, x1_ + 10, y1_ + 1, y1_ + title_height_, text_);

        }

        // The three window boxes.

        const char *glyphs[3] = { "_", "[]", "X" };

        for (int i = 0; i < 3; ++i)

        {

            int bx = box_left(i);

            int by = y1_ + 4;

            int bw = 22;

            int bh = title_height_ - 8;

            SStyle bst = st;

            int box_code = (i == 2) ? 1 : (i == 0 ? 2 : 3);

            if (pressed_box_ == box_code)

            {

                unsigned long swap = bst.fillColor_;

                bst.fillColor_ = bst.outlineColor_;

                bst.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, bst, bx, by, bx + bw, by + bh);

            xf::draw_text_centered(display_, window_, gc_, bst, bx, by, bx + bw, by + bh, glyphs[i]);

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

        int box = box_at(x, y);

        if (box != 0)

        {

            pressed_box_ = box;

            return true;

        }

        if (y >= y1_ && y <= y1_ + title_height_)

        {

            // Dragging the caption moves the whole form.

            dragging_ = true;

            drag_offset_x_ = x - x1_;

            drag_offset_y_ = y - y1_;

            return true;

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (button != 1)

        {

            return false;

        }

        bool was_dragging = dragging_;

        dragging_ = false;

        if (pressed_box_ != 0)

        {

            int box = box_at(x, y);

            int released = pressed_box_;

            pressed_box_ = 0;

            if (box != released)

            {

                return true;

            }

            if (released == 1)

            {

                closed_ = true;

                if (on_close)

                {

                    on_close();

                }

            }

            else if (released == 2)

            {

                minimized_ = true;

            }

            else if (released == 3)

            {

                maximized_ = !maximized_;

                minimized_ = false;

            }

            return true;

        }

        if (was_dragging)

        {

            return true;

        }

        if (contains(x, y) && y > y1_ + title_height_ && on_click)

        {

            on_click();

            return true;

        }

        return false;

    }

    bool on_motion(int x, int y)

    {

        if (dragging_)

        {

            int w = width();

            int h = height();

            x1_ = x - drag_offset_x_;

            y1_ = y - drag_offset_y_;

            x2_ = x1_ + w;

            y2_ = y1_ + h;

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

        if (key == XK_Escape)

        {

            closed_ = true;

            if (on_close)

            {

                on_close();

            }

            return true;

        }

        return false;

    }

};

//endregion
