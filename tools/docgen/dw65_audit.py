import re, os
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
    access = []
    cur = "private"
    for line in nc.split("\n"):
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
    for line in nc.split("\n"):
        line_starts.append(off)
        off += len(line) + 1
    import bisect
    def acc_at(pos):
        i = bisect.bisect_right(line_starts, pos) - 1
        return access[max(i, 0)]
    out = {}
    for m in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", nc):
        mname = m.group(1)
        if mname in KEYWORDS:
            continue
        depth = 0
        i = m.end() - 1
        ok = False
        while i < len(nc):
            c = nc[i]
            if c == "(":
                depth += 1
            elif c == ")":
                depth -= 1
                if depth == 0:
                    ok = True
                    break
            i += 1
        if not ok:
            continue
        after = nc[i+1:i+12].strip()
        if not (after.startswith("{") or after.startswith(";") or after.startswith("const")):
            continue
        before = nc[max(0, m.start()-120):m.start()]
        before = before.replace("\n", " ")
        if mname == name or mname == "~" + name:
            continue
        if acc_at(m.start()) != "public":
            continue
        tail = before.rstrip()
        if tail and tail[-1] in ".->=,;:":
            continue
        out[mname] = after[:6]
    return out
missing_total = 0
for name in NAMES:
    pm = public_methods(name)
    undoc = {m for m in pm if m not in COMMON and m not in documented}
    if undoc:
        print(name + " UNDOCUMENTED-PUBLIC: " + str(sorted(undoc)))
        missing_total += len(undoc)
print("total undocumented public methods:", missing_total)
