#pragma once

#include "core/xframe_core.h"

#include <cmath>

//region GtkColorChooserWidget

class GtkColorChooserWidget

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

    Srgb current_ = {200, 60, 40};

    int alpha_ = 255;

    int grid_cols_ = 12;

    int grid_rows_ = 6;

    //region Colour math

    Srgb hsv_to_rgb(double h, double s, double v)

    {

        double c = v * s;

        double x = c * (1.0 - std::fabs(std::fmod(h / 60.0, 2.0) - 1.0));

        double m = v - c;

        double r = 0.0, g = 0.0, b = 0.0;

        if (h < 60.0) { r = c; g = x; }

        else if (h < 120.0) { r = x; g = c; }

        else if (h < 180.0) { g = c; b = x; }

        else if (h < 240.0) { g = x; b = c; }

        else if (h < 300.0) { r = x; b = c; }

        else { r = c; b = x; }

        Srgb out;

        out.r = (int)((r + m) * 255.0 + 0.5);

        out.g = (int)((g + m) * 255.0 + 0.5);

        out.b = (int)((b + m) * 255.0 + 0.5);

        return out;

    }

    void rgb_to_hsv(Srgb in, double& h, double& s, double& v)

    {

        double r = in.r / 255.0, g = in.g / 255.0, b = in.b / 255.0;

        double mx = std::max(r, std::max(g, b));

        double mn = std::min(r, std::min(g, b));

        double d = mx - mn;

        v = mx;

        s = mx > 0.0 ? d / mx : 0.0;

        if (d == 0.0)

        {

            h = 0.0;

        }

        else if (mx == r)

        {

            h = 60.0 * std::fmod((g - b) / d, 6.0);

        }

        else if (mx == g)

        {

            h = 60.0 * ((b - r) / d + 2.0);

        }

        else

        {

            h = 60.0 * ((r - g) / d + 4.0);

        }

        if (h < 0.0)

        {

            h += 360.0;

        }

    }

    Srgb cell_color(int col, int row)

    {

        double hue = col * (360.0 / grid_cols_);

        double val = 1.0 - row * 0.13;

        return hsv_to_rgb(hue, 0.85, val);

    }

    //endregion

    //region Layout

    int grid_x2()

    {

        return x2_ - 140;

    }

    int grid_y2()

    {

        return y2_ - 44;

    }

    int cell_w()

    {

        return std::max(8, (grid_x2() - x1_ - 12) / grid_cols_);

    }

    int cell_h()

    {

        return std::max(8, (grid_y2() - y1_ - 12) / grid_rows_);

    }

    int panel_x()

    {

        return x2_ - 132;

    }

    // Channel rows in the side panel: 0=R 1=G 2=B 3=A.

    int channel_row_y(int channel)

    {

        return y1_ + 66 + channel * 26;

    }

    int channel_of(int channel)

    {

        if (channel == 0)

        {

            return current_.r;

        }

        if (channel == 1)

        {

            return current_.g;

        }

        if (channel == 2)

        {

            return current_.b;

        }

        return alpha_;

    }

    void set_channel(int channel, int value)

    {

        value = std::max(0, std::min(255, value));

        if (channel == 0)

        {

            current_.r = value;

        }

        else if (channel == 1)

        {

            current_.g = value;

        }

        else if (channel == 2)

        {

            current_.b = value;

        }

        else

        {

            alpha_ = value;

        }

        if (on_color_changed)

        {

            on_color_changed();

        }

    }

    //endregion

