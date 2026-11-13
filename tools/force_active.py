#!/usr/bin/env python3
"""Adds a FORCE_ACTIVE list to the linker script, naming every global symbol of the objects that dsd delinked.

The linker strips what nothing refers to, which the original link did too: a compiled file holds functions that the
game never calls, and they must go for the rest to land where it did. But the delinked objects are the game as it is,
and only as much of them is known to be referred to as relocs.txt knows. So everything in them is kept by name, and
stripping is left to work on the compiled objects alone.
"""
import argparse
import re
import struct
import sys
from pathlib import Path

SHT_SYMTAB = 2
STB_GLOBAL = 1
# What the linker script takes for a name. The labels left out (".L_02020bf8") are inside functions that are named.
IDENTIFIER = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")


def global_symbols(path: Path) -> list[str]:
    """Returns the names of the global symbols that an ELF object defines."""
    data = path.read_bytes()
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, _ = struct.unpack_from("<3H", data, 0x2E)
    sections = [struct.unpack_from("<10I", data, shoff + i * shentsize) for i in range(shnum)]
    names = []
    for _, kind, _, _, offset, size, link, _, _, entsize in sections:
        if kind != SHT_SYMTAB:
            continue
        strings = sections[link][4]
        for i in range(size // entsize):
            name, _, _, info, _, shndx = struct.unpack_from("<3I2BH", data, offset + i * entsize)
            if info >> 4 == STB_GLOBAL and shndx != 0 and name:
                end = data.index(b"\0", strings + name)
                names.append(data[strings + name:end].decode("latin-1"))
    return names


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("lcf", type=Path, help="linker script from dsd")
    parser.add_argument("output", type=Path)
    parser.add_argument("--objects", type=Path, required=True, help="file that lists the delinked objects to keep whole")
    args = parser.parse_args()

    symbols = []
    for line in args.objects.read_text(encoding="utf-8").splitlines():
        # ninja quotes a path that has a character the shell would act on, such as the @ of dsd's gap objects
        path = line.strip().strip("'\"")
        if path:
            symbols += global_symbols(Path(path))
    symbols = sorted(symbol for symbol in set(symbols) if IDENTIFIER.match(symbol))
    block = "FORCE_ACTIVE {\n" + ",\n".join(f"    {symbol}" for symbol in symbols) + "\n}\n\n"

    script = args.lcf.read_text(encoding="utf-8")
    marker = "SECTIONS {"
    if marker not in script:
        print(f"{args.lcf} has no {marker}", file=sys.stderr)
        return 1
    args.output.write_text(script.replace(marker, block + marker, 1), encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main())
