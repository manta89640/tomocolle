#!/usr/bin/env python3
"""Writes objdiff.json from the dsd config, shaped for objdiff's GUI and for ByteMatch's unit map.

dsd writes the units. This adds what a progress tracker reads from them:
  - short paths, relative to the repository's root
  - a path-like name for each unit, since the tracker draws a name's directories as nested boxes
  - progress categories: each unit's kind of module, whether it is the game's own code or a library's (for a gap,
    by where it is), and which library or which of the game's subsystems
  - the source file of a decompiled unit, for links to the code

A delinks.txt may name a source file that is not written yet, to give a known file a unit of its own. Such a unit has
nothing to compare with, so it gets no object to build and no source to link to.
"""
import argparse
import json
import os
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from place_data import Object, STT_FUNC  # noqa: E402

# What is left of a module once its decompiled files are taken out, which dsd delinks as "_dsd_gap@<module>_<n>"
GAP = re.compile(r"^_dsd_gap@(?P<module>.+)_(?P<index>\d+)$")

CATEGORIES = [
    # By module
    {"id": "main", "name": "Main program"},
    {"id": "itcm", "name": "ITCM"},
    {"id": "overlays", "name": "Overlays"},
    # By origin: all of the game's own code, and all of the libraries', whether or not it is in files yet
    {"id": "game", "name": "Game"},
    {"id": "libs", "name": "Libraries"},
]
ORIGINS = {"src": "game", "lib": "libs"}

# What no file claims yet is the gaps. Measured (docs/binary.md, "Libraries"): the main program's code holds the
# libraries in these ranges, and the game's own code everywhere else, along with the libraries not found yet (WH, NFL
# and a few files of the SDK). The ITCM's six functions are all NitroSDK's, and the overlays are the game's scenes.
# So a gap's code counts toward "game" or "libs" by the address of its first function.
LIBRARY_RANGES = [
    (0x02000000, 0x0201F4DC),  # NitroSDK, then MSL's C library
    (0x02053C64, 0x02069BF4),  # NitroSystem
]

# Finer categories, by the directory under lib/ or src/ that a file is in. A file of a directory that is not listed
# counts only toward its origin and module (src/overlays/ is not listed: the module category already covers it).
LIBRARIES = {
    "NitroSDK": ("nitrosdk", "NitroSDK"),
    "NitroSystem": ("nitrosystem", "NitroSystem"),
    "SHTTS": ("shtts", "Speech synthesis (SHTTS)"),
    "MSL_C": ("msl", "C library (MSL)"),
}
SUBSYSTEMS = {
    "sound": ("sound", "Sound"),
    "text": ("text", "Text"),
    "voice": ("voice", "Speech player"),
}
CATEGORIES += [{"id": id, "name": name} for id, name in [*LIBRARIES.values(), *SUBSYSTEMS.values()]]


def clean(path: str) -> str:
    return Path(os.path.normpath(path)).as_posix()


def source_modules(config: Path) -> dict[str, str]:
    """Maps each source file in the delinks.txt files to its module, such as "main" or "ov075"."""
    modules = {}
    arm9_dir = config.parent
    for delinks in arm9_dir.rglob("delinks.txt"):
        relative = delinks.parent.relative_to(arm9_dir).parts
        module = "main" if not relative else relative[-1]
        for line in delinks.read_text(encoding="utf-8").splitlines():
            # A file's entry starts at the first column and ends with a colon, and its sections are indented below it
            if line and not line[0].isspace() and line.rstrip().endswith(":"):
                modules[line.rstrip()[:-1].split()[0]] = module
    return modules


def symbol_addresses(config: Path) -> dict[str, dict[str, int]]:
    """Maps each module ("main", "itcm", "ov075", ...) to its symbols.txt's names and addresses."""
    found = {}
    for symbols in config.parent.rglob("symbols.txt"):
        relative = symbols.parent.relative_to(config.parent).parts
        module = "main" if not relative else relative[-1]
        table = found.setdefault(module, {})
        for line in symbols.read_text(encoding="utf-8").splitlines():
            match = re.match(r"(\S+) kind:function\S* addr:(0x[0-9a-fA-F]+)", line)
            if match:
                table.setdefault(match[1], int(match[2], 16))
    return found


