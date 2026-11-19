#!/usr/bin/env python3
"""Finds, and with --apply renames, the compiler's numbered names of complete units that symbols.txt has stale.

The compiler numbers its anonymous objects (`@13194`) and function-local statics (`sClothes$13870`) through the
whole translation unit, headers included, so a change to a shared header renumbers them in every complete unit that
includes it, and symbols.txt keeps the old numbers. Neither the ROM nor `dsd check symbols` notices (the bytes and
addresses are the same); `tools/check_relocs.py` does. This lays each complete unit's compiled data out as
`tools/name_unit_data.py` does (the sections the link keeps, from the start of each range) and compares the names
with symbols.txt at the same addresses. --apply renames, in place, a numbered name whose compiled counterpart at
the same address differs only in its number; anything else is only listed. Run it after `ninja`, and again after
merging branches that change shared headers. The speech engine (C without -ipa file) has no symbols for its
literals and is left out.

    python3 tools/stale_names.py [--apply]
"""
import argparse
import glob
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).parent.parent.resolve()


def numbered_alike(compiled: str, listed: str) -> bool:
    """Whether two names are the same numbered name but for its number."""
    if compiled.startswith("@") and listed.startswith("@"):
        return True
    return "$" in compiled and "$" in listed and compiled.split("$")[0] == listed.split("$")[0]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="jpn_rev1")
    parser.add_argument("--apply", action="store_true", help="rename stale numbered names in symbols.txt")
    args = parser.parse_args()

    config = ROOT / "config" / args.version / "arm9"
    total = renamed = 0
    for delinks in [config / "delinks.txt", *sorted(Path(p) for p in glob.glob(str(config / "overlays/*/delinks.txt")))]:
        symbols = delinks.with_name("symbols.txt")
        lines = symbols.read_text(encoding="utf-8").split("\n")
        by_address = {}
        for number, line in enumerate(lines):
            match = re.match(r"(\S+) kind:(\S+) addr:0x([0-9a-fA-F]+)", line)
            if match and not match[2].startswith("function"):
                by_address.setdefault(int(match[3], 16), []).append(number)
        changed = False
        text = delinks.read_text(encoding="utf-8")
        for unit in re.finditer(r"^(\S+\.(?:c|cpp)):\n((?:[ \t]+.*\n?)*)", text, re.M):
            source, body = unit[1], unit[2]
            if not re.search(r"^\s+complete\s*$", body, re.M) or source.startswith("lib/SHTTS/"):
                continue
            ranges = re.findall(r"(\.rodata|\.data|\.bss|\.sbss)\s+start:0x([0-9a-fA-F]+) end:0x([0-9a-fA-F]+)", body)
            obj = ROOT / "build" / args.version / Path(source).with_suffix(".o")
            if not ranges or not obj.exists():
                continue
            out = subprocess.run([sys.executable, str(ROOT / "tools/name_unit_data.py"), str(obj), str(symbols),
                                  *(f"{k}=0x{a}:0x{b}" for k, a, b in ranges)],
                                 capture_output=True, text=True, check=True).stdout
            differences = []
            for line in out.splitlines():
                match = re.match(r"0x([0-9a-f]+) (\S+) \w+$", line)
                if not match:
                    continue
                address, name = int(match[1], 16), match[2]
                numbers = by_address.get(address, [])
                listed = [lines[n].split()[0] for n in numbers]
                if name not in listed:
                    differences.append((address, name, listed, numbers))
            if not differences:
                continue
            print(f"{source}: {len(differences)} names differ")
            total += len(differences)
            for address, name, listed, numbers in differences:
                if args.apply and len(listed) == 1 and numbered_alike(name, listed[0]):
                    lines[numbers[0]] = name + lines[numbers[0]][len(listed[0]):]
                    changed = True
                    renamed += 1
                else:
                    print(f"    {address:#010x}: compiled {name}, symbols.txt {', '.join(listed) or 'nothing'}")
        if changed:
            symbols.write_text("\n".join(lines), encoding="utf-8")
    print(f"{total} names of complete units differ from the compiled objects" + (f", {renamed} renamed" if args.apply else ""))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
