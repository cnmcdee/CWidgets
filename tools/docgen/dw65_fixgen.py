Q = chr(34)
BS = chr(92)
NL = chr(10)
path = "/tmp/dw65_gen_examples.py"
text = open(path).read()
head, sep, tail = text.partition("TMPL = ")
assert sep, "TMPL marker not found"
res = []
in_str = False
esc = False
changed = 0
for ch in head:
    if in_str:
        if esc:
            esc = False
            res.append(ch)
        elif ch == BS:
            esc = True
            res.append(ch)
        elif ch == Q:
            in_str = False
            res.append(ch)
        elif ch == NL:
            res.append(BS)
            res.append(BS)
            res.append("n")
            changed += 1
        else:
            res.append(ch)
    else:
        if ch == Q:
            in_str = True
        res.append(ch)
open(path, "w").write("".join(res) + sep + tail)
print("newline escapes fixed in generator:", changed)
