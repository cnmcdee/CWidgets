import re, os, sys, subprocess
sys.path.insert(0, "/tmp")
import docdata
WIN = "GtkAboutDialog GtkAppChooserDialog GtkApplicationWindow GtkAssistant GtkColorChooserDialog GtkDialog GtkFileChooserDialog GtkFontChooserDialog GtkMessageDialog GtkPrintUnixDialog GtkShortcutsWindow GtkWindow TCalculatorDialog TCalendarDialog TColorDialog TFindDialog TFontDialog TForm TOpenDialog TOpenPictureDialog TPageSetupDialog TPrintDialog TPrinterSetupDialog TReplaceDialog TSaveDialog TSavePictureDialog TSelectDirectoryDialog TTaskDialog".split()
CON = "GtkAspectFrame GtkBox GtkButtonBox GtkCenterBox GtkExpander GtkFixed GtkFlowBox GtkFrame GtkGrid GtkLayout GtkNotebook GtkOverlay GtkPaned GtkRevealer GtkScrolledWindow GtkScrollbar GtkSeparator GtkStack GtkStackSidebar GtkStackSwitcher GtkViewport TControlBar TCoolBar TFlowPanel TFrame TGroupBox TNotebook TPageControl TPairSplitter TPanel TScrollBar TScrollBox TSplitter TTabControl".split()
ADDITIONS = {
 "GtkColorChooserDialog": ["Srgb get_color()"],
 "GtkBox": ["Rect child_rect(int i, int count)"],
 "GtkCenterBox": ["Rect start_rect()", "Rect center_rect()", "Rect end_rect()"],
 "GtkFixed": ["Rect child_rect(std::string name)"],
 "GtkGrid": ["Rect cell_rect(int col, int row, int col_count, int row_count)"],
 "GtkLayout": ["Rect visible_rect()"],
 "GtkNotebook": ["Rect tab_rect(int index)", "Rect page_rect()"],
 "GtkOverlay": ["Rect main_rect()", "Rect overlay_rect(int i)"],
 "GtkPaned": ["Rect first_pane_rect()", "Rect second_pane_rect()", "Rect handle_rect()"],
 "GtkStackSidebar": ["std::vector<std::string> get_page_names()", "Rect row_rect(int index)"],
 "GtkStackSwitcher": ["std::vector<std::string> get_page_names()", "Rect button_rect(int index)"],
 "TFrame": ["Rect client_rect()"],
}
RECT_WIDGETS = set(["GtkBox", "GtkCenterBox", "GtkFixed", "GtkGrid", "GtkLayout", "GtkNotebook", "GtkOverlay", "GtkPaned", "GtkStackSidebar", "GtkStackSwitcher", "TFrame", "TGroupBox"])
def parse_api():
    txt = open("/tmp/api62s.txt").read()
    out = {}
    for block in txt.split("===== ")[1:]:
        name = block.split(" ")[0]
        consts, sigs, apis = [], [], []
        for line in block.splitlines():
            s = line.strip()
            if s.startswith("CONST "): consts.append(s[6:])
            elif s.startswith("SIG "): sigs.append(s[4:])
            elif s.startswith("API "): apis.append(s[4:])
        apis += ADDITIONS.get(name, [])
        out[name] = (consts, sigs, apis)
    return out
API = parse_api()
def human(s):
    return s.replace("_", " ")
def mname(sig):
    m = re.search(r"(\w+)\s*\(", sig)
    return m.group(1) if m else "?"
def rtype(sig):
    return sig.split("(")[0].rsplit(" ", 1)[0].strip() if "(" in sig else ""
