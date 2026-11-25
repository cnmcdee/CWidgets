#pragma once

#include "core/xframe_core.h"

//region TControlBar

class TControlBar

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

    struct Band

    {

        std::string caption;

        int width;

    };

    struct BandRect

    {

        int x1;

        int y1;

        int x2;

        int y2;

    };

    std::vector<Band> bands_;

    int row_height_ = 26;

    int gripper_width_ = 10;

    int dragging_band_ = -1;

    int drag_offset_x_ = 0;

    int drag_offset_y_ = 0;

    int drag_x_ = 0;

    int drag_y_ = 0;

    // Packing simulation shared by draw, hit-test and drop logic.
    // When skip >= 0 that band is left out (used while dragging).

    void layout_bands(int skip, std::vector<BandRect>& rects, std::vector<int>& order)

    {

        rects.clear();

        order.clear();

        int cx = x1_ + 2;

        int cy = y1_ + 2;

        for (int i = 0; i < (int)bands_.size(); ++i)

        {

            if (i == skip)

            {

                continue;

            }

            int bw = bands_[i].width;

            if (cx > x1_ + 2 && cx + bw > x2_ - 2)

            {

                cx = x1_ + 2;

                cy += row_height_ + 2;

            }

            BandRect r;

            r.x1 = cx;

            r.y1 = cy;

            r.x2 = cx + bw;

            r.y2 = cy + row_height_;

            rects.push_back(r);

            order.push_back(i);

            cx += bw + 4;

        }

    }

    int band_at(int x, int y, bool gripper_only, bool& in_gripper)

    {

        std::vector<BandRect> rects;

        std::vector<int> order;

        layout_bands(-1, rects, order);

        in_gripper = false;

        for (int k = 0; k < (int)order.size(); ++k)

        {

            BandRect r = rects[k];

            if (x >= r.x1 && x <= r.x2 && y >= r.y1 && y <= r.y2)

            {

                in_gripper = (x <= r.x1 + gripper_width_);

                if (gripper_only && !in_gripper)

                {

                    return -1;

                }

                return order[k];

            }

        }

        return -1;

    }

public:

    std::function<void()> on_band_move;

    TControlBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TControlBar";

        text_ = "ControlBar";

        bands_.push_back(Band{"File", 140});

        bands_.push_back(Band{"Edit", 120});

        bands_.push_back(Band{"View", 160});

    }

    ~TControlBar() {}

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

    bool band_rect(int i, int& bx1, int& by1, int& bx2, int& by2)

    {

        std::vector<BandRect> rects;

        std::vector<int> order;

        layout_bands(-1, rects, order);

        for (int k = 0; k < (int)order.size(); ++k)

        {

            if (order[k] == i)

            {

                bx1 = rects[k].x1;

                by1 = rects[k].y1;

                bx2 = rects[k].x2;

                by2 = rects[k].y2;

                return true;

            }

        }

        return false;

    }

    void draw_band(int bx1, int by1, int bx2, int by2, std::string caption, const SStyle& st)

    {

        xf::draw_polished(display_, window_, gc_, st, bx1, by1, bx2, by2);

        // Gripper: two vertical columns of 2px dots in a 10px zone.

        unsigned long dot = styles_->get_color_rgb(110, 110, 110);

        XSetForeground(display_, gc_, dot);

        for (int col = 0; col < 2; ++col)

        {

            for (int row = 0; row < 4; ++row)

            {

                int dx = bx1 + 3 + col * 4;

                int dy = by1 + 5 + row * 5;

                XFillRectangle(display_, window_, gc_, dx, dy, 2, 2);

            }

        }

        xf::draw_text_left(display_, window_, gc_, st, bx1 + gripper_width_ + 4, by1, by2, caption);

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

        std::vector<BandRect> rects;

        std::vector<int> order;

        layout_bands(dragging_band_, rects, order);

        for (int k = 0; k < (int)order.size(); ++k)

        {

            draw_band(rects[k].x1, rects[k].y1, rects[k].x2, rects[k].y2, bands_[order[k]].caption, st);

        }

        if (dragging_band_ >= 0 && dragging_band_ < (int)bands_.size())

        {

            int bw = bands_[dragging_band_].width;

            draw_band(drag_x_, drag_y_, drag_x_ + bw, drag_y_ + row_height_, bands_[dragging_band_].caption, st);

        }

        if (focused_ && dragging_band_ < 0)

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

        bool in_gripper = false;

        int band = band_at(x, y, false, in_gripper);

        if (band >= 0 && in_gripper)

        {

            dragging_band_ = band;

            int bx1 = 0;

            int by1 = 0;

            int bx2 = 0;

            int by2 = 0;

            band_rect(band, bx1, by1, bx2, by2);

            drag_offset_x_ = x - bx1;

            drag_offset_y_ = y - by1;

            drag_x_ = bx1;

            drag_y_ = by1;

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (button != 1 || dragging_band_ < 0)

        {

            return false;

        }

        int dragged = dragging_band_;

        dragging_band_ = -1;

        // Find the insertion slot: walk the other bands in packed
        // order and insert before the first whose centre is past
        // the release point (row first, then column).

        std::vector<BandRect> rects;

        std::vector<int> order;

        layout_bands(dragged, rects, order);

        int slot = (int)order.size();

        for (int k = 0; k < (int)order.size(); ++k)

        {

            int cx = (rects[k].x1 + rects[k].x2) / 2;

            int cy = (rects[k].y1 + rects[k].y2) / 2;

            if (y < cy - row_height_ / 2 || (y <= cy + row_height_ / 2 && x < cx))

            {

                slot = k;

                break;

            }

        }

        // order[slot] is the original index to insert before; convert
        // to a position in the shortened vector.

        int insert_at = slot;

        Band moved = bands_[dragged];

        bands_.erase(bands_.begin() + dragged);

        if (insert_at > dragged)

        {

            insert_at--;

        }

        if (insert_at < 0)

        {

            insert_at = 0;

        }

        if (insert_at > (int)bands_.size())

        {

            insert_at = (int)bands_.size();

        }

        bands_.insert(bands_.begin() + insert_at, moved);

        if (insert_at != dragged && on_band_move)

        {

            on_band_move();

        }

        return true;

    }

    bool on_motion(int x, int y)

    {

        if (dragging_band_ < 0)

        {

            return false;

        }

        drag_x_ = x - drag_offset_x_;

        drag_y_ = y - drag_offset_y_;

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)key;

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        return false;

    }

    //region TControlBar API

    void add_band(std::string caption, int band_width)

    {

        bands_.push_back(Band{caption, band_width < 20 ? 20 : band_width});

    }

    int band_count()

    {

        return (int)bands_.size();

    }

    std::string get_band_caption(int i)

    {

        if (i < 0 || i >= (int)bands_.size())

        {

            return "";

        }

        return bands_[i].caption;

    }

    int get_dragging_band()

    {

        return dragging_band_;

    }

    //endregion

};

//endregion
