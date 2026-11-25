import re, os, bisect
BASE = "/opt/cwidgets/components"
DOCS = "/opt/cwidgets/docs"
NAMES = """GtkComboBox GtkComboBoxText GtkDropDown GtkEditableLabel GtkEntry GtkEntryCompletion GtkPasswordEntry GtkScale GtkScaleButton GtkSearchBar GtkSearchEntry GtkSpinButton GtkTextView TCalcEdit TComboBox TComboBoxEx TDateEdit TDirectoryEdit TEdit TEditButton TFileNameEdit TFilterComboBox TFloatSpinEdit TLabeledEdit TMaskEdit TMemo TSpinEdit TValueListEditor GtkColumnView GtkGridView GtkIconView GtkListBox GtkListView GtkTreeView TCheckGroup TCheckListBox TDrawGrid THeaderControl TListBox TListView TRadioGroup TShellListView TShellTreeView TStringGrid TTreeView GtkActionBar GtkCheckMenuItem GtkHeaderBar GtkMenu GtkMenuBar GtkMenuButton GtkMenuItem GtkPopover GtkRadioMenuItem GtkSeparatorMenuItem GtkToolbar TActionList TMainMenu TPopupMenu TPopupNotifier TToolBar GtkEventBox TApplicationProperties XGraph XSpreadsheet""".split()
doc_text = ""
for f in os.listdir(DOCS):
    doc_text += open(os.path.join(DOCS, f)).read()
documented = set(re.findall(r"`([A-Za-z_]\w*)\s*\(", doc_text))
documented |= set(re.findall(r"signal `([A-Za-z_]\w*)`", doc_text))
COMMON = {"set_bounds", "width", "height", "contains", "set_visible", "get_visible", "set_enabled", "get_enabled", "set_style_name", "style", "draw", "on_button_press", "on_button_release", "on_motion", "on_key"}
KEYWORDS = {"if", "for", "while", "switch", "return", "sizeof", "catch", "else"}
def public_methods(name):
    txt = open(os.path.join(BASE, name, name + ".h"), errors="ignore").read()
    nc = re.sub(r"/\*.*?\*/", "", txt, flags=re.S)
    nc = re.sub(r"//[^\n]*", "", nc)
    lines = nc.split("\n")
    access = []
    cur = "private"
    for line in lines:
        s = line.strip()
        if s.startswith("public:"):
            cur = "public"
        elif s.startswith("private:"):
            cur = "private"
        elif s.startswith("protected:"):
            cur = "protected"
        access.append(cur)
    line_starts = []
    off = 0
    for line in lines:
        line_starts.append(off)
        off += len(line) + 1
    def acc_at(pos):
        i = bisect.bisect_right(line_starts, pos) - 1
        return access[max(i, 0)]
    # brace depth per offset: depth after processing each char; compute depth at each position
    depth = 0
    depths = []
    instr = False
    esc = False
    for ch in nc:
        depths.append(depth)
        if instr:
            if esc:
                esc = False
            elif ch == chr(92):
                esc = True
            elif ch == chr(34):
                instr = False
        else:
            if ch == chr(34):
                instr = True
            elif ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
    out = {}
    for m in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", nc):
        mname = m.group(1)
        if mname in KEYWORDS:
            continue
        if depths[m.start()] != 1:
            continue
        if acc_at(m.start()) != "public":
            continue
        if mname == name or mname == "~" + name:
            continue
        # balanced parens + what follows
        d2 = 0
        i = m.end() - 1
        ok = False
        while i < len(nc):
            c = nc[i]
            if c == "(":
                d2 += 1
            elif c == ")":
                d2 -= 1
                if d2 == 0:
                    ok = True
                    break
            i += 1
        if not ok:
            continue
        after = nc[i+1:i+12].strip()
        if not (after.startswith("{") or after.startswith(";") or after.startswith("const")):
            continue
        before = nc[max(0, m.start()-80):m.start()].replace("\n", " ")
        tail = before.rstrip()
        if tail and tail[-1] in ".->=,;:":
            continue
        out[mname] = after[:6]
    return out
for name in NAMES:
    pm = public_methods(name)
    undoc = {m for m in pm if m not in COMMON and m not in documented}
    if undoc:
        print(name + ": " + str(sorted(undoc)))
print("DONE")