SIGDESC = {
 "on_response": "Fires with the response id when a button is pressed.",
 "on_click": "Fires when the widget is clicked.",
 "on_close": "Fires when the window is closed.",
 "on_minimize": "Fires when the window is minimised.",
 "on_maximize": "Fires when the window is maximised.",
 "on_menu": "Fires with the name of the clicked menu.",
 "on_apply": "Fires when Apply is pressed.",
 "on_page_changed": "Fires with the new page index.",
 "on_color_changed": "Fires when the picked colour changes.",
 "on_toggled": "Fires with the new expanded state.",
 "on_child_activated": "Fires with the index of the activated item.",
 "on_overlay_click": "Fires with the name of the clicked overlay.",
 "on_position_changed": "Fires with the new divider position.",
 "on_reveal_changed": "Fires with the new revealed state.",
 "on_value_changed": "Fires when the value changes.",
 "on_visible_child_changed": "Fires with the name of the newly visible page.",
 "on_page_selected": "Fires with the index of the selected page.",
 "on_band_move": "Fires when a band is dragged.",
 "on_reorder": "Fires after a drag reorder.",
 "on_change_page": "Fires when the page changes.",
 "on_change": "Fires when the selection changes.",
 "on_find": "Fires when Find Next is pressed.",
 "on_find_next": "Fires when Find Next is pressed.",
 "on_replace": "Fires when Replace is pressed.",
 "on_replace_all": "Fires when Replace All is pressed.",
 "on_button_clicked": "Fires with the index of the clicked button.",
}
OVERRIDE = {
 "respond": "Records the response id and fires on_response(int).",
 "run": "Marks the dialog running and visible; it does not block.",
 "get_response": "Returns the last response id, or RESPONSE_NONE (-1) if none yet.",
 "get_result": "Returns the modal result code (0 until the dialog is dismissed).",
 "get_modal_result": "Returns the modal result code.",
 "close": "Closes the window and fires on_close().",
 "iconify": "Minimises the window and fires on_minimize().",
 "deiconify": "Restores the window from minimised.",
 "maximize": "Grows the window to fill the host window and fires on_maximize().",
 "unmaximize": "Restores the bounds saved by maximise().",
 "is_closed": "Reports whether the window is closed.",
 "is_iconified": "Reports whether the window is minimised.",
 "is_maximized": "Reports whether the window is maximised.",
 "is_running": "Reports whether the dialog is running.",
 "is_animating": "Reports whether the reveal animation is still moving.",
 "is_dragging": "Reports whether a thumb drag is in progress.",
 "is_horizontal": "Reports whether the orientation is horizontal.",
 "is_vertical": "Reports whether the splitter is vertical.",
 "has_page": "Reports whether a page with the given name exists.",
 "revealed": "Reports whether the child is currently revealed.",
 "get_progress": "Returns the current reveal progress (0.0 to 1.0).",
 "get_hex": "Returns the current colour as a #RRGGBB string.",
 "get_date": "Returns the selected date as YYYY-MM-DD.",
 "set_date": "Sets the selected date (year, month, day).",
 "get_display": "Returns the calculator display text.",
 "get_value": "Returns the numeric value.",
 "set_value": "Sets the numeric value.",
 "match_case": "Reports whether match case is on.",
 "whole_words": "Reports whether whole words only is on.",
 "search_up": "Reports whether the search direction is up.",
 "find_text": "Returns the find text.",
 "replace_text": "Returns the replace text.",
 "get_find_count": "Returns how many Find Next searches have run.",
 "get_strikeout": "Reports whether strikeout is on.",
 "get_underline": "Reports whether underline is on.",
 "get_closed": "Reports whether the form is closed.",
 "get_minimized": "Reports whether the form is minimised.",
 "get_maximized": "Reports whether the form is maximised.",
 "get_duplex": "Reports whether duplex printing is on.",
 "get_properties_open": "Reports whether the properties pane is open.",
 "get_range_mode": "Returns the range mode of the selected radio.",
 "get_page_from": "Returns the first page of the range.",
 "get_page_to": "Returns the last page of the range.",
 "get_overwrite_confirmed": "Reports whether the overwrite was confirmed.",
 "get_confirm_showing": "Reports whether the overwrite prompt is showing.",
 "get_button_index": "Returns the index of the clicked button.",
 "get_radio_index": "Returns the index of the selected radio.",
 "get_margins": "Fills the left, top, right and bottom margins via out parameters.",
 "get_last_menu": "Returns the name of the last menu added.",
 "get_clicked_zone": "Returns which zone was clicked.",
 "get_dragging_band": "Returns the index of the band being dragged.",
 "get_drag_band": "Returns the index of the band being dragged.",
 "get_hadjust": "Returns the horizontal scroll offset.",
 "get_vadjust": "Returns the vertical scroll offset.",
 "get_scroll_x": "Returns the horizontal scroll offset.",
 "get_scroll_y": "Returns the vertical scroll offset.",
 "get_selected_col": "Returns the selected column.",
 "get_selected_row": "Returns the selected row.",
 "set_hscroll": "Sets the horizontal scroll offset.",
 "set_vscroll": "Sets the vertical scroll offset.",
 "set_hadjust": "Sets the horizontal scroll offset.",
 "set_vadjust": "Sets the vertical scroll offset.",
 "set_hpolicy": "Sets the horizontal scrollbar policy (0 never, 1 automatic, 2 always).",
 "set_vpolicy": "Sets the vertical scrollbar policy (0 never, 1 automatic, 2 always).",
 "get_hpolicy": "Returns the horizontal scrollbar policy.",
 "get_vpolicy": "Returns the vertical scrollbar policy.",
 "set_orientation": "Sets the orientation (0 horizontal, 1 vertical).",
 "get_orientation": "Returns the orientation (0 horizontal, 1 vertical).",
 "set_layout_style": "Sets the layout style (plain int; values unclear from the header).",
 "get_layout_style": "Returns the layout style (plain int; values unclear from the header).",
 "set_baseline_position": "Sets the baseline position (plain int; meaning unclear from the header).",
 "get_baseline_position": "Returns the baseline position (plain int; meaning unclear from the header).",
 "set_label_xalign": "Sets the label alignment along the top edge (0.0 to 1.0).",
 "get_label_xalign": "Returns the label alignment along the top edge.",
 "set_homogeneous": "Sets whether children share the space equally.",
 "get_homogeneous": "Reports whether children share the space equally.",
 "set_show_other": "Sets whether the Show other applications row is shown.",
 "get_show_other": "Reports whether the Show other applications row is shown.",
 "set_page_complete": "Sets whether a page is complete.",
 "get_page_complete": "Reports whether a page is complete.",
 "set_kind": "Sets the scrollbar kind (0 vertical, 1 horizontal; no named constants).",
 "get_kind": "Returns the scrollbar kind (0 vertical, 1 horizontal).",
 "set_aligned": "Sets which edge the splitter docks to (EAligned).",
 "set_resize_style": "Sets the resize style (plain int; values unclear from the header).",
 "get_resize_style": "Returns the resize style (plain int; values unclear from the header).",
 "set_tab_position": "Sets the tab strip position (plain int; values unclear from the header).",
 "get_tab_position": "Returns the tab strip position (plain int; values unclear from the header).",
 "set_flow_style": "Sets the flow direction (TFlowStyle).",
 "set_text2": "Sets the second pane caption (same as set_pane_text(1, ...)).",
 "get_text2": "Returns the second pane caption.",
 "set_action": "Sets the chooser action (ACTION_OPEN, ACTION_SAVE, ACTION_SELECT_FOLDER, ACTION_CREATE_FOLDER).",
 "add_filter": "Adds a filter name to the filter list.",
 "add_shortcut": "Adds one shortcut row to a section.",
 "add_application": "Adds one application (name and description) to the list.",
 "add_printer": "Adds a printer (name and status) to the list.",
 "add_button": "Adds a button with a label and a response id.",
 "clear_buttons": "Removes all buttons.",
 "clear_radios": "Removes all radio buttons.",
 "button_count": "Returns the number of buttons.",
 "item_count": "Returns the number of items.",
 "overlay_count": "Returns the number of overlays.",
 "band_count": "Returns the number of bands.",
 "child_count": "Returns the number of children.",
 "printer_count": "Returns the number of printers.",
 "section_count": "Returns the number of sections.",
 "page_count": "Returns the number of pages.",
 "tab_count": "Returns the number of tabs.",
 "get_n_pages": "Returns the number of pages.",
 "get_page_count": "Returns the number of pages.",
 "append_page": "Appends a page; returns its index.",
 "remove_page": "Removes the page at an index.",
 "delete_page": "Removes the page at an index.",
 "next_page": "Moves to the next page.",
 "previous_page": "Moves to the previous page.",
 "toggle": "Flips the expanded state.",
 "cycle": "Steps the visible page forward (positive delta) or back (negative delta).",
 "step": "Advances the reveal animation by 0.2 toward its target.",
 "clamp_position": "Clamps the divider position into range.",
 "clamp_align": "Clamps an alignment value into range.",
 "scroll_to": "Moves the view to x, y.",
 "put": "Places a named child at x, y with width and height.",
 "move": "Moves a named child to x, y; returns false when there is no such child.",
 "remove": "Removes a named child; returns false when there is no such child.",
 "find_child": "Returns the index of the named child, or -1 when it is not found.",
 "index_at": "Returns the index at a point.",
 "columns": "Returns the number of columns in the computed layout.",
 "rows": "Returns the number of rows in the computed layout.",
 "draw_zone": "Draws one zone (public helper used by draw()).",
 "draw_pane": "Draws one pane (public helper used by draw()).",
 "draw_band": "Draws one band (public helper used by draw()).",
 "draw_scrollbars": "Draws the scrollbars (public helper used by draw()).",
 "zone_contains": "Reports whether a point is inside the given zone rectangle.",
 "contains_client": "Reports whether a point is inside the client area.",
 "in_client": "Reports whether a point is inside the client area.",
 "near_handle": "Reports whether a point is on the divider handle.",
 "horz_visible": "Reports whether the horizontal scrollbar is showing.",
 "vert_visible": "Reports whether the vertical scrollbar is showing.",
 "set_show_close_buttons": "Sets whether tabs show close buttons.",
 "get_show_close_buttons": "Reports whether tabs show close buttons.",
 "set_show_caption": "Sets whether the caption is shown.",
 "get_show_caption": "Reports whether the caption is shown.",
 "set_auto_scroll": "Sets whether automatic scrolling is on.",
 "get_auto_scroll": "Reports whether automatic scrolling is on.",
 "set_reveal_child": "Sets whether the child is revealed.",
 "get_reveal_child": "Reports whether the child is revealed.",
 "set_transition_duration": "Sets the transition duration in milliseconds.",
 "get_transition_duration": "Returns the transition duration in milliseconds.",
}
WIDGET_OVERRIDES = {
 ("TTaskDialog", "add_button"): "Adds a button with a caption.",
 ("GtkLayout", "scroll_by"): "Scrolls the view by dx, dy pixels.",
 ("GtkScrollbar", "scroll_by"): "Moves the value by delta.",
 ("GtkShortcutsWindow", "scroll_by"): "Scrolls by a number of lines.",
 ("GtkScrollbar", "set_range"): "Sets the range and page size (upper, page).",
 ("TScrollBar", "set_range"): "Sets the range (lo, hi).",
 ("GtkAssistant", "append_page"): "Appends a page with a title, content and type; returns its index.",
 ("GtkNotebook", "append_page"): "Appends a page with the given label.",
 ("GtkFlowBox", "set_spacing"): "Sets the column and row spacing.",
 ("TSplitter", "on_moved"): "Fires with the new position.",
 ("TPairSplitter", "on_moved"): "Fires when the divider is moved.",
 ("GtkCenterBox", "start_rect"): "Returns the Start zone rectangle as a Rect.",
 ("GtkCenterBox", "center_rect"): "Returns the Center zone rectangle as a Rect.",
 ("GtkCenterBox", "end_rect"): "Returns the End zone rectangle as a Rect.",
 ("GtkAspectFrame", "set_obey_child"): "Sets whether the frame ratio follows the child ratio.",
 ("GtkAspectFrame", "get_obey_child"): "Reports whether the frame ratio follows the child ratio.",
}
RECTWORDS = {"rect": "rectangle", "bounds": "area"}
def describe(widget, sig):
    name = mname(sig)
    if (widget, name) in WIDGET_OVERRIDES:
        return WIDGET_OVERRIDES[(widget, name)]
    if name in OVERRIDE:
        return OVERRIDE[name]
    if "rect" in name or "bounds" in name:
        what = human(name).replace(" rect", " rectangle").replace(" bounds", " area")
        rt = rtype(sig)
        if rt == "Rect":
            return "Returns the " + what + " as a Rect {x1, y1, x2, y2}."
        if rt == "bool":
            return "Fills the " + what + " via out parameters; returns false when the index is out of range."
        return "Fills the " + what + " via out parameters."
    if name.startswith("client_"):
        return "Returns the client area " + human(name[7:]) + "."
    if name.startswith("content_"):
        return "Returns the content area " + human(name[8:]) + "."
    if name.startswith("set_"):
        return "Sets the " + human(name[4:]) + "."
    if name.startswith("get_"):
        return "Returns the " + human(name[4:]) + "."
    if name.startswith("is_") or name.startswith("has_"):
        return "Reports whether " + human(name[3:]) + "."
    if name.startswith("add_") or name.startswith("append_"):
        return "Adds " + human(name[4:]) + "."
    if name.startswith("remove_") or name.startswith("delete_"):
        return "Removes " + human(name.split("_", 1)[1]) + "."
    return "See the header for details."
