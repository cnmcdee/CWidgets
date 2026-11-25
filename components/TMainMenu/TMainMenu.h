#pragma once

#include "core/xframe_core.h"

//region TMainMenu

class TMainMenu

{

public:

    struct MenuEntry

    {

        std::string caption;

        std::vector<std::string> dropdown;   // "-" = separator

    };

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

    std::vector<MenuEntry> items_;

    int open_index_ = -1;

    int hover_index_ = -1;

    int dropdown_hover_ = -1;

    int row_height_ = 22;

    int item_left(int index)

    {

        SStyle st = style();

        int x = x1_ + 4;

        for (int i = 0; i < index && i < (int)items_.size(); ++i)

        {

            x += xf::text_width(st, items_[i].caption) + 20;

        }

        return x;

    }

    int item_width(int index)

    {

        SStyle st = style();

        return xf::text_width(st, items_[index].caption) + 20;

    }

    int top_index_at(int x, int y)

    {

        if (y < y1_ || y > y2_)

        {

            return -1;

        }

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            int lx = item_left(i);

            if (x >= lx && x <= lx + item_width(i))

            {

                return i;

            }

        }

        return -1;

    }

    int dropdown_width(int index)

    {

        SStyle st = style();

        int w = 90;

        for (auto& e : items_[index].dropdown)

        {

            if (e != "-")

            {

                int tw = xf::text_width(st, e) + 36;

                if (tw > w)

                {

                    w = tw;

                }

            }

        }

        return w;

    }

    int dropdown_height(int index)

    {

        return (int)items_[index].dropdown.size() * row_height_ + 8;

    }

    bool in_dropdown(int x, int y)

    {

        if (open_index_ < 0)

        {

            return false;

        }

        int dx = item_left(open_index_);

        int dy = y2_;

        return x >= dx && x <= dx + dropdown_width(open_index_) && y >= dy && y <= dy + dropdown_height(open_index_);

    }

    int dropdown_index_at(int x, int y)

    {

        if (!in_dropdown(x, y))

        {

            return -1;

        }

        int row = (y - y2_ - 4) / row_height_;

        if (row < 0 || row >= (int)items_[open_index_].dropdown.size())

        {

            return -1;

        }

        return row;

    }

