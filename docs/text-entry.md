# Text & Entry

Text input, selection and numeric entry: single-line fields, multi-line views, drop-down lists, sliders and spin controls. Every widget here draws its own field, caret and selection with X primitives, and reports back through `on_change` / `on_activate` style signals.

> Every widget here shares the common XFrame widget API — same constructor, set_bounds/draw/event handlers, visible/enabled state. The shared contract is documented in docs/index.md (written by the coordinator). Each subsection below lists only what is specific to that widget.

## GtkComboBox

A drop-down list. The closed box shows the active item's text with a divider and a down-pointing triangle at the right; opening it (by clicking, or with popup()) drops a white list below the box, with the active row highlighted and a scroll offset when the list is longer than the visible rows.

**Constructor:** `GtkComboBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the text shown in the closed box
- `add_item(std::string item)` — appends an item; the first item added becomes the active one
- `remove_item(int index)` — removes one item; an out-of-range index is ignored
- `clear_items()` — removes all items and clears the active item and the text
- `get_item_count()` / `get_item_text(int index)` — number of items / text of one item ("" out of range)
- `set_active(int index)` / `get_active()` — active index, -1 for none; setting it also updates the displayed text
- `get_active_text()` — text of the active item, "" when there is none
- `set_active_text(std::string t)` — activates the first item equal to `t`; nothing happens when there is no match
- `get_popup_open()` — whether the drop-down list is currently open
- `popup()` / `popdown()` — open / close the list programmatically (popup() does nothing when the widget is disabled or has no items)
- signal `on_changed` (`std::function<void()>`) — fires when the active item changes

**Example:**
```cpp
#include "GtkComboBox.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkComboBox - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkComboBox widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.add_item("Apple");
    widget.add_item("Banana");
    widget.add_item("Cherry");
    widget.set_active(1);
    widget.on_changed = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** The widget starts with no items, so add at least one before calling popup().

## GtkComboBoxText

A drop-down list like GtkComboBox, but with a text-row API (append / prepend / insert / remove by position) instead of add_item / remove_item. The closed box and the open list draw the same way: active text, divider, down triangle, white list with the highlighted row.

**Constructor:** `GtkComboBoxText(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the text shown in the closed box
- `append_text(std::string t)` — adds a row at the end
- `prepend_text(std::string t)` — adds a row at the start
- `insert_text(int position, std::string t)` — inserts a row; the position is clamped to the list
- `remove_text(int position)` — removes a row; out of range is ignored and the active index follows the removal
- `remove_all()` — clears all rows, the active item and the text, and closes the list
- `get_row_count()` — number of rows
- `set_active(int index)` / `get_active()` — active row index, -1 for none
- `get_active_text()` / `set_active_text(std::string t)` — text of the active row / activate the first row equal to `t`
- `get_popup_open()` — whether the list is open
- signal `on_changed` (`std::function<void()>`) — fires when the active row changes

**Example:**
```cpp
#include "GtkComboBoxText.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkComboBoxText - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkComboBoxText widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.append_text("Apple");
    widget.append_text("Banana");
    widget.append_text("Cherry");
    widget.set_active(0);
    widget.on_changed = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** Unlike GtkComboBox there is no popup() / popdown() — the list opens and closes from mouse clicks only.

## GtkDropDown

A drop-down list with a selection model (set_selected / get_selected) rather than an active-item model, and an optional search row at the top of the open list. The closed box shows the selected text with a down triangle; when searching is enabled the open list shows a filter field that narrows the items as you type.

