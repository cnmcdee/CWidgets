import os
D = "/opt/cwidgets/docs"
def patch(fname, pairs):
    p = os.path.join(D, fname)
    t = open(p).read()
    for old, new in pairs:
        assert old in t, "NOT FOUND in " + fname + ": " + old[:90]
        t = t.replace(old, new, 1)
    open(p, "w").write(t)
    print(fname + ": patched " + str(len(pairs)) + " spots")

# --- text-entry.md ---
patch("text-entry.md", [
# TComboBox items()
("- `get_cursor_pos()` — caret position in the editable text",
 "- `get_cursor_pos()` — caret position in the editable text\n- `std::vector<std::string>& items()` — direct (mutable) access to the item list"),
# TLabeledEdit get_label_position + rects
("- `set_label_position(TLabelPosition p)` — lpAbove (default), lpBelow, lpLeft or lpRight (the `TLabelPosition` enum)",
 "- `set_label_position(TLabelPosition p)` — lpAbove (default), lpBelow, lpLeft or lpRight (the `TLabelPosition` enum)\n- `get_label_position()` — the current label position as a `TLabelPosition`\n- `label_rect()` / `edit_rect()` — the label / edit rectangles as `TLabeledEditRect` (`{x1, y1, x2, y2}`), computed from the label position"),
# TMemo lines()
("- `get_top_line()` — first visible line (the scroll position)",
 "- `get_top_line()` — first visible line (the scroll position)\n- `std::vector<std::string>& lines()` — direct (mutable) access to the lines"),
])
# --- views-lists.md ---
patch("views-lists.md", [
# GtkTreeView make_node + corrected note + ctor sample in description
("- `append_child(int root_index, std::vector<std::string> cells)` — adds a child under a root",
 "- `append_child(int root_index, std::vector<std::string> cells)` — adds a child under a root\n- `Node make_node(std::vector<std::string> cells)` — build a node value from a cell vector (used by the constructor for its sample tree)"),
("Children can only be added under roots (append_child takes a root index) — there is no add-grandchild method; deeper trees are built through the public `Node` struct and set_expanded_rec().",
 "append_child only adds children under roots, so the public API builds two-level trees; the constructor itself builds its sample tree with make_node() plus direct children.push_back() on local Node values, but there is no public method to insert such a node — the root list is private. The constructor pre-fills a sample tree: Documents (report.odt, budget.ods), Pictures (holiday.png, cat.jpg), Music (song.flac) and notes.txt."),
# TDrawGrid cell_rect
("- `col_left(int c)` / `row_top(int r)` — pixel position of a column / row\n- `cell_at(int x, int y, int& out_col, int& out_row)` — cell under a point",
 "- `col_left(int c)` / `row_top(int r)` — pixel position of a column / row\n- `cell_rect(int c, int r)` — rectangle of one cell as a `GridRect` (`{x1, y1, x2, y2}`)\n- `cell_at(int x, int y, int& out_col, int& out_row)` — cell under a point"),
# TListBox items()
("- `count()` — number of items",
 "- `count()` — number of items\n- `std::vector<std::string>& items()` — direct (mutable) access to the item list"),
# TRadioGroup items()
("- `get_item(int index)` — caption of one item",
 "- `get_item(int index)` — caption of one item\n- `std::vector<std::string>& items()` — direct (mutable) access to the item list"),
# TStringGrid cell_rect
("- `col_left(int c)` / `row_top(int r)` — pixel position of a column / row",
 "- `col_left(int c)` / `row_top(int r)` — pixel position of a column / row\n- `cell_rect(int c, int r)` — rectangle of one cell as a `GridRect` (`{x1, y1, x2, y2}`)"),
])
# --- menus-toolbars.md ---
patch("menus-toolbars.md", [
# TActionList accessors
("- `find_index(const std::string& name)` — index of an action by name, -1 when absent",
 "- `find_index(const std::string& name)` — index of an action by name, -1 when absent\n- `TAction* get_action(std::string name)` / `TAction* get_action_at(int index)` — pointer to one action (null when there is none); the public `TAction` value holds its name, caption, shortcut, enabled flag and its own `on_execute` callback\n- `set_on_execute(std::string name, std::function<void()> fn)` — attach the callback run when the named action is executed"),
])
# --- misc-x.md ---
patch("misc-x.md", [
# XGraph helpers
("- `fire_view_change()` — fire on_view_change() with the current view",
 "- `fire_view_change()` — fire on_view_change() with the current view\n- `static double nice_step(double range, int target)` — a \"nice\" axis step for a range (used by draw() for the grid lines)\n- `static std::string fmt(double v)` — formats a value for the axis labels (used by draw())"),
# XSpreadsheet helpers + corrected error note
("- `begin_edit(bool replace)` / `commit_edit()` / `cancel_edit()` — start / finish / abandon editing the active cell",
 "- `begin_edit(bool replace)` / `commit_edit()` / `cancel_edit()` — start / finish / abandon editing the active cell\n- `static std::string col_name(int col)` — column letters for an index (\"A\", \"B\", ...)\n- `static std::string format_number(double v)` — formats a numeric cell for display\n- `static bool parse_ref(const std::string& s, size_t& pos, int& col, int& row)` — parse an A1-style cell reference out of a formula string"),
("Formula errors (bad reference, cycle, parse failure) mark the cell as an error and display as such; get_cell_value() returns 0 for them — the exact display text for an error cell is unclear from the header.",
 "A failed formula (bad reference, circular reference, parse failure) marks the cell as an error and evaluates to 0.0; the exact text drawn for an error cell is unclear from the header."),
])
print("ALL PATCHES APPLIED")
