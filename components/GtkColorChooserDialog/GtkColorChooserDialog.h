#pragma once

#include "core/xframe_core.h"

#include <cmath>

//region GtkColorChooserDialog

class GtkColorChooserDialog

{

public:

    // Real GTK response ids.

    static const int RESPONSE_NONE = -1;

    static const int RESPONSE_DELETE_EVENT = -4;

    static const int RESPONSE_ACCEPT = -3;

    static const int RESPONSE_OK = -5;

    static const int RESPONSE_CANCEL = -6;

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;          // the dialog title

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    // The colour being edited, kept as HSV like the real widget:
    // hue 0..360, saturation 0..1, value 0..1.

    double hue_ = 0.0;

    double saturation_ = 0.76;

    double value_ = 0.80;

    bool use_alpha_ = false;

    double alpha_ = 1.0;

    int response_ = RESPONSE_NONE;

    int pressed_button_ = -1;   // 0 cancel, 1 ok

    bool dragging_ = false;     // mouse held in the palette / hue strip

    int drag_target_ = 0;       // 1 = palette, 2 = hue strip

    int title_bar_height()

    {

        return 26;

    }

    int action_bar_height()

    {

        return 40;

    }

    // Palette (saturation x value square) geometry.

    int pal_x1()

    {

        return x1_ + 12;

    }

    int pal_y1()

    {

        return y1_ + title_bar_height() + 12;

    }

    int pal_x2()

    {

        return x2_ - 90;

    }

    int pal_y2()

    {

        return y2_ - action_bar_height() - 44;

    }

    // Hue strip geometry, right of the palette.

    int hue_x1()

    {

        return pal_x2() + 10;

    }

    int hue_x2()

    {

        return hue_x1() + 20;

    }

    void button_rect(int which, int &bx1, int &by1, int &bx2, int &by2)

    {

        by2 = y2_ - 7;

        by1 = by2 - 26;

        bx2 = x2_ - 10 - (1 - which) * 96;

        bx1 = bx2 - 88;

    }

    bool in_rect(int x, int y, int rx1, int ry1, int rx2, int ry2)

    {

        return x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2;

    }

    // Standard HSV -> RGB conversion, channels out 0..255.

    void hsv_to_rgb(double h, double s, double v, int &r, int &g, int &b)

    {

        double c = v * s;

        double hp = h / 60.0;

        double xx = c * (1.0 - std::fabs(std::fmod(hp, 2.0) - 1.0));

        double m = v - c;

        double rr = 0, gg = 0, bb = 0;

        if (hp < 1.0)

        {

            rr = c; gg = xx;

        }

        else if (hp < 2.0)

        {

            rr = xx; gg = c;

        }

        else if (hp < 3.0)

        {

            gg = c; bb = xx;

        }

        else if (hp < 4.0)

        {

            gg = xx; bb = c;

        }

        else if (hp < 5.0)

        {

            rr = xx; bb = c;

        }

        else

        {

            rr = c; bb = xx;

        }

        r = (int)((rr + m) * 255.0 + 0.5);

        g = (int)((gg + m) * 255.0 + 0.5);

        b = (int)((bb + m) * 255.0 + 0.5);

    }

    void pick_from_palette(int x, int y)

    {

        double s = (double)(x - pal_x1()) / std::max(1, pal_x2() - pal_x1());

        double v = 1.0 - (double)(y - pal_y1()) / std::max(1, pal_y2() - pal_y1());

        if (s < 0.0) s = 0.0;

        if (s > 1.0) s = 1.0;

        if (v < 0.0) v = 0.0;

        if (v > 1.0) v = 1.0;

        saturation_ = s;

        value_ = v;

    }

    void pick_from_hue(int y)

    {

        double f = (double)(y - pal_y1()) / std::max(1, pal_y2() - pal_y1());

        if (f < 0.0) f = 0.0;

        if (f > 1.0) f = 1.0;

        hue_ = f * 360.0;

    }

public:

    std::function<void(int)> on_response;

    std::function<void()> on_color_changed;

    GtkColorChooserDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkColorChooserDialog";

