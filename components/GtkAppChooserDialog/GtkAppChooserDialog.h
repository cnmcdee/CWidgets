#pragma once

#include "core/xframe_core.h"

//region GtkAppChooserDialog

class GtkAppChooserDialog

{

public:

    // Real GTK response ids.

    static const int RESPONSE_NONE = -1;

    static const int RESPONSE_DELETE_EVENT = -4;

    static const int RESPONSE_ACCEPT = -3;

    static const int RESPONSE_CANCEL = -6;

    static const int RESPONSE_OK = -5;

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

    std::string content_type_ = "text/plain";

    struct AppInfo

    {

        std::string name;

        std::string description;

        bool other;              // only shown with "show other" on

    };

    std::vector<AppInfo> apps_;

    int selected_ = 0;           // index into the visible app list

    int last_row_ = -1;          // double-click detection

    bool show_other_ = false;

    int response_ = RESPONSE_NONE;

    int pressed_button_ = -1;   // 0 cancel, 1 select

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

        return 34;

    }

    int list_top()

    {

        return y1_ + title_bar_height() + 34;

    }

    int list_bottom()

    {

        return y2_ - action_bar_height() - 30;

    }

    // Apps currently listed: recommended ones always, the "other"
    // ones only when the checkbox at the bottom is on.

    std::vector<AppInfo> visible_apps()

    {

        std::vector<AppInfo> out;

        for (size_t i = 0; i < apps_.size(); ++i)

        {

            if (!apps_[i].other || show_other_)

            {

                out.push_back(apps_[i]);

            }

        }

        return out;

    }

    // The sample application set depends on the content type,
    // like the real dialog asking the desktop for handlers.

    void load_apps()

    {

        apps_.clear();

        AppInfo a;

        if (content_type_.rfind("image/", 0) == 0)

        {

            a = { "Image Viewer", "View and organise your images", false };

            apps_.push_back(a);

            a = { "GNU Image Manipulation Program", "Edit images and photos", false };

            apps_.push_back(a);

            a = { "Shotwell", "Photo manager", false };

            apps_.push_back(a);

        }

        else

        {

            a = { "Text Editor", "Edit text files", false };

            apps_.push_back(a);

            a = { "LibreOffice Writer", "Word processor", false };

            apps_.push_back(a);

            a = { "Document Viewer", "View multi-page documents", false };

            apps_.push_back(a);

        }

        a = { "Files", "Access and organise files", true };

        apps_.push_back(a);

        a = { "Terminal", "Use the command line", true };

        apps_.push_back(a);

        a = { "Software Centre", "Find and install software", true };

        apps_.push_back(a);

        selected_ = 0;

    }

    void button_rect(int which, int &bx1, int &by1, int &bx2, int &by2)

    {

        by2 = y2_ - 7;

        by1 = by2 - 26;

        bx2 = x2_ - 10 - (1 - which) * 96;

        bx1 = bx2 - 88;

    }

    bool in_rect(int x, int y, int rx1, int ry1, int rx2, int ry2)

    {

        return x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2;

    }

