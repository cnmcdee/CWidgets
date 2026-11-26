#!/usr/bin/env python3
"""Driver for Instrumentation groups D (gauges) + E (panel furniture).

Builds all 12 demos with -Wall -Wextra -Werror (0 warnings), runs the
in-process clamp/pixel test program, runs each demo under Xvfb and
screenshots it (non-blank check), and compiles every code example in
docs/instrumentation-gauges.md. Prints a per-item pass/fail table.
Exit 0 only if everything passes.
Attribution: XFrame.
"""
import os, re, subprocess, sys, time
from PIL import Image

REPO = "/opt/cwidgets"
WIDGETS = os.path.join(REPO, "widgets")
BUILD = "/tmp/inst-de-build"
SHOTS = os.path.join(WIDGETS, "screenshots")
DOC = os.path.join(REPO, "docs", "instrumentation-gauges.md")
DISPLAY = None  # picked at runtime: first free :91-:99 (other workers use Xvfb too)

NAMES = ["InstAnalogGauge", "InstPressureGauge", "InstTemperatureGauge",
         "InstBarMeter", "InstVuMeter", "InstDigitalReadout", "InstManometer",
         "InstFlowMeter", "InstPanelPlate", "InstNameplate", "InstBezel",
         "InstControlPanel"]

results = {}

def record(name, ok, note=""):
    results[name] = (ok, note)
    print(("PASS " if ok else "FAIL ") + name + ("  " + note if note else ""))

def run(cmd, env=None, timeout=120):
    e = dict(os.environ); e.update(env or {})
    return subprocess.run(cmd, capture_output=True, text=True, timeout=timeout, env=e)

def build_demo(name):
    d = os.path.join(WIDGETS, "components", name)
    b = os.path.join(BUILD, name)
    os.makedirs(b, exist_ok=True)
    r = run(["cmake", "-S", d, "-B", b, "-DCMAKE_CXX_FLAGS=-Wall -Wextra -Werror",
             "-DCMAKE_BUILD_TYPE=Release"])
    if r.returncode != 0:
        return False, "cmake failed: " + (r.stderr or r.stdout)[-500:]
    r = run(["cmake", "--build", b, "-j4"])
    if r.returncode != 0 or "warning:" in r.stdout + r.stderr:
        return False, "build failed/warnings: " + (r.stdout + r.stderr)[-500:]
    return True, os.path.join(b, name)

def screenshot(name, binary):
    env = {"DISPLAY": DISPLAY}
    p = subprocess.Popen([binary], env={**os.environ, **env},
                         stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(1.0)
    alive = p.poll() is None
    shot = os.path.join(SHOTS, name + ".png")
    ok, note = False, ""
    if alive:
        xwd = subprocess.run(["xwd", "-root", "-silent", "-display", DISPLAY],
                             capture_output=True, timeout=30)
        conv = subprocess.run(["convert", "xwd:-", "-crop", "800x600+100+100", "png:" + shot],
                              input=xwd.stdout, capture_output=True, timeout=30)
        if conv.returncode == 0 and os.path.exists(shot):
            im = Image.open(shot).convert("RGB")
            px = list(im.getdata())
            nonwhite = sum(1 for t in px if t != (255, 255, 255))
            ok = nonwhite > 10000
            note = "%d non-white px" % nonwhite if ok else "blank shot (%d non-white px)" % nonwhite
        else:
            note = "convert failed"
    else:
        note = "demo exited early rc=%s" % p.returncode
    try:
        p.terminate(); p.wait(timeout=5)
    except Exception:
        try: p.kill()
        except Exception: pass
    return ok, note

HARNESS_PRE = """#include <cstdio>
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
    XCloseDisplay(display);
    return 0;
}
"""

def verify_doc_examples():
    text = open(DOC).read()
    blocks = []
    cur = None
    for line in text.splitlines():
        m = re.match(r"## (\w+)", line)
        if m: cur = m.group(1)
        if line.strip() == "```cpp": blocks.append([cur, []])
        elif line.strip() == "```" and blocks and blocks[-1][1] is not None and len(blocks[-1][1]) >= 0 and not blocks[-1][1].__class__ == list:
            pass
        elif blocks and line.strip() == "```":
            pass
    # simpler: regex over fenced blocks with preceding heading
    blocks = []
    parts = re.split(r"(?m)^## (\w+)\s*$", text)
    for i in range(1, len(parts), 2):
        name, body = parts[i], parts[i + 1]
        for m in re.finditer(r"```cpp\n(.*?)```", body, re.S):
            blocks.append((name, m.group(1)))
    allok = True
    for name, code in blocks:
        src = os.path.join(BUILD, "docex_" + name + ".cpp")
        with open(src, "w") as f:
            f.write("#include \"" + name + ".h\"\n" + HARNESS_PRE + code + HARNESS_POST)
        inc = os.path.join(WIDGETS, "components", name)
        r = run(["g++", "-std=c++17", "-Wall", "-Wextra", "-Werror", "-I", inc,
                 src, "-lX11", "-o", os.path.join(BUILD, "docex_" + name)])
        record("docex:" + name, r.returncode == 0,
               "" if r.returncode == 0 else (r.stderr or "")[-300:])
        allok = allok and r.returncode == 0
    return allok

def pick_display():
    import socket
    for n in range(91, 100):
        xvfb = subprocess.Popen(["Xvfb", ":%d" % n, "-screen", "0", "1024x768x24"],
                                stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        time.sleep(2)
        if xvfb.poll() is not None:
            continue
        try:
            with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as sk:
                sk.settimeout(1)
                sk.connect("/tmp/.X11-unix/X%d" % n)
            return ":%d" % n, xvfb
        except OSError:
            xvfb.kill()
    raise RuntimeError("no free X display")

def main():
    global DISPLAY
    os.makedirs(BUILD, exist_ok=True)
    os.makedirs(SHOTS, exist_ok=True)
    DISPLAY, xvfb = pick_display()
    print("using display", DISPLAY)
    try:
        binaries = {}
        for n in NAMES:
            ok, info = build_demo(n)
            record("build:" + n, ok, "" if ok else info)
            if ok: binaries[n] = info
        # in-process tests
        test_bin = os.path.join(BUILD, "test_inst_de")
        r = run(["g++", "-std=c++17", "-Wall", "-Wextra", "-Werror",
                 os.path.join(WIDGETS, "tools", "test_inst_de.cpp"),
                 "-I", REPO, "-lX11", "-o", test_bin])
        if r.returncode != 0:
            record("testprog:build", False, (r.stderr or "")[-500:])
        else:
            record("testprog:build", True)
            r = run([test_bin], env={"DISPLAY": DISPLAY}, timeout=120)
            print(r.stdout)
            record("testprog:run", r.returncode == 0,
                   "rc=%d" % r.returncode if r.returncode else "")
        # demos + screenshots
        for n in NAMES:
            if n in binaries:
                ok, note = screenshot(n, binaries[n])
                record("demo:" + n, ok, note)
        verify_doc_examples()
    finally:
        xvfb.terminate()
        try: xvfb.wait(timeout=5)
        except Exception: xvfb.kill()
    nfail = sum(1 for ok, _ in results.values() if not ok)
    print("TOTAL: %d checks, %d passed, %d failed" % (len(results), len(results) - nfail, nfail))
    return 1 if nfail else 0

if __name__ == "__main__":
    sys.exit(main())
