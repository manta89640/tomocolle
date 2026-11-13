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
- In `docs/binary.md`, a fact read from the ROM is "measured" and a conclusion is "inferred". Keep that distinction
  when adding to it, and name unknown things by address (`func_02001234`, `unk_154`) instead of guessing.
- The compiler version and flags in `configure.py` are a hypothesis until a function that depends on them matches.
  If a function will not match, consider them before contorting the source.
