#pragma once

#include "core/xframe_core.h"

//region TPaintBox

class TPaintBox

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

    Canvas *canvas_ = nullptr;

    XImage *image_ = nullptr;

    // The core Canvas presents at 0,0, so the paint box keeps its
    // own XImage (built exactly like Canvas::present builds its)
    // and puts it at the widget offset x1_, y1_ instead.

    void create_image()

    {

        destroy_image();

        int w = std::max(1, width());

        int h = std::max(1, height());

        image_ = XCreateImage(display_, DefaultVisual(display_, screen_), DefaultDepth(display_, screen_), ZPixmap, 0, nullptr, w, h, 32, 0);

        if (!image_)

        {

            return;

        }

        image_->data = static_cast<char*>(std::malloc(image_->bytes_per_line * h));

    }

    void destroy_image()

    {

        if (image_)

        {

            XDestroyImage(image_);

            image_ = nullptr;

        }

    }

    void present_at()

    {

        if (!canvas_ || !image_)

        {

            return;

        }

        int w = std::min(width(), canvas_->getWidth());

        int h = std::min(height(), canvas_->getHeight());

        for (int y = 0; y < h; ++y)

        {

            for (int x = 0; x < w; ++x)

            {

                uint32_t rgb = canvas_->getPixel(x, y);

                unsigned long x11Pixel = ((rgb >> 16) & 0xff) << 16 | ((rgb >> 8) & 0xff) << 8 | (rgb & 0xff);

                XPutPixel(image_, x, y, x11Pixel);

            }

        }

        XPutImage(display_, window_, gc_, image_, 0, 0, x1_, y1_, w, h);

    }

    void draw_default_scene()

    {

        canvas_->drawGradient();

        canvas_->fillRect(10, 10, 60, 40, 0x204060);

        canvas_->fillRect(30, 30, 60, 40, 0xd04040);

        for (int i = 0; i < 40; ++i)

        {

            canvas_->setPixel(10 + i, 70 + i / 2, 0xffffff);

        }

    }

public:

    std::function<void(TPaintBox*)> on_paint;

    TPaintBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TPaintBox";

        text_ = "TPaintBox";

        canvas_ = new Canvas(display_, window_, gc_, std::max(1, width), std::max(1, height));

        create_image();

    }

    ~TPaintBox()

    {

        destroy_image();

        delete canvas_;

        canvas_ = nullptr;

    }

    void set_bounds(int x, int y, int width, int height)

    {

        x1_ = x;

        y1_ = y;

        x2_ = x + width;

        y2_ = y + height;

        if (canvas_)

        {

            canvas_->resize(std::max(1, width), std::max(1, height));

        }

        create_image();

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

    Canvas* canvas()

    {

        return canvas_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        if (on_paint)

        {

            on_paint(this);

        }

        else

        {

            draw_default_scene();

        }

        present_at();

        SStyle st = style();

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

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

        (void)key;

        (void)str;

        return false;

    }

};

//endregion
