#pragma once

#include "core/xframe_core.h"

//region TColorButton

class TColorButton

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

    bool pressed_ = false;

    bool palette_open_ = false;

    Srgb color_{204, 51, 51};

    int palette_cols_ = 5;

    int palette_rows_ = 4;

    int cell_ = 18;

    std::vector<Srgb> palette_

    {

        Srgb{0, 0, 0},       Srgb{128, 128, 128}, Srgb{192, 192, 192}, Srgb{255, 255, 255}, Srgb{128, 0, 0},

        Srgb{255, 0, 0},     Srgb{255, 128, 0},   Srgb{255, 255, 0},   Srgb{128, 255, 0},   Srgb{0, 200, 0},

        Srgb{0, 128, 64},    Srgb{0, 255, 255},   Srgb{0, 128, 255},   Srgb{0, 0, 255},     Srgb{128, 0, 255},

        Srgb{255, 0, 255},   Srgb{128, 64, 0},    Srgb{255, 128, 128}, Srgb{255, 204, 153}, Srgb{204, 51, 51}

    };

    int palette_x2()

    {

        return x1_ + palette_cols_ * (cell_ + 2) + 4;

    }

    int palette_y1()

    {

        return y2_ + 2;

    }

    int palette_y2()

    {

        return palette_y1() + palette_rows_ * (cell_ + 2) + 4;

    }

    bool in_palette(int x, int y)

    {

        return palette_open_ && x >= x1_ && x <= palette_x2() && y >= palette_y1() && y <= palette_y2();

    }

public:

    std::function<void()> on_click;

    std::function<void(Srgb)> on_color_changed;

    TColorButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TColorButton";

        text_ = "Color";

    }

    ~TColorButton() {}

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

        return visible_ && ((x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_) || in_palette(x, y));

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

    Srgb get_color()

    {

        return color_;

    }

    void set_color(Srgb c)

    {

        color_ = c;

        if (on_color_changed)

        {

            on_color_changed(color_);

        }

    }

    bool palette_open()

    {

        return palette_open_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        if (!enabled_)

        {

            st.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        if (pressed_)

        {

            unsigned long swap = st.fillColor_;

            st.fillColor_ = st.outlineColor_;

            st.outlineColor_ = swap;

        }

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        // The colour swatch on the left of the button face.

        int sw = 22;

        int sx1 = x1_ + 5;

        int sy1 = y1_ + 4;

        int sy2 = y2_ - 4;

        XSetForeground(display_, gc_, styles_->get_color_Srgb(color_));

        XFillRectangle(display_, window_, gc_, sx1, sy1, sw, sy2 - sy1);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, sx1, sy1, sw, sy2 - sy1);

        xf::draw_text_left(display_, window_, gc_, st, sx1 + sw + 6, y1_, y2_, text_);

        xf::draw_triangle_down(display_, window_, gc_, st.text_color_, x2_ - 11, y1_ + height() / 2 - 3, 4);

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 3, y1_ + 3, width() - 6, height() - 6);

        }

        if (palette_open_)

        {

            int px2 = palette_x2();

            int py1 = palette_y1();

            int py2 = palette_y2();

            XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

            XFillRectangle(display_, window_, gc_, x1_, py1, px2 - x1_, py2 - py1);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_, py1, px2 - x1_ - 1, py2 - py1 - 1);

            for (int row = 0; row < palette_rows_; ++row)

            {

                for (int col = 0; col < palette_cols_; ++col)

                {

                    int i = row * palette_cols_ + col;

                    int cx = x1_ + 3 + col * (cell_ + 2);

                    int cy = py1 + 3 + row * (cell_ + 2);

                    XSetForeground(display_, gc_, styles_->get_color_Srgb(palette_[i]));

                    XFillRectangle(display_, window_, gc_, cx, cy, cell_, cell_);

                    bool current = palette_[i].r == color_.r && palette_[i].g == color_.g && palette_[i].b == color_.b;

                    XSetForeground(display_, gc_, current ? st.outlineColor_ : styles_->get_color_rgb(160, 160, 160));

                    XDrawRectangle(display_, window_, gc_, cx, cy, cell_, cell_);

                }

            }

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!visible_ || !enabled_ || button != 1)

        {

            return false;

        }

        if (in_palette(x, y))

        {

            int col = (x - x1_ - 3) / (cell_ + 2);

            int row = (y - palette_y1() - 3) / (cell_ + 2);

            if (col >= 0 && col < palette_cols_ && row >= 0 && row < palette_rows_)

            {

                set_color(palette_[row * palette_cols_ + col]);

            }

            palette_open_ = false;

            return true;

        }

        if (x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_)

        {

            pressed_ = true;

            focused_ = true;

            palette_open_ = !palette_open_;

            if (on_click)

            {

                on_click();

            }

            return true;

        }

        palette_open_ = false;

        return false;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button != 1 || !pressed_)

        {

            return false;

        }

        pressed_ = false;

        return true;

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

        if (key == XK_Return || key == XK_space)

        {

            palette_open_ = !palette_open_;

            return true;

        }

        if (key == XK_Escape)

        {

            palette_open_ = false;

            return true;

        }

        return false;

    }

};

//endregion
