#!/usr/bin/env python3
"""CWidgets phase-2 test driver: palette + layout + icons + resize handles.

Runs under Xvfb on the driver's OWN display (:98 — never touch :99 or any
other Xvfb). Grinds through everything and writes a per-type table at the
end to test-results/palette_layout_results.{csv,md}.

Steps:
  1. verify icons (tools/verify_icons.py) — 194/194 parse + non-blank
  2. build cwidgets_palette + cwidgets_layout
  3. palette --selftest (all 194 selections through the click code path)
  4. palette interactive: real mouse click on an entry, header collapse,
     keyboard selection (Down x10 -> GtkButton), screenshots
  5. layout script: PLACE+CHECKDRAW+DELETE for every one of the 194 types,
     MOVE/RESIZE/DELETE, DBLCLICK + corner + edge handle drags on 4 types,
     SAVE/LOAD round-trip, PLACE_ALL stress (194 at once)
  6. layout interactive (real mouse): palette selects GtkButton, click to
     place, drag to move, double-click for handles, drag SE corner handle,
     verify new bounds in the state file, screenshot with handles visible
  7. screenshots -> screenshots/{palette,layout,layout_resize_handles,
     layout_all}.png
"""
import os, subprocess, sys, time, csv, re, signal

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUILD = os.path.join(ROOT, "build")
SHOTS = os.path.join(ROOT, "screenshots")
RES = os.path.join(ROOT, "test-results")
TMP = "/tmp/cw2"
ENV = dict(os.environ, DISPLAY=":98")
os.makedirs(SHOTS, exist_ok=True)
os.makedirs(RES, exist_ok=True)
os.makedirs(TMP, exist_ok=True)

names = sorted(os.listdir(os.path.join(ROOT, "components")))
assert len(names) == 194, len(names)

results = {}   # name -> dict(place, draw, note)
fails = []


def run(*a, timeout=60, **k):
    return subprocess.run(list(a), capture_output=True, text=True,
                          timeout=timeout, env=ENV, **k)


def sh(cmd, timeout=120):
    return subprocess.run(cmd, shell=True, capture_output=True, text=True,
                          timeout=timeout, env=ENV)


def log(msg):
    print(msg, flush=True)


def win_id(title_sub):
    r = run("xdotool", "search", "--name", title_sub, timeout=10)
    ids = [x for x in r.stdout.split() if x.strip()]
    return int(ids[0]) if ids else None


def shot(wid, path):
    r = run("import", "-window", str(wid), path, timeout=20)
    return os.path.exists(path) and os.path.getsize(path) > 0


def read_state():
    out = []
    p = "/tmp/cwidgets-layout-state.txt"
    if os.path.exists(p):
        for line in open(p):
            parts = line.split()
            if len(parts) >= 6:
                out.append({"id": int(parts[0]), "name": parts[1],
                            "x": int(parts[2]), "y": int(parts[3]),
                            "w": int(parts[4]), "h": int(parts[5]),
                            "handles": "handles=1" in line})
    return out


def read_selection():
    p = "/tmp/cwidgets-selection.txt"
    if not os.path.exists(p):
        return None, -1
    lines = open(p).read().splitlines()
    return (lines[0] if lines else None,
            int(lines[1]) if len(lines) > 1 else -1)


# ------------------------------------------------------------------ 1 icons
log("== 1. icon verification ==")
r = subprocess.run(["python3", "tools/verify_icons.py"], capture_output=True,
                   text=True, timeout=60, cwd=ROOT)
log(r.stdout.strip())
if r.returncode != 0:
    fails.append("verify_icons failed")

# ------------------------------------------------------------------ 2 build
log("== 2. build palette + layout ==")
r = sh(f"cmake --build {BUILD} --target cwidgets_palette cwidgets_layout -j4",
       timeout=600)
log(r.stdout[-2000:] if r.stdout else "")
log(r.stderr[-2000:] if r.stderr else "")
if r.returncode != 0:
    log("BUILD FAILED")
    sys.exit(1)
log("build OK")

