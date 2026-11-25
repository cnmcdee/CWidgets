import re, os, sys
import os
REPO_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
sys.path.insert(0, REPO_ROOT + "/widgets/tools")
from sections import ASSIGNMENT
DOCS = REPO_ROOT + "/docs"
EXDIR = "/tmp/dw65-examples"
FILES = {"Text & Entry": "text-entry.md", "Views & Lists": "views-lists.md", "Menus & Toolbars": "menus-toolbars.md", "Misc / X": "misc-x.md"}
errors = []
total = 0
for sec, fname in FILES.items():
    text = open(os.path.join(DOCS, fname)).read()
    heads = re.findall(r"^## (\S+)\s*$", text, re.M)
    expect = ASSIGNMENT[sec]
    total += len(expect)
    if heads != expect:
        errors.append(fname + ": headings != sections.py order; missing=" + str(set(expect)-set(heads)) + " extra=" + str(set(heads)-set(expect)))
    # structure checks
    if not text.startswith("# " + sec + "\n"):
        errors.append(fname + ": bad title line")
    if "docs/index.md" not in text:
        errors.append(fname + ": missing index.md reference")
    for name in expect:
        sec_text = text.split("## " + name, 1)[1].split("\n## ", 1)[0]
        for token in ["**Constructor:** `" + name + "(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`",
                      "**API**", "**Example:**", "```cpp", "**Notes:**"]:
            if token not in sec_text:
                errors.append(fname + "/" + name + ": missing " + token[:40])
        # example matches compiled file byte for byte
        m = re.search(r"```cpp\n(.*?)\n```", sec_text, re.S)
        if not m:
            errors.append(fname + "/" + name + ": no example fence")
        else:
            cpp = open(os.path.join(EXDIR, name + ".cpp")).read().rstrip("\n")
            if m.group(1) != cpp:
                errors.append(fname + "/" + name + ": example differs from compiled .cpp")
print("widgets checked: " + str(total))
print("ERRORS: " + str(len(errors)))
for e in errors:
    print("  " + e)
for f in FILES.values():
    print(f + ": " + str(os.path.getsize(os.path.join(DOCS, f))) + " bytes")
