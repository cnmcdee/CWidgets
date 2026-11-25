import re, os, sys
names = """GtkButton GtkCheckButton GtkColorButton GtkColorDialogButton GtkFileChooserButton GtkFontButton GtkLinkButton GtkLockButton GtkModelButton GtkRadioButton GtkSwitch GtkToggleButton GtkToggleToolButton GtkToolButton GtkVolumeButton TBitBtn TButton TButtonPanel TCheckBox TCheckComboBox TRadioButton TSpeedButton TToggleBox TUpDown GtkAccelLabel GtkCalendar GtkCellView GtkDrawingArea GtkGLArea GtkImage GtkInfoBar GtkInscription GtkInvisible GtkLabel GtkLevelBar GtkMediaControls GtkPicture GtkProgressBar GtkSpinner GtkStatusbar GtkVideo TArrow TBevel TCalendar TDateTimePicker TImage TImageList TLabel TPaintBox TProgressBar TShape TStaticText TStatusBar TTimeEdit TTimer TTrayIcon TTrackBar GtkAppChooserButton GtkAppChooserWidget GtkColorChooserWidget GtkFileChooserWidget GtkFontChooserWidget GtkPlacesSidebar TColorBox TColorButton TColorListBox TFileListBox""".split()
COMMON = {"set_bounds","width","height","contains","set_visible","get_visible","set_enabled","get_enabled","set_style_name","style","draw","on_button_press","on_button_release","on_motion","on_key"}
for n in names:
    h = f"/opt/cwidgets/components/{n}/{n}.h"
    d = f"/opt/cwidgets/components/{n}/main_{n}.cpp"
    print(f"\n{'='*100}\n### {n}")
    if not os.path.exists(h):
        print("MISSING HEADER"); continue
    txt = open(h).read()
    m = re.search(r"\bpublic\s*:", txt)
    pub = txt[m.start():] if m else txt
    pub1 = re.sub(r"\s+", " ", pub)
    for mm in re.finditer(r"(std::function<[^;]+?>|(?:void|bool|int|double|float|std::string|unsigned long|long|char|const char\*|SStyle|KeySym|Window|Display\*|GC)[^;{}()]*?\([^;{}]*\))\s*(?:;|\{)", pub1):
        s = re.sub(r"\s+", " ", mm.group(1)).strip()
        nm = re.search(r"(\w+)\s*\(", s)
        name = nm.group(1) if nm else "?"
        if name in COMMON and "std::function" not in s:
            continue
        if name == n:
            print(f"  CTOR: {s[:220]}")
            continue
        print(f"  API: {s[:320]}")
    for mm in re.finditer(r"(std::vector<[^;]+>|std::string\s+\w+\s*(?:=|;)|int\s+\w+\s*(?:=|;)|bool\s+\w+\s*(?:=|;)|double\s+\w+\s*(?:=|;))", pub1):
        s = re.sub(r"\s+", " ", mm.group(0)).strip()
        if any(k in s for k in ["display_","screen_","window_","gc_","styles_","x1_","y1_","x2_","y2_","name_","style_name_","visible_","enabled_","focused_","pressed_"]):
            continue
        print(f"  FIELD: {s[:200]}")
    di = txt.find("void draw()")
    if di >= 0:
        body = re.sub(r"\s+"," ", txt[di:di+2600].replace("\n"," "))
        print(f"  DRAW: {body[:1500]}")
    else:
        print("  DRAW: NO draw() FOUND")
    if os.path.exists(d):
        dtxt = open(d).read()
        print("  DEMO-USAGE:")
        for line in dtxt.splitlines():
            s = line.strip()
            if not s: continue
            if ("widget." in s or "widget->" in s) and not any(k in s for k in ["on_button_press","on_button_release","on_motion","on_key",".draw()"]):
                print(f"    {s[:220]}")
        for line in dtxt.splitlines():
            s=line.strip()
            if re.search(rf"{n}\s+widget", s):
                print(f"    CTORLINE: {s[:220]}")
    else:
        print("  DEMO MISSING")
