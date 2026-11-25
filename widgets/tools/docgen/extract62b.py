import re
import os
REPO_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
DQ = chr(34)
names_win = "GtkAboutDialog GtkAppChooserDialog GtkApplicationWindow GtkAssistant GtkColorChooserDialog GtkDialog GtkFileChooserDialog GtkFontChooserDialog GtkMessageDialog GtkPrintUnixDialog GtkShortcutsWindow GtkWindow TCalculatorDialog TCalendarDialog TColorDialog TFindDialog TFontDialog TForm TOpenDialog TOpenPictureDialog TPageSetupDialog TPrintDialog TPrinterSetupDialog TReplaceDialog TSaveDialog TSavePictureDialog TSelectDirectoryDialog TTaskDialog".split()
names_con = "GtkAspectFrame GtkBox GtkButtonBox GtkCenterBox GtkExpander GtkFixed GtkFlowBox GtkFrame GtkGrid GtkLayout GtkNotebook GtkOverlay GtkPaned GtkRevealer GtkScrolledWindow GtkScrollbar GtkSeparator GtkStack GtkStackSidebar GtkStackSwitcher GtkViewport TControlBar TCoolBar TFlowPanel TFrame TGroupBox TNotebook TPageControl TPairSplitter TPanel TScrollBar TScrollBox TSplitter TTabControl".split()
COMMON = {"set_bounds","width","height","contains","set_visible","get_visible","set_enabled","get_enabled","set_style_name","style","draw","on_button_press","on_button_release","on_motion","on_key","set_text","get_text"}
STD = "(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)"
for name in names_win+names_con:
    h=open(REPO_ROOT + "/widgets/components/%s/%s.h" % (name,name)).read()
    hc=re.sub(r"\s+"," ",h)
    ok = (name+STD) in hc
    print("===== %s ctor_std=%s =====" % (name, ok))
    lines=[l.rstrip() for l in h.splitlines() if l.strip()]
    pub=False
    for i,l in enumerate(lines):
        s=l.strip()
        if s=="public:": pub=True; continue
        if s in ("private:","protected:"): pub=False; continue
        if not pub: continue
        m=re.match(r"std::function<([^;]+)>\s+(\w+)\s*;", s)
        if m:
            print("  SIG %s : std::function<%s>" % (m.group(2), m.group(1))); continue
        m=re.match(r"static const int (\w+) = (-?\d+);", s)
        if m:
            print("  CONST %s = %s" % (m.group(1), m.group(2))); continue
        m=re.match(r"((?:void|bool|int|unsigned|float|double|std::string|SStyle|const char\*|char\*|long|Srgb)\s+[\w:]+)\s*\(", s)
        if m and not s.startswith("//"):
            sig=s; j=i
            while ")" not in sig and j+1<len(lines):
                j+=1; sig+=" "+lines[j].strip()
            idx=sig.find(")")
            if idx>=0: sig=sig[:idx+1]
            mm=re.search(r"(\w+)\s*\(", sig)
            mname=mm.group(1) if mm else "?"
            if mname in COMMON: continue
            if mname==name: continue
            print("  API "+sig)
    di=h.find("void draw()")
    if di>=0:
        seg=h[di:di+4000]
        pat = DQ + "([^" + DQ + "]{2,40})" + DQ
        strs=re.findall(pat, seg)
        print("  DRAWSTRS: %s" % (strs[:14],))
        calls=sorted(set(re.findall(r"(xf::\w+|X\w+|draw_\w+)\(", seg)))
        print("  DRAWCALLS: %s" % (calls,))
