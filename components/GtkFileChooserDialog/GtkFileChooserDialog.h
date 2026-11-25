#pragma once

#include "core/xframe_core.h"

//region GtkFileChooserDialog

class GtkFileChooserDialog

{

public:

    // GtkFileChooserAction values.

    static const int ACTION_OPEN = 0;

    static const int ACTION_SAVE = 1;

    static const int ACTION_SELECT_FOLDER = 2;

    static const int ACTION_CREATE_FOLDER = 3;

    // Real GTK response ids.

    static const int RESPONSE_NONE = -1;

    static const int RESPONSE_DELETE_EVENT = -4;

    static const int RESPONSE_ACCEPT = -3;

    static const int RESPONSE_CANCEL = -6;

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;          // the dialog title

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    struct FileEntry

    {

        std::string name;

        bool is_dir;

        std::string size;        // "" for folders

    };

    int action_ = ACTION_OPEN;

    std::string current_folder_ = "/home/chris";

    std::vector<FileEntry> entries_;

    int selected_ = -1;          // index into entries_, -1 = none

    int scroll_ = 0;             // first visible row

    std::string filename_edit_;  // the Name: field at the bottom

    bool edit_focused_ = false;

    int response_ = RESPONSE_NONE;

    int pressed_button_ = -1;    // 0 cancel, 1 open/save, 2 up

    int last_row_ = -1;          // double-click detection

    int last_row_presses_ = 0;

    std::vector<std::string> filters_;   // filter names, display only

    int title_bar_height()

    {

        return 26;

    }

    int action_bar_height()

    {

        return 40;

    }

    int row_height()

    {

        return 20;

    }

    int sidebar_width()

    {

        return 112;

    }

    int list_top()

    {

        return y1_ + title_bar_height() + 34;

    }

    int list_bottom()

    {

        return y2_ - action_bar_height() - 34;

    }

    int visible_rows()

    {

        return (list_bottom() - list_top()) / row_height();

    }

    // Sample folder contents: the demo ships a tiny fake file
    // system so navigation visibly works.

    std::vector<FileEntry> sample_listing(const std::string& folder)

    {

        std::vector<FileEntry> out;

        FileEntry e;

        if (folder == "/home/chris")

        {

            e = { "Documents", true, "" };

            out.push_back(e);

            e = { "Downloads", true, "" };

            out.push_back(e);

            e = { "Pictures", true, "" };

            out.push_back(e);

            e = { "notes.txt", false, "4.2 kB" };

            out.push_back(e);

            e = { "budget.ods", false, "38 kB" };

            out.push_back(e);

        }

        else if (folder == "/home/chris/Documents")

        {

            e = { "report.txt", false, "12 kB" };

            out.push_back(e);

            e = { "letter.odt", false, "9.1 kB" };

            out.push_back(e);

            e = { "manual.pdf", false, "2.4 MB" };

            out.push_back(e);

        }

        else if (folder == "/home/chris/Downloads")

        {

            e = { "installer.run", false, "84 MB" };

            out.push_back(e);

            e = { "photo.png", false, "1.1 MB" };

            out.push_back(e);

        }

        else if (folder == "/home/chris/Pictures")

        {

            e = { "holiday.jpg", false, "3.2 MB" };

            out.push_back(e);

            e = { "screenshot.png", false, "212 kB" };

            out.push_back(e);

        }

        else

        {

            e = { "readme.txt", false, "1.0 kB" };

            out.push_back(e);

        }

        return out;

    }

    void load_folder(const std::string& folder)

    {

        current_folder_ = folder;

        entries_ = sample_listing(folder);

        selected_ = -1;

        scroll_ = 0;

        filename_edit_.clear();

    }

    std::string parent_folder(const std::string& folder)

    {

        size_t slash = folder.find_last_of('/');

        if (slash == std::string::npos || slash == 0)

        {

            return "/";

        }

        return folder.substr(0, slash);

    }

    void button_rect(int which, int &bx1, int &by1, int &bx2, int &by2)

    {

        if (which == 2)

        {

            // The Up button sits at the right of the path bar.

            by1 = y1_ + title_bar_height() + 5;

            by2 = by1 + 22;

            bx2 = x2_ - 10;

            bx1 = bx2 - 52;

            return;

        }

        by2 = y2_ - 7;

        by1 = by2 - 26;

        bx2 = x2_ - 10 - (1 - which) * 96;

        bx1 = bx2 - 88;

    }