def first_function(path: Path, addresses: dict[str, int]) -> int | None:
    """The address of the lowest function of a delinked object, or None if it has none (or is not delinked yet)."""
    if not path.exists():
        return None
    found = [addresses[name] for name, _, _, kind, _, _ in Object(path).symbols if kind == STT_FUNC and name in addresses]
    return min(found) if found else None


def gap_origin(module: str, address: int | None) -> str | None:
    if module.startswith("ov"):
        return "game"
    if module == "itcm":
        return "libs"
    if module == "main" and address is not None:
        return "libs" if any(start <= address < end for start, end in LIBRARY_RANGES) else "game"
    return None  # No code, so no telling


def category(module: str) -> str:
    if module.startswith("ov"):
        return "overlays"
    return module if module in ("main", "itcm") else "main"


def finer_category(source: str) -> str | None:
    """The library or subsystem category of a source file, from the directory under lib/ or src/ it is in."""
    parts = Path(source).parts
    if len(parts) < 3:
        return None
    table = {"lib": LIBRARIES, "src": SUBSYSTEMS}.get(parts[0], {})
    entry = table.get(parts[1])
    return entry[0] if entry else None


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--dsd", required=True, help="path to dsd")
    parser.add_argument("--config", type=Path, required=True, help="path to the dsd config.yaml")
    parser.add_argument("--compiler", required=True, help="decomp.me compiler name")
    parser.add_argument("--c-flags", required=True, help="compiler flags for decomp.me scratches")
    parser.add_argument("-o", "--output", type=Path, required=True)
    args = parser.parse_args()

    result = subprocess.run([
        args.dsd, "objdiff", "--config-path", str(args.config), "--stdout", "--scratch",
        "--compiler", args.compiler, "--c-flags", args.c_flags, "--custom-make", "ninja",
    ], capture_output=True, text=True)
    if result.returncode != 0:
        print(result.stderr, file=sys.stderr)
        return result.returncode
    objdiff = json.loads(result.stdout)

    for key in ("target_dir", "base_dir"):
        if key in objdiff:
            objdiff[key] = clean(objdiff[key])
    objdiff["progress_categories"] = CATEGORIES

    modules = source_modules(args.config)
    addresses = symbol_addresses(args.config)
    for unit in objdiff["units"]:
        for key in ("target_path", "base_path"):
            if unit.get(key):
                unit[key] = clean(unit[key])
        scratch = unit.get("scratch")
        if scratch and scratch.get("ctx_path"):
            scratch["ctx_path"] = clean(scratch["ctx_path"])
        metadata = unit.setdefault("metadata", {})
        name = unit["name"]
        gap = GAP.match(name)
        if gap:
            module = gap["module"]
            folder = f"overlays/{module}" if module.startswith("ov") else module
            unit["name"] = f"{folder}/gap_{gap['index']}"
            categories = [category(module)]
            target = unit.get("target_path")
            origin = gap_origin(module, first_function(Path(target), addresses.get(module, {})) if target else None)
            if origin:
                categories.append(origin)
        else:
            # dsd names a unit after its source file, less the extension, and gives the file's path
            source = metadata.get("source_path", name)
            module = modules.get(source, "main")
            categories = [category(module)]
            origin = ORIGINS.get(Path(source).parts[0])
            if origin:
                categories.append(origin)
            finer = finer_category(source)
            if finer:
                categories.append(finer)
            if origin == "libs":
                # A scratch is for writing code, and it would be set up with the game's compiler, not the library's
                unit.pop("scratch", None)
            if not Path(source).exists():
                for key in ("base_path", "scratch"):
                    unit.pop(key, None)
                metadata.pop("source_path", None)
        metadata["progress_categories"] = categories

    args.output.write_text(json.dumps(objdiff, indent=2) + "\n", encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main())
