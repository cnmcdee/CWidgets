#!/usr/bin/env python3
"""Add per-widget code examples for the 17 stock widgets whose doc entries
only had a shared/prose example: the 8 OHLC widgets and the 9
compare-volume widgets with prose-only examples.

Writes into:
  - docs/stock-market-compare-volume.md (replace prose Example with a block)
  - docs/stock-market-ohlc.md (append a 'Per-widget examples' section)
  - docs/stock-market.md (same change, per widget section)
Idempotent: a widget section that already contains a cpp block is skipped.
Every snippet below was checked against its widget header; run
verify_doc_examples.py --only <names> afterwards to confirm compilation.
"""
import os, re, sys

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

CTOR = "%s w(display, screen, window, gc, &styles, 20, 20, 720, 480"
SERIES60 = 'xframe::make_seeded_series(60, 42)'
SERIES60B = 'xframe::make_seeded_series(60, 7)'

def ohlc(name):
    return ["%s, xframe::make_seeded_series(250));" % (CTOR % name),
            "w.draw();"]

def multi(name, extra=None):
    lines = ["%s);" % (CTOR % name),
             'w.add_series("SYM-A", %s);' % SERIES60,
             'w.add_series("SYM-B", %s);' % SERIES60B]
    if extra:
        lines.append(extra)
    lines.append("w.draw();")
    return lines

def single(name, extra=None):
    lines = ["%s);" % (CTOR % name),
             "w.set_series(%s);" % SERIES60]
    if extra:
        lines.append(extra)
    lines.append("w.draw();")
    return lines

SNIPPETS = {}
for _n in ["StockCandlestickChart", "StockOHLCBarChart", "StockLineChart",
           "StockAreaChart", "StockHeikinAshi", "StockRenko",
           "StockHollowCandles", "StockBaselineChart"]:
    SNIPPETS[_n] = ohlc(_n)
SNIPPETS["StockMultiSymbolChart"] = multi("StockMultiSymbolChart")
SNIPPETS["StockSpreadChart"] = multi("StockSpreadChart",
                                     "double s = w.spread_at(10);")
SNIPPETS["StockRatioChart"] = multi("StockRatioChart",
                                    "double r = w.ratio_at(10);")
SNIPPETS["StockIndexedComparison"] = multi("StockIndexedComparison",
                                          "w.set_base_value(1000.0);")
SNIPPETS["StockVWAP"] = single("StockVWAP", "double v = w.vwap_at(59);")
SNIPPETS["StockOBVChart"] = single("StockOBVChart", "double o = w.obv_at(59);")
SNIPPETS["StockVolumeCandles"] = single("StockVolumeCandles",
                                       "double f = w.body_width_at(3);")
SNIPPETS["StockAccumulationDistribution"] = single(
    "StockAccumulationDistribution", "double a = w.ad_at(59);")
SNIPPETS["StockMoneyFlowChart"] = single(
    "StockMoneyFlowChart", "double m = w.total_money_flow();")

FENCE = chr(96) * 3

def block(lines, indent):
    pad = " " * indent
    out = [pad + FENCE + "cpp"]
    out += [pad + ln for ln in lines]
    out.append(pad + FENCE)
    return out

def section_bounds(lines, name):
    """(start, end) line indexes of the widget's section in a doc, or None."""
    pat = re.compile(r"^#{2,3}\s+.*\b%s\b" % re.escape(name))
    start = None
    for i, ln in enumerate(lines):
        if pat.match(ln):
            start = i
            break
    if start is None:
        return None
    end = len(lines)
    for j in range(start + 1, len(lines)):
        if re.match(r"^#{1,3}\s+", lines[j]):
            end = j
            break
    return start, end

def section_has_code(lines, start, end):
    return any(lines[i].strip() == FENCE + "cpp"
               for i in range(start, end))

def replace_prose_example(lines, start, end, name, indent):
    """Replace a prose '- **Example:** ...' (no fence) with a code block.
    Returns new lines, or None if no prose example found."""
    for i in range(start, end):
        if lines[i].strip().startswith("- **Example:**"):
            # if a fence already follows, nothing to do
            j = i + 1
            while j < end and not lines[j].strip():
                j += 1
            if j < end and lines[j].strip() == FENCE + "cpp":
                return None
            # prose may wrap over 2 lines; drop lines until the next
            # '- **' bullet, heading, or blank line followed by a bullet
            k = i + 1
            while k < end:
                s = lines[k]
                if s.strip().startswith("- **") or s.startswith("#"):
                    break
                if not s.strip():
                    # blank line ends the prose example
                    break
                k += 1
            new = lines[:i] + ["- **Example:**"] + block(SNIPPETS[name], indent) + lines[k:]
            return new
    return None

def append_example(lines, start, end, name, indent):
    """Append an Example block at the end of the section (OHLC style)."""
    new = lines[:end] + ["", "- **Example:**"] + block(SNIPPETS[name], indent) + lines[end:]
    return new

def process(path, names, indent, mode):
    p = os.path.join(REPO, path)
    lines = open(p).read().splitlines()
    changed = []
    for name in names:
        b = section_bounds(lines, name)
        if b is None:
            print("  SKIP %s: no section in %s" % (name, path))
            continue
        start, end = b
        if section_has_code(lines, start, end):
            continue
        if mode == "replace":
            new = replace_prose_example(lines, start, end, name, indent)
            if new is None:
                new = append_example(lines, start, end, name, indent)
        else:
            new = append_example(lines, start, end, name, indent)
        lines = new
        changed.append(name)
    open(p, "w").write("\n".join(lines) + "\n")
    print("%s: added examples for %d: %s" % (path, len(changed), ", ".join(changed)))

def append_ohlc_fragment():
    """Append per-widget ### sections to the OHLC fragment doc."""
    p = os.path.join(REPO, "docs/stock-market-ohlc.md")
    txt = open(p).read()
    names = [n for n in SNIPPETS if n.startswith("Stock") and
             n not in ("StockMultiSymbolChart", "StockSpreadChart",
                       "StockRatioChart", "StockIndexedComparison",
                       "StockVWAP", "StockOBVChart", "StockVolumeCandles",
                       "StockAccumulationDistribution", "StockMoneyFlowChart")]
    if "## Per-widget examples" in txt:
        print("docs/stock-market-ohlc.md: Per-widget examples already present")
        return
    parts = [txt.rstrip("\n"), "", "## Per-widget examples", "",
             "One working example per widget (same shape as the shared example",
             "above, with the widget's own class name).", ""]
    for name in names:
        parts.append("### %s" % name)
        parts.append("")
        parts.append("- **Example:**")
        parts += block(SNIPPETS[name], 0)
        parts.append("")
    open(p, "w").write("\n".join(parts))
    print("docs/stock-market-ohlc.md: added Per-widget examples for %d" % len(names))

def main():
    cv = ["StockMultiSymbolChart", "StockSpreadChart", "StockRatioChart",
          "StockIndexedComparison", "StockVWAP", "StockOBVChart",
          "StockVolumeCandles", "StockAccumulationDistribution",
          "StockMoneyFlowChart"]
    ohlc_names = ["StockCandlestickChart", "StockOHLCBarChart",
                  "StockLineChart", "StockAreaChart", "StockHeikinAshi",
                  "StockRenko", "StockHollowCandles", "StockBaselineChart"]
    process("docs/stock-market-compare-volume.md", cv, 2, "replace")
    append_ohlc_fragment()
    process("docs/stock-market.md", cv + ohlc_names, 2, "replace")

if __name__ == "__main__":
    main()
