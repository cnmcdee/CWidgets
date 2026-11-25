#pragma once

#include "core/xframe_core.h"

#include <ctime>

//region GtkFileChooserWidget

class GtkFileChooserWidget

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

    struct Entry

    {

        std::string name;

        bool is_dir;

        std::string size;

    };

    std::string current_path_;

    std::vector<Entry> entries_;

    int selected_ = -1;

    int scroll_row_ = 0;

    int row_height_ = 20;

    int crumb_height_ = 26;

    long last_click_ms_ = 0;

    int last_click_row_ = -1;

    long now_ms()

    {

        return (long)((double)std::clock() * 1000.0 / (double)CLOCKS_PER_SEC);

    }

    // Sample listings: the demo file system. Folders first.

    std::vector<Entry> listing_for(std::string path)

    {

        std::vector<Entry> out;

        if (path == "/home/user/Documents")

        {

            out.push_back({"..", true, ""});

            out.push_back({"Work", true, ""});

            out.push_back({"Personal", true, ""});

            out.push_back({"letter.odt", false, "12 KB"});

            out.push_back({"taxes.pdf", false, "240 KB"});

            out.push_back({"cv.docx", false, "38 KB"});

        }

        else if (path == "/home/user/Downloads")

        {

            out.push_back({"..", true, ""});

            out.push_back({"installer.deb", false, "84 MB"});

            out.push_back({"photo-pack.zip", false, "310 MB"});

        }

        else

        {

            out.push_back({"Documents", true, ""});

            out.push_back({"Downloads", true, ""});

            out.push_back({"Music", true, ""});

            out.push_back({"Pictures", true, ""});

            out.push_back({"notes.txt", false, "4 KB"});

            out.push_back({"report.pdf", false, "1.2 MB"});

            out.push_back({"holiday.png", false, "2.1 MB"});

        }

        return out;

    }

    void enter_folder(std::string folder)

    {

        if (folder == "..")

        {

            size_t slash = current_path_.find_last_of('/');

            if (slash != std::string::npos && slash > 0)

            {

                current_path_ = current_path_.substr(0, slash);

            }

        }

        else

        {

            current_path_ = current_path_ + "/" + folder;

        }

        entries_ = listing_for(current_path_);

        selected_ = -1;

        scroll_row_ = 0;

        if (on_folder_changed)

        {

            on_folder_changed();

        }

    }

    int visible_rows()

    {

        return std::max(1, (height() - crumb_height_ - 26) / row_height_);

    }

    int row_at(int y)

    {

        int top = y1_ + crumb_height_ + 24;

        if (y < top)

        {

            return -1;

        }

        int index = scroll_row_ + (y - top) / row_height_;

        if (index < 0 || index >= (int)entries_.size())

        {

            return -1;

        }

        return index;

    }

