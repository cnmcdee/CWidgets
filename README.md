# CWidgets

**XFrame** — a pure X11/Xlib hand-drawn C++17 widget library.

Every widget is drawn by hand with raw Xlib calls (XFillRectangle, XFillArc,
XDrawLine, XDrawString, ...). There is no GTK here, no Qt, no toolkit at all —
despite the `Gtk*` names. The `Gtk*` components recreate the look of GTK
widgets (GtkButton, GtkCalendar, GtkTreeView, ...) as standalone X11 classes;
the `T*` components are the same idea in Lazarus/Delphi style (TButton,
TMemo, TStringGrid, ...); plus `XGraph` and `XSpreadsheet`.

The shared core is `core/xframe_core.h` — display/screen/window boilerplate,
the GC-as-pen drawing primitives, a Canvas class, an SStyle style manager,
and the polished-widget construction from the "Writing Your Own X-11
Framework Basics" articles on hotconfig.com (Parts 1–4).

## Components (194)

Each component lives in `components/<Name>/` and ships exactly four files:

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

## The shared core

All 194 per-component copies of `core/xframe_core.h` are byte-identical
(md5 `6e3874f2c33bbdf16caa7695d8cc4899`, 18,448 bytes), so one canonical copy
is hoisted to top-level `core/xframe_core.h`.

**Standalone builds are NOT broken by this:** the per-component copy is kept
in every `components/<Name>/core/` exactly as shipped, and each widget
header includes `"core/xframe_core.h"` relative to its own directory, so a
component dir copied out on its own still builds with no changes. The
top-level `core/` copy is the canonical reference for the whole repo; the
top-level build uses each component's own copy (identical bytes).

## Build

Prerequisites: CMake >= 3.20, a C++17 compiler, X11 dev headers
(`libx11-dev` on Debian/Ubuntu).

Build all 194 demos:

```sh
cmake -S . -B build
cmake --build build -j"$(nproc)"
```

Binaries land in `build/components/<Name>/<Name>`.

Build a single component standalone:

```sh
cmake -S components/GtkButton -B /tmp/build-button
cmake --build /tmp/build-button
```

Or build just one target from the top-level tree:

```sh
cmake --build build --target GtkCalendar
```

## Run a demo

Demos need an X display (real X server or Xvfb):

```sh
./build/components/GtkButton/GtkButton
```

Each demo opens an 800x600 window, draws its widget, and handles expose,
key-press, mouse button press/release, pointer motion, and window-close
(WM_DELETE_WINDOW). Close the window (or press a key, depending on the demo)
to exit.

Under Xvfb:

```sh
Xvfb :99 -screen 0 1920x1080x24 &
DISPLAY=:99 ./build/components/GtkButton/GtkButton
```

## Test status

Tested Nov 26, 2026 on the VPS (Debian 12, g++ 12.2, Xvfb `:99` 1280x1024) with the
autonomous driver `test-driver/run_tests.py` — one demo at a time: open window, screenshot
to `screenshots/<Name>.png`, mouse sweep (hover), mousedown/mouseup at the widget centre
(click), Tab + letter (key), WM_DELETE_WINDOW (clean exit). Full table:
[`test-results/results.md`](test-results/results.md) (raw CSV:
[`test-results/results.csv`](test-results/results.csv), build log:
[`test-results/build.log`](test-results/build.log)).

- [x] All 194 demos build — clean from-scratch build, **0 errors, 0 warnings** with `-Wall -Wextra`
- [x] 193/194 demos draw (window opens, widget renders) — the one exception is GtkInvisible, which is non-visual by design
- [x] 125/194 demos show a visible click response (the other 69 are display-only widgets or containers that ignore a centre click by design; none crashed or hung)
- [ ] 12/194 demos show a visible hover response — the other 182 implement `on_motion` as a no-op by design (only 5 headers even declare a `hovered_` member)
- [x] All 194 demos survive Tab + letter keypresses
- [x] All 194 demos exit cleanly (rc 0 on WM_DELETE_WINDOW)
- [ ] Composite contact-sheet PNG of all 194 demos: `screenshots/` (per-demo PNGs done; composite by the screenshot agent)

Bugs fixed during testing (see `test-results/results.md` for detail): clean-exit heap
corruption in `SStyle_Manager::~SStyle_Manager` (`XFreeFont` after `XCloseDisplay` — crashed
all 194 demos on exit); GtkOverlay link failure (`static const int` odr-use);
TPopupMenu demo never called `popup()` (blank window); 7 `-Wall -Wextra` warnings fixed.

Reproduce:

```sh
cmake -S . -B build && cmake --build build -j4   # 0 warnings with -Wall -Wextra
Xvfb :99 -screen 0 1280x1024x24 &
DISPLAY=:99 python3 test-driver/run_tests.py     # writes test-results/results.csv
```

## License

MIT — see [LICENSE](LICENSE). Copyright (c) 2026 cnmcdee.
