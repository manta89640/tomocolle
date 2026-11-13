# Tomodachi Collection

[![Code](https://bytematch.dev/manta89640/tomocolle.svg?label=Code&measure=matched_code_percent)](https://bytematch.dev/manta89640/tomocolle)
[![Functions](https://bytematch.dev/manta89640/tomocolle.svg?label=Functions&measure=matched_functions)](https://bytematch.dev/manta89640/tomocolle)
[![Data](https://bytematch.dev/manta89640/tomocolle.svg?label=Data&measure=matched_data_percent)](https://bytematch.dev/manta89640/tomocolle)

A work-in-progress matching decompilation of **Tomodachi Collection** (トモダチコレクション, Nintendo DS, 2009): C++
source that compiles, with the game's own compiler, back into the bytes of the original ROM.

This repository holds none of the game. You bring your own ROM, and the build takes everything else from it.

| Version    | ROM                                  | Game code | SHA-1                                      |
| ---------- | ------------------------------------ | --------- | ------------------------------------------ |
| `jpn_rev1` | Tomodachi Collection (Japan) (Rev 1) | `CCUJ`    | `f37d5ff0e3444cf3fab38890efb71f4f14c96994` |

## Where it stands

The ROM is taken apart into 114 modules (the main program, its two autoloads and 111 overlays) and linked back
together by the original linker, and every module comes out identical. That is the baseline the rest stands on: any
piece of it can now be replaced by source, and the build says at once whether the result still matches.

Of the game's 7,356 functions (1.96 MB of code), the first six are decompiled. See
[ByteMatch](https://bytematch.dev/manta89640/tomocolle) for the current numbers.

## Building

You need Python 3.11 or later, [ninja](https://ninja-build.org), and on macOS [wine](https://www.winehq.org)
(`brew install --cask wine-stable`), which runs the compiler. Linux needs nothing more: the build downloads
[wibo](https://github.com/decompals/wibo) for that.

1. Put your ROM at `orig/baserom_jpn_rev1.nds`, see [orig/README.md](orig/README.md).
2. Configure, which downloads the tools into `tools/bin/` and checks each against its pinned SHA-256:
   ```sh
   python3 configure.py
   ```
3. Build and check:
   ```sh
   ninja
   ```

`ninja` extracts the ROM, delinks it, compiles the decompiled sources, links, rebuilds the ROM as
`build/jpn_rev1/tomocolle_jpn_rev1.nds`, and checks every module, every symbol's address and the ROM itself against
the original.

| Command            | What it does                                                                        |
| ------------------ | ----------------------------------------------------------------------------------- |
| `ninja`            | Builds and checks everything, and writes `objdiff.json` for the objdiff GUI          |
| `ninja report`     | Writes the progress report, `build/jpn_rev1/report.json`                             |
| `ninja dis`        | Disassembles the whole game into `build/jpn_rev1/asm/`, to read                      |
| `ninja compile`    | Compiles the sources only, which needs no ROM                                        |

Without the DS's ARM7 BIOS the built ROM differs from the original in four bytes of its header, a checksum of the
encrypted secure area, and the build says so. With your own dump of the BIOS as `arm7_bios.bin` in this directory, the
ROM is identical.

## Decompiling

Open the repository in [objdiff](https://github.com/encounter/objdiff) to compare your code with the original as
you write it. [docs/decompiling.md](docs/decompiling.md) walks through adding a file, and
[docs/binary.md](docs/binary.md) holds what is known about the ROM and how each fact was measured.

| Directory  | What it holds                                                                                       |
| ---------- | --------------------------------------------------------------------------------------------------- |
| `src/`     | Decompiled code                                                                                      |
| `include/` | Headers                                                                                              |
| `config/`  | The [dsd](https://github.com/AetiasHax/ds-decomp) config of each version: symbols, relocations, and which addresses belong to which source file |
| `tools/`   | The build's own scripts                                                                              |
| `docs/`    | Documentation                                                                                        |
| `orig/`    | Your base ROM (not committed)                                                                        |

## Credits

Built on [dsd](https://github.com/AetiasHax/ds-decomp) and [objdiff](https://github.com/encounter/objdiff). The build
follows the one of the [Phantom Hourglass decompilation](https://github.com/zeldaret/ph).
