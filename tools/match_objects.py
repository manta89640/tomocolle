#!/usr/bin/env python3
"""Finds the functions of known libraries in the game, by comparing compiled objects of those libraries with it.

The game links code that exists elsewhere as objects with names: the SDK, NitroSystem and the compiler's own library,
as built by other decompilations. A function of such an object and one of the game are the same when their bytes are
equal everywhere except where the object has a relocation, which the linker filled in.

Equal bytes are not always proof: a short function has many twins. So a match is only accepted on one of these
grounds, and each match says which:

  unique     enough bytes outside relocations, and only one function of the game and one name fit each other
  reference  an accepted function refers to this name at a relocation, and the game's code refers to this
             function at the same place
  order      the linker keeps an object's functions in order, and this is the only function that fits between two
             accepted ones of the same object

Give each library as NAME=PATH, where PATH is a directory of .o files (searched recursively) or a .a archive:

  tools/match_objects.py NitroSDK=~/pokeplatinum/build/subprojects/NitroSDK-4.2.30001/libraries

Without --apply it only reports. With it, it renames the symbols in symbols.txt that have no name yet, and with
--units it also cuts each object's range of functions out of the main program as a unit, under lib/NAME/. A unit is
named after its source file. Meson names an object after its source with the slashes replaced ("src_load_a.c.o"),
so give the library's sources as well, with --sources NAME=PATH, for the path to be told from the name.
"""
import argparse
import collections
import re
import struct
import sys
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).parent.parent.resolve()

# A match on bytes alone needs this many of them outside relocations
UNIQUE_BYTES = 32

SHT_PROGBITS, SHT_SYMTAB, SHT_RELA, SHT_REL = 1, 2, 4, 9
SHF_EXECINSTR = 4
STT_FUNC, STT_SECTION = 2, 3
R_ARM_PC24, R_ARM_ABS32, R_ARM_THM_CALL, R_ARM_CALL, R_ARM_JUMP24 = 1, 2, 10, 28, 29
ARM_CALLS = (R_ARM_PC24, R_ARM_CALL, R_ARM_JUMP24)


@dataclass
class Reloc:
    offset: int        # In the function
    kind: int
    symbol: str | None  # None for a symbol without a name, such as a section
    addend: int


@dataclass
class ObjectFunction:
    library: str
    object: str        # Path of the object in its library, such as "os/libos.a.p/src_os_cache.c.o"
    index: int         # Place among the functions of the object
    name: str
    size: int
    masked: int        # The bytes, with those at relocations zeroed, as one integer
    mask: int
    fixed: int         # How many bytes are outside relocations
    relocs: list[Reloc]


@dataclass
class GameFunction:
    module: str
    name: str
    address: int
    size: int
    data: bytes
    line: int          # In the module's symbols.txt

    @property
    def end(self) -> int:
        return self.address + self.size


@dataclass
class Module:
    name: str
    base: int
    data: bytes
    symbols_path: Path
    lines: list[str] = field(default_factory=list)


def read_archive(data: bytes):
    """Yields (name, contents) for each member of an ar archive."""
    assert data[:8] == b"!<arch>\n"
    offset, long_names = 8, b""
    while offset + 60 <= len(data):
        header = data[offset:offset + 60]
        name, size = header[:16].decode("latin-1").rstrip(), int(header[48:58])
        body = data[offset + 60:offset + 60 + size]
        offset += 60 + size + (size & 1)
        if name == "//":
            long_names = body
        elif name in ("/", "/SYM64/"):
            continue
        else:
            if name.startswith("/") and name[1:].isdigit():
                start = int(name[1:])
                name = long_names[start:long_names.index(b"\n", start)].decode("latin-1")
            yield name.rstrip("/"), body


