#!/usr/bin/env python3
"""Builds a library source file from the fetched one with some of its top-level definitions replaced by the project's.

The SDK's sources are fetched, not kept here (see the README). Where the game links another release of a file than the
fetched one, the functions that differ are written here, from the game's code, in a file of their own under
lib/overrides/ at the fetched file's path (lib/overrides/NitroSDK/libraries/mi/src/mi_dma.c for
lib/NitroSDK/libraries/mi/src/mi_dma.c). That file holds only the project's code: it names the fetched definitions it
takes the place of, and never repeats their text. This writes the fetched file with those changes made, into the build
directory, which is what gets compiled.

The override file is a list of blocks, each begun by a directive comment on a line of its own:

    /* override: replace NAME */     the block takes the place of the fetched definition of NAME
    /* override: remove NAME */      the fetched definition of NAME, and any declaration of it, goes; the block is empty
    /* override: before NAME */      the block goes just before the fetched definition of NAME
    /* override: after NAME */       the block goes just after it
    /* override: end */              the block goes at the end of the file

NAME is that of a function, or of a variable defined at file scope. A definition is a function with its body, or a
declaration of a variable at file scope; a block that replaces a variable's definition can hold several. Text before the
first directive is ignored (a comment saying what the file is for). Each fetched definition can be named once.
"""
import argparse
import os
import re
import sys

DIRECTIVE = re.compile(r"^[ \t]*/\*[ \t]*override:[ \t]*(replace|remove|before|after|end)(?:[ \t]+(\w+))?[ \t]*\*/[ \t]*$",
                       re.M)
IDENT = re.compile(r"[A-Za-z_]\w*")
KEYWORDS = {"static", "inline", "const", "volatile", "extern", "struct", "union", "enum", "void", "int", "char",
            "short", "long", "signed", "unsigned", "if", "while", "for", "return", "sizeof"}


def items(text: str):
    """Yields (start, end, name, is_definition) for each top-level item of a C file: a function with its body, a
    declaration ending in ';', or a preprocessor line. Comments and strings are skipped."""
    i, n = 0, len(text)
    start = None
    depth = paren = 0
    while i < n:
        c = text[i]
        if start is None:
            if c.isspace():
                i += 1
                continue
            if text.startswith("/*", i):
                i = text.index("*/", i) + 2
                continue
            if text.startswith("//", i):
                i = text.find("\n", i) if text.find("\n", i) >= 0 else n
                continue
            if c == "#":
                # A preprocessor line, with its continuations
                j = i
                while True:
                    k = text.find("\n", j)
                    if k < 0:
                        k = n
                        break
                    if text[k - 1] != "\\":
                        break
                    j = k + 1
                yield i, k, None, False
                i = k
                continue
            start = i
        if text.startswith("/*", i):
            i = text.index("*/", i) + 2
            continue
        if text.startswith("//", i):
            i = text.find("\n", i)
            continue
        if c in "\"'":
            j = i + 1
            while text[j] != c:
                j += 2 if text[j] == "\\" else 1
            i = j + 1
            continue
        if c == "(":
            paren += 1
        elif c == ")":
            paren -= 1
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0 and paren == 0:
                head = text[start:i]
                brace = head.index("{")
                if "=" not in head[:brace] and ")" in head[:brace]:
                    # A function's body ends the item
                    yield start, i + 1, function_name(head[:brace]), True
                    start = None
        elif c == ";" and depth == 0 and paren == 0:
            body = text[start:i]
            yield start, i + 1, declared_name(body), "(" not in body.split("=")[0] or "=" in body
            start = None
        i += 1


def strip(text: str) -> str:
    return re.sub(r"/\*.*?\*/|//[^\n]*", " ", text, flags=re.S)


def function_name(head: str) -> str | None:
    head = strip(head)
    names = [m for m in IDENT.finditer(head[:head.index("(")]) if m.group() not in KEYWORDS] if "(" in head else []
    return names[-1].group() if names else None


def declared_name(body: str) -> str | None:
    body = strip(body).split("=")[0]
    if "(" in body:
        # A function's declaration (or a pointer to one): the name before the first parenthesis, or inside it
        inner = re.search(r"\(\s*\*\s*(\w+)", body)
        return inner.group(1) if inner else function_name(body)
    body = body.split("[")[0]
    names = [m.group() for m in IDENT.finditer(body) if m.group() not in KEYWORDS]
    return names[-1] if names else None


def apply(fetched: str, override: str) -> str:
    marks = list(DIRECTIVE.finditer(override))
    blocks = []
    for k, mark in enumerate(marks):
        end = marks[k + 1].start() if k + 1 < len(marks) else len(override)
        blocks.append((mark.group(1), mark.group(2), override[mark.end():end].strip("\n") + "\n"))
    found = list(items(fetched))
    definitions = {}
    for start, end, name, is_definition in found:
        if name and is_definition:
            definitions.setdefault(name, (start, end))
    edits = []  # (start, end, text)
    tail = []
    seen = set()
    for kind, name, block in blocks:
        if kind == "end":
            tail.append(block)
            continue
        if name in seen:
            sys.exit(f"{name} is named twice")
        seen.add(name)
        if name not in definitions:
            sys.exit(f"the fetched file has no definition of {name}")
        start, end = definitions[name]
        if kind == "replace":
            edits.append((start, end, block))
        elif kind == "remove":
            edits += [(s, e, "") for s, e, nm, _ in found if nm == name]
        elif kind == "before":
            edits.append((start, start, block + "\n"))
        elif kind == "after":
            edits.append((end, end, "\n\n" + block))
    out = fetched
    for start, end, text in sorted(edits, key=lambda e: (e[0], e[1]), reverse=True):
        out = out[:start] + text + out[end:]
    if tail:
        out = out.rstrip("\n") + "\n\n" + "\n".join(tail)
    return out


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("fetched")
    parser.add_argument("override")
    parser.add_argument("output")
    args = parser.parse_args()
    with open(args.fetched, encoding="utf-8", errors="surrogateescape") as f:
        fetched = f.read()
    with open(args.override, encoding="utf-8") as f:
        override = f.read()
    text = apply(fetched, override)
    os.makedirs(os.path.dirname(args.output) or ".", exist_ok=True)
    with open(args.output, "w", encoding="utf-8", errors="surrogateescape") as f:
        f.write(text)


if __name__ == "__main__":
    main()
