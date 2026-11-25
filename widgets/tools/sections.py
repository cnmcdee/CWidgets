# CWidgets palette section assignment.
# Every one of the 194 widgets belongs to exactly one section.
# Sections follow the natural GTK groupings, plus Misc / X for the
# X-prefixed custom widgets and non-visual helpers.

SECTIONS = [
    "Windows & Dialogs",
    "Containers & Layout",
    "Buttons",
    "Display",
    "Text & Entry",
    "Choosers",
    "Views & Lists",
    "Menus & Toolbars",
    "Misc / X",
]

# Section accent colours (used by the icon generator + palette headers).
SECTION_COLORS = {
    "Windows & Dialogs": (37, 99, 235),    # blue
    "Containers & Layout": (8, 145, 178),  # cyan
    "Buttons": (22, 163, 74),              # green
    "Display": (147, 51, 234),             # purple
    "Text & Entry": (234, 88, 12),         # orange
    "Choosers": (219, 39, 119),            # pink
    "Views & Lists": (77, 124, 15),        # olive
    "Menus & Toolbars": (180, 83, 9),      # amber
    "Misc / X": (220, 38, 38),             # red
}

ASSIGNMENT = {
    "Windows & Dialogs": [
        "GtkAboutDialog", "GtkAppChooserDialog", "GtkApplicationWindow",
        "GtkAssistant", "GtkColorChooserDialog", "GtkDialog",
        "GtkFileChooserDialog", "GtkFontChooserDialog", "GtkMessageDialog",
        "GtkPrintUnixDialog", "GtkShortcutsWindow", "GtkWindow",
        "TCalculatorDialog", "TCalendarDialog", "TColorDialog",
        "TFindDialog", "TFontDialog", "TForm", "TOpenDialog",
        "TOpenPictureDialog", "TPageSetupDialog", "TPrintDialog",
        "TPrinterSetupDialog", "TReplaceDialog", "TSaveDialog",
        "TSavePictureDialog", "TSelectDirectoryDialog", "TTaskDialog",
    ],
    "Containers & Layout": [
        "GtkAspectFrame", "GtkBox", "GtkButtonBox", "GtkCenterBox",
        "GtkExpander", "GtkFixed", "GtkFlowBox", "GtkFrame", "GtkGrid",
        "GtkLayout", "GtkNotebook", "GtkOverlay", "GtkPaned",
        "GtkRevealer", "GtkScrolledWindow", "GtkScrollbar",
        "GtkSeparator", "GtkStack", "GtkStackSidebar", "GtkStackSwitcher",
        "GtkViewport",
        "TControlBar", "TCoolBar", "TFlowPanel", "TFrame", "TGroupBox",
        "TNotebook", "TPageControl", "TPairSplitter", "TPanel",
        "TScrollBar", "TScrollBox", "TSplitter", "TTabControl",
    ],
    "Buttons": [
        "GtkButton", "GtkCheckButton", "GtkColorButton",
        "GtkColorDialogButton", "GtkFileChooserButton", "GtkFontButton",
        "GtkLinkButton", "GtkLockButton", "GtkModelButton",
        "GtkRadioButton", "GtkSwitch", "GtkToggleButton",
        "GtkToggleToolButton", "GtkToolButton", "GtkVolumeButton",
        "TBitBtn", "TButton", "TButtonPanel", "TCheckBox",
        "TCheckComboBox", "TRadioButton", "TSpeedButton", "TToggleBox",
        "TUpDown",
    ],
    "Display": [
        "GtkAccelLabel", "GtkCalendar", "GtkCellView", "GtkDrawingArea",
        "GtkGLArea", "GtkImage", "GtkInfoBar", "GtkInscription",
        "GtkInvisible", "GtkLabel", "GtkLevelBar", "GtkMediaControls",
        "GtkPicture", "GtkProgressBar", "GtkSpinner", "GtkStatusbar",
        "GtkVideo",
        "TArrow", "TBevel", "TCalendar", "TDateTimePicker", "TImage",
        "TImageList", "TLabel", "TPaintBox", "TProgressBar", "TShape",
        "TStaticText", "TStatusBar", "TTimeEdit", "TTimer", "TTrayIcon",
        "TTrackBar",
    ],
    "Text & Entry": [
        "GtkComboBox", "GtkComboBoxText", "GtkDropDown",
        "GtkEditableLabel", "GtkEntry", "GtkEntryCompletion",
        "GtkPasswordEntry", "GtkScale", "GtkScaleButton", "GtkSearchBar",
        "GtkSearchEntry", "GtkSpinButton", "GtkTextView",
        "TCalcEdit", "TComboBox", "TComboBoxEx", "TDateEdit",
        "TDirectoryEdit", "TEdit", "TEditButton", "TFileNameEdit",
        "TFilterComboBox", "TFloatSpinEdit", "TLabeledEdit", "TMaskEdit",
        "TMemo", "TSpinEdit", "TValueListEditor",
    ],
    "Choosers": [
        "GtkAppChooserButton", "GtkAppChooserWidget",
        "GtkColorChooserWidget", "GtkFileChooserWidget",
        "GtkFontChooserWidget", "GtkPlacesSidebar",
        "TColorBox", "TColorButton", "TColorListBox", "TFileListBox",
    ],
    "Views & Lists": [
        "GtkColumnView", "GtkGridView", "GtkIconView", "GtkListBox",
        "GtkListView", "GtkTreeView",
        "TCheckGroup", "TCheckListBox", "TDrawGrid", "THeaderControl",
        "TListBox", "TListView", "TRadioGroup", "TShellListView",
        "TShellTreeView", "TStringGrid", "TTreeView",
    ],
    "Menus & Toolbars": [
        "GtkActionBar", "GtkCheckMenuItem", "GtkHeaderBar", "GtkMenu",
        "GtkMenuBar", "GtkMenuButton", "GtkMenuItem", "GtkPopover",
        "GtkRadioMenuItem", "GtkSeparatorMenuItem", "GtkToolbar",
        "TActionList", "TMainMenu", "TPopupMenu", "TPopupNotifier",
        "TToolBar",
    ],
    "Misc / X": [
        "GtkEventBox", "TApplicationProperties", "XGraph", "XSpreadsheet",
    ],
}


def section_of(name):
    for sec, names in ASSIGNMENT.items():
        if name in names:
            return sec
    return None


def validate(all_names):
    """Check every widget appears in exactly one section."""
    seen = {}
    errors = []
    for sec, names in ASSIGNMENT.items():
        if len(names) != len(set(names)):
            errors.append(f"duplicate inside section {sec}")
        for n in names:
            if n in seen:
                errors.append(f"{n} in both {seen[n]} and {sec}")
            seen[n] = sec
    for n in all_names:
        if n not in seen:
            errors.append(f"{n} has no section")
    for n in seen:
        if n not in all_names:
            errors.append(f"{n} assigned but no such component")
    return errors


if __name__ == "__main__":
    import os, sys
    comps = sorted(os.listdir(os.path.join(os.path.dirname(__file__), "..", "components")))
    errs = validate(comps)
    total = sum(len(v) for v in ASSIGNMENT.values())
    print(f"sections: {len(SECTIONS)}  assigned: {total}  components: {len(comps)}")
    for sec in SECTIONS:
        print(f"  {sec}: {len(ASSIGNMENT[sec])}")
    if errs:
        print("ERRORS:")
        for e in errs:
            print("  " + e)
        sys.exit(1)
    print("section assignment OK: 194 widgets, exactly one section each")
