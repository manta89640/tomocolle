#!/usr/bin/env python3
"""Gives the inline functions in dsd's delinked objects the symbol binding the compiler gave them.

mwccarm emits an inline function (an implicit destructor, a member defined in its class) in every file that uses it,
under its symbol binding 13 (readelf shows it as "processor specific"), and the linker keeps the copy of the first
object in link order and drops the others. dsd delinks every symbol as global. So a complete unit that emits such a
function cannot be linked while the copy the game kept is in a unit that is not complete: the linker takes a global
and a binding 13 definition of one name as defined twice. The file named on the command line lists these functions;
this tool gives each of them, in the delinked objects, the binding the original object had, and the linker then keeps
the game's copy as it did.

A name belongs on the list when the code shows it is one of these (see docs/decompiling.md, "Functions emitted in
every file that uses them"). Once the unit that holds the game's copy is complete, its own compiled object has the
binding, and the entry does no harm.
"""
import argparse
import struct
import sys
from pathlib import Path

SHT_SYMTAB = 2
STB_GLOBAL = 1
STB_MW_INLINE = 13


def read_list(path: Path) -> set[str]:
    names = set()
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.split("#", 1)[0].strip()
        if line:
            names.add(line)
    return names


def patch(path: Path, names: set[str]) -> set[str]:
    """Sets the binding of the listed global symbols that the object defines, and returns the names it found."""
    data = bytearray(path.read_bytes())
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, _ = struct.unpack_from("<3H", data, 0x2E)
    sections = [struct.unpack_from("<10I", data, shoff + i * shentsize) for i in range(shnum)]
    found = set()
    for _, kind, _, _, offset, size, link, _, _, entsize in sections:
        if kind != SHT_SYMTAB:
            continue
        strings = sections[link][4]
        for i in range(size // entsize):
            entry = offset + i * entsize
            name, _, _, info, _, shndx = struct.unpack_from("<3I2BH", data, entry)
            if shndx == 0 or not name or info >> 4 not in (STB_GLOBAL, STB_MW_INLINE):
                continue
            end = data.index(b"\0", strings + name)
            symbol = data[strings + name:end].decode("latin-1")
            if symbol in names:
                found.add(symbol)
                data[entry + 12] = (STB_MW_INLINE << 4) | (info & 0xF)
    if found:
        path.write_bytes(data)
    return found


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("list", type=Path, help="file naming the inline functions, one per line")
    parser.add_argument("delinks", type=Path, help="directory of the delinked objects")
    args = parser.parse_args()

    names = read_list(args.list)
    found = set()
    for path in sorted(args.delinks.rglob("*.o")):
        found |= patch(path, names)
    missing = sorted(names - found)
    if missing:
        print(f"{args.list}: defined by no delinked object: {', '.join(missing)}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
