#pragma once

#include "core/xframe_core.h"

//region TTreeNode

struct TTreeNode

{

    std::string text;

    std::vector<TTreeNode> children;

    bool expanded = true;

};

//endregion

//region TTreeView

class TTreeView

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

    std::vector<TTreeNode> roots_;

    struct VisibleRow

    {

        TTreeNode* node;

        int level;

    };

    std::vector<VisibleRow> rows_;   // flattened view of expanded nodes

    int selected_row_ = 0;

    int top_row_ = 0;

    int row_h_ = 18;

    int indent_ = 16;

    void append_row(TTreeNode* node, int level)

    {

        VisibleRow row;

        row.node = node;

        row.level = level;

        rows_.push_back(row);

        if (node->expanded)

        {

            for (auto& child : node->children)

            {

                append_row(&child, level + 1);

            }

        }

    }

    void rebuild_rows()

    {

        rows_.clear();

        for (auto& root : roots_)

        {

            append_row(&root, 0);

        }

        if (selected_row_ >= (int)rows_.size())

        {

            selected_row_ = (int)rows_.size() - 1;

        }

        if (selected_row_ < 0)

        {

            selected_row_ = 0;

        }

    }

    int visible_row_count()

    {

        return (height() - 4) / row_h_;

    }

    void keep_selection_visible()

    {

        int count = visible_row_count();

        if (selected_row_ < top_row_)

        {

            top_row_ = selected_row_;

        }

        if (selected_row_ >= top_row_ + count)

        {

            top_row_ = selected_row_ - count + 1;

        }

        if (top_row_ < 0)

        {

            top_row_ = 0;

        }

    }

    int row_top(int row)

    {

        return y1_ + 2 + (row - top_row_) * row_h_;

    }

    int toggle_box_x(int level)

    {

        return x1_ + 4 + level * indent_;

    }

public:

    std::function<void()> on_change;

    TTreeView(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TTreeView";

        text_ = "TTreeView";

        // Preload a sample project tree.

        TTreeNode project;

        project.text = "Project1";

        TTreeNode unit1;

        unit1.text = "Unit1.pas";

        TTreeNode unit2;

        unit2.text = "Unit2.pas";

        project.children.push_back(unit1);

        project.children.push_back(unit2);

        TTreeNode forms;

        forms.text = "Forms";

        TTreeNode main_form;

        main_form.text = "MainForm";

        TTreeNode about;

        about.text = "AboutBox";

        forms.children.push_back(main_form);

        forms.children.push_back(about);

        project.children.push_back(forms);

        TTreeNode data;

        data.text = "DataModule";

        roots_.push_back(project);

        roots_.push_back(data);

        rebuild_rows();

    }

    ~TTreeView() {}

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

    TTreeNode& add_root(std::string caption)

    {

        TTreeNode node;

        node.text = caption;

        roots_.push_back(node);

        rebuild_rows();

        return roots_.back();

    }

    void clear()

    {

        roots_.clear();

        rebuild_rows();

    }

    int get_selected_row()

    {

        return selected_row_;

    }

    std::string get_selected_text()

    {

        if (selected_row_ >= 0 && selected_row_ < (int)rows_.size())

        {

            return rows_[selected_row_].node->text;

        }

        return "";

    }

    int get_count()

    {

        return (int)rows_.size();

    }

    void full_expand()

    {

        std::function<void(TTreeNode&)> walk = [&](TTreeNode& n)

        {

            n.expanded = true;

            for (auto& c : n.children)

            {

                walk(c);

            }

        };

        for (auto& r : roots_)

        {

            walk(r);

        }

        rebuild_rows();

    }

    void full_collapse()

    {

        std::function<void(TTreeNode&)> walk = [&](TTreeNode& n)

        {

            n.expanded = false;

            for (auto& c : n.children)

            {

                walk(c);

            }

        };

        for (auto& r : roots_)

        {

            walk(r);

        }

        rebuild_rows();

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        unsigned long white = styles_->get_color_rgb(255, 255, 255);

        unsigned long highlight = styles_->get_color_rgb(180, 205, 240);

        unsigned long guide = styles_->get_color_rgb(190, 190, 190);

        XSetForeground(display_, gc_, white);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, (int)st.lineWidth_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        int count = visible_row_count();

        for (int i = 0; i < count; ++i)

        {

            int row = top_row_ + i;

            if (row < 0 || row >= (int)rows_.size())

            {

                break;

            }

            VisibleRow vr = rows_[row];

            int top = row_top(row);

            if (row == selected_row_)

            {

                XSetForeground(display_, gc_, highlight);

                XFillRectangle(display_, window_, gc_, x1_ + 2, top, width() - 4, row_h_);

            }

            int bx = toggle_box_x(vr.level);

            // Dotted indent guide down to the toggle box.

            XSetForeground(display_, gc_, guide);

            XDrawLine(display_, window_, gc_, bx + 5, top, bx + 5, top + row_h_ / 2);

            if (!vr.node->children.empty())

            {

                // The + / - box.

                XSetForeground(display_, gc_, white);

                XFillRectangle(display_, window_, gc_, bx, top + 4, 10, 10);

                XSetForeground(display_, gc_, st.lineColor_);

                XDrawRectangle(display_, window_, gc_, bx, top + 4, 10, 10);

                XDrawLine(display_, window_, gc_, bx + 2, top + 9, bx + 8, top + 9);

                if (!vr.node->expanded)

                {

                    XDrawLine(display_, window_, gc_, bx + 5, top + 6, bx + 5, top + 12);

                }

            }

            SStyle ts = st;

            if (!enabled_)

            {

                ts.text_color_ = styles_->get_color_rgb(140, 140, 140);

            }

            xf::draw_text_left(display_, window_, gc_, ts, bx + 15, top, top + row_h_, vr.node->text);

        }

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 3, height() - 3);

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

        int row = top_row_ + (y - y1_ - 2) / row_h_;

        if (row < 0 || row >= (int)rows_.size())

        {

            return true;

        }

        VisibleRow vr = rows_[row];

        int bx = toggle_box_x(vr.level);

        int top = row_top(row);

        if (!vr.node->children.empty() && x >= bx && x <= bx + 10 && y >= top + 4 && y <= top + 14)

        {

            vr.node->expanded = !vr.node->expanded;

            rebuild_rows();

            selected_row_ = row;

            if (on_change)

            {

                on_change();

            }

            return true;

        }

        selected_row_ = row;

        if (on_change)

        {

            on_change();

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

        if (!focused_ || !enabled_ || rows_.empty())

        {

            return false;

        }

        if (key == XK_Up)

        {

            if (selected_row_ > 0)

            {

                --selected_row_;

            }

            keep_selection_visible();

            return true;

        }

        if (key == XK_Down)

        {

            if (selected_row_ < (int)rows_.size() - 1)

            {

                ++selected_row_;

            }

            keep_selection_visible();

            return true;

        }

        if (key == XK_Right)

        {

            TTreeNode* node = rows_[selected_row_].node;

            if (!node->children.empty() && !node->expanded)

            {

                node->expanded = true;

                rebuild_rows();

            }

            return true;

        }

        if (key == XK_Left)

        {

            TTreeNode* node = rows_[selected_row_].node;

            if (!node->children.empty() && node->expanded)

            {

                node->expanded = false;

                rebuild_rows();

            }

            return true;

        }

        return false;

    }

};

//endregion
