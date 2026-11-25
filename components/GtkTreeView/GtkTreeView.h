#pragma once

#include "core/xframe_core.h"

//region GtkTreeView

class GtkTreeView

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

    //region Tree data

    struct Column

    {

        std::string title;

        int width;

    };

    struct Node

    {

        std::vector<std::string> cells;

        std::vector<Node> children;

        bool expanded = true;

    };

    struct FlatRow

    {

        Node *node;

        int depth;

    };

    std::vector<Column> columns_;

    std::vector<Node> roots_;

    int selected_row_ = 0;      // index into the flattened visible rows

    int scroll_row_ = 0;        // first visible row

    int header_height_ = 22;

    int row_height_ = 20;

    //endregion

    void flatten_rec(Node& node, int depth, std::vector<FlatRow>& out)

    {

        FlatRow row;

        row.node = &node;

        row.depth = depth;

        out.push_back(row);

        if (node.expanded)

        {

            for (size_t i = 0; i < node.children.size(); ++i)

            {

                flatten_rec(node.children[i], depth + 1, out);

            }

        }

    }

    std::vector<FlatRow> flatten()

    {

        std::vector<FlatRow> out;

        for (size_t i = 0; i < roots_.size(); ++i)

        {

            flatten_rec(roots_[i], 0, out);

        }

        return out;

    }

    int visible_rows()

    {

        return std::max(1, (height() - header_height_ - 4) / row_height_);

    }

    void keep_selection_visible()

    {

        if (selected_row_ < scroll_row_)

        {

            scroll_row_ = selected_row_;

        }

        if (selected_row_ >= scroll_row_ + visible_rows())

        {

            scroll_row_ = selected_row_ - visible_rows() + 1;

        }

        if (scroll_row_ < 0)

        {

            scroll_row_ = 0;

        }

    }

