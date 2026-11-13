#!/usr/bin/env python3
"""Filters the compiler's preprocessed output (-EP) from stdin to stdout, into a context for a decomp.me scratch.

Drops the line markers and the pragmas about the dump itself, which only the compiler that wrote them understands.
"""
import re
import sys

DROP = re.compile(r"^#(line\b|pragma \w+_prepdump\b)")


def main() -> int:
    text = sys.stdin.buffer.read().decode("utf-8", errors="replace")
    sys.stdout.write("".join(line for line in text.splitlines(keepends=True) if not DROP.match(line)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