    bool in_rect(int x, int y, int rx1, int ry1, int rx2, int ry2)

    {

        return x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2;

    }

    void accept_current()

    {

        if (action_ == ACTION_SELECT_FOLDER || action_ == ACTION_CREATE_FOLDER)

        {

            respond(RESPONSE_ACCEPT);

            return;

        }

        if (!filename_edit_.empty())

        {

            respond(RESPONSE_ACCEPT);

            return;

        }

        if (selected_ >= 0 && !entries_[selected_].is_dir)

        {

            filename_edit_ = entries_[selected_].name;

            respond(RESPONSE_ACCEPT);

        }

    }

public:

    std::function<void(int)> on_response;

    GtkFileChooserDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkFileChooserDialog";

        text_ = "Open File";

        load_folder("/home/chris");

        filters_.push_back("All files");

        filters_.push_back("Text files");

    }

    ~GtkFileChooserDialog() {}

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

    //region GtkFileChooserDialog specific API

    void set_action(int a)

    {

        action_ = a;

        text_ = (a == ACTION_SAVE) ? "Save File" : (a == ACTION_SELECT_FOLDER ? "Select Folder" : "Open File");

    }

    int get_action()

    {

        return action_;

    }

    void set_current_folder(std::string folder)

    {

        load_folder(folder);

    }

    std::string get_current_folder()

    {

        return current_folder_;

    }

    void set_filename(std::string f)

    {

        filename_edit_ = f;

    }

    // The chosen file: current folder + the name field (or the
    // selected row when the field is empty).

    std::string get_filename()

    {

        std::string base = filename_edit_;

        if (base.empty() && selected_ >= 0)

        {

            base = entries_[selected_].name;

        }

        if (base.empty())

        {

            return current_folder_;

        }

        if (current_folder_ == "/")

        {

            return "/" + base;

        }

        return current_folder_ + "/" + base;

    }

    std::string get_selected_name()

    {

        if (selected_ >= 0 && selected_ < (int)entries_.size())

        {

            return entries_[selected_].name;

        }

        return "";

    }

    void add_filter(std::string filter_name)

    {

        filters_.push_back(filter_name);

    }

    int get_response()

    {

        return response_;

    }

    void respond(int response_id)

    {

        response_ = response_id;

        if (on_response)

        {

            on_response(response_id);

        }

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

        int tb = title_bar_height();

        // Title bar.

        XSetForeground(display_, gc_, st.outlineColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, tb - 2);

        SStyle tst = st;

        tst.text_color_ = st.fillColor_;

        xf::draw_text_left(display_, window_, gc_, tst, x1_ + 10, y1_, y1_ + tb, text_);

        xf::draw_text_centered(display_, window_, gc_, tst, x2_ - 28, y1_, x2_ - 4, y1_ + tb, "X");

        // Sidebar places.

        const char* places[4] = { "Home", "Documents", "Downloads", "Pictures" };

        const char* place_paths[4] = { "/home/chris", "/home/chris/Documents", "/home/chris/Downloads", "/home/chris/Pictures" };

        int sx1 = x1_ + 8;

        int sx2 = x1_ + sidebar_width();

        for (int i = 0; i < 4; ++i)

        {

            int ry1 = list_top() + i * 24;

            int ry2 = ry1 + 22;

            if (current_folder_ == place_paths[i])

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, sx1, ry1, sx2 - sx1, ry2 - ry1);

                SStyle sst = st;

                sst.text_color_ = st.fillColor_;

                xf::draw_text_left(display_, window_, gc_, sst, sx1 + 8, ry1, ry2, places[i]);

            }

            else

            {

                xf::draw_text_left(display_, window_, gc_, st, sx1 + 8, ry1, ry2, places[i]);

            }

        }

        // Path bar: the current folder, with an Up button.

        int px1 = sx2 + 8;

        int py1 = y1_ + tb + 5;

        int py2 = py1 + 22;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, px1, py1, x2_ - 70 - px1, py2 - py1);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, px1, py1, x2_ - 70 - px1, py2 - py1);

        xf::draw_text_left(display_, window_, gc_, st, px1 + 6, py1, py2, current_folder_);

        int ux1, uy1, ux2, uy2;

        button_rect(2, ux1, uy1, ux2, uy2);

        SStyle ust = st;

        if (pressed_button_ == 2)

        {

            unsigned long swap = ust.fillColor_;

            ust.fillColor_ = ust.outlineColor_;

            ust.outlineColor_ = swap;

        }

        xf::draw_polished(display_, window_, gc_, ust, ux1, uy1, ux2, uy2);

        xf::draw_text_centered(display_, window_, gc_, ust, ux1, uy1, ux2, uy2, "Up");

        // File list frame + header.

        int lx1 = px1;

        int lx2 = x2_ - 10;

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, lx1, list_top(), lx2 - lx1, list_bottom() - list_top());

        xf::draw_text_left(display_, window_, gc_, st, lx1 + 8, list_top() - 20, list_top() - 2, "Name");

        xf::draw_text_left(display_, window_, gc_, st, lx2 - 70, list_top() - 20, list_top() - 2, "Size");

        for (int row = 0; row < visible_rows(); ++row)

        {

            int index = scroll_ + row;

            if (index >= (int)entries_.size())

            {

                break;

            }

            int ry1 = list_top() + 1 + row * row_height();

            int ry2 = ry1 + row_height();

            if (index == selected_)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, lx1 + 1, ry1, lx2 - lx1 - 2, row_height() - 1);

            }

            SStyle rst = st;

            if (index == selected_)

            {

                rst.text_color_ = st.fillColor_;

            }

            std::string label = entries_[index].is_dir ? entries_[index].name + "/" : entries_[index].name;

            xf::draw_text_left(display_, window_, gc_, rst, lx1 + 8, ry1, ry2, label);

            xf::draw_text_left(display_, window_, gc_, rst, lx2 - 70, ry1, ry2, entries_[index].size);

        }

        // Name field row.

        int ny = list_bottom() + 8;

        xf::draw_text_left(display_, window_, gc_, st, lx1, ny, ny + 20, "Name:");

        int ex1 = lx1 + 44;

        int ex2 = lx2;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, ex1, ny, ex2 - ex1, 20);

        XSetForeground(display_, gc_, edit_focused_ ? st.lineColor_ : styles_->get_color_rgb(150, 150, 150));

        xf::set_line_width(display_, gc_, edit_focused_ ? 2 : 1);

        XDrawRectangle(display_, window_, gc_, ex1, ny, ex2 - ex1, 20);

        xf::set_line_width(display_, gc_, 1);

        xf::draw_text_left(display_, window_, gc_, st, ex1 + 5, ny, ny + 20, filename_edit_);

        if (edit_focused_)

        {

            int cursor_x = ex1 + 5 + xf::text_width(st, filename_edit_);

            XSetForeground(display_, gc_, st.text_color_);

            XDrawLine(display_, window_, gc_, cursor_x, ny + 3, cursor_x, ny + 17);

        }

        // Action bar.

        int ay = y2_ - action_bar_height();

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x1_ + 2, ay, x2_ - 2, ay);

        const char* labels[2] = { "Cancel", action_ == ACTION_SAVE ? "Save" : "Open" };

        for (int i = 0; i < 2; ++i)

        {

            int bx1, by1, bx2, by2;

            button_rect(i, bx1, by1, bx2, by2);

            SStyle bst = st;

            if (pressed_button_ == i)

            {

                unsigned long swap = bst.fillColor_;

                bst.fillColor_ = bst.outlineColor_;

                bst.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, bst, bx1, by1, bx2, by2);

            xf::draw_text_centered(display_, window_, gc_, bst, bx1, by1, bx2, by2, labels[i]);

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

        int lx1 = x1_ + sidebar_width() + 8;

        int lx2 = x2_ - 10;

        // Mouse wheel scrolls the file list.

        if (button == 4 || button == 5)

        {

            if (x >= lx1 && x <= lx2 && y >= list_top() && y <= list_bottom())

            {

                scroll_ += (button == 5) ? 3 : -3;

                int max_scroll = (int)entries_.size() - visible_rows();

                if (max_scroll < 0)

                {

                    max_scroll = 0;

                }

                if (scroll_ > max_scroll)

                {

                    scroll_ = max_scroll;

                }

                if (scroll_ < 0)

                {

                    scroll_ = 0;

                }

                return true;

            }

            return false;

        }

        if (button != 1)

        {

            return false;

        }

        edit_focused_ = false;

        if (in_rect(x, y, x2_ - 28, y1_, x2_ - 4, y1_ + title_bar_height()))

        {

            respond(RESPONSE_DELETE_EVENT);

            return true;

        }

        // Sidebar places.

        const char* place_paths[4] = { "/home/chris", "/home/chris/Documents", "/home/chris/Downloads", "/home/chris/Pictures" };

        if (x >= x1_ + 8 && x <= x1_ + sidebar_width() && y >= list_top())

        {

            int index = (y - list_top()) / 24;

            if (index >= 0 && index < 4)

            {

                load_folder(place_paths[index]);

                return true;

            }

        }

        // Up button.

        {

            int ux1, uy1, ux2, uy2;

            button_rect(2, ux1, uy1, ux2, uy2);

            if (in_rect(x, y, ux1, uy1, ux2, uy2))

            {

                pressed_button_ = 2;

                return true;

            }

        }

        // File rows.

        if (x >= lx1 && x <= lx2 && y >= list_top() && y <= list_bottom())

        {

            int row = (y - list_top() - 1) / row_height();

            int index = scroll_ + row;

            if (index >= 0 && index < (int)entries_.size())

            {

                selected_ = index;

                if (index == last_row_)

                {

                    ++last_row_presses_;

                }

                else

                {

                    last_row_ = index;

                    last_row_presses_ = 1;

                }

                if (entries_[index].is_dir)

                {

                    if (last_row_presses_ >= 2)

                    {

                        load_folder(current_folder_ + "/" + entries_[index].name);

                        last_row_ = -1;

                        last_row_presses_ = 0;

                    }

                }

                else

                {

                    filename_edit_ = entries_[index].name;

                    if (last_row_presses_ >= 2)

                    {

                        accept_current();

                        last_row_ = -1;

                        last_row_presses_ = 0;

                    }

                }

                return true;

            }

            selected_ = -1;

            return true;

        }

        // Name field.

        int ny = list_bottom() + 8;

        if (x >= lx1 + 44 && x <= lx2 && y >= ny && y <= ny + 20)

        {

            edit_focused_ = true;

            return true;

        }

        // Action buttons.

        for (int i = 0; i < 2; ++i)

        {

            int bx1, by1, bx2, by2;

            button_rect(i, bx1, by1, bx2, by2);

            if (in_rect(x, y, bx1, by1, bx2, by2))

            {

                pressed_button_ = i;

                return true;

            }

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (button != 1 || pressed_button_ < 0)

        {

            return false;

        }

        int which = pressed_button_;

        pressed_button_ = -1;

        int bx1, by1, bx2, by2;

        button_rect(which, bx1, by1, bx2, by2);

        if (!in_rect(x, y, bx1, by1, bx2, by2))

        {

            return true;

        }

        if (which == 2)

        {

            if (current_folder_ != "/")

            {

                load_folder(parent_folder(current_folder_));

            }

        }

        else if (which == 0)

        {

            respond(RESPONSE_CANCEL);

        }

        else

        {

            accept_current();

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Escape)

        {

            respond(RESPONSE_CANCEL);

            return true;

        }

        if (key == XK_Return)

        {

            accept_current();

            return true;

        }

        if (key == XK_Down || key == XK_Up)

        {

            if (entries_.empty())

            {

                return true;

            }

            selected_ += (key == XK_Down) ? 1 : -1;

            if (selected_ < 0)

            {

                selected_ = 0;

            }

            if (selected_ >= (int)entries_.size())

            {

                selected_ = (int)entries_.size() - 1;

            }

            if (!entries_[selected_].is_dir)

            {

                filename_edit_ = entries_[selected_].name;

            }

            return true;

        }

        if (edit_focused_)

        {

            if (key == XK_BackSpace)

            {

                if (!filename_edit_.empty())

                {

                    filename_edit_.pop_back();

                }

                return true;

            }

            if (str && str[0] >= 32 && str[0] < 127)

            {

                filename_edit_ += str[0];

                return true;

            }

        }

        return false;

    }

};

//endregion
