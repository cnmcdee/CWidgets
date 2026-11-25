#!/usr/bin/env python3
"""Convert a CWidgets layout file into an FDesigner project (.fdj).

The layout tool saves placed XFrame widgets as one line each:
    <XFrameName> <x> <y> <width> <height>
(e.g. `GtkButton 40 40 120 30`, written by `cwidgets_layout` on `s` or
by its SAVE script command). This converter maps each XFrame widget to
the closest FDesigner (GTK) widget type and writes a .fdj project that
the designer opens with File -> Open project. From there the normal
designer flow applies: File -> Generate C++/GTK, Build with CMake,
and debug with the stepping debugger (code window + auto variables).

What the seam is and is not:
  IS     - geometry + type mapping: layout -> .fdj -> generated GTK code.
  IS     - honest about approximation: many XFrame widgets have
           a real GTK counterpart; dialogs, choosers, menus and other
           XFrame-only widgets have none and are reported, never silently
           forced into a wrong type.
  IS NOT - a live bridge: the XFrame widgets are re-created as GTK widgets
           in the generated code; the generated app does not link or
           instantiate XFrame classes, and edits in the layout window do
           not flow back into the designer (re-run the converter).
  IS NOT - pixel-identical: XFrame draws its own look with raw Xlib;
           the GTK equivalents look like GTK widgets.

Usage:
    layout_to_fdesigner.py [--class-name Name] [--title Title]
                           <layout.txt> <out.fdj>
Exit codes: 0 = at least one widget mapped; 1 = usage/input error;
2 = layout parsed but nothing mapped.
"""

import argparse
import json
import sys

# Designer type -> XFrame names. Names listed in APPROX below are
# closest-match mappings (not the same widget, but the nearest GTK
# equivalent); the rest are direct counterparts.
GROUPS = [
    ("Button", ["GtkButton", "TButton", "TBitBtn", "TSpeedButton"]),
    ("Label", ["GtkLabel", "GtkAccelLabel", "GtkInscription", "TLabel",
               "TStaticText", "GtkEditableLabel"]),
    ("Entry", ["GtkEntry", "GtkPasswordEntry", "GtkSearchEntry", "TEdit",
               "TMaskEdit", "TLabeledEdit", "TEditButton", "TFileNameEdit",
               "TDirectoryEdit", "GtkEntryCompletion"]),
    ("Checkbutton", ["GtkCheckButton", "GtkCheckMenuItem", "GtkToggleButton",
                     "TCheckBox", "TToggleBox", "GtkSwitch",
                     "GtkToggleToolButton"]),
    ("Radiobutton", ["GtkRadioButton", "TRadioButton"]),
    ("Frame", ["GtkFrame", "GtkAspectFrame", "TFrame"]),
    ("GroupBox", ["TGroupBox", "GtkExpander", "TCheckGroup", "TRadioGroup"]),
    ("Panel", ["TPanel", "GtkBox", "GtkCenterBox", "GtkButtonBox", "GtkFlowBox",
               "GtkGrid", "TFlowPanel", "GtkFixed", "GtkLayout", "GtkOverlay",
               "GtkViewport", "GtkEventBox"]),
    ("Combobox", ["GtkComboBox", "GtkComboBoxText", "GtkDropDown", "TComboBox",
                  "TComboBoxEx", "TCheckComboBox", "TFilterComboBox"]),
    ("Listbox", ["GtkListBox", "TListBox", "TCheckListBox", "TFileListBox"]),
    ("ListView", ["GtkListView", "TListView", "GtkColumnView", "GtkIconView",
                  "GtkGridView", "TShellListView", "TColorListBox"]),
    ("TreeView", ["GtkTreeView", "TTreeView", "TShellTreeView"]),
    ("Menubutton", ["GtkMenuButton", "GtkModelButton", "GtkLinkButton"]),
    ("MenuBar", ["GtkMenuBar", "TMainMenu"]),
    ("ToolBar", ["GtkToolbar", "TToolBar", "TControlBar", "TCoolBar",
                 "GtkHeaderBar", "GtkActionBar"]),
    ("Canvas", ["GtkDrawingArea", "TPaintBox", "GtkGLArea"]),
    ("Scale", ["GtkScale", "GtkScaleButton"]),
    ("TrackBar", ["TTrackBar"]),
    ("SpinEdit", ["GtkSpinButton", "TSpinEdit", "TFloatSpinEdit", "TCalcEdit",
                  "TUpDown"]),
    ("Scrollbar", ["GtkScrollbar", "TScrollBar"]),
    ("Memo", ["GtkTextView", "TMemo"]),
    ("Notebook", ["GtkNotebook", "TNotebook", "TPageControl", "GtkStack",
                  "GtkStackSwitcher", "TTabControl"]),
    ("ScrollBox", ["GtkScrolledWindow", "TScrollBox"]),
    ("Splitter", ["GtkPaned", "TPairSplitter", "TSplitter"]),
    ("StatusBar", ["GtkStatusbar", "TStatusBar"]),
    ("ProgressBar", ["GtkProgressBar", "GtkLevelBar", "TProgressBar",
                     "GtkSpinner"]),
    ("DateTimePicker", ["GtkCalendar", "TCalendar", "TDateEdit",
                        "TDateTimePicker", "TTimeEdit"]),
    ("ColorBox", ["GtkColorButton", "TColorBox", "TColorButton"]),
    ("Image", ["GtkImage", "GtkPicture", "TImage", "TImageList"]),
    ("StringGrid", ["TStringGrid", "TDrawGrid", "TValueListEditor"]),
    ("Spreadsheet", ["XSpreadsheet"]),
    ("Plot", ["XGraph"]),
    ("Toplevel", ["GtkWindow", "GtkApplicationWindow", "TForm"]),
    ("Shape", ["TShape", "TBevel", "TArrow", "GtkSeparator"]),
]

