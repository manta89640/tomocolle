#!/usr/bin/env python3
"""Compares the functions of a compiled object with those of the same names in the object dsd delinked.

A quick answer while writing a file, without the whole build or objdiff: for each function, whether its bytes are
those of the original outside relocations, and if not, where they first differ. With --diff NAME it prints the
two disassemblies of one function side by side as a diff (it needs arm-none-eabi-objdump for that).
"""
import argparse
import difflib
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from place_data import STT_FUNC, Object  # noqa: E402


def functions(obj: Object) -> dict:
    """Returns each function's bytes and the offsets in it that relocations fill in."""
    by_section = {}
    for name, value, size, kind, _, shndx in obj.symbols:
        if kind == STT_FUNC and name and not name.startswith("$") and 0 < shndx < len(obj.sections):
            by_section.setdefault(shndx, []).append((value & ~1, name, size))
    result = {}
    for shndx, symbols in by_section.items():
        symbols.sort()
        data = obj.bytes_of(shndx)
        for i, (start, name, size) in enumerate(symbols):
            end = start + size if size else (symbols[i + 1][0] if i + 1 < len(symbols) else len(data))
            sites = {where - start for where, _, _, _ in obj.relocs[shndx] if start <= where < end}
            result[name] = (data[start:end], sites)
    return result


def listing(path: Path, name: str) -> list[str]:
    out = subprocess.run(["arm-none-eabi-objdump", "-d", f"--disassemble={name}", str(path)],
                         capture_output=True, text=True).stdout
    lines = []
    for line in out.splitlines():
        match = re.match(r"^\s+[0-9a-f]+:\s+[0-9a-f]+\s+(.*)$", line)
        if match:
            text = re.sub(r"[;@].*|<[^>]*>", "", match[1])
            text = re.sub(r"^(b[a-z]*)\s+[0-9a-f]+", r"\1 X", text)
            lines.append(re.sub(r"\[pc, #\d+\]", "[pc]", text).replace("\t", " ").strip())
    return lines


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("compiled", type=Path)
    parser.add_argument("target", type=Path)
    parser.add_argument("--diff", metavar="NAME", help="print the difference in one function's instructions")
    args = parser.parse_args()

    if args.diff:
        ours, theirs = listing(args.compiled, args.diff), listing(args.target, args.diff)
        for line in difflib.unified_diff(theirs, ours, "original", "compiled", lineterm="", n=2):
            print(line)
        return 0

    compiled, target = functions(Object(args.compiled)), functions(Object(args.target))
    matched = 0
    for name, (code, sites) in compiled.items():
        if name not in target:
            print(f"{name}: not in the original under this name")
            continue
        original, original_sites = target[name]
        masked = set()
        for site in sites | original_sites:
            masked.update(range(site, site + 4))
        if len(code) != len(original):
            print(f"{name}: {len(code):#x} bytes, the original is {len(original):#x}")
        else:
            different = [i for i in range(len(code)) if code[i] != original[i] and i not in masked]
            if different:
                print(f"{name}: differs, first at {different[0] & ~3:#x} of {len(code):#x}, "
                      f"{len({i & ~3 for i in different})} instructions in all")
            else:
                matched += 1
                print(f"{name}: OK")
    print(f"{matched} of {len(compiled)} match")
    return 0


if __name__ == "__main__":
    sys.exit(main())
