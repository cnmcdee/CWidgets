# CWidgets test results

Driver: `test-driver/run_tests.py` under Xvfb (`:99`, 1280x1024x24), one demo at a time: window opens, default screenshot to `screenshots/<Name>.png`, mouse sweep for hover, mousedown/mouseup at the widget centre for click, Tab + `a` for key, WM_DELETE_WINDOW for clean exit. Build: clean from-scratch `cmake -S . -B build && cmake --build build -j4` with `-Wall -Wextra` (see `test-results/build.log`). Thresholds: draw = >20 non-background pixels; hover/click = >10 changed pixels vs the default screenshot (all genuine non-responses measured exactly 0 changed pixels, so the cut is clean).

## Summary (194 components)

| Check | PASS | NORESP | FAIL |
|---|---:|---:|---:|
| Build | 194 | 0 | 0 |
| Draw (non-blank window) | 193 | 0 | 1 |
| Hover (visible response) | 12 | 182 | 0 |
| Click (visible response) | 125 | 69 | 0 |
| Key (survives Tab + letter) | 194 | 0 | 0 |
| Clean exit (rc 0 on WM_DELETE_WINDOW) | 194 | 0 | 0 |

Fully passing (build + draw + click + key + exit): **125/194**.
NORESP is not a crash: it means the widget produced no *observable* response to that input. See the design notes below.

## FAILs

- **GtkInvisible** (draw): blank window: only 0 non-bg px, modal 100.0%; no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo)

## Design notes (not bugs)

- **Hover: 182/194 NORESP.** Only 12 widgets have a visible hover state (GtkCheckMenuItem, GtkLinkButton, GtkMenu, GtkMenuItem, GtkModelButton, GtkRadioMenuItem, GtkToolButton, GtkToolbar, TMainMenu, TPopupMenu, TSpeedButton, XGraph). The other 182 implement `on_motion` as a no-op returning false (only 5 headers even declare a `hovered_` member) — the demos dispatch MotionNotify, the widgets just don't change appearance. No crash, no hang.
- **GtkInvisible** is the single draw FAIL, by design: its `draw()` intentionally does nothing ("Non-visual component"), so its window is blank; click/hover are no-ops too. It builds, survives keys, and exits cleanly.
- **Click NORESP (69):** GtkAboutDialog, GtkAccelLabel, GtkActionBar, GtkAppChooserDialog, GtkApplicationWindow, GtkAspectFrame, GtkAssistant, GtkDialog, GtkEventBox, GtkFileChooserDialog, GtkFileChooserWidget, GtkFontChooserWidget, GtkFrame, GtkGrid, GtkHeaderBar, GtkImage, GtkInfoBar, GtkInscription, GtkInvisible, GtkLabel, GtkLayout, GtkMenuBar, GtkMessageDialog, GtkPicture, GtkPopover, GtkPrintUnixDialog, GtkScrolledWindow, GtkSeparator, GtkSeparatorMenuItem, GtkShortcutsWindow, GtkStatusbar, GtkViewport, TActionList, TApplicationProperties, TArrow, TBevel, TButtonPanel, TCalculatorDialog, TColorDialog, TCoolBar, TDrawGrid, TFontDialog, TGroupBox, TImageList, TLabel, TLabeledEdit, TListView, TOpenDialog, TOpenPictureDialog, TPageControl, TPageSetupDialog, TPaintBox, TPopupNotifier, TProgressBar, TRadioGroup, TReplaceDialog, TSaveDialog, TSavePictureDialog, TScrollBar, TScrollBox, TSelectDirectoryDialog, TShape, TStatusBar, TStringGrid, TTabControl, TTaskDialog, TTimer, TToolBar, TTrayIcon. Mostly display-only widgets (labels, separators, frames, progress bars, status bars) and containers/dialogs whose inner controls aren't hit by a centre click. All 194 demos survived the click with no crash or hang.
- **Animated widgets** (e.g. GtkSpinner) redraw on their own timer, so a screenshot diff after an input can partly reflect animation, not the input.
- **TRadioButton** exited rc=1 at startup once in the first full run, then passed a manual run and the entire second full run plus standalone re-runs — treated as a transient XOpenDisplay failure under load, not a component bug.

