#pragma once

#include "core/xframe_core.h"

#include <cmath>

//region GtkGLArea

// A placeholder OpenGL viewport: a dark panel with a perspective-
// style grid and axes drawn in 2D, a "GL" caption, and the state
// the real widget carries (render callback, queue_render count,
// current-context flag). No real GL context is created.

class GtkGLArea

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

    bool auto_render_ = true;

    bool has_depth_buffer_ = false;

    bool has_stencil_buffer_ = false;

    bool use_es_ = false;

    bool is_current_ = false;

    int render_requests_ = 0;

    double rotation_ = 30.0;   // demo scene angle, degrees

public:

    std::function<bool()> on_render;   // return false to stop

    GtkGLArea(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkGLArea";

        text_ = "GL";

    }

    ~GtkGLArea() {}

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

    //region GtkGLArea API

    void set_render_callback(std::function<bool()> cb)

    {

        on_render = cb;

    }

    void queue_render()

    {

        ++render_requests_;

    }

    int get_render_requests()

    {

        return render_requests_;

    }

    void make_current()

    {

        is_current_ = true;

    }

    bool get_is_current()

    {

        return is_current_;

    }

    void set_auto_render(bool a)

    {

        auto_render_ = a;

    }

    bool get_auto_render()

    {

        return auto_render_;

    }

    void set_has_depth_buffer(bool d)

    {

        has_depth_buffer_ = d;

    }

    bool get_has_depth_buffer()

    {

        return has_depth_buffer_;

    }

    void set_has_stencil_buffer(bool s)

    {

        has_stencil_buffer_ = s;

    }

    bool get_has_stencil_buffer()

    {

        return has_stencil_buffer_;

    }

    void set_use_es(bool e)

    {

        use_es_ = e;

    }

    bool get_use_es()

    {

        return use_es_;

    }

    void set_rotation(double deg)

    {

        rotation_ = deg;

    }

    double get_rotation()

    {

        return rotation_;

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // Viewport: dark blue-grey.

        XSetForeground(display_, gc_, styles_->get_color_rgb(24, 26, 32));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        int cx = (x1_ + x2_) / 2;

        int cy = (y1_ + y2_) / 2;

        // Grid: verticals converge slightly toward a horizon feel,
        // horizontals bunch toward the bottom — a floor plane.

        XSetForeground(display_, gc_, styles_->get_color_rgb(52, 58, 70));

        for (int i = -4; i <= 4; ++i)

        {

            XDrawLine(display_, window_, gc_, cx + i * 8, cy, cx + i * (width() / 9), y2_ - 2);

        }

        for (int i = 1; i <= 4; ++i)

        {

            int yy = cy + (i * i) * (height() / 40);

            if (yy < y2_ - 1)

            {

                XDrawLine(display_, window_, gc_, x1_ + 2, yy, x2_ - 3, yy);

            }

        }

        XDrawLine(display_, window_, gc_, x1_ + 2, cy, x2_ - 3, cy);

        // Axes: X red, Y green, Z blue (drawn as a diagonal).

        xf::set_line_width(display_, gc_, 2);

        XSetForeground(display_, gc_, styles_->get_color_rgb(200, 60, 60));

        XDrawLine(display_, window_, gc_, cx, cy, cx + width() / 4, cy);

        XSetForeground(display_, gc_, styles_->get_color_rgb(60, 190, 80));

        XDrawLine(display_, window_, gc_, cx, cy, cx, cy - height() / 4);

        XSetForeground(display_, gc_, styles_->get_color_rgb(70, 110, 220));

        XDrawLine(display_, window_, gc_, cx, cy, cx - width() / 6, cy + height() / 6);

        xf::set_line_width(display_, gc_, 1);

        // A wireframe square, rotated by rotation_, as the scene.

        double rad = rotation_ * 3.14159265 / 180.0;

        int r = std::min(width(), height()) / 5;

        int px[4];

        int py[4];

        for (int i = 0; i < 4; ++i)

        {

            double a = rad + i * 3.14159265 / 2.0;

            px[i] = cx + (int)(std::cos(a) * r);

            py[i] = cy + (int)(std::sin(a) * r * 0.6);

        }

        XSetForeground(display_, gc_, styles_->get_color_rgb(230, 200, 90));

        for (int i = 0; i < 4; ++i)

        {

            int j = (i + 1) % 4;

            XDrawLine(display_, window_, gc_, px[i], py[i], px[j], py[j]);

        }

        // Caption.

        SStyle cap = st;

        cap.text_color_ = styles_->get_color_rgb(200, 205, 215);

        xf::draw_text_at(display_, window_, gc_, cap, x1_ + 6, y1_ + 15, text_);

        if (focused_)

        {

            XSetForeground(display_, gc_, styles_->get_color_rgb(130, 130, 140));

            XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        }

        // The render callback gets its turn after the base scene.

        if (on_render)

        {

            on_render();

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || button != 1)

        {

            return false;

        }

        focused_ = true;

        make_current();

        queue_render();

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

        (void)y;

        if (focused_ && contains(x, y))

        {

            // Dragging across the viewport spins the demo scene.

            rotation_ = (double)((x - x1_) * 360 / std::max(1, width()));

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

        if (key == XK_Left)

        {

            rotation_ -= 5.0;

            queue_render();

            return true;

        }

        if (key == XK_Right)

        {

            rotation_ += 5.0;

            queue_render();

            return true;

        }

        return false;

    }

};

//endregion
