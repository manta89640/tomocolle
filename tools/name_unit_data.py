#!/usr/bin/env python3
"""Names the data of a unit in symbols.txt by the compiled object's own symbols, for marking the unit complete.

A complete unit needs every symbol of its ranges in symbols.txt by the name the linked program has, its anonymous
objects (string literals, `@13102`) and file statics included. This lays out the object's sections of each kind in
the order of their indices, each aligned to four bytes or more (the linker script's ALIGNALL(4)), from the start of
the range given for that kind, and prints the address each data symbol gets. With --apply it replaces the data
symbols in those ranges of symbols.txt with them (statics and anonymous objects as local).

Only the sections the link keeps are laid out, as the linker strips the rest: those reachable, through the object's
relocations, from a function that symbols.txt has (so the game has it) or from a static initialiser, or holding a
global object symbols.txt has. A constructor emitted as C1 and C2 has its string literals twice, and the game keeps
one of the two functions and so one set (Keyboard.cpp); an object that a function the game does not have alone
refers to goes too. A binding-13 object (an inline class's virtual table) that symbols.txt places outside the
ranges, in the file whose copy the linker kept, is not laid out either. --all lays out every section, as before.

It only says where the compiled data would go: check that it is where the original has it (the addresses of the
symbols it replaces) before marking the unit complete; `ninja` then checks the ROM. The compiler's numbers for
anonymous objects change when the file, or a header it includes, changes: run it again then.

    python3 tools/name_unit_data.py build/jpn_rev1/src/overlays/ov107/SaveErrorScene.o \\
        config/jpn_rev1/arm9/overlays/ov107/symbols.txt .rodata=0x02149ffc:0x0214a050 .data=0x0214a060:0x0214a1a0
"""
import argparse
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from place_data import SHF_ALLOC, SHF_EXECINSTR, STT_FUNC, STT_SECTION, Object  # noqa: E402


def sections(obj: str) -> dict:
    """Index -> (name, size, alignment) of each section of the object."""
    out = subprocess.run(["arm-none-eabi-readelf", "-SW", obj], capture_output=True, text=True, check=True).stdout
    result = {}
    for line in out.splitlines():
        match = re.match(r"\s*\[\s*(\d+)\]\s+(\S+)\s+\S+\s+[0-9a-f]+\s+[0-9a-f]+\s+([0-9a-f]+)\s+\S+\s+\S*\s+\d+\s+\d+\s+(\d+)",
                         line)
        if match:
            result[int(match[1])] = (match[2], int(match[3], 16), int(match[4]))
    return result


def data_symbols(obj: str) -> dict:
    """Section index -> [(offset, name, binding)] of the object's data symbols."""
    out = subprocess.run(["arm-none-eabi-readelf", "-sW", obj], capture_output=True, text=True, check=True).stdout
    result = {}
    for line in out.splitlines():
        fields = line.split()
        if len(fields) >= 8 and fields[3] in ("OBJECT", "NOTYPE") and fields[6].isdigit():
            name = fields[7]
            if name.startswith("$") or name.startswith("."):
                continue
            result.setdefault(int(fields[6]), []).append((int(fields[1], 16), name, fields[4]))
    return result


def kept_sections(path: str, symbols: str, ranges: dict) -> set:
    """The indices of the object's sections that the link keeps (see the module's description)."""
    obj = Object(Path(path))
    known = {}  # name -> (kind, address) in symbols.txt
    for line in open(symbols, encoding="utf-8"):
        match = re.match(r"(\S+) kind:(\w+)\S* addr:0x([0-9a-fA-F]+)", line)
        if match:
            known[match[1]] = (match[2], int(match[3], 16))
    inside = lambda address: any(start <= address < end for start, end in ranges.values())
    # Objects of MW's binding 13, which the linker keeps one copy of
    out = subprocess.run(["arm-none-eabi-readelf", "-sW", path], capture_output=True, text=True, check=True).stdout
    shared = {line.split()[-1] for line in out.splitlines() if "processor specific>: 13" in line}
    roots, dropped = set(), set()
    for name, _, _, kind, is_global, shndx in obj.symbols:
        if not 0 < shndx < len(obj.sections) or not name or name.startswith("$"):
            continue
        section_name, _, flags, _, _ = obj.sections[shndx]
        binding = known.get(name)
        if kind == STT_FUNC and name in known and known[name][0] == "function":
            roots.add(shndx)
        elif section_name in (".ctor", ".init", ".sinit"):
            roots.add(shndx)
        elif not flags & SHF_EXECINSTR and binding and binding[0] != "function" and (is_global or name in shared):
            if inside(binding[1]):
                roots.add(shndx)
            elif name in shared:
                dropped.add(shndx)  # The game's copy is another file's
    kept, todo = set(), [s for s in roots if s not in dropped]
    while todo:
        section = todo.pop()
        if section in kept:
            continue
        kept.add(section)
        for _, _, index, _ in obj.relocs.get(section, ()):
            target = obj.symbols[index][5]
            if 0 < target < len(obj.sections) and obj.sections[target][2] & SHF_ALLOC and target not in dropped:
                todo.append(target)
    return kept


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("object")
    parser.add_argument("symbols")
    parser.add_argument("ranges", nargs="+", metavar="SECTION=START:END")
    parser.add_argument("--apply", action="store_true", help="write the names into symbols.txt")
    parser.add_argument("--all", action="store_true", help="lay out every section, also those the link drops")
    args = parser.parse_args()

    ranges = {}
    for text in args.ranges:
        kind, bounds = text.split("=")
        start, end = bounds.split(":")
        ranges[kind] = (int(start, 16), int(end, 16))

    secs = sections(args.object)
    syms = data_symbols(args.object)
    kept = None if args.all else kept_sections(args.object, args.symbols, ranges)
    placed = []
    for kind, (start, end) in ranges.items():
        address = start
        for index in sorted(secs):
            name, size, align = secs[index]
            if name != kind or (kept is not None and index not in kept):
                continue
            align = max(align, 4)
            address = (address + align - 1) // align * align
            for offset, symbol, binding in sorted(syms.get(index, [])):
                placed.append((address + offset, symbol, binding, kind))
            address += size
        print(f"{kind}: ends at {address:#010x}, the range at {end:#010x}")
    for address, symbol, binding, _ in placed:
        print(f"{address:#010x} {symbol} {binding.lower()}")

    if args.apply:
        lines = open(args.symbols).read().rstrip("\n").split("\n")
        # Only a symbol at an address the object names is replaced. One the object has but readelf does not list
        # as OBJECT or NOTYPE, such as the virtual tables MW emits with binding 13, is kept as it is.
        named = {address for address, _, _, _ in placed}
        kept = []
        for line in lines:
            match = re.search(r"addr:(0x[0-9a-fA-F]+)", line)
            address = int(match[1], 16) if match else None
            if address in named and ("kind:data" in line or "kind:bss" in line) and any(s <= address < e for s, e in ranges.values()):
                continue
            kept.append(line)
        for address, symbol, binding, kind in placed:
            # A .bss object is kind:bss: dsd dis reads the bytes of a data symbol, and panics on .bss (overlay 110)
            kind = "bss" if kind in (".bss", ".sbss") else "data(any)"
            kept.append(f"{symbol} kind:{kind} addr:{address:#010x}" + (" local" if binding == "LOCAL" else ""))

        def where(line: str) -> int:
            match = re.search(r"addr:(0x[0-9a-fA-F]+)", line)
            return int(match[1], 16) if match else 0

        kept.sort(key=where)
        open(args.symbols, "w").write("\n".join(kept) + "\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
