#!/usr/bin/env python3
"""Checks that the functions of the compiled objects refer to the same symbols as the original's.

objdiff calls a function matched when its instructions are equal, and a call or a load of an address is equal to it
whatever it refers to: a source that calls the wrong function, or one by a name that symbols.txt does not have, still
scores 100%. This compares, for every function that is in both the compiled object and the delinked original, the
symbol each relocation refers to. A unit that is linked from source is checked by the ROM as well; one that is not is
checked by this alone.

A relocation against a section (a static object, a string) is compared by the symbol at that place of the section
when there is one, and skipped when there is not.

With --near-misses it lists the opposite error instead: functions that objdiff's report scores below 100% although
their bytes outside relocations and the symbols their relocations refer to are the original's. objdiff turns a
relocation into a branch within the function when the symbol it refers to, plus the addend, falls inside the
function; for an ARM call the addend is the PC's -8, so a call to the function that starts within 8 bytes after the
caller's end (the next function of the section) is taken for a branch into the caller's last two instructions. When
that holds in one object and not in the other, the call scores as a wrong argument (99.5% for a function of ten
instructions). See docs/decompiling.md, "A match is instructions and references".
"""
import argparse
import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from place_data import Object, STT_FUNC, STT_SECTION  # noqa: E402

ROOT = Path(__file__).parent.parent.resolve()


def functions(obj: Object) -> dict:
    """Returns name -> (section, start, end) for the functions an object defines."""
    found = {}
    for name, value, size, kind, _, shndx in obj.symbols:
        if kind == STT_FUNC and name and not name.startswith("$") and 0 < shndx < len(obj.sections) and size:
            found[name] = (shndx, value & ~1, (value & ~1) + size)
    return found


def targets(obj: Object, section: int, start: int, end: int) -> dict:
    """Returns offset -> name of what the relocation there refers to (None when it cannot be named)."""
    named = {}
    for name, value, _, kind, _, shndx in obj.symbols:
        if name and not name.startswith("$") and kind != STT_SECTION and 0 < shndx < len(obj.sections):
            named.setdefault((shndx, value & ~1), name)
    data = obj.bytes_of(section)
    result = {}
    for where, _, index, addend in obj.relocs.get(section, ()):
        if not start <= where < end:
            continue
        name, value, _, kind, _, shndx = obj.symbols[index]
        if kind == STT_SECTION or not name:
            # The place within the section is in the word itself (REL) or the addend (RELA)
            word = int.from_bytes(data[where:where + 4], "little") if where + 4 <= len(data) else 0
            name = named.get((shndx, (addend or word) & ~1))
        result[where - start] = name
    return result


def masked(obj: Object, section: int, start: int, end: int) -> bytes:
    """Returns the bytes of a function with the words that relocations fill in zeroed."""
    code = bytearray(obj.bytes_of(section)[start:end])
    for where, _, _, _ in obj.relocs.get(section, ()):
        if start <= where < end:
            code[where - start:where - start + 4] = bytes(min(4, end - where))
    return bytes(code)


def near_misses(units: list, report: dict) -> int:
    """Lists the functions scored below 100% that equal the original's in bytes and references."""
    scores = {(u["name"], f["name"]): float(f.get("fuzzy_match_percent", 0))
              for u in report["units"] for f in u.get("functions", [])}
    found = 0
    for unit in units:
        target, base = unit.get("target_path"), unit.get("base_path")
        if not target or not base or not (ROOT / target).exists() or not (ROOT / base).exists():
            continue
        original, compiled = Object(ROOT / target), Object(ROOT / base)
        theirs, ours = functions(original), functions(compiled)
        for name in sorted(set(theirs) & set(ours)):
            score = scores.get((unit["name"], name))
            if score is None or score >= 100:
                continue
            t_section, t_start, t_end = theirs[name]
            o_section, o_start, o_end = ours[name]
            if t_end - t_start != o_end - o_start:
                continue
            # Both sides' relocation sites are masked in both, so a site only one side has is not a difference
            t_code = bytearray(masked(original, t_section, t_start, t_end))
            o_code = bytearray(masked(compiled, o_section, o_start, o_end))
            for code, obj, section, start, end in ((o_code, original, t_section, t_start, t_end),
                                                   (t_code, compiled, o_section, o_start, o_end)):
                for where, _, _, _ in obj.relocs.get(section, ()):
                    if start <= where < end:
                        code[where - start:where - start + 4] = bytes(min(4, end - where))
            if t_code != o_code:
                continue
            t_targets = targets(original, t_section, t_start, t_end)
            o_targets = targets(compiled, o_section, o_start, o_end)
            if any(t_targets.get(o) is not None and o_targets.get(o) is not None and t_targets[o] != o_targets[o]
                   for o in set(t_targets) | set(o_targets)):
                continue
            found += 1
            print(f"{unit['name']}: {name} scores {score}% but equals the original")
    print(f"{found} functions score below 100% and equal the original in bytes and references")
    return 0


