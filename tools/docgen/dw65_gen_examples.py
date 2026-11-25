import os
BOUNDS = {
"GtkComboBox": "40, 40, 280, 36", "GtkComboBoxText": "40, 40, 280, 36",
"GtkDropDown": "40, 40, 280, 36", "GtkEditableLabel": "40, 40, 280, 36",
"GtkEntry": "40, 40, 280, 36", "GtkEntryCompletion": "40, 40, 280, 36",
"GtkPasswordEntry": "40, 40, 280, 36", "GtkScale": "10, 10, 760, 34",
"GtkScaleButton": "40, 40, 280, 36", "GtkSearchBar": "10, 10, 760, 34",
"GtkSearchEntry": "40, 40, 280, 36", "GtkSpinButton": "40, 40, 280, 36",
"GtkTextView": "20, 20, 720, 480", "TCalcEdit": "40, 40, 280, 36",
"TComboBox": "40, 40, 340, 200", "TComboBoxEx": "40, 40, 280, 36",
"TDateEdit": "40, 40, 280, 36", "TDirectoryEdit": "40, 40, 280, 36",
"TEdit": "40, 40, 280, 36", "TEditButton": "40, 40, 280, 36",
"TFileNameEdit": "40, 40, 280, 36", "TFilterComboBox": "40, 40, 280, 36",
"TFloatSpinEdit": "40, 40, 280, 36", "TLabeledEdit": "40, 40, 280, 36",
"TMaskEdit": "40, 40, 280, 36", "TMemo": "20, 20, 720, 480",
"TSpinEdit": "40, 40, 280, 36", "TValueListEditor": "20, 20, 720, 480",
"GtkColumnView": "20, 20, 720, 480", "GtkGridView": "40, 40, 340, 200",
"GtkIconView": "40, 40, 340, 200", "GtkListBox": "40, 40, 340, 200",
"GtkListView": "40, 40, 340, 200", "GtkTreeView": "20, 20, 720, 480",
"TCheckGroup": "40, 40, 340, 200", "TCheckListBox": "40, 40, 340, 200",
"TDrawGrid": "20, 20, 720, 480", "THeaderControl": "10, 10, 760, 34",
"TListBox": "40, 40, 340, 200", "TListView": "20, 20, 720, 480",
"TRadioGroup": "40, 40, 340, 200", "TShellListView": "20, 20, 720, 480",
"TShellTreeView": "40, 40, 340, 200", "TStringGrid": "20, 20, 720, 480",
"TTreeView": "20, 20, 720, 480", "GtkActionBar": "10, 10, 760, 34",
"GtkCheckMenuItem": "40, 40, 340, 200", "GtkHeaderBar": "10, 10, 760, 34",
"GtkMenu": "40, 40, 340, 200", "GtkMenuBar": "10, 10, 760, 34",
"GtkMenuButton": "40, 40, 280, 36", "GtkMenuItem": "40, 40, 340, 200",
"GtkPopover": "40, 40, 340, 200", "GtkRadioMenuItem": "40, 40, 340, 200",
"GtkSeparatorMenuItem": "40, 40, 340, 200", "GtkToolbar": "10, 10, 760, 34",
"TActionList": "40, 40, 340, 200", "TMainMenu": "10, 10, 760, 34",
"TPopupMenu": "40, 40, 340, 200", "TPopupNotifier": "40, 40, 340, 200",
"TToolBar": "10, 10, 760, 34", "GtkEventBox": "40, 40, 340, 200",
"TApplicationProperties": "40, 40, 340, 200", "XGraph": "20, 20, 720, 480",
"XSpreadsheet": "20, 20, 720, 480",
}
SETUP = {
"GtkComboBox": ["widget.add_item(\"Apple\");", "widget.add_item(\"Banana\");", "widget.add_item(\"Cherry\");", "widget.set_active(1);"],
"GtkComboBoxText": ["widget.append_text(\"Apple\");", "widget.append_text(\"Banana\");", "widget.append_text(\"Cherry\");", "widget.set_active(0);"],
"GtkDropDown": ["widget.add_item(\"Apple\");", "widget.add_item(\"Banana\");", "widget.add_item(\"Cherry\");", "widget.set_selected(1);", "widget.set_enable_search(true);"],
"GtkEditableLabel": ["widget.set_text(\"Double-click to edit\");"],
"GtkEntry": ["widget.set_text(\"Hello, XFrame\");", "widget.set_placeholder_text(\"Type here...\");", "widget.set_max_length(64);"],
"GtkEntryCompletion": ["widget.add_completion(\"apple\");", "widget.add_completion(\"apricot\");", "widget.add_completion(\"banana\");", "widget.set_minimum_key_length(1);"],
"GtkPasswordEntry": ["widget.set_text(\"hunter2\");", "widget.set_show_peek_text(true);"],
"GtkScale": ["widget.set_range(0.0, 100.0);", "widget.set_value(42.0);", "widget.add_mark(50.0);"],
"GtkScaleButton": ["widget.set_range(0.0, 10.0);", "widget.set_value(5.0);", "widget.set_step(1.0);"],
"GtkSearchBar": ["widget.set_reveal(true);", "widget.set_search_mode(true);", "widget.set_text(\"query\");"],
"GtkSearchEntry": ["widget.set_placeholder_text(\"Search...\");", "widget.set_text(\"frame\");"],
"GtkSpinButton": ["widget.set_range(0.0, 100.0);", "widget.set_value(7.0);", "widget.set_digits(1);"],
"GtkTextView": ["widget.set_text(\"Line one\\nLine two\\nLine three\");", "widget.set_wrap_mode(true);"],
"TCalcEdit": ["widget.set_value(3.14);"],
"TComboBox": ["widget.add_item(\"Apple\");", "widget.add_item(\"Banana\");", "widget.add_item(\"Cherry\");", "widget.set_item_index(1);"],
"TComboBoxEx": ["widget.add_item(\"Apple\", 0, -1);", "widget.add_item(\"Banana\", 1, -1);", "widget.add_item(\"Cherry\", 0, -1);", "widget.set_item_index(1);"],
"TDateEdit": ["widget.set_date(2026, 11, 26);"],
"TDirectoryEdit": ["widget.set_directory(\"/tmp\");"],
"TEdit": ["widget.set_text(\"Hello\");"],
"TEditButton": ["widget.set_text(\"Hello\");", "widget.set_glyph(\"...\");"],
"TFileNameEdit": ["widget.set_filename(\"/tmp/notes.txt\");", "widget.set_filter(\"*.txt\");"],
"TFilterComboBox": ["widget.add_filter(\"Text files\", \"*.txt\");", "widget.add_filter(\"All files\", \"*\");", "widget.set_item_index(0);"],
"TFloatSpinEdit": ["widget.set_min_value(0.0);", "widget.set_max_value(10.0);", "widget.set_value(2.5);", "widget.set_decimals(2);"],
"TLabeledEdit": ["widget.set_label_text(\"Name:\");", "widget.set_label_position(lpAbove);"],
"TMaskEdit": ["widget.set_mask(\"(999) 000-0000\");", "widget.set_text(\"5551234567\");"],
"TMemo": ["widget.add_line(\"First line\");", "widget.add_line(\"Second line\");"],
"TSpinEdit": ["widget.set_min_value(0);", "widget.set_max_value(100);", "widget.set_value(42);"],
"TValueListEditor": ["widget.insert_row(\"Name\", \"Ada\");", "widget.insert_row(\"Age\", \"36\");", "widget.set_selected_row(0);"],
"GtkColumnView": ["widget.add_column(\"Name\", 200);", "widget.add_column(\"Age\", 80);", "widget.add_row(std::vector<std::string>{\"Ada\", \"36\"});"],
"GtkGridView": ["widget.add_item(\"One\");", "widget.add_item(\"Two\");", "widget.add_item(\"Three\");", "widget.add_item(\"Four\");", "widget.set_tile_size(100, 80);", "widget.select(1);"],
"GtkIconView": ["widget.add_item(\"One\");", "widget.add_item(\"Two\");", "widget.add_item(\"Three\");", "widget.select_item(0);"],
"GtkListBox": ["widget.add_row(\"Alpha\");", "widget.add_row(\"Beta\");", "widget.add_row(\"Gamma\");", "widget.set_selected_row(1);"],
"GtkListView": ["widget.add_item(\"One\");", "widget.add_item(\"Two\");", "widget.add_item(\"Three\");", "widget.select_row(1);"],
"GtkTreeView": ["widget.add_column(\"Name\", 200);", "widget.add_column(\"Size\", 100);", "widget.append_root(std::vector<std::string>{\"src\"});", "widget.append_child(0, std::vector<std::string>{\"main.cpp\"});"],
"TCheckGroup": ["widget.add_item(\"Bold\", true);", "widget.add_item(\"Italic\");", "widget.add_item(\"Underline\");", "widget.set_columns(2);"],
"TCheckListBox": ["widget.add_item(\"One\", true);", "widget.add_item(\"Two\");", "widget.add_item(\"Three\");", "widget.set_item_index(0);"],
"TDrawGrid": ["widget.set_col_count(5);", "widget.set_row_count(6);", "widget.set_selection(1, 1);"],
"THeaderControl": ["widget.add_section(\"Name\", 200);", "widget.add_section(\"Age\", 80);", "widget.set_sort(0, true);"],
"TListBox": ["widget.add_item(\"One\");", "widget.add_item(\"Two\");", "widget.add_item(\"Three\");", "widget.set_item_index(1);"],
"TListView": ["widget.add_column(\"Name\", 200);", "widget.add_column(\"Age\", 80);", "widget.add_item(std::vector<std::string>{\"Ada\", \"36\"});", "widget.set_selected(0);"],
"TRadioGroup": ["widget.add_item(\"Small\");", "widget.add_item(\"Medium\");", "widget.add_item(\"Large\");", "widget.set_item_index(1);"],
"TShellListView": ["widget.set_folder(\"/tmp\");"],
"TShellTreeView": ["widget.set_path(\"/tmp\");"],
"TStringGrid": ["widget.set_col_count(5);", "widget.set_row_count(6);", "widget.set_cell(1, 1, \"Hello\");", "widget.set_selection(1, 1);", "widget.set_go_editing(true);"],
"TTreeView": ["widget.add_root(\"Project\");", "widget.add_root(\"Data\");", "widget.full_expand();"],
"GtkActionBar": ["widget.pack_start(\"New\");", "widget.pack_start(\"Open\");", "widget.pack_end(\"Help\");", "widget.set_center_label(\"Document\");"],
"GtkCheckMenuItem": ["widget.set_text(\"Show toolbar\");", "widget.set_active(true);"],
"GtkHeaderBar": ["widget.set_title(\"Demo App\");", "widget.set_subtitle(\"v1.0\");", "widget.set_show_close_button(true);"],
"GtkMenu": ["widget.add_item(\"Open\");", "widget.add_submenu_item(\"Recent Files\");", "widget.add_disabled_item(\"Print...\");", "widget.add_separator();", "widget.add_item(\"Quit\");"],
"GtkMenuBar": ["widget.add_item(\"File\", std::vector<std::string>{\"New\", \"Open\", \"Quit\"});", "widget.add_item(\"Edit\", std::vector<std::string>{\"Copy\", \"Paste\"});"],
"GtkMenuButton": ["widget.set_text(\"Menu\");"],
"GtkMenuItem": ["widget.set_text(\"Open\");", "widget.set_accel(\"Ctrl+O\");", "widget.set_has_submenu(true);"],
"GtkPopover": ["widget.add_line(\"Hello\");", "widget.add_line(\"World\");", "widget.point_at(100, 100);", "widget.popup();"],
"GtkRadioMenuItem": ["widget.set_text(\"Option A\");", "widget.set_group(\"view\");", "widget.set_active(true);"],
"GtkSeparatorMenuItem": [],
"GtkToolbar": ["widget.add_item(\"New\");", "widget.add_item(\"Open\");", "widget.add_item(\"Save\");"],
"TActionList": ["widget.add_action(\"open\", \"Open\", \"Ctrl+O\");", "widget.add_action(\"save\", \"Save\", \"Ctrl+S\");", "widget.execute(\"open\");"],
"TMainMenu": ["widget.add_menu(\"File\", std::vector<std::string>{\"New\", \"Open\", \"Quit\"});", "widget.add_menu(\"Edit\", std::vector<std::string>{\"Copy\", \"Paste\"});"],
"TPopupMenu": ["widget.add_item(\"Cut\");", "widget.add_item(\"Copy\");", "widget.add_separator();", "widget.add_item(\"Paste\");", "widget.popup();"],
"TPopupNotifier": ["widget.show_message(\"Hello\", \"This is a notification.\");"],
"TToolBar": ["widget.add_button(\"New\");", "widget.add_button(\"Open\");", "widget.add_separator();", "widget.add_button(\"Save\");"],
"GtkEventBox": ["widget.set_text(\"EventBox\");", "widget.set_visible_window(true);"],
"TApplicationProperties": ["widget.set_title(\"Demo App\");", "widget.set_version(\"1.0\");", "widget.set_hint(\"A demo application\");"],
"XGraph": ["widget.add_line_series(\"Price\", Srgb{70, 110, 180}, std::vector<double>{1.0, 2.0, 1.5, 3.0, 2.5});", "widget.set_x_labels(std::vector<std::string>{\"Mon\", \"Tue\", \"Wed\", \"Thu\", \"Fri\"});", "widget.set_auto_y(true);"],
"XSpreadsheet": ["widget.set_size(6, 10);", "widget.set_cell_text(0, 0, \"Item\");", "widget.set_cell_text(0, 1, \"10\");", "widget.set_cell_text(0, 2, \"20\");", "widget.set_cell_text(0, 3, \"=SUM(A2:A3)\");", "widget.set_active_cell(0, 3);"],
}
CALLBACKS = {
"GtkComboBox": ["widget.on_changed = []{ std::printf(\"changed\\n\"); };"],
"GtkComboBoxText": ["widget.on_changed = []{ std::printf(\"changed\\n\"); };"],
"GtkDropDown": ["widget.on_changed = []{ std::printf(\"changed\\n\"); };"],
"GtkEditableLabel": ["widget.on_editing_done = []{ std::printf(\"editing done\\n\"); };"],
"GtkEntry": ["widget.on_changed = []{ std::printf(\"changed\\n\"); };", "widget.on_activate = []{ std::printf(\"activate\\n\"); };"],
"GtkEntryCompletion": ["widget.on_match_selected = [](const std::string& s){ std::printf(\"match: %s\\n\", s.c_str()); };"],
"GtkPasswordEntry": ["widget.on_activate = []{ std::printf(\"activate\\n\"); };"],
"GtkScale": ["widget.on_value_changed = []{ std::printf(\"value changed\\n\"); };"],
"GtkScaleButton": ["widget.on_value_changed = [](double v){ std::printf(\"value: %f\\n\", v); };"],
"GtkSearchBar": ["widget.on_search_changed = [](const std::string& s){ std::printf(\"search: %s\\n\", s.c_str()); };"],
"GtkSearchEntry": ["widget.on_search_changed = [](const std::string& s){ std::printf(\"search: %s\\n\", s.c_str()); };"],
"GtkSpinButton": ["widget.on_value_changed = [](double v){ std::printf(\"value: %f\\n\", v); };"],
"GtkTextView": ["widget.on_changed = []{ std::printf(\"changed\\n\"); };"],
"TCalcEdit": ["widget.on_change = []{ std::printf(\"changed\\n\"); };", "widget.on_button_click = []{ std::printf(\"button\\n\"); };"],
"TComboBox": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"TComboBoxEx": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"TDateEdit": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"TDirectoryEdit": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"TEdit": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"TEditButton": ["widget.on_button_click = []{ std::printf(\"button\\n\"); };"],
"TFileNameEdit": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"TFilterComboBox": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"TFloatSpinEdit": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"TLabeledEdit": [],
"TMaskEdit": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"TMemo": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"TSpinEdit": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"TValueListEditor": ["widget.on_change = [](int row){ std::printf(\"changed row %d\\n\", row); };"],
"GtkColumnView": ["widget.on_selection_changed = []{ std::printf(\"selection changed\\n\"); };"],
"GtkGridView": ["widget.on_selection_changed = []{ std::printf(\"selection changed\\n\"); };"],
"GtkIconView": ["widget.on_selection_changed = []{ std::printf(\"selection changed\\n\"); };", "widget.on_item_activated = []{ std::printf(\"activated\\n\"); };"],
"GtkListBox": ["widget.on_row_selected = [](int r){ std::printf(\"row %d\\n\", r); };"],
"GtkListView": ["widget.on_selection_changed = []{ std::printf(\"selection changed\\n\"); };"],
"GtkTreeView": ["widget.on_selection_changed = []{ std::printf(\"selection changed\\n\"); };"],
"TCheckGroup": ["widget.on_item_change = [](int i){ std::printf(\"item %d\\n\", i); };"],
"TCheckListBox": ["widget.on_click_check = [](int i){ std::printf(\"check %d\\n\", i); };"],
"TDrawGrid": ["widget.on_draw_cell = [](TDrawGrid*, int, int, int, int, int, int){};", "widget.on_select_cell = [](int c, int r){ std::printf(\"cell %d,%d\\n\", c, r); };"],
"THeaderControl": ["widget.on_section_click = [](int i){ std::printf(\"section %d\\n\", i); };"],
"TListBox": ["widget.on_selection_change = []{ std::printf(\"selection changed\\n\"); };"],
"TListView": ["widget.on_select = []{ std::printf(\"selected\\n\"); };"],
"TRadioGroup": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"TShellListView": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"TShellTreeView": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"TStringGrid": ["widget.on_select_cell = [](int c, int r){ std::printf(\"cell %d,%d\\n\", c, r); };"],
"TTreeView": ["widget.on_change = []{ std::printf(\"changed\\n\"); };"],
"GtkActionBar": ["widget.on_item_clicked = [](std::string s){ std::printf(\"clicked: %s\\n\", s.c_str()); };"],
"GtkCheckMenuItem": ["widget.on_toggled = []{ std::printf(\"toggled\\n\"); };"],
"GtkHeaderBar": ["widget.on_decoration_clicked = [](std::string s){ std::printf(\"decoration: %s\\n\", s.c_str()); };"],
"GtkMenu": ["widget.on_activate = [](int i){ std::printf(\"item %d\\n\", i); };"],
"GtkMenuBar": ["widget.on_activate = [](int m, int r){ std::printf(\"menu %d row %d\\n\", m, r); };"],
"GtkMenuButton": ["widget.on_click = []{ std::printf(\"clicked\\n\"); };"],
"GtkMenuItem": ["widget.on_click = []{ std::printf(\"clicked\\n\"); };"],
"GtkPopover": ["widget.on_show = []{ std::printf(\"shown\\n\"); };", "widget.on_hide = []{ std::printf(\"hidden\\n\"); };"],
"GtkRadioMenuItem": ["widget.on_toggled = []{ std::printf(\"toggled\\n\"); };"],
"GtkSeparatorMenuItem": [],
"GtkToolbar": ["widget.on_activate = [](int i){ std::printf(\"item %d\\n\", i); };"],
"TActionList": ["widget.on_click = []{ std::printf(\"clicked\\n\"); };"],
"TMainMenu": ["widget.on_menu_click = [](std::string s){ std::printf(\"menu: %s\\n\", s.c_str()); };"],
"TPopupMenu": ["widget.on_menu_click = [](std::string s){ std::printf(\"menu: %s\\n\", s.c_str()); };"],
"TPopupNotifier": ["widget.on_close = []{ std::printf(\"closed\\n\"); };"],
"TToolBar": ["widget.on_button_click = [](int i){ std::printf(\"button %d\\n\", i); };"],
"GtkEventBox": ["widget.on_click = []{ std::printf(\"clicked\\n\"); };"],
"TApplicationProperties": ["widget.on_exception = [](const std::string& m){ std::printf(\"exception: %s\\n\", m.c_str()); };"],
"XGraph": ["widget.on_view_change = [](double a, double b){ std::printf(\"view %f %f\\n\", a, b); };"],
"XSpreadsheet": ["widget.on_cell_change = [](int c, int r){ std::printf(\"cell %d,%d\\n\", c, r); };"],
}
TMPL = """#include "@NAME@.h"
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
    XStoreName(display, window, "@NAME@ - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    @NAME@ widget(display, screen, window, gc, &styles, @BOUNDS@);
@SETUP@@CALLBACKS@    widget.draw();
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
"""
OUT = "/tmp/dw65-examples"
os.makedirs(OUT, exist_ok=True)
names = sorted(set(list(BOUNDS.keys())))
assert len(names) == 65, len(names)
missing = [n for n in names if n not in SETUP or n not in CALLBACKS]
print("missing setup/callback entries:", missing)
for name in names:
    setup = "".join("    " + s + "\n" for s in SETUP[name])
    cbs = "".join("    " + s + "\n" for s in CALLBACKS[name])
    src = TMPL.replace("@NAME@", name).replace("@BOUNDS@", BOUNDS[name]).replace("@SETUP@", setup).replace("@CALLBACKS@", cbs)
    open(os.path.join(OUT, name + ".cpp"), "w").write(src)
print("wrote", len(names), "examples")
