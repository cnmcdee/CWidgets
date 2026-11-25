#pragma once

#include "core/xframe_core.h"

//region TFlowPanel

enum TFlowStyle

{

    fsLeftRightTopBottom,

    fsRightLeftTopBottom,

    fsLeftRightBottomTop,

    fsRightLeftBottomTop,

    fsTopBottomLeftRight,

    fsTopBottomRightLeft,

    fsBottomTopLeftRight,

    fsBottomTopRightLeft

};

class TFlowPanel

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

    TFlowStyle flow_style_ = fsLeftRightTopBottom;

    int control_count_ = 6;

    int tile_width_ = 90;

    int tile_height_ = 30;

    int spacing_h_ = 4;

    int spacing_v_ = 4;

    void decode_flow(bool& horizontal, bool& reverse_x, bool& reverse_y)

    {

        horizontal = true;

        reverse_x = false;

        reverse_y = false;

        switch (flow_style_)

        {

            case fsLeftRightTopBottom: horizontal = true; reverse_x = false; reverse_y = false; break;

            case fsRightLeftTopBottom: horizontal = true; reverse_x = true; reverse_y = false; break;

            case fsLeftRightBottomTop: horizontal = true; reverse_x = false; reverse_y = true; break;

            case fsRightLeftBottomTop: horizontal = true; reverse_x = true; reverse_y = true; break;

            case fsTopBottomLeftRight: horizontal = false; reverse_x = false; reverse_y = false; break;

            case fsTopBottomRightLeft: horizontal = false; reverse_x = true; reverse_y = false; break;

            case fsBottomTopLeftRight: horizontal = false; reverse_x = false; reverse_y = true; break;

            case fsBottomTopRightLeft: horizontal = false; reverse_x = true; reverse_y = true; break;

        }

    }

public:

    TFlowPanel(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TFlowPanel";

        text_ = "FlowPanel";

    }

    ~TFlowPanel() {}

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

    // Geometry: simulate the flow with wrapping at the panel bounds.
    // For horizontal styles tiles run along a row and wrap to the next
    // row; for vertical styles they run down a column and wrap to the
    // next column. Reverse flags flip the origin on that axis.

    bool get_tile_rect(int i, int& tx1, int& ty1, int& tx2, int& ty2)

    {

        if (i < 0 || i >= control_count_)

        {

            return false;

        }

        bool horizontal = true;

        bool reverse_x = false;

        bool reverse_y = false;

        decode_flow(horizontal, reverse_x, reverse_y);

        int pad = 4;

        int inner_w = width() - 2 * pad;

        int inner_h = height() - 2 * pad;

        int step_x = tile_width_ + spacing_h_;

        int step_y = tile_height_ + spacing_v_;

        int per_line = 1;

        int line = 0;

        int slot = 0;

        if (horizontal)

        {

            per_line = step_x > 0 ? (inner_w + spacing_h_) / step_x : 1;

            if (per_line < 1)

            {

                per_line = 1;

            }

            line = i / per_line;

            slot = i % per_line;

            int lx = pad + slot * step_x;

            int ly = pad + line * step_y;

            if (reverse_x)

            {

                lx = width() - pad - tile_width_ - slot * step_x;

            }

            if (reverse_y)

            {

                ly = height() - pad - tile_height_ - line * step_y;

            }

            tx1 = x1_ + lx;

            ty1 = y1_ + ly;

        }

        else

        {

            per_line = step_y > 0 ? (inner_h + spacing_v_) / step_y : 1;

            if (per_line < 1)

            {

                per_line = 1;

            }

            line = i / per_line;

            slot = i % per_line;

            int lx = pad + line * step_x;

            int ly = pad + slot * step_y;

            if (reverse_x)

            {

                lx = width() - pad - tile_width_ - line * step_x;

            }

            if (reverse_y)

            {

                ly = height() - pad - tile_height_ - slot * step_y;

            }

            tx1 = x1_ + lx;

            ty1 = y1_ + ly;

        }

        tx2 = tx1 + tile_width_;

        ty2 = ty1 + tile_height_;

        return true;

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

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        for (int i = 0; i < control_count_; ++i)

        {

            int tx1 = 0;

            int ty1 = 0;

            int tx2 = 0;

            int ty2 = 0;

            if (!get_tile_rect(i, tx1, ty1, tx2, ty2))

            {

                continue;

            }

            if (tx1 > x2_ || ty1 > y2_ || tx2 < x1_ || ty2 < y1_)

            {

                continue;

            }

            xf::draw_polished(display_, window_, gc_, st, tx1, ty1, tx2, ty2);

            xf::draw_text_centered(display_, window_, gc_, st, tx1, ty1, tx2, ty2, "Tile " + std::to_string(i));

        }

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, height() - 4);

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

    //region TFlowPanel API

    void set_control_count(int n)

    {

        control_count_ = n < 0 ? 0 : n;

    }

    int get_control_count()

    {

        return control_count_;

    }

    void set_tile_size(int w, int h)

    {

        tile_width_ = w < 8 ? 8 : w;

        tile_height_ = h < 8 ? 8 : h;

    }

    int get_tile_width()

    {

        return tile_width_;

    }

    int get_tile_height()

    {

        return tile_height_;

    }

    void set_spacing(int h, int v)

    {

        spacing_h_ = h < 0 ? 0 : h;

        spacing_v_ = v < 0 ? 0 : v;

    }

    void set_flow_style(TFlowStyle s)

    {

        flow_style_ = s;

    }

    TFlowStyle get_flow_style()

    {

        return flow_style_;

    }

    //endregion

};

//endregion