# Names whose mapping is a nearest-match approximation, not the same widget.
APPROX = {
    "GtkEditableLabel", "GtkEntryCompletion", "GtkSwitch", "GtkToggleToolButton",
    "GtkExpander", "TCheckGroup", "TRadioGroup", "GtkOverlay", "GtkViewport",
    "GtkEventBox", "TColorListBox", "GtkLinkButton", "GtkHeaderBar", "GtkActionBar",
    "GtkGLArea", "GtkScaleButton", "TCalcEdit", "TUpDown", "GtkStack",
    "GtkStackSwitcher", "TTabControl", "GtkSpinner", "GtkCalendar", "TCalendar",
    "TDateEdit", "TTimeEdit", "TImageList", "TDrawGrid", "TValueListEditor",
    "GtkWindow", "GtkApplicationWindow", "TForm", "TBevel", "TArrow", "GtkSeparator",
}

MAPPING = {}   # XFrame name -> (designer type, approximate?)
for _dtype, _names in GROUPS:
    for _n in _names:
        assert _n not in MAPPING, f"duplicate mapping for {_n}"
        MAPPING[_n] = (_dtype, _n in APPROX)


def parse_layout(path):
    """Return (entries, errors): entries are (name, x, y, w, h)."""
    entries, errors = [], []
    with open(path) as f:
        for lineno, raw in enumerate(f, 1):
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            if len(parts) < 5:
                errors.append(f"line {lineno}: expected 5 fields, got {len(parts)}: {line!r}")
                continue
            name = parts[0]
            try:
                x, y, w, h = (int(p) for p in parts[1:5])
            except ValueError:
                errors.append(f"line {lineno}: non-integer geometry: {line!r}")
                continue
            entries.append((name, x, y, max(1, w), max(1, h)))
    return entries, errors


def convert(entries, class_name, title):
    """Build the .fdj dict; return (project, mapped, unmapped, approx_used)."""
    counters = {}
    widgets = []
    mapped, unmapped, approx_used = [], [], []
    max_right, max_bottom = 0, 0
    for name, x, y, w, h in entries:
        hit = MAPPING.get(name)
        if hit is None:
            unmapped.append(name)
            continue
        dtype, approx = hit
        counters[dtype] = counters.get(dtype, 0) + 1
        wname = f"{dtype}_{counters[dtype]:03d}"
        widgets.append({
            "name": wname,
            "type": dtype,
            "x": x, "y": y, "width": w, "height": h,
            # The label keeps the XFrame origin visible in the designer
            # and in the generated code. The generator only emits a
            # "label" property where the widget type declares one.
            "props": {"label": name},
            "signals": [],
        })
        mapped.append(name)
        if approx:
            approx_used.append((name, dtype))
        max_right = max(max_right, x + w)
        max_bottom = max(max_bottom, y + h)
    project = {
        "format": "fdesigner",
        "version": 1,
        "window": {
            "title": title,
            "class_name": class_name,
            "width": max(800, max_right + 20),
            "height": max(600, max_bottom + 20),
            "background": "",
            "menubar": "",
        },
        "widgets": widgets,
    }
    return project, mapped, unmapped, approx_used


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("layout", help="layout file saved by cwidgets_layout")
    ap.add_argument("out", help="output .fdj project file")
    ap.add_argument("--class-name", default="XFrameLayout",
                    help="generated window class name (default: XFrameLayout)")
    ap.add_argument("--title", default="XFrame Layout",
                    help="window title (default: XFrame Layout)")
    args = ap.parse_args(argv)

    try:
        entries, errors = parse_layout(args.layout)
    except OSError as e:
        print(f"layout_to_fdesigner: cannot read {args.layout}: {e}", file=sys.stderr)
        return 1
    for e in errors:
        print(f"layout_to_fdesigner: warning: {e}", file=sys.stderr)
    if not entries:
        print("layout_to_fdesigner: no widget entries found", file=sys.stderr)
        return 1

    project, mapped, unmapped, approx_used = convert(entries, args.class_name, args.title)
    with open(args.out, "w") as f:
        json.dump(project, f, indent=2)
        f.write("\n")

    print(f"layout_to_fdesigner: {len(entries)} entries, "
          f"{len(mapped)} mapped to {len(project['widgets'])} designer widgets, "
          f"{len(unmapped)} unmapped")
    if approx_used:
        print("approximate mappings (nearest GTK equivalent, not the same widget):")
        for name, dtype in sorted(set(approx_used)):
            print(f"  {name} -> {dtype}")
    if unmapped:
        print("unmapped (no FDesigner equivalent -- skipped, not forced):")
        for name in sorted(set(unmapped)):
            print(f"  {name}")
    print(f"wrote {args.out} "
          f"(window {project['window']['width']}x{project['window']['height']})")
    return 0 if mapped else 2


if __name__ == "__main__":
    sys.exit(main())
