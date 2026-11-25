#!/usr/bin/env python3
"""Test driver for the 85 momentum indicators. Re-derives every count from
fresh artifacts on each round. Exits non-zero on any failure.
Attribution: XFrame.
"""
import concurrent.futures
import hashlib
import os
import subprocess
import sys

REPO = "/opt/cwidgets"
WIDGETS = os.path.join(REPO, "widgets")
COMP = os.path.join(WIDGETS, "components")
TOOLS = os.path.join(WIDGETS, "tools")
TESTS = os.path.join(WIDGETS, "tests")
BUILD = "/tmp/momentum_build"
os.makedirs(BUILD, exist_ok=True)

failures = []
results = []  # (name, build, draw, click, key, exit, smoke)

def run(cmd, timeout=120, env=None):
    e = dict(os.environ)
    e.update(env or {})
    try:
        p = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout, env=e)
        return p.returncode, p.stdout + p.stderr
    except subprocess.TimeoutExpired:
        return -99, "TIMEOUT"

def main():
    # 1. regenerate from the spec (source of truth)
    rc, out = run(["python3", os.path.join(TOOLS, "gen_indicators_momentum.py")], timeout=300)
    print(out.strip())
    blocked = []
    for ln in out.splitlines():
        if ln.startswith("BLOCKED "):
            blocked = [b.strip() for b in ln.split(":", 1)[1].split(",") if b.strip()]
    if rc not in (0, 2):
        print("GENERATOR FAILED")
        return 1
    if blocked:
        print("generator skipped blocked dirs (another group's versions): %s" % blocked)
    # 2. list file: exactly 85 lines, unique names
    with open(os.path.join(TOOLS, "stock_indicators_momentum.txt")) as f:
        lines = [ln.rstrip("\n") for ln in f if ln.strip()]
    names = []
    for ln in lines:
        parts = ln.split(" | ")
        if len(parts) != 3 or parts[1] != "momentum":
            failures.append("list line malformed: %r" % ln)
        else:
            names.append(parts[0])
    print("list lines=%d unique=%d" % (len(lines), len(set(names))))
    if len(lines) != 85 or len(set(names)) != 85:
        failures.append("list count != 85")
    # 3. per-widget dirs: exactly 4 files, core md5 matches
    with open(os.path.join(REPO, "widgets", "core", "xframe_core.h"), "rb") as f:
        core_md5 = hashlib.md5(f.read()).hexdigest()
    for name in names:
        if name in blocked:
            continue  # another group's dir; never verified or built as mine
        d = os.path.join(COMP, "Ind" + name)
        got = []
        for root, _dirs, files in os.walk(d):
            for fn in files:
                rel = os.path.relpath(os.path.join(root, fn), d)
                if rel.startswith("build" + os.sep) or rel == "build":
                    continue  # cmake build artifacts from another sweep, not source
                got.append(rel)
        expect = {"Ind%s.h" % name, "main_Ind%s.cpp" % name, "CMakeLists.txt",
                  os.path.join("core", "xframe_core.h")}
        if set(got) != expect:
            failures.append("Ind%s files mismatch: %s" % (name, sorted(got)))
            continue
        with open(os.path.join(d, "core", "xframe_core.h"), "rb") as f:
            if hashlib.md5(f.read()).hexdigest() != core_md5:
                failures.append("Ind%s core md5 mismatch" % name)

    def build_and_run(name):
        d = os.path.join(COMP, "Ind" + name)
        exe = os.path.join(BUILD, "Ind" + name)
        rc, out = run(["g++", "-std=c++17", "-Wall", "-Wextra", "-Werror", "-O0",
                       "-I" + d, "-I" + COMP,
                       os.path.join(d, "main_Ind%s.cpp" % name),
                       "-lX11", "-o", exe], timeout=180)
        if rc != 0:
            return (name, "FAIL", "-", "-", "-", "FAIL", out[-2000:])
        env = {"DISPLAY": ":98"}
        rc, out = run([exe], timeout=30, env=env)
        ok = rc == 0
        return (name, "PASS", "PASS" if ok else "FAIL", "PASS" if ok else "FAIL",
                "PASS" if ok else "FAIL", "PASS" if ok else "FAIL", "" if ok else out[-1000:])

    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as ex:
        per = list(ex.map(build_and_run, [n for n in names if n not in blocked]))
    for name, build, draw, click, key, exit_, extra in per:
        smoke = "PASS" if build == "PASS" and exit_ == "PASS" else "FAIL"
        results.append((name, build, draw, click, key, exit_, smoke))
        if build != "PASS" or exit_ != "PASS":
            failures.append("Ind%s build/run failed: %s" % (name, extra))

    # 4. smoke harness: all 85 compute functions on seeded data + hand checks
    smoke_src = os.path.join(TESTS, "smoke_momentum_indicators.cpp")
    smoke_exe = os.path.join(BUILD, "smoke_momentum")
    rc, out = run(["g++", "-std=c++17", "-Wall", "-Wextra", "-Werror", "-O0",
                   "-I" + COMP, smoke_src, "-lX11", "-o", smoke_exe], timeout=600)
    smoke_pass = 0
    smoke_total = 0
    if rc != 0:
        failures.append("smoke compile failed: " + out[-3000:])
    else:
        rc, out = run([smoke_exe], timeout=120)
        for ln in out.splitlines():
            if ln.startswith("PASS ") or ln.startswith("FAIL "):
                smoke_total += 1
                if ln.startswith("PASS "):
                    smoke_pass += 1
                else:
                    failures.append("smoke: " + ln)
        if "HANDCHECK RSI" not in out or "HANDCHECK MACD-CONSTANT" not in out or "HANDCHECK ROC" not in out:
            failures.append("smoke hand checks missing")
        if rc != 0:
            failures.append("smoke exit %d" % rc)
        print(out.strip().splitlines()[-1] if out.strip() else "smoke no output")
    print("smoke pass=%d/%d" % (smoke_pass, smoke_total))

    # 5. results table
    for name in blocked:
        results.append((name, "BLOCKED", "BLOCKED", "BLOCKED", "BLOCKED", "BLOCKED", "BLOCKED"))
        failures.append("BLOCKED dir (another group's version, not overwritten): " + name)
    with open(os.path.join(TESTS, "momentum_results.txt"), "w") as f:
        f.write("indicator | build | draw | click | key | exit | smoke\n")
        for row in results:
            f.write(" | ".join(row) + "\n")
        f.write("smoke %d/%d\n" % (smoke_pass, smoke_total))
    nbuild = sum(1 for r in results if r[1] == "PASS")
    nrun = sum(1 for r in results if r[5] == "PASS")
    print("widgets=%d build_pass=%d run_pass=%d smoke=%d/%d failures=%d"
          % (len(results), nbuild, nrun, smoke_pass, smoke_total, len(failures)))
    for fl in failures[:20]:
        print("FAILURE: " + fl)
    return 1 if failures else 0

if __name__ == "__main__":
    sys.exit(main())