def sigdesc(widget, sig):
    m = re.match(r"std::function<([^;]+)>\s+(\w+)\s*;", sig)
    if not m:
        return ""
    name = m.group(2)
    if (widget, name) in WIDGET_OVERRIDES:
        return WIDGET_OVERRIDES[(widget, name)]
    return SIGDESC.get(name, "Fires when the widget state changes.")
TEMPLATE = []
TEMPLATE.append("#include \"__NAME__.h\"")
TEMPLATE.append("#include <cstdio>")
TEMPLATE.append("")
TEMPLATE.append("int main()")
TEMPLATE.append("{")
TEMPLATE.append("    Display* display = XOpenDisplay(nullptr);")
TEMPLATE.append("    if (!display) { return 1; }")
TEMPLATE.append("    int screen = DefaultScreen(display);")
TEMPLATE.append("    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));")
TEMPLATE.append("    XStoreName(display, window, \"__NAME__ - XFrame demo\");")
TEMPLATE.append("    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);")
TEMPLATE.append("    Atom deleteWindowMessage = XInternAtom(display, \"WM_DELETE_WINDOW\", False);")
TEMPLATE.append("    XSetWMProtocols(display, window, &deleteWindowMessage, 1);")
TEMPLATE.append("    XMapWindow(display, window);")
TEMPLATE.append("    GC gc = DefaultGC(display, screen);")
TEMPLATE.append("    SStyle_Manager styles(display, window, screen, gc);")
TEMPLATE.append("    styles.add_style_ex(\"default\", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, \"-misc-fixed-medium-r-normal--13-*\");")
TEMPLATE.append("    styles.set_active_style_byName(\"default\");")
TEMPLATE.append("    __NAME__ widget(display, screen, window, gc, &styles, 20, 20, 760, 560);")
TEMPLATE.append("__SETUP__")
TEMPLATE.append("__CALLBACK__")
TEMPLATE.append("    widget.draw();")
TEMPLATE.append("    bool running = true;")
TEMPLATE.append("    while (running)")
TEMPLATE.append("    {")
TEMPLATE.append("        XEvent event;")
TEMPLATE.append("        XNextEvent(display, &event);")
TEMPLATE.append("        bool handled = false;")
TEMPLATE.append("        switch (event.type)")
TEMPLATE.append("        {")
TEMPLATE.append("        case Expose:")
TEMPLATE.append("            handled = true;")
TEMPLATE.append("            break;")
TEMPLATE.append("        case ButtonPress:")
TEMPLATE.append("            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);")
TEMPLATE.append("            break;")
TEMPLATE.append("        case ButtonRelease:")
TEMPLATE.append("            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);")
TEMPLATE.append("            break;")
TEMPLATE.append("        case MotionNotify:")
TEMPLATE.append("            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);")
TEMPLATE.append("            break;")
TEMPLATE.append("        case KeyPress:")
TEMPLATE.append("        {")
TEMPLATE.append("            char buffer[32];")
TEMPLATE.append("            KeySym key = 0;")
TEMPLATE.append("            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);")
TEMPLATE.append("            buffer[n] = 0;")
TEMPLATE.append("            handled = widget.on_key(key, buffer);")
TEMPLATE.append("            break;")
TEMPLATE.append("        }")
TEMPLATE.append("        case ClientMessage:")
TEMPLATE.append("            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)")
TEMPLATE.append("            {")
TEMPLATE.append("                running = false;")
TEMPLATE.append("            }")
TEMPLATE.append("            break;")
TEMPLATE.append("        }")
TEMPLATE.append("        if (handled)")
TEMPLATE.append("        {")
TEMPLATE.append("            XClearWindow(display, window);")
TEMPLATE.append("            widget.draw();")
TEMPLATE.append("            XFlush(display);")
TEMPLATE.append("        }")
TEMPLATE.append("    }")
TEMPLATE.append("    XCloseDisplay(display);")
TEMPLATE.append("    return 0;")
TEMPLATE.append("}")
def make_example(name):
    desc, notes, setup, callback = docdata.DATA[name]
    lines = []
    for t in TEMPLATE:
        t = t.replace("__NAME__", name)
        if t == "__SETUP__":
            for s in setup:
                lines.append("    " + s)
            continue
        if t == "__CALLBACK__":
            if callback:
                lines.append("    " + callback)
            continue
        lines.append(t)
    return "\n".join(lines) + "\n"
