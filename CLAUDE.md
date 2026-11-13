# Tomodachi Collection decompilation

A matching decompilation of the DS game, built with dsd and checked with objdiff. Read `README.md` for the build,
`docs/decompiling.md` for the workflow and conventions, and `docs/binary.md` for what is known about the ROM.

## Commands

- `python3 configure.py`: regenerate `build.ninja`. Needed after adding a source file to a `delinks.txt`.
- `ninja`: build, and check every module, symbol and the ROM against the original. Run it before every commit.
- `ninja report`: write `build/jpn_rev1/report.json`, the progress report.
- `ninja dis`: disassemble the game into `build/jpn_rev1/asm/`.

## Rules

- Never commit anything taken from the ROM: no assembly, data dumps or assets. `orig/`, `extract/` and `build/` are
  ignored for that reason. The configs under `config/` hold addresses and names only.
- A change is done when `ninja` passes, not when the code looks right. For a decompiled function, "matches" means
  objdiff reports 100%, which `ninja report` records.
- Document with symbol names as you go: the moment the code shows what a function or variable is, rename it in
  `symbols.txt`, decompiled or not, and give the evidence in the commit message. Unknown things keep their address
  names (`func_02001234`, `unk_154`): never guess a name.
- Subdivide units as their bounds become known: cut a file out of a module's gap in `delinks.txt` before its code is
  written, under `src/` for game code and `lib/<library>/` for library code. Only cut where there is evidence.
- In `docs/binary.md`, a fact read from the ROM is "measured" and a conclusion is "inferred". Keep that distinction
  when adding to it.
- The compiler version and flags in `configure.py` are a hypothesis until a function that depends on them matches.
  If a function will not match, consider them before contorting the source.
