#pragma once

#include "core/xframe_core.h"

//region GtkPicture

// Displays a picture with a natural (intrinsic) size, scaled into
// the widget while optionally keeping the aspect ratio — the
// scaling math is the point of the widget, like GTK's GtkPicture.

class GtkPicture

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

    bool keep_aspect_ratio_ = true;

    bool can_shrink_ = true;

    int content_w_ = 640;      // intrinsic size of the "picture"

    int content_h_ = 400;

public:

    GtkPicture(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkPicture";

        text_ = "GtkPicture";

        filename_ = "photo.jpg";

    }

    ~GtkPicture() {}

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

    //region GtkPicture API

    void set_filename(std::string f)

    {

        filename_ = f;

    }

    std::string get_filename()

    {

        return filename_;

    }

    void set_keep_aspect_ratio(bool k)

    {

        keep_aspect_ratio_ = k;

    }

    bool get_keep_aspect_ratio()

    {

        return keep_aspect_ratio_;

    }

    void set_can_shrink(bool s)

    {

        can_shrink_ = s;

    }

    bool get_can_shrink()

    {

        return can_shrink_;

    }

    void set_content_size(int w, int h)

    {

        content_w_ = std::max(1, w);

        content_h_ = std::max(1, h);

    }

    int get_content_width()

    {

        return content_w_;

    }

    int get_content_height()

    {

        return content_h_;

    }

    // Where the picture actually lands inside the widget.

    void paintable_rect(int& rx, int& ry, int& rw, int& rh)

    {

        if (!keep_aspect_ratio_)

        {

            rx = x1_;

            ry = y1_;

            rw = width();

            rh = height();

            return;

        }

        double scale = std::min((double)width() / content_w_, (double)height() / content_h_);

        if (!can_shrink_)

        {

            scale = std::max(1.0, scale);

        }

        rw = (int)(content_w_ * scale);

        rh = (int)(content_h_ * scale);

        rx = x1_ + (width() - rw) / 2;

        ry = y1_ + (height() - rh) / 2;

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // Letterbox area behind the picture.

        XSetForeground(display_, gc_, styles_->get_color_rgb(40, 40, 44));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        int rx, ry, rw, rh;

        paintable_rect(rx, ry, rw, rh);

        // The picture itself: a little landscape scene, scaled.

        XSetForeground(display_, gc_, styles_->get_color_rgb(135, 190, 235));

        XFillRectangle(display_, window_, gc_, rx, ry, rw, rh);

        int sun_r = std::max(3, rw / 9);

        XSetForeground(display_, gc_, styles_->get_color_rgb(250, 215, 60));

        XFillArc(display_, window_, gc_, rx + rw / 8, ry + rh / 8, 2 * sun_r, 2 * sun_r, 0, 360 * 64);

        XPoint ridge[4] = {

            { (short)(rx), (short)(ry + rh) },

            { (short)(rx + rw / 3), (short)(ry + rh / 3) },

            { (short)(rx + (2 * rw) / 3), (short)(ry + (2 * rh) / 3) },

            { (short)(rx + rw), (short)(ry + rh / 2) }

        };

        // Close the ridge into a filled hill shape.

        XPoint hill[6] = {

            ridge[0], ridge[1], ridge[2], ridge[3],

            { (short)(rx + rw), (short)(ry + rh) },

            { (short)(rx), (short)(ry + rh) }

        };

        XSetForeground(display_, gc_, styles_->get_color_rgb(80, 140, 80));

        XFillPolygon(display_, window_, gc_, hill, 6, Nonconvex, CoordModeOrigin);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, rx, ry, rw - 1, rh - 1);

        if (!filename_.empty())

        {

            SStyle cap = st;

            cap.text_color_ = styles_->get_color_rgb(230, 230, 230);

            std::string base = filename_;

            size_t slash = base.find_last_of('/');

            if (slash != std::string::npos)

            {

                base = base.substr(slash + 1);

            }

            xf::draw_text_at(display_, window_, gc_, cap, x1_ + 4, y2_ - 5, base);

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
