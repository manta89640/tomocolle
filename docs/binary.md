# The ROM

What is known about `jpn_rev1`, and how each fact was found. Facts read straight from the ROM are marked **measured**;
conclusions drawn from them are marked **inferred**, with the reasoning, so that a wrong one can be traced and fixed.

`python3 tools/rom_info.py orig/baserom_jpn_rev1.nds` prints most of the measured ones.

## Identity

| | | |
| --- | --- | --- |
| Title, game code, maker | `TOMODACHI`, `CCUJ`, `01` (Nintendo) | measured, header |
| ROM version | 1 | measured, header byte `0x1E` |
| Size | 64 MiB, of which `0x364DBE0` bytes are used | measured, header |
| Unit code | 0: a plain DS game, not DSi-enhanced | measured, header byte `0x12` |
| SHA-1 | `f37d5ff0e3444cf3fab38890efb71f4f14c96994` | measured |

The dump has its secure area decrypted (it starts with `E7FFDEFF E7FFDEFF`). The header's checksum of the secure area
is over the encrypted form, which is why rebuilding an identical header needs the ARM7 BIOS.

## Programs

| | Loaded at | Size | |
| --- | --- | --- | --- |
| ARM9 main program | `0x02000000`, entry `0x02000800` | `0xB52BC` compressed, `0x115818` decompressed | measured |
| ITCM autoload | `0x01FF8000` | `0x320` | measured |
| DTCM autoload | `0x027E0000` | `0x60`, and `0x20` of BSS | measured |
| ARM9 overlays | `0x02149A40`, all 111 of them | `0x160E20` in total, none compressed | measured |
| ARM7 program | `0x02380000` | `0x270F0` | measured |

- Every overlay loads at the same address, directly after the main program's BSS. **Inferred:** only one overlay is in
  memory at a time, so an overlay never calls another, only the main program.
- The asset paths in the main program follow the pattern `Scn/Scene/Scene<Name>_LZ.bin`. **Inferred:** an overlay is
  the code of one scene, more or less.
- The ARM7 program is not part of this project so far. **Inferred, not checked:** it is the stock one of the SDK, as
  in most games.
- The game has DS Protect, anti-piracy code that is encrypted in the ROM. Measured, by dsd, which decrypts it on
  extraction and encrypts it again on building (`enable_dsprot: true` in `config/jpn_rev1/arm9/config.yaml`).

## Code

From dsd's analysis and objdiff's report:

| | Functions | Code | Data |
| --- | --- | --- | --- |
| Main program | 4,205 | 720,900 bytes | 627,372 bytes |
| ITCM | 6 | 796 bytes | |
| Overlays | 3,145 | 1,235,564 bytes | 217,948 bytes |
| **Total** | **7,356** | **1,957,260 bytes** | **845,320 bytes** |

- **ARM, not Thumb.** Measured: all but 28 functions are ARM. Of those 28, 18 are the system call stubs at the start
  of the program and the other ten are a few bytes each, below `0x0200A000`. So the sources are compiled without
  `-thumb`.
- **C++.** Measured: the main program has a `.init` section of 26 static initializers (`__sinit_`) and a `.ctor`
  table of them, and 20 of the overlays have one each; overlays hold tables of function pointers that are called
  through an object (overlay 0's is at `0x02149C68`); and code reaches part of an object through an offset that it
  reads from 12 bytes before such a table. **Inferred:** the game code is C++, those tables are virtual function
  tables, and that offset is how a virtual base class is found, so some of its classes use virtual inheritance. The
  libraries below are C.
- **No exceptions.** Measured: the exception table section, `.exceptix`, is empty in every module. Hence
  `-Cpp_exceptions off`.
- **Text is UTF-16, and in `.data`**, not `.rodata`. Measured in overlay 0, whose only string is `テスト12３`. The
  compiler reproduces those bytes from `L"テスト12３"` in a UTF-8 source file (through sjiswrap and `-enc SJIS`).

## Libraries

The main program is not all game code. Measured, from strings:

| Evidence | Library |
| --- | --- |
| SDK version word `0x04027537` in the module parameters | NitroSDK 4.2, release 30007 |
| `[SDK+NINTENDO:BACKUP]` | NitroSDK's backup (save) device code |
| `[SDK+SHARP:SHTTS-1_2_0]`, `Spk_male`, `Spk_female`, ... | Sharp's text-to-speech middleware 1.2.0, the voices of the Miis |
| `NFL:/NFL_Res.dat`, `Cmn/Nfl/NFL_Res_LZ.bin` | NFL, Nintendo's Mii face library |
| `WH_StateInStartParentKeyShare failed`, ... | WH, the SDK's wireless sample code, compiled into the game |
| `.nsc`, `.ncg`, `.nce`, `.imd`, `.ica`, `.NFTR` files | NitroSystem (2D and 3D graphics, fonts) |
| `sound_data.sdat` | NitroSystem's sound library |
| `tmd_msg_JP.bmg`, ... | BMG message files; `tmd` appears to be the project's own prefix |

The low addresses of the main program are SDK code (the cache functions that write to CP15 are at `0x02008054` and
after). Where the libraries end and the game begins is not mapped yet, and it matters: library code is identified
rather than decompiled, and should be counted apart from the game's own.

## Compiler

**Inferred, and only weakly tested so far:** mwccarm 2.0 SP2 patch 2 ("Version 3.0 build 137").

- NitroSDK 4.2 is from 2008, and the game came out in June 2009. Pokémon Platinum (2008), also on NitroSDK 4.2, is
  matched by this version.
- The six functions decompiled so far match with it. They are too simple to tell compiler versions apart, so this
  shows that the pipeline works, not that the version is right.

A function with enough arithmetic, branching and register pressure to tell the versions apart has yet to be matched.
The WH code is a good candidate, since its source is the SDK's own sample: compiled with the right compiler and flags,
it must come out as it is in the ROM. Compiling it needs the SDK's headers and the C library headers that those
include (`stdio.h`, `stdarg.h`), which do not come with the compiler.

The flags in `configure.py` are the usual ones of games of this SDK, adjusted to what is measured above. Each is a
guess until a function that depends on it matches.

## Linking

The original was linked with dead-stripping, as C++ needs: the compiler emits variants of constructors and destructors
that nothing calls. The build links with `-nodead` for now. With `-dead`, the linker strips code that is in the ROM,
because `relocs.txt` does not yet know every reference to it (a pointer in a table that the analysis took for a
number). Switching back to `-dead` is a milestone of its own, and files with unused variants cannot be marked complete
before it.
