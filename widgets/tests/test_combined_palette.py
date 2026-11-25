#!/usr/bin/env python3
"""End-to-end test for the combined 230-entry palette.

  1. Freshness: widgets/tools/gen_xframe_palette_data.py --check
     (the generated designer/src/xframe_palette.h is current).
  2. Presence: FDESIGNER_PALETTE_DUMP lists 36 FDesigner entries (exact
     type set from designer/src/model.h) + 194 XFrame entries in their 9
     sections with the exact per-section counts, registry indices
     matching widgets/common/registry.h, names unique.
  3. Selection: FDESIGNER_PALETTE_SELFTEST drives every palette button
     through its real clicked handler — 36 canvas adds + 194 selection
     writes — and must report pass=230 fail=0.
  4. Interactive (real mouse): under Xvfb, launch ./ide, click the
     first XFrame entry (GtkAboutDialog) in the combined palette,
     check the selection file, click in the layout window, check the
     layout state file shows the real widget placed. Screenshots of
     the palette (top + scrolled to the XFrame sections) go to
     screenshots/ by default.

Usage: test_combined_palette.py [--display :97] [--shots-dir DIR]
The script starts its own Xvfb on the display if none is running there
(and stops it afterwards only if it started it).
"""

import argparse
import os
import re
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
WIDGETS = os.path.join(REPO, "widgets")
DESIGNER = os.path.join(REPO, "designer")
SHOTS_DEFAULT = os.path.join(REPO, "screenshots")
DISPLAY = ":97"

SECTION_COUNTS = {
    "Windows & Dialogs": 28, "Containers & Layout": 34, "Buttons": 24,
    "Display": 33, "Text & Entry": 28, "Choosers": 10, "Views & Lists": 17,
    "Menus & Toolbars": 16, "Misc / X": 4,
}


def run(cmd, **kw):
    return subprocess.run(cmd, capture_output=True, text=True, **kw)


def fail(msg):
    print(f"FAIL: {msg}")
    sys.exit(1)


def registry_names():
    text = open(os.path.join(WIDGETS, "common", "registry.h")).read()
    m = re.search(r"kWidgetNames\[kWidgetCount\] = \{(.*?)\};", text, re.S)
    return re.findall(r'"([^"]+)"', m.group(1))


def designer_types():
    text = open(os.path.join(DESIGNER, "src", "model.h")).read()
    return re.findall(r'\{"(\w+)",\s*"pal_', text)


def xdotool(*args, timeout=10):
    env = dict(os.environ, DISPLAY=DISPLAY)
    return subprocess.run(["xdotool"] + list(args), capture_output=True,
                          text=True, timeout=timeout, env=env)


def win_id(title_sub):
    r = xdotool("search", "--name", title_sub)
    ids = [x for x in r.stdout.split() if x.strip()]
    return int(ids[0]) if ids else None


def win_geom(wid):
    r = xdotool("getwindowgeometry", "--shell", str(wid))
    d = dict(line.split("=", 1) for line in r.stdout.splitlines() if "=" in line)
    return int(d["X"]), int(d["Y"]), int(d["WIDTH"]), int(d["HEIGHT"])


