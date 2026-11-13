#!/usr/bin/env python3
"""Works out where a compiled library file's data is in the game, and adds those sections to its delinks.txt entry.

A matched function holds, wherever it refers to data, the address the linker put that data at. So the functions of a
unit that match say where the data sections of its object went. A section is placed only when
  - a matched function refers to it, or to a global symbol in it, so its address is known, not guessed
  - its bytes in the game are those of the object, outside relocations
and a unit only when its placed sections of each kind lie end to end. The symbols that symbols.txt has in those
ranges are renamed to the object's names for them, and marked local where the object's are.

It only edits the configs. Whether a unit can then be linked from its object is for tools/complete_units.py and the
build to say.
"""
import argparse
import collections
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).parent.parent.resolve()
SHT_PROGBITS, SHT_SYMTAB, SHT_RELA, SHT_NOBITS, SHT_REL = 1, 2, 4, 8, 9
SHF_ALLOC, SHF_EXECINSTR = 2, 4
STT_SECTION, STT_FUNC = 3, 2
R_ARM_ABS32 = 2
KINDS = {".rodata": ".rodata", ".data": ".data", ".bss": ".bss", ".sbss": ".sbss"}
SYMBOL = re.compile(r"^(\S+) (kind:(\w+)\S*) addr:0x([0-9a-f]+)(.*)$")


