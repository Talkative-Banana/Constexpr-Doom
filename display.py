import re

with open("dump.txt") as f:
    text = f.read()

cutoff = text.find("note:")
if cutoff != -1:
    text = text[:cutoff]

chars = re.findall(r"'(\\.|[^'])'", text)

def unescape(c):
    return {'\\n':'\n', '\\t':'\t', "\\'":"'", '\\\\':'\\'}.get(c, c)

chars = [unescape(c) for c in chars]

print(f"Extracted {len(chars)} characters", flush=True)

W = 320
with open("frame.txt", "w") as out:
    for i in range(0, len(chars), W):
        out.write("".join(chars[i:i+W]) + "\n")