public:

    std::function<void()> on_selection_changed;

    GtkTreeView(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkTreeView";

        text_ = "TreeView";

        add_column("Name", 190);

        add_column("Size", 70);

        add_column("Kind", 90);

        // Sample tree: a little home folder.

        Node documents = make_node({"Documents", "", "Folder"});

        documents.children.push_back(make_node({"report.odt", "24 KB", "Document"}));

        documents.children.push_back(make_node({"budget.ods", "18 KB", "Spreadsheet"}));

        Node pictures = make_node({"Pictures", "", "Folder"});

        pictures.children.push_back(make_node({"holiday.png", "2.1 MB", "Image"}));

        pictures.children.push_back(make_node({"cat.jpg", "860 KB", "Image"}));

        Node music = make_node({"Music", "", "Folder"});

        music.children.push_back(make_node({"song.flac", "31 MB", "Audio"}));

        roots_.push_back(documents);

        roots_.push_back(pictures);

        roots_.push_back(music);

        roots_.push_back(make_node({"notes.txt", "4 KB", "Text"}));

    }

    ~GtkTreeView() {}

    Node make_node(std::vector<std::string> cells)

    {

        Node node;

        node.cells = cells;

        node.expanded = true;

        return node;

    }

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

    //region Tree specific API

    void add_column(std::string title, int col_width)

    {

        Column col;

        col.title = title;

        col.width = col_width;

        columns_.push_back(col);

    }

    int column_count()

    {

        return (int)columns_.size();

    }

    void append_root(std::vector<std::string> cells)

    {

        roots_.push_back(make_node(cells));

    }

    void append_child(int root_index, std::vector<std::string> cells)

    {

        if (root_index >= 0 && root_index < (int)roots_.size())

        {

            roots_[root_index].children.push_back(make_node(cells));

        }

    }

    void clear()

    {

        roots_.clear();

        selected_row_ = 0;

        scroll_row_ = 0;

    }

    void expand_all()

    {

        set_expanded_rec(roots_, true);

    }

    void collapse_all()

    {

        set_expanded_rec(roots_, false);

    }

    void set_expanded_rec(std::vector<Node>& nodes, bool expanded)

    {

        for (size_t i = 0; i < nodes.size(); ++i)

        {

            nodes[i].expanded = expanded;

            set_expanded_rec(nodes[i].children, expanded);

        }

    }

    int get_selected_row()

    {

        return selected_row_;

    }

    std::string get_selected_text()

    {

        std::vector<FlatRow> rows = flatten();

        if (selected_row_ >= 0 && selected_row_ < (int)rows.size() && !rows[selected_row_].node->cells.empty())

        {

            return rows[selected_row_].node->cells[0];

        }

        return "";

    }

    void set_scroll_row(int row)

    {

        scroll_row_ = std::max(0, row);

    }

    int get_scroll_row()

    {

        return scroll_row_;

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

        // Header band.

        unsigned long header_fill = styles_->get_color_rgb(225, 225, 225);

        XSetForeground(display_, gc_, header_fill);

        XFillRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 2, header_height_);

        int col_x = x1_ + 4;

        for (size_t c = 0; c < columns_.size(); ++c)

        {

            xf::draw_text_left(display_, window_, gc_, st, col_x, y1_ + 1, y1_ + 1 + header_height_, columns_[c].title);

            col_x += columns_[c].width;

            if (c + 1 < columns_.size())

            {

                XSetForeground(display_, gc_, st.lineColor_);

                XDrawLine(display_, window_, gc_, col_x - 4, y1_ + 3, col_x - 4, y1_ + header_height_ - 3);

            }

        }

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x1_ + 1, y1_ + header_height_ + 1, x2_ - 1, y1_ + header_height_ + 1);

        // Rows.

        std::vector<FlatRow> rows = flatten();

        unsigned long sel_fill = styles_->get_color_rgb(70, 110, 180);

        unsigned long guide = styles_->get_color_rgb(200, 200, 200);

        int rows_top = y1_ + header_height_ + 3;

        for (int i = scroll_row_; i < (int)rows.size(); ++i)

        {

            int ry = rows_top + (i - scroll_row_) * row_height_;

            if (ry + row_height_ > y2_ - 2)

            {

                break;

            }

            Node *node = rows[i].node;

            if (i == selected_row_)

            {

                XSetForeground(display_, gc_, sel_fill);

                XFillRectangle(display_, window_, gc_, x1_ + 2, ry, width() - 4, row_height_);

            }

            SStyle row_style = st;

            if (i == selected_row_)

            {

                row_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            int indent = rows[i].depth * 16;

            // Indent guide line.

            if (rows[i].depth > 0)

            {

                XSetForeground(display_, gc_, guide);

                XDrawLine(display_, window_, gc_, x1_ + 12 + indent - 8, ry, x1_ + 12 + indent - 8, ry + row_height_);

            }

            // Expander triangle.

            if (!node->children.empty())

            {

                int cx = x1_ + 10 + indent;

                int cy = ry + row_height_ / 2;

                if (node->expanded)

                {

                    xf::draw_triangle_down(display_, window_, gc_, row_style.text_color_, cx, cy - 3, 4);

                }

                else

                {

                    xf::draw_triangle_right(display_, window_, gc_, row_style.text_color_, cx - 2, cy, 4);

                }

            }

            // Cells across the columns.

            int cell_x = x1_ + 4;

            for (size_t c = 0; c < columns_.size(); ++c)

            {

                std::string value = c < node->cells.size() ? node->cells[c] : "";

                int tx = cell_x + (c == 0 ? 14 + indent : 0);

                xf::draw_text_left(display_, window_, gc_, row_style, tx, ry, ry + row_height_, value);

                cell_x += columns_[c].width;

            }

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

        if (button == 4)

        {

            scroll_row_ = std::max(0, scroll_row_ - 3);

            return true;

        }

        if (button == 5)

        {

            scroll_row_ += 3;

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        int rows_top = y1_ + header_height_ + 3;

        if (y < rows_top)

        {

            return true;

        }

        int index = scroll_row_ + (y - rows_top) / row_height_;

        std::vector<FlatRow> rows = flatten();

        if (index < 0 || index >= (int)rows.size())

        {

            return true;

        }

        Node *node = rows[index].node;

        int indent = rows[index].depth * 16;

        // Click on the expander toggles the node instead of selecting.

        if (!node->children.empty() && x >= x1_ + 2 + indent && x <= x1_ + 18 + indent)

        {

            node->expanded = !node->expanded;

            if (selected_row_ >= (int)flatten().size())

            {

                selected_row_ = (int)flatten().size() - 1;

            }

            return true;

        }

        if (index != selected_row_)

        {

            selected_row_ = index;

            if (on_selection_changed)

            {

                on_selection_changed();

            }

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

        std::vector<FlatRow> rows = flatten();

        if (rows.empty())

        {

            return false;

        }

        if (key == XK_Down && selected_row_ < (int)rows.size() - 1)

        {

            selected_row_++;

            keep_selection_visible();

            if (on_selection_changed)

            {

                on_selection_changed();

            }

            return true;

        }

        if (key == XK_Up && selected_row_ > 0)

        {

            selected_row_--;

            keep_selection_visible();

            if (on_selection_changed)

            {

                on_selection_changed();

            }

            return true;

        }

        if (key == XK_Right || key == XK_Left)

        {

            Node *node = rows[selected_row_].node;

            if (!node->children.empty())

            {

                node->expanded = (key == XK_Right);

                return true;

            }

        }

        return false;

    }

};

//endregion
