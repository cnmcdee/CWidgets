BS = chr(92)
path = "/tmp/dw65_gen_examples.py"
text = open(path).read()
head, sep, tail = text.partition("TMPL = ")
assert sep, "TMPL marker not found"
old = BS + "n"
new = BS + BS + "n"
count = head.count(old)
head2 = head.replace(old, new)
open(path, "w").write(head2 + sep + tail)
print("backslash-n sequences doubled in generator:", count)