public:

    std::function<void()> on_click;

    std::function<void(std::string)> on_menu_click;

    TMainMenu(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TMainMenu";

        text_ = "MainMenu";

        add_menu("File", { "New", "Open...", "Save", "Save As...", "-", "Exit" });

        add_menu("Edit", { "Undo", "Redo", "-", "Cut", "Copy", "Paste" });

        add_menu("View", { "Toolbar", "Status Bar", "-", "Zoom In", "Zoom Out" });

        add_menu("Help", { "Contents", "-", "About" });

    }

    ~TMainMenu() {}

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

        if (!visible_)

        {

            return false;

        }

        if (x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_)

        {

            return true;

        }

        return in_dropdown(x, y);

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

    void add_menu(std::string caption, std::vector<std::string> dropdown)

    {

        MenuEntry e;

        e.caption = caption;

        e.dropdown = dropdown;

        items_.push_back(e);

    }

    void add_dropdown_item(int menu_index, std::string caption)

    {

        if (menu_index >= 0 && menu_index < (int)items_.size())

        {

            items_[menu_index].dropdown.push_back(caption);

        }

    }

    void clear_menus()

    {

        items_.clear();

        open_index_ = -1;

    }

    int menu_count()

    {

        return (int)items_.size();

    }

    int get_open_index()

    {

        return open_index_;

    }

    void close_menu()

    {

        open_index_ = -1;

        dropdown_hover_ = -1;

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

        // Menu bar body.

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        // Top level items.

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            int lx = item_left(i);

            int lw = item_width(i);

            if (i == open_index_ || i == hover_index_)

            {

                SStyle hl = st;

                unsigned long swap = hl.fillColor_;

                hl.fillColor_ = hl.outlineColor_;

                hl.outlineColor_ = swap;

                xf::fill_rounded_rect(display_, window_, gc_, hl, lx, y1_ + 2, lx + lw, y2_ - 2);

            }

            xf::draw_text_centered(display_, window_, gc_, st, lx, y1_, lx + lw, y2_, items_[i].caption);

        }

        // Open dropdown panel.

        if (open_index_ >= 0 && open_index_ < (int)items_.size())

        {

            int dx = item_left(open_index_);

            int dy = y2_;

            int dw = dropdown_width(open_index_);

            int dh = dropdown_height(open_index_);

            xf::draw_polished(display_, window_, gc_, st, dx, dy, dx + dw, dy + dh);

            for (int r = 0; r < (int)items_[open_index_].dropdown.size(); ++r)

            {

                std::string entry = items_[open_index_].dropdown[r];

                int ry = dy + 4 + r * row_height_;

                if (entry == "-")

                {

                    XSetForeground(display_, gc_, st.lineColor_);

                    xf::set_line_width(display_, gc_, 1);

                    XDrawLine(display_, window_, gc_, dx + 6, ry + row_height_ / 2, dx + dw - 6, ry + row_height_ / 2);

                    continue;

                }

                if (r == dropdown_hover_)

                {

                    XSetForeground(display_, gc_, st.outlineColor_);

                    XFillRectangle(display_, window_, gc_, dx + 3, ry, dw - 6, row_height_);

                    SStyle hl = st;

                    hl.text_color_ = st.fillColor_;

                    xf::draw_text_left(display_, window_, gc_, hl, dx + 12, ry, ry + row_height_, entry);

                }

                else

                {

                    xf::draw_text_left(display_, window_, gc_, st, dx + 12, ry, ry + row_height_, entry);

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

        // Click on a dropdown entry fires and closes.

        if (open_index_ >= 0 && in_dropdown(x, y))

        {

            int row = dropdown_index_at(x, y);

            if (row >= 0)

            {

                std::string entry = items_[open_index_].dropdown[row];

                if (entry != "-")

                {

                    close_menu();

                    if (on_menu_click)

                    {

                        on_menu_click(entry);

                    }

                    if (on_click)

                    {

                        on_click();

                    }

                }

                return true;

            }

            return true;

        }

        int top = top_index_at(x, y);

        if (top >= 0)

        {

            focused_ = true;

            if (open_index_ == top)

            {

                close_menu();

            }

            else

            {

                open_index_ = top;

                dropdown_hover_ = -1;

            }

            return true;

        }

        // Clicking elsewhere closes an open menu.

        if (open_index_ >= 0)

        {

            close_menu();

            return true;

        }

        return false;

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

        if (!visible_)

        {

            return false;

        }

        int top = top_index_at(x, y);

        if (top >= 0)

        {

            hover_index_ = top;

            if (open_index_ >= 0 && open_index_ != top)

            {

                open_index_ = top;

                dropdown_hover_ = -1;

            }

            return true;

        }

        hover_index_ = -1;

        if (open_index_ >= 0 && in_dropdown(x, y))

        {

            dropdown_hover_ = dropdown_index_at(x, y);

            return true;

        }

        dropdown_hover_ = -1;

        return false;

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

            close_menu();

            return true;

        }

        if (open_index_ < 0)

        {

            return false;

        }

        if (key == XK_Right && open_index_ + 1 < (int)items_.size())

        {

            open_index_++;

            dropdown_hover_ = -1;

            return true;

        }

        if (key == XK_Left && open_index_ > 0)

        {

            open_index_--;

            dropdown_hover_ = -1;

            return true;

        }

        if (key == XK_Down)

        {

            int n = (int)items_[open_index_].dropdown.size();

            if (n > 0)

            {

                dropdown_hover_ = (dropdown_hover_ + 1) % n;

                if (items_[open_index_].dropdown[dropdown_hover_] == "-")

                {

                    dropdown_hover_ = (dropdown_hover_ + 1) % n;

                }

            }

            return true;

        }

        if (key == XK_Up)

        {

            int n = (int)items_[open_index_].dropdown.size();

            if (n > 0)

            {

                dropdown_hover_ = (dropdown_hover_ + n - 1) % n;

                if (items_[open_index_].dropdown[dropdown_hover_] == "-")

                {

                    dropdown_hover_ = (dropdown_hover_ + n - 1) % n;

                }

            }

            return true;

        }

        if ((key == XK_Return || key == XK_space) && dropdown_hover_ >= 0)

        {

            std::string entry = items_[open_index_].dropdown[dropdown_hover_];

            close_menu();

            if (entry != "-" && on_menu_click)

            {

                on_menu_click(entry);

            }

            return true;

        }

        return false;

    }

};

//endregion