        text_ = "Select a Color";

    }

    ~GtkColorChooserDialog() {}

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

    //region GtkColorChooserDialog specific API

    void set_color(Srgb c)

    {

        double r = c.r / 255.0;

        double g = c.g / 255.0;

        double b = c.b / 255.0;

        double mx = std::max(r, std::max(g, b));

        double mn = std::min(r, std::min(g, b));

        double d = mx - mn;

        value_ = mx;

        saturation_ = (mx == 0.0) ? 0.0 : d / mx;

        if (d == 0.0)

        {

            hue_ = 0.0;

        }

        else if (mx == r)

        {

            hue_ = 60.0 * std::fmod((g - b) / d, 6.0);

        }

        else if (mx == g)

        {

            hue_ = 60.0 * ((b - r) / d + 2.0);

        }

        else

        {

            hue_ = 60.0 * ((r - g) / d + 4.0);

        }

        if (hue_ < 0.0)

        {

            hue_ += 360.0;

        }

    }

    Srgb get_color()

    {

        Srgb c;

        hsv_to_rgb(hue_, saturation_, value_, c.r, c.g, c.b);

        return c;

    }

    std::string get_hex()

    {

        Srgb c = get_color();

        const char* digits = "0123456789ABCDEF";

        std::string out = "#";

        int channels[3] = { c.r, c.g, c.b };

        for (int i = 0; i < 3; ++i)

        {

            out += digits[(channels[i] >> 4) & 0xF];

            out += digits[channels[i] & 0xF];

        }

        return out;

    }

    void set_use_alpha(bool u)

    {

        use_alpha_ = u;

    }

    bool get_use_alpha()

    {

        return use_alpha_;

    }

    void set_alpha(double a)

    {

        alpha_ = a;

    }

    double get_alpha()

    {

        return alpha_;

    }

    int get_response()

    {

        return response_;

    }

    void respond(int response_id)

    {

        response_ = response_id;

        if (on_response)

        {

            on_response(response_id);

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

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        int tb = title_bar_height();

        // Title bar.

        XSetForeground(display_, gc_, st.outlineColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, tb - 2);

        SStyle tst = st;

        tst.text_color_ = st.fillColor_;

        xf::draw_text_left(display_, window_, gc_, tst, x1_ + 10, y1_, y1_ + tb, text_);

        xf::draw_text_centered(display_, window_, gc_, tst, x2_ - 28, y1_, x2_ - 4, y1_ + tb, "X");

        // The saturation/value palette, drawn as a grid of cells
        // (X11 has no gradients; a 32x20 grid reads as one).

        int cols = 32;

        int rows = 20;

        int pw = pal_x2() - pal_x1();

        int ph = pal_y2() - pal_y1();

        for (int row = 0; row < rows; ++row)

        {

            for (int col = 0; col < cols; ++col)

            {

                int r, g, b;

                hsv_to_rgb(hue_, (double)col / (cols - 1), 1.0 - (double)row / (rows - 1), r, g, b);

                XSetForeground(display_, gc_, styles_->get_color_rgb((unsigned char)r, (unsigned char)g, (unsigned char)b));

                int cx1 = pal_x1() + col * pw / cols;

                int cy1 = pal_y1() + row * ph / rows;

                int cx2 = pal_x1() + (col + 1) * pw / cols;

                int cy2 = pal_y1() + (row + 1) * ph / rows;

                XFillRectangle(display_, window_, gc_, cx1, cy1, cx2 - cx1 + 1, cy2 - cy1 + 1);

            }

        }

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, pal_x1(), pal_y1(), pw, ph);

        // Palette cursor: a small ring at the current s/v point.

        int cur_x = pal_x1() + (int)(saturation_ * pw);

        int cur_y = pal_y1() + (int)((1.0 - value_) * ph);

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XDrawArc(display_, window_, gc_, cur_x - 5, cur_y - 5, 10, 10, 0, 360 * 64);

        XSetForeground(display_, gc_, styles_->get_color_rgb(0, 0, 0));

        XDrawArc(display_, window_, gc_, cur_x - 6, cur_y - 6, 12, 12, 0, 360 * 64);

        // Hue strip: rainbow segments top (0) to bottom (360).

        int segments = 36;

        for (int i = 0; i < segments; ++i)

        {

            int r, g, b;

            hsv_to_rgb(i * 360.0 / segments, 1.0, 1.0, r, g, b);

            XSetForeground(display_, gc_, styles_->get_color_rgb((unsigned char)r, (unsigned char)g, (unsigned char)b));

            int sy1 = pal_y1() + i * ph / segments;

            int sy2 = pal_y1() + (i + 1) * ph / segments;

            XFillRectangle(display_, window_, gc_, hue_x1(), sy1, hue_x2() - hue_x1(), sy2 - sy1 + 1);

        }

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, hue_x1(), pal_y1(), hue_x2() - hue_x1(), ph);

        // Hue marker line.

        int hy = pal_y1() + (int)(hue_ / 360.0 * ph);

        XSetForeground(display_, gc_, styles_->get_color_rgb(0, 0, 0));

        xf::set_line_width(display_, gc_, 2);

        XDrawLine(display_, window_, gc_, hue_x1() - 2, hy, hue_x2() + 2, hy);

        xf::set_line_width(display_, gc_, 1);

        // Readout row: swatch + RGB + hex, under the palette.

        int ry = pal_y2() + 10;

        Srgb c = get_color();

        XSetForeground(display_, gc_, styles_->get_color_rgb((unsigned char)c.r, (unsigned char)c.g, (unsigned char)c.b));

        XFillRectangle(display_, window_, gc_, pal_x1(), ry, 34, 24);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, pal_x1(), ry, 34, 24);

        std::string readout = "R: " + std::to_string(c.r) + "   G: " + std::to_string(c.g) + "   B: " + std::to_string(c.b) + "      " + get_hex();

        xf::draw_text_left(display_, window_, gc_, st, pal_x1() + 46, ry, ry + 24, readout);

        // Action bar.

        int ay = y2_ - action_bar_height();

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x1_ + 2, ay, x2_ - 2, ay);

        const char* labels[2] = { "Cancel", "OK" };

        for (int i = 0; i < 2; ++i)

        {

            int bx1, by1, bx2, by2;

            button_rect(i, bx1, by1, bx2, by2);

            SStyle bst = st;

            if (pressed_button_ == i)

            {

                unsigned long swap = bst.fillColor_;

                bst.fillColor_ = bst.outlineColor_;

                bst.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, bst, bx1, by1, bx2, by2);

            xf::draw_text_centered(display_, window_, gc_, bst, bx1, by1, bx2, by2, labels[i]);

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

        if (in_rect(x, y, x2_ - 28, y1_, x2_ - 4, y1_ + title_bar_height()))

        {

            respond(RESPONSE_DELETE_EVENT);

            return true;

        }

        if (in_rect(x, y, pal_x1(), pal_y1(), pal_x2(), pal_y2()))

        {

            dragging_ = true;

            drag_target_ = 1;

            pick_from_palette(x, y);

            if (on_color_changed)

            {

                on_color_changed();

            }

            return true;

        }

        if (in_rect(x, y, hue_x1(), pal_y1(), hue_x2(), pal_y2()))

        {

            dragging_ = true;

            drag_target_ = 2;

            pick_from_hue(y);

            if (on_color_changed)

            {

                on_color_changed();

            }

            return true;

        }

        for (int i = 0; i < 2; ++i)

        {

            int bx1, by1, bx2, by2;

            button_rect(i, bx1, by1, bx2, by2);

            if (in_rect(x, y, bx1, by1, bx2, by2))

            {

                pressed_button_ = i;

                return true;

            }

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (button != 1)

        {

            return false;

        }

        if (dragging_)

        {

            dragging_ = false;

            drag_target_ = 0;

            return true;

        }

        if (pressed_button_ < 0)

        {

            return false;

        }

        int which = pressed_button_;

        pressed_button_ = -1;

        int bx1, by1, bx2, by2;

        button_rect(which, bx1, by1, bx2, by2);

        if (in_rect(x, y, bx1, by1, bx2, by2))

        {

            respond(which == 1 ? RESPONSE_OK : RESPONSE_CANCEL);

        }

        return true;

    }

    bool on_motion(int x, int y)

    {

        if (!dragging_)

        {

            return false;

        }

        if (drag_target_ == 1)

        {

            pick_from_palette(x, y);

        }

        else

        {

            pick_from_hue(y);

        }

        if (on_color_changed)

        {

            on_color_changed();

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

        if (key == XK_Escape)

        {

            respond(RESPONSE_CANCEL);

            return true;

        }

        if (key == XK_Return)

        {

            respond(RESPONSE_OK);

            return true;

        }

        // Arrow keys nudge the value / saturation.

        if (key == XK_Up || key == XK_Down)

        {

            value_ += (key == XK_Up) ? 0.02 : -0.02;

            if (value_ > 1.0) value_ = 1.0;

            if (value_ < 0.0) value_ = 0.0;

            return true;

        }

        if (key == XK_Left || key == XK_Right)

        {

            saturation_ += (key == XK_Right) ? 0.02 : -0.02;

            if (saturation_ > 1.0) saturation_ = 1.0;

            if (saturation_ < 0.0) saturation_ = 0.0;

            return true;

        }

        return false;

    }

};

//endregion
