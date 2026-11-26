#!/usr/bin/env python3
"""Link each widget's example picture into its doc entry.

Inserts `![<Name>](../screenshots/widgets/<Name>.png)` directly under the
widget's heading in:
  - the 9 original section docs (docs/*.md, `## <Name>` headings), and
  - docs/stock-market.md (287 stock widgets; headings vary: `## IndX`,
    `## <bare> (`IndX`)`, `## StockX`, `### StockX`).
Idempotent: skips a heading that already has the link. Verifies afterwards
that every widget heading carries the link and prints a per-doc count.

Usage: python3 widgets/tools/link_widget_pics.py [--check]
"""
import os, re, sys

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
WIDGETS = os.path.join(REPO, "widgets")
sys.path.insert(0, os.path.join(WIDGETS, "tools"))
from widget_count import registry_names

SECTION_DOCS = [
    "docs/windows-dialogs.md", "docs/containers-layout.md",
    "docs/buttons.md", "docs/display.md", "docs/text-entry.md",
    "docs/choosers.md", "docs/views-lists.md", "docs/menus-toolbars.md",
    "docs/misc-x.md",
]
STOCK_DOC = "docs/stock-market.md"
INSTRUMENTATION_DOC = "docs/instrumentation.md"

def heading_widget(heading, names):
    """Map a doc heading to a registry widget name."""
    toks = re.findall(r"[A-Za-z][A-Za-z0-9]+", heading)
    for t in toks:
        if t in names:
            return t
    for t in toks:
        for prefix in ("Ind", "Stock"):
            if prefix + t in names:
                return prefix + t
    return None

def link_for(name):
    return "![%s](../screenshots/widgets/%s.png)" % (name, name)

def process(path, check_only=False):
    names = set(registry_names())
    lines = open(os.path.join(REPO, path)).read().splitlines()
    out = []
    linked = 0
    missing = []
    i = 0
    while i < len(lines):
        ln = lines[i]
        out.append(ln)
        m = re.match(r"#{2,3}\s+(.*)$", ln)
        if m:
            name = heading_widget(m.group(1), names)
            if name:
                # the link sits one blank line under the heading
                nxt = lines[i + 1] if i + 1 < len(lines) else ""
                nxt2 = lines[i + 2] if i + 2 < len(lines) else ""
                if nxt.strip() == link_for(name) or nxt2.strip() == link_for(name):
                    linked += 1
                elif check_only:
                    missing.append(name)
                else:
                    out.append("")
                    out.append(link_for(name))
                    linked += 1
        i += 1
    if not check_only:
        open(os.path.join(REPO, path), "w").write("\n".join(out) + "\n")
    return linked, missing

def main():
    check_only = "--check" in sys.argv
    total = 0
    all_missing = []
    for doc in SECTION_DOCS + [STOCK_DOC, INSTRUMENTATION_DOC]:
        linked, missing = process(doc, check_only)
        print("%s: %d picture links" % (doc, linked))
        total += linked
        all_missing += missing
    print("total links: %d" % total)
    if all_missing:
        print("headings WITHOUT a picture link: %d" % len(all_missing))
        for n in sorted(set(all_missing)):
            print("  MISSING", n)
        return 1
    return 0

if __name__ == "__main__":
    sys.exit(main())
