# Choosers

Choosers let the user pick something: an application, a colour, a file, a font, or a place. The big chooser widgets are panels with lists and previews; the button-sized ones report a value that is already set, since no real chooser dialogs exist in this framework. Two of them (`GtkFileChooserWidget`, `TFileListBox`) work from canned listings and never read the real filesystem.

> Every widget here shares the common XFrame widget API — same constructor, set_bounds/draw/event handlers, visible/enabled state. The shared contract is documented in docs/index.md (written by the coordinator). Each subsection below lists only what is specific to that widget.

## GtkAppChooserButton

A button that shows the selected application: a little window glyph (outline, title bar, two content lines) at the left, the app name, and a dropdown triangle at the right.

**Constructor:** `GtkAppChooserButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):

- `set_text(std::string t)` / `get_text()` — the displayed name; mirrors the app name (default `"Text Editor"`)

- `set_selected_app(std::string name, std::string id)` — sets the app name and id together

- `set_app_name(std::string name)` / `get_app_name()` — the app name (default `"Text Editor"`)

- `set_app_id(std::string id)` / `get_app_id()` — the app id (default `"org.gnome.TextEditor"`)

- signal `on_click` (`std::function<void()>`) — fires on click

- signal `on_changed` (`std::function<void(std::string)>`) — fires on click with the app id

**Example:**

```cpp
#include "GtkAppChooserButton.h"

#include <cstdio>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkAppChooserButton - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkAppChooserButton widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_app_name("Text Editor");
    widget.on_changed = [](std::string id) { std::printf("app: %s\n", id.c_str()); };
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

**Notes:** Clicking does not open an app chooser — there is none in this framework; the callbacks simply report the app that is already set.

## GtkAppChooserWidget

A scrollable list of applications: each row has a 24x24 icon box with a glyph letter, the app name, and a grey description underneath. Eight apps are preloaded (Firefox Web Browser, Files, Terminal, Text Editor, Calculator, Image Viewer, Music Player, Software Centre).

**Constructor:** `GtkAppChooserWidget(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):

- `set_text(std::string t)` / `get_text()` — stored but not drawn (default `"App Chooser"`)

- `add_app(std::string app_name, std::string glyph, std::string description)` — appends an app

- `app_count()` — number of apps

- `get_selected()` — the selected row index (default `0`)

- `get_app_name()` — the selected app's name

- `select_app(std::string app_name)` — selects by name (no-op when the name is not found)

- signal `on_app_selected` (`std::function<void()>`) — fires when a row is clicked

- signal `on_app_activated` (`std::function<void()>`) — fires on a double-click (two clicks on the same row within 400 ms)

**Example:**

```cpp
#include "GtkAppChooserWidget.h"

#include <cstdio>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkAppChooserWidget - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkAppChooserWidget widget(display, screen, window, gc, &styles, 20, 20, 720, 480);
    widget.add_app("My App", "M", "Does things");
    widget.select_app("Text Editor");
    widget.on_app_selected = [] { std::printf("app selected\n"); };
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

**Notes:** Rows are 34px tall; only the rows that fit in the widget height are drawn (scrolled from `scroll_row_`, which the widget manages itself).

## GtkColorChooserWidget

A colour chooser: a 12x6 hue/value palette grid, a hue strip underneath, and a side panel with the current swatch plus R/G/B channel sliders and a hex readout.

**Constructor:** `GtkColorChooserWidget(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):

- `set_text(std::string t)` / `get_text()` — stored but not drawn (default `"Color Chooser"`)

- `set_rgba(Srgb color, int alpha)` — sets the current colour and alpha (alpha clamped to 0..255; default colour `{200, 60, 40}`, alpha `255`)

- `get_rgba()` (`Srgb`) — the current colour; `get_alpha()` — the alpha

- `get_hex()` — the current colour as `"#RRGGBB"`

- `set_channel(int channel, int value)` — sets one channel directly (0 = red, 1 = green, 2 = blue)

- signal `on_color_changed` (`std::function<void()>`) — fires when the colour changes

**Example:**

```cpp
#include "GtkColorChooserWidget.h"

#include <cstdio>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkColorChooserWidget - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkColorChooserWidget widget(display, screen, window, gc, &styles, 20, 20, 720, 480);
    widget.set_rgba(Srgb{200, 60, 40}, 255);
    widget.on_color_changed = [] { std::printf("colour changed\n"); };
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

**Notes:** Clicking a palette cell picks that colour; clicking the hue strip sets the hue; the channel sliders adjust one channel. The grid value falls off by 0.13 per row from the top.

## GtkFileChooserWidget

A file chooser: a breadcrumb band across the top, a Name/Size column header, and rows with folder/file glyphs. Double-click a folder to enter it, double-click a file to activate it.

