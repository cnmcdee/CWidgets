# CWidgets IDE — Visual Designer + 194 Hand-Drawn X11 Widgets + GDB Debugger

One repository, one IDE:

- **Design** a form in **FDesigner** (`designer/`) — a C++/GTK 3
  visual designer with a separate code window, a tight widget
  palette, and a built-in GDB stepping debugger.
- **Place widgets** from **XFrame** (`widgets/`) — a pure X11/Xlib
  hand-drawn C++17 widget library: **194 widgets**, drawn with raw
  Xlib calls. No GTK, no Qt, no toolkit at all — despite the `Gtk*`
  names.
- **Generate** compilable C++/GTK code, **build** it with CMake, and
  **debug** it: breakpoints in the gutter, a red paused line, and
  automatic variable inspection.

Read the full guide: **[docs/ide.md](docs/ide.md)** — how the IDE
works, the end-to-end workflow (design → place → generate → build →
debug), and troubleshooting. The designer's own design notes are in
[designer/DESIGN.md](designer/DESIGN.md); the widget API reference
is in [docs/index.md](docs/index.md).

> **Status note:** the combined top-level build (`make`) and the
> single `./ide` launcher are landed; the per-half builds below still
> work on their own. Combined tree: FDesigner round-3
> (`cfree` HEAD `310ff34`, round 3 + palette icons) + CWidgets HEAD `ab3a2a3`, integrated
> in this repo. See [docs/ide.md](docs/ide.md) for the full guide.

## Quickstart

Prerequisites: a C++17 compiler (`g++`), CMake >= 3.20, X11 dev
headers (`libx11-dev`), GTK 3 dev files (`libgtk-3-dev`), and `gdb`.
Everything needs an X display — a real X server or Xvfb.

```sh
# Build both halves, then start the IDE (designer + palette + layout,
# each as its own top-level window)
make
./ide
# ./ide builds anything missing on its own; `make` alone just builds.
# `./ide --build` forces a rebuild; `./ide --no-xframe` is designer-only.
```

Per-half builds (verified today):

```sh
# Designer: plain Makefile — builds designer/fdesigner, zero warnings
cd designer && make

# XFrame library: CMake — all 194 demos + palette + layout, 0 warnings
cd widgets && cmake -S . -B build && cmake --build build -j"$(nproc)"
```

Launch each half:

```sh
./designer/fdesigner                              # designer + palette + code windows
./widgets/build/palette/cwidgets_palette          # XFrame palette (194 widgets)
./widgets/build/layout/cwidgets_layout            # XFrame layout window
```

Under Xvfb:

```sh
Xvfb :99 -screen 0 1920x1080x24 &
DISPLAY=:99 ./designer/fdesigner
```

## What you get

| Piece | Where | What it does |
|---|---|---|
| Designer | `designer/` | GTK 3 canvas designer — place widgets, edit properties, Events pane, save `.fdj` JSON projects |
| Widget palette | `designer/` (View → Widget Palette) | Separate top-level window, 250x780, **230 entries**: 36 tight FDesigner buttons (Tk set + 20 Lazarus-style widgets) + all 194 XFrame widgets in their 9 sections, each with its 16x16 icon; scrolls |
| Code window | `designer/` (View → Code View) | Separate top-level window, 1000x700 — generated C++ with line numbers, red-dot breakpoints, folding, themes, automatic variable inspection |
| Debugger | `designer/` (Debug → Open Debugger…) | Real GDB/MI stepping debugger: Run, Continue (F5), Step Over (F10), Step Into (F11), Step Out, Stop, Restart; plain / conditional / logpoint breakpoints with ignore counts and groups; locals, watches, stack |
| XFrame library | `widgets/` | 194 hand-drawn X11 widgets — see below |
| XFrame palette | `widgets/palette/` | `cwidgets_palette` — all 194 widgets in 9 collapsible sections with 16x16 icons |
| XFrame layout | `widgets/layout/` | `cwidgets_layout` — live design surface for real XFrame widget instances |

