#!/usr/bin/env python3
"""Checks that the functions of the compiled objects refer to the same symbols as the original's.

objdiff calls a function matched when its instructions are equal, and a call or a load of an address is equal to it
whatever it refers to: a source that calls the wrong function, or one by a name that symbols.txt does not have, still
scores 100%. This compares, for every function that is in both the compiled object and the delinked original, the
symbol each relocation refers to. A unit that is linked from source is checked by the ROM as well; one that is not is
checked by this alone.

A relocation against a section (a static object, a string) is compared by the symbol at that place of the section
when there is one, and skipped when there is not.
"""
import argparse
import json
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


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--unit", help="only units whose name contains this")
    parser.add_argument("--verbose", action="store_true", help="list every difference")
    args = parser.parse_args()

    config = json.loads((ROOT / "objdiff.json").read_text())
    bad_units = 0
    for unit in config["units"]:
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
