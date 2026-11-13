#!/usr/bin/env python3
"""Compares the functions of a compiled object with the game's code at the addresses a map gives them.

For code whose functions have new names, as C++ member functions do: the object knows them by their mangled names
and the game by address, so a map says which is which, one "ADDRESS NAME" per line (0x0202d5a8 _ZN4Font4InitEv).
For each mapped function it says whether the bytes are the game's outside the object's relocations. Functions of the
object that the map does not have are listed with their sizes, in the object's order, to help write the map.
With --diff ADDRESS it prints a diff of the instructions of one function (it needs arm-none-eabi-objdump).
"""
import argparse
import difflib
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from place_data import STT_FUNC, Object  # noqa: E402

ROOT = Path(__file__).parent.parent.resolve()
BASE = 0x02000000


def clean(lines):
    out = []
    for line in lines:
        match = re.match(r"^\s*[0-9a-f]+:\s+[0-9a-f]+\s+(.*)$", line)
        if match:
            text = re.sub(r"[;@].*|<[^>]*>", "", match[1])
            text = re.sub(r"^(b[a-z]*)\s+(0x)?[0-9a-f]+", r"\1 X", text)
            out.append(re.sub(r"\[pc, #-?\d+\]", "[pc]", text).replace("\t", " ").strip())
    return out


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("compiled", type=Path)
    parser.add_argument("--map", type=Path, required=True)
    parser.add_argument("--version", default="jpn_rev1")
    parser.add_argument("--diff", metavar="ADDRESS")
    args = parser.parse_args()

    rom_path = ROOT / "extract" / args.version / "arm9" / "arm9.bin"
    rom = rom_path.read_bytes()
    sizes = {}
    for line in (ROOT / "config" / args.version / "arm9" / "symbols.txt").read_text(encoding="utf-8").splitlines():
        match = re.match(r"\S+ kind:function\(\w+,size=0x([0-9a-f]+)[^)]*\) addr:0x([0-9a-f]+)", line)
        if match:
            sizes[int(match[2], 16)] = int(match[1], 16)
    names = {}
    for line in args.map.read_text(encoding="utf-8").splitlines():
        parts = line.split("#")[0].split()
        if len(parts) >= 2:
            names[parts[1]] = int(parts[0], 16)

    obj = Object(args.compiled)
    functions = []
    for name, value, size, kind, _, shndx in obj.symbols:
        if kind == STT_FUNC and name and not name.startswith("$") and 0 < shndx < len(obj.sections):
            data = obj.bytes_of(shndx)
            start = value & ~1
            end = start + size if size else len(data)
            sites = {w - start for w, _, _, _ in obj.relocs[shndx] if start <= w < end}
            functions.append((shndx, start, name, data[start:end], sites))
    functions.sort()

    if args.diff:
        address = int(args.diff, 16)
        name = next((n for n, a in names.items() if a == address), None)
        if name is None:
            print(f"{args.diff} is not in the map", file=sys.stderr)
            return 1
        ours = subprocess.run(["arm-none-eabi-objdump", "-d", f"--disassemble={name}", str(args.compiled)],
                              capture_output=True, text=True).stdout.splitlines()
        theirs = subprocess.run(["arm-none-eabi-objdump", "-D", "-b", "binary", "-marm", f"--adjust-vma={BASE:#x}",
                                 f"--start-address={address:#x}", f"--stop-address={address + sizes[address]:#x}",
                                 str(rom_path)], capture_output=True, text=True).stdout.splitlines()
        for line in difflib.unified_diff(clean(theirs), clean(ours), "original", "compiled", lineterm="", n=2):
            print(line)
        return 0

    matched = mapped = 0
    for _, _, name, code, sites in functions:
        if name not in names:
            print(f"unmapped: {name} ({len(code):#x} bytes)")
            continue
        mapped += 1
        address = names[name]
        size = sizes.get(address)
        if size is None:
            print(f"{address:#010x} {name}: the game has no function at that address")
            continue
        if len(code) != size:
            print(f"{address:#010x} {name}: {len(code):#x} bytes, the original is {size:#x}")
            continue
        original = rom[address - BASE:address - BASE + size]
        masked = set()
        for site in sites:
            masked.update(range(site, site + 4))
        different = sorted({i & ~3 for i in range(size) if code[i] != original[i] and i not in masked})
        if different:
            print(f"{address:#010x} {name}: differs, first at {different[0]:#x} of {size:#x}, "
                  f"{len(different)} instructions in all")
        else:
            matched += 1
            print(f"{address:#010x} {name}: OK")
    print(f"{matched} of {mapped} mapped functions match")
    return 0


if __name__ == "__main__":
    sys.exit(main())
