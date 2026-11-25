import re, os
BASE = "/opt/cwidgets/components"
NAMES = """GtkComboBox GtkComboBoxText GtkDropDown GtkEditableLabel GtkEntry GtkEntryCompletion GtkPasswordEntry GtkScale GtkScaleButton GtkSearchBar GtkSearchEntry GtkSpinButton GtkTextView TCalcEdit TComboBox TComboBoxEx TDateEdit TDirectoryEdit TEdit TEditButton TFileNameEdit TFilterComboBox TFloatSpinEdit TLabeledEdit TMaskEdit TMemo TSpinEdit TValueListEditor GtkColumnView GtkGridView GtkIconView GtkListBox GtkListView GtkTreeView TCheckGroup TCheckListBox TDrawGrid THeaderControl TListBox TListView TRadioGroup TShellListView TShellTreeView TStringGrid TTreeView GtkActionBar GtkCheckMenuItem GtkHeaderBar GtkMenu GtkMenuBar GtkMenuButton GtkMenuItem GtkPopover GtkRadioMenuItem GtkSeparatorMenuItem GtkToolbar TActionList TMainMenu TPopupMenu TPopupNotifier TToolBar GtkEventBox TApplicationProperties XGraph XSpreadsheet""".split()
# broad: any "Type name(args) {" or "Type name(args);" in public region, where Type is a word possibly with * & <...> and name is not a keyword
for name in NAMES:
    txt = open(os.path.join(BASE, name, name + ".h"), errors="ignore").read()
    nc = re.sub(r"/\*.*?\*/", "", txt, flags=re.S)
    nc = re.sub(r"//[^\n]*", "", nc)
    norm = re.sub(r"\s+", " ", nc)
    pub = norm.split("public:", 1)[1] if "public:" in norm else norm
    # find all word name( patterns preceded by a type-ish token
    cands = re.findall(r"([A-Za-z_][\w:<>,\s\*&]*?)\b([A-Za-z_]\w*)\s*\(([^()]*)\)\s*(?:const\s*)?(?:\{|;)", pub)
    found = set()
    for typ, m, args in cands:
        typ = typ.strip()
        # last word of typ is the return type
        rt = typ.split()[-1] if typ.split() else ""
        if m in ("if", "for", "while", "switch", "return", "sizeof", "catch"):
            continue
        if rt in ("void", "bool", "int", "double", "float", "std::string", "SStyle", "Srgb", "long", "char", "unsigned", "size_t"):
            continue
        # skip ctor/dtor and known private helpers called like functions inside bodies (they appear with { after)
        found.add((rt, m, args.strip()))
    if found:
        print(name + ":")
        for rt, m, args in sorted(found):
            print("   RET=" + rt + " METHOD=" + m + "(" + args + ")")
