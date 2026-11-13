#!/usr/bin/env python3
"""Rewrites a dependency file from mwccarm in place, so that ninja can read it.

Run through wine or wibo, the compiler names its files as Windows sees them, such as Z:\\home\\me\\include\\a.h: with
a drive letter, and with backslashes that a depfile takes for escapes.
"""
import re
import sys
from pathlib import Path

SPACE = "\0"


def fix_path(path: str) -> str:
    path = re.sub(r"^[A-Za-z]:", "", path).replace("\\", "/")
    return path.replace(SPACE, "\\ ")


def fix(text: str) -> str:
    # One logical line: join the continued lines, and set escaped spaces aside from the ones between paths
    text = re.sub(r"\\\r?\n", " ", text).replace("\\ ", SPACE)
    # The target ends at the first colon that is followed by a space, which a drive letter's colon is not
    target, _, dependencies = text.partition(": ")
    lines = [f"{fix_path(target.strip())}:"]
    lines += [f"  {fix_path(dependency)}" for dependency in dependencies.split()]
    return " \\\n".join(lines) + "\n"


def main() -> int:
    path = Path(sys.argv[1])
    path.write_text(fix(path.read_text(encoding="utf-8", errors="replace")), encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main())