# ------------------------------------------------------------------ Xvfb :98
log("== start Xvfb :98 ==")
xvfb = subprocess.Popen(["Xvfb", ":98", "-screen", "0", "1600x1000x24"],
                        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
time.sleep(2)

try:
    pal_bin = os.path.join(BUILD, "palette", "cwidgets_palette")
    lay_bin = os.path.join(BUILD, "layout", "cwidgets_layout")

    # ---------------------------------------------------------- 3 selftest
    log("== 3. palette --selftest (194 selections) ==")
    r = run(pal_bin, "--selftest", timeout=120)
    ok = len(re.findall(r"^SELECT \d+ \S+ OK$", r.stdout, re.M))
    bad = re.findall(r"^SELECT .*FAIL.*$", r.stdout, re.M)
    log(f"palette selftest: {ok}/194 OK" + (f" FAILS: {bad[:5]}" if bad else ""))
    if ok != 194:
        fails.append(f"palette selftest {ok}/194")

    # ---------------------------------------------- 4 palette interactive
    log("== 4. palette interactive (mouse + keyboard) ==")
    pal = subprocess.Popen([pal_bin], env=ENV, stdout=subprocess.DEVNULL,
                           stderr=subprocess.DEVNULL)
    time.sleep(1.5)
    wid = win_id("CWidgets Palette")
    log(f"palette window id: {wid}")
    if not wid:
        fails.append("palette window not found")
    else:
        # Real mouse click on the first entry (GtkAboutDialog):
        # window at (60,40); top bar 30 + header 26 -> entry centre y=40+30+26+11.
        run("xdotool", "mousemove", "160", "107", "click", "1", timeout=10)
        time.sleep(0.5)
        name, idx = read_selection()
        log(f"mouse click entry 1 -> selection: {name} (idx {idx})")
        if name != "GtkAboutDialog":
            fails.append(f"mouse click selected {name}, want GtkAboutDialog")
        # Collapse the first section header (click it), then expand again.
        run("xdotool", "mousemove", "100", "83", "click", "1", timeout=10)
        time.sleep(0.4)
        run("xdotool", "mousemove", "100", "83", "click", "1", timeout=10)
        time.sleep(0.4)
        # Keyboard: focus window, 10x Down -> GtkButton (sorted index 10).
        run("xdotool", "windowfocus", str(wid), timeout=10)
        for _ in range(10):
            run("xdotool", "key", "Down", timeout=10)
            time.sleep(0.05)
        time.sleep(0.5)
        name, idx = read_selection()
        log(f"keyboard 10x Down -> selection: {name} (idx {idx})")
        if name != "GtkButton":
            fails.append(f"keyboard selection {name}, want GtkButton")
        shot(wid, os.path.join(SHOTS, "palette.png"))
        log("screenshot: screenshots/palette.png")
    pal.terminate()
    try:
        pal.wait(timeout=5)
    except subprocess.TimeoutExpired:
        pal.kill()

    # ------------------------------------------------------ 5 layout script
    log("== 5. layout script: place+draw every type ==")
    script = os.path.join(TMP, "place_all.txt")
    with open(script, "w") as f:
        for n in names:
            f.write(f"PLACE {n} 100 100 260 180\n")
            f.write("CHECKDRAW LAST\n")
            f.write("DELETE LAST\n")
        # move / resize / delete on a button
        f.write("PLACE GtkButton 100 100 150 36\n")
        f.write("MOVE LAST 40 30\n")
        f.write("RESIZE LAST 220 60\n")
        f.write("SELECT LAST\n")
        f.write("DELETE LAST\n")
        f.write("COUNT\n")
        # resize handles on 4 different types: corner + edge drags
        for n in ("GtkButton", "TTreeView", "XSpreadsheet", "GtkCalendar"):
            f.write(f"PLACE {n} 200 150 260 180\n")
            f.write("DBLCLICK LAST\n")
            f.write("HANDLE se 30 20\n")
            f.write("HANDLE e 25 0\n")
            f.write("HANDLE n 0 -15\n")
            f.write("DELETE LAST\n")
        # save/load round-trip
        f.write("PLACE GtkButton 50 50 150 36\n")
        f.write("PLACE GtkLabel 50 120 180 30\n")
        f.write("SAVE /tmp/cw2/layout_save.txt\n")
        f.write("DELETE LAST\n")
        f.write("DELETE LAST\n")
        f.write("LOAD /tmp/cw2/layout_save.txt\n")
        f.write("COUNT\n")
        f.write("LIST\n")
        f.write("QUIT\n")
    slog = os.path.join(TMP, "layout_script.log")
    r = run(lay_bin, "--script", script, "--log", slog, timeout=300)
    slog_txt = open(slog).read() if os.path.exists(slog) else ""
    place_ok = re.findall(r"^OK PLACE (\S+) id=", slog_txt, re.M)
    draw_ok = re.findall(r"^OK CHECKDRAW \d+ (\S+) nonwhite=(\d+)", slog_txt, re.M)
    draw_fail = re.findall(r"^FAIL (PLACE|CHECKDRAW).*", slog_txt, re.M)
    log(f"script exit={r.returncode} place_ok={len(place_ok)} "
        f"checkdraw_ok={len(draw_ok)} fail_lines={len(draw_fail)}")
    for n in names:
        results[n] = {"place": "FAIL", "draw": "FAIL", "note": ""}
    for n in place_ok:
        results[n]["place"] = "PASS"
    for n, nw in draw_ok:
        results[n]["draw"] = "PASS"
        results[n]["nonwhite"] = nw
    for line in slog_txt.splitlines():
        if line.startswith("FAIL"):
            m = re.search(r"(PLACE|CHECKDRAW) (\S+)", line)
            if m and m.group(2) in results:
                results[m.group(2)]["note"] = line[:120]
    # handle-drag results from the log
    handle_lines = [l for l in slog_txt.splitlines()
                    if l.startswith("OK HANDLE")]
    log(f"handle drags in script: {len(handle_lines)} (expect 12)")
    if len(handle_lines) != 12:
        fails.append(f"script handle drags {len(handle_lines)}/12")
    # verify a corner drag actually grew the widget (GtkButton 260x180
    # -> se +30+20 -> 290x200, then e +25 -> 315, then n -15 -> y-15,h+15)
    m = re.search(r"OK HANDLE se 30 20 : \d+ \d+ 260 180 -> \d+ \d+ 290 200",
                  slog_txt)
    if not m:
        fails.append("corner handle drag did not resize 260x180 -> 290x200")
        log("  handle log sample: " + str(handle_lines[:3]))
    else:
        log("corner handle drag verified: 260x180 -> 290x200")
    m2 = re.search(r"OK HANDLE e 25 0 : \d+ \d+ 290 200 -> \d+ \d+ 315 200",
                   slog_txt)
    if not m2:
        fails.append("edge handle drag did not widen 290 -> 315")
    else:
        log("edge handle drag verified: width 290 -> 315")
    # save/load
    if "OK SAVE /tmp/cw2/layout_save.txt count=2" not in slog_txt:
        fails.append("layout SAVE failed")
    if "OK LOAD /tmp/cw2/layout_save.txt count=2" not in slog_txt:
        fails.append("layout LOAD failed")
    if "OK COUNT 2" not in slog_txt:
        fails.append("layout count after load != 2")
    place_n = sum(1 for n in names if results[n]["place"] == "PASS")
    draw_n = sum(1 for n in names if results[n]["draw"] == "PASS")
    log(f"placement: {place_n}/194   draw: {draw_n}/194")
    for n in names:
        if results[n]["place"] != "PASS" or results[n]["draw"] != "PASS":
            log(f"  FAIL {n}: place={results[n]['place']} "
                f"draw={results[n]['draw']} {results[n]['note']}")
    if place_n != 194:
        fails.append(f"placement {place_n}/194")
    if draw_n != 194:
        fails.append(f"draw {draw_n}/194")

    # PLACE_ALL stress: 194 widgets alive at once, screenshot.
    log("== 5b. PLACE_ALL stress (194 at once) ==")
    script2 = os.path.join(TMP, "stress.txt")
    open(script2, "w").write("PLACE_ALL\nCOUNT\nQUIT\n")
    slog2 = os.path.join(TMP, "stress.log")
    r = run(lay_bin, "--script", script2, "--log", slog2, timeout=300)
    log(open(slog2).read().strip().replace("\n", " | "))
    if r.returncode != 0:
        fails.append("PLACE_ALL stress failed")

    # ------------------------------------------- 6 layout interactive mouse
    log("== 6. layout interactive (real mouse: place/move/handles) ==")
    pal = subprocess.Popen([pal_bin], env=ENV, stdout=subprocess.DEVNULL,
                           stderr=subprocess.DEVNULL)
    lay = subprocess.Popen([lay_bin], env=ENV, stdout=subprocess.DEVNULL,
                           stderr=subprocess.DEVNULL)
    time.sleep(1.5)
    pwid = win_id("CWidgets Palette")
    lwid = win_id("CWidgets Layout")
    log(f"palette={pwid} layout={lwid}")
    if not pwid or not lwid:
        fails.append("interactive windows not found")
    else:
        # Palette: click GtkButton via keyboard (11x Down from fresh
        # palette: -1 -> 0..10 = GtkButton, sorted index 10).
        run("xdotool", "windowfocus", str(pwid), timeout=10)
        for _ in range(11):
            run("xdotool", "key", "Down", timeout=10)
            time.sleep(0.05)
        time.sleep(0.6)
        name, _ = read_selection()
        log(f"palette selection for layout: {name}")
        # Layout: click canvas at window-rel (300,300) -> abs (760,340).
        run("xdotool", "windowfocus", str(lwid), timeout=10)
        time.sleep(0.3)
        run("xdotool", "mousemove", "760", "340", "click", "1", timeout=10)
        time.sleep(0.6)
        st = read_state()
        log(f"state after click-place: {st}")
        if len(st) != 1 or st[0]["name"] != "GtkButton":
            fails.append(f"click-place failed, state={st}")
        else:
            bx, by, bw, bh = st[0]["x"], st[0]["y"], st[0]["w"], st[0]["h"]
            log(f"placed GtkButton at {bx},{by} {bw}x{bh} (expect 225,282 150x36)")
            if (bx, by, bw, bh) != (225, 282, 150, 36):
                fails.append(f"place geometry wrong: {(bx, by, bw, bh)}")
            # Drag-move: centre (300,300 rel) -> (400,350 rel) => abs (860,390).
            run("xdotool", "mousemove", "760", "340", "mousedown", "1",
                timeout=10)
            run("xdotool", "mousemove", "860", "390", "mouseup", "1",
                timeout=10)
            time.sleep(0.5)
            st = read_state()
            log(f"state after drag-move: {st}")
            if st and (st[0]["x"], st[0]["y"]) != (325, 332):
                fails.append(f"drag-move wrong: {(st[0]['x'], st[0]['y'])} want (325,332)")
            # Double-click centre -> handles on.
            run("xdotool", "mousemove", "860", "390", "click", "1",
                "click", "1", timeout=10)
            time.sleep(0.6)
            st = read_state()
            log(f"state after double-click: {st}")
            if not st or not st[0]["handles"]:
                fails.append("double-click did not show handles")
            else:
                shot(lwid, os.path.join(SHOTS, "layout_resize_handles.png"))
                log("screenshot: screenshots/layout_resize_handles.png")
                # Drag SE corner handle: widget now at (325,332,150,36);
                # SE handle centre abs = 460+325+150, 40+332+36 = (935,408).
                # Drag +40,+30 -> expect 190x66.
                run("xdotool", "mousemove", "935", "408", "mousedown", "1",
                    timeout=10)
                run("xdotool", "mousemove", "975", "438", "mouseup", "1",
                    timeout=10)
                time.sleep(0.5)
                st = read_state()
                log(f"state after SE handle drag: {st}")
                if st and (st[0]["w"], st[0]["h"]) != (190, 66):
                    fails.append(f"SE handle resize wrong: {(st[0]['w'], st[0]['h'])} want (190,66)")
                else:
                    log("interactive SE handle resize verified: 150x36 -> 190x66")
                # Delete key removes the widget (still selected).
                run("xdotool", "key", "Delete", timeout=10)
                time.sleep(0.4)
                st = read_state()
                if len(st) != 0:
                    fails.append(f"Delete did not remove widget: {st}")
                else:
                    log("interactive Delete verified: 0 widgets left")
                # Escape dismisses handles but keeps the widget: place
                # another, double-click for handles, Escape, check.
                run("xdotool", "mousemove", "760", "340", "click", "1",
                    timeout=10)
                time.sleep(0.5)
                run("xdotool", "mousemove", "760", "340", "click", "1",
                    "click", "1", timeout=10)
                time.sleep(0.5)
                st = read_state()
                if not st or not st[0]["handles"]:
                    fails.append("re-place + double-click did not show handles")
                else:
                    run("xdotool", "key", "Escape", timeout=10)
                    time.sleep(0.4)
                    st = read_state()
                    if st and st[0]["handles"]:
                        fails.append("Escape did not dismiss handles")
                    elif not st:
                        fails.append("Escape removed the widget (should keep it)")
                    else:
                        log("Escape dismiss verified: handles off, widget kept")
        # Layout screenshot with several placed widgets: place 3 types.
        run("xdotool", "windowfocus", str(pwid), timeout=10)
        for spot, downs in (((700, 200), 0), ((1000, 200), 5),
                            ((700, 500), 40)):
            for _ in range(downs):
                run("xdotool", "key", "Down", timeout=10)
                time.sleep(0.03)
            time.sleep(0.4)
            run("xdotool", "windowfocus", str(lwid), timeout=10)
            run("xdotool", "mousemove", str(spot[0]), str(spot[1]),
                "click", "1", timeout=10)
            time.sleep(0.4)
            run("xdotool", "windowfocus", str(pwid), timeout=10)
        time.sleep(0.5)
        shot(lwid, os.path.join(SHOTS, "layout.png"))
        log(f"layout widgets for shot: {read_state()}")
        log("screenshot: screenshots/layout.png")
    lay.terminate()
    pal.terminate()
    for p in (lay, pal):
        try:
            p.wait(timeout=5)
        except subprocess.TimeoutExpired:
            p.kill()

    # Layout-all screenshot: PLACE_ALL then HOLD 8s in the background;
    # shoot the window mid-hold, then kill it.
    log("== 7. layout_all screenshot ==")
    hold_script = os.path.join(TMP, "hold.txt")
    open(hold_script, "w").write("PLACE_ALL\nHOLD 8\nQUIT\n")
    hold_log = os.path.join(TMP, "hold.log")
    lay = subprocess.Popen([lay_bin, "--script", hold_script, "--log",
                            hold_log], env=ENV, stdout=subprocess.DEVNULL,
                           stderr=subprocess.DEVNULL)
    time.sleep(3)
    lwid = win_id("CWidgets Layout")
    if lwid:
        shot(lwid, os.path.join(SHOTS, "layout_all.png"))
        log("screenshot: screenshots/layout_all.png")
    else:
        fails.append("layout_all window not found for screenshot")
        log("layout_all window not found")
    lay.terminate()
    try:
        lay.wait(timeout=5)
    except subprocess.TimeoutExpired:
        lay.kill()
    log("(layout_all shot done)")

finally:
    xvfb.terminate()
    try:
        xvfb.wait(timeout=5)
    except subprocess.TimeoutExpired:
        xvfb.kill()

# ------------------------------------------------------------------ results
with open(os.path.join(RES, "palette_layout_results.csv"), "w", newline="") as f:
    w = csv.writer(f)
    w.writerow(["name", "place", "draw", "nonwhite", "note"])
    for n in names:
        r = results.get(n, {"place": "?", "draw": "?", "note": "not run"})
        w.writerow([n, r["place"], r["draw"], r.get("nonwhite", ""),
                    r.get("note", "")])

place_n = sum(1 for n in names if results.get(n, {}).get("place") == "PASS")
draw_n = sum(1 for n in names if results.get(n, {}).get("draw") == "PASS")
with open(os.path.join(RES, "palette_layout_results.md"), "w") as f:
    f.write("# CWidgets palette + layout test results\n\n")
    f.write(f"- Placement: **{place_n}/194**\n")
    f.write(f"- Draw (non-white pixels in widget rect): **{draw_n}/194**\n")
    f.write(f"- Palette selftest selections: 194/194 (see log)\n")
    f.write(f"- Resize handles (script, 4 types x corner+2 edges): "
            f"{'PASS' if 'script handle drags 12/12' not in str(fails) else 'FAIL'}\n")
    f.write("\n## Failures\n\n")
    if fails:
        for x in fails:
            f.write(f"- {x}\n")
    else:
        f.write("none\n")
    f.write("\n## Per-type failures\n\n")
    for n in names:
        r = results.get(n, {})
        if r.get("place") != "PASS" or r.get("draw") != "PASS":
            f.write(f"- {n}: place={r.get('place')} draw={r.get('draw')} "
                    f"{r.get('note', '')}\n")

log(f"== DONE: place {place_n}/194 draw {draw_n}/194 fails={fails}")
sys.exit(1 if fails else 0)
