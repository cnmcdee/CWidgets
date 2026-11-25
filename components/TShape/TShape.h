#pragma once

#include "core/xframe_core.h"

//region TShape

enum TShapeType

{

    stRectangle,

    stSquare,

    stRoundRect,

    stRoundSquare,

    stEllipse,

    stCircle,

    stTriangle,

    stDiamond,

    stPentagon,

    stHexagon

};

class TShape

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

    TShapeType shape_ = stEllipse;

    Srgb brush_color_ = {70, 130, 200};

    Srgb pen_color_ = {20, 40, 80};

    int pen_width_ = 2;

    // The box the shape is actually drawn in: the square / circle
    // variants constrain to the smaller dimension, centred.

    void shape_box(int& bx1, int& by1, int& bx2, int& by2)

    {

        bx1 = x1_;

        by1 = y1_;

        bx2 = x2_;

        by2 = y2_;

        if (shape_ == stSquare || shape_ == stRoundSquare || shape_ == stCircle)

        {

            int side = std::min(width(), height());

            int cx = (x1_ + x2_) / 2;

            int cy = (y1_ + y2_) / 2;

            bx1 = cx - side / 2;

            by1 = cy - side / 2;

            bx2 = bx1 + side;

            by2 = by1 + side;

        }

    }

    void fill_polygon(XPoint* points, int count)

    {

        XSetForeground(display_, gc_, styles_->get_color_Srgb(brush_color_));

        XFillPolygon(display_, window_, gc_, points, count, Complex, CoordModeOrigin);

        XSetForeground(display_, gc_, styles_->get_color_Srgb(pen_color_));

        xf::set_line_width(display_, gc_, pen_width_);

        XDrawLines(display_, window_, gc_, points, count, CoordModeOrigin);

        XDrawLine(display_, window_, gc_, points[count - 1].x, points[count - 1].y, points[0].x, points[0].y);

        xf::set_line_width(display_, gc_, 1);

    }

public:

    TShape(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TShape";

        text_ = "TShape";

    }

    ~TShape() {}

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

    void set_shape(TShapeType s)

    {

        shape_ = s;

    }

    TShapeType get_shape()

    {

        return shape_;

    }

    void set_brush_color(Srgb c)

    {

        brush_color_ = c;

    }

    Srgb get_brush_color()

    {

        return brush_color_;

    }

    void set_pen_color(Srgb c)

    {

        pen_color_ = c;

    }

    Srgb get_pen_color()

    {

        return pen_color_;

    }

    void set_pen_width(int w)

    {

        pen_width_ = std::max(1, w);

    }

    int get_pen_width()

    {

        return pen_width_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        int bx1, by1, bx2, by2;

        shape_box(bx1, by1, bx2, by2);

        int bw = bx2 - bx1;

        int bh = by2 - by1;

        unsigned long brush = styles_->get_color_Srgb(brush_color_);

        unsigned long pen = styles_->get_color_Srgb(pen_color_);

        if (shape_ == stRectangle || shape_ == stSquare)

        {

            XSetForeground(display_, gc_, brush);

            XFillRectangle(display_, window_, gc_, bx1, by1, bw, bh);

            XSetForeground(display_, gc_, pen);

            xf::set_line_width(display_, gc_, pen_width_);

            XDrawRectangle(display_, window_, gc_, bx1, by1, bw, bh);

            xf::set_line_width(display_, gc_, 1);

        }

        else if (shape_ == stRoundRect || shape_ == stRoundSquare)

        {

            SStyle st = style();

            st.fillColor_ = brush;

            st.lineColor_ = pen;

            st.lineWidth_ = (unsigned long)pen_width_;

            if ((int)st.radius_ <= 0)

            {

                st.radius_ = 10;

            }

            xf::fill_rounded_rect(display_, window_, gc_, st, bx1, by1, bx2, by2);

            xf::draw_rounded_outline(display_, window_, gc_, st, bx1, by1, bx2, by2);

        }

        else if (shape_ == stEllipse || shape_ == stCircle)

        {

            XSetForeground(display_, gc_, brush);

            XFillArc(display_, window_, gc_, bx1, by1, bw, bh, 0, 360 * 64);

            XSetForeground(display_, gc_, pen);

            xf::set_line_width(display_, gc_, pen_width_);

            XDrawArc(display_, window_, gc_, bx1, by1, bw, bh, 0, 360 * 64);

            xf::set_line_width(display_, gc_, 1);

        }

        else if (shape_ == stTriangle)

        {

            XPoint points[3];

            points[0] = {(short)((bx1 + bx2) / 2), (short)by1};

            points[1] = {(short)bx2, (short)by2};

            points[2] = {(short)bx1, (short)by2};

            fill_polygon(points, 3);

        }

        else if (shape_ == stDiamond)

        {

            XPoint points[4];

            points[0] = {(short)((bx1 + bx2) / 2), (short)by1};

            points[1] = {(short)bx2, (short)((by1 + by2) / 2)};

            points[2] = {(short)((bx1 + bx2) / 2), (short)by2};

            points[3] = {(short)bx1, (short)((by1 + by2) / 2)};

            fill_polygon(points, 4);

        }

        else if (shape_ == stPentagon)

        {

            XPoint points[5];

            points[0] = {(short)(bx1 + bw * 50 / 100), (short)by1};

            points[1] = {(short)bx2, (short)(by1 + bh * 38 / 100)};

            points[2] = {(short)(bx1 + bw * 81 / 100), (short)by2};

            points[3] = {(short)(bx1 + bw * 19 / 100), (short)by2};

            points[4] = {(short)bx1, (short)(by1 + bh * 38 / 100)};

            fill_polygon(points, 5);

        }

        else if (shape_ == stHexagon)

        {

            XPoint points[6];

            points[0] = {(short)(bx1 + bw * 25 / 100), (short)by1};

            points[1] = {(short)(bx1 + bw * 75 / 100), (short)by1};

            points[2] = {(short)bx2, (short)((by1 + by2) / 2)};

            points[3] = {(short)(bx1 + bw * 75 / 100), (short)by2};

            points[4] = {(short)(bx1 + bw * 25 / 100), (short)by2};

            points[5] = {(short)bx1, (short)((by1 + by2) / 2)};

            fill_polygon(points, 6);

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

};

//endregion
