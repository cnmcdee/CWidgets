#!/usr/bin/env python3
"""Instrumentation tank-level group driver: builds the 8 tank widgets,
checks InstData.h determinism + set_level clamping for every widget,
runs each demo headless under Xvfb :94, screenshots + hover/click/key/
clean-exit. Re-derives every count from fresh artifacts each round;
exits non-zero on any failure. Attribution: XFrame."""
import os, subprocess, sys, time
from PIL import Image, ImageChops

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
COMP = os.path.join(ROOT, "components")
BUILD = os.path.join(ROOT, "build", "components")
TMP = "/tmp/inst-tanks-shots"
ENV = dict(os.environ, DISPLAY=":94")
WIDGETS = ["InstVerticalTankLevel", "InstHorizontalTankLevel",
           "InstSphericalTankLevel", "InstSightGlassLevel",
           "InstLevelColumnGauge", "InstTankLevelAlarms",
           "InstOpenChannelLevel", "InstLevelTransmitter"]
os.makedirs(TMP, exist_ok=True)
failures = []
rows = []

def run(*a, timeout=30, **k):
    return subprocess.run(*a, capture_output=True, text=True, timeout=timeout, env=ENV, **k)

def check(cond, msg):
    if not cond:
        failures.append(msg)
    return cond

def nonbg(path):
    im = Image.open(path).convert("L")
    h = im.histogram()
    modal = max(range(256), key=lambda v: h[v])
    return sum(c for v, c in enumerate(h) if abs(v - modal) > 10)

def diff(a, b):
    ia = Image.open(a).convert("RGB"); ib = Image.open(b).convert("RGB")
    if ia.size != ib.size:
        ib = ib.resize(ia.size)
    return sum(ImageChops.difference(ia, ib).convert("L").histogram()[31:])

def shot(wid, path):
    r = run(["import", "-window", str(wid), path], timeout=15)
    return os.path.exists(path) and os.path.getsize(path) > 0

# --- Xvfb :94 ---
xvfb = None
if not os.path.exists("/tmp/.X94-lock"):
    xvfb = subprocess.Popen(["Xvfb", ":94", "-screen", "0", "1024x768x24"])
    time.sleep(1.5)

# --- send_delete helper ---
sd = "/tmp/send_delete"
if not os.path.exists(sd):
    subprocess.run(["cc", os.path.join(ROOT, "test-driver", "send_delete.c"), "-lX11", "-o", sd],
                   capture_output=True, timeout=60)

# --- InstData + set_level clamp check (one program, all 8 widgets) ---
clamp_src = os.path.join(TMP, "inst_tanks_clamp.cpp")
parts = ['#include "%s.h"\n' % w for w in WIDGETS]
parts.append('#include <cstdio>\n#include <cmath>\nint main() {\n')
parts.append('    Display* display = XOpenDisplay(nullptr);\n    if (!display) return 1;\n')
parts.append('    int screen = DefaultScreen(display);\n')
parts.append('    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));\n')
parts.append('    GC gc = DefaultGC(display, screen);\n')
parts.append('    SStyle_Manager styles(display, window, screen, gc);\n')
parts.append('    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");\n')
parts.append('    styles.set_active_style_byName("default");\n')
parts.append('    int fails = 0;\n')
parts.append('    auto chk = [&](bool ok, const char* m) { if (!ok) { std::printf("FAIL %s\\n", m); fails++; } };\n')
for w in WIDGETS:
    short = w.replace("Inst", "")
    parts.append('    { %s wd(display, screen, window, gc, &styles, 20, 20, 720, 480);\n' % w)
    parts.append('      wd.set_level(-10.0); chk(wd.get_level() == 0.0, "%s clamp low");\n' % short)
    parts.append('      wd.set_level(150.0); chk(wd.get_level() == 100.0, "%s clamp high");\n' % short)
    parts.append('      wd.set_level(42.5); chk(wd.get_level() == 42.5, "%s set/get");\n' % short)
    parts.append('      chk(wd.alarm_bands().hi == 80.0 && wd.alarm_bands().hi_hi == 95.0 && wd.alarm_bands().lo == 20.0 && wd.alarm_bands().lo_lo == 5.0, "%s bands");\n' % short)
    parts.append('      wd.draw(); }\n')
