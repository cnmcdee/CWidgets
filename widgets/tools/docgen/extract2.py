import re, os
import os
REPO_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
names = """GtkButton GtkCheckButton GtkColorButton GtkColorDialogButton GtkFileChooserButton GtkFontButton GtkLinkButton GtkLockButton GtkModelButton GtkRadioButton GtkSwitch GtkToggleButton GtkToggleToolButton GtkToolButton GtkVolumeButton TBitBtn TButton TButtonPanel TCheckBox TCheckComboBox TRadioButton TSpeedButton TToggleBox TUpDown GtkAccelLabel GtkCalendar GtkCellView GtkDrawingArea GtkGLArea GtkImage GtkInfoBar GtkInscription GtkInvisible GtkLabel GtkLevelBar GtkMediaControls GtkPicture GtkProgressBar GtkSpinner GtkStatusbar GtkVideo TArrow TBevel TCalendar TDateTimePicker TImage TImageList TLabel TPaintBox TProgressBar TShape TStaticText TStatusBar TTimeEdit TTimer TTrayIcon TTrackBar GtkAppChooserButton GtkAppChooserWidget GtkColorChooserWidget GtkFileChooserWidget GtkFontChooserWidget GtkPlacesSidebar TColorBox TColorButton TColorListBox TFileListBox""".split()
for n in names:
    h = f"{REPO_ROOT}/widgets/components/{n}/{n}.h"
    txt = open(h).read()
    print(f"\n### {n}")
    # all std::function members anywhere
    for mm in re.finditer(r"std::function<([^;]+?)>\s+(\w+)\s*;", txt):
        print(f"  SIG: {mm.group(2)} : std::function<{mm.group(1)}>")
    # enums defined in header
    for mm in re.finditer(r"enum\s+(?:class\s+)?(\w+)\s*\{([^}]+)\}", txt):
        vals = re.sub(r"\s+", " ", mm.group(2)).strip()
        print(f"  ENUM {mm.group(1)}: {vals[:300]}")
    # constants: static const / const int etc at class scope
    for mm in re.finditer(r"(?:static\s+)?(?:const\s+)?(?:int|double|float|bool)\s+(\w+)\s*=\s*([^;]+);", txt):
        print(f"  CONST: {mm.group(1)} = {mm.group(2).strip()[:80]}")
    # private member defaults (first occurrence lines with = in private section, key fields only)
    priv = txt.split("public:")[0]
    for line in priv.splitlines():
        s = line.strip()
        if re.match(r"(std::string|int|double|float|bool|unsigned long|Srgb)\s+\w+\s*=", s):
            print(f"  PRIV: {s[:160]}")
    # constructor default text_ / initial values: find text_ = "..." etc in ctor
    for mm in re.finditer(r"(text_|value_|position_|min_|max_|increment_|active_|checked_|color_|font_name_|uri_|filename_|title_|orientation_|kind_|state_|interval_|shape_|direction_|arrow_|bevel_|progress_|fraction_|level_|angle_|date_|time_|format_|caption_|label_)\s*=\s*([^;]+);", txt):
        print(f"  INIT: {mm.group(1)} = {mm.group(2).strip()[:100]}")
    # check hover_ member existence and on_motion non-noop: does on_motion body contain hover_?
    di = txt.find("bool on_motion")
    if di >= 0:
        body = txt[di:di+600]
        print(f"  MOTION: {'HOVER-STATE' if 'hover_' in body else 'noop'} :: {re.sub(chr(10),' ',body)[:220]}")
    # get_min/get_max presence etc: list all get_ methods
    gets = sorted(set(re.findall(r"\b(get_\w+)\s*\(", txt)))
    print(f"  GETS: {gets}")