The workflow, in one line: **design a form → place widgets from a
palette → generate C++/GTK code → build with CMake → debug it**
(breakpoint pause, red line, automatic variables). Full walkthrough
in [docs/ide.md](docs/ide.md).

## XFrame — the widget library (194)

**XFrame** — a pure X11/Xlib hand-drawn C++17 widget library.

Every widget is drawn by hand with raw Xlib calls (XFillRectangle, XFillArc,
XDrawLine, XDrawString, ...). There is no GTK here, no Qt, no toolkit at all —
despite the `Gtk*` names. The `Gtk*` components recreate the look of GTK
widgets (GtkButton, GtkCalendar, GtkTreeView, ...) as standalone X11 classes;
the `T*` components are the same idea in Lazarus/Delphi style (TButton,
TMemo, TStringGrid, ...); plus `XGraph` and `XSpreadsheet`.

The shared core is `widgets/core/xframe_core.h` — display/screen/window boilerplate,
the GC-as-pen drawing primitives, a Canvas class, an SStyle style manager,
and the polished-widget construction from the "Writing Your Own X-11
Framework Basics" articles on hotconfig.com (Parts 1–4).

### Components (194)

Each component lives in `widgets/components/<Name>/` and ships exactly four files:

- `<Name>.h` — the widget class
- `main_<Name>.cpp` — a standalone demo that opens an 800x600 window
- `CMakeLists.txt` — builds the demo (C++17, links X11)
- `core/xframe_core.h` — the shared core, copied in per component

```
1. GtkAboutDialog
2. GtkAccelLabel
3. GtkActionBar
4. GtkAppChooserButton
5. GtkAppChooserDialog
6. GtkAppChooserWidget
7. GtkApplicationWindow
8. GtkAspectFrame
9. GtkAssistant
10. GtkBox
11. GtkButton
12. GtkButtonBox
13. GtkCalendar
14. GtkCellView
15. GtkCenterBox
16. GtkCheckButton
17. GtkCheckMenuItem
18. GtkColorButton
19. GtkColorChooserDialog
20. GtkColorChooserWidget
21. GtkColorDialogButton
22. GtkColumnView
23. GtkComboBox
24. GtkComboBoxText
25. GtkDialog
26. GtkDrawingArea
27. GtkDropDown
28. GtkEditableLabel
29. GtkEntry
30. GtkEntryCompletion
31. GtkEventBox
32. GtkExpander
33. GtkFileChooserButton
34. GtkFileChooserDialog
35. GtkFileChooserWidget
36. GtkFixed
37. GtkFlowBox
38. GtkFontButton
39. GtkFontChooserDialog
40. GtkFontChooserWidget
41. GtkFrame
42. GtkGLArea
43. GtkGrid
44. GtkGridView
45. GtkHeaderBar
46. GtkIconView
47. GtkImage
48. GtkInfoBar
49. GtkInscription
50. GtkInvisible
51. GtkLabel
52. GtkLayout
53. GtkLevelBar
54. GtkLinkButton
55. GtkListBox
56. GtkListView
57. GtkLockButton
58. GtkMediaControls
59. GtkMenu
60. GtkMenuBar
61. GtkMenuButton
62. GtkMenuItem
63. GtkMessageDialog
64. GtkModelButton
65. GtkNotebook
66. GtkOverlay
67. GtkPaned
68. GtkPasswordEntry
69. GtkPicture
70. GtkPlacesSidebar
71. GtkPopover
72. GtkPrintUnixDialog
73. GtkProgressBar
74. GtkRadioButton
75. GtkRadioMenuItem
76. GtkRevealer
77. GtkScale
78. GtkScaleButton
79. GtkScrollbar
80. GtkScrolledWindow
81. GtkSearchBar
82. GtkSearchEntry
83. GtkSeparator
84. GtkSeparatorMenuItem
85. GtkShortcutsWindow
86. GtkSpinButton
87. GtkSpinner
88. GtkStack
89. GtkStackSidebar
90. GtkStackSwitcher
91. GtkStatusbar
92. GtkSwitch
93. GtkTextView
94. GtkToggleButton
95. GtkToggleToolButton
96. GtkToolbar
97. GtkToolButton
98. GtkTreeView
99. GtkVideo
100. GtkViewport
101. GtkVolumeButton
102. GtkWindow
103. TActionList
104. TApplicationProperties
105. TArrow
106. TBevel
107. TBitBtn
108. TButton
109. TButtonPanel
110. TCalcEdit
111. TCalculatorDialog
112. TCalendar
113. TCalendarDialog
114. TCheckBox
115. TCheckComboBox
116. TCheckGroup
117. TCheckListBox
118. TColorBox
119. TColorButton
120. TColorDialog
121. TColorListBox
122. TComboBox
123. TComboBoxEx
124. TControlBar
125. TCoolBar
126. TDateEdit
127. TDateTimePicker
128. TDirectoryEdit
129. TDrawGrid
130. TEdit
131. TEditButton
132. TFileListBox
133. TFileNameEdit
134. TFilterComboBox
135. TFindDialog
136. TFloatSpinEdit
137. TFlowPanel
138. TFontDialog
139. TForm
140. TFrame
141. TGroupBox
142. THeaderControl
143. TImage
144. TImageList
145. TLabel
146. TLabeledEdit
147. TListBox
148. TListView
149. TMainMenu
150. TMaskEdit
151. TMemo
152. TNotebook
153. TOpenDialog
154. TOpenPictureDialog
155. TPageControl
156. TPageSetupDialog
157. TPaintBox
158. TPairSplitter
159. TPanel
160. TPopupMenu
161. TPopupNotifier
162. TPrintDialog
163. TPrinterSetupDialog
164. TProgressBar
165. TRadioButton
166. TRadioGroup
167. TReplaceDialog
168. TSaveDialog
169. TSavePictureDialog
170. TScrollBar
171. TScrollBox
172. TSelectDirectoryDialog
173. TShape
174. TShellListView
175. TShellTreeView
176. TSpeedButton
177. TSpinEdit
178. TSplitter
179. TStaticText
180. TStatusBar
181. TStringGrid
182. TTabControl
183. TTaskDialog
184. TTimeEdit
185. TTimer
186. TToggleBox
187. TToolBar
188. TTrackBar
189. TTrayIcon
190. TTreeView
191. TUpDown
192. TValueListEditor
193. XGraph
194. XSpreadsheet
```

