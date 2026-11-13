#!/usr/bin/env python3
"""Marks the units complete whose every function matches, and keeps each only if the ROM still matches with it.

A complete unit is linked from its compiled object instead of the original's bytes. Whether that works is not decided
here but by the build: this marks a batch of candidates in delinks.txt, runs configure.py and ninja, and when the
checks fail, halves the batch until it finds the unit that breaks them, and takes that one back.

A candidate is a unit of the progress report (`ninja report`) with a source file, all of whose functions match. A
unit's delinks.txt entry has to name every section its object puts in the ROM. This does not work those out: a unit
whose object has data that the entry does not name is left for when its entry does.
"""
import argparse
import json
import re
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).parent.parent.resolve()
SHT_NOBITS = 8
SHF_ALLOC, SHF_EXECINSTR = 2, 4


def data_sections(path: Path) -> int:
    """Returns how many bytes of an object's sections are data that would go in the ROM or its memory."""
    data = path.read_bytes()
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, _ = struct.unpack_from("<3H", data, 0x2E)
    total = 0
    for i in range(shnum):
        _, _, flags, _, _, size, _, _, _, _ = struct.unpack_from("<10I", data, shoff + i * shentsize)
        if flags & SHF_ALLOC and not flags & SHF_EXECINSTR:
            total += size
    return total


def entry(text: str, source: str) -> re.Match | None:
    """Finds a unit's entry in delinks.txt: its name, and the indented lines below it."""
    return re.search(rf"^{re.escape(source)}:\n((?:[ \t]+.*\n?)*)", text, re.M)


def set_complete(delinks: Path, sources: list[str], complete: bool):
    text = delinks.read_text(encoding="utf-8")
    for source in sources:
        match = entry(text, source)
        body = "".join(line for line in match[1].splitlines(keepends=True) if line.strip() != "complete")
        if complete:
            body = "    complete\n" + body
        text = text[:match.start(1)] + body + text[match.end(1):]
    delinks.write_text(text, encoding="utf-8")


def builds() -> bool:
    for command in ([sys.executable, "configure.py"], ["ninja"]):
        if subprocess.run(command, cwd=ROOT, capture_output=True).returncode != 0:
            return False
    return True


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="jpn_rev1")
    parser.add_argument("--batch", type=int, default=16, help="how many units to try at once")
    args = parser.parse_args()

    build = ROOT / "build" / args.version
    delinks = ROOT / "config" / args.version / "arm9" / "delinks.txt"
    report = json.loads((build / "report.json").read_text())
    text = delinks.read_text(encoding="utf-8")

    candidates, with_data = [], []
    for unit in report["units"]:
        metadata, measures = unit.get("metadata", {}), unit.get("measures", {})
        source = metadata.get("source_path")
        if not source or metadata.get("complete") or not entry(text, source):
            continue
        if measures.get("matched_functions", 0) != measures.get("total_functions", 0):
            continue
        named = {line.split()[0] for line in entry(text, source)[1].splitlines() if line.strip()}
        obj = build / Path(source).with_suffix(".o")
        if data_sections(obj) and named <= {".text"}:
            with_data.append(source)
        else:
            candidates.append(source)
    print(f"{len(candidates)} units to try, {len(with_data)} left because their object has data that "
          "delinks.txt does not place")

    if not builds():
        print("The build does not pass as it is: fix that first", file=sys.stderr)
        return 1

    kept, rejected = [], []

    def attempt(batch: list[str]):
        """Marks a batch complete, and narrows it down when the build no longer passes."""
        set_complete(delinks, batch, True)
        if builds():
            kept.extend(batch)
            print(f"  kept {len(batch)}: {batch[0]}{' ...' if len(batch) > 1 else ''}", flush=True)
            return
        set_complete(delinks, batch, False)
        if len(batch) == 1:
            rejected.extend(batch)
            print(f"  rejected {batch[0]}", flush=True)
            return
        half = len(batch) // 2
        attempt(batch[:half])
        attempt(batch[half:])

    for start in range(0, len(candidates), args.batch):
        attempt(candidates[start:start + args.batch])

    if not builds():
        print("The build does not pass with the units that were kept", file=sys.stderr)
        return 1
    print(f"{len(kept)} units are now complete, {len(rejected)} would break the ROM:")
    for source in rejected:
        print(f"  {source}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