## Bugs fixed during testing

1. **Clean-exit crash (all 194 demos).** Every demo segfaulted/aborted on WM_DELETE_WINDOW: `SStyle_Manager::~SStyle_Manager()` called `XFreeFont()` after `main` had already called `XCloseDisplay()` (stack unwinding), corrupting the heap (gdb: SIGSEGV in `XFreeFont` <- `~SStyle_Manager` <- `main`). Fixed in `core/xframe_core.h` (all 195 copies, identical): the destructor no longer calls `XFreeFont` — the fonts are process-lifetime resources the X server reclaims on disconnect; `delete_style()` still frees a single style while the display is open.
2. **GtkOverlay link failure.** `static const int ALIGN_START/CENTER/END` odr-used by `std::max/min` in `clamp_align()` with no out-of-class definition. Changed to `static constexpr int` (C++17: implicitly inline).
3. **TPopupMenu demo showed a blank window.** The widget starts hidden by design and the demo never called `popup()`. `main_TPopupMenu.cpp` now calls `widget.popup()` before `draw()`.
4. **Warnings (7, all fixed, build is now `-Wall -Wextra` clean):** unused parameter `y` in `GtkEntry::on_motion`, `GtkSwitch::on_button_release`, `THeaderControl::on_button_release` (`(void)y;`); sign-compare in `GtkTextView` (`nl == std::string::npos` -> `nl == -1`, 2 sites); `char num[8]` -> `num[16]` in `TCalendar` / `TDateEdit` (`-Wformat-truncation`). `-Wall -Wextra` added to the top-level `CMakeLists.txt`.

## Per-component table