### The shared core

All 194 per-component copies of `core/xframe_core.h` are byte-identical
(md5 `6e3874f2c33bbdf16caa7695d8cc4899`, 18,448 bytes), so one canonical copy
is hoisted to top-level `widgets/core/xframe_core.h`.

**Standalone builds are NOT broken by this:** the per-component copy is kept
in every `widgets/components/<Name>/core/` exactly as shipped, and each widget
header includes `"core/xframe_core.h"` relative to its own directory, so a
component dir copied out on its own still builds with no changes. The
top-level `widgets/core/` copy is the canonical reference for the whole repo; the
top-level build uses each component's own copy (identical bytes).

### Build (widgets half)

Prerequisites: CMake >= 3.20, a C++17 compiler, X11 dev headers
(`libx11-dev` on Debian/Ubuntu).

Build all 194 demos:

```sh
cd widgets
cmake -S . -B build
cmake --build build -j"$(nproc)"
```

Binaries land in `widgets/build/components/<Name>/<Name>`.

Build a single component standalone:

```sh
cmake -S widgets/components/GtkButton -B /tmp/build-button
cmake --build /tmp/build-button
```

Or build just one target from the top-level tree:

```sh
cmake --build widgets/build --target GtkCalendar
```

### Run a demo

Demos need an X display (real X server or Xvfb):

```sh
./widgets/build/components/GtkButton/GtkButton
```

Each demo opens an 800x600 window, draws its widget, and handles expose,
key-press, mouse button press/release, pointer motion, and window-close
(WM_DELETE_WINDOW). Close the window (or press a key, depending on the demo)
to exit.

Under Xvfb:

