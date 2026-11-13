#!/usr/bin/env python3
"""Writes the SDK's fx_const.h from its table of constants, fx_const.csv: each as a fixed-point number of its type.

The SDK's own build generates this header with a script of its own. This one does the same from the same table, so
that building here runs nothing that was fetched.
"""
import argparse
import csv
import sys
from pathlib import Path

# Fraction bits and literal suffix of each fixed-point type
TYPES = {"fx16": (12, ""), "fx32": (12, "L"), "fx64": (12, "LL"), "fx64c": (32, "LL")}


def encode(value: float, fraction_bits: int) -> int:
    """Returns a number in fixed point: its whole part, and its fraction rounded half up."""
    magnitude = abs(value)
    one = 1 << fraction_bits
    whole = int(magnitude * one) & ~(one - 1)
    fraction = int((magnitude - int(magnitude)) * one + 0.5) & (one - 1)
    return -(whole + fraction) if value < 0 else whole + fraction


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("table", type=Path)
    parser.add_argument("header", type=Path)
    args = parser.parse_args()

    lines = ["#ifndef FX_CONST_H", "#define FX_CONST_H", ""]
    with args.table.open(newline="", encoding="utf-8") as table:
        for row in csv.reader(table):
            if not row or row[0].startswith("#"):
                continue
            name, kind, value = row[0], row[1], float(row[2])
            fraction_bits, suffix = TYPES[kind]
            encoded = encode(value, fraction_bits)
            sign = "-" if encoded < 0 else ""
            lines.append(f"#define {kind.upper()}_{name} (({kind}) {sign}0x{abs(encoded):016x}{suffix}) // {value:.12f}")
    lines += ["", "#endif // FX_CONST_H", ""]
    args.header.parent.mkdir(parents=True, exist_ok=True)
    args.header.write_text("\n".join(lines), encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main())
