#!/usr/bin/env python3
"""Takes the data mapping symbols off DS Protect's marker words in a compiled object, so that objdiff reads them as
the delinked original does.

Each encrypted range of DS Protect's checks lies between two copies of a marker word (docs/binary.md, "DS Protect").
The source writes the word with the inline assembler's `dcd`, which gives it a `$d` mapping symbol: objdiff then
shows it as data. dsd delinks the original's word as code (the marker is an encoding of `bl`), with no `$d`, so the
two disagree on an equal word and the function scores below 100% (96.7% to 98.9% for the six checks). symbols.txt
names each range of a function as `dsprot_range=(start..end,marker)`, the marker being the word before `start` and
the word at `end`; this blanks the name of a `$d` at either word, in place, which takes it out of objdiff's mapping
symbols and leaves the object otherwise as it was. Nothing in the link reads mapping symbols.

    python3 tools/dsprotect_markers.py build/jpn_rev1/lib/DSProtect/DSProtect.o config/jpn_rev1/arm9/symbols.txt
"""
import argparse
import re
import struct
from pathlib import Path

RANGE = re.compile(r"dsprot_range=\(0x([0-9a-fA-F]+)\.\.0x([0-9a-fA-F]+),0x[0-9a-fA-F]+\)")


def marker_offsets(symbols: Path) -> dict:
    """Function name -> offsets of its marker words from the function's start."""
    result = {}
    for line in symbols.read_text(encoding="utf-8").splitlines():
        ranges = RANGE.findall(line)
        address = re.search(r"addr:0x([0-9a-fA-F]+)", line)
        if ranges and address:
            start = int(address[1], 16)
            result[line.split()[0]] = {o for s, e in ranges for o in (int(s, 16) - 4 - start, int(e, 16) - start)}
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("object", type=Path)
    parser.add_argument("symbols", type=Path)
    args = parser.parse_args()

    markers = marker_offsets(args.symbols)
    data = bytearray(args.object.read_bytes())
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, _ = struct.unpack_from("<3H", data, 0x2E)
    headers = [struct.unpack_from("<10I", data, shoff + i * shentsize) for i in range(shnum)]
    symtab = next(h for h in headers if h[1] == 2)  # SHT_SYMTAB
    strings = headers[symtab[6]][4]
    entries = []  # (offset of the entry, name, value, section)
    for i in range(symtab[5] // symtab[9]):
        at = symtab[4] + i * symtab[9]
        name, value, _, _, _, shndx = struct.unpack_from("<3I2BH", data, at)
        end = data.index(b"\0", strings + name)
        entries.append((at, data[strings + name:end].decode("latin-1"), value, shndx))
    # Where each function of DS Protect is, and so where its markers are
    wanted = set()
    for _, name, value, shndx in entries:
        for offset in markers.get(name, ()):
            wanted.add((shndx, (value & ~1) + offset))
    # A function that does not match yet may have no $d where the original's marker is: nothing is done there
    for at, name, value, shndx in entries:
        if name == "$d" and (shndx, value) in wanted:
            struct.pack_into("<I", data, at, 0)  # The empty name
    args.object.write_bytes(data)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
