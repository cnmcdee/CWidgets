#pragma once

#include "core/xframe_core.h"

//region TShellTreeView

class TShellTreeView

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

    struct SNode

    {

        std::string label;

        std::string path;

        int depth;

        bool expanded;

        bool has_children;

    };

    std::vector<SNode> nodes_

    {

        {"/", "/", 0, true, true},

        {"home", "/home", 1, true, true},

        {"user", "/home/user", 2, true, true},

        {"Documents", "/home/user/Documents", 3, false, true},

        {"2026", "/home/user/Documents/2026", 4, false, false},

        {"Pictures", "/home/user/Pictures", 3, false, false},

        {"Downloads", "/home/user/Downloads", 3, false, false},

        {"etc", "/etc", 1, false, true},

        {"systemd", "/etc/systemd", 2, false, false},

        {"usr", "/usr", 1, false, true},

        {"bin", "/usr/bin", 2, false, false},

        {"var", "/var", 1, false, false}

    };

    int selected_ = 2;   // /home/user

    int top_row_ = 0;

    int row_h_ = 18;

    // Indices of the nodes currently visible (collapsed subtrees
    // are skipped), recomputed on demand.

    std::vector<int> visible_nodes()

    {

        std::vector<int> out;

        int hidden_below = -1;

        for (int i = 0; i < (int)nodes_.size(); ++i)

        {

            if (hidden_below >= 0 && nodes_[i].depth > hidden_below)

            {

                continue;

            }

            hidden_below = -1;

            out.push_back(i);

            if (nodes_[i].has_children && !nodes_[i].expanded)

            {

                hidden_below = nodes_[i].depth;

            }

        }

        return out;

    }

    int visible_rows()

    {

        return std::max(1, height() / row_h_);

    }

public:

    std::function<void()> on_click;

    std::function<void()> on_change;

    TShellTreeView(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TShellTreeView";

        text_ = nodes_[selected_].path;

    }

    ~TShellTreeView() {}

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

        set_path(t);

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    std::string get_path()

    {

        if (selected_ >= 0 && selected_ < (int)nodes_.size())

        {

            return nodes_[selected_].path;

        }

        return "/";

    }

    void set_path(std::string p)

    {

        for (int i = 0; i < (int)nodes_.size(); ++i)

        {

            if (nodes_[i].path == p)

            {

                selected_ = i;

                text_ = p;

                return;

            }

        }

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

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        std::vector<int> vis = visible_nodes();

        int rows = visible_rows();

        for (int r = 0; r < rows; ++r)

        {

            int vi = top_row_ + r;

            if (vi >= (int)vis.size())

            {

                break;

            }

            int i = vis[vi];

            SNode& n = nodes_[i];

            int ry = y1_ + r * row_h_;

            if (i == selected_)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, x1_ + 1, ry, width() - 2, row_h_);

            }

            SStyle rs = st;

            if (i == selected_)

            {

                rs.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            int indent = x1_ + 4 + n.depth * 14;

            // Expander box for parents.

            if (n.has_children)

            {

                XSetForeground(display_, gc_, st.lineColor_);

                XDrawRectangle(display_, window_, gc_, indent, ry + 4, 9, 9);

                XDrawLine(display_, window_, gc_, indent + 2, ry + 8, indent + 7, ry + 8);

                if (!n.expanded)

                {

                    XDrawLine(display_, window_, gc_, indent + 4, ry + 6, indent + 4, ry + 11);

                }

            }

            // Folder glyph.

            int fx = indent + 13;

            int fy = ry + 5;

            XSetForeground(display_, gc_, styles_->get_color_rgb(240, 200, 90));

            XFillRectangle(display_, window_, gc_, fx, fy + 2, 13, 8);

            XFillRectangle(display_, window_, gc_, fx, fy, 6, 3);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, fx, fy + 2, 13, 8);

            xf::draw_text_left(display_, window_, gc_, rs, fx + 17, ry, ry + row_h_, n.label);

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

        if (!contains(x, y) || !enabled_)

        {

            return false;

        }

        focused_ = true;

        std::vector<int> vis = visible_nodes();

        if (button == 4)

        {

            if (top_row_ > 0)

            {

                --top_row_;

            }

            return true;

        }

        if (button == 5)

        {

            if (top_row_ + visible_rows() < (int)vis.size())

            {

                ++top_row_;

            }

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        int vi = top_row_ + (y - y1_) / row_h_;

        if (vi < 0 || vi >= (int)vis.size())

        {

            return true;

        }

        int i = vis[vi];

        SNode& n = nodes_[i];

        int indent = x1_ + 4 + n.depth * 14;

        if (n.has_children && x >= indent && x <= indent + 10)

        {

            n.expanded = !n.expanded;

            return true;

        }

        if (i != selected_)

        {

            selected_ = i;

            text_ = n.path;

            if (on_change)

            {

                on_change();

            }

        }

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

        std::vector<int> vis = visible_nodes();

        int pos = -1;

        for (int i = 0; i < (int)vis.size(); ++i)

        {

            if (vis[i] == selected_)

            {

                pos = i;

            }

        }

        if (key == XK_Up && pos > 0)

        {

            selected_ = vis[pos - 1];

        }

        else if (key == XK_Down && pos >= 0 && pos < (int)vis.size() - 1)

        {

            selected_ = vis[pos + 1];

        }

        else if ((key == XK_Right || key == XK_plus) && selected_ >= 0 && nodes_[selected_].has_children)

        {

            nodes_[selected_].expanded = true;

            return true;

        }

        else if ((key == XK_Left || key == XK_minus) && selected_ >= 0 && nodes_[selected_].has_children)

        {

            nodes_[selected_].expanded = false;

            return true;

        }

        else

        {

            return false;

        }

        text_ = get_path();

        if (on_change)

        {

            on_change();

        }

        return true;

    }

};

//endregion
