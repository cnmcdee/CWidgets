# The Combined IDE — Designer + XFrame Widgets + GDB Debugger

> **Status: landed.** Written against the FDesigner round-3 tree
> (`cfree` HEAD `310ff34`, round 3 + palette icons) and the CWidgets tree (HEAD `ab3a2a3`).
> The combined top-level build (`make`) and the single `./ide`
> launcher are in the tree — see [Build](#build) and [Launch](#launch) —
> as is the layout → designer seam
> (`widgets/tools/layout_to_fdesigner.py`).

One repository, one IDE, two halves that fit together:

- **`designer/` — FDesigner**, a C++/GTK 3 visual designer. Place
  widgets on a canvas, edit properties, generate compilable C++/GTK
  code, and step through it with a built-in GDB debugger. The
  designer itself is GTK, built with GtkBuilder from a Glade `.ui`
  file. No Qt anywhere.
- **`widgets/` — XFrame**, a pure X11/Xlib hand-drawn C++17 widget
  library: **194 widgets** (102 `Gtk*`, 90 `T*`, plus `XGraph` and
  `XSpreadsheet`), drawn with raw Xlib calls. No GTK, no Qt, no
  toolkit at all — despite the `Gtk*` names. The `Gtk*` components
  recreate the look of GTK widgets as standalone X11 classes; the
  `T*` components are the same idea in Lazarus/Delphi style.

The two halves share one repo, one launcher, and one workflow:
design a form, place widgets from a palette, generate code, build it,
debug it. The full widget API reference lives in
[docs/index.md](index.md) (the shared contract) and the nine section
pages; the designer's own design notes live in
[designer/DESIGN.md](../designer/DESIGN.md).

## The windows

Starting the designer opens three top-level windows at once (the
code window opens together with the designer, parked beside the
palette instead of over the canvas). `./ide` adds the XFrame palette
and layout windows, for five in total. Each is a plain top-level window — never
transient or modal — so they coexist; closing one only hides it, and
its View-menu entry brings it back.

| Window | Title | Size | What it is |
|---|---|---|---|
| Designer | `FDesigner — C++/GTK — <name>` | 1150x720 | Canvas, property editor, Events pane, menus |
| Widget palette | `FDesigner — Widgets` | 250x780 | **Combined palette, 230 entries**: the 36 FDesigner buttons in grouped sections (the original Tk set plus 20 Lazarus-style widgets), 2px grid spacing, 20px buttons — Lazarus palette density — then all 194 XFrame widgets in their 9 section groups, each with its 16x16 icon; the window scrolls |
| Code | `FDesigner — Code` | 1000x700 | Generated C++ with gutter, folding, themes, breakpoints, automatic variable inspection |
| Debugger | `FDesigner Debugger` | — | GDB/MI stepping debugger (Debug → Open Debugger…) |
| XFrame palette | `CWidgets Palette - 194 widgets` | 380x880 | All 194 XFrame widgets in 9 collapsible sections, each with its 16x16 icon (`cwidgets_palette`) |
| XFrame layout | `CWidgets Layout` | 1024x720 | Live XFrame design surface: place, move, resize real widget instances (`cwidgets_layout`) |

The **combined palette** (`FDesigner — Widgets`) holds both sets:
the 36 FDesigner entries place widgets onto the GTK canvas (those
become generated C++/GTK code); the 194 XFrame entries select an
XFrame widget and hand the choice to the **layout window** via the
shared selection file (`/tmp/cwidgets-selection.txt`), exactly as the
standalone palette does. The layout window instantiates the *real*
XFrame widget class and lets you place, move, and resize it live.

There is also a **standalone XFrame palette**
(`cwidgets_palette`, `CWidgets Palette - 194 widgets`): the same 194
widgets in 9 collapsible sections, writing the same selection file.
Either palette can drive the layout window. The layout window is
the XFrame design surface; it saves a layout text file, it does not
emit FDesigner projects — the seam in section 2 below is how a saved
layout reaches the designer.

## Build

Prerequisites: a C++17 compiler (`g++`), CMake >= 3.20, X11 dev
headers (`libx11-dev` on Debian/Ubuntu), GTK 3 dev files
(`libgtk-3-dev`), and `gdb` for the debugger. Everything needs an X
display — a real X server or Xvfb (see
[Troubleshooting](#troubleshooting)).

**Combined build:** a top-level `Makefile` builds both halves:

```sh
make            # designer + widgets (194 demos + palette + layout)
make designer   # designer/fdesigner only
make widgets    # widgets/build only
make test       # seam test: layout -> .fdj -> codegen -> CMake build
make clean      # remove built artefacts
```

**Building each half today** (verified against the current trees):

```sh
# Designer: plain Makefile, g++ -O2 -Wall -Wextra -std=c++17, GTK 3 via pkg-config
cd designer
make            # builds ./fdesigner, zero warnings
make clean && make   # from scratch

# XFrame library: CMake, one target per component demo + palette + layout
cd widgets
cmake -S . -B build
cmake --build build -j"$(nproc)"   # all 194 demos, 0 errors, 0 warnings (-Wall -Wextra)
```

Binaries: `designer/fdesigner`, `widgets/build/palette/cwidgets_palette`,
`widgets/build/layout/cwidgets_layout`,
`widgets/build/components/<Name>/<Name>`.

A single component builds standalone — copy its directory out and
run `cmake -S . -B build` in it; each component ships its own copy of
`core/xframe_core.h`, byte-identical to the canonical
`widgets/core/xframe_core.h`.

## Launch

**Single launcher:** `./ide` at the repo root starts the IDE. It
builds anything missing first (`--build` forces a rebuild), then
starts the designer, the XFrame palette and the XFrame layout window
as separate top-level windows on the same X display; closing the
designer ends the session (the palette/layout windows go with it).
`./ide --no-xframe` starts the designer alone. All three programs
must share one display — run under Xvfb if there is no real X server.

**Launching each half today:**

```sh
./designer/fdesigner          # designer + palette + code windows together
./widgets/build/palette/cwidgets_palette   # XFrame palette (same X display)
./widgets/build/layout/cwidgets_layout     # XFrame layout window
```

All three XFrame programs and the designer must share one X display.
Under Xvfb:

```sh
Xvfb :99 -screen 0 1920x1080x24 &
DISPLAY=:99 ./designer/fdesigner
DISPLAY=:99 ./widgets/build/palette/cwidgets_palette &
DISPLAY=:99 ./widgets/build/layout/cwidgets_layout &
```

## End-to-end workflow

### 1. Design a form (designer)

Click a widget in the `FDesigner — Widgets` palette to add it to the
canvas; click to select, drag to move, Delete to remove it. The
property editor edits the selected widget's properties (per-type
tables in `designer/src/props.h`); bulk operations set or clear a
parameter on all, selected, or filtered widgets. The Events pane
lists the widget type's natural GTK signals — ticking one connects
it: the generator emits a stub handler plus `g_signal_connect` for
it, ready to fill in.

Widget set on the canvas: the original Tk set (Button, Label, Entry,
CheckButton, RadioButton, ComboBox, ListBox, Frame, Scale, Text,
StockChart, Spreadsheet, Toplevel, Canvas, Scrollbar, MenuButton)
plus 20 Lazarus-style widgets (Memo, GroupBox, Panel, Image, Shape,
Notebook, ScrollBox, Splitter, StatusBar, ToolBar, MenuBar,
SpinEdit, TrackBar, ProgressBar, DateTimePicker, ColorBox, ListView,
TreeView, StringGrid, Plot). `StockChart` is the chart widget: candle
charts from CSV (`data-file`, `up-color`, `down-color`,
`max-candles`), with 250 named technical indicators
(`indicators` property, e.g. `"SMA(20), RSI, MACD(12,26,9)"`),
drawings, comparison files, and a percent scale — see
[designer/DESIGN.md](../designer/DESIGN.md).

Save the design as a JSON project file (`.fdj`).

### 2. Place XFrame widgets (palette + layout)

Pick the widget in either palette. In the **combined palette**
(`FDesigner — Widgets`), scroll down past the FDesigner buttons to
the XFrame section groups and click an entry — its 16x16 icon and
name are shown, and the choice is written to
`/tmp/cwidgets-selection.txt` for the layout window. In the
standalone XFrame palette, click a section header to collapse/expand
it, scroll with the wheel or keyboard, and click (or Up/Down + Enter)
to select a widget — the title bar shows the selection, and the
choice is written to the same file. In the layout window,
click empty canvas to place the selected widget at its default size,
drag a placed widget to move it, Delete to remove it. Double-click a
placed widget for 8 orange resize handles (corners + edges); drag a
handle to resize live; Escape dismisses the handles, `s` saves the
layout to `/tmp/cwidgets-layout.txt`, `q` quits. The layout window
re-reads the selection file every 100 ms, so picking in the palette
updates the "selected to place" readout in its status bar.

**From layout to designer (the seam):** the layout is not a dead end.
Save it (`s`, or `SAVE` in a script) and convert it to an FDesigner
project:

```sh
python3 widgets/tools/layout_to_fdesigner.py /tmp/cwidgets-layout.txt /tmp/layout.fdj
```

Open `/tmp/layout.fdj` in the designer (**File → Open project**) and
continue with section 3 (generate) and section 5 (debug) below.
136 of the 194 XFrame widgets map to a real GTK counterpart (35 are
nearest-match approximations, listed by the converter); 58 —
dialogs, choosers, menus and other XFrame-only widgets — have no
designer equivalent and are reported, never forced into a wrong type.
The generated app re-creates the widgets as GTK widgets: it does not
link XFrame classes, and layout edits do not flow back into the
designer — re-run the converter. Proved end to end by
`widgets/tests/test_layout_to_fdesigner.py` (`make test`).

The layout tool also has a script mode for automated testing:
`cwidgets_layout --script cmds.txt --log out.log` with `PLACE`,
`PLACE_ALL`, `MOVE`, `RESIZE`, `DELETE`, `SELECT`, `DBLCLICK`,
`HANDLE`, `CHECKDRAW`, `SAVE` / `LOAD`, `LIST`, `COUNT`, `QUIT`.

### 3. Generate code (designer)

**File → Generate C++/GTK…** asks for a folder and writes a complete
project. The output is **all code — no `.ui` file** — so a stepping
debugger can walk widget construction line by line, and every widget
is a **standalone class in its own `.h`/`.cpp` pair**: creation plus
configuration in the constructor, the same nine mouse-event stub
handlers per widget (`buttonPress1-3`, `dblbuttonPress1-3`,
`buttonRelease1-3`) routed by two dispatchers, and a
`util_error_print` helper. The window class just instantiates one
widget class per designed widget. Regions in the generated code are
marked `// #region`, which the code view folds.

Every generated project also gets a `CMakeLists.txt` next to its
`Makefile` (`cmake_minimum_required(VERSION 3.10)`,
`project(<ClassName> CXX)`, C++17, `find_package(PkgConfig)` /
`pkg_check_modules(GTK3 REQUIRED gtk+-3.0)`, every generated `.cpp`
listed explicitly in `add_executable(app …)`). The executable is
named `app`, matching the Makefile target, and builds standalone
with:

```sh
g++ -o app *.cpp $(pkg-config --cflags --libs gtk+-3.0)
# or
cmake -S . -B build && cmake --build build
```

From the designer, **File → Generate C++/GTK…** offers **Build with
CMake** right after generating (the default build path for a
generated project); **File → Build with CMake…** rebuilds the last
generated project (or asks for a folder); **File → Open CMake
Project…** points the designer at any directory holding a
`CMakeLists.txt` — generated or hand-written — and builds it
(configure + build), reporting success/failure plus the executable
path in a log dialog. A missing or broken `CMakeLists.txt`, or a
compile error, is reported as a failure, never as success. CMake
settings (command, generator, build type, build directory) live in
`~/.config/fdesigner/cmake.conf` and are edited via **File → CMake
Settings…**.

### 4. Read the code (code window)

**View → Code View** shows the generated C++ for the current canvas
in the `FDesigner — Code` window. If nothing has been generated yet
it says so (`Generate a design first: File -> Generate C++/GTK...`).
The view is a `GtkTextView` with a gutter beside it in one
`GtkScrolledWindow`, so the gutter scrolls in lockstep:

- **Line numbers** in the left gutter.
- **Red-dot breakpoints:** click anywhere in the gutter (outside the
  fold-marker column) to toggle a breakpoint at that line. This goes
  through the real debugger backend and persists with the other
  breakpoints (see [Debug](#5-debug-with-the-stepping-debugger)).
- **Folding:** `[-]` / `[+]` toggles in the fold-marker column.
  Everything foldable folds: `#region` / `#endregion` (bare and the
  `// #region` form the generator emits), every matched-brace span
  (functions, if/else, loops, classes, structs, namespaces,
  initializers), multi-line `/* */` comments, and `#if` / `#ifdef` /
  `#ifndef` … `#endif` blocks — nested in all directions. Folded
  lines collapse to zero height, so file line numbers stay exact.
- **Colour themes:** **View → Color Theme…** picks one of four
  built-ins — Light, Dark, Monokai, Solarized Dark — colouring
  keywords, types, strings, comments, numbers, preprocessor lines,
  background/foreground and the current line; per-role colours are
  customisable in the picker. The choice persists to
  `~/.config/fdesigner/theme.conf`.
- **Fonts:** **Edit → Preferences…** sets the interface font and the
  code/text-view font (defaults `Sans 10` / `Monospace 11`, sizes
  clamped to 6–48 pt), persisted to
  `~/.config/fdesigner/settings.conf` and applied live.

### 5. Debug with the stepping debugger

**Debug → Open Debugger…** opens the `FDesigner Debugger` window. It
drives a real `gdb` over the MI protocol (`gdb -q -i=mi2`, backend in
`designer/src/debugger.h` / `debugger.cpp`) — no mock, no simulation.

- **Toolbar:** Run, Continue (**F5**), Step Over (**F10**), Step Into
  (**F11**), Step Out, Stop (interrupts a running target; press again
  to kill it), Restart, Settings.
- **Targets:** any program compiled with `-g` — console programs and
  GTK apps alike (a GTK app runs under the same X display as the
  designer).
- **Breakpoints:** type `file.cpp:123` or a function name and press
  Add BP. The list shows id / location / enabled toggle / hit count.
  The Add/Edit dialog sets the full handler set for a breakpoint:
  **kind** — Plain, Conditional (stops only when a gdb expression is
  true, e.g. `i == 5`), or Logpoint (a gdb `dprintf`: prints a
  message and never stops) — plus **ignore count** (skip the first N
  hits), **log message** template (literal text plus `{expr}`
  placeholders; `{s:expr}` prints a string, `{f:expr}` a float,
  `{x:expr}` hex, `{p:expr}` a pointer), and a **group** label.
  Breakpoints persist to `~/.config/fdesigner/debugger_bps.conf`
  (one per line:
  `location|enabled|kind|condition|ignore|log_message|group`) and are
  restored on the next run.
- **Variables:** Locals (name / type / value) and a watch list (add
  any expression, e.g. `sum + 1`) refresh on every stop; the Stack
  pane shows the current frames; the status line shows the stopped
  file:line and function. Target stdout/stderr appears in the bottom
  pane; logpoint output lands in the gdb log (it arrives on gdb's
  console stream, not the target's output).
- **Settings:** the Settings toolbar button, **Debug → Debugger
  Settings…**, and the **Debug → Debugger** sub-menu (which also has
  Break on main / Stop on entry / Pretty-print values toggles).
  Settings live in `~/.config/fdesigner/debugger.conf`: `gdb_path`
  (default `/usr/bin/gdb`), `break_on_main`, `stop_on_entry`,
  `pretty_print`, `print_all_locals`, `default_args`. They take
  effect on the next Run.

**What happens in the code window when the debugger pauses:** the
code window shares the debugger's `GdbSession` and polls it every
200 ms — one session, no second gdb. On a stop it loads the stopped
file (if it is not already shown), unfolds any region hiding the
paused line, and:

- the paused line's **background goes solid red (`#c00000`) with
  white text**, plus a white ▶ arrow in the gutter — the Lazarus
  execution-point style (full-line background + gutter arrow,
  distinct from the red-dot breakpoint marker). If the paused line
  is also a breakpoint line, both marks show (the arrow tip touches
  the red dot by ~2px).
- the **automatic variable inspection pane** at the bottom of the
  code window shows the scope (`func() — file:line (reason)`) and
  the locals of the current frame, refreshed on every stop/step.
  While the target is running it shows "running" and clears the
  paused line — it never presents stale values as current. With no
  target it says so; after exit it shows the exit code.

Step with F10/F11 (or the toolbar) and the red line and the
variables follow the execution point. Click a red dot in the gutter
while paused to add or remove a breakpoint at that line; it stages
into the shared session and persists.

## The XFrame widget library

The library reference is [docs/index.md](index.md) plus one page per
section. Every widget shares the same contract: the identical
constructor

```cpp
<Name>(Display* display, int screen, Window window, GC gc,
       SStyle_Manager* styles, int x, int y, int width, int height)
```

`set_bounds` / `width` / `height` / `contains`, visibility and
enabled state, `set_style_name` / `style`, `draw()`, the four event
handlers (`on_button_press`, `on_button_release`, `on_motion`,
`on_key`), and signals as public `std::function` members (most
commonly `on_click`). The exact constructor, widget-specific
methods, signals, and a minimal working example per widget are on
its section page — never guess a signal name: if it is not in the
header, it does not exist.

| Section | Widgets | Page |
|---|---|---|
| Windows & Dialogs | 28 | [windows-dialogs.md](windows-dialogs.md) |
| Containers & Layout | 34 | [containers-layout.md](containers-layout.md) |
| Buttons | 24 | [buttons.md](buttons.md) |
| Display | 33 | [display.md](display.md) |
| Text & Entry | 28 | [text-entry.md](text-entry.md) |
| Choosers | 10 | [choosers.md](choosers.md) |
| Views & Lists | 17 | [views-lists.md](views-lists.md) |
| Menus & Toolbars | 16 | [menus-toolbars.md](menus-toolbars.md) |
| Misc / X | 4 | [misc-x.md](misc-x.md) |
| **Total** | **194** | |

Section assignment is defined in `widgets/tools/sections.py` — every
widget belongs to exactly one section. Every example in the docs was
built with `g++ -std=c++17 -I widgets/components/<Name> -lX11` and
is byte-identical to the code in the pages. Where a header leaves
something unclear, the docs say so instead of inventing an answer.

Test status (VPS, Debian 12, g++ 12.2, Xvfb): all 194 demos build
with 0 warnings; 193 draw (`GtkInvisible` is non-visual by design);
125 show a visible click response (the rest are display-only by
design); all 194 survive keypresses and exit cleanly; zero crashes,
zero hangs. Palette self-test 194/194; layout placement 194/194 and
draw 194/194, all 194 alive at once. Full tables:
`widgets/test-results/results.md` and
`widgets/test-results/palette_layout_results.csv`.

Designer test status (round-3 tree): `make clean && make`, zero
warnings; self-test 29/29 (`FDESIGNER_SELFTEST=1 ./fdesigner`);
debugger console 37/37, GTK 7/7; widget tests 5/5; font tests 20/20;
theme tests 59/59; event handlers 6/6 + runtime 41/41; CMake probe
31/31; code-window suite all pass; palette-window tests 16/16.

## Repo layout

```
.                          # combined IDE repo (CWidgets)
├── ide                    # single launcher — starts the IDE
├── Makefile               # top-level build: designer + widgets
├── README.md              # this repo: IDE headline, library, designer
├── LICENSE                # MIT, 2026 cnmcdee
├── designer/              # FDesigner (cfree round-3 tree)
│   ├── DESIGN.md          # designer design notes (all phases)
│   ├── Makefile           # builds ./fdesigner
│   ├── src/               # designer, code view, debugger, CMake, generator
│   ├── ui/designer.ui     # main window + palette window (Glade)
│   ├── tests/             # self-test, matrix, debugger, theme, font, CMake
│   └── screenshots/       # 01–14: designer, code view, debugger, 3-window shots
├── widgets/               # XFrame library (194 widgets)
│   ├── components/<Name>/ # <Name>.h + main_<Name>.cpp + CMakeLists.txt
│   ├── core/xframe_core.h # canonical shared core
│   ├── palette/           # cwidgets_palette
│   ├── layout/            # cwidgets_layout
│   ├── common/            # registry.h, ipc.h, ico.h
│   ├── icons/             # 194 real 16x16 .ico icons
│   ├── tools/             # docgen, gen_icons.py, gen_registry.py, sections.py
│   │   └── layout_to_fdesigner.py  # seam: layout file -> .fdj project
│   ├── tests/             # test_palette_layout.py, test_layout_to_fdesigner.py
│   ├── test-results/      # results.md/csv, build.log, palette_layout_results.*
│   └── screenshots/       # all_widgets.png + one PNG per demo + palette/layout
└── docs/
    ├── index.md           # XFrame shared contract + tools
    ├── ide.md             # this file — the combined IDE guide
    ├── windows-dialogs.md # 28 widgets
    ├── containers-layout.md
    ├── buttons.md
    ├── display.md
    ├── text-entry.md
    ├── choosers.md
    ├── views-lists.md
    ├── menus-toolbars.md
    └── misc-x.md
└── screenshots/           # combined-IDE screenshots (ide_*.png)
```

## Troubleshooting

| Symptom | Cause / fix |
|---|---|
| `cannot open display` | No X server. Run under Xvfb (`Xvfb :99 -screen 0 1920x1080x24 &`, `DISPLAY=:99 …`) or a real X session. Give each project its own display number — never take over another project's display. |
| Designer opens but the palette/code window is missing | They open together at startup; closing one only hides it. Reopen with **View → Widget Palette** / **View → Code View**. Under bare Xvfb the palette has no title bar (expected); use the View menu, not the window decoration, to close it. |
| Code view is empty / shows "Generate a design first" | The code view shows the generated code for the current canvas. Design something, then **File → Generate C++/GTK…** (or reopen the view — it refreshes on open). |
| Breakpoint red dot does not appear | Dots come from the persisted breakpoint file (`~/.config/fdesigner/debugger_bps.conf`). Gutter clicks toggle through the real session; a dot on a line whose file is not the one shown belongs to that file. Check the file exists and the code view is showing the same file. |
| Paused line is not red / variables are stale | The red line and the inspection pane follow the shared `GdbSession` (200 ms poll). If the target is running, the pane deliberately shows "running" and clears the line — that is the design, not a bug. If it never updates, the debugger window's session is not the one running the target. |
| `gdb` fails to start / target never runs | Check **Debug → Debugger Settings…**: `gdb_path` (default `/usr/bin/gdb`), and that the target was compiled with `-g`. The debugger reports a bad gdb path or a missing executable as a start failure. |
| CMake build reports failure | Read the log dialog: a missing or broken `CMakeLists.txt` or a compile error is reported as failure, never as success. **File → CMake Settings…** edits command, generator, build type, and build directory (`~/.config/fdesigner/cmake.conf`). |
| Layout window does not pick up the palette selection | The palette writes `/tmp/cwidgets-selection.txt`; the layout window polls it every 100 ms. If the file is stale, click a widget in the palette again. Both programs must run on the same machine/display. |
| Palette exits with "FATAL: not all icons loaded" | It needs all 194 icons from `icons/`. Set `CWIDGETS_ICONS` to the icons directory, or run from the repo root so the `icons` / `../icons` candidates resolve. A missing icon is also drawn as a red box, never silently skipped. |
| A generated project does not compile | Generated projects are covered by the matrix (`designer/tests/run_matrix.sh`): every type, property, enum, menu, and hostile string compiles with `-Wall -Wextra`, zero warnings. Re-generate from the current canvas; if it still fails, the compiler output names the file — the per-widget classes are one `.h`/`.cpp` pair each, so the fault is local to one widget class. |
| `GtkInvisible` draws nothing | By design — it is a non-visual widget. The test suite expects it blank. |

## Sources

Every claim above was checked against one of these (round-3 designer
tree, CWidgets tree at `ab3a2a3`, combined IDE in this repo):

- `designer/DESIGN.md` — designer phases, debugger, CMake, Lazarus
  widgets, fonts, themes/code view.
- `designer/src/main.cpp` — window titles/sizes/parking, View/File/
  Debug/Edit menus, 200 ms code-window poll, startup behaviour.
- `designer/src/codeview.h` — gutter, folding, `#c00000` paused line,
  inspection states.
- `designer/src/debugger.h` / `debugger_ui.cpp` — GDB/MI backend,
  breakpoint kinds, F5/F10/F11, settings and persistence paths.
- `designer/src/preferences.h`, `theme.h`, `generator.h`, `Makefile`,
  `ui/designer.ui` — fonts, themes, all-code generation, build.
- `widgets/docs/index.md` — XFrame contract, palette/layout tools,
  test status.
- `widgets/palette/main_palette.cpp`, `widgets/layout/main_layout.cpp`,
  `widgets/common/ipc.h` — palette/layout behaviour, selection file,
  save paths, script commands.
- `widgets/README.md` (pre-combination) — build, test status,
  screenshots.

Verified in the integration pass (VPS, Nov 26, 2026): the top-level
build, the `./ide` launcher, the `designer/` + `widgets/` split paths
used above, the combined 230-entry palette
(`designer/src/xframe_palette.h`, generated by
`widgets/tools/gen_xframe_palette_data.py`, covered by
`widgets/tests/test_combined_palette.py`: presence 36+194 exact,
self-test 230/230, real-mouse place of GtkAboutDialog), and the
layout → designer seam (`widgets/tools/layout_to_fdesigner.py`,
covered by `widgets/tests/test_layout_to_fdesigner.py`).
Combined-IDE test results are in the repo README and in the
integration report; the per-half suites still apply unchanged (see
the test-status paragraphs above).
