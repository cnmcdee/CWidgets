import re, os, sys
import os
REPO_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
names_win = "GtkAboutDialog GtkAppChooserDialog GtkApplicationWindow GtkAssistant GtkColorChooserDialog GtkDialog GtkFileChooserDialog GtkFontChooserDialog GtkMessageDialog GtkPrintUnixDialog GtkShortcutsWindow GtkWindow TCalculatorDialog TCalendarDialog TColorDialog TFindDialog TFontDialog TForm TOpenDialog TOpenPictureDialog TPageSetupDialog TPrintDialog TPrinterSetupDialog TReplaceDialog TSaveDialog TSavePictureDialog TSelectDirectoryDialog TTaskDialog".split()
names_con = "GtkAspectFrame GtkBox GtkButtonBox GtkCenterBox GtkExpander GtkFixed GtkFlowBox GtkFrame GtkGrid GtkLayout GtkNotebook GtkOverlay GtkPaned GtkRevealer GtkScrolledWindow GtkScrollbar GtkSeparator GtkStack GtkStackSidebar GtkStackSwitcher GtkViewport TControlBar TCoolBar TFlowPanel TFrame TGroupBox TNotebook TPageControl TPairSplitter TPanel TScrollBar TScrollBox TSplitter TTabControl".split()
for name in names_win+names_con:
    hpath=REPO_ROOT + "/widgets/components/%s/%s.h" % (name,name)
    dpath=REPO_ROOT + "/widgets/components/%s/main_%s.cpp" % (name,name)
    print("===== %s =====" % name)
    try:
        h=open(hpath).read()
    except Exception as e:
        print("  NO HEADER",e); continue
    lines=[l.rstrip() for l in h.splitlines() if l.strip()]
    pub=False
    for i,l in enumerate(lines):
        s=l.strip()
        if s=="public:": pub=True; continue
        if s in ("private:","protected:"): pub=False; continue
        if not pub: continue
        m=re.match(r"std::function<[^;]+>\s+(\w+)\s*;", s)
        if m:
            print("  SIGNAL "+s); continue
        m=re.match(r"((?:void|bool|int|unsigned|float|double|std::string|SStyle|const char\*|char\*|long)\s+[\w:]+)\s*\(", s)
        if m and not s.startswith("//"):
            sig=s; j=i
            while ")" not in sig and j+1<len(lines):
                j+=1; sig+=" "+lines[j].strip()
            idx=sig.find(")")
            if idx>=0: sig=sig[:idx+1]
            print("  METH "+sig)
    di=h.find("void draw()")
    if di>=0:
        seg=h[di:di+2500].splitlines()
        seg=[l.strip() for l in seg if l.strip() and not l.strip().startswith("//")][:18]
        print("  DRAW: "+" | ".join(seg)[:1200])
    try:
        d=open(dpath).read()
        dlines=[l.strip() for l in d.splitlines() if l.strip()]
        print("  DEMO-WIDGET-LINES:")
        for l in dlines:
            if "widget." in l and "on_button" not in l and "on_motion" not in l and "on_key" not in l and ".draw()" not in l:
                print("   "+l)
        print("  DEMO-INCLUDES:", [l for l in dlines if l.startswith("#include")])
    except Exception as e:
        print("  NO DEMO",e)
