#pragma once

#include "core/xframe_core.h"

//region GtkImage

// Displays an image. With no file loaded it draws the classic
// placeholder picture (sun over mountains) from X11 primitives;
// set_filename() switches it to a checkerboard "loaded image"
// stand-in with the file name captioned underneath.

class GtkImage

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

    std::string filename_;

    std::string icon_name_ = "image-x-generic";

    int icon_size_ = 1;        // 0 = small, 1 = normal, 2 = large

    int pixel_size_ = -1;      // -1 = use the widget bounds

public:

    GtkImage(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkImage";

        text_ = "GtkImage";

    }

    ~GtkImage() {}

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

    //region GtkImage API

    void set_filename(std::string f)

    {

        filename_ = f;

    }

    std::string get_filename()

    {

        return filename_;

    }

    void clear()

    {

        filename_.clear();

    }

    void set_icon_name(std::string n)

    {

        icon_name_ = n;

    }

    std::string get_icon_name()

    {

        return icon_name_;

    }

    void set_icon_size(int s)

    {

        icon_size_ = std::max(0, std::min(2, s));

    }

    int get_icon_size()

    {

        return icon_size_;

    }

    void set_pixel_size(int px)

    {

        pixel_size_ = px;

    }

    int get_pixel_size()

    {

        return pixel_size_;

    }

    // The square the picture itself is drawn into, centred.

    void image_rect(int& rx, int& ry, int& rw, int& rh)

    {

        int side = pixel_size_ > 0 ? pixel_size_ : std::min(width(), height());

        rw = side;

        rh = side;

        rx = x1_ + (width() - rw) / 2;

        ry = y1_ + (height() - rh) / 2;

    }

    // The placeholder glyph: sky, sun, two mountains.

    void draw_placeholder(int rx, int ry, int rw, int rh)

    {

        XSetForeground(display_, gc_, styles_->get_color_rgb(135, 190, 235));

        XFillRectangle(display_, window_, gc_, rx, ry, rw, rh);

        // Sun.

        int sun_r = std::max(4, rw / 7);

        XSetForeground(display_, gc_, styles_->get_color_rgb(250, 215, 60));

        XFillArc(display_, window_, gc_, rx + rw - 2 * sun_r - rw / 12, ry + rh / 10, 2 * sun_r, 2 * sun_r, 0, 360 * 64);

        // Back mountain.

        XPoint back[3] = {

            { (short)(rx), (short)(ry + rh) },

            { (short)(rx + rw / 2), (short)(ry + rh / 3) },

            { (short)(rx + rw), (short)(ry + rh) }

        };

        XSetForeground(display_, gc_, styles_->get_color_rgb(105, 125, 150));

        XFillPolygon(display_, window_, gc_, back, 3, Convex, CoordModeOrigin);

        // Front mountain.

        XPoint front[3] = {

            { (short)(rx), (short)(ry + rh) },

            { (short)(rx + rw / 3), (short)(ry + rh / 2) },

            { (short)(rx + (2 * rw) / 3), (short)(ry + rh) }

        };

        XSetForeground(display_, gc_, styles_->get_color_rgb(70, 90, 115));

        XFillPolygon(display_, window_, gc_, front, 3, Convex, CoordModeOrigin);

    }

    // Checkerboard stand-in for a loaded image file.

    void draw_checkerboard(int rx, int ry, int rw, int rh)

    {

        int cell = 8;

        for (int yy = 0; yy < rh; yy += cell)

        {

            for (int xx = 0; xx < rw; xx += cell)

            {

                bool dark = ((xx / cell) + (yy / cell)) % 2 == 0;

                XSetForeground(display_, gc_, dark ? styles_->get_color_rgb(170, 170, 170) : styles_->get_color_rgb(225, 225, 225));

                XFillRectangle(display_, window_, gc_, rx + xx, ry + yy, std::min(cell, rw - xx), std::min(cell, rh - yy));

            }

        }

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        int rx, ry, rw, rh;

        image_rect(rx, ry, rw, rh);

        if (filename_.empty())

        {

            draw_placeholder(rx, ry, rw, rh);

        }

        else

        {

            draw_checkerboard(rx, ry, rw, rh);

        }

        if (!enabled_)

        {

            XSetForeground(display_, gc_, styles_->get_color_rgb(200, 200, 200));

            for (int yy = ry; yy < ry + rh; yy += 3)

            {

                XDrawLine(display_, window_, gc_, rx, yy, rx + rw, yy);

            }

        }

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, rx, ry, rw - 1, rh - 1);

        if (!filename_.empty())

        {

            SStyle cap = st;

            cap.text_color_ = styles_->get_color_rgb(90, 90, 90);

            std::string base = filename_;

            size_t slash = base.find_last_of('/');

            if (slash != std::string::npos)

            {

                base = base.substr(slash + 1);

            }

            xf::draw_text_centered(display_, window_, gc_, cap, rx, ry + rh - 16, rx + rw, ry + rh, base);

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