```sh
Xvfb :99 -screen 0 1920x1080x24 &
DISPLAY=:99 ./widgets/build/components/GtkButton/GtkButton
```

### Test status (widgets)

Tested Nov 26, 2026 on the VPS (Debian 12, g++ 12.2, Xvfb `:99` 1280x1024) with the
autonomous driver `widgets/test-driver/run_tests.py` — one demo at a time: open window, screenshot
to `widgets/screenshots/<Name>.png`, mouse sweep (hover), mousedown/mouseup at the widget centre
(click), Tab + letter (key), WM_DELETE_WINDOW (clean exit). Full table:
[`widgets/test-results/results.md`](widgets/test-results/results.md) (raw CSV:
[`widgets/test-results/results.csv`](widgets/test-results/results.csv), build log:
[`widgets/test-results/build.log`](widgets/test-results/build.log)).

- [x] All 194 demos build — clean from-scratch build, **0 errors, 0 warnings** with `-Wall -Wextra`
- [x] 193/194 demos draw (window opens, widget renders) — the one exception is GtkInvisible, which is non-visual by design
- [x] 125/194 demos show a visible click response (the other 69 are display-only widgets or containers that ignore a centre click by design; none crashed or hung)
- [ ] 12/194 demos show a visible hover response — the other 182 implement `on_motion` as a no-op by design (only 5 headers even declare a `hovered_` member)
- [x] All 194 demos survive Tab + letter keypresses
- [x] All 194 demos exit cleanly (rc 0 on WM_DELETE_WINDOW)
- [x] Palette self-test 194/194 selections OK; layout placement 194/194 and draw 194/194 (all 194 alive at once) — [`widgets/test-results/palette_layout_results.csv`](widgets/test-results/palette_layout_results.csv)
- [x] Composite contact-sheet PNG of all 194 demos: [`widgets/screenshots/all_widgets.png`](widgets/screenshots/all_widgets.png) — 3428x2952, 14x14 grid, 194 labelled tiles (plus one PNG per demo in `widgets/screenshots/`)

Bugs fixed during testing (see `widgets/test-results/results.md` for detail): clean-exit heap
corruption in `SStyle_Manager::~SStyle_Manager` (`XFreeFont` after `XCloseDisplay` — crashed
all 194 demos on exit); GtkOverlay link failure (`static const int` odr-use);
TPopupMenu demo never called `popup()` (blank window); 7 `-Wall -Wextra` warnings fixed.

Reproduce:

```sh
cd widgets
cmake -S . -B build && cmake --build build -j4   # 0 warnings with -Wall -Wextra
Xvfb :99 -screen 0 1280x1024x24 &
DISPLAY=:99 python3 test-driver/run_tests.py     # writes test-results/results.csv
```

### Documentation (widgets)

Full API documentation for all 194 widgets lives in [`docs/`](docs/)
— one page per section, plus the shared contract:

- [`docs/index.md`](docs/index.md) — the widget contract every widget shares (identical constructor, geometry, visibility/enabled, style, drawing, the four event handlers, signals), the minimal program skeleton, and tool summaries.
- [`docs/windows-dialogs.md`](docs/windows-dialogs.md) — Windows & Dialogs (28)
- [`docs/containers-layout.md`](docs/containers-layout.md) — Containers & Layout (34)
- [`docs/buttons.md`](docs/buttons.md) — Buttons (24)
- [`docs/display.md`](docs/display.md) — Display (33)
- [`docs/text-entry.md`](docs/text-entry.md) — Text & Entry (28)
- [`docs/choosers.md`](docs/choosers.md) — Choosers (10)
- [`docs/views-lists.md`](docs/views-lists.md) — Views & Lists (17)
- [`docs/menus-toolbars.md`](docs/menus-toolbars.md) — Menus & Toolbars (16)
- [`docs/misc-x.md`](docs/misc-x.md) — Misc / X (4)

