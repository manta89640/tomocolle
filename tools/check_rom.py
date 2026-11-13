#!/usr/bin/env python3
"""Checks a base ROM against its SHA-1, or a built ROM against the base ROM.

A built ROM is identical to the base ROM only when the ARM7 BIOS was there to build it with: the header holds a
checksum of the encrypted secure area, and the BIOS holds the key. Without it dsd leaves that checksum zero, which
also changes the header's own checksum. Those four bytes are then the only difference allowed.
"""
import argparse
import hashlib
import sys
from pathlib import Path

SECURE_AREA_CRC = range(0x6C, 0x6E)
HEADER_CRC = range(0x15E, 0x160)


def sha1(data: bytes) -> str:
    return hashlib.sha1(data).hexdigest()


def stamp(path: Path | None):
    if path:
        path.parent.mkdir(parents=True, exist_ok=True)
        path.touch()


def check_base(args) -> int:
    actual = sha1(args.rom.read_bytes())
    if actual != args.sha1:
        print(f"{args.rom}: SHA-1 is {actual}, expected {args.sha1}", file=sys.stderr)
        print("This is not the ROM this version of the project is for, or it is a bad dump.", file=sys.stderr)
        return 1
    stamp(args.stamp)
    return 0


def check_built(args) -> int:
    built = args.rom.read_bytes()
    base = args.base.read_bytes()
    if built == base:
        print(f"{args.rom}: OK, identical to {args.base} (SHA-1 {sha1(built)})")
        stamp(args.stamp)
        return 0
    if len(built) != len(base):
        print(f"{args.rom}: {len(built):#x} bytes, the base ROM is {len(base):#x}", file=sys.stderr)
        return 1
    different = [i for i in range(len(base)) if built[i] != base[i]]
    allowed = {*SECURE_AREA_CRC, *HEADER_CRC}
    if set(different) <= allowed and built[SECURE_AREA_CRC.start:SECURE_AREA_CRC.stop] == b"\0\0":
        print(f"{args.rom}: OK, identical to {args.base} except for the secure area checksum, "
              "which needs arm7_bios.bin")
        stamp(args.stamp)
        return 0
    print(f"{args.rom}: {len(different)} bytes differ from {args.base}, the first at {different[0]:#x}",
          file=sys.stderr)
    return 1


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    commands = parser.add_subparsers(dest="command", required=True)
    base = commands.add_parser("base", help="check a base ROM against its SHA-1")
    base.add_argument("rom", type=Path)
    base.add_argument("--sha1", required=True)
    base.add_argument("--stamp", type=Path, help="file to touch on success")
    base.set_defaults(run=check_base)
    built = commands.add_parser("built", help="check a built ROM against the base ROM")
    built.add_argument("rom", type=Path)
    built.add_argument("--base", type=Path, required=True)
    built.add_argument("--stamp", type=Path, help="file to touch on success")
    built.set_defaults(run=check_built)
    args = parser.parse_args()
    return args.run(args)


if __name__ == "__main__":
    sys.exit(main())