class Object:
    """The sections, symbols and relocations of a relocatable ELF object."""

    def __init__(self, path: Path):
        data = self.data = path.read_bytes()
        shoff, = struct.unpack_from("<I", data, 0x20)
        shentsize, shnum, _ = struct.unpack_from("<3H", data, 0x2E)
        raw = [struct.unpack_from("<10I", data, shoff + i * shentsize) for i in range(shnum)]
        shstr = raw[struct.unpack_from("<H", data, 0x32)[0]][4]
        self.sections = []  # (name, kind, flags, offset, size)
        for name, kind, flags, _, offset, size, _, _, _, _ in raw:
            end = data.index(b"\0", shstr + name)
            self.sections.append((data[shstr + name:end].decode("latin-1"), kind, flags, offset, size))
        self.symbols = []  # (name, value, size, kind, is_global, section)
        for _, kind, _, _, offset, size, link, _, _, entsize in raw:
            if kind == SHT_SYMTAB:
                strings = raw[link][4]
                for i in range(size // entsize):
                    name, value, symbol_size, info, _, shndx = struct.unpack_from("<3I2BH", data, offset + i * entsize)
                    end = data.index(b"\0", strings + name)
                    self.symbols.append((data[strings + name:end].decode("latin-1"), value, symbol_size, info & 0xF,
                                         info >> 4 == 1, shndx))
        self.relocs = collections.defaultdict(list)  # Section -> (offset, kind, symbol index, addend)
        for _, kind, _, _, offset, size, _, info, _, entsize in raw:
            if kind in (SHT_REL, SHT_RELA) and entsize:
                for i in range(size // entsize):
                    if kind == SHT_RELA:
                        where, word, addend = struct.unpack_from("<IIi", data, offset + i * entsize)
                    else:
                        (where, word), addend = struct.unpack_from("<II", data, offset + i * entsize), 0
                    self.relocs[info].append((where, word & 0xFF, word >> 8, addend))

    def bytes_of(self, section: int) -> bytes:
        _, _, _, offset, size = self.sections[section]
        return self.data[offset:offset + size]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="jpn_rev1")
    parser.add_argument("--apply", action="store_true", help="edit delinks.txt and symbols.txt")
    args = parser.parse_args()

    config = ROOT / "config" / args.version / "arm9"
    build = ROOT / "build" / args.version
    delinks_path, symbols_path = config / "delinks.txt", config / "symbols.txt"
    delinks = delinks_path.read_text(encoding="utf-8")
    lines = symbols_path.read_text(encoding="utf-8").splitlines()
    rom = (ROOT / "extract" / args.version / "arm9" / "arm9.bin").read_bytes()
    module = {m[1]: (int(m[2], 16), int(m[3], 16))
              for m in re.finditer(r"^\s+(\.\w+)\s+start:0x([0-9a-f]+) end:0x([0-9a-f]+) kind:", delinks, re.M)}
    base = min(start for start, _ in module.values())

    by_name, by_address = {}, {}
    for number, line in enumerate(lines):
        match = SYMBOL.match(line)
        if match:
            address = int(match[4], 16)
            by_address[address] = number
            size = re.search(r"size=0x([0-9a-f]+)", match[2])
            if match[3] == "function" and size:
                by_name[match[1]] = (address, int(size[1], 16))

    # Units: source files with an entry and a compiled object
    units = {}
    for match in re.finditer(r"^(\S+\.(?:c|cpp)):\n((?:[ \t]+.*\n?)*)", delinks, re.M):
        obj = build / Path(match[1]).with_suffix(".o")
        if obj.exists():
            units[match[1]] = (Object(obj), match[2])

    # Where the game put things, read off the matched functions
    global_address = {}
    section_address = collections.defaultdict(dict)  # Unit -> section -> address
    for source, (obj, _) in units.items():
        for name, value, _, kind, _, shndx in obj.symbols:
            if kind != STT_FUNC or name not in by_name or name.startswith("$"):
                continue
            address, size = by_name[name]
            start = value & ~1
            code = obj.bytes_of(shndx)[start:start + size]
            game = rom[address - base:address - base + size]
            sites = [r for r in obj.relocs[shndx] if start <= r[0] < start + size]
            masked = bytearray(len(code))
            for where, _, _, _ in sites:
                masked[where - start:where - start + 4] = b"\xff" * 4
            if len(code) != size or any(a != b and not m for a, b, m in zip(code, game, masked)):
                continue  # Not a match, so it says nothing
            for where, reloc_kind, symbol, addend in sites:
                if reloc_kind != R_ARM_ABS32:
                    continue
                target_name, target_value, _, target_kind, _, target_section = obj.symbols[symbol]
                target = (struct.unpack_from("<I", game, where - start)[0] - addend) & 0xFFFFFFFF
                if target_section == 0:
                    global_address[target_name] = target
                elif target_section < len(obj.sections):
                    section_address[source][target_section] = target - target_value
    for source, (obj, _) in units.items():
        for name, value, _, kind, is_global, shndx in obj.symbols:
            if is_global and name in global_address and 0 < shndx < len(obj.sections) \
                    and not obj.sections[shndx][2] & SHF_EXECINSTR:
                section_address[source].setdefault(shndx, global_address[name] - value)

    placed_units, renames = {}, {}
    for source, (obj, body) in units.items():
        if {line.split()[0] for line in body.splitlines() if line.strip()} - {".text", "complete"}:
            continue  # Its data is placed already
        ranges = collections.defaultdict(list)
        ok = True
        for index, address in section_address[source].items():
            name, kind, flags, _, size = obj.sections[index]
            if not flags & SHF_ALLOC or flags & SHF_EXECINSTR or not size:
                continue
            if name not in KINDS or not module.get(KINDS[name]) \
                    or not module[KINDS[name]][0] <= address < module[KINDS[name]][1]:
                ok = False
                break
            if kind == SHT_PROGBITS:
                content = bytearray(obj.bytes_of(index))
                game = bytearray(rom[address - base:address - base + size])
                for where, _, _, _ in obj.relocs[index]:
                    content[where:where + 4] = game[where:where + 4] = bytes(4)
                if content != game:
                    ok = False
                    break
            ranges[KINDS[name]].append((address, address + size, index))
        if not ok or not ranges:
            continue
        entry, names = [], {}
        for kind, sections in ranges.items():
            sections.sort()
            # End to end, give or take the padding between two sections
            if any(not 0 <= b[0] - a[1] < 32 for a, b in zip(sections, sections[1:])):
                ok = False
                break
            start, end = sections[0][0], (sections[-1][1] + 3) & ~3
            entry.append((kind, start, end))
            for address, _, index in sections:
                for name, value, _, symbol_kind, is_global, shndx in obj.symbols:
                    if shndx == index and name and symbol_kind != STT_SECTION and not name.startswith(("$", ".")):
                        names[address + value] = (name, is_global)
            # Every symbol the config has in the range needs a name that the object has there
            if any(a not in names for a in by_address if start <= a < end):
                ok = False
                break
        if ok:
            placed_units[source] = entry
            renames.update(names)

    # A name can only be global once
    counts = collections.Counter(name for name, is_global in renames.values() if is_global)
    existing = {m[1] for line in lines if (m := SYMBOL.match(line))}
    print(f"{len(placed_units)} units can have their data placed, of {len(units)} with an object")
    for source, entry in sorted(placed_units.items()):
        print(f"  {source}: " + ", ".join(f"{kind} {start:#010x}-{end:#010x}" for kind, start, end in entry))
    if not args.apply:
        return 0

    for address, (name, is_global) in renames.items():
        number = by_address.get(address)
        if number is None:
            continue
        match = SYMBOL.match(lines[number])
        if match[1] == name:
            continue
        local = not is_global or counts[name] > 1 or name in existing
        attributes = match[5].replace(" local", "") + (" local" if local else "")
        lines[number] = f"{name} {match[2]} addr:0x{match[4]}{attributes}"
    symbols_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    for source, entry in placed_units.items():
        match = re.search(rf"^{re.escape(source)}:\n((?:[ \t]+.*\n?)*)", delinks, re.M)
        added = "".join(f"    {kind:<11} start:{start:#010x} end:{end:#010x}\n" for kind, start, end in entry)
        delinks = delinks[:match.end(1)] + added + delinks[match.end(1):]
    delinks_path.write_text(delinks, encoding="utf-8")
    print(f"Placed the data of {len(placed_units)} units")
    return 0


if __name__ == "__main__":
    sys.exit(main())
