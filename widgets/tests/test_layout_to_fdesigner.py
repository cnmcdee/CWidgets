#!/usr/bin/env python3
"""Seam test: XFrame layout file -> .fdj -> real FDesigner load + codegen
+ CMake build of the generated project.

Steps:
  1. Build a layout file with all 194 XFrame names (read from
     widgets/common/registry.h, the single source of truth).
  2. Run tools/layout_to_fdesigner.py; check exit code, .fdj JSON,
     every widget type is a real designer type, geometry preserved,
     every name accounted for (mapped or reported unmapped).
  3. Compile a small C++ harness against designer/src that loads the
     .fdj with the real project_io and generates code with the real
     generator; run it.
  4. Build the generated project with CMake; assert the `app` binary
     is produced.
  5. With --gui (and DISPLAY set) launch the generated app briefly
     and check its window appears.
"""

import json
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
WIDGETS = os.path.join(REPO, "widgets")
DESIGNER_SRC = os.path.join(REPO, "designer", "src")
CONVERTER = os.path.join(WIDGETS, "tools", "layout_to_fdesigner.py")

# The 36 designer types (designer/src/model.h kTypes).
DESIGNER_TYPES = {
    "Button", "Label", "Entry", "Checkbutton", "Radiobutton", "Frame",
    "Combobox", "Listbox", "Menubutton", "Canvas", "Scale", "Scrollbar",
    "Text", "Toplevel", "Spreadsheet", "StockChart", "Memo", "GroupBox",
    "Panel", "Image", "Shape", "Notebook", "ScrollBox", "Splitter",
    "StatusBar", "ToolBar", "MenuBar", "SpinEdit", "TrackBar",
    "ProgressBar", "DateTimePicker", "ColorBox", "ListView", "TreeView",
    "StringGrid", "Plot",
}

HARNESS_CPP = r"""
#include <cstdio>
#include "project_io.h"
#include "generator.h"
int main(int, char** argv) {
    Project p;
    std::string err;
    if (!load_project_file(argv[1], p, err)) {
        fprintf(stderr, "load failed: %s\n", err.c_str());
        return 1;
    }
    printf("loaded %d widgets\n", (int)p.widgets.size());
    GenResult r = generate_project(p, p.widgets, argv[2]);
    if (!r.ok) {
        fprintf(stderr, "generate failed: %s\n", r.error.c_str());
        return 2;
    }
    printf("generated %d files\n", (int)r.files.size());
    return 0;
}
"""


def run(cmd, **kw):
    r = subprocess.run(cmd, capture_output=True, text=True, **kw)
    return r


def fail(msg):
    print(f"FAIL: {msg}")
    sys.exit(1)