public:

    std::function<void()> on_file_activated;

    std::function<void()> on_folder_changed;

    std::function<void()> on_selection_changed;

    GtkFileChooserWidget(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkFileChooserWidget";

        text_ = "File Chooser";

        current_path_ = "/home/user";

        entries_ = listing_for(current_path_);

    }

    ~GtkFileChooserWidget() {}

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

    //region File chooser specific API

    std::string get_current_folder()

    {

        return current_path_;

    }

    void set_current_folder(std::string path)

    {

        current_path_ = path;

        entries_ = listing_for(current_path_);

        selected_ = -1;

        scroll_row_ = 0;

    }

    std::string get_filename()

    {

        if (selected_ >= 0 && selected_ < (int)entries_.size() && !entries_[selected_].is_dir)

        {

            return current_path_ + "/" + entries_[selected_].name;

        }

        return "";

    }

    std::string get_selected_name()

    {

        if (selected_ >= 0 && selected_ < (int)entries_.size())

        {

            return entries_[selected_].name;

        }

        return "";

    }

    int entry_count()

    {

        return (int)entries_.size();

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

        // Breadcrumb band: one segment box per path part.

        unsigned long crumb_fill = styles_->get_color_rgb(232, 232, 232);

        XSetForeground(display_, gc_, crumb_fill);

        XFillRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 2, crumb_height_);

        std::vector<std::string> parts;

        std::string rest = current_path_;

        size_t pos = 0;

        while ((pos = rest.find('/')) != std::string::npos)

        {

            if (pos > 0)

            {

                parts.push_back(rest.substr(0, pos));

            }

            rest.erase(0, pos + 1);

        }

        if (!rest.empty())

        {

            parts.push_back(rest);

        }

        int cx = x1_ + 6;

        xf::draw_text_left(display_, window_, gc_, st, cx, y1_ + 1, y1_ + 1 + crumb_height_, "/");

        cx += 10;

        for (size_t i = 0; i < parts.size(); ++i)

        {

            int pw = xf::text_width(st, parts[i]) + 12;

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, cx, y1_ + 4, pw, crumb_height_ - 7);

            xf::draw_text_centered(display_, window_, gc_, st, cx, y1_ + 2, cx + pw, y1_ + crumb_height_ - 2, parts[i]);

            cx += pw + 4;

        }

        // Column header.

        int header_y = y1_ + crumb_height_ + 2;

        XSetForeground(display_, gc_, crumb_fill);

        XFillRectangle(display_, window_, gc_, x1_ + 1, header_y, width() - 2, 20);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 28, header_y, header_y + 20, "Name");

        xf::draw_text_left(display_, window_, gc_, st, x2_ - 90, header_y, header_y + 20, "Size");

        // Rows.

        unsigned long sel_fill = styles_->get_color_rgb(70, 110, 180);

        unsigned long folder_fill = styles_->get_color_rgb(240, 200, 90);

        unsigned long file_fill = styles_->get_color_rgb(235, 235, 235);

        int rows_top = header_y + 22;

        for (int i = scroll_row_; i < (int)entries_.size(); ++i)

        {

            int ry = rows_top + (i - scroll_row_) * row_height_;

            if (ry + row_height_ > y2_ - 2)

            {

                break;

            }

            SStyle row_style = st;

            if (i == selected_)

            {

                XSetForeground(display_, gc_, sel_fill);

                XFillRectangle(display_, window_, gc_, x1_ + 2, ry, width() - 4, row_height_);

                row_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            // Folder / file glyph.

            int gx = x1_ + 8;

            int gy = ry + 4;

            if (entries_[i].is_dir)

            {

                XSetForeground(display_, gc_, folder_fill);

                XFillRectangle(display_, window_, gc_, gx, gy + 3, 15, 10);

                XFillRectangle(display_, window_, gc_, gx, gy, 6, 4);

                XSetForeground(display_, gc_, st.lineColor_);

                XDrawRectangle(display_, window_, gc_, gx, gy + 3, 15, 10);

            }

            else

            {

                XSetForeground(display_, gc_, file_fill);

                XFillRectangle(display_, window_, gc_, gx + 2, gy, 11, 14);

                XSetForeground(display_, gc_, st.lineColor_);

                XDrawRectangle(display_, window_, gc_, gx + 2, gy, 11, 14);

            }

            xf::draw_text_left(display_, window_, gc_, row_style, x1_ + 28, ry, ry + row_height_, entries_[i].name);

            xf::draw_text_left(display_, window_, gc_, row_style, x2_ - 90, ry, ry + row_height_, entries_[i].size);

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

        int row = row_at(y);

        if (row < 0)

        {

            return true;

        }

        long now = now_ms();

        bool double_click = (row == last_click_row_ && now - last_click_ms_ < 400);

        last_click_row_ = row;

        last_click_ms_ = now;

        if (row != selected_)

        {

            selected_ = row;

            if (on_selection_changed)

            {

                on_selection_changed();

            }

        }

        if (double_click)

        {

            if (entries_[row].is_dir)

            {

                enter_folder(entries_[row].name);

            }

            else if (on_file_activated)

            {

                on_file_activated();

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

        if (!focused_ || !enabled_ || entries_.empty())

        {

            return false;

        }

        if (key == XK_Down && selected_ < (int)entries_.size() - 1)

        {

            selected_++;

        }

        else if (key == XK_Up && selected_ > 0)

        {

            selected_--;

        }

        else if (key == XK_Return && selected_ >= 0)

        {

            if (entries_[selected_].is_dir)

            {

                enter_folder(entries_[selected_].name);

            }

            else if (on_file_activated)

            {

                on_file_activated();

            }

            return true;

        }

        else

        {

            return false;

        }

        if (selected_ < scroll_row_)

        {

            scroll_row_ = selected_;

        }

        if (selected_ >= scroll_row_ + visible_rows())

        {

            scroll_row_ = selected_ - visible_rows() + 1;

        }

        if (on_selection_changed)

        {

            on_selection_changed();

        }

        return true;

    }

};

//endregion