BT = chr(96)
def sigdesc(widget, sig):
    m = re.match(r"(\w+) : std::function<([^;]+)>", sig)
    if not m:
        return ""
    name = m.group(1)
    if (widget, name) in WIDGET_OVERRIDES:
        return WIDGET_OVERRIDES[(widget, name)]
    return SIGDESC.get(name, "Fires when the widget state changes.")
def make_doc(section, names):
    out = []
    out.append("# " + section)
    out.append("")
    if section == "Windows & Dialogs":
        out.append("Top level windows and modal style dialogs: application windows, forms, file and colour and font choosers, message boxes, wizards and the Lazarus style T-prefixed dialogs. Every widget here draws its own window chrome inside the host window, with a title bar, buttons and a client area.")
    else:
        out.append("Containers that arrange other content: boxes, grids, notebooks, stacks, splitters, scroll areas and frames. Most of these draw labelled sample children (slots, tiles, pages, panes) rather than hosting real child widgets, so their API is about geometry, counts and selection.")
    out.append("")
    out.append("> Every widget here shares the common XFrame widget API - same constructor, set_bounds/draw/event handlers, visible/enabled state. The shared contract is documented in docs/index.md (written by the coordinator). Each subsection below lists only what is specific to that widget.")
    out.append("")
    for name in names:
        desc, notes, setup, callback = docdata.DATA[name]
        consts, sigs, apis = API[name]
        out.append("## " + name)
        out.append("")
        out.append(desc)
        out.append("")
        out.append("**Constructor:** " + BT + name + "(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)" + BT)
        out.append("")
        out.append("**API** (widget-specific methods/properties/signals ONLY - skip the common ones):")
        for s in sigs:
            m = re.match(r"(\w+) : std::function<([^;]+)>", s)
            out.append("- signal " + BT + m.group(1) + BT + " (" + BT + "std::function<" + m.group(2) + ">" + BT + ") - " + sigdesc(name, s))
        for a in apis:
            out.append("- " + BT + a + BT + " - " + describe(name, a))
        if consts:
            out.append("- Constants: " + ", ".join(BT + c + BT for c in consts))
        out.append("")
        out.append("**Example:**")
        out.append(BT + BT + BT + "cpp")
        out.append(make_example(name))
        out.append(BT + BT + BT)
        out.append("")
        n = notes
        if name in RECT_WIDGETS:
            n = n + " Rect is the widget own nested struct { int x1, y1, x2, y2; }."
        out.append("**Notes:** " + n)
        out.append("")
    return "\n".join(out)
def main():
    os.makedirs("/opt/cwidgets/docs", exist_ok=True)
    os.makedirs("/tmp/doc-examples", exist_ok=True)
    open("/opt/cwidgets/docs/windows-dialogs.md", "w").write(make_doc("Windows & Dialogs", WIN))
    open("/opt/cwidgets/docs/containers-layout.md", "w").write(make_doc("Containers & Layout", CON))
    fails = []
    ok = 0
    for name in WIN + CON:
        src = "/tmp/doc-examples/" + name + ".cpp"
        open(src, "w").write(make_example(name))
        cmd = ["g++", "-std=c++17", "-I", "/opt/cwidgets/components/" + name, src, "-lX11", "-o", "/tmp/doc-examples/" + name]
        r = subprocess.run(cmd, capture_output=True, text=True)
        if r.returncode == 0:
            ok += 1
        else:
            fails.append(name)
            print("FAIL " + name + ": " + (r.stderr.strip().splitlines() or ["?"])[0][:400])
    print("BUILD " + str(ok) + "/62 ok, fails: " + str(fails))
    for name in WIN + CON:
        consts, sigs, apis = API[name]
        if not apis and not sigs:
            print("EMPTY-API " + name)
    print("DOCS-WRITTEN")
main()