parts.append('    double a = xframe::inst_demo_level(42), b = xframe::inst_demo_level(42);\n')
parts.append('    chk(a == b && a >= 5.0 && a <= 95.0, "demo level deterministic/range");\n')
parts.append('    chk(xframe::inst_clamp_level(-3.0) == 0.0 && xframe::inst_clamp_level(101.0) == 100.0, "clamp fn");\n')
parts.append('    std::printf(fails == 0 ? "clamp ok\\n" : "clamp FAILURES\\n");\n')
parts.append('    return fails == 0 ? 0 : 2;\n}\n')
open(clamp_src, "w").write("".join(parts))
inc = []
for w in WIDGETS:
    inc += ["-I", os.path.join(COMP, w)]
clamp_bin = os.path.join(TMP, "inst_tanks_clamp")
r = subprocess.run(["g++", "-std=c++17", "-Wall", "-Wextra"] + inc +
                   [clamp_src, "-lX11", "-o", clamp_bin],
                   capture_output=True, text=True, timeout=120)
check(r.returncode == 0 and "warning:" not in r.stderr, "clamp program compile failed/warnings: %s" % r.stderr[:500])
if r.returncode == 0:
    r2 = run([clamp_bin], timeout=30)
    check(r2.returncode == 0, "clamp check rc=%d out=%s" % (r2.returncode, (r2.stdout + r2.stderr)[:300]))
    print("InstData + set_level clamp: %s" % ("PASS (8/8 widgets: -10->0, 150->100, 42.5->42.5, bands 95/80/20/5)" if r2.returncode == 0 else "FAIL"))

