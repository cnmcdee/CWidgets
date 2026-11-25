#pragma once

#include "core/xframe_core.h"

//region TShellListView

class TShellListView

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

    struct SRow

    {

        std::string name;

        std::string size;

        std::string type;

        bool is_dir;

    };

    std::string folder_ = "/home/user";

    std::vector<SRow> rows_;

    int item_index_ = 1;

    int top_index_ = 0;

    int last_press_index_ = -1;

    int header_h_ = 18;

    int row_h_ = 17;

    std::vector<SRow> rows_for(const std::string& folder)

    {

        std::vector<SRow> out;

        out.push_back(SRow{"..", "", "Folder", true});

        if (folder == "/home/user")

        {

            out.push_back(SRow{"Documents", "", "Folder", true});

            out.push_back(SRow{"Pictures", "", "Folder", true});

            out.push_back(SRow{"Downloads", "", "Folder", true});

            out.push_back(SRow{"document.txt", "4 KB", "Text file", false});

            out.push_back(SRow{"photo.png", "812 KB", "PNG image", false});

            out.push_back(SRow{"archive.zip", "2.1 MB", "ZIP archive", false});

            out.push_back(SRow{"notes.md", "1 KB", "Markdown", false});

        }

        else if (folder == "/home/user/Documents")

        {

            out.push_back(SRow{"report.pdf", "96 KB", "PDF document", false});

            out.push_back(SRow{"budget.csv", "8 KB", "CSV file", false});

            out.push_back(SRow{"letter.txt", "2 KB", "Text file", false});

        }

        else

        {

            out.push_back(SRow{"empty.txt", "0 KB", "Text file", false});

        }

        return out;

    }

    void load_folder()

    {

        rows_ = rows_for(folder_);

        item_index_ = rows_.size() > 1 ? 1 : 0;

        top_index_ = 0;

        last_press_index_ = -1;

    }

    int visible_rows()

    {

        return std::max(1, (height() - header_h_) / row_h_);

    }

    void keep_visible()

    {

        if (item_index_ < top_index_)

        {

            top_index_ = item_index_;

        }

        if (item_index_ >= top_index_ + visible_rows())

        {

            top_index_ = item_index_ - visible_rows() + 1;

        }

    }

    std::string parent_of(const std::string& path)

    {

        size_t slash = path.find_last_of('/');

        if (slash == std::string::npos || slash == 0)

        {

            return "/";

        }

        return path.substr(0, slash);

    }

public:

    std::function<void()> on_click;

    std::function<void()> on_change;

    TShellListView(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TShellListView";

        load_folder();

        text_ = folder_;

    }

    ~TShellListView() {}

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

        set_folder(t);

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    std::string get_folder()

    {

        return folder_;

    }

    void set_folder(std::string f)

    {

        folder_ = f;

        text_ = f;

        load_folder();

    }

    std::string get_selected()

    {

        if (item_index_ >= 0 && item_index_ < (int)rows_.size())

        {

            return rows_[item_index_].name;

        }

        return "";

    }

    std::string get_selected_path()

    {

        std::string sel = get_selected();

        if (sel.empty())

        {

            return folder_;

        }

        if (sel == "..")

        {

            return parent_of(folder_);

        }

        return folder_ == "/" ? "/" + sel : folder_ + "/" + sel;

    }

    int get_count()

    {

        return (int)rows_.size();

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

        int size_x = x2_ - 190;

        int type_x = x2_ - 118;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        // Column header.

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 2, header_h_ - 1);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 4, y1_, y1_ + header_h_, "Name");

        xf::draw_text_left(display_, window_, gc_, st, size_x + 4, y1_, y1_ + header_h_, "Size");

        xf::draw_text_left(display_, window_, gc_, st, type_x + 4, y1_, y1_ + header_h_, "Type");

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, size_x, y1_ + 1, size_x, y1_ + header_h_);

        XDrawLine(display_, window_, gc_, type_x, y1_ + 1, type_x, y1_ + header_h_);

        int rows = visible_rows();

        for (int r = 0; r < rows; ++r)

        {

            int i = top_index_ + r;

            if (i >= (int)rows_.size())

            {

                break;

            }

            int ry = y1_ + header_h_ + r * row_h_;

            if (i == item_index_)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, x1_ + 1, ry, width() - 2, row_h_);

            }

            SStyle rs = st;

            if (i == item_index_)

            {

                rs.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            if (rows_[i].is_dir)

            {

                XSetForeground(display_, gc_, styles_->get_color_rgb(240, 200, 90));

                XFillRectangle(display_, window_, gc_, x1_ + 4, ry + 5, 11, 7);

                XFillRectangle(display_, window_, gc_, x1_ + 4, ry + 3, 5, 3);

            }

            xf::draw_text_left(display_, window_, gc_, rs, x1_ + 19, ry, ry + row_h_, rows_[i].name);

            xf::draw_text_left(display_, window_, gc_, rs, size_x + 4, ry, ry + row_h_, rows_[i].size);

            xf::draw_text_left(display_, window_, gc_, rs, type_x + 4, ry, ry + row_h_, rows_[i].type);

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

            if (top_index_ > 0)

            {

                --top_index_;

            }

            return true;

        }

        if (button == 5)

        {

            if (top_index_ + visible_rows() < (int)rows_.size())

            {

                ++top_index_;

            }

            return true;

        }

        if (button != 1 || y < y1_ + header_h_)

        {

            return button == 1;

        }

        int i = top_index_ + (y - y1_ - header_h_) / row_h_;

        if (i < 0 || i >= (int)rows_.size())

        {

            return true;

        }

        // A second press on the same folder row navigates into it.

        if (i == last_press_index_ && rows_[i].is_dir)

        {

            if (rows_[i].name == "..")

            {

                set_folder(parent_of(folder_));

            }

            else

            {

                set_folder(folder_ == "/" ? "/" + rows_[i].name : folder_ + "/" + rows_[i].name);

            }

            if (on_change)

            {

                on_change();

            }

            return true;

        }

        item_index_ = i;

        last_press_index_ = i;

        keep_visible();

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

        if (key == XK_Up)

        {

            if (item_index_ > 0)

            {

                --item_index_;

                keep_visible();

            }

            return true;

        }

        if (key == XK_Down)

        {

            if (item_index_ < (int)rows_.size() - 1)

            {

                ++item_index_;

                keep_visible();

            }

            return true;

        }

        if (key == XK_Return && item_index_ >= 0 && rows_[item_index_].is_dir)

        {

            if (rows_[item_index_].name == "..")

            {

                set_folder(parent_of(folder_));

            }

            else

            {

                set_folder(folder_ + "/" + rows_[item_index_].name);

            }

            if (on_change)

            {

                on_change();

            }

            return true;

        }

        if (key == XK_BackSpace)

        {

            set_folder(parent_of(folder_));

            return true;

        }

        return false;

    }

};

//endregion
