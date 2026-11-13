#!/usr/bin/env python3
"""Lists every string in the game's code modules as a table: where it is, what it is called, who uses it.

The table is made from your own ROM's extracted files and written under build/, which git ignores: it is game data,
and is not committed. Each row is one string in a data section of the main program or of an overlay:

    address  module  section  encoding  symbol  unit  users  text

  encoding  ascii, sjis (Shift JIS, at least one two-byte character) or utf16 (the game's text for the screen)
  symbol    its name in symbols.txt, if it has one there
  unit      the source file that owns it in delinks.txt, if one does
  users     how many places in relocs.txt refer to it

A string is taken to be one when something in relocs.txt refers to its address, it ends in a zero, and all of it
is text: ASCII, kana, kanji, full-width forms and the signs that go with them. Unreferenced data that happens to
read as text is left out, and so is a string that is only reached through an offset from another.
"""
import argparse
import bisect
import csv
import re
import sys
from pathlib import Path

ROOT = Path(__file__).parent.parent.resolve()
SECTION = re.compile(r"^\s+(\.\w+)\s+start:0x([0-9a-f]+) end:0x([0-9a-f]+)")
SYMBOL = re.compile(r"^(\S+) kind:\w+.* addr:0x([0-9a-f]+)")
RELOC = re.compile(r"to:0x([0-9a-f]+)(?: add:0x([0-9a-f]+))?")
DATA_SECTIONS = (".rodata", ".data")


def is_text(text: str) -> bool:
    """Whether every character is one that text is written with: ASCII, kana, kanji, full-width and CJK signs."""
    for c in text:
        code = ord(c)
        if c in "\n\r\t" or 0x20 <= code < 0x7F:
            continue
        if 0x3000 <= code <= 0x30FF or 0x4E00 <= code <= 0x9FFF or 0xFF00 <= code <= 0xFFEF:
            continue
        if 0x2010 <= code <= 0x203B or 0x2190 <= code <= 0x21FF or 0x2460 <= code <= 0x26FF or code in (0xD7, 0xF7):
            continue
        return False
    return True


def read_ascii_or_sjis(data: bytes, start: int) -> tuple[str, str, int] | None:
    end = data.find(b"\0", start)
    if end <= start:
        return None
    raw = data[start:end]
    try:
        text = raw.decode("cp932")
    except UnicodeDecodeError:
        return None
    if not is_text(text):
        return None
    return ("ascii" if all(b < 0x80 for b in raw) else "sjis"), text, end - start + 1


def read_utf16(data: bytes, start: int) -> tuple[str, str, int] | None:
    end = start
    while end + 1 < len(data) and data[end:end + 2] != b"\0\0":
        end += 2
    raw = data[start:end]
    if len(raw) < 2:
        return None
    try:
        text = raw.decode("utf-16-le")
    except UnicodeDecodeError:
        return None
    # Text for the screen has kana, kanji or full-width forms in it. Plain ASCII in 16 bits is rarely a string.
    if not is_text(text) or not any(ord(c) >= 0x3000 for c in text):
        return None
    return "utf16", text, end - start + 2


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="jpn_rev1")
    parser.add_argument("--min", type=int, default=1, help="shortest text to count as a string")
    parser.add_argument("-o", "--output", type=Path)
    args = parser.parse_args()

    config = ROOT / "config" / args.version / "arm9"
    extract = ROOT / "extract" / args.version
    output = args.output or ROOT / "build" / args.version / "strings.tsv"
    rows = []
    for delinks in sorted(config.rglob("delinks.txt")):
        directory = delinks.parent
        module = "main" if directory == config else directory.name
        if module == "main":
            binary = extract / "arm9" / "arm9.bin"
        elif module.startswith("ov"):
            binary = extract / "arm9_overlays" / f"{module}.bin"
        else:
            continue
        data = binary.read_bytes()

        # The module's sections come first in delinks.txt, then each file with the ranges it owns
        sections, owned, unit = {}, [], None
        for line in delinks.read_text(encoding="utf-8").splitlines():
            if line and not line[0].isspace() and line.rstrip().endswith(":"):
                unit = line.rstrip()[:-1]
                continue
            match = SECTION.match(line)
            if match and unit is None:
                sections[match[1]] = (int(match[2], 16), int(match[3], 16))
            elif match:
                owned.append((int(match[2], 16), int(match[3], 16), unit))
        base = min(start for start, _ in sections.values())

        symbols = {}
        for line in (directory / "symbols.txt").read_text(encoding="utf-8").splitlines():
            match = SYMBOL.match(line)
            if match:
                symbols[int(match[2], 16)] = match[1]
        users = {}
        for line in (directory / "relocs.txt").read_text(encoding="utf-8").splitlines():
            match = RELOC.search(line)
            if match:
                target = int(match[1], 16) + int(match[2] or "0", 16)
                users[target] = users.get(target, 0) + 1
        starts = sorted(symbols)

        for section in DATA_SECTIONS:
            if section not in sections:
                continue
            address, end = sections[section]
            while address < end:
                at_symbol = address in symbols
                if address % 4 and not at_symbol:
                    address += 1
                    continue
                offset = address - base
                # Plain ASCII first: read as 16-bit text, a run of ASCII letters looks like kanji
                found = read_ascii_or_sjis(data, offset)
                if found is None or found[0] != "ascii":
                    found = (read_utf16(data, offset) if address % 2 == 0 else None) or found
                # Something refers to a string, or it would not be there: that is what tells text from a table
                # of numbers that happens to read as text
                if found and len(found[1]) >= args.min and address in users:
                    encoding, text, size = found
                    # Not past the next symbol: that would be two things read as one
                    following = bisect.bisect_right(starts, address)
                    if following < len(starts) and address + size > starts[following] + 3:
                        address += 4 - address % 4
                        continue
                    owner = next((name for low, high, name in owned if low <= address < high), "")
                    rows.append((f"{address:#010x}", module, section, encoding, symbols.get(address, ""), owner,
                                 users.get(address, 0), text.replace("\\", "\\\\").replace("\n", "\\n")
                                 .replace("\r", "\\r").replace("\t", "\\t")))
                    address += size
                    address += -address % 4
                else:
                    address += 4 - address % 4

    output.parent.mkdir(parents=True, exist_ok=True)
    with output.open("w", encoding="utf-8", newline="") as file:
        writer = csv.writer(file, delimiter="\t", lineterminator="\n", quoting=csv.QUOTE_NONE, escapechar=None,
                            quotechar=None)
        writer.writerow(("address", "module", "section", "encoding", "symbol", "unit", "users", "text"))
        for row in rows:
            writer.writerow(tuple(str(field).replace("\t", " ") for field in row))
    by_encoding = {}
    for row in rows:
        by_encoding[row[3]] = by_encoding.get(row[3], 0) + 1
    print(f"{output.relative_to(ROOT)}: {len(rows)} strings ("
          + ", ".join(f"{count} {name}" for name, count in sorted(by_encoding.items())) + ")")
    return 0


if __name__ == "__main__":
    sys.exit(main())
