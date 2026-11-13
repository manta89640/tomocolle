#!/usr/bin/env python3
"""Gives every string of the table from tools/strings.py a name in symbols.txt, made from its text.

A string's text is the best evidence there is of what it is, so the name is the text: "M00:/sky.imd" becomes
gStr_M00_sky_imd. Japanese text is named by how it is read: ワタシ becomes gText_Watashi, and 今日はいい天気 becomes
gText_KyouHaIiTenki. The readings of kanji come from the system's Japanese tokenizer (tools/romaji.swift, so this
runs on macOS), and a sign that has no reading is named by what it is ("！" is Exclamation). No name is an address.
Where equal strings are in several places, an overlay's has the overlay after it, and the second and later of a
kind are numbered.

Only symbols that have no name yet (data_<address>) are renamed. Strings that a source file owns are left alone,
since their names are the compiler's, and so is a symbol that the sources refer to as data_<address>. Run
tools/strings.py first.
"""
import argparse
import collections
import csv
import re
import shutil
import subprocess
import sys
import unicodedata
from pathlib import Path

ROOT = Path(__file__).parent.parent.resolve()
SYMBOL = re.compile(r"^(\S+) (kind:\w+.*) addr:0x([0-9a-f]+)(.*)$")
MAX = 40

DROP = {"FULLWIDTH", "HALFWIDTH", "IDEOGRAPHIC", "KATAKANA", "HIRAGANA", "LETTER", "DIGIT", "SIGN", "MARK", "CJK",
        "SMALL", "CAPITAL", "LATIN", "LEFT", "RIGHT", "WHITE", "BLACK", "PROLONGED", "SOUND", "AND", "WITH"}


def sign_name(c: str) -> str:
    """A word for a character that has no reading: what Unicode calls it, without the words that say its width."""
    words = [w for w in unicodedata.name(c, "Sign").replace("-", " ").split() if w not in DROP]
    return "".join(w.capitalize() for w in words) or "Sign"


def readings(texts: list[str]) -> list[str]:
    """Returns how each Japanese text is read, in Latin letters, a word at a time."""
    if not texts:
        return []
    if shutil.which("swift") is None:
        sys.exit("Naming Japanese text needs the system's tokenizer, through swift (macOS)")
    lines = "\n".join(re.sub(r"\s+", " ", unicodedata.normalize("NFKC", t)) for t in texts) + "\n"
    result = subprocess.run(["swift", str(ROOT / "tools" / "romaji.swift")], input=lines, capture_output=True,
                            text=True)
    if result.returncode != 0:
        sys.exit(result.stderr)
    out = result.stdout.split("\n")[:len(texts)]
    return out + [""] * (len(texts) - len(out))


def spell(reading: str) -> str:
    """Makes an identifier's body of a reading: its words capitalised and joined, its signs named."""
    parts = []
    for word in reading.split():
        clean = "".join(c for c in unicodedata.normalize("NFD", word) if c.isascii() and c.isalnum())
        if clean:
            parts.append(clean[0].upper() + clean[1:])
        else:
            parts.extend(sign_name(c) for c in word if not c.isspace())
    body = ""
    for part in parts:
        if body and len(body) + len(part) > MAX:
            break
        body += part
    return body[:MAX + 16]


def identifier(text: str) -> str:
    name = re.sub(r"[^A-Za-z0-9]+", "_", text).strip("_")
    return name[:MAX].rstrip("_")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="jpn_rev1")
    parser.add_argument("--apply", action="store_true", help="write the names to the symbols.txt files")
    args = parser.parse_args()

    table = ROOT / "build" / args.version / "strings.tsv"
    if not table.exists():
        print(f"{table} is not there: run tools/strings.py first", file=sys.stderr)
        return 1
    config = ROOT / "config" / args.version / "arm9"

    wanted = {}  # (module, address) -> name without a suffix
    japanese = []
    with table.open(encoding="utf-8", newline="") as file:
        for row in csv.DictReader(file, delimiter="\t", quoting=csv.QUOTE_NONE):
            if row["unit"]:
                continue
            text = row["text"].replace("\\n", "\n").replace("\\r", "\r").replace("\\t", "\t").replace("\\\\", "\\")
            key = (row["module"], int(row["address"], 16))
            if row["encoding"] == "ascii":
                body = identifier(text) or spell(" ".join(text))
                wanted[key] = f"gStr_{body}"
            else:
                japanese.append((key, text))
    unread = 0
    for (key, text), reading in zip(japanese, readings([text for _, text in japanese])):
        # Kanji that the tokenizer cannot read are not words: this is data that decodes as text, not a string
        kanji = any(0x4E00 <= ord(c) <= 0x9FFF for c in text)
        if kanji and (not reading.strip() or any(0x4E00 <= ord(c) <= 0x9FFF for c in reading)):
            unread += 1
            continue
        wanted[key] = f"gText_{spell(reading) or spell(' '.join(text))}"
    if unread:
        print(f"{unread} entries of the table are left unnamed: their kanji have no reading, so they are not text")

    # Names already in use anywhere: one name, one place
    files = {}
    taken = collections.Counter()
    for path in sorted(config.rglob("symbols.txt")):
        module = "main" if path.parent == config else path.parent.name
        files[module] = (path, path.read_text(encoding="utf-8").splitlines())
        for line in files[module][1]:
            match = SYMBOL.match(line)
            if match:
                taken[match[1]] += 1
    counts = collections.Counter(wanted.values())

    # A symbol that the sources refer to by its placeholder name keeps it: renaming it here alone would leave the
    # source pointing at nothing. Those are tables that begin with text more often than they are strings.
    in_source = set()
    for directory in ("src", "include", "lib/SHTTS"):
        for path in (ROOT / directory).rglob("*"):
            if path.suffix in (".c", ".cpp", ".h"):
                in_source.update(re.findall(r"\bdata_(?:ov\d+_)?[0-9a-f]{8}\b", path.read_text(encoding="utf-8")))

    # Equal names are told apart by the overlay they are in, and then by number in order of address
    final, used = {}, set(taken)
    for name in sorted(counts):
        keys = sorted(key for key, value in wanted.items() if value == name)
        for module, address in keys:
            candidate = f"{name}_{module}" if len(keys) > 1 and module.startswith("ov") else name
            number, unique = 1, candidate
            while unique in used:
                number += 1
                unique = f"{candidate}_{number}"
            used.add(unique)
            final[(module, address)] = unique

    renamed = added = 0
    for module, (path, lines) in files.items():
        by_address = {}
        for number, line in enumerate(lines):
            match = SYMBOL.match(line)
            if match:
                by_address[int(match[3], 16)] = (number, match)
        for (owner, address), name in sorted(final.items()):
            if owner != module:
                continue
            if address in by_address:
                number, match = by_address[address]
                if not match[1].startswith("data_") or match[1] in in_source:
                    continue
                lines[number] = f"{name} {match[2]} addr:0x{match[3]}{match[4]}"
                renamed += 1
            else:
                lines.append(f"{name} kind:data(any) addr:{address:#010x}")
                added += 1
        if args.apply:
            path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"{renamed} symbols renamed, {added} added" + ("" if args.apply else " (not written: pass --apply)"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