| Component | Build | Draw | Hover | Click | Key | Exit | Notes |
|---|:---:|:---:|:---:|:---:|:---:|:---:|---|
| GtkAboutDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkAccelLabel | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkActionBar | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkAppChooserButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkAppChooserDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkAppChooserWidget | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkApplicationWindow | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkAspectFrame | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkAssistant | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkButtonBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkCalendar | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkCellView | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkCenterBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkCheckButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkCheckMenuItem | PASS | PASS | PASS | PASS | PASS | PASS |  |
| GtkColorButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkColorChooserDialog | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkColorChooserWidget | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkColorDialogButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkColumnView | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkComboBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkComboBoxText | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkDrawingArea | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op); minimal but real click response (text cursor / click mark, 25 px) |
| GtkDropDown | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkEditableLabel | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkEntry | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkEntryCompletion | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkEventBox | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkExpander | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkFileChooserButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkFileChooserDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkFileChooserWidget | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkFixed | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkFlowBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkFontButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkFontChooserDialog | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkFontChooserWidget | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkFrame | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkGLArea | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkGrid | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkGridView | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkHeaderBar | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkIconView | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkImage | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkInfoBar | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkInscription | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkInvisible | PASS | FAIL | NORESP | NORESP | PASS | PASS | blank window: only 0 non-bg px, modal 100.0%; no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkLabel | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkLayout | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkLevelBar | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkLinkButton | PASS | PASS | PASS | PASS | PASS | PASS |  |
| GtkListBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkListView | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkLockButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkMediaControls | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkMenu | PASS | PASS | PASS | PASS | PASS | PASS |  |
| GtkMenuBar | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkMenuButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkMenuItem | PASS | PASS | PASS | PASS | PASS | PASS |  |
| GtkMessageDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkModelButton | PASS | PASS | PASS | PASS | PASS | PASS |  |
| GtkNotebook | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkOverlay | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkPaned | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkPasswordEntry | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkPicture | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkPlacesSidebar | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkPopover | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkPrintUnixDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkProgressBar | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkRadioButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkRadioMenuItem | PASS | PASS | PASS | PASS | PASS | PASS |  |
| GtkRevealer | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkScale | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkScaleButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkScrollbar | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkScrolledWindow | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkSearchBar | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkSearchEntry | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkSeparator | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkSeparatorMenuItem | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkShortcutsWindow | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkSpinButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkSpinner | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkStack | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkStackSidebar | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkStackSwitcher | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkStatusbar | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkSwitch | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkTextView | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkToggleButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkToggleToolButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkToolButton | PASS | PASS | PASS | PASS | PASS | PASS |  |
| GtkToolbar | PASS | PASS | PASS | PASS | PASS | PASS |  |
| GtkTreeView | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkVideo | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkViewport | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| GtkVolumeButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| GtkWindow | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TActionList | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TApplicationProperties | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TArrow | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TBevel | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TBitBtn | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TButtonPanel | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TCalcEdit | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op); minimal but real click response (text cursor / click mark, 30 px) |
| TCalculatorDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TCalendar | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TCalendarDialog | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TCheckBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TCheckComboBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TCheckGroup | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TCheckListBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TColorBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TColorButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TColorDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TColorListBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TComboBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TComboBoxEx | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TControlBar | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TCoolBar | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TDateEdit | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TDateTimePicker | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TDirectoryEdit | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op); minimal but real click response (text cursor / click mark, 30 px) |
| TDrawGrid | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TEdit | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op); minimal but real click response (text cursor / click mark, 29 px) |
| TEditButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op); minimal but real click response (text cursor / click mark, 30 px) |
| TFileListBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TFileNameEdit | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op); minimal but real click response (text cursor / click mark, 30 px) |
| TFilterComboBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TFindDialog | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op); minimal but real click response (text cursor / click mark, 18 px) |
| TFloatSpinEdit | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op); minimal but real click response (text cursor / click mark, 30 px) |
| TFlowPanel | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TFontDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TForm | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TFrame | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TGroupBox | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| THeaderControl | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TImage | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TImageList | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TLabel | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TLabeledEdit | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TListBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TListView | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TMainMenu | PASS | PASS | PASS | PASS | PASS | PASS |  |
| TMaskEdit | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op); minimal but real click response (text cursor / click mark, 28 px) |
| TMemo | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op); minimal but real click response (text cursor / click mark, 13 px) |
| TNotebook | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TOpenDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TOpenPictureDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TPageControl | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TPageSetupDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TPaintBox | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TPairSplitter | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TPanel | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TPopupMenu | PASS | PASS | PASS | PASS | PASS | PASS |  |
| TPopupNotifier | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TPrintDialog | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TPrinterSetupDialog | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TProgressBar | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TRadioButton | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TRadioGroup | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TReplaceDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TSaveDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TSavePictureDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TScrollBar | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TScrollBox | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TSelectDirectoryDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TShape | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TShellListView | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TShellTreeView | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TSpeedButton | PASS | PASS | PASS | PASS | PASS | PASS |  |
| TSpinEdit | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op); minimal but real click response (text cursor / click mark, 30 px) |
| TSplitter | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TStaticText | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TStatusBar | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TStringGrid | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TTabControl | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TTaskDialog | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TTimeEdit | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TTimer | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TToggleBox | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TToolBar | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TTrackBar | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TTrayIcon | PASS | PASS | NORESP | NORESP | PASS | PASS | no visible hover response (on_motion no-op); no observable click response (no pressed/focus visual, no on_click in demo) |
| TTreeView | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TUpDown | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| TValueListEditor | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
| XGraph | PASS | PASS | PASS | PASS | PASS | PASS |  |
| XSpreadsheet | PASS | PASS | NORESP | PASS | PASS | PASS | no visible hover response (on_motion no-op) |