Every widget gets its own subsection: what it is and what it draws, its
constructor, its widget-specific methods/properties/signals (with exact
signatures from the header), a minimal working example, and notes on
quirks. Every example in the docs compiles — all 194 were built with
`g++ -std=c++17 -I widgets/components/<Name> -lX11` and are byte-identical to the
code blocks in the pages. Where the header leaves something unclear
(unnamed int constants, a signal with no call site, a return value the
header never documents), the docs say so instead of inventing an answer.

### Selector palette

`widgets/palette/` builds `cwidgets_palette` (included in the top-level build;
binary at `widgets/build/palette/cwidgets_palette`). It is itself a pure X11/XFrame
window listing all 194 widgets in their 9 sections, each with its 16x16
icon and name. Section headers are collapsible; scroll with the wheel or
keyboard; Up/Down moves the selection; click (or Enter) selects a widget
and writes the choice to the shared selection file
(`/tmp/cwidgets-selection.txt`) for the layout tool. Self-test: 194/194
selections OK. Screenshot: [`widgets/screenshots/palette.png`](widgets/screenshots/palette.png).

```sh
cmake --build widgets/build --target cwidgets_palette
./widgets/build/palette/cwidgets_palette
```

### Layout window

`widgets/layout/` builds `cwidgets_layout` (binary at
`widgets/build/layout/cwidgets_layout`). It instantiates the **real** XFrame widget
classes via the type-erased factory in `widgets/common/registry.h` (generated by
`widgets/tools/gen_registry.py` — do not edit by hand): click to place the widget
selected in the palette, drag to move it, Delete to remove it, `s` to
save/load the layout from a file, `q` to quit. Double-click a placed
widget and 8 orange resize handles appear (corners + edges); drag a handle
to resize live; Escape dismisses the handles, Delete removes the widget.
Placement 194/194 PASS, draw 194/194 PASS, all 194 widgets alive at once.
Screenshots: [`widgets/screenshots/layout.png`](widgets/screenshots/layout.png),
[`widgets/screenshots/layout_resize_handles.png`](widgets/screenshots/layout_resize_handles.png),
[`widgets/screenshots/layout_all.png`](widgets/screenshots/layout_all.png).

```sh
cmake --build widgets/build --target cwidgets_layout
./widgets/build/layout/cwidgets_layout
```

### Icons

`widgets/icons/<Name>.ico` — one real 16x16 ICO per widget, purpose-drawn glyphs
generated by `widgets/tools/gen_icons.py` (70 glyph kinds, section accent colours
— not downscaled screenshots). Verified by `widgets/tools/verify_icons.py`:
194/194 parse as 16x16, all non-blank.

### Screenshots (widgets)

- [`widgets/screenshots/all_widgets.png`](widgets/screenshots/all_widgets.png) — composite contact sheet, all 194 demos, 3428x2952
- [`widgets/screenshots/palette.png`](widgets/screenshots/palette.png) — selector palette
- [`widgets/screenshots/layout.png`](widgets/screenshots/layout.png) — layout window
- [`widgets/screenshots/layout_resize_handles.png`](widgets/screenshots/layout_resize_handles.png) — resize handles on GtkButton
- [`widgets/screenshots/layout_all.png`](widgets/screenshots/layout_all.png) — all 194 widgets placed at once
- `widgets/screenshots/<Name>.png` — one PNG per demo (194)

## The seam: XFrame layout → designer project

The two halves are genuinely connected at one seam —
`widgets/tools/layout_to_fdesigner.py`. Place widgets in the XFrame
layout window, save the layout (`s` writes `/tmp/cwidgets-layout.txt`),
and convert it:

```sh
python3 widgets/tools/layout_to_fdesigner.py /tmp/cwidgets-layout.txt /tmp/layout.fdj
```

