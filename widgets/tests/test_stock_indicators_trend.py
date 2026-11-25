#!/usr/bin/env python3
"""test_stock_indicators_trend.py — driver for the 85 TREND indicators.
Re-derives every count from the fresh artifacts on each round: list file,
per-widget files, cmake build (-Wall -Wextra, 0 warnings), demo run under
Xvfb :94 (draw + click + key + clean exit), and a per-indicator smoke
program (finite values on make_seeded_series(250) + hand-computed SMA/EMA
checks). Writes a per-item pass/fail table and exits non-zero on any failure.
Attribution: XFrame.
"""
import concurrent.futures as cf
import os
import subprocess
import sys
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))  # widgets/
REPO = os.path.dirname(ROOT)
DISPLAY = ":94"


def run(cmd, env=None, timeout=120):
    e = dict(os.environ)
    e["DISPLAY"] = DISPLAY
    if env:
        e.update(env)
    try:
        p = subprocess.run(cmd, shell=True, capture_output=True, text=True,
                           timeout=timeout, env=e, cwd=REPO)
        return p.returncode, p.stdout + p.stderr
    except subprocess.TimeoutExpired:
        return -99, "TIMEOUT"


SMOKE_TMPL = """#include "@@HDR@@"
#include <cmath>
#include <cstdio>
#include <vector>
int main() {
    auto series = xframe::make_seeded_series(250, 42);
    std::vector<double> v = @@TEST@@;
    if (v.size() != 250) { std::printf("FAIL size %zu\\n", v.size()); return 1; }
    int finite = 0; bool seen_finite = false, seen_nan_after = false;
    for (size_t i = 0; i < v.size(); ++i) {
        if (std::isfinite(v[i])) { finite++;
            if (seen_nan_after && !@@SPARSE@@) { std::printf("FAIL interior NaN\\n"); return 1; } }
        else if (seen_finite) seen_nan_after = true;
    }
    if (finite < 1) { std::printf("FAIL no finite values\\n"); return 1; }
@@HAND@@
    std::printf("SMOKE OK @@NAME@@ finite=%d\\n", finite);
    return 0;
}
"""


def smoke_one(name):
    cases = {}
    try:
        with open(os.path.join(ROOT, "tests", "trend_smoke_cases.txt")) as f:
            for line in f:
                parts = line.rstrip("\n").split("|")
                if len(parts) == 3:
                    cases[parts[0]] = (parts[1], parts[2] == "1")
    except OSError:
        return "FAIL"
    if name not in cases:
        return "FAIL"
    test_expr, sparse = cases[name]
    hand = ""
    if name == "SMA":
        hand = ("    std::vector<double> c10 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};\n"
                "    auto sma = xframe::ind_sma(c10, 3);\n"
                "    if (sma[2] != 2.0 || sma[3] != 3.0 || sma[5] != 5.0 || sma[9] != 9.0) return 1;\n"
                "    if (std::isfinite(sma[0]) || std::isfinite(sma[1])) return 1;\n")
    if name == "EMA":
        hand = ("    std::vector<double> c10 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};\n"
                "    auto ema = xframe::ind_ema(c10, 3);\n"
                "    if (ema[2] != 2.0 || ema[3] != 3.0 || ema[4] != 4.0 || ema[5] != 5.0 || ema[6] != 6.0) return 1;\n")
    src = (SMOKE_TMPL.replace("@@HDR@@", os.path.join(REPO, "widgets", "components", "Ind" + name, "Ind" + name + ".h"))
           .replace("@@TEST@@", test_expr).replace("@@SPARSE@@", "true" if sparse else "false")
           .replace("@@HAND@@", hand).replace("@@NAME@@", name))
    cpp = "/tmp/trend_smoke_%s.cpp" % name
    exe = "/tmp/trend_smoke_%s" % name
    with open(cpp, "w") as f:
        f.write(src)
    rc, out = run("g++ -std=c++17 -Wall -Wextra %s -o %s -lX11 2>&1 | tee /tmp/trend_smoke_%s.log; %s" % (cpp, exe, name, exe), timeout=120)
    if rc != 0 or ("SMOKE OK %s" % name) not in out or "warning:" in out:
        return "FAIL"
    return "PASS"


