#!/usr/bin/env python3
"""Names the data of a unit in symbols.txt by the compiled object's own symbols, for marking the unit complete.

A complete unit needs every symbol of its ranges in symbols.txt by the name the linked program has, its anonymous
objects (string literals, `@13102`) and file statics included. This lays out the object's sections of each kind in
the order of their indices, each aligned to four bytes or more (the linker script's ALIGNALL(4)), from the start of
the range given for that kind, and prints the address each data symbol gets. With --apply it replaces the data
symbols in those ranges of symbols.txt with them (statics and anonymous objects as local).

It only says where the compiled data would go: check that it is where the original has it (the addresses of the
symbols it replaces) before marking the unit complete; `ninja` then checks the ROM. The compiler's numbers for
anonymous objects change when the file, or a header it includes, changes: run it again then.

    python3 tools/name_unit_data.py build/jpn_rev1/src/overlays/ov107/SaveErrorScene.o \\
        config/jpn_rev1/arm9/overlays/ov107/symbols.txt .rodata=0x02149ffc:0x0214a050 .data=0x0214a060:0x0214a1a0
"""
import argparse
import re
import subprocess


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


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("object")
    parser.add_argument("symbols")
    parser.add_argument("ranges", nargs="+", metavar="SECTION=START:END")
    parser.add_argument("--apply", action="store_true", help="write the names into symbols.txt")
    args = parser.parse_args()

    ranges = {}
    for text in args.ranges:
        kind, bounds = text.split("=")
        start, end = bounds.split(":")
        ranges[kind] = (int(start, 16), int(end, 16))

    secs = sections(args.object)
    syms = data_symbols(args.object)
    placed = []
    for kind, (start, end) in ranges.items():
        address = start
        for index in sorted(secs):
            name, size, align = secs[index]
            if name != kind:
                continue
            align = max(align, 4)
            address = (address + align - 1) // align * align
            for offset, symbol, binding in sorted(syms.get(index, [])):
                placed.append((address + offset, symbol, binding))
            address += size
        print(f"{kind}: ends at {address:#010x}, the range at {end:#010x}")
    for address, symbol, binding in placed:
        print(f"{address:#010x} {symbol} {binding.lower()}")

    if args.apply:
        lines = open(args.symbols).read().rstrip("\n").split("\n")
        kept = []
        for line in lines:
            match = re.search(r"addr:(0x[0-9a-fA-F]+)", line)
            address = int(match[1], 16) if match else None
            if address is not None and "kind:data" in line and any(s <= address < e for s, e in ranges.values()):
                continue
            kept.append(line)
        for address, symbol, binding in placed:
            kept.append(f"{symbol} kind:data(any) addr:{address:#010x}" + (" local" if binding == "LOCAL" else ""))

        def where(line: str) -> int:
            match = re.search(r"addr:(0x[0-9a-fA-F]+)", line)
            return int(match[1], 16) if match else 0

        kept.sort(key=where)
        open(args.symbols, "w").write("\n".join(kept) + "\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