try:
    for name in WIDGETS:
        row = {"name": name, "build": "FAIL", "draw": "FAIL", "hover": "FAIL",
               "click": "FAIL", "key": "FAIL", "exit": "FAIL", "notes": ""}
        notes = []
        bdir = os.path.join(BUILD, name)
        r = subprocess.run(["cmake", "-S", os.path.join(COMP, name), "-B", bdir,
                            "-DCMAKE_CXX_FLAGS=-Wall -Wextra"],
                           capture_output=True, text=True, timeout=120)
        out = r.stdout + r.stderr
        r2 = subprocess.run(["cmake", "--build", bdir], capture_output=True, text=True, timeout=180) if r.returncode == 0 else None
        out2 = (r2.stdout + r2.stderr) if r2 else ""
        binp = os.path.join(bdir, name)
        if r.returncode != 0 or (r2 and r2.returncode != 0):
            notes.append("build failed: %s" % (out + out2)[-400:])
            row["notes"] = "; ".join(notes); rows.append(row); failures.append("%s: build failed" % name); continue
        if "warning:" in out2:
            notes.append("compiler warnings present")
            failures.append("%s: warnings: %s" % (name, out2[out2.find("warning:"):out2.find("warning:") + 200]))
        if not (os.path.exists(binp) and os.access(binp, os.X_OK)):
            notes.append("binary missing"); row["notes"] = "; ".join(notes); rows.append(row); failures.append("%s: binary missing" % name); continue
        row["build"] = "PASS"
        logf = open(os.path.join(TMP, name + ".stderr"), "wb")
        p = subprocess.Popen([binp], env=ENV, stdout=subprocess.DEVNULL, stderr=logf)
        wid = None
        try:
            for _ in range(50):
                if p.poll() is not None:
                    break
                rr = run(["xdotool", "search", "--name", "%s - XFrame demo" % name], timeout=10)
                if rr.stdout.strip():
                    wid = rr.stdout.strip().split()[0]; break
                time.sleep(0.2)
            if wid is None:
                notes.append("no window; rc=%s" % p.poll()); failures.append("%s: no window" % name)
                for c in ("draw", "hover", "click", "key", "exit"): row[c] = "FAIL"
                row["notes"] = "; ".join(notes); rows.append(row)
                try: p.wait(timeout=3)
                except Exception: p.kill()
                logf.close(); continue
            rr = run(["xdotool", "getwindowgeometry", "--shell", str(wid)], timeout=10)
            g = {}
            for line in rr.stdout.splitlines():
                if "=" in line:
                    k, v = line.split("=", 1)
                    try: g[k] = int(v)
                    except ValueError: pass
            wx, wy = g.get("X", 100), g.get("Y", 100)
            cx, cy = wx + 20 + 360, wy + 20 + 240
            time.sleep(0.4)
            dflt = os.path.join(TMP, name + "_default.png")
            if not shot(wid, dflt):
                notes.append("screenshot failed"); failures.append("%s: screenshot failed" % name)
            else:
                nb = nonbg(dflt)
                if nb > 20: row["draw"] = "PASS"
                else: notes.append("blank: %d non-bg px" % nb); failures.append("%s: blank draw" % name)
            for i in range(1, 9):
                run(["xdotool", "mousemove", str(wx + 20 + 720 * i // 9), str(wy + 20 + 480 * i // 9)], timeout=10)
                time.sleep(0.04)
            run(["xdotool", "mousemove", str(cx), str(cy)], timeout=10)
            time.sleep(0.35)
            hov = os.path.join(TMP, name + "_hover.png")
            if shot(wid, hov) and os.path.exists(dflt):
                hd = diff(dflt, hov)
                row["hover"] = "PASS" if hd > 50 else "FAIL"
                if hd <= 50: notes.append("no hover response (%d)" % hd); failures.append("%s: no hover response" % name)
            else:
                notes.append("hover shot failed"); failures.append("%s: hover shot failed" % name)
            run(["xdotool", "mousemove", str(cx), str(cy)], timeout=10)
            run(["xdotool", "mousedown", "1"], timeout=10)
            time.sleep(0.35)
            prs = os.path.join(TMP, name + "_pressed.png")
            if shot(wid, prs) and os.path.exists(dflt):
                pd = diff(dflt, prs)
                row["click"] = "PASS" if pd > 10 else "FAIL"
                if pd <= 10: notes.append("no click response (%d)" % pd); failures.append("%s: no click response" % name)
            else:
                notes.append("click shot failed"); failures.append("%s: click shot failed" % name)
            run(["xdotool", "mouseup", "1"], timeout=10)
            time.sleep(0.2)
            if p.poll() is None:
                run(["xdotool", "key", "--window", str(wid), "Tab"], timeout=10)
                time.sleep(0.2)
                run(["xdotool", "key", "--window", str(wid), "a"], timeout=10)
                time.sleep(0.3)
                row["key"] = "PASS" if p.poll() is None else "FAIL"
                if p.poll() is not None: notes.append("crashed on key rc=%s" % p.poll()); failures.append("%s: crashed on key" % name)
            else:
                row["key"] = "FAIL"; notes.append("died before key rc=%s" % p.poll()); failures.append("%s: died before key" % name)
            if p.poll() is None:
                run([sd, str(wid)], timeout=10)
                try:
                    rc = p.wait(timeout=5)
                    row["exit"] = "PASS" if rc == 0 else "FAIL"
                    if rc != 0: notes.append("exit rc=%s" % rc); failures.append("%s: exit rc=%s" % (name, rc))
                except subprocess.TimeoutExpired:
                    row["exit"] = "FAIL"; notes.append("hang on WM_DELETE"); failures.append("%s: hang on exit" % name)
                    p.kill(); p.wait(timeout=5)
            else:
                row["exit"] = "FAIL"; notes.append("already dead rc=%s" % p.poll()); failures.append("%s: dead before exit" % name)
        finally:
            if p.poll() is None:
                p.kill()
                try: p.wait(timeout=3)
                except Exception: pass
            logf.close()
        row["notes"] = "; ".join(notes)
        rows.append(row)
        print("%s: build=%s draw=%s hover=%s click=%s key=%s exit=%s" % (name, row["build"], row["draw"], row["hover"], row["click"], row["key"], row["exit"]) + ("  [%s]" % row["notes"] if row["notes"] else ""), flush=True)
finally:
    if xvfb is not None:
        xvfb.terminate()
        try: xvfb.wait(timeout=5)
        except Exception: xvfb.kill()

npass = sum(1 for r in rows if all(r[c] == "PASS" for c in ("build", "draw", "hover", "click", "key", "exit")))
print("Inst tanks group: %d/%d widgets fully PASS; failures=%d" % (npass, len(WIDGETS), len(failures)))
for f in failures:
    print("FAIL:", f)
sys.exit(0 if not failures and npass == len(WIDGETS) else 1)
