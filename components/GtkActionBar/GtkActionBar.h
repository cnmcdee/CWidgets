#pragma once

#include "core/xframe_core.h"

//region GtkActionBar

class GtkActionBar

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

    std::vector<std::string> start_items_;

    std::vector<std::string> end_items_;

    std::string center_label_;

    int item_height_ = 26;

    int item_padding_ = 10;

    int spacing_ = 6;

    int pressed_zone_ = 0;   // 0 none, 1 start, 2 end

    int pressed_index_ = -1;

    int item_width(const SStyle& st, const std::string& label)

    {

        return xf::text_width(st, label) + item_padding_ * 2;

    }

public:

    std::function<void(std::string)> on_item_clicked;

    GtkActionBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkActionBar";

        text_ = "ActionBar";

        start_items_ = { "Open" };

        end_items_ = { "Save" };

        center_label_ = "";

    }

    ~GtkActionBar() {}

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

    void pack_start(std::string label)

    {

        start_items_.push_back(label);

    }

    void pack_end(std::string label)

    {

        end_items_.push_back(label);

    }

    void set_center_label(std::string label)

    {

        center_label_ = label;

    }

    std::string get_center_label()

    {

        return center_label_;

    }

    bool start_rect(int i, int& rx1, int& ry1, int& rx2, int& ry2)

    {

        if (i < 0 || i >= (int)start_items_.size())

        {

            return false;

        }

        SStyle st = style();

        int cx = x1_ + 6;

        for (int k = 0; k < i; ++k)

        {

            cx += item_width(st, start_items_[k]) + spacing_;

        }

        rx1 = cx;

        rx2 = cx + item_width(st, start_items_[i]);

        ry1 = y1_ + (height() - item_height_) / 2;

        ry2 = ry1 + item_height_;

        return true;

    }

    bool end_rect(int i, int& rx1, int& ry1, int& rx2, int& ry2)

    {

        if (i < 0 || i >= (int)end_items_.size())

        {

            return false;

        }

        SStyle st = style();

        int cx = x2_ - 6;

        for (int k = (int)end_items_.size() - 1; k > i; --k)

        {

            cx -= item_width(st, end_items_[k]) + spacing_;

        }

        rx2 = cx;

        rx1 = cx - item_width(st, end_items_[i]);

        ry1 = y1_ + (height() - item_height_) / 2;

        ry2 = ry1 + item_height_;

        return true;

    }

    void center_rect(int& rx1, int& ry1, int& rx2, int& ry2)

    {

        SStyle st = style();

        int w = center_label_.empty() ? 0 : xf::text_width(st, center_label_) + item_padding_ * 2;

        rx1 = x1_ + (width() - w) / 2;

        rx2 = rx1 + w;

        ry1 = y1_ + (height() - item_height_) / 2;

        ry2 = ry1 + item_height_;

    }

    // Hit test: zone 1 = start, 2 = end, 0 = none; index set on a hit.

    int zone_at(int x, int y, int& index)

    {

        index = -1;

        for (int i = 0; i < (int)start_items_.size(); ++i)

        {

            int rx1, ry1, rx2, ry2;

            if (start_rect(i, rx1, ry1, rx2, ry2) && x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2)

            {

                index = i;

                return 1;

            }

        }

        for (int i = 0; i < (int)end_items_.size(); ++i)

        {

            int rx1, ry1, rx2, ry2;

            if (end_rect(i, rx1, ry1, rx2, ry2) && x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2)

            {

                index = i;

                return 2;

            }

        }

        return 0;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        // Separator line along the top edge of the bar.

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, std::max(1, (int)st.lineWidth_));

        XDrawLine(display_, window_, gc_, x1_, y1_, x2_, y1_);

        for (int i = 0; i < (int)start_items_.size(); ++i)

        {

            int rx1, ry1, rx2, ry2;

            if (!start_rect(i, rx1, ry1, rx2, ry2))

            {

                continue;

            }

            SStyle item = st;

            if (!enabled_)

            {

                item.text_color_ = styles_->get_color_rgb(140, 140, 140);

            }

            if (pressed_zone_ == 1 && pressed_index_ == i)

            {

                unsigned long swap = item.fillColor_;

                item.fillColor_ = item.outlineColor_;

                item.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, item, rx1, ry1, rx2, ry2);

            xf::draw_text_centered(display_, window_, gc_, item, rx1, ry1, rx2, ry2, start_items_[i]);

        }

        for (int i = 0; i < (int)end_items_.size(); ++i)

        {

            int rx1, ry1, rx2, ry2;

            if (!end_rect(i, rx1, ry1, rx2, ry2))

            {

                continue;

            }

            SStyle item = st;

            if (!enabled_)

            {

                item.text_color_ = styles_->get_color_rgb(140, 140, 140);

            }

            if (pressed_zone_ == 2 && pressed_index_ == i)

            {

                unsigned long swap = item.fillColor_;

                item.fillColor_ = item.outlineColor_;

                item.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, item, rx1, ry1, rx2, ry2);

            xf::draw_text_centered(display_, window_, gc_, item, rx1, ry1, rx2, ry2, end_items_[i]);

        }

        if (!center_label_.empty())

        {

            int rx1, ry1, rx2, ry2;

            center_rect(rx1, ry1, rx2, ry2);

            xf::draw_text_centered(display_, window_, gc_, st, rx1, ry1, rx2, ry2, center_label_);

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

        int index = -1;

        int zone = zone_at(x, y, index);

        if (zone == 0)

        {

            return true;

        }

        pressed_zone_ = zone;

        pressed_index_ = index;

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (pressed_zone_ == 0 || button != 1)

        {

            return false;

        }

        int zone = pressed_zone_;

        int index = pressed_index_;

        pressed_zone_ = 0;

        pressed_index_ = -1;

        int hit_index = -1;

        if (zone_at(x, y, hit_index) == zone && hit_index == index && on_item_clicked)

        {

            on_item_clicked(zone == 1 ? start_items_[index] : end_items_[index]);

        }

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

        (void)key;

        (void)str;

        return false;

    }

};

//endregion
