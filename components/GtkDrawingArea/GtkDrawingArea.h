#pragma once

#include "core/xframe_core.h"

//region GtkDrawingArea

// A blank canvas the program draws on itself. It owns a Part 1
// Canvas sized to its bounds and blits it into its rectangle;
// clicking stamps a small white brush dot so a bare demo shows
// the surface is live.

class GtkDrawingArea

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

    Canvas *canvas_ = nullptr;

    XImage *image_ = nullptr;  // blit image mirroring the canvas

    int draw_count_ = 0;

public:

    std::function<void(int, int)> on_draw_point;   // local x, y stamped

    GtkDrawingArea(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkDrawingArea";

        text_ = "GtkDrawingArea";

        canvas_ = new Canvas(display_, window_, gc_, std::max(1, width), std::max(1, height));

        canvas_->drawGradient();   // demo content so the demo shows something

    }

    ~GtkDrawingArea()

    {

        destroy_image();

        delete canvas_;

    }

    void set_bounds(int x, int y, int width, int height)

    {

        bool resized = (width != this->width()) || (height != this->height());

        x1_ = x;

        y1_ = y;

        x2_ = x + width;

        y2_ = y + height;

        if (resized && canvas_)

        {

            canvas_->resize(std::max(1, width), std::max(1, height));

            canvas_->drawGradient();

            destroy_image();

        }

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

    //region GtkDrawingArea API

    Canvas& canvas()

    {

        return *canvas_;

    }

    Canvas* canvas_ptr()

    {

        return canvas_;

    }

    int get_draw_count()

    {

        return draw_count_;

    }

    void clear(uint32_t rgb)

    {

        if (canvas_)

        {

            canvas_->clear(rgb);

        }

    }

    void queue_draw()

    {

        ++draw_count_;

    }

    //endregion

    void draw()

    {

        if (!visible_ || !canvas_)

        {

            return;

        }

        present_at(x1_, y1_);

        // A thin frame so the area reads as a widget on the form.

        SStyle st = style();

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        int lx = x - x1_;

        int ly = y - y1_;

        // Stamp a 5x5 white brush dot on the canvas.

        canvas_->fillRect(lx - 2, ly - 2, 5, 5, 0xFFFFFF);

        if (on_draw_point)

        {

            on_draw_point(lx, ly);

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

        if (key == XK_c)

        {

            canvas_->clear(0x202020);

            return true;

        }

        if (key == XK_g)

        {

            canvas_->drawGradient();

            return true;

        }

        return false;

    }

private:

    void destroy_image()

    {

        if (image_)

        {

            XDestroyImage(image_);

            image_ = nullptr;

        }

    }

    // Same conversion as Canvas::present(), but blitted at the
    // widget's offset instead of the window origin.

    void present_at(int dx, int dy)

    {

        int w = canvas_->getWidth();

        int h = canvas_->getHeight();

        if (!image_ || image_->width != w || image_->height != h)

        {

            destroy_image();

            Visual* visual = DefaultVisual(display_, screen_);

            int depth = DefaultDepth(display_, screen_);

            image_ = XCreateImage(display_, visual, depth, ZPixmap, 0, nullptr, w, h, 32, 0);

            if (!image_)

            {

                return;

            }

            image_->data = static_cast<char*>(std::malloc(image_->bytes_per_line * h));

            if (!image_->data)

            {

                destroy_image();

                return;

            }

        }

        for (int yy = 0; yy < h; ++yy)

        {

            for (int xx = 0; xx < w; ++xx)

            {

                uint32_t rgb = canvas_->getPixel(xx, yy);

                unsigned long px = ((rgb >> 16) & 0xff) << 16 | ((rgb >> 8) & 0xff) << 8 | (rgb & 0xff);

                XPutPixel(image_, xx, yy, px);

            }

        }

        XPutImage(display_, window_, gc_, image_, 0, 0, dx, dy, w, h);

    }

};

//endregion
