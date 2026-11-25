#pragma once

#include "core/xframe_core.h"

//region TActionList

class TActionList

{

public:

    struct TAction

    {

        std::string name;

        std::string caption;

        std::string shortcut;

        bool enabled = true;

        std::function<void()> on_execute;

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

    std::vector<TAction> actions_;

    int find_index(const std::string& name)

    {

        for (int i = 0; i < (int)actions_.size(); ++i)

        {

            if (actions_[i].name == name)

            {

                return i;

            }

        }

        return -1;

    }

public:

    std::function<void()> on_click;

    TActionList(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TActionList";

        text_ = "ActionList";

        add_action("FileOpen", "Open", "Ctrl+O");

        add_action("FileSave", "Save", "Ctrl+S");

        add_action("FileExit", "Exit", "Alt+F4");

        add_action("EditCopy", "Copy", "Ctrl+C");

        add_action("EditPaste", "Paste", "Ctrl+V");

        add_action("HelpAbout", "About", "F1");

    }

    ~TActionList() {}

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

    //region Actions

    void add_action(std::string name, std::string caption, std::string shortcut)

    {

        if (find_index(name) >= 0)

        {

            return;

        }

        TAction action;

        action.name = name;

        action.caption = caption;

        action.shortcut = shortcut;

        action.enabled = true;

        actions_.push_back(action);

    }

    void remove_action(std::string name)

    {

        int index = find_index(name);

        if (index < 0)

        {

            return;

        }

        actions_.erase(actions_.begin() + index);

    }

    void clear_actions()

    {

        actions_.clear();

    }

    int action_count()

    {

        return (int)actions_.size();

    }

    bool has_action(std::string name)

    {

        return find_index(name) >= 0;

    }

    TAction* get_action(std::string name)

    {

        int index = find_index(name);

        if (index < 0)

        {

            return nullptr;

        }

        return &actions_[index];

    }

    TAction* get_action_at(int index)

    {

        if (index < 0 || index >= (int)actions_.size())

        {

            return nullptr;

        }

        return &actions_[index];

    }

    std::string get_caption(std::string name)

    {

        TAction* action = get_action(name);

        if (!action)

        {

            return "";

        }

        return action->caption;

    }

    void set_caption(std::string name, std::string caption)

    {

        TAction* action = get_action(name);

        if (!action)

        {

            return;

        }

        action->caption = caption;

    }

    std::string get_shortcut(std::string name)

    {

        TAction* action = get_action(name);

        if (!action)

        {

            return "";

        }

        return action->shortcut;

    }

    void set_shortcut(std::string name, std::string shortcut)

    {

        TAction* action = get_action(name);

        if (!action)

        {

            return;

        }

        action->shortcut = shortcut;

    }

    void set_action_enabled(std::string name, bool value)

    {

        TAction* action = get_action(name);

        if (!action)

        {

            return;

        }

        action->enabled = value;

    }

    bool get_action_enabled(std::string name)

    {

        TAction* action = get_action(name);

        if (!action)

        {

            return false;

        }

        return action->enabled;

    }

    void set_on_execute(std::string name, std::function<void()> fn)

    {

        TAction* action = get_action(name);

        if (!action)

        {

            return;

        }

        action->on_execute = fn;

    }

    bool execute(std::string name)

    {

        if (!enabled_)

        {

            return false;

        }

        TAction* action = get_action(name);

        if (!action)

        {

            return false;

        }

        if (!action->enabled)

        {

            return false;

        }

        if (!action->on_execute)

        {

            return false;

        }

        action->on_execute();

        return true;

    }

    // Find the action bound to a shortcut string such as "Ctrl+O".

    std::string action_for_shortcut(std::string shortcut)

    {

        for (size_t i = 0; i < actions_.size(); ++i)

        {

            if (actions_[i].shortcut == shortcut)

            {

                return actions_[i].name;

            }

        }

        return "";

    }

    bool execute_shortcut(std::string shortcut)

    {

        std::string name = action_for_shortcut(shortcut);

        if (name.empty())

        {

            return false;

        }

        return execute(name);

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // Design-time glyph only: this component has no runtime
        // appearance. A bordered rect holding its bounds, with the
        // action names listed as small rows of text inside it.

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        int row_h = 16;

        int baseline = y1_ + 14;

        if (st.font_)

        {

            baseline = y1_ + st.font_->ascent + 4;

            row_h = st.font_->ascent + st.font_->descent + 4;

        }

        xf::draw_text_at(display_, window_, gc_, st, x1_ + 6, baseline, text_);

        baseline += row_h;

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x1_ + 4, baseline - row_h + 4, x2_ - 5, baseline - row_h + 4);

        for (size_t i = 0; i < actions_.size(); ++i)

        {

            if (baseline > y2_ - 4)

            {

                break;

            }

            SStyle row_style = st;

            if (!actions_[i].enabled || !enabled_)

            {

                row_style.text_color_ = styles_->get_color_rgb(140, 140, 140);

            }

            std::string row = actions_[i].name;

            if (!actions_[i].shortcut.empty())

            {

                row += "  (" + actions_[i].shortcut + ")";

            }

            xf::draw_text_at(display_, window_, gc_, row_style, x1_ + 10, baseline, row);

            baseline += row_h;

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        (void)button;

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