def main():
    gui = "--gui" in sys.argv
    tmp = tempfile.mkdtemp(prefix="cwidgets-seam-")
    print(f"work dir: {tmp}")

    # 1. All 194 names from the registry header.
    reg = open(os.path.join(WIDGETS, "common", "registry.h")).read()
    m = re.search(r"kWidgetNames\[kWidgetCount\] = \{(.*?)\};", reg, re.S)
    names = re.findall(r'"([^"]+)"', m.group(1))
    assert len(names) == 194, f"registry has {len(names)} names, expected 194"
    layout_path = os.path.join(tmp, "all194.txt")
    with open(layout_path, "w") as f:
        for i, n in enumerate(names):
            f.write(f"{n} {(i % 10) * 130} {(i // 10) * 90} 120 40\n")
    # Plus a comment, a blank line, and one malformed line (must warn, not die).
    with open(layout_path, "a") as f:
        f.write("# comment line\n\nGtkButton broken\n")

    # 2. Convert.
    fdj_path = os.path.join(tmp, "all194.fdj")
    r = run([sys.executable, CONVERTER, layout_path, fdj_path])
    print(r.stdout, end="")
    print(r.stderr, end="")
    if r.returncode != 0:
        fail(f"converter exited {r.returncode}")
    fdj = json.load(open(fdj_path))
    assert fdj["format"] == "fdesigner" and fdj["version"] == 1
    by_type = {}
    for w in fdj["widgets"]:
        if w["type"] not in DESIGNER_TYPES:
            fail(f"widget {w['name']} has unknown designer type {w['type']}")
        by_type.setdefault(w["type"], 0)
        by_type[w["type"]] += 1
    names_in = {w["name"] for w in fdj["widgets"]}
    assert len(names_in) == len(fdj["widgets"]), "duplicate widget names"
    # Geometry preserved: find the GtkButton entry (first line of the layout).
    btn = [w for w in fdj["widgets"] if w["props"].get("label") == "GtkButton"]
    assert len(btn) == 1, f"expected 1 GtkButton widget, got {len(btn)}"
    _i = names.index("GtkButton")
    assert (btn[0]["x"], btn[0]["y"], btn[0]["width"], btn[0]["height"]) == ((_i % 10) * 130, (_i // 10) * 90, 120, 40), btn[0]
    sys.path.insert(0, os.path.join(WIDGETS, "tools"))
    import layout_to_fdesigner as conv
    mapped_n = len(conv.MAPPING)
    unmapped_n = 194 - mapped_n
    assert len(fdj["widgets"]) == mapped_n, (
        f"expected {mapped_n} mapped widgets, got {len(fdj['widgets'])}")
    assert "unmapped" in r.stdout and str(unmapped_n) in r.stdout, (
        f"report should mention {unmapped_n} unmapped")
    print(f"PASS converter: {mapped_n} mapped, {unmapped_n} unmapped, "
          f"{len(by_type)} designer types used, geometry preserved")

    # 3. Real load + generate via the designer's own sources.
    harness_cpp = os.path.join(tmp, "harness.cpp")
    open(harness_cpp, "w").write(HARNESS_CPP)
    harness = os.path.join(tmp, "harness")
    r = run(["g++", "-std=c++17", "-O2", "-Wall", "-Wextra",
             f"-I{DESIGNER_SRC}", harness_cpp,
             os.path.join(DESIGNER_SRC, "project_io.cpp"),
             os.path.join(DESIGNER_SRC, "generator.cpp"),
             os.path.join(DESIGNER_SRC, "cmake_build.cpp"),
             "-o", harness])
    if r.returncode != 0:
        print(r.stdout, r.stderr)
        fail("harness compile failed")
    if r.stderr.strip():
        print("harness build warnings:\n" + r.stderr)
    gen_out = os.path.join(tmp, "generated")
    r = run([harness, fdj_path, gen_out])
    print(r.stdout, end="")
    if r.returncode != 0:
        print(r.stderr)
        fail("harness load/generate failed")
    assert os.path.exists(os.path.join(gen_out, "CMakeLists.txt")), "no CMakeLists.txt generated"
    assert os.path.exists(os.path.join(gen_out, "Makefile")), "no Makefile generated"
    print("PASS load+generate with the real designer sources")

    # 4. CMake-build the generated project.
    gen_build = os.path.join(gen_out, "build")
    r = run(["cmake", "-S", gen_out, "-B", gen_build])
    if r.returncode != 0:
        print(r.stdout, r.stderr)
        fail("cmake configure of generated project failed")
    r = run(["cmake", "--build", gen_build, "-j4"])
    if r.returncode != 0:
        print(r.stdout[-3000:], r.stderr[-3000:])
        fail("cmake build of generated project failed")
    app = os.path.join(gen_build, "app")
    assert os.path.exists(app), "generated app binary missing"
    print("PASS generated project builds with CMake -> build/app")

    # 5. Optional GUI: the generated app opens its window.
    if gui:
        display = os.environ.get("DISPLAY")
        if not display:
            fail("--gui needs DISPLAY set")
        import time
        proc = subprocess.Popen([app], stdout=subprocess.DEVNULL,
                                stderr=subprocess.DEVNULL)
        try:
            ok = False
            for _ in range(50):
                time.sleep(0.1)
                rr = run(["xdotool", "search", "--name", "XFrame Layout"])
                if rr.stdout.strip():
                    ok = True
                    break
            if not ok:
                fail("generated app window did not appear")
            print("PASS generated app window appears under X")
        finally:
            proc.terminate()
            try:
                proc.wait(timeout=5)
            except subprocess.TimeoutExpired:
                proc.kill()

    print("SEAM TEST: ALL PASS")
    return 0


if __name__ == "__main__":
    sys.exit(main())
