import re
names = "GtkAboutDialog GtkAppChooserDialog GtkApplicationWindow GtkAssistant GtkColorChooserDialog GtkDialog GtkFileChooserDialog GtkFontChooserDialog GtkMessageDialog GtkPrintUnixDialog GtkShortcutsWindow GtkWindow TCalculatorDialog TCalendarDialog TColorDialog TFindDialog TFontDialog TForm TOpenDialog TOpenPictureDialog TPageSetupDialog TPrintDialog TPrinterSetupDialog TReplaceDialog TSaveDialog TSavePictureDialog TSelectDirectoryDialog TTaskDialog GtkAspectFrame GtkBox GtkButtonBox GtkCenterBox GtkExpander GtkFixed GtkFlowBox GtkFrame GtkGrid GtkLayout GtkNotebook GtkOverlay GtkPaned GtkRevealer GtkScrolledWindow GtkScrollbar GtkSeparator GtkStack GtkStackSidebar GtkStackSwitcher GtkViewport TControlBar TCoolBar TFlowPanel TFrame TGroupBox TNotebook TPageControl TPairSplitter TPanel TScrollBar TScrollBox TSplitter TTabControl".split()
COMMON = {"set_bounds","width","height","contains","set_visible","get_visible","set_enabled","get_enabled","set_style_name","style","draw","on_button_press","on_button_release","on_motion","on_key","set_text","get_text"}
old = open("/tmp/api62s.txt").read()
BARETYPE = re.compile(r"^(void|bool|int|double|float|std::string|Srgb|SStyle|unsigned|long|std::vector<std::string>|Rect)$")
for name in names:
    h=open("/opt/cwidgets/components/%s/%s.h" % (name,name)).read()
    raw=[l.rstrip() for l in h.splitlines() if l.strip()]
    lines=[]
    i=0
    while i < len(raw):
        s=raw[i].strip()
        if BARETYPE.match(s) and i+1 < len(raw):
            lines.append(s+" "+raw[i+1].strip()); i+=2; continue
        lines.append(s); i+=1
    # brace-depth tracking in public section only
    pub=False; depth=0
    for idx,l in enumerate(lines):
        s=l
        if s=="public:": pub=True; continue
        if s in ("private:","protected:"): pub=False; continue
        if not pub: continue
        if depth==0:
            m=re.match(r"std::function<([^;]+)>\s+(\w+)\s*;", s)
            if m:
                blk=old.split("===== %s " % name)[1].split("=====")[0]
                if ("SIG %s " % m.group(2)) not in blk:
                    print("%s: NEW-SIG %s : std::function<%s>" % (name, m.group(2), m.group(1)))
            m=re.match(r"((?:void|bool|int|unsigned|float|double|std::string|SStyle|Srgb|Rect|const char\*|char\*|long|std::vector<std::string>)\s+[\w:]+)\s*\(", s)
            if m and not s.startswith("//"):
                sig=s; j=idx
                while ")" not in sig and j+1<len(lines):
                    j+=1; sig+=" "+lines[j]
                p=sig.find(")")
                if p>=0: sig=sig[:p+1]
                mm=re.search(r"(\w+)\s*\(", sig)
                mname=mm.group(1) if mm else "?"
                if mname in COMMON or mname==name: pass
                else:
                    blk=old.split("===== %s " % name)[1].split("=====")[0]
                    if (mname+"(") not in blk:
                        print("%s: NEW-API %s" % (name, sig))
        depth += s.count("{") - s.count("}")