def xvfb_ensure():
    env = dict(os.environ, DISPLAY=DISPLAY)
    r = subprocess.run(["xdotool", "getdisplaygeometry"], capture_output=True,
                       env=env)
    if r.returncode == 0:
        print(f"display {DISPLAY} already up, reusing it")
        return None
    p = subprocess.Popen(["Xvfb", DISPLAY, "-screen", "0", "2560x1440x24"],
                         stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    for _ in range(100):
        time.sleep(0.1)
        r = subprocess.run(["xdotool", "getdisplaygeometry"],
                           capture_output=True, env=env)
        if r.returncode == 0:
            print(f"started Xvfb {DISPLAY} (pid {p.pid})")
            return p
    p.kill()
    fail(f"Xvfb {DISPLAY} did not come up")


def main():
    global DISPLAY
    ap = argparse.ArgumentParser()
    ap.add_argument("--display", default=":97")
    ap.add_argument("--shots-dir", default=SHOTS_DEFAULT)
    args = ap.parse_args()
    DISPLAY = args.display
    os.makedirs(args.shots_dir, exist_ok=True)

    # 0. Clean stale IPC files so reads below are fresh.
    for p in ("/tmp/cwidgets-selection.txt", "/tmp/cwidgets-layout-state.txt"):
        try:
            os.unlink(p)
        except FileNotFoundError:
            pass

    # 1. Generated header is current.
    r = run([sys.executable,
             os.path.join(WIDGETS, "tools", "gen_xframe_palette_data.py"),
             "--check"])
    print(r.stdout.strip())
    if r.returncode != 0:
        fail("xframe_palette.h is stale")

    # 2. Designer builds.
    r = run(["make", "-C", DESIGNER])
    if r.returncode != 0:
        print(r.stdout[-2000:], r.stderr[-2000:])
        fail("designer build failed")
    if "warning:" in r.stdout + r.stderr:
        print("WARNINGS in designer build:\n" + r.stdout + r.stderr)
        fail("designer build has warnings")

    xvfb = xvfb_ensure()
    env = dict(os.environ, DISPLAY=DISPLAY)
    try:
        # 3. Presence via the dump hook.
        dump = "/tmp/combined-palette-dump.txt"
        try:
            os.unlink(dump)
        except FileNotFoundError:
            pass
        proc = subprocess.Popen(
            [os.path.join(DESIGNER, "fdesigner")],
            cwd=DESIGNER, env=dict(env, FDESIGNER_PALETTE_DUMP=dump),
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        try:
            for _ in range(100):
                if os.path.exists(dump):
                    break
                time.sleep(0.1)
            else:
                fail("palette dump was not written")
            time.sleep(0.5)  # let allocations settle
            fd, xf = [], []
            for line in open(dump):
                line = line.strip()
                if line.startswith("FDESIGNER "):
                    fd.append(line.split()[1])
                elif line.startswith("XFRAME "):
                    parts = line.split()
                    # XFRAME <section...> <name> <index> <x> <y> — section
                    # may contain spaces, so parse from the right.
                    y, x, idx = int(parts[-1]), int(parts[-2]), int(parts[-3])
                    name = parts[-4]
                    section = " ".join(parts[1:-4])
                    xf.append((section, name, idx, x, y))
            want_fd = designer_types()
            assert len(want_fd) == 36, f"model.h has {len(want_fd)} types"
            if sorted(fd) != sorted(want_fd):
                fail(f"FDesigner entries mismatch: {sorted(set(fd) ^ set(want_fd))}")
            if len(xf) != 194:
                fail(f"expected 194 XFrame entries, got {len(xf)}")
            names = [n for _, n, _, _, _ in xf]
            if len(set(names)) != 194:
                fail("duplicate XFrame names in palette")
            reg = registry_names()
            assert len(reg) == 194
            counts = {}
            for section, name, idx, x, y in xf:
                counts[section] = counts.get(section, 0) + 1
                if reg.index(name) != idx:
                    fail(f"{name}: dump index {idx} != registry {reg.index(name)}")
            if counts != SECTION_COUNTS:
                fail(f"section counts {counts} != {SECTION_COUNTS}")
            ys = [y for *_, y in xf]
            if ys != sorted(ys):
                fail("XFrame entries not in increasing y order")
            print(f"PASS presence: 36 FDesigner + 194 XFrame, sections {counts}")
        finally:
            proc.terminate()
            try:
                proc.wait(timeout=10)
            except subprocess.TimeoutExpired:
                proc.kill()

        # 4. Selection self-test: every button through its real handler.
        slog = "/tmp/combined-palette-selftest.log"
        r = run([os.path.join(DESIGNER, "fdesigner")], cwd=DESIGNER,
                env=dict(env, FDESIGNER_PALETTE_SELFTEST=slog), timeout=300)
        log = open(slog).read() if os.path.exists(slog) else ""
        fails = [l for l in log.splitlines() if l.startswith("FAIL")]
        m = re.search(r"SUMMARY pass=(\d+) fail=(\d+)", log)
        if r.returncode != 0 or not m or m.group(1) != "230" or m.group(2) != "0":
            print(log[-2000:])
            fail(f"palette self-test failed (exit {r.returncode}): {fails[:5]}")
        print("PASS self-test: 230/230 buttons select (36 canvas + 194 selection writes)")

        # 5. Interactive: real mouse through ./ide.
        try:
            os.unlink(dump)  # stale coords from step 3 must not satisfy the wait
        except FileNotFoundError:
            pass
        ide_log = open("/tmp/combined-palette-ide.log", "w")
        ide = subprocess.Popen(
            [os.path.join(REPO, "ide")], cwd=REPO,
            env=dict(env, FDESIGNER_PALETTE_DUMP=dump),
            stdout=ide_log, stderr=subprocess.STDOUT)
        try:
            for _ in range(100):
                if os.path.exists(dump) and win_id("FDesigner — Widgets"):
                    break
                time.sleep(0.1)
            else:
                fail("ide did not open the palette window")
            time.sleep(1.0)  # let all five windows settle
            titles = xdotool("search", "--name", ".*", "getwindowname", "%@").stdout
            for want in ("FDesigner — C++/GTK", "FDesigner — Widgets",
                         "FDesigner — Code", "CWidgets Palette",
                         "CWidgets Layout"):
                if want not in titles:
                    fail(f"ide window missing: {want}")
            print("PASS ide opens all five top-level windows")

            # Dump coords for the first XFrame entry (visible at scroll-top).
            entry = None
            for line in open(dump):
                if line.startswith("XFRAME Windows & Dialogs GtkAboutDialog "):
                    p = line.split()
                    entry = (int(p[-2]), int(p[-1]))
                    break
            assert entry, "GtkAboutDialog not in dump"
            pal = win_id("FDesigner — Widgets")
            px, py, pw, ph = win_geom(pal)
            cx, cy = px + entry[0] + 60, py + entry[1] + 10
            xdotool("mousemove", str(cx), str(cy), "click", "1")
            time.sleep(0.7)
            sel = open("/tmp/cwidgets-selection.txt").read().splitlines()
            if sel[:1] != ["GtkAboutDialog"] or sel[1:2] != ["0"]:
                fail(f"selection file after palette click: {sel}")
            print("PASS real click selects GtkAboutDialog (selection file)")

            lay = win_id("CWidgets Layout")
            lx, ly, lw, lh = win_geom(lay)
            # Raise it first: the designer/code windows overlap it, and a
            # click lands on whatever is topmost at that point.
            xdotool("windowraise", str(lay))
            time.sleep(0.5)
            xdotool("mousemove", str(lx + 200), str(ly + 200), "click", "1")
            state = ""
            for _ in range(30):
                time.sleep(0.1)
                try:
                    state = open("/tmp/cwidgets-layout-state.txt").read()
                except FileNotFoundError:
                    continue
                if "GtkAboutDialog" in state:
                    break
            if "GtkAboutDialog" not in state:
                fail(f"layout state after place click has no GtkAboutDialog:\n{state[:500]}")
            print("PASS real click places the real GtkAboutDialog in the layout window")

            # Screenshots: palette at top, then scrolled to XFrame sections.
            def shot(wid, path):
                xdotool("windowraise", str(wid))
                time.sleep(0.5)
                subprocess.run(["import", "-window", str(wid), path],
                               env=env, capture_output=True, timeout=30)
                print(f"shot {path} ({os.path.getsize(path)} bytes)")
            shot(pal, os.path.join(args.shots_dir, "combined_palette_top.png"))
            # Scroll with the pointer over the palette: scroll events sent
            # to the toplevel window never reach the scrolled window.
            px, py, pw, ph = win_geom(pal)
            xdotool("mousemove", str(px + pw // 2), str(py + ph // 2))
            for _ in range(14):
                xdotool("click", "5")
                time.sleep(0.05)
            shot(pal, os.path.join(args.shots_dir, "combined_palette_xframe.png"))
            shot(lay, os.path.join(args.shots_dir, "combined_palette_layout_placed.png"))
        finally:
            did = win_id("FDesigner — C\+\+")
            if did:
                xdotool("windowclose", str(did))
            try:
                ide.wait(timeout=15)
            except subprocess.TimeoutExpired:
                ide.kill()
            ide_log.close()
            print(f"ide exit: {ide.returncode}")
    finally:
        if xvfb:
            xvfb.terminate()

    print("COMBINED PALETTE TEST: ALL PASS")
    return 0


if __name__ == "__main__":
    sys.exit(main())