Then open `/tmp/layout.fdj` in the designer (**File → Open project**)
and use the normal flow: **File → Generate C++/GTK…**, **Build with
CMake**, debug with the stepping debugger. **136 of the 194** XFrame
widgets map to a real GTK counterpart (35 are nearest-match
approximations, e.g. `GtkCalendar` → `DateTimePicker`); **58** —
dialogs, choosers, menus and other XFrame-only widgets — have no
designer equivalent and are reported, never forced into a wrong type.
The generated app re-creates the widgets as GTK widgets; it does not
link XFrame classes, and layout edits do not flow back (re-run the
converter). Proved end to end by
`widgets/tests/test_layout_to_fdesigner.py` (`make test`): convert →
real `.fdj` load → real code generation (277 files for all 194) →
CMake build of the generated project. Full honest accounting in
[docs/ide.md](docs/ide.md).

## FDesigner — the designer half

FDesigner is a C++/GTK 3 visual designer (`designer/`): place widgets
on a canvas at absolute positions, edit their properties, generate
runnable C++/GTK code. The designer itself is GTK, defined in a Glade
`.ui` file, and the generated code is GTK too — no Qt anywhere.

- **Canvas + palette:** the design window (1150x720) plus a separate
  combined palette window (`FDesigner — Widgets`, 250x780, **230 entries**: the 36 FDesigner buttons in their grouped sections, then all 194 XFrame widgets in their 9 section groups, each with its 16x16 icon; the window scrolls). Click an FDesigner entry to add it to the canvas; drag to move, Delete to remove. Click an XFrame entry to hand it to the layout window, which places the real XFrame widget (see [the seam](#the-seam-xframe-layout--designer-project)).
  Widgets: the original Tk set (Button, Label, Entry, CheckButton,
  RadioButton, ComboBox, ListBox, Frame, Scale, Text, StockChart,
  Spreadsheet, Toplevel, Canvas, Scrollbar, MenuButton) plus 20
  Lazarus-style widgets (Memo, GroupBox, Panel, Image, Shape,
  Notebook, ScrollBox, Splitter, StatusBar, ToolBar, MenuBar,
  SpinEdit, TrackBar, ProgressBar, DateTimePicker, ColorBox, ListView,
  TreeView, StringGrid, Plot). `StockChart` plots candle charts from
  CSV with 250 technical indicators, drawings, and comparisons.
- **Code generation:** **File → Generate C++/GTK…** emits an all-code
  project — no `.ui` file — with every widget a standalone class in
  its own `.h`/`.cpp` pair, plus a `CMakeLists.txt` and a `Makefile`
  (executable `app`). Build a generated project with CMake
  (`cmake -S . -B build && cmake --build build`) or from the designer
  (**File → Build with CMake…** / **Open CMake Project…** / **CMake
  Settings…**).
- **Code window:** **View → Code View** — separate 1000x700 window
  with line numbers, red-dot breakpoints (click the gutter), folding
  (`#region`, `{}` blocks, comments, `#if` blocks), and four colour
  themes (Light, Dark, Monokai, Solarized Dark — **View → Color
  Theme…**). **Edit → Preferences…** sets the interface and code
  fonts.
- **Debugger:** **Debug → Open Debugger…** — a real GDB/MI stepping
  debugger. Breakpoints (plain / conditional / logpoint, ignore
  counts, groups) persist across runs. When it pauses, the code
  window's paused line goes red (`#c00000` + white text + gutter
  arrow, Lazarus-style) and the automatic variable inspection pane
  shows the in-scope locals — refreshed on every stop/step from the
  one shared GDB session (200 ms poll), never stale.
- **Events:** the Events pane connects any natural GTK signal per
  widget; the generator emits a stub handler + `g_signal_connect`.

Build it: `cd designer && make` → `./fdesigner`. Self-test:
`FDESIGNER_SELFTEST=1 ./fdesigner` (29 items, all pass). Full design
notes: [designer/DESIGN.md](designer/DESIGN.md). Full IDE guide:
[docs/ide.md](docs/ide.md).

Test status (combined IDE, re-verified in the integration pass):
`make clean && make`, zero warnings; self-test 29/29; debugger
console 37/37, GTK 7/7; widget tests 5/5; font tests 20/20; theme
tests 59/59; event handlers 6/6 + runtime 41/41; CMake probe 31/31,
script 4/4; code-window suite all pass; palette-window tests 16/16;
palette icons 13/13; matrix 21/21; combined palette self-test 230/230
+ real-mouse place of GtkAboutDialog; palette/layout 194/194 place,
194/194 draw; seam test all pass (136 mapped, 58 unmapped,
generated app window appears under X).

### Screenshots (designer)

`designer/screenshots/` — 16 shots at 2560x1440 under Xvfb:

- `01_designer.png` — designer + palette, `02_props_edit.png` —
  property editing, `03_codeview.png` — code view (Monokai, red dot,
  fold), `04_debugger.png` — debugger window,
  `05_dbg_settings.png` — debugger settings, `06_prefs.png` —
  font preferences, `07_cmake.png` — CMake build,
  `08_lazarus.png` — Lazarus widgets, `09_app.png` — generated app,
  `10_showcase.png` — showcase, `11_three_windows.png` — palette +
  code + design windows together, `12_code_paused.png` — code window
  paused on a breakpoint (red line), `13_palette.png` — tight
  palette, `14_three_windows_paused.png` — all three windows +
  debugger paused, `15_palette_icons.png` — combined palette with
  icons, `16_three_windows_icons.png` — all three windows with icons.

### Screenshots (combined IDE)

`screenshots/` — 2560x1440 under Xvfb:

- `ide_all.png` — all five IDE windows together, `ide_designer.png`
  — designer, `ide_palette.png` — combined palette (top),
  `ide_code.png` — code window, `ide_xframe_palette.png` — XFrame
  palette, `ide_xframe_layout.png` — XFrame layout window,
  `combined_palette_top.png` — combined palette, FDesigner section,
  `combined_palette_xframe.png` — combined palette, XFrame sections,
  `combined_palette_layout_placed.png` — GtkAboutDialog placed in the
  layout window from the combined palette.

## Repo layout

```
.
├── ide                    # single launcher — starts the IDE
├── Makefile               # top-level build: designer + widgets
├── README.md              # this file
├── LICENSE                # MIT, 2026 cnmcdee
├── designer/              # FDesigner (designer, code window, debugger, CMake)
│   └── src/xframe_palette.h   # combined-palette data (generated, 194 XFrame entries)
├── widgets/               # XFrame library (194 widgets, palette, layout, icons, tools)
│   ├── tools/layout_to_fdesigner.py   # seam: layout file -> .fdj project
│   ├── tools/gen_xframe_palette_data.py  # generates designer/src/xframe_palette.h
│   └── tests/test_combined_palette.py  # 230-entry palette end-to-end test
├── docs/                  # ide.md + index.md + 9 widget-section pages
└── screenshots/           # combined-IDE screenshots
```

## License

MIT — see [LICENSE](LICENSE). Copyright (c) 2026 cnmcdee.

<!-- SECTIONS:BEGIN -->

## Palette sections (194 widgets)

### Windows & Dialogs (28)

`GtkAboutDialog`, `GtkAppChooserDialog`, `GtkApplicationWindow`, `GtkAssistant`, `GtkColorChooserDialog`, `GtkDialog`, `GtkFileChooserDialog`, `GtkFontChooserDialog`, `GtkMessageDialog`, `GtkPrintUnixDialog`, `GtkShortcutsWindow`, `GtkWindow`, `TCalculatorDialog`, `TCalendarDialog`, `TColorDialog`, `TFindDialog`, `TFontDialog`, `TForm`, `TOpenDialog`, `TOpenPictureDialog`, `TPageSetupDialog`, `TPrintDialog`, `TPrinterSetupDialog`, `TReplaceDialog`, `TSaveDialog`, `TSavePictureDialog`, `TSelectDirectoryDialog`, `TTaskDialog`

### Containers & Layout (34)

`GtkAspectFrame`, `GtkBox`, `GtkButtonBox`, `GtkCenterBox`, `GtkExpander`, `GtkFixed`, `GtkFlowBox`, `GtkFrame`, `GtkGrid`, `GtkLayout`, `GtkNotebook`, `GtkOverlay`, `GtkPaned`, `GtkRevealer`, `GtkScrolledWindow`, `GtkScrollbar`, `GtkSeparator`, `GtkStack`, `GtkStackSidebar`, `GtkStackSwitcher`, `GtkViewport`, `TControlBar`, `TCoolBar`, `TFlowPanel`, `TFrame`, `TGroupBox`, `TNotebook`, `TPageControl`, `TPairSplitter`, `TPanel`, `TScrollBar`, `TScrollBox`, `TSplitter`, `TTabControl`

### Buttons (24)

`GtkButton`, `GtkCheckButton`, `GtkColorButton`, `GtkColorDialogButton`, `GtkFileChooserButton`, `GtkFontButton`, `GtkLinkButton`, `GtkLockButton`, `GtkModelButton`, `GtkRadioButton`, `GtkSwitch`, `GtkToggleButton`, `GtkToggleToolButton`, `GtkToolButton`, `GtkVolumeButton`, `TBitBtn`, `TButton`, `TButtonPanel`, `TCheckBox`, `TCheckComboBox`, `TRadioButton`, `TSpeedButton`, `TToggleBox`, `TUpDown`

### Display (33)

`GtkAccelLabel`, `GtkCalendar`, `GtkCellView`, `GtkDrawingArea`, `GtkGLArea`, `GtkImage`, `GtkInfoBar`, `GtkInscription`, `GtkInvisible`, `GtkLabel`, `GtkLevelBar`, `GtkMediaControls`, `GtkPicture`, `GtkProgressBar`, `GtkSpinner`, `GtkStatusbar`, `GtkVideo`, `TArrow`, `TBevel`, `TCalendar`, `TDateTimePicker`, `TImage`, `TImageList`, `TLabel`, `TPaintBox`, `TProgressBar`, `TShape`, `TStaticText`, `TStatusBar`, `TTimeEdit`, `TTimer`, `TTrayIcon`, `TTrackBar`

### Text & Entry (28)

`GtkComboBox`, `GtkComboBoxText`, `GtkDropDown`, `GtkEditableLabel`, `GtkEntry`, `GtkEntryCompletion`, `GtkPasswordEntry`, `GtkScale`, `GtkScaleButton`, `GtkSearchBar`, `GtkSearchEntry`, `GtkSpinButton`, `GtkTextView`, `TCalcEdit`, `TComboBox`, `TComboBoxEx`, `TDateEdit`, `TDirectoryEdit`, `TEdit`, `TEditButton`, `TFileNameEdit`, `TFilterComboBox`, `TFloatSpinEdit`, `TLabeledEdit`, `TMaskEdit`, `TMemo`, `TSpinEdit`, `TValueListEditor`

### Choosers (10)

`GtkAppChooserButton`, `GtkAppChooserWidget`, `GtkColorChooserWidget`, `GtkFileChooserWidget`, `GtkFontChooserWidget`, `GtkPlacesSidebar`, `TColorBox`, `TColorButton`, `TColorListBox`, `TFileListBox`

### Views & Lists (17)

`GtkColumnView`, `GtkGridView`, `GtkIconView`, `GtkListBox`, `GtkListView`, `GtkTreeView`, `TCheckGroup`, `TCheckListBox`, `TDrawGrid`, `THeaderControl`, `TListBox`, `TListView`, `TRadioGroup`, `TShellListView`, `TShellTreeView`, `TStringGrid`, `TTreeView`

### Menus & Toolbars (16)

`GtkActionBar`, `GtkCheckMenuItem`, `GtkHeaderBar`, `GtkMenu`, `GtkMenuBar`, `GtkMenuButton`, `GtkMenuItem`, `GtkPopover`, `GtkRadioMenuItem`, `GtkSeparatorMenuItem`, `GtkToolbar`, `TActionList`, `TMainMenu`, `TPopupMenu`, `TPopupNotifier`, `TToolBar`

### Misc / X (4)

`GtkEventBox`, `TApplicationProperties`, `XGraph`, `XSpreadsheet`

<!-- SECTIONS:END -->
