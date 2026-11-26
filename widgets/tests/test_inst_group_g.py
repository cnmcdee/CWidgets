#!/usr/bin/env python3
"""Instrumentation Group G driver: builds the 12 segment displays, runs the
headless harness (logic + pixel + repertoire checks), runs each demo under
Xvfb :148 (draw / click / key / clean exit), and verifies every code example
in docs/instrumentation-segments.md compiles. Re-derives every count from
fresh artifacts each round; exits non-zero on any failure.
Attribution: XFrame."""
import os, re, subprocess, sys, time
from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
COMP = os.path.join(ROOT, "components")
BUILD = os.path.join(ROOT, "build", "components")
TMP = "/tmp/inst-group-g"
ENV = dict(os.environ, DISPLAY=":148")
WIDGETS = ["InstSeg14Display1", "InstSeg14Display2", "InstSeg14Display3", "InstSeg14Display4", "InstSeg14Display5", "InstSeg14Display6", "InstSeg16Display1", "InstSeg16Display2", "InstSeg16Display3", "InstSeg16Display4", "InstSeg16Display5", "InstSeg16Display6"]
os.makedirs(TMP, exist_ok=True)
failures = []
rows = []

def run(*a, timeout=30, **k):
    return subprocess.run(*a, capture_output=True, text=True, timeout=timeout, env=ENV, **k)

def nonbg(path):
    im = Image.open(path).convert("L")
    h = im.histogram()
    modal = max(range(256), key=lambda v: h[v])
    return sum(c for v, c in enumerate(h) if abs(v - modal) > 10)

# --- harness: compile + run ---
hsrc = os.path.join(ROOT, "tests", "test_inst_group_g.cpp")
hbin = os.path.join(TMP, "test_inst_group_g")
r = subprocess.run(["g++", "-std=c++17", "-Wall", "-Wextra", "-I", COMP, hsrc,
                    "-o", hbin, "-lX11"], capture_output=True, text=True, timeout=120)
if r.returncode != 0 or "warning:" in r.stderr:
    failures.append(f"harness compile failed/warnings: {r.stderr[:600]}")
    print("HARNESS COMPILE FAILED")
    print(r.stderr[:2000])
else:
    print("harness compiled, 0 warnings")

xvfb = None
if not os.path.exists("/tmp/.X148-lock"):
    xvfb = subprocess.Popen(["Xvfb", ":148", "-screen", "0", "1024x768x24"])
    time.sleep(1.5)