**Constructor:** `GtkFileChooserWidget(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):

- `set_text(std::string t)` / `get_text()` — stored but not drawn (default `"File Chooser"`)

- `get_current_folder()` / `set_current_folder(std::string path)` — the current path (changing it reloads the listing and clears the selection)

- `get_filename()` — the full path of the selected file (folder + name), or empty when nothing is selected

- `get_selected_name()` — the selected entry's name

- `entry_count()` — the number of entries in the current listing

- signal `on_file_activated` (`std::function<void()>`) — fires on double-clicking a file

- signal `on_folder_changed` (`std::function<void()>`) — fires when a folder is entered

- signal `on_selection_changed` (`std::function<void()>`) — fires when the selection changes

**Example:**

```cpp
#include "GtkFileChooserWidget.h"

#include <cstdio>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkFileChooserWidget - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkFileChooserWidget widget(display, screen, window, gc, &styles, 20, 20, 720, 480);
    widget.set_current_folder("/home/user/Documents");
    widget.on_selection_changed = [] { std::printf("selection changed\n"); };
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

**Notes:** The listing is CANNED, not a real directory read: `listing_for()` returns hardcoded entries for a few paths (home, Documents, Downloads, …) and never touches the filesystem. `set_current_folder()` to an unknown path is unclear from the header — the fallback listing is not documented.

## GtkFontChooserWidget

A font chooser: three columns (Font, Style, Size) with a preview band underneath showing `"AaBbGg 0123456789"` and the selected font name.

**Constructor:** `GtkFontChooserWidget(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):

- `set_text(std::string t)` / `get_text()` — stored but not drawn (default `"Font Chooser"`)

- `get_font_name()` — the selected font as `"<family> <style> <size>"`

- `get_family()` / `get_face_style()` / `get_font_size()` — the selected parts

- `set_family(std::string family)` — selects a family by name

- `set_font_size(int size)` — selects a size by value

- signal `on_font_changed` (`std::function<void()>`) — fires when the selection changes

**Example:**

```cpp
#include "GtkFontChooserWidget.h"

#include <cstdio>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkFontChooserWidget - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkFontChooserWidget widget(display, screen, window, gc, &styles, 20, 20, 720, 480);
    widget.set_family("Sans");
    widget.set_font_size(12);
    widget.on_font_changed = [] { std::printf("font changed\n"); };
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

**Notes:** The family list is fixed: Sans, Serif, Monospace, DejaVu Sans, DejaVu Serif, Liberation Sans, Liberation Mono, Courier. The header does not expose a setter for the face style — it changes only by clicking the Style column.

## GtkPlacesSidebar

A sidebar of places: rows with an 18x18 icon box (glyph letter) and a place name. Eight places are preloaded (Home, Desktop, Documents, Downloads, Music, Pictures, Videos, Trash).

**Constructor:** `GtkPlacesSidebar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):

- `set_text(std::string t)` / `get_text()` — stored but not drawn (default `"Places"`)

- `add_place(std::string place_name, std::string glyph)` — appends a place

- `place_count()` — number of places

- `get_selected()` — the selected row index (default `0`)

- `get_selected_place()` — the selected place's name

- `select_place(std::string place_name)` — selects by name

- signal `on_place_selected` (`std::function<void(std::string)>`) — fires with the place name when a row is clicked

**Example:**

```cpp
#include "GtkPlacesSidebar.h"

#include <cstdio>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkPlacesSidebar - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkPlacesSidebar widget(display, screen, window, gc, &styles, 40, 40, 340, 200);
    widget.add_place("Work", "W");
    widget.select_place("Home");
    widget.on_place_selected = [](std::string place) { std::printf("place: %s\n", place.c_str()); };
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

**Notes:** Real hover state: `on_motion` tracks the hovered row and it is drawn highlighted. Rows are 28px tall.

## TColorBox

A colour dropdown: the field shows the selected colour's swatch and name, with a dropdown button at the right. The popup lists the 16 built-in `cl*` colours.

**Constructor:** `TColorBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):

- `set_text(std::string t)` / `get_text()` — stored but not drawn (default `"TColorBox"`)

- `is_dropped()` — whether the popup list is currently open

- `set_selected_color_by_name(std::string n)` — selects by `cl*` name

- `get_selected_name()` — the selected colour's name (default `"clBlue"`)

- `get_selected_color()` (`unsigned long`) / `get_selected_rgb()` (`Srgb`) — the selected colour as a pixel / as RGB

- `set_selected_index(int idx)` / `get_selected_index()` — the selection by index (default `12`, clBlue)

- `color_count()` — the number of colours (`16`)

- signal `on_change` (`std::function<void()>`) — fires when the selection changes

- signal `on_click` (`std::function<void()>`) — fires on click

**Example:**

```cpp
#include "TColorBox.h"

