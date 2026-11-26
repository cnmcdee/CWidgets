#!/usr/bin/env python3
"""Verify every doc code example compiles against its widget header.

Extracts every ```cpp block from the 9 original section docs + 6 stock
fragment docs, maps each block to its widget (by the enclosing ##/### heading),
and compiles it:
  - full programs (contain 'int main') compile as-is;
  - fragments are wrapped in a harness that opens a display/window/styles
    and provides display/screen/window/gc/styles, then compiles.
Writes widgets/test-results/doc_examples.csv (name, doc, kind, result, note)
and prints a summary. Exit code 0 only if every mapped widget passes.

Usage: python3 widgets/tools/verify_doc_examples.py [--only NAME,...]
"""
import csv, os, re, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REPO = os.path.dirname(ROOT)
WIDGETS = os.path.join(REPO, "widgets")
TMP = "/tmp/doc-examples"
OUT = os.environ.get("DOC_EXAMPLES_OUT",
                     os.path.join(WIDGETS, "test-results", "doc_examples.csv"))
os.makedirs(TMP, exist_ok=True)
os.makedirs(os.path.dirname(OUT), exist_ok=True)

sys.path.insert(0, os.path.join(WIDGETS, "tools"))
from widget_count import registry_names

DOCS = [
    ("docs/windows-dialogs.md", "orig"),
    ("docs/containers-layout.md", "orig"),
    ("docs/buttons.md", "orig"),
    ("docs/display.md", "orig"),
    ("docs/text-entry.md", "orig"),
    ("docs/choosers.md", "orig"),
    ("docs/views-lists.md", "orig"),
    ("docs/menus-toolbars.md", "orig"),
    ("docs/misc-x.md", "orig"),
    ("docs/stock-market-ohlc.md", "stock"),
    ("docs/stock-market-compare-volume.md", "stock"),
    ("docs/stock-market-drawing.md", "stock"),
    ("docs/stock-market-indicators-trend.md", "stock"),
    ("docs/stock-market-indicators-momentum.md", "stock"),
    ("docs/stock-market-indicators-volatility.md", "stock"),
]

FENCE = chr(96) * 3

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

def heading_to_widget(heading, group):
    """Map a doc heading to a registry widget name."""
    names = registry_names()
    if heading in names:
        return heading
    if heading.startswith("Ind") and heading in names:
        return heading
    # bare indicator name, e.g. ## ADX -> IndADX
    cand = "Ind" + heading
    if cand in names:
        return cand
    # bare stock name, e.g. ## VWAP -> StockVWAP
    cand = "Stock" + heading
    if cand in names:
        return cand
    return None

def extract_blocks(docpath):
    """Yield (heading, block) in order; heading is the nearest ##/### above."""
    txt = open(os.path.join(REPO, docpath)).read()
    lines = txt.splitlines()
    # find headings with their line numbers
    events = []
    for i, ln in enumerate(lines):
        m = re.match(r"#{2,3}\s+(\S+)", ln)
        if m:
            events.append((i, m.group(1)))
        if ln.strip() == FENCE + "cpp":
            events.append((i, "FENCE_START"))
        if ln.strip() == FENCE and events and events[-1][1] == "FENCE_START":
            events.append((i, "FENCE_END"))
    out = []
    heading = None
    start = None
    for i, kind in events:
        if kind == "FENCE_START":
            start = i
        elif kind == "FENCE_END" and start is not None:
            block = "\n".join(lines[start + 1:i])
            out.append((heading, block))
            start = None
        elif kind not in ("FENCE_START", "FENCE_END"):
            heading = kind
    return out

def compile_src(name, src, kind):
    srcf = os.path.join(TMP, name + ".cpp")
    binf = os.path.join(TMP, name)
    open(srcf, "w").write(src)
    cmd = ["g++", "-std=c++17", "-Wall", "-Wextra",
           "-I", WIDGETS, "-I", os.path.join(WIDGETS, "components", name),
           srcf, "-o", binf, "-lX11"]
    r = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
    if r.returncode == 0:
        return True, ""
    err = (r.stderr or "").strip().splitlines()
    return False, " | ".join(err[:4])

def main():
    only = None
    if len(sys.argv) > 1 and sys.argv[1].startswith("--only"):
        only = set(sys.argv[1].split("=", 1)[1].split(","))
    names = registry_names()
    # map widget -> list of (doc, block)
    found = {}
    unmapped_blocks = []
    for doc, group in DOCS:
        for heading, block in extract_blocks(doc):
            if not block.strip():
                continue
            w = heading_to_widget(heading, group) if heading else None
            if w is None:
                unmapped_blocks.append((doc, heading, block[:60]))
                continue
            found.setdefault(w, []).append((doc, block))
    rows = []
    for name in names:
        if only and name not in only:
            continue
        blocks = found.get(name, [])
        if not blocks:
            rows.append((name, "", "missing", "FAIL",
                         "no code example in any doc"))
            continue
        # compile every block for this widget; all must pass
        ok_all = True
        notes = []
        kinds = []
        for doc, block in blocks:
            if "int main" in block:
                kind = "full"
                src = block
            else:
                kind = "fragment"
                # strip leading #include of the widget header if present to
                # avoid a duplicate include (harness adds it)
                body = re.sub(r'^#include\s+"%s\.h"\s*$' % re.escape(name),
                              "", block, flags=re.M)
                src = (HARNESS_PRE % name) + body + HARNESS_POST
            kinds.append(kind)
            ok, note = compile_src(name, src, kind)
            if not ok:
                ok_all = False
                notes.append("%s:%s: %s" % (doc, kind, note))
        rows.append((name, ";".join(d for d, _ in blocks),
                     "+".join(sorted(set(kinds))),
                     "PASS" if ok_all else "FAIL", "; ".join(notes)))
    with open(OUT, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["name", "doc", "kind", "result", "note"])
        w.writerows(rows)
    npass = sum(1 for r in rows if r[3] == "PASS")
    nfail = [r for r in rows if r[3] == "FAIL"]
    print("widgets checked: %d  PASS: %d  FAIL: %d" % (len(rows), npass, len(nfail)))
    for r in nfail:
        print("FAIL %s [%s] %s" % (r[0], r[2], r[4][:300]))
    if unmapped_blocks:
        print("unmapped code blocks (no widget heading): %d" % len(unmapped_blocks))
        for doc, h, s in unmapped_blocks[:10]:
            print("  %s heading=%s :: %s" % (doc, h, s.replace("\n", " ")))
    print("wrote", OUT)
    return 0 if not nfail else 1

if __name__ == "__main__":
    sys.exit(main())
