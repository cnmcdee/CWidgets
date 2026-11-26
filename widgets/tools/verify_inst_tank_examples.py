#!/usr/bin/env python3
"""Verify every code example in docs/instrumentation-tanks.md compiles
against its widget header. Extracts every ```cpp block, maps it to its
widget by the enclosing ## heading, wraps fragments in a harness that
opens a display/window/styles and provides display/screen/window/gc/
styles, then compiles + links (no run). Writes
widgets/test-results/inst_tank_examples.csv. Exit 0 only if all pass.
Attribution: XFrame."""
import csv, os, re, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REPO = os.path.dirname(ROOT)
WIDGETS = os.path.join(REPO, "widgets")
COMP = os.path.join(WIDGETS, "components")
DOC = os.path.join(REPO, "docs", "instrumentation-tanks.md")
TMP = "/tmp/inst-tank-examples"
OUT = os.path.join(WIDGETS, "test-results", "inst_tank_examples.csv")
os.makedirs(TMP, exist_ok=True)
os.makedirs(os.path.dirname(OUT), exist_ok=True)

HARNESS_PRE = """#include "%s.h"
#include <cstdio>
int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
"""

HARNESS_POST = """
    return 0;
}
"""

FENCE = chr(96) * 3
text = open(DOC).read()
# split into (heading, block) pairs: a ## heading followed by ```cpp blocks
rows = []
fails = []
cur = None
for m in re.finditer(r"^## (\S+)\s*$|```cpp\n(.*?)```", text, re.M | re.S):
    if m.group(1):
        cur = m.group(1)
    elif m.group(2) is not None:
        rows.append((cur, m.group(2)))

print("found %d cpp blocks in %s" % (len(rows), DOC))
results = []
for i, (name, code) in enumerate(rows):
    if not name or not name.startswith("Inst"):
        results.append((name, "skip", "no widget heading"))
        continue
    src = os.path.join(TMP, "ex_%d.cpp" % i)
    body = code if "int main" in code else (HARNESS_PRE % name) + code + HARNESS_POST
    open(src, "w").write(body)
    out = os.path.join(TMP, "ex_%d" % i)
    r = subprocess.run(["g++", "-std=c++17", "-Wall", "-Wextra",
                        "-I", os.path.join(COMP, name), src, "-lX11", "-o", out],
                       capture_output=True, text=True, timeout=120)
    ok = r.returncode == 0 and "warning:" not in r.stderr
    note = "" if ok else (r.stderr.strip()[-400:] or "rc=%d" % r.returncode)
    results.append((name, "PASS" if ok else "FAIL", note))
    print("%s: %s %s" % (name, "PASS" if ok else "FAIL", note))
    if not ok:
        fails.append(name)

with open(OUT, "w", newline="") as f:
    w = csv.writer(f)
    w.writerow(["widget", "doc", "kind", "result", "note"])
    for name, res, note in results:
        w.writerow([name, "docs/instrumentation-tanks.md", "fragment", res, note])

npass = sum(1 for _, res, _ in results if res == "PASS")
print("tank doc examples: %d/%d PASS" % (npass, len(results)))
sys.exit(0 if not fails else 1)