**Constructor:** `GtkDropDown(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — aliases for set_selected_text / get_selected_text
- `add_item(std::string item)` — appends an item; the first item added becomes selected
- `remove_item(int index)` / `clear_items()` — remove one / all items
- `get_item_count()` — number of items
- `set_selected(int index)` / `get_selected()` — selected index, -1 for none
- `get_selected_text()` / `set_selected_text(std::string t)` — selected text / select the first item equal to `t`
- `set_enable_search(bool e)` / `get_enable_search()` — whether the open list shows a search row
- `get_popup_open()` — whether the list is open
- signal `on_changed` (`std::function<void()>`) — fires when the selection changes

**Example:**
```cpp
#include "GtkDropDown.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkDropDown - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkDropDown widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.add_item("Apple");
    widget.add_item("Banana");
    widget.add_item("Cherry");
    widget.set_selected(1);
    widget.set_enable_search(true);
    widget.on_changed = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** There is no popup() / popdown() — the list opens from a click. The search row only appears while the list is open and searching is enabled.

## GtkEditableLabel

A plain text label that turns into an editable field. At rest it draws just the text (with a focus rectangle when focused); while editing it draws a white entry with the text and a caret. A double-click starts editing, Enter applies the change, Escape cancels it, and clicking outside applies it.