def script_symbols() -> tuple[dict, dict]:
    """The symbols the linker script defines (the SDK's SDK_* and the like), name -> address, and the addresses of
    every symbol of the linked program, name -> set. The original refers to such an address by the name symbols.txt
    has there (SDK_AUTOLOAD_DTCM_START is OS_IRQTable's address), and that is the same reference."""
    lcf, elf = ROOT / "build/jpn_rev1/arm9_link.lcf", ROOT / "build/jpn_rev1/arm9.o"
    if not lcf.exists() or not elf.exists():
        return {}, {}
    names = set(re.findall(r"^\s+([A-Za-z_]\w*) = [^;]+;$", lcf.read_text(encoding="utf-8"), re.M))
    addresses = {}
    for name, value, _, _, _, _ in Object(elf).symbols:
        if name:
            addresses.setdefault(name, set()).add(value & ~1)
    return {n: min(addresses[n]) for n in names if n in addresses}, addresses


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--unit", help="only units whose name contains this")
    parser.add_argument("--verbose", action="store_true", help="list every difference")
    parser.add_argument("--near-misses", action="store_true",
                        help="list the functions the report scores below 100%% that equal the original")
    parser.add_argument("--pair", nargs=2, type=Path, metavar=("ORIGINAL", "COMPILED"),
                        help="compare these two objects instead of the units of objdiff.json")
    args = parser.parse_args()

    if args.pair:
        units = [{"name": args.pair[1].name, "target_path": args.pair[0].resolve(),
                  "base_path": args.pair[1].resolve()}]
    else:
        units = json.loads((ROOT / "objdiff.json").read_text())["units"]
    if args.near_misses:
        report = json.loads((ROOT / "build" / "jpn_rev1" / "report.json").read_text())
        return near_misses([u for u in units if not args.unit or args.unit in u["name"]], report)
    script, addresses = script_symbols()
    bad_units = 0
    for unit in units:
        target, base = unit.get("target_path"), unit.get("base_path")
        if not target or not base or (args.unit and args.unit not in unit["name"]):
            continue
        if not (ROOT / target).exists() or not (ROOT / base).exists():
            continue
        original, compiled = Object(ROOT / target), Object(ROOT / base)
        theirs, ours = functions(original), functions(compiled)
        differences = []
        for name in sorted(set(theirs) & set(ours)):
            t_section, t_start, t_end = theirs[name]
            o_section, o_start, o_end = ours[name]
            if t_end - t_start != o_end - o_start:
                continue  # Not matching anyway
            t_targets = targets(original, t_section, t_start, t_end)
            o_targets = targets(compiled, o_section, o_start, o_end)
            for offset in sorted(set(t_targets) | set(o_targets)):
                a, b = t_targets.get(offset), o_targets.get(offset)
                if a is None or b is None or a == b:
                    continue
                if b in script and script[b] in addresses.get(a, ()):
                    continue  # A symbol of the linker script, at the address the original's symbol has
                differences.append((name, offset, a, b))
        if differences:
            bad_units += 1
            pairs = sorted({(a, b) for _, _, a, b in differences})
            print(f"{unit['name']}: {len(differences)} references differ, {len(pairs)} distinct")
            for a, b in pairs if not args.verbose else ():
                print(f"    original {a}, compiled {b}")
            for name, offset, a, b in differences if args.verbose else ():
                print(f"    {name}+{offset:#x}: original {a}, compiled {b}")
    if bad_units:
        print(f"{bad_units} units refer to other symbols than the original does")
        return 1
    print("Every reference of the compiled functions is to the symbol the original refers to")
    return 0


if __name__ == "__main__":
    sys.exit(main())
