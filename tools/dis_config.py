#!/usr/bin/env python3
"""Writes a copy of the dsd config for `dsd dis`, which does not migrate sections.

A file whose code is partly in another module (os_irqHandler.c and mi_dma.c, whose `.itcm` ranges sit in the main
program's delinks.txt: see tools/migrated_gaps.py) makes `dsd dis` stop with "section .itcm starts at 0x01ff8000
before base address 0x02000000". This copies the config to a directory of its own, leaves out each file section that
lies outside its module's sections, and makes the config's relative paths absolute so that the copy finds the
extracted ROM and the build. The listing then shows that code where it is, in its own module.
"""
import argparse
import re
import shutil
from pathlib import Path

from migrated_gaps import SECTION, module_delinks, sections


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("config", type=Path, help="the config.yaml of arm9")
    parser.add_argument("out_dir", type=Path, help="where to write the copy")
    args = parser.parse_args()

    source_dir = args.config.parent.resolve()
    if args.out_dir.exists():
        shutil.rmtree(args.out_dir)
    shutil.copytree(source_dir, args.out_dir)

    # Paths that leave the config directory, made absolute
    config_text = args.config.read_text(encoding="utf-8")
    config_text = re.sub(r"(:\s*)(\.\./\S+)", lambda m: m[1] + str((source_dir / m[2]).resolve()), config_text)
    (args.out_dir / args.config.name).write_text(config_text, encoding="utf-8")

    for delinks in module_delinks(args.config).values():
        text = delinks.read_text(encoding="utf-8")
        module, _ = sections(text)
        inside = lambda start, end: any(lo <= start and end <= hi for _, lo, hi in module)
        head, sep, files = text.partition("\n\n")
        kept = [line for line in files.split("\n")
                if not (m := SECTION.match(line)) or inside(int(m[2], 16), int(m[3], 16))]
        copy = args.out_dir / delinks.resolve().relative_to(source_dir)
        copy.write_text(head + sep + "\n".join(kept), encoding="utf-8")


if __name__ == "__main__":
    main()