harness_out = ""
try:
    if os.path.exists(hbin):
        r = run([hbin], timeout=120)
        harness_out = r.stdout + r.stderr
        print(harness_out)
        if r.returncode != 0 or "ALL PASS" not in harness_out:
            failures.append("harness checks failed")

    sd = "/tmp/send_delete"
    if not os.path.exists(sd):
        subprocess.run(["cc", os.path.join(ROOT, "test-driver", "send_delete.c"),
                        "-lX11", "-o", sd], capture_output=True, timeout=60)

    for name in WIDGETS:
        row = {"name": name, "build": "FAIL", "draw": "FAIL", "click": "FAIL",
               "key": "FAIL", "exit": "FAIL"}
        bdir = os.path.join(BUILD, name)
        r = subprocess.run(["cmake", "-S", os.path.join(COMP, name), "-B", bdir,
                            "-DCMAKE_CXX_FLAGS=-Wall -Wextra"],
                           capture_output=True, text=True, timeout=120)
        out = r.stdout + r.stderr
        r2 = subprocess.run(["cmake", "--build", bdir], capture_output=True,
                            text=True, timeout=180) if r.returncode == 0 else None
        out2 = (r2.stdout + r2.stderr) if r2 else ""
        binp = os.path.join(bdir, name)
        if r.returncode != 0 or (r2 and r2.returncode != 0):
            failures.append(f"{name}: build failed: {(out + out2)[-400:]}")
            rows.append(row)
            continue
        if "warning:" in out2:
            failures.append(f"{name}: warnings in build")
        if not (os.path.exists(binp) and os.access(binp, os.X_OK)):
            failures.append(f"{name}: binary missing")
            rows.append(row)
            continue
        row["build"] = "PASS"
        logf = open(os.path.join(TMP, name + ".stderr"), "wb")
        p = subprocess.Popen([binp], env=ENV, stdout=subprocess.DEVNULL, stderr=logf)
        wid = None
        try:
            for _ in range(50):
                if p.poll() is not None:
                    break
                rr = run(["xdotool", "search", "--name", f"{name} - XFrame demo"], timeout=10)
                if rr.stdout.strip():
                    wid = rr.stdout.strip().split()[0]
                    break
                time.sleep(0.2)
            if wid is None:
                failures.append(f"{name}: window not found")
                rows.append(row)
                continue
            time.sleep(0.5)
            shot = os.path.join(TMP, name + ".png")
            if run(["import", "-window", str(wid), shot], timeout=15).returncode == 0 \
                    and os.path.exists(shot) and nonbg(shot) > 200:
                row["draw"] = "PASS"
            else:
                failures.append(f"{name}: blank draw screenshot")
            run(["xdotool", "mousemove", "--window", str(wid), "60", "60",
                 "click", "--window", str(wid), "1"], timeout=10)
            time.sleep(0.3)
            if p.poll() is None:
                row["click"] = "PASS"
            else:
                failures.append(f"{name}: died on click")
            run(["xdotool", "key", "--window", str(wid), "Up"], timeout=10)
            time.sleep(0.3)
            if p.poll() is None:
                row["key"] = "PASS"
            else:
                failures.append(f"{name}: died on key")
            run([sd, str(wid)], timeout=10)
            try:
                rc = p.wait(timeout=10)
                if rc == 0:
                    row["exit"] = "PASS"
                else:
                    failures.append(f"{name}: exit rc={rc}")
            except subprocess.TimeoutExpired:
                failures.append(f"{name}: no clean exit")
                p.kill()
        finally:
            if p.poll() is None:
                p.kill()
            logf.close()
        rows.append(row)

    # --- doc examples: extract first cpp block after each ## heading, wrap, compile ---
    doc = open(os.path.join(os.path.dirname(ROOT), "docs", "instrumentation-segments.md")).read()
    heads = list(re.finditer(r"^## (\w+)\n", doc, re.M))
    ex_ok = 0
    for m in heads:
        nm = m.group(1)
        seg = doc[m.end():]
        bm = re.search(r"```cpp\n(.*?)```", seg, re.S)
        if not bm:
            failures.append(f"doc: no cpp example for {nm}")
            continue
        snippet = bm.group(1)
        prog = ('#include "%s/%s.h"\n' % (nm, nm) +
                "int main() {\n"
                " Display* display = XOpenDisplay(nullptr);\n"
                " if (!display) return 1;\n"
                " int screen = DefaultScreen(display);\n"
                " Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, 800, 600, 1, 0, 0);\n"
                " GC gc = DefaultGC(display, screen);\n"
                " SStyle_Manager styles(display, window, screen, gc);\n"
                ' styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");\n'
                ' styles.set_active_style_byName("default");\n'
                + snippet +
                " XCloseDisplay(display);\n return 0;\n}\n")
        src = os.path.join(TMP, "ex_" + nm + ".cpp")
        open(src, "w").write(prog)
        r = subprocess.run(["g++", "-std=c++17", "-Wall", "-Wextra", "-I", COMP, src,
                            "-o", os.path.join(TMP, "ex_" + nm), "-lX11"],
                           capture_output=True, text=True, timeout=60)
        if r.returncode == 0 and "warning:" not in r.stderr:
            ex_ok += 1
        else:
            failures.append(f"doc example {nm} failed to compile: {r.stderr[:400]}")
    print(f"doc examples compiled: {ex_ok}/{len(heads)}")
    if ex_ok != len(heads):
        failures.append("doc examples incomplete")
finally:
    if xvfb is not None:
        xvfb.terminate()

print("\n%-22s %-6s %-6s %-6s %-6s %-6s" % ("widget", "build", "draw", "click", "key", "exit"))
for row in rows:
    print("%-22s %-6s %-6s %-6s %-6s %-6s" % (row["name"], row["build"], row["draw"],
                                              row["click"], row["key"], row["exit"]))
npass = sum(1 for row in rows if all(v == "PASS" for k, v in row.items() if k != "name"))
print(f"\nwidgets fully PASS: {npass}/{len(WIDGETS)}")
print("harness: " + ("ALL PASS" if "ALL PASS" in harness_out else "FAILED"))
if failures:
    print("FAILURES:")
    for f in failures:
        print(" - " + f)
    sys.exit(1)
print("ALL PASS")
