import re, os
import os
REPO_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
BASE=REPO_ROOT + "/widgets/components"
NAMES="""GtkComboBox GtkComboBoxText GtkDropDown GtkEditableLabel GtkEntry GtkEntryCompletion GtkPasswordEntry GtkScale GtkScaleButton GtkSearchBar GtkSearchEntry GtkSpinButton GtkTextView TCalcEdit TComboBox TComboBoxEx TDateEdit TDirectoryEdit TEdit TEditButton TFileNameEdit TFilterComboBox TFloatSpinEdit TLabeledEdit TMaskEdit TMemo TSpinEdit TValueListEditor GtkColumnView GtkGridView GtkIconView GtkListBox GtkListView GtkTreeView TCheckGroup TCheckListBox TDrawGrid THeaderControl TListBox TListView TRadioGroup TShellListView TShellTreeView TStringGrid TTreeView GtkActionBar GtkCheckMenuItem GtkHeaderBar GtkMenu GtkMenuBar GtkMenuButton GtkMenuItem GtkPopover GtkRadioMenuItem GtkSeparatorMenuItem GtkToolbar TActionList TMainMenu TPopupMenu TPopupNotifier TToolBar GtkEventBox TApplicationProperties XGraph XSpreadsheet""".split()
COMMON={"set_bounds","width","height","contains","set_visible","get_visible","set_enabled","get_enabled","set_style_name","style","draw","on_button_press","on_button_release","on_motion","on_key"}
STD="Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height"
for name in NAMES:
    hp=os.path.join(BASE,name,name+".h")
    dp=os.path.join(BASE,name,"main_"+name+".cpp")
    txt=open(hp,errors="ignore").read()
    _nc=re.sub(r"/\*.*?\*/","",txt,flags=re.S)
    _nc=re.sub(r"//[^" + chr(10) + r"]*","",_nc)
    norm=re.sub(r"\s+"," ",_nc)
    pub = norm.split("public:",1)[1] if "public:" in norm else norm
    funcs=re.findall(r"std::function<([^;]*?)>\s*(\w+)\s*;", pub)
    meths=re.findall(r"(?:^|;|\{|\})\s*(void|bool|int|double|float|std::string|std::vector<[^;{}]*>|SStyle|unsigned long|long|char|Window|Display\*|GC|Srgb|time_t)\s+(\w+)\s*\(([^)]*)\)\s*(?:const)?\s*(?:\{|;)", pub)
    ctor = re.search(re.escape(name)+r"\s*\(([^)]*)\)", norm)
    cdiff = "CTOR-DIFF" if (ctor and STD not in ctor.group(0)) else "std"
    print("="*100)
    print("### "+name+" hdr_lines="+str(txt.count(chr(10)))+" ctor="+cdiff)
    if cdiff=="CTOR-DIFF" and ctor: print("  CTOR: "+ctor.group(0)[:300])
    print("  SIGNALS: "+str(funcs))
    uniq=[]; seen=set()
    for ret,m,args in meths:
        if m in COMMON or m==name or m.startswith("~"): continue
        key=(ret,m,args.strip())
        if key not in seen: seen.add(key); uniq.append(key)
    print("  METHODS(%d):"%len(uniq))
    for ret,m,args in uniq: print("    "+ret+" "+m+"("+args.strip()+")")
    # public fields: look in public section for "type name;" or "type name ="
    flds=re.findall(r"(?:^|;|\})\s*(std::string|int|bool|double|float|std::vector<[^;{}]*>|Srgb|long)\s+(\w+)\s*(?:=[^;{}]*)?;", pub)
    # filter out locals mistakenly caught: only keep fields declared before first method body? keep all, dedup
    print("  FIELDS: "+str(sorted(set(flds))))
    di=norm.find("void draw()")
    if di>=0:
        body=norm[di:di+1100]
        print("  DRAW: "+body[:1400])
    else:
        print("  DRAW: NO draw() FOUND")
    if os.path.exists(dp):
        d=open(dp,errors="ignore").read()
        print("  DEMO:")
        for line in d.splitlines():
            s=line.strip()
            if not s: continue
            if "(display, screen, window, gc, &styles," in s:
                print("    CTOR: "+s)
            elif ".popup(" in s or ".show(" in s or ".set_" in s or ".add_" in s or ".append" in s or ".insert" in s or ".select" in s or ".clear" in s or ".load" in s or ".open" in s or ".refresh" in s or ".update" in s or ".remove_" in s or ".set" in s:
                print("    CALL: "+s)

