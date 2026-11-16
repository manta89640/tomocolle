#!/usr/bin/env python3
"""Lists the gap units of the ITCM that dsd does not delink, because files of other modules claim all of its code.

A library file whose code is partly in the ITCM (the SDK's interrupt handler and DMA setters, in os_irqHandler.c and
mi_dma.c) is one unit of the main program: its entry in the main program's delinks.txt names an `.itcm` range as
well, and dsd moves that range out of the ITCM module into the file's object when it delinks and links ("section
migration"). `dsd json delinks` and `dsd objdiff` do not migrate, so they still list the ITCM's gap as it was before,
an object that `dsd delink` no longer writes. This finds such gaps, for configure.py and objdiff_config.py to leave
out. A gap that a migration covers only in part is left in, with a warning: dsd would number what is left of it
differently, and nothing here needs that yet.
"""
import re
import sys
from pathlib import Path

SECTION = re.compile(r"^\s+(\.\w+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)", re.M)


def module_delinks(config_path: Path) -> dict[str, Path]:
    """Returns each module's name and delinks.txt, read from config.yaml (only the keys this needs)."""
    modules, name = {}, None
    for line in config_path.read_text(encoding="utf-8").splitlines():
        match = re.match(r"^\s*(?:- )?name:\s*(\S+)", line)
        if match:
            name = match[1]
        match = re.match(r"^\s*delinks:\s*(\S+)", line)
        if match and name:
            modules[name] = config_path.parent / match[1]
    return modules


def sections(text: str) -> tuple[list[tuple[str, int, int]], list[tuple[str, int, int]]]:
    """Returns a delinks.txt's module sections (before the first file) and its files' sections."""
    head, _, files = text.partition("\n\n")
    parse = lambda part: [(m[1], int(m[2], 16), int(m[3], 16)) for m in SECTION.finditer(part)]
    return parse(head), parse(files)


def migrated_gaps(config_path: Path) -> set[str]:
    modules = module_delinks(config_path)
    if "itcm" not in modules:
        return set()
    own, own_files = sections(modules["itcm"].read_text(encoding="utf-8"))
    claimed = sorted((start, end) for module, path in modules.items() if module != "itcm"
                     for name, start, end in sections(path.read_text(encoding="utf-8"))[1] if name == ".itcm")
    if not claimed:
        return set()
    # The code of the ITCM, less what its own files claim
    code = [(start, end) for name, start, end in own if name == ".text" and end > start]
    covered = all(any(s <= start and end <= e for s, e in merge(claimed + [(s, e) for _, s, e in own_files]))
                  for start, end in code)
    if not covered:
        print("warning: the ITCM's code is claimed only in part by .itcm ranges of other modules; its gap is kept",
              file=sys.stderr)
        return set()
    return {"_dsd_gap@itcm_0"} | {f"_dsd_gap@itcm_{i}" for i in range(1, len(own) + 1)}


def merge(ranges: list[tuple[int, int]]) -> list[tuple[int, int]]:
    merged = []
    for start, end in sorted(ranges):
        if merged and start <= merged[-1][1]:
            merged[-1] = (merged[-1][0], max(merged[-1][1], end))
        else:
            merged.append((start, end))
    return merged


if __name__ == "__main__":
    print("\n".join(sorted(migrated_gaps(Path(sys.argv[1])))))
