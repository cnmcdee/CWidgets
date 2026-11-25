#pragma once

#include "core/xframe_core.h"

//region TArrow

class TArrow

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

    // Lazarus TArrowType: atUp, atDown, atLeft, atRight.

    int arrow_type_ = 3;   // atRight

    // Lazarus TShadowType: stNone, stSingle, stDouble.

    int shadow_type_ = 1;   // stSingle

    // The arrow outline as a polygon, for one direction, in a
    // unit box; draw() scales it into the widget bounds.

    std::vector<XPoint> arrow_points(int type, int ox, int oy, int w, int h)

    {

        // Unit coordinates (0..100) for a block arrow.

        static const int up[7][2] = {{50, 2}, {98, 55}, {70, 55}, {70, 98}, {30, 98}, {30, 55}, {2, 55}};

        static const int dn[7][2] = {{50, 98}, {2, 45}, {30, 45}, {30, 2}, {70, 2}, {70, 45}, {98, 45}};

        static const int lf[7][2] = {{2, 50}, {55, 2}, {55, 30}, {98, 30}, {98, 70}, {55, 70}, {55, 98}};

        static const int rt[7][2] = {{98, 50}, {45, 98}, {45, 70}, {2, 70}, {2, 30}, {45, 30}, {45, 2}};

        const int (*src)[2] = (type == 0) ? up : (type == 1) ? dn : (type == 2) ? lf : rt;

        std::vector<XPoint> pts;

        for (int i = 0; i < 7; ++i)

        {

            XPoint p;

            p.x = (short)(ox + src[i][0] * w / 100);

            p.y = (short)(oy + src[i][1] * h / 100);

            pts.push_back(p);

        }

        return pts;

    }

    void stroke_arrow(int type, int ox, int oy, unsigned long color, int line_width)

    {

        std::vector<XPoint> pts = arrow_points(type, ox, oy, width() - 4, height() - 4);

        pts.push_back(pts.front());

        XSetForeground(display_, gc_, color);

        xf::set_line_width(display_, gc_, line_width);

        XDrawLines(display_, window_, gc_, pts.data(), (int)pts.size(), CoordModeOrigin);

        xf::set_line_width(display_, gc_, 1);

    }

public:

    std::function<void()> on_click;

    TArrow(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TArrow";

        text_ = "TArrow";

    }

    ~TArrow() {}

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

    void set_arrow_type(int t)

    {

        arrow_type_ = t < 0 ? 0 : (t > 3 ? 3 : t);

    }

    int get_arrow_type()

    {

        return arrow_type_;

    }

    void set_shadow_type(int t)

    {

        shadow_type_ = t < 0 ? 0 : (t > 2 ? 2 : t);

    }

    int get_shadow_type()

    {

        return shadow_type_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        unsigned long main_color = enabled_ ? st.lineColor_ : styles_->get_color_rgb(140, 140, 140);

        unsigned long shadow_color = styles_->get_color_rgb(170, 170, 170);

        int lw = std::max(1, (int)st.lineWidth_);

        // Shadow pass(es) first, offset down-right.

        if (shadow_type_ >= 2)

        {

            stroke_arrow(arrow_type_, x1_ + 4, y1_ + 4, shadow_color, lw);

        }

        if (shadow_type_ >= 1)

        {

            stroke_arrow(arrow_type_, x1_ + 3, y1_ + 3, shadow_color, lw);

        }

        stroke_arrow(arrow_type_, x1_ + 2, y1_ + 2, main_color, lw + 1);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        if (on_click)

        {

            on_click();

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

        // Arrow keys turn the arrow, like setting ArrowType at run time.

        if (key == XK_Up)

        {

            arrow_type_ = 0;

            return true;

        }

        if (key == XK_Down)

        {

            arrow_type_ = 1;

            return true;

        }

        if (key == XK_Left)

        {

            arrow_type_ = 2;

            return true;

        }

        if (key == XK_Right)

        {

            arrow_type_ = 3;

            return true;

        }

        return false;

    }

};

//endregion