def read_object(data: bytes, library: str, path: str) -> list[ObjectFunction]:
    """Returns the functions of a relocatable ELF object for ARM."""
    if data[:4] != b"\x7fELF" or data[4] != 1 or data[5] != 1:
        return []
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, _ = struct.unpack_from("<3H", data, 0x2E)
    sections = [struct.unpack_from("<10I", data, shoff + i * shentsize) for i in range(shnum)]

    symbols = []  # (name, value, kind, section)
    for _, kind, _, _, offset, size, link, _, _, entsize in sections:
        if kind != SHT_SYMTAB:
            continue
        strings = sections[link][4]
        for i in range(size // entsize):
            name, value, _, info, _, shndx = struct.unpack_from("<3I2BH", data, offset + i * entsize)
            end = data.index(b"\0", strings + name)
            symbols.append((data[strings + name:end].decode("latin-1"), value, info & 0xF, shndx))

    relocs = collections.defaultdict(list)  # Section -> (offset, kind, symbol name, addend)
    for _, kind, _, _, offset, size, _, info, _, entsize in sections:
        if kind not in (SHT_REL, SHT_RELA) or not entsize:
            continue
        for i in range(size // entsize):
            if kind == SHT_RELA:
                where, word, addend = struct.unpack_from("<IIi", data, offset + i * entsize)
            else:
                (where, word), addend = struct.unpack_from("<II", data, offset + i * entsize), 0
            name, _, symbol_kind, _ = symbols[word >> 8]
            relocs[info].append((where, word & 0xFF, name if name and symbol_kind != STT_SECTION else None, addend))

    functions = []
    for index, (_, kind, flags, _, offset, size, _, _, _, _) in enumerate(sections):
        if kind != SHT_PROGBITS or not flags & SHF_EXECINSTR or not size:
            continue
        starts = sorted({(value & ~1, name) for name, value, symbol_kind, shndx in symbols
                         if shndx == index and symbol_kind == STT_FUNC and name and not name.startswith("$")})
        for i, (start, name) in enumerate(starts):
            end = starts[i + 1][0] if i + 1 < len(starts) else size
            if end <= start:
                continue
            code = bytearray(data[offset + start:offset + end])
            mask = bytearray(b"\xff" * len(code))
            own = []
            for where, reloc_kind, symbol, addend in relocs[index]:
                if start <= where < end:
                    mask[where - start:where - start + 4] = bytes(4)
                    own.append(Reloc(where - start, reloc_kind, symbol, addend))
            mask = mask[:len(code)]
            mask_int = int.from_bytes(mask, "little")
            functions.append(ObjectFunction(library, path, len(functions), name, len(code),
                                            int.from_bytes(code, "little") & mask_int, mask_int,
                                            sum(1 for b in mask if b), own))
    return functions


def load_library(name: str, path: Path) -> list[ObjectFunction]:
    functions = []
    if path.is_file():
        for member, body in read_archive(path.read_bytes()):
            functions += read_object(body, name, member)
    else:
        for obj in sorted(path.rglob("*.o")):
            functions += read_object(obj.read_bytes(), name, obj.relative_to(path).as_posix())
    return functions


def load_modules(version: str) -> list[Module]:
    config = ROOT / "config" / version / "arm9"
    extract = ROOT / "extract" / version
    modules = []
    for symbols_path in sorted(config.rglob("symbols.txt")):
        directory = symbols_path.parent
        name = "main" if directory == config else directory.name
        if name == "main":
            binary = extract / "arm9" / "arm9.bin"
        elif name.startswith("ov"):
            binary = extract / "arm9_overlays" / f"{name}.bin"
        else:
            binary = extract / "arm9" / f"{name}.bin"
        starts = re.findall(r"^\s+\.\w+\s+start:0x([0-9a-f]+)", (directory / "delinks.txt").read_text(), re.M)
        modules.append(Module(name, min(int(s, 16) for s in starts), binary.read_bytes(), symbols_path,
                              symbols_path.read_text(encoding="utf-8").splitlines()))
    return modules


FUNCTION = re.compile(r"^(\S+) kind:function\((arm|thumb),size=0x([0-9a-f]+)[^)]*\) addr:0x([0-9a-f]+)")
SYMBOL = re.compile(r"^(\S+) kind:(\w+).* addr:0x([0-9a-f]+)")


def game_functions(modules: list[Module]) -> list[GameFunction]:
    functions = []
    for module in modules:
        for number, line in enumerate(module.lines):
            match = FUNCTION.match(line)
            if match:
                address, size = int(match[4], 16), int(match[3], 16)
                start = address - module.base
                functions.append(GameFunction(module.name, match[1], address, size,
                                              module.data[start:start + size], number))
    return functions


def source_of(library: str, path: str, source_roots: dict[str, str]) -> str | None:
    """Returns the source file of an object, relative to its library, or None when the object does not say."""
    parts = Path(path).parts
    mangled = parts[-1].removesuffix(".o")
    if not mangled.endswith((".c", ".cpp")):
        return None  # A member of an archive is named after its object, not its source
    directory = Path(*parts[:-2]) if len(parts) > 2 else Path()
    root = source_roots.get(library)
    if root:
        base = Path(root).expanduser() / directory
        for candidate in base.rglob(f"*{Path(mangled).suffix}"):
            relative = candidate.relative_to(base).as_posix()
            if relative.replace("/", "_") == mangled:
                return (directory / relative.removeprefix("src/")).as_posix()
    return (directory / mangled.removeprefix("src_")).as_posix()


def unnamed(name: str) -> bool:
    return name.startswith(("func_", "data_"))


def target_of(function: GameFunction, reloc: Reloc) -> int | None:
    """Returns the address the game's code refers to at a relocation, or None for a kind that is not decoded."""
    if reloc.offset + 4 > len(function.data):
        return None
    word, = struct.unpack_from("<I", function.data, reloc.offset)
    if reloc.kind in ARM_CALLS:
        offset = (word & 0xFFFFFF) << 2
        if offset & 0x2000000:
            offset -= 0x4000000
        target = function.address + reloc.offset + 8 + offset
        if word >> 25 == 0x7D:  # blx: bit 24 is half a word more, and the target is Thumb
            target += (word >> 23) & 2
        return target
    if reloc.kind == R_ARM_ABS32:
        return (word - reloc.addend) & 0xFFFFFFFF
    return None


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("libraries", nargs="+", metavar="NAME=PATH")
    parser.add_argument("--sources", action="append", default=[], metavar="NAME=PATH",
                        help="directory of a library's sources, laid out as its objects are")
    parser.add_argument("--version", default="jpn_rev1")
    parser.add_argument("--apply", action="store_true", help="rename the matched symbols that have no name yet")
    parser.add_argument("--units", action="store_true", help="with --apply, also cut each object out as a unit")
    parser.add_argument("--verbose", action="store_true", help="list every match")
    args = parser.parse_args()

    objects: list[ObjectFunction] = []
    for spec in args.libraries:
        name, _, path = spec.partition("=")
        loaded = load_library(name, Path(path).expanduser())
        print(f"{name}: {len(loaded)} functions in {len({f.object for f in loaded})} objects")
        objects += loaded

    modules = load_modules(args.version)
    game = game_functions(modules)
    by_size = collections.defaultdict(list)
    for function in game:
        by_size[function.size].append(function)
    by_address = {(f.module, f.address): f for f in game}
    shared = {f.address: f for f in game if not f.module.startswith("ov")}  # Main program and autoloads

    # Every game function whose bytes fit each object function
    fits: dict[int, list[GameFunction]] = {}
    for i, function in enumerate(objects):
        fits[i] = [g for g in by_size.get(function.size, ())
                   if int.from_bytes(g.data, "little") & function.mask == function.masked]
    names_fitting = collections.defaultdict(set)  # Game function -> names of the object functions that fit it
    for i, candidates in fits.items():
        for g in candidates:
            names_fitting[(g.module, g.address)].add(objects[i].name)

    accepted: dict[tuple[str, int], tuple[ObjectFunction, str]] = {}
    taken: dict[int, GameFunction] = {}  # Object function -> game function

    def accept(i: int, g: GameFunction, ground: str) -> bool:
        key = (g.module, g.address)
        if key in accepted or i in taken:
            return False
        accepted[key] = (objects[i], ground)
        taken[i] = g
        return True

    for i, candidates in fits.items():
        if (len(candidates) == 1 and objects[i].fixed >= UNIQUE_BYTES
                and len(names_fitting[(candidates[0].module, candidates[0].address)]) == 1):
            accept(i, candidates[0], "unique")

    by_name = collections.defaultdict(list)
    for i, function in enumerate(objects):
        by_name[function.name].append(i)
    by_object = collections.defaultdict(list)
    for i, function in enumerate(objects):
        by_object[(function.library, function.object)].append(i)

    changed = True
    while changed:
        changed = False
        # reference: what accepted functions refer to
        for i, g in list(taken.items()):
            for reloc in objects[i].relocs:
                target = target_of(g, reloc)
                if reloc.symbol is None or target is None:
                    continue
                callee = by_address.get((g.module, target)) if g.module.startswith("ov") else None
                callee = callee or shared.get(target)
                if callee is None:
                    continue
                for j in by_name.get(reloc.symbol, ()):
                    if callee in fits[j] and accept(j, callee, "reference"):
                        changed = True
                        break
        # order: between two accepted functions of the same object
        for members in by_object.values():
            placed = [(k, taken[k]) for k in members if k in taken]
            for j in members:
                if j in taken:
                    continue
                before = [g for k, g in placed if k < j]
                after = [g for k, g in placed if k > j]
                if not before or not after or before[-1].module != after[0].module:
                    continue
                low, high, module = before[-1].end, after[0].address, before[-1].module
                between = [g for g in fits[j] if g.module == module and low <= g.address < high
                           and (g.module, g.address) not in accepted]
                if len(between) == 1 and accept(j, between[0], "order"):
                    changed = True

    # An object whose functions are not in order in the game was matched wrongly somewhere: drop all of it
    dropped = []
    for (library, path), members in by_object.items():
        placed = [taken[k] for k in members if k in taken]
        per_module = collections.defaultdict(list)
        for g in placed:
            per_module[g.module].append(g.address)
        if any(addresses != sorted(addresses) for addresses in per_module.values()):
            dropped.append(f"{library}/{path}")
            for k in members:
                g = taken.pop(k, None)
                if g:
                    del accepted[(g.module, g.address)]

    # Report
    grounds = collections.Counter(ground for _, ground in accepted.values())
    per_library = collections.Counter(function.library for function, _ in accepted.values())
    code = sum(by_address[key].size for key in accepted)
    print(f"\nMatched {len(accepted)} functions, {code} bytes of code: "
          + ", ".join(f"{count} by {ground}" for ground, count in grounds.most_common()))
    for library, count in per_library.most_common():
        print(f"  {library}: {count}")
    if dropped:
        print(f"Dropped for being out of order: {', '.join(dropped)}")
    agree = disagree = 0
    for key, (function, _) in sorted(accepted.items()):
        current = by_address[key].name
        if not unnamed(current):
            if current == function.name:
                agree += 1
            else:
                disagree += 1
                print(f"  {key[0]} {key[1]:#010x} is named {current}, matched {function.name}")
    print(f"Of the functions that already had a name, {agree} agree and {disagree} differ")
    if args.verbose:
        for key, (function, ground) in sorted(accepted.items()):
            print(f"  {key[0]:6} {key[1]:#010x} {function.name:40} {ground:9} {function.library}/{function.object}")

    # A name can only go to one address of a module, or the linker would see it twice
    new_names = collections.defaultdict(list)
    for key, (function, _) in accepted.items():
        if unnamed(by_address[key].name):
            new_names[function.name].append(key)
    existing = {match[1] for module in modules for line in module.lines if (match := SYMBOL.match(line))}
    renames = {keys[0]: name for name, keys in new_names.items() if len(keys) == 1 and name not in existing}
    skipped = sum(len(keys) for name, keys in new_names.items() if len(keys) > 1 or name in existing)
    print(f"{len(renames)} symbols to rename, {skipped} left alone because their name would not be unique")

    # Units: the range of each object's matched functions in the main program
    source_roots = dict(spec.split("=", 1) for spec in args.sources)
    object_sizes = collections.Counter((f.library, f.object) for f in objects)
    main_functions = sorted(f.address for f in game if f.module == "main")
    ranges = collections.defaultdict(list)
    for key in sorted(key for key in accepted if key[0] == "main"):
        function, _ = accepted[key]
        ranges[(function.library, function.object)].append(by_address[key])
    units, doubtful = [], []
    for (library, path), placed in ranges.items():
        source = source_of(library, path, source_roots)
        if source is None:
            continue
        # A Thumb function can end between words. The padding up to the next word belongs to its file.
        start, end = min(g.address for g in placed), (max(g.end for g in placed) + 3) & ~3
        # The range cannot hold more functions than the object has. If it does, it spans something else.
        if sum(1 for address in main_functions if start <= address < end) > object_sizes[(library, path)]:
            doubtful.append(f"{library}/{path}")
            continue
        units.append((start, end, (Path("lib") / library / source).as_posix(), len(placed)))
    units.sort()
    clean = [unit for i, unit in enumerate(units)
             if (i == 0 or units[i - 1][1] <= unit[0]) and (i + 1 == len(units) or unit[1] <= units[i + 1][0])]
    print(f"{len(clean)} objects have a range of their own in the main program, "
          f"{len(units) - len(clean)} overlap another and are left in the gap")
    if doubtful:
        print(f"Left in the gap for holding more functions than the object has: {', '.join(doubtful)}")

    if not args.apply:
        return 0

    for module in modules:
        for function in game:
            name = renames.get((function.module, function.address))
            if name and function.module == module.name:
                module.lines[function.line] = name + module.lines[function.line][len(function.name):]
        module.symbols_path.write_text("\n".join(module.lines) + "\n", encoding="utf-8")
    print(f"Renamed {len(renames)} symbols")

    if args.units:
        delinks = ROOT / "config" / args.version / "arm9" / "delinks.txt"
        text = delinks.read_text(encoding="utf-8").rstrip("\n")
        claimed = {match[1] for match in re.finditer(r"^(\S+):$", text, re.M)}
        added = 0
        for start, end, unit, _ in clean:
            if unit not in claimed:
                text += f"\n\n{unit}:\n    .text       start:{start:#010x} end:{end:#010x}"
                added += 1
        delinks.write_text(text + "\n", encoding="utf-8")
        print(f"Added {added} units to {delinks.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