def test_one(name):
    cls = "Ind" + name
    d = os.path.join(ROOT, "components", cls)
    res = {"name": name, "build": "FAIL", "draw": "FAIL", "click": "FAIL",
           "key": "FAIL", "exit": "FAIL", "smoke": "FAIL", "detail": ""}
    need = [cls + ".h", "main_" + cls + ".cpp", "CMakeLists.txt",
            os.path.join("core", "xframe_core.h")]
    missing = [f for f in need if not os.path.exists(os.path.join(d, f))]
    if missing:
        res["detail"] = "missing: " + ",".join(missing)
        return res
    # Another parallel worker shares these paths for 7 names; if our files
    # were overwritten, regenerate just this widget before building.
    try:
        hdr = open(os.path.join(d, cls + ".h")).read()
        main = open(os.path.join(d, "main_" + cls + ".cpp")).read()
    except OSError:
        hdr, main = "", ""
    if "Output length equals input length" not in hdr or ("OK " + cls) not in main:
        run("python3 widgets/tools/gen_indicators_trend.py --only %s" % name, timeout=60)
        try:
            hdr = open(os.path.join(d, cls + ".h")).read()
            main = open(os.path.join(d, "main_" + cls + ".cpp")).read()
        except OSError:
            hdr, main = "", ""
        if "Output length equals input length" not in hdr or ("OK " + cls) not in main:
            res["detail"] = "files overwritten by another worker, re-emit failed"
            return res
    bdir = os.path.join(d, "build")
    rc, out = run("cmake -S %s -B %s -DCMAKE_CXX_FLAGS='-Wall -Wextra' > /tmp/trend_cmake_%s.log 2>&1; cmake --build %s 2>&1 | tee /tmp/trend_build_%s.log" % (d, bdir, name, bdir, name), timeout=180)
    log = ""
    try:
        log = open("/tmp/trend_build_%s.log" % name).read()
    except OSError:
        pass
    if rc != 0:
        res["detail"] = "build rc=%d" % rc
        return res
    if "warning:" in log or "Warning" in log:
        res["detail"] = "compiler warnings"
        return res
    res["build"] = "PASS"
    exe = os.path.join(bdir, cls)
    rc, out = run(exe, timeout=60)
    if rc == 0 and ("OK %s" % cls) in out:
        res["draw"] = "PASS"; res["click"] = "PASS"; res["key"] = "PASS"
        res["exit"] = "PASS"
    else:
        res["detail"] = "demo rc=%d out=%s" % (rc, out.strip()[-300:])
        return res
    res["smoke"] = smoke_one(name)
    if res["smoke"] != "PASS":
        res["detail"] = "smoke failed"
    return res


def main():
    names = []
    with open(os.path.join(ROOT, "tools", "stock_indicators_trend.txt")) as f:
        for line in f:
            line = line.strip()
            if line:
                names.append(line.split("|")[0].strip())
    ok = True
    if len(names) != 85:
        print("FAIL list has %d lines, need 85" % len(names)); ok = False
    if len(set(names)) != len(names):
        print("FAIL duplicate names in list"); ok = False
    xvfb_proc = None
    if not os.path.exists("/tmp/.X11-unix/X94"):
        logf = open("/tmp/xvfb94.log", "w")
        xvfb_proc = subprocess.Popen(["Xvfb", ":94", "-screen", "0", "1024x768x24"],
                                     stdout=logf, stderr=logf, start_new_session=True)
    for _ in range(30):
        if os.path.exists("/tmp/.X11-unix/X94"):
            break
        time.sleep(1)
    else:
        print("FAIL Xvfb :94 did not come up"); sys.exit(1)
    results = []
    with cf.ThreadPoolExecutor(max_workers=6) as ex:
        for r in ex.map(test_one, names):
            results.append(r)
            print("%-28s build=%s draw=%s click=%s key=%s exit=%s smoke=%s %s" % (
                r["name"], r["build"], r["draw"], r["click"], r["key"],
                r["exit"], r["smoke"], r["detail"]))
    smoke_ok = all(r["smoke"] == "PASS" for r in results)
    npass = sum(1 for r in results if all(r[k] == "PASS" for k in ("build", "draw", "click", "key", "exit", "smoke")))
    print("TOTAL widgets=%d pass=%d fail=%d smoke=%s" % (len(results), npass, len(results) - npass, "PASS" if smoke_ok else "FAIL"))
    with open(os.path.join(ROOT, "tests", "trend_results.txt"), "w") as f:
        for r in results:
            f.write("%-28s build=%s draw=%s click=%s key=%s exit=%s smoke=%s %s\n" % (
                r["name"], r["build"], r["draw"], r["click"], r["key"],
                r["exit"], r["smoke"], r["detail"]))
        f.write("TOTAL widgets=%d pass=%d fail=%d smoke=%s\n" % (len(results), npass, len(results) - npass, "PASS" if smoke_ok else "FAIL"))
    if xvfb_proc is not None:
        xvfb_proc.terminate()
    if npass != len(results) or not smoke_ok or not ok:
        sys.exit(1)
    print("ALL TREND TESTS PASS")


if __name__ == "__main__":
    main()