**Constructor:** `GtkEditableLabel(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the label text (get_text returns the edit buffer while editing)
- `get_editing()` — whether the label is currently being edited
- `start_editing()` — enter edit mode, copying the text into the edit buffer; no-op when disabled or already editing
- `stop_editing(bool apply)` — leave edit mode; with apply=true the buffer becomes the text and on_editing_done fires, otherwise the buffer is discarded and on_editing_canceled fires
- signal `on_editing_done` (`std::function<void()>`) — fires when an edit is applied
- signal `on_editing_canceled` (`std::function<void()>`) — fires when an edit is cancelled

**Example:**
```cpp
#include "GtkEditableLabel.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkEditableLabel - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkEditableLabel widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_text("Double-click to edit");
    widget.on_editing_done = []{ std::printf("editing done\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** The edit buffer is separate from the text until it is applied.

## GtkEntry

A single-line text field. It draws a white rounded body with a blue focus ring when focused, grey placeholder text when empty, a blinking caret (advance the blink with tick()), and a highlighted selection that can be dragged with the mouse. Text scrolls horizontally when it is wider than the field.

**Constructor:** `GtkEntry(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the field text; setting it moves the caret to the end
- `set_placeholder_text(std::string p)` / `get_placeholder_text()` — grey hint shown when the field is empty
- `set_max_length(int m)` / `get_max_length()` — maximum text length, 0 = unlimited
- `set_editable(bool e)` / `get_editable()` — whether the user can change the text
- `get_cursor_position()` / `set_cursor_position(int pos)` — caret index, clamped to the text
- `select_region(int start, int end)` — select a character range
- `get_selection_bounds(int& start, int& end)` — writes the selection bounds, returns false when there is no selection
- `get_selected_text()` — the selected text ("" when nothing is selected)
- `delete_selected_text()` — deletes the selection, if any, and fires on_changed
- `tick()` — advance the caret blink; call from the host timer while the field is focused
- signal `on_changed` (`std::function<void()>`) — fires when the text changes
- signal `on_activate` (`std::function<void()>`) — fires on Enter

**Example:**
```cpp
#include "GtkEntry.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkEntry - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkEntry widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_text("Hello, XFrame");
    widget.set_placeholder_text("Type here...");
    widget.set_max_length(64);
    widget.on_changed = []{ std::printf("changed\n"); };
    widget.on_activate = []{ std::printf("activate\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** The constructor pre-fills the field with "Sample text" and the placeholder "Type here...".

## GtkEntryCompletion

A GtkEntry with a completion popup. As you type, the widget lists the completions matching the text typed so far (only once it reaches the minimum key length); picking one, or calling complete(), fills the field with the first current match and fires on_match_selected.

**Constructor:** `GtkEntryCompletion(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the field text
- `add_completion(std::string item)` / `remove_completion(std::string item)` / `clear_completions()` — manage the completion list
- `get_completion_count()` — number of completions
- `set_minimum_key_length(int n)` / `get_minimum_key_length()` — how many characters must be typed before matches are offered
- `get_match_count()` — how many completions match the current text
- `complete()` — accept the first current match, if any
- `tick()` — caret blink, as in GtkEntry
- signal `on_changed` (`std::function<void()>`) — fires when the text changes
- signal `on_match_selected` (`std::function<void(const std::string&)>`) — fires with the chosen completion text
- signal `on_activate` (`std::function<void()>`) — fires on Enter

**Example:**
```cpp
#include "GtkEntryCompletion.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkEntryCompletion - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkEntryCompletion widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.add_completion("apple");
    widget.add_completion("apricot");
    widget.add_completion("banana");
    widget.set_minimum_key_length(1);
    widget.on_match_selected = [](const std::string& s){ std::printf("match: %s\n", s.c_str()); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** The default minimum key length is unclear from the header — set it explicitly with set_minimum_key_length().

## GtkPasswordEntry

A GtkEntry that never shows the password: it draws one dot per character. An eye / peek zone at the right edge reveals the real text while peek is on; set_show_peek_text controls whether that toggle is available.

**Constructor:** `GtkPasswordEntry(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the real password; get_text() always returns the unmasked text
- `set_placeholder_text(std::string p)` / `get_placeholder_text()` — grey hint shown when empty
- `set_show_peek_text(bool show)` / `get_show_peek_text()` — whether the eye / peek toggle is shown
- `tick()` — caret blink
- signal `on_changed` (`std::function<void()>`) — fires when the text changes
- signal `on_activate` (`std::function<void()>`) — fires on Enter

**Example:**
```cpp
#include "GtkPasswordEntry.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkPasswordEntry - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkPasswordEntry widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_text("hunter2");
    widget.set_show_peek_text(true);
    widget.on_activate = []{ std::printf("activate\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** Clicking the peek zone toggles the plain-text view; the dots are drawn from the real buffer.

## GtkScale

A slider. It draws a grey trough with a blue filled portion up to the handle, a handle, and the current value as text underneath when drawing the value is on. It works horizontally or vertically, and add_mark() adds tick marks at values along the trough.

**Constructor:** `GtkScale(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — a text value stored on the widget; the slider draws the numeric value, not this text
- `set_range(double min, double max)` — the value range
- `set_value(double v)` / `get_value()` — the current value, clamped to the range
- `get_min()` / `get_max()` — the range ends
- `set_step(double step)` — drag / keyboard step
- `set_orientation_vertical(bool vertical)` / `get_orientation_vertical()` — vertical instead of horizontal
- `set_draw_value(bool draw)` — whether the value is drawn next to the slider
- `add_mark(double at_value)` / `clear_marks()` — tick marks on the trough
- signal `on_value_changed` (`std::function<void()>`) — fires when the value changes

**Example:**
```cpp
#include "GtkScale.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkScale - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkScale widget(display, screen, window, gc, &styles, 10, 10, 760, 34);
    widget.set_range(0.0, 100.0);
    widget.set_value(42.0);
    widget.add_mark(50.0);
    widget.on_value_changed = []{ std::printf("value changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** None.

## GtkScaleButton

A button that is also a value control: it draws a minus glyph at the left, the current value in the middle, and a plus glyph at the right. Clicking the ends steps the value by the step amount; on_value_changed carries the new value.

**Constructor:** `GtkScaleButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the widget text
- `set_value(double v)` / `get_value()` — the current value
- `set_range(double min, double max)` / `get_min()` / `get_max()` — the value range
- `set_step(double s)` / `get_step()` — step per click
- signal `on_click` (`std::function<void()>`) — fires when the button is pressed
- signal `on_value_changed` (`std::function<void(double)>`) — fires with the new value

**Example:**
```cpp
#include "GtkScaleButton.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkScaleButton - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkScaleButton widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_range(0.0, 10.0);
    widget.set_value(5.0);
    widget.set_step(1.0);
    widget.on_value_changed = [](double v){ std::printf("value: %f\n", v); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** The value is drawn with no decimals when the step is 1 or more, and with one decimal otherwise.

## GtkSearchBar

A search bar that draws nothing at all until it is revealed. Call set_reveal(true) to show it: it then draws a bar containing an entry field. Search mode enables typing in the entry; Escape stops the search.

**Constructor:** `GtkSearchBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the search text
- `set_reveal(bool reveal)` / `get_reveal()` — show / hide the bar; draw() returns early while it is not revealed
- `set_search_mode(bool enabled)` / `get_search_mode()` — whether the entry accepts input
- `handle_event_key(KeySym key, const char* str)` — feed a key to the bar (Escape stops the search); returns whether the key was handled
- `tick()` — caret blink
- signal `on_search_changed` (`std::function<void(const std::string&)>`) — fires with the current search text as it changes
- signal `on_stop_search` (`std::function<void()>`) — fires when the search is stopped

**Example:**
```cpp
#include "GtkSearchBar.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkSearchBar - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkSearchBar widget(display, screen, window, gc, &styles, 10, 10, 760, 34);
    widget.set_reveal(true);
    widget.set_search_mode(true);
    widget.set_text("query");
    widget.on_search_changed = [](const std::string& s){ std::printf("search: %s\n", s.c_str()); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** Unlike the other entries, on_key() ignores keys unless search mode is on; the host can route global keys (such as Escape) through handle_event_key().

## GtkSearchEntry

A search field: a white rounded entry with a magnifier drawn at the left and a clear button at the right when there is text. Typing fires on_search_changed with the current text; Enter fires on_activate.

**Constructor:** `GtkSearchEntry(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the search text
- `set_placeholder_text(std::string p)` / `get_placeholder_text()` — hint shown when empty
- `clear()` — empties the field
- `tick()` — caret blink
- signal `on_search_changed` (`std::function<void(const std::string&)>`) — fires with the current search text
- signal `on_activate` (`std::function<void()>`) — fires on Enter

**Example:**
```cpp
#include "GtkSearchEntry.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkSearchEntry - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkSearchEntry widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_placeholder_text("Search...");
    widget.set_text("frame");
    widget.on_search_changed = [](const std::string& s){ std::printf("search: %s\n", s.c_str()); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** None.

## GtkSpinButton

A numeric entry with up / down spinner buttons at the right. The value is a double with a configurable number of decimals; typing commits on Enter (commit()) and the spinner arrows step it by the step increment, wrapping if wrap is on.

**Constructor:** `GtkSpinButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the text shown in the field
- `set_range(double min, double max)` / `get_range(double& min, double& max)` — the value range
- `set_increments(double step, double page)` — step and page increments
- `set_digits(int digits)` / `get_digits()` — decimals shown
- `set_wrap(bool w)` / `get_wrap()` — wrap past the ends of the range
- `set_value(double v)` / `get_value()` — the current value
- `get_value_as_int()` — the current value truncated to int
- `commit()` — parse the typed text into the value
- `spin(int direction)` — step the value by +1 or -1 increments
- `tick()` — caret blink
- signal `on_value_changed` (`std::function<void(double)>`) — fires with the new value
- signal `on_activate` (`std::function<void()>`) — fires on Enter

**Example:**
```cpp
#include "GtkSpinButton.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkSpinButton - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkSpinButton widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_range(0.0, 100.0);
    widget.set_value(7.0);
    widget.set_digits(1);
    widget.on_value_changed = [](double v){ std::printf("value: %f\n", v); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** None.

## GtkTextView

A multi-line editable text area. It draws a white body with a blue focus ring, wraps long lines when wrap mode is on, and shows a blinking caret. The text is one string with newlines; the cursor is a single character index with line / column helpers.

**Constructor:** `GtkTextView(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the whole text
- `set_editable(bool e)` / `get_editable()` — whether the user can edit
- `set_wrap_mode(bool w)` / `get_wrap_mode()` — wrap long lines at the edge
- `get_cursor_position()` / `set_cursor_position(int pos)` — caret character index
- `get_cursor_line_col(int& line, int& col)` — caret line and column (0-based)
- `insert_at_cursor(const std::string& s)` — insert text at the caret
- `get_line_count()` — number of lines
- `scroll_to_top()` — scroll back to the first line
- `tick()` — caret blink
- signal `on_changed` (`std::function<void()>`) — fires when the text changes

**Example:**
```cpp
#include "GtkTextView.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkTextView - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkTextView widget(display, screen, window, gc, &styles, 20, 20, 720, 480);
    widget.set_text("Line one\nLine two\nLine three");
    widget.set_wrap_mode(true);
    widget.on_changed = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** Long lines are wrapped only when wrap mode is on; otherwise they are clipped at the edge.

## TCalcEdit

An edit field with a button at the right that opens an on-canvas calculator keypad. Clicking the button toggles the keypad (calc_open()); keys pressed on the keypad build the value, and the field shows the result as text. The value is a double.

**Constructor:** `TCalcEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the field text
- `get_value()` / `set_value(double v)` — the numeric value
- `calc_open()` — whether the calculator keypad is currently open
- signal `on_click` (`std::function<void()>`) — fires on a click in the field
- signal `on_button_click` (`std::function<void()>`) — fires when the calculator button is pressed
- signal `on_change` (`std::function<void()>`) — fires when the text / value changes

**Example:**
```cpp
#include "TCalcEdit.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TCalcEdit - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TCalcEdit widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_value(3.14);
    widget.on_change = []{ std::printf("changed\n"); };
    widget.on_button_click = []{ std::printf("button\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** The keypad is drawn by the widget itself below the field; it is not a separate window.

## TComboBox

The Lazarus-style combo box: an editable text field with a drop-down button. With style 0 the text is editable (caret shown); with style 1 it is list-only. The drop-down list opens below the field; items are plain strings.

**Constructor:** `TComboBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the field text
- `add_item(std::string item)` — appends an item
- `clear()` — removes all items
- `count()` — number of items
- `get_item(int index)` — text of one item
- `set_item_index(int index)` / `get_item_index()` — selected index, -1 for none
- `set_style(int s)` / `get_style()` — 0 = editable text, 1 = list-only (there are no named constants in the header)
- `set_dropdown_open(bool open)` / `get_dropdown_open()` — open / close the list programmatically
- `get_cursor_pos()` — caret position in the editable text
- `std::vector<std::string>& items()` — direct (mutable) access to the item list
- signal `on_change` (`std::function<void()>`) — fires when the text or selection changes
- signal `on_click` (`std::function<void()>`) — fires on a click in the field

**Example:**
```cpp
#include "TComboBox.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TComboBox - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TComboBox widget(display, screen, window, gc, &styles, 40, 40, 340, 200);
    widget.add_item("Apple");
    widget.add_item("Banana");
    widget.add_item("Cherry");
    widget.set_item_index(1);
    widget.on_change = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** The constructor pre-fills six fruit items (Apple, Banana, Cherry, Date, Elderberry, Fig); call clear() first if you want an empty list.

## TComboBoxEx

An extended combo box whose items carry an indent level and an image index as well as a caption. The selected item is drawn indented by 12 px per indent level; each item stores its image index for the host to use.

**Constructor:** `TComboBoxEx(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the field text
- `get_item_index()` / `set_item_index(int i)` — selected index
- `add_item(std::string caption, int indent, int image_index)` — appends an item with an indent level and an image index
- `get_count()` — number of items
- `get_item_indent(int i)` / `get_item_image(int i)` — indent level / image index of one item
- signal `on_click` (`std::function<void()>`) — fires on a click in the field
- signal `on_change` (`std::function<void()>`) — fires when the selection changes

**Example:**
```cpp
#include "TComboBoxEx.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TComboBoxEx - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TComboBoxEx widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.add_item("Apple", 0, -1);
    widget.add_item("Banana", 1, -1);
    widget.add_item("Cherry", 0, -1);
    widget.set_item_index(1);
    widget.on_change = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** There is no remove / clear method — items can only be added. Whether the image index is drawn anywhere is unclear from the header.

## TDateEdit

A date field with a button that opens a month calendar popup. The field itself shows the date as YYYY-MM-DD and is edited in day / month / year segments with the keyboard; the popup is a drawn month grid you navigate by clicking days.

**Constructor:** `TDateEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the field text
- `get_date()` — the date as "YYYY-MM-DD"
- `get_day()` / `get_month()` / `get_year()` — the date parts
- `set_date(int y, int m, int d)` — set the date
- signal `on_click` (`std::function<void()>`) — fires on a click in the field
- signal `on_button_click` (`std::function<void()>`) — fires when the calendar button is pressed
- signal `on_change` (`std::function<void()>`) — fires when the date changes

**Example:**
```cpp
#include "TDateEdit.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TDateEdit - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TDateEdit widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_date(2026, 11, 26);
    widget.on_change = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** The calendar popup is drawn by the widget (a month grid with weekday headers); it is not a separate window.

## TDirectoryEdit

An edit field with a button that opens a directory picker. The field holds the chosen directory path; set_root_directory limits the picker to a subtree, and set_show_hidden controls whether hidden entries are listed.

**Constructor:** `TDirectoryEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the path text
- `get_directory()` / `set_directory(std::string d)` — the chosen directory
- `set_root_directory(std::string d)` / `get_root_directory()` — the picker root
- `set_show_hidden(bool s)` / `get_show_hidden()` — list hidden (dot) entries in the picker
- signal `on_click` (`std::function<void()>`) — fires on a click in the field
- signal `on_button_click` (`std::function<void()>`) — fires when the picker button is pressed
- signal `on_change` (`std::function<void()>`) — fires when the path changes

**Example:**
```cpp
#include "TDirectoryEdit.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TDirectoryEdit - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TDirectoryEdit widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_directory("/tmp");
    widget.on_change = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** The picker is drawn by the widget itself; the header exposes no separate dialog object.

## TEdit

The plain Lazarus single-line edit: a white field with a thin outline, horizontal scrolling for long text, and a caret. No placeholder, no max length — just text, a cursor and a scroll offset.

**Constructor:** `TEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the field text
- `get_cursor_pos()` / `set_cursor_pos(int pos)` — caret index
- `get_scroll_offset()` — horizontal scroll offset in pixels (read-only)
- `clear()` — empties the field
- `select_all()` — selects the whole text
- signal `on_change` (`std::function<void()>`) — fires when the text changes
- signal `on_click` (`std::function<void()>`) — fires on a click in the field

**Example:**
```cpp
#include "TEdit.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TEdit - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TEdit widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_text("Hello");
    widget.on_change = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** None.

## TEditButton

An edit field with a button at the right showing a short glyph string. Pressing the button fires on_button_click; what the button does is up to the host.

**Constructor:** `TEditButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the field text
- `set_glyph(std::string g)` / `get_glyph()` — the string drawn on the button
- signal `on_click` (`std::function<void()>`) — fires on a click in the field
- signal `on_button_click` (`std::function<void()>`) — fires when the button is pressed
- signal `on_change` (`std::function<void()>`) — fires when the text changes

**Example:**
```cpp
#include "TEditButton.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TEditButton - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TEditButton widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_text("Hello");
    widget.set_glyph("...");
    widget.on_button_click = []{ std::printf("button\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** Unlike TCalcEdit / TDateEdit / TDirectoryEdit, this button has no built-in popup — the host decides what a click does.

## TFileNameEdit

An edit field with a button that opens a file dialog. The dialog kind (open, save, open-recent), the file filter, the starting directory and the default extension are all configurable; the chosen file lands in the field.

**Constructor:** `TFileNameEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the path text
- `get_filename()` / `set_filename(std::string f)` — the chosen file
- `set_dialog_kind(int k)` / `get_dialog_kind()` — 0 = open, 1 = save, 2 = open-recent
- `set_filter(std::string f)` / `get_filter()` — file filter, e.g. "*.txt"
- `set_initial_directory(std::string d)` / `get_initial_directory()` — where the dialog starts
- `set_default_extension(std::string e)` / `get_default_extension()` — extension added when the user types a bare name
- signal `on_click` (`std::function<void()>`) — fires on a click in the field
- signal `on_button_click` (`std::function<void()>`) — fires when the dialog button is pressed
- signal `on_change` (`std::function<void()>`) — fires when the path changes

**Example:**
```cpp
#include "TFileNameEdit.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TFileNameEdit - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TFileNameEdit widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_filename("/tmp/notes.txt");
    widget.set_filter("*.txt");
    widget.on_change = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** The dialog is drawn by the widget itself; the header exposes no separate dialog object.

## TFilterComboBox

A combo box of file filters. Each entry is a description plus a filter pattern ("Text files" / "*.txt"); get_filter() returns the pattern of the selected entry, which is what a file dialog consumes.

**Constructor:** `TFilterComboBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the displayed text
- `get_filter()` / `set_filter(std::string f)` — the selected filter pattern / select the entry with this pattern
- `get_item_index()` / `set_item_index(int i)` — selected entry index
- `add_filter(std::string description, std::string filter)` — appends an entry
- `get_count()` — number of entries
- signal `on_click` (`std::function<void()>`) — fires on a click in the field
- signal `on_change` (`std::function<void()>`) — fires when the selection changes

**Example:**
```cpp
#include "TFilterComboBox.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TFilterComboBox - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TFilterComboBox widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.add_filter("Text files", "*.txt");
    widget.add_filter("All files", "*");
    widget.set_item_index(0);
    widget.on_change = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** There is no remove / clear method.

## TFloatSpinEdit

A spin edit for floating-point values: an edit field with up / down buttons that step the value by the increment. The value is clamped to the min / max range and drawn with the configured number of decimals.

**Constructor:** `TFloatSpinEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the field text
- `get_value()` / `set_value(double v)` — the numeric value
- `set_min_value(double v)` / `get_min_value()` — lower bound
- `set_max_value(double v)` / `get_max_value()` — upper bound
- `set_increment(double v)` / `get_increment()` — step per click
- `set_decimals(int d)` / `get_decimals()` — decimals shown
- signal `on_change` (`std::function<void()>`) — fires when the value changes

**Example:**
```cpp
#include "TFloatSpinEdit.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TFloatSpinEdit - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TFloatSpinEdit widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_min_value(0.0);
    widget.set_max_value(10.0);
    widget.set_value(2.5);
    widget.set_decimals(2);
    widget.on_change = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** None.

## TLabeledEdit

An edit field with a caption label attached to it. The label sits above the field by default and can be placed below, to the left or to the right; the widget computes the label and edit rectangles from the position and draws both.

**Constructor:** `TLabeledEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the field text
- `set_label_text(std::string t)` / `get_label_text()` — the caption
- `set_label_position(TLabelPosition p)` — lpAbove (default), lpBelow, lpLeft or lpRight (the `TLabelPosition` enum)
- `get_label_position()` — the current label position as a `TLabelPosition`
- `label_rect()` / `edit_rect()` — the label / edit rectangles as `TLabeledEditRect` (`{x1, y1, x2, y2}`), computed from the label position
- `get_cursor()` — caret position in the field

**Example:**
```cpp
#include "TLabeledEdit.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TLabeledEdit - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TLabeledEdit widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_label_text("Name:");
    widget.set_label_position(lpAbove);
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** This is the only widget in this section with no signals at all — there is no on_change; read get_text() on demand.

## TMaskEdit

An edit field with an input mask. Mask characters '0' and '9' accept digits, 'L' and 'l' accept letters, 'A' and 'a' accept letters or digits; every other character in the mask is a literal that is drawn as-is and cannot be typed over. Empty slots show the blank character.

**Constructor:** `TMaskEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_mask(std::string m)` / `get_mask()` — the mask; the default is "(999) 000-0000"
- `set_blank_char(char c)` / `get_blank_char()` — the character drawn in empty slots
- `display_text()` — the text as drawn, literals included
- `get_plain_text()` — only the characters the user typed, literals stripped
- `set_text(std::string t)` / `get_text()` — set / read the field (get_text returns the displayed text)
- `get_cursor()` — caret position
- signal `on_click` (`std::function<void()>`) — fires on a click in the field
- signal `on_change` (`std::function<void()>`) — fires when the text changes

**Example:**
```cpp
#include "TMaskEdit.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TMaskEdit - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TMaskEdit widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_mask("(999) 000-0000");
    widget.set_text("5551234567");
    widget.on_change = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** get_text() returns the masked display text; use get_plain_text() for the raw input.

## TMemo

A multi-line memo: a white area with a scrollbar thumb at the right, one line per row, word wrap optional. Lines are added with add_line(); the caret moves by line and column, and the view scrolls to follow it.

**Constructor:** `TMemo(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the whole text
- `line_count()` — number of lines
- `get_line(int index)` — one line (0-based)
- `add_line(std::string line)` — appends a line
- `clear()` — empties the memo
- `get_cursor_line()` / `get_cursor_col()` — caret line / column
- `get_top_line()` — first visible line (the scroll position)
- `std::vector<std::string>& lines()` — direct (mutable) access to the lines
- `set_wordwrap(bool wrap)` / `get_wordwrap()` — wrap long lines
- signal `on_change` (`std::function<void()>`) — fires when the text changes
- signal `on_click` (`std::function<void()>`) — fires on a click in the memo

**Example:**
```cpp
#include "TMemo.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TMemo - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TMemo widget(display, screen, window, gc, &styles, 20, 20, 720, 480);
    widget.add_line("First line");
    widget.add_line("Second line");
    widget.on_change = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** There is no per-line setter — rebuild the text with set_text(), or clear() + add_line(), to change a line.

## TSpinEdit

The integer version of TFloatSpinEdit: an edit field with up / down buttons stepping an int value between a min and a max.

**Constructor:** `TSpinEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the field text
- `get_value()` / `set_value(int v)` — the integer value
- `set_min_value(int v)` / `get_min_value()` — lower bound
- `set_max_value(int v)` / `get_max_value()` — upper bound
- `set_increment(int v)` / `get_increment()` — step per click
- signal `on_change` (`std::function<void()>`) — fires when the value changes

**Example:**
```cpp
#include "TSpinEdit.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TSpinEdit - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TSpinEdit widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_min_value(0);
    widget.set_max_value(100);
    widget.set_value(42);
    widget.on_change = []{ std::printf("changed\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** None.

## TValueListEditor

A two-column key / value table with a header row. Keys go in the left column, values in the right; clicking a value cell edits it in place. Rows are addressed by index (0-based); find_key() / get_value_by_key() look rows up by name.

**Constructor:** `TValueListEditor(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the widget text
- `set_key_width(int w)` / `get_key_width()` — width of the key column in pixels
- `get_count()` — number of rows
- `get_selected_row()` / `set_selected_row(int i)` — selected row index
- `get_editing()` — whether a value cell is being edited right now
- `insert_row(std::string key, std::string value)` — appends a row
- `delete_row(int i)` — removes a row
- `get_key(int i)` / `get_value(int i)` / `set_value(int i, std::string v)` — key / value of one row
- `find_key(std::string name)` — row index for a key, -1 when the key is absent
- `get_value_by_key(std::string name)` — value for a key ("" when absent)
- signal `on_change` (`std::function<void(int)>`) — fires with the index of the row that changed

**Example:**
```cpp
#include "TValueListEditor.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TValueListEditor - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TValueListEditor widget(display, screen, window, gc, &styles, 20, 20, 720, 480);
    widget.insert_row("Name", "Ada");
    widget.insert_row("Age", "36");
    widget.set_selected_row(0);
    widget.on_change = [](int row){ std::printf("changed row %d\n", row); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** Row indexes are 0-based throughout.