#include <cstdio>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TColorBox - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TColorBox widget(display, screen, window, gc, &styles, 40, 40, 280, 36);
    widget.set_selected_index(12);
    widget.on_change = [] { std::printf("colour changed\n"); };
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

**Notes:** The built-in list is clBlack, clMaroon, clGreen, clOlive, clNavy, clPurple, clTeal, clGray, clSilver, clRed, clLime, clYellow, clBlue, clFuchsia, clAqua, clWhite. Hover highlights a row in the open popup; the popup scrolls so the selected row stays visible.

## TColorButton

A colour button: a 22px colour swatch at the left of the face, the caption beside it, and a dropdown triangle at the right. Clicking opens a 5x4 palette popup under the button.

**Constructor:** `TColorButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):

- `set_text(std::string t)` / `get_text()` — the caption (default `"Color"`)

- `set_color(Srgb c)` / `get_color()` (`Srgb`) — the colour (default `{204, 51, 51}`, red); setting it fires `on_color_changed`

- `palette_open()` — whether the palette popup is currently open

- signal `on_click` (`std::function<void()>`) — fires on click

- signal `on_color_changed` (`std::function<void(Srgb)>`) — fires with the new colour when a palette cell is picked (or `set_color()` is called)

**Example:**

```cpp
#include "TColorButton.h"

#include <cstdio>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TColorButton - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TColorButton widget(display, screen, window, gc, &styles, 40, 40, 340, 200);
    widget.set_color(Srgb{204, 51, 51});
    widget.on_color_changed = [](Srgb c) { std::printf("colour: %d %d %d\n", c.r, c.g, c.b); };
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

**Notes:** The palette is a fixed 5 columns x 4 rows of colours with 18px cells; the current colour's cell is outlined in the popup. Clicking outside the palette (or picking a cell) closes it.

## TColorListBox

A list box of colours: a white inset list where each row shows a colour swatch and its `cl*` name. The selected row is blue with white text.

**Constructor:** `TColorListBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):

- `set_text(std::string t)` / `get_text()` — stored but not drawn (default `"TColorListBox"`)

- `get_selected_name()` — the selected colour's name (default `"clBlue"`)

- `get_selected_color()` (`unsigned long`) / `get_selected_rgb()` (`Srgb`) — the selected colour as a pixel / as RGB

- `set_selected_index(int idx)` / `get_selected_index()` — the selection by index (default `12`, clBlue)

- `set_selected_color_by_name(std::string n)` — selects by `cl*` name

- `color_count()` — the number of colours (`16`)

- `get_top_index()` — the first visible row (scroll position)

- signal `on_change` (`std::function<void()>`) — fires when the selection changes

- signal `on_click` (`std::function<void()>`) — fires on click

**Example:**

```cpp
#include "TColorListBox.h"

#include <cstdio>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TColorListBox - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TColorListBox widget(display, screen, window, gc, &styles, 40, 40, 340, 200);
    widget.set_selected_index(12);
    widget.on_change = [] { std::printf("colour changed\n"); };
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

**Notes:** The list is the same 16 built-in colours as `TColorBox` (clBlack…clWhite). Arrow keys move the selection; the list scrolls to keep it visible.

## TFileListBox

A file list box: a header bar showing the active mask, then rows of file names — directories in `[brackets]`. The selected row is highlighted.

**Constructor:** `TFileListBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):

- `set_text(std::string t)` / `get_text()` — the selected file's name (the widget overwrites it on selection change)

- `get_filename()` — the selected file's name

- `get_item_index()` — the selected row (default `0`)

- `get_count()` — the number of rows after filtering

- `set_mask(std::string m)` / `get_mask()` — the filter mask (default `"*.*"`; `;` separates alternatives, `*`/`?` wildcards)

- `set_file_type(int ft)` / `get_file_type()` — a bitmask: bit0 = files, bit1 = directories, bit2 = hidden (default `1`, files only)

- signal `on_click` (`std::function<void()>`) — fires on click

- signal `on_change` (`std::function<void()>`) — fires when the selection changes

- signal `on_dbl_click` (`std::function<void()>`) — fires on double-click

**Example:**

```cpp
#include "TFileListBox.h"

#include <cstdio>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TFileListBox - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TFileListBox widget(display, screen, window, gc, &styles, 40, 40, 340, 200);
    widget.set_mask("*.txt");
    widget.set_file_type(1);
    widget.on_change = [] { std::printf("file changed\n"); };
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

**Notes:** The item list is CANNED, not a real directory read: the constructor loads a fixed set of names (src, tests, document.txt, notes.txt, main.cpp, …) and the mask/file-type only filter that set. There is no way to point it at a real directory.
