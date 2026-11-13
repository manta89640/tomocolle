#!/usr/bin/env python3
"""Gives every string of the table from tools/strings.py a name in symbols.txt, made from its text.

A string's text is the best evidence there is of what it is, so the name is the text: "M00:/sky.imd" becomes
gStr_M00_sky_imd. Text in kana is written in Latin letters (ワタシ becomes gText_Watashi). Text with kanji cannot be
spelled as an identifier, so it is named by where it is, gText_<address>, which still says that it is text.
Where two strings would get one name, as equal literals in different files do, each gets its address after it.

Only symbols that have no name yet (data_<address>) are renamed. Strings that a source file owns are left alone,
since their names are the compiler's, and so is a symbol that the sources refer to as data_<address>. Run
tools/strings.py first.
"""
import argparse
import collections
import csv
import re
import sys
import unicodedata
from pathlib import Path

ROOT = Path(__file__).parent.parent.resolve()
SYMBOL = re.compile(r"^(\S+) (kind:\w+.*) addr:0x([0-9a-f]+)(.*)$")
MAX = 40

KANA = {
    "ア": "a", "イ": "i", "ウ": "u", "エ": "e", "オ": "o", "カ": "ka", "キ": "ki", "ク": "ku", "ケ": "ke", "コ": "ko",
    "サ": "sa", "シ": "shi", "ス": "su", "セ": "se", "ソ": "so", "タ": "ta", "チ": "chi", "ツ": "tsu", "テ": "te",
    "ト": "to", "ナ": "na", "ニ": "ni", "ヌ": "nu", "ネ": "ne", "ノ": "no", "ハ": "ha", "ヒ": "hi", "フ": "fu",
    "ヘ": "he", "ホ": "ho", "マ": "ma", "ミ": "mi", "ム": "mu", "メ": "me", "モ": "mo", "ヤ": "ya", "ユ": "yu",
    "ヨ": "yo", "ラ": "ra", "リ": "ri", "ル": "ru", "レ": "re", "ロ": "ro", "ワ": "wa", "ヲ": "wo", "ン": "n",
    "ガ": "ga", "ギ": "gi", "グ": "gu", "ゲ": "ge", "ゴ": "go", "ザ": "za", "ジ": "ji", "ズ": "zu", "ゼ": "ze",
    "ゾ": "zo", "ダ": "da", "ヂ": "ji", "ヅ": "zu", "デ": "de", "ド": "do", "バ": "ba", "ビ": "bi", "ブ": "bu",
    "ベ": "be", "ボ": "bo", "パ": "pa", "ピ": "pi", "プ": "pu", "ペ": "pe", "ポ": "po", "ァ": "a", "ィ": "i",
    "ゥ": "u", "ェ": "e", "ォ": "o", "ャ": "ya", "ュ": "yu", "ョ": "yo", "ッ": "", "ー": "", "ヴ": "vu",
}


def romanize(text: str) -> str | None:
    """Returns kana text in Latin letters, or None if it has anything else that cannot be spelled."""
    text = unicodedata.normalize("NFKC", text)  # Half-width kana and full-width letters to their plain forms
    out = []
    for c in text:
        if "ぁ" <= c <= "ゖ":
            c = chr(ord(c) + 0x60)  # Hiragana to katakana
        if c in KANA:
            out.append(KANA[c])
        elif ord(c) < 0x80:
            out.append(c)
        elif c in "　、。・「」！？":
            out.append("_")
        else:
            return None
    return "".join(out)


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
    with table.open(encoding="utf-8", newline="") as file:
        for row in csv.DictReader(file, delimiter="\t", quoting=csv.QUOTE_NONE):
            if row["unit"]:
                continue
            text = row["text"].replace("\\n", "\n").replace("\\r", "\r").replace("\\t", "\t").replace("\\\\", "\\")
            address = int(row["address"], 16)
            if row["encoding"] == "ascii":
                body = identifier(text)
                name = f"gStr_{body}" if body else None
            else:
                spelled = romanize(text)
                body = identifier(spelled.title() if spelled and spelled.islower() else spelled) if spelled else ""
                name = f"gText_{body}" if body else None
            wanted[(row["module"], address)] = name or f"gText_{address:08x}"

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

    renamed = added = 0
    for module, (path, lines) in files.items():
        by_address = {}
        for number, line in enumerate(lines):
            match = SYMBOL.match(line)
            if match:
                by_address[int(match[3], 16)] = (number, match)
        for (owner, address), name in sorted(wanted.items()):
            if owner != module:
                continue
            if counts[name] > 1 or taken[name] or name.startswith("gText_0"):
                # Told apart by where they are; an overlay's by the overlay too, as dsd's own names are
                name = re.sub(r"_[0-9a-f]{8}$", "", name) if name.startswith("gText_0") else name
                where = f"{module}_{address:08x}" if module.startswith("ov") else f"{address:08x}"
                name = f"{name}_{where}" if name != "gText" else f"gText_{where}"
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