public:

    std::function<void()> on_color_changed;

    GtkColorChooserWidget(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkColorChooserWidget";

        text_ = "Color Chooser";

    }

    ~GtkColorChooserWidget() {}

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

    //region Color chooser specific API

    void set_rgba(Srgb color, int alpha)

    {

        current_ = color;

        alpha_ = std::max(0, std::min(255, alpha));

    }

    Srgb get_rgba()

    {

        return current_;

    }

    int get_alpha()

    {

        return alpha_;

    }

    std::string get_hex()

    {

        char buf[16];

        snprintf(buf, sizeof(buf), "#%02X%02X%02X", current_.r, current_.g, current_.b);

        return std::string(buf);

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

        // Palette grid: hue across, value down.

        int cw = cell_w();

        int ch = cell_h();

        for (int row = 0; row < grid_rows_; ++row)

        {

            for (int col = 0; col < grid_cols_; ++col)

            {

                Srgb c = cell_color(col, row);

                int cx = x1_ + 6 + col * cw;

                int cy = y1_ + 6 + row * ch;

                XSetForeground(display_, gc_, styles_->get_color_Srgb(c));

                XFillRectangle(display_, window_, gc_, cx, cy, cw - 1, ch - 1);

                if (c.r == current_.r && c.g == current_.g && c.b == current_.b)

                {

                    XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

                    XDrawRectangle(display_, window_, gc_, cx, cy, cw - 2, ch - 2);

                }

            }

        }

        // Hue strip under the grid.

        int strip_y = y2_ - 36;

        int strip_w = grid_x2() - x1_ - 12;

        for (int i = 0; i < 36; ++i)

        {

            Srgb c = hsv_to_rgb(i * 10.0, 1.0, 1.0);

            int sx = x1_ + 6 + (strip_w * i) / 36;

            int sw = (strip_w * (i + 1)) / 36 - (strip_w * i) / 36;

            XSetForeground(display_, gc_, styles_->get_color_Srgb(c));

            XFillRectangle(display_, window_, gc_, sx, strip_y, sw, 18);

        }

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_ + 6, strip_y, strip_w, 18);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 6, strip_y + 18, y2_ - 2, "Hue");

        // Side panel: current swatch.

        int px = panel_x();

        XSetForeground(display_, gc_, styles_->get_color_Srgb(current_));

        XFillRectangle(display_, window_, gc_, px, y1_ + 6, x2_ - px - 8, 34);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, px, y1_ + 6, x2_ - px - 8, 34);

        xf::draw_text_left(display_, window_, gc_, st, px, y1_ + 42, y1_ + 62, get_hex());

        // Channel rows with - / + steppers.

        const char *labels[] = {"R", "G", "B", "A"};

        for (int channel = 0; channel < 4; ++channel)

        {

            int ry = channel_row_y(channel);

            xf::draw_text_left(display_, window_, gc_, st, px, ry, ry + 20, labels[channel]);

            char buf[8];

            snprintf(buf, sizeof(buf), "%d", channel_of(channel));

            xf::draw_text_left(display_, window_, gc_, st, px + 16, ry, ry + 20, buf);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x2_ - 52, ry, 18, 20);

            XDrawRectangle(display_, window_, gc_, x2_ - 30, ry, 18, 20);

            xf::draw_text_centered(display_, window_, gc_, st, x2_ - 52, ry, x2_ - 34, ry + 20, "-");

            xf::draw_text_centered(display_, window_, gc_, st, x2_ - 30, ry, x2_ - 12, ry + 20, "+");

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

        // Channel steppers first.

        for (int channel = 0; channel < 4; ++channel)

        {

            int ry = channel_row_y(channel);

            if (y >= ry && y <= ry + 20)

            {

                if (x >= x2_ - 52 && x <= x2_ - 34)

                {

                    set_channel(channel, channel_of(channel) - 8);

                    return true;

                }

                if (x >= x2_ - 30 && x <= x2_ - 12)

                {

                    set_channel(channel, channel_of(channel) + 8);

                    return true;

                }

            }

        }

        // Hue strip: keep saturation / value, swap the hue.

        int strip_y = y2_ - 36;

        int strip_w = grid_x2() - x1_ - 12;

        if (y >= strip_y && y <= strip_y + 18 && x >= x1_ + 6 && x <= x1_ + 6 + strip_w)

        {

            double hue = (double)(x - x1_ - 6) / (double)strip_w * 360.0;

            double h, s, v;

            rgb_to_hsv(current_, h, s, v);

            if (s < 0.05)

            {

                s = 0.85;

            }

            if (v < 0.05)

            {

                v = 1.0;

            }

            current_ = hsv_to_rgb(hue, s, v);

            if (on_color_changed)

            {

                on_color_changed();

            }

            return true;

        }

        // Palette grid.

        int cw = cell_w();

        int ch = cell_h();

        if (x >= x1_ + 6 && x < grid_x2() - 6 && y >= y1_ + 6 && y < grid_y2() - 6)

        {

            int col = (x - x1_ - 6) / cw;

            int row = (y - y1_ - 6) / ch;

            if (col >= 0 && col < grid_cols_ && row >= 0 && row < grid_rows_)

            {

                current_ = cell_color(col, row);

                if (on_color_changed)

                {

                    on_color_changed();

                }

            }

            return true;

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

        (void)key;

        (void)str;

        return false;

    }

};

//endregion