public:

    std::function<void(int)> on_response;

    GtkAppChooserDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkAppChooserDialog";

        text_ = "Select Application";

        load_apps();

    }

    ~GtkAppChooserDialog() {}

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

    //region GtkAppChooserDialog specific API

    void set_content_type(std::string content_type)

    {

        content_type_ = content_type;

        load_apps();

    }

    std::string get_content_type()

    {

        return content_type_;

    }

    void add_application(std::string app_name, std::string description)

    {

        AppInfo a;

        a.name = app_name;

        a.description = description;

        a.other = false;

        apps_.push_back(a);

    }

    std::string get_selected_app()

    {

        std::vector<AppInfo> vis = visible_apps();

        if (selected_ >= 0 && selected_ < (int)vis.size())

        {

            return vis[selected_].name;

        }

        return "";

    }

    void set_show_other(bool s)

    {

        show_other_ = s;

    }

    bool get_show_other()

    {

        return show_other_;

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

        // Heading line naming the content type.

        std::string heading = "Select an application to open '" + content_type_ + "':";

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y1_ + tb + 4, list_top() - 4, heading);

        // Application list.

        int lx1 = x1_ + 12;

        int lx2 = x2_ - 12;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, lx1, list_top(), lx2 - lx1, list_bottom() - list_top());

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, lx1, list_top(), lx2 - lx1, list_bottom() - list_top());

        std::vector<AppInfo> vis = visible_apps();

        for (size_t i = 0; i < vis.size(); ++i)

        {

            int ry1 = list_top() + 1 + (int)i * row_height();

            int ry2 = ry1 + row_height();

            if (ry2 > list_bottom())

            {

                break;

            }

            bool sel = (int)i == selected_;

            if (sel)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, lx1 + 1, ry1, lx2 - lx1 - 2, row_height() - 1);

            }

            // Icon placeholder: a polished square with the
            // application's initial in it.

            SStyle ist = st;

            if (sel)

            {

                ist.text_color_ = st.fillColor_;

            }

            xf::draw_polished(display_, window_, gc_, ist, lx1 + 6, ry1 + 5, lx1 + 30, ry1 + 29);

            std::string initial = vis[i].name.empty() ? "?" : vis[i].name.substr(0, 1);

            xf::draw_text_centered(display_, window_, gc_, ist, lx1 + 6, ry1 + 5, lx1 + 30, ry1 + 29, initial);

            SStyle rst = st;

            if (sel)

            {

                rst.text_color_ = st.fillColor_;

            }

            xf::draw_text_left(display_, window_, gc_, rst, lx1 + 40, ry1, ry1 + 17, vis[i].name);

            SStyle dst = rst;

            if (!sel)

            {

                dst.text_color_ = styles_->get_color_rgb(120, 120, 120);

            }

            xf::draw_text_left(display_, window_, gc_, dst, lx1 + 40, ry1 + 16, ry2, vis[i].description);

        }

        // "Show other applications" checkbox row.

        int cy = list_bottom() + 8;

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_ + 12, cy + 1, 13, 13);

        if (show_other_)

        {

            xf::draw_checkmark(display_, window_, gc_, st.text_color_, x1_ + 19, cy + 7, 10);

        }

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 32, cy, cy + 16, "Show other applications");

        // Action bar.

        int ay = y2_ - action_bar_height();

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x1_ + 2, ay, x2_ - 2, ay);

        const char* labels[2] = { "Cancel", "Select" };

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

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        if (in_rect(x, y, x2_ - 28, y1_, x2_ - 4, y1_ + title_bar_height()))

        {

            respond(RESPONSE_DELETE_EVENT);

            return true;

        }

        // Application rows (double press on a row selects at once).

        if (x >= x1_ + 12 && x <= x2_ - 12 && y >= list_top() && y <= list_bottom())

        {

            int row = (y - list_top() - 1) / row_height();

            std::vector<AppInfo> vis = visible_apps();

            if (row >= 0 && row < (int)vis.size())

            {

                if (row == last_row_)

                {

                    // Second press on the same row: double click.

                    selected_ = row;

                    last_row_ = -1;

                    respond(RESPONSE_ACCEPT);

                }

                else

                {

                    selected_ = row;

                    last_row_ = row;

                }

            }

            return true;

        }

        // Show-other checkbox.

        int cy = list_bottom() + 8;

        if (x >= x1_ + 12 && x <= x1_ + 220 && y >= cy && y <= cy + 16)

        {

            show_other_ = !show_other_;

            std::vector<AppInfo> vis = visible_apps();

            if (selected_ >= (int)vis.size())

            {

                selected_ = (int)vis.size() - 1;

            }

            return true;

        }

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

        if (in_rect(x, y, bx1, by1, bx2, by2))

        {

            respond(which == 1 ? RESPONSE_ACCEPT : RESPONSE_CANCEL);

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

        (void)str;

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

            respond(RESPONSE_ACCEPT);

            return true;

        }

        if (key == XK_Down || key == XK_Up)

        {

            std::vector<AppInfo> vis = visible_apps();

            if (!vis.empty())

            {

                selected_ += (key == XK_Down) ? 1 : -1;

                if (selected_ < 0)

                {

                    selected_ = 0;

                }

                if (selected_ >= (int)vis.size())

                {

                    selected_ = (int)vis.size() - 1;

                }

            }

            return true;

        }

        return false;

    }

};

//endregion
