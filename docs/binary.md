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

### Where they are

`tools/match_objects.py` compares the functions of the game with those of compiled objects of these libraries, as
another decompilation builds them (Pokémon Platinum's: NitroSDK 4.2.30001, NitroSystem 071126.1 and the compiler's
MSL). A function is taken as found when its bytes are equal outside the places the linker fills in, and one of three
further grounds holds, which the tool's header describes. Measured that way:

| Library | Functions found | Where in the main program |
| --- | --- | --- |
| NitroSDK | 646 | `0x02000800` to about `0x02014600`, and all six functions of the ITCM |
| MSL C library | 93 | `0x02014AA4` to `0x0201F4DC` |
| NitroSystem | 554 | `0x02053C64` to about `0x02069C00` |
| NitroWiFi, NitroDWC | none | Not linked: the game has no Nintendo Wi-Fi Connection code |

Every object's functions come out in the game in the order they have in the object, with none out of place, which is
what a correct match looks like and a coincidence does not. The names the matcher gave agree with the ten assigned by
hand from the CP15 operations of `os_cache.c`, and with dsd's own signatures.

MSL's wide-character files the game links are written and complete: `wprintf.c` (`swprintf`, `vswprintf` and the
formatter, ten functions, and its wide string literals at `0x0210F3EC`), `mbstring.c` (6), `wchar_io.c` (`fwide`,
`__fwide`) and `wmem.c` (`wmemcpy`, `wmemchr`). Measured: the game links of them only what `swprintf` needs, and
`mbtowc`/`wctomb` convert through `_current_locale` (`0x0210F1E4`), whose "C" locale passes a byte through unchanged.
`wprintf.o`'s formatter is `printf.o`'s with wide characters, except that `float2str` converts in a `char` buffer and
widens the result with `mbstowcs`, `__wStringWrite` returns `wmemcpy`'s result where `__StringWrite` returns 1, and a
narrow `%#s` reads its length through the wide buffer pointer instead of the string.

So 1,293 of the main program's 4,205 functions are library code with known names, and 181 library files are cut
out as units under `lib/`: 155 of NitroSDK and NitroSystem, and 26 of MSL's C library (`lib/MSL_C/`). `ansi_fp.c` and
`math_api.c` were cut later. Measured: their ranges seemed to hold one function more than the objects have because
dsd had split `__dorounddecup` (0x50 bytes in the archive's `ansi_fp.o`) at its unreachable last `bx lr` into
`func_0201e014` and a 4-byte `func_0201e060`; with that one function, `ansi_fp.o`'s 13 functions fill `0x0201DFA8` to
`0x0201F3C4` in the object's order, and `math_api.o`'s `__msl_generic_count_bits64`, `__signbitd` and `__fpclassifyd`
(the others are not linked) fill `0x0201F3C4` to `0x0201F4DC`. `ansi_fp.o`'s `.data` (`pow_10`, then its 22 digit
strings) is at `0x0210F450` to `0x0210F58C`, byte for byte, and `strtold.o`'s `.rodata` ("NAN(", "INFINITY") at
`0x020B5704`.

MSL's own files are compiled with 2.0/sp2 (measured: its objects say "Metrowerks C/C++ for ARM 3.0.0.136", which is
what that compiler writes) and the headers of that MSL, internal ones included. `lib/MSL_C/wstring.c` is the first:
its seven functions match and it is linked from source. `lib/MSL_C/e_pow.c`, fdlibm's `pow` as MSL changed it (see its
header), has its data matching and its one function at 99.88%: three values are given registers in another order.
The other fdlibm and math files the game links (`s_copysign`, `s_fabs`, `s_frexp`, `s_ldexp`, `w_pow`, `math_arm`,
`math_sun`, `math_api`) match and are linked from source; `strtold.c`'s `__strtold` is at 99.1%, one pointer of its
hexadecimal tail in another register (see its header). `ansi_fp.c`, the decimal conversion that the formatters and
`__strtold` use (`__num2dec`, `__dec2num` and their eleven static helpers), matches and is linked from source, its
`.data` with it. Measured from its calls: the soft floating-point runtime's `_dfltu` (unsigned to double) is at
`0x0201FAC4` and `_ll_ufrom_d` (double to unsigned long long) at `0x0201F9F8`; `__dec2num` calls `pow` and `ldexp`,
and reads `INFINITY` through `__float_huge`.
Measured from the original objects (in the compiler's MSL archive): MSL was compiled with exception tables, which the
game's link leaves out (its `.exceptix` is empty), and the soft floating-point helpers it calls are `_dadd`, `_dsub`,
`_dmul`, `_ddiv`, `_dsqrt`, `_dgr`, `_dleq`, `_dflt`, `_f2d` and `_d2f`, now named. MSL's files are built here without
exception tables (`-Cpp_exceptions off`): measured, the code of `strtoul.c` and `scanf.c` is the same either way, and
the link keeps every `.exceptix` it is given, so a complete unit with tables fails every module's check.
`strtoul.c` and `scanf.c` are complete; measured, each ended one function short of the archive's object in
`delinks.txt` (`atoi`, `sscanf`), which the matcher had left in the gap. What is left in the main program is two stretches: `0x0201F4DC` to `0x02053C64`, and from
the end of NitroSystem to the end of the code at `0x020B04C8`. They hold the game's own code and the libraries not
matched yet: NFL, and whatever of the SDK differs between release 30001 and the game's 30007. The text-to-speech
middleware and WH (`0x0202F42C` to `0x0203115C`, see "WH, the wireless wrapper") have since been found and decompiled.

How MSL was built, measured on the archive's objects and confirmed by files of it that match and link: each object
pools its 4-byte statics and globals into one section (`abort_exit_arm.o`'s `.bss` holds `__console_exit`,
`__stdio_exit`, `__atexit_curr_func`, `__aborting` and the 256-byte `__atexit_funcs` at 0, 4, 8, 12 and 16), which
the compiler does only with `-ipa file`; the stdio, exit and signal code calls `OS_TryLockMutex` and `OS_LockMutex` for
its critical regions, so the library was built thread-safe, which only MSL's prefix file `PREFIX_NITRO.h` sets (and
`critical_regions.NITRO.h` includes the SDK's thread and mutex headers); and the console is buffered
(`_MSL_BUFFERED_CONSOLE`): `ansi_files.o` has three 256-byte buffers, with stdin and stdout line buffered and stderr
unbuffered in the FILE table. The build of `lib/MSL_C/` does all three, and leaves out the exception tables.

The MSL files the game links whole are written and complete: `string.c` (8 functions), `mem.c` (4), `mem_funcs.c`
(1), `buffer_io.c` (5), `char_io.c` (2), `file_io.c` (1), `ansi_files.c` (1, and the FILE table at `0x0210F09C`),
`signal.c` (1, and its handlers), `abort_exit_arm.c` (3: it begins at `abort`, `0x02014A80`), `arith.c` (`abs`), and
two files of data only, the archive's `critical_regions` member (`__cs_id`, `__cs_ref`, `__cs`, `0x0211D1C0`) and
`errno.c`. Measured: `abs` is byte for byte `labs` too, so which of the two the game links is not known; and `atoi`
(`0x0201A8EC`) is `atol` too. `snprintf` (`0x020175A8`), `sscanf` (`0x020188F4`) and `atoi` end `printf.o`,
`scanf.o` and `strtoul.o`'s stretches in the game, each the next function of its object after the one before it and
calling `vsnprintf`, `vsscanf` and `strtol`. `__read_console`, `__write_console` and `__close_console` are at
`0x02021778`, `0x020217C8` and `0x020217FC`, outside MSL: the FILE table refers to them.

`lib/MSL_C/printf.c` (`0x0201549C` to `0x020175D0`, data `0x0210F3AC` to `0x0210F3EC`) is complete. Measured: the game
keeps ten of the archive's sixteen functions of `printf.o`, the conversion helpers, `__pformatter`, `__StringWrite`,
`vsnprintf` and `snprintf`; `printf`, `vprintf`, `vfprintf`, `vsprintf`, `sprintf` and `__FileWrite` are not in the
ROM. The object calls the runtime's `_dls`, `_deq` and `_ull_mod`; `_ull_mod` is a second entry point inside
`_ll_udiv`, at `0x02020DB4`.

### What differs from the fetched release

Most of the SDK and NitroSystem files the build compiles come out identical to the game's; where a function seemed
not to, the cause was mostly the configuration, not the code. Measured, function by function, by comparing the
compiled object with the delinked one in object order, size and bytes outside relocations:

- **Names.** 104 functions matched already and only lacked their names: static functions that dsd had given address
  names (the eight `texmtxCalc_flag*_` builders of each of `cgtool/3dsmax.c`, `maya.c` and `xsi.c`, gfd's 36
  `DoTransfer*`, snd's `BeginSleep`/`EndSleep`/`StrmCallback`/..., fnd's `AllocFromHead`/`AllocFromTail`,
  `updateHintVec_`), and a few that dsd's signatures had named otherwise (`FS_LoadOverlayImage` and
  `FS_UnloadOverlayImage` were `FS_LoadOverlayFile` and `FS_StopOverlay`; crt0's `_start` was `Entry`). crt0's
  `_start_AutoloadDoneCallback` keeps dsd's name `AutoloadCallback`: `dsd rom config` finds it by that name.
- **Ranges.** `3dsmax.c` and `maya.c` ended two functions early in `delinks.txt` (`texmtxCalc_flagTRS_` and
  `NNSi_G3dSendTexSRT3dsMax`/`Maya` were in the gap), and `os_system.c` one (`OS_WaitVBlankIntr`, `0x02008E10`).
- **Relocations.** Three that dsd made wrong: the constants `0x01FF8000` (`CARDi_TryReadCardDma`) and `0x027E0021`
  (`init_cp15`) are numbers that look like addresses, not references; and `OSi_DisplayExContext`'s call of
  `OS_DisableProtectionUnit` after the `bx` to the user's handler, and its load of that call's address as the
  handler's return address, had none.
- **Structures cut into fields.** dsd puts a symbol on every address the code loads, so `NNS_G3dGlb`, `gGXState`,
  `NNS_G3dRSOnGlb`, sbc's command statics and nsbca's `pivotUtil_` were each several symbols. They are one again,
  referred to with an offset (the scenes' `data_02142438` is `&NNS_G3dGlb.cameraMtx`).

Four files are another release than the fetched one, in code (the last only in how it is written):

- **`mi/src/mi_dma.c`** has `MI_StopAllDma` (`0x02009400`, `MI_StopDma` of channels 0 to 3) after `MI_StopDma`,
  which the fetched file lacks. Its name is the one later SDKs document for it.
- **`spi/src/pm.c`** (`0x0200ECA8` to `0x0200F668`) is later than the fetched release. Measured: the busy wait is
  called through a pointer (`.data` `0x020FC794`, formerly named `gText_Mushiro` as if it were text) that holds one of
  three waits: in an IRQ handler, with IRQs or IME off, or either (`PM_ForceToPowerOffAsync` and the synchronous
  power-off choose them, so power-off works with interrupts disabled); the utility commands go through one function
  that takes a 16-bit parameter and a result buffer, sent as PXI command 0x61 (the fetched header's numbers are
  0x63); `PM_Init` sets up no register buffers and no mutex; `PMi_SetLCDPower` sends utility command 0x10 around
  turning the LCDs off and on, and waits two frames when the LCDs go off within two frames of `PM_Init`;
  `PM_ForceToPowerOff` retries until it succeeds, then disables interrupts, stops all DMA and halts; and before
  powering off, a function at `0x0200F5DC`, the file's last, turns the LCDs on with their backlights off. The file's
  `.bss` pool is 0x2C bytes (`PMi_IsInit` at `0x0211B68C`, the LCD frame counts, a flag the ARM7's command 0x63 sets,
  utility 0x10's parameter, the two sleep callback lists, `PMi_Work` at +0x1C).
  Inferred: the flag is the sleep-end flag, whose command number changed with the others. The functions the fetched
  file does not have, and the statics, have the project's names for what their code does.

- **`card/src/card_spi.c`**: `CARDi_IdentifyBackupCore` (`0x020105CC`, 0x384 bytes against the fetched 0x348) knows
  one device more, a 128 KB EEPROM (size bit 17: 256-byte pages, three address bytes, five pages a program
  operation). Measured: everything else it sets is the fetched release's. It is the only function of the file the game
  keeps.
- **`math/src/qsort.c`**: `MATH_QSort` (`0x020141F4`, 0x2F0 bytes against the fetched 0x288) is the same
  hand-written routine, written out the way the SDK's other assembly is under the define
  `SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS`, which `configure.py` gives the SDK's files. Measured: it keeps the
  fetched text's register use, its `stmfd`/`ldmfd` of r4-r11 and lr, its frame on the stack when `stackBuf` is NULL,
  its swap loops (`swp`/`swpb`) and its stack of sub-ranges, instruction for instruction, with no compiler idiom; the
  only difference is that each of its 13 conditionally executed instructions other than branches (`addlt` and
  `subgt` of the partition scans, the eight `strlt` that push the sub-ranges, `ldreq`/`addeq`/`addeq` that free the
  frame) is a conditional branch to it and a branch past it, `b<cc> 1f; b 2f; 1: <op>; 2:`, 8 bytes more each:
  0x288 + 13 * 8 = 0x2F0. A later commit of the fetched repository, `ccbbcc8` ("Add options for HeartGold"), writes
  those 13 out so under that define, and its file compiles, with this unit's compiler and flags, to the game's bytes;
  `configure.py` fetches that one file. Inferred: the branches are what the SDK's own compiler, 2.0 SP1p3 by the
  define's name, made of conditional instructions in inline assembly, which the fetched repository's other assembly
  (`crt0.c`, `mi_memory.c`, `os_*.c`) already spells out the same way.

The first three are built as `lib/overrides/` describes, `qsort.c` from the later commit's file (decompiling.md,
"Where the game's release of a file differs").

The small gaps between the units turned out to be functions of the files on either side, or small files of their own
(measured, by the compiled objects: bytes equal outside relocations, in the objects' order): the ranges of 19 files
ended or began one to eight functions early, and `fx.c`, `mi_wram.c`, `mi_init.c`, `wm_ks.c`, `card_spi.c`,
`qsort.c`, `g2d_NCG_load.c`, `g2di_SplitChar.c`, `gfd_TexVramMan.c`, `gfd_PlttVramMan.c` and
`gfd_FramePlttVramMan.c` are units now. Where two functions of an object have the same bytes, the object's order, or
the data that points at them, told them apart, and so found three that dsd's signatures had named wrongly:
`0x02004718` is `GX_DisableBankForLCDC` (it passes `&gGXState.lcdc`), `0x0205FDA4` is `NNSi_G2dSplitCharUTF16` and
`0x0205E2EC` `NNS_G2dFontInitUTF16`, and one more when its file was linked from source: `0x0205B9C4` is
`NNS_G2dDrawCellAnimation`, not `NNS_G2dDrawCell` (the same bytes, but it calls `DrawCellAnimationImpl_`, which the
linker otherwise drops). Linking a file from source checks what objdiff does not: a wrong name for a function with a
twin, or a static that only a misnamed function calls, changes the ROM. With their data placed, `os_ownerInfo.c`,
`tp.c`, `fs_rom.c`, `fs_command_default.c`, `card_common.c`, `g2d_Renderer.c`, `g2d_OAM.c` and the gfd files are
complete; among what the data showed, `fs_command_default.c`'s table of commands (`0x020B5478`) had been named as text,
and `card_common.c` gives the card thread the top of its 1 KB stack, `cardi_thread_stack + 0x400`, which dsd had taken
for `card_rom.c`'s `cardi_rom_base`. Still incomplete: `os_irqHandler.c`, `os_irqTable.c` and `os_interrupt.c`, which
need the DTCM's `OS_IRQTable` (`0x027E0000`) and `OSi_IrqThreadQueue` and the linker's `SDK_AUTOLOAD_DTCM_START` and
`SDK_IRQ_STACKSIZE`; and `card_backup.c`, whose `[SDK+NINTENDO:BACKUP]` string is in a `.version` section, at
`0x02000BC0` after the build info, which dsd's sections do not describe. `g2d_NCG_load.c`'s two functions are either the OBJ or the BG pair
(`NNS_G2dGetUnpackedCharacterData`/`NNS_G2dUnpackNCG` or `...BGCharacterData`/`...BGNCG`), which are byte for byte the
same, and keep their address names.

The ITCM is `OS_IrqHandler`, `OS_IrqHandler_ThreadSwitch` (`os_irqHandler.c`) and `MIi_DmaSetParams` with its three
variants (`mi_dma.c`), compiled from the fetched files, where they are assembly functions in the ITCM's section. Each
of the two files is one unit of the main program whose `delinks.txt` entry names an `.itcm` range too: dsd moves that
range into the file's object ("section migration"). `CHT_IsPictochatParent` (`0x020144E4`, after `MATH_QSort`) is
from the PictoChat search library, `libcht`, which the SDK ships compiled, without sources (its header,
`nitro/cht/ARM9/pictocatch.h`, is in the fetched SDK): `lib/CHT/pictocatch.c` is the project's decompilation of it,
built with the SDK's compiler and flags, and complete. Measured: it takes a beacon for a PictoChat room's when its
game group ID is 0 and the first four half-words of its user data, copied out, begin with `0x2348` or `0xBD8A` and
have 4 as the fourth.

## Voice synthesis

The Miis' voices come from Sharp's text-to-speech engine, SHTTS 1.2.0 (the version is in the SDK's list of library
tags). Measured:

| | Where | |
| --- | --- | --- |
| `TtsPlayer`, the game's wrapper | `0x02083DC4` to `0x0208473C`, 28 functions | Decompiled, `src/voice/TtsPlayer.cpp` |
| The engine | `0x0208473C` to `0x0208EB18`, 123 functions, 44 KB | Decompiled, `lib/SHTTS/`: all 123 functions match and are linked from source |

The engine's bounds are those of a closed module: nothing in that range is called from outside it but by `TtsPlayer`,
and it calls nothing but the C library and the OS (threads, mutexes, `OS_Sleep`). It is one unit for now, because
where its files begin and end is not known.

The engine takes Shift JIS text with control codes in it, such as `<\Spk_male` to pick a speaker and `#B4800` to
set the pitch. It hands back 16-bit samples a block at a time through a callback, and the game plays them at
22,000 Hz.

The engine is built with the game's compiler and flags, as C: its decompiled functions match with them. So it was
most likely compiled from Sharp's source as part of the game's build.

Its state is one struct of `0x14000` bytes that the caller provides (`Shtts` in `lib/SHTTS/shtts.h`), with a thread
of its own that synthesizes, and two copies of the voice settings: those a caller sets, at `0xEF20`, and those in
use, at `0xEF50`. `Shtts_SetParam` sets a setting by number, and refuses while the engine is synthesizing. Measured
from the setters, the settings and the values each accepts:

| Setting | Command | Name | Values | Field of `ShttsParams` |
| --- | --- | --- | --- | --- |
| 1 | | The speaker | `0x101` to `0x10C` | A preset, see below |
| 2 | | The way of speaking | `0x201` to `0x211` | A preset |
| 3 | `#S` | Speed | 20 to 2200 | `speed`, 100 by default |
| 4 | `#B` | Pitch | 3500 to 7000 | `pitch`, 5058 by default |
| 5 | `#V` | Volume | -10000 to 360 | `volume` |
| 6 | `#X` | Warp | 80 to 130 | `warp`, 100 by default |
| 7 | `#r` | Ring | 0 to 5000 | `ring` |
| 8 | `#I` | Echo Delay | 0 to 3000 | `echoDelay` |
| 9 | `#J` | Echo Gain | 0 to 100 | `echoGain` |
| 10 | `#R` | Fir Delay | 0 to 1000 | `firDelay` |
| 11 | `#G` | Fir Gain | 0 to 100 | `firGain` |
| 12 | `#C` | Not named | 0 to 3, a mode | `mode` and `unk_14` |
| 13 | `#k` | Chorus | 0 to 100 | `chorus` |
| 14 | `#A` | Accent | 0 to 400 | `accent`, 100 by default |
| 15 | `#P` | Phrase | 0 to 400 | `phrase`, 100 by default |
| 16 | `#T` | PTrans | 0 to 1000 | `pitchTrans` |

The names are the engine's own: `ShttsParam_RunCommand`, which carries out a command, passes each to the function that keeps
a value in range, though that function does nothing with it (most likely it named the setting in a message that the
retail build leaves out). The same function shows what the commands in text do. A number after `+` is added to the
value in use, for `#B`, `#P`, `#S` and `#V`. Besides the settings there are:

| Command | Effect |
| --- | --- |
| `#E` | Emphasis: pitch +100, accent +5, phrase +5, speed -10, volume +10 |
| `#D` | The opposite of `#E` |
| `#F` | Pitch back to its default, 5058 |
| `#M` | Pitch to 4608 |
| `#Z` | All settings back to what the caller set |

So the `#B4800`, `#S90`, `#X90`, `#I500` and `#J20` of a preset are a pitch, a speed, a warp, an echo delay and an
echo gain. A command changes either the caller's settings or the ones in use, by an argument: presets change the
caller's.

Settings 1 and 2 pick a preset by name. `ShttsPresets`, at `0x020F2708`, is a table of 53 of them: a name as it is
written in text, and a string of `#` commands that the preset stands for. Measured from the two setters:

- Speakers: `Spk_male`, `Spk_female`, `Spk_admale`, `Spk_adfemale`, `Spk_emale`, `Spk_efemale`, `Spk_chmale`,
  `Spk_chfemale`, `Spk_sisters`, `Spk_meimu`, `Spk_space`, `Spk_giant`.
- Ways of speaking: `angry`, `business`, `calm`, `depressed`, `excited`, `falsetto`, `happy`, `loud`, `monotone`,
  `perky`, `quiet`, `sarcastic`, `scared`, `shout`, `tence`, `whisper`, `sing`.

`ShttsPhonemeNames`, at `0x020BC174`, lists the 38 sounds the engine speaks in, as it writes them: `S` for silence,
the vowels `a i u e o`, `N`, `X`, then the consonants from `k` to `kw`. The engine keeps the number of the sound it
is speaking, and `Shtts_GetPhonemeName` returns its name, which is what a game needs to move a mouth in time with speech.

Open questions from the engine's code, which may be one question:

- A few loops in the original are unrolled four times (a 16-byte clear in `Shtts_Init` and `ShttsEngine_ResetSynthesis`), and
  some loops over a count test the count twice before they start (`ShttsMath_ScaleSamples`, `ShttsSynth_FadeSamples`). The compiler
  does neither for the source as written here, and has no option for unrolling that its help lists.
- The array those two functions clear is reached as the engine's address plus `0x730`, then plus `0xB000`, where
  everything near it is reached from plus `0xB000`. That suggests it belongs to a structure that starts at `0x730`.

Four of the engine's functions are assembly in the original too: sums of products with the multiply-accumulate
instructions for 16-bit fixed point. They are written as assembly functions here.

The engine's source is four files, `shtts_1.c` to `shtts_4.c`, and three of them are linked into the ROM in place of
the original's bytes. Where the engine's own files began and ended is not known; these are cut where linking needs:

- `shtts_2.c` is one function, `ShttsDuration_SetWordSyllables`, in a file of its own only because it was the last to
  match. What it took is in the comment above it: three of its parameters are `s8`, and so is its caller's variable.
- The engine's strings in `.data` are interrupted, between those of `ShttsText_ParseWideStrengthDigit` and `ShttsPhone_BuildFromSyllables`, by the
  names of the phonemes. Those belong to `ShttsPhonemeNames`, which is in `.rodata` and yet is read by the engine's
  code as if it were not `const`: so it is defined in another file, of tables, which is not decompiled.

One thing about the linked files is fragile. The strings of a file have no names, and the compiler numbers them
(`@8870`). dsd needs a symbol at the start of each range a file owns, and it has to be the compiled object's name
for it, so `symbols.txt` has four such names, and `relocs.txt` refers to the other strings as one of them plus an
offset. An edit that adds or removes a literal or a local before them renumbers them, and `ninja` will then say that
a symbol is not found: read the new name off the object (`arm-none-eabi-nm`) and put it in `symbols.txt`.

## Sound

The game plays everything but the voices through NitroSystem's sound archive player, from `sound_data.sdat`.
Measured, from the decompiled code in `src/sound/`:

| | Where | |
| --- | --- | --- |
| `SoundSystem` | `0x02028630` to `0x020287F8`, 6 functions | Sets the library up: a sound heap from the main heap, the archive, 2 sequence handles, 6 sequence archive handles, 1 stream handle, 3 saved heap levels. 0xCC bytes |
| `SoundManager` | `0x02035B04` to `0x02036558`, 43 functions | `SoundSystem` with a mutex (at 0xCC) around every call into the library. The one instance is `gSoundManager`; `main` initialises it and 411 places in the overlays use it |
| `ValueRamp`, `SoundParamRamps` | `0x02036558` to `0x0203671C`, 6 functions | A value that steps each frame, freely or to a target; four of them (volume, pitch, tempo ratio, pan, as 20.12 fixed point) drive a playing sound |

- The player's volume settings are three bytes of `gSettings` (at 0xC9: music, effects, voice; each 0 to 3) after a
  stereo flag at 0xC8. Three tables in `SoundManager.cpp` turn a setting into a volume: a scale in 32nds
  (0, 11, 21, 32) for a sound that has a volume of its own in the archive, a volume (0, 42, 84, 127) for one that
  has none, and the voice stream's channel volume (-10000, -180, -60, 0).
- `TtsPlayer` takes the same mutex (`gSoundManager.mMutex`) around its own stream and wave output calls, and asks
  the manager for the voice volume.

Inferred: sequences are music and sequence archive entries are sound effects, from which setting scales which.

## Text

Measured, from the decompiled code in `src/text/`:

| | Where | |
| --- | --- | --- |
| `Font` | `0x0202D5A4` to `0x0202D5D0` | `gFonts`: the 10, 8 and 12 pixel fonts (`LD10UTF16.NFTR`, `LD08UTF16.NFTR`, `LD12UTF16.NFTR`), loaded by `main` |
| `CharCanvas`, `TextCanvas`, `BgTextCanvas`, `ObjTextCanvas`, `ObjCharBox`, `ObjTextBox` | `0x0202D5D0` to `0x0202E3EC`, 32 functions | NitroSystem's character canvas and text canvas, drawn with a one pixel shadow in up to eight directions, shown on a background layer or on sprites arranged over the canvas |
| `CardPullOutMonitor` | `0x0202E3EC` to `0x0202E44C` | Turns the screens off and stops when the card is pulled out (`gCardPullOutMonitor`, which `main` initialises and updates) |
| `MessageFile`, `MessageText` | `0x0202E44C` to `0x0202F42C`, 30 functions | A BMG message file (`gMessageFiles.mFiles[0]` is `BMG:/tmd_msg_JP.bmg`), and a text box that is both kinds of canvas and expands, aligns and draws a message, all at once or a character at a time |

All five files are linked from source, with all their sections: `.text`, `.init` and `.ctor` (static
initialisers), `.rodata`, `.data` and `.bss`.

- `BgTextCanvas` and `ObjTextCanvas` have `TextCanvas` as a virtual base, and `MessageText` inherits from both: their
  tables hold only the offset of that base (4, 0x150, and 0x588 and 0x438 in a `MessageText`). Measured, in `.data`:

  | Address | Table | Base offset | File |
  | --- | --- | --- | --- |
  | `0x0210F6EC` | `_ZTV12BgTextCanvas` | 4 | `TextCanvas.cpp` |
  | `0x0210F6F8` | `_ZTV13ObjTextCanvas` | 0x150 | `TextCanvas.cpp` |
  | `0x0210F704` | `_ZTV12BgTextCanvas__10TextCanvas_at_1080`, `BgTextCanvas` in a `MessageText` | 0x438 | `MessageText.cpp` |
  | `0x0210F710` | `_ZTV13ObjTextCanvas__10TextCanvas_at_1416`, `ObjTextCanvas` in a `MessageText` | 0x588 | `MessageText.cpp` |
  | `0x0210F71C` | `_ZTT11MessageText`, the VTT: where the constructors point the object and its two bases, first through the construction tables | | `MessageText.cpp` |
  | `0x0210F72C` | `_ZTV11MessageText`, two tables of three words: 0x588 for the object, 0x438 for its `BgTextCanvas` | | `MessageText.cpp` |

  A table is three words per object it serves, and an object points at the end of its three. A VTT of another
  class of the main program (`0x02110860`) points at `_ZTV13ObjTextCanvas` too.
- `gMessageFiles` (`0x02138F1C`): measured, the static initialiser constructs one `MessageFile` there with
  `__cxa_vec_ctor` and a count of 1, and registers `0x0202F408`, which destroys it with `__cxa_vec_cleanup` and is
  the last function of `MessageText.cpp`. Inferred: it is an object of a class whose only member is an array of one
  `MessageFile` and `0x0202F408` that class's implicit destructor; an array of one compiles to a single object, and
  this is the shape found that compiles to the ROM's code. The class name (`MessageFileList`) is ours.
- A message's tags start with the character 0x1A, followed by the tag's length in bytes; `MessageText::ExpandTags`
  replaces them with up to 20 strings and 2 messages set beforehand.
- The fonts are in a file of their own: `gFonts` comes before `gTextObjResource` in memory, and the compiler puts it
  after when both are defined in one file.

## The main program's static initialisers

`.init` (`0x020B04C8` to `0x020B0810`) holds 26 static initialisers. `.ctor` (`0x020FC494` to `0x020FC4FC`) holds
26 pointers to them, in the same order: the n-th initialiser's pointer is at `0x020FC494 + 4n`. Measured: the
initialisers of the decompiled files are in the order of those files' `.text` and `.bss`. No file after
`KeyboardCore.cpp` has one. So an initialiser lies between those of its neighbours, and a file's place in the code
is bounded by theirs. "Measured" in the table below means the initialiser's code shows the object. "Inferred" means
the file is placed by that order, and by which object of that part of `.bss` has an `Init` that `NitroMain` or
`NitroStartUp` calls. A lone `bx lr` is a global whose class has a constructor that does nothing, as
`gCardPullOutMonitor` shows. No code shows which object such an initialiser belongs to.

| `.init` | Size | File (code) | What it constructs | |
| --- | --- | --- | --- | --- |
| `0x020B04C8` | 4 | ScreenControl's, the first file of the game code (`0x0202320C` on) | `bx lr`: `data_0211D604` (`ScreenControl`, 0xF8 bytes: `ScreenControl_Init` clears 0xD4 + 0x24) | inferred |
| `0x020B04CC` | 4 | The archive manager's (`0x02024370` to `0x020249FC`) | `bx lr`: `gArchiveManager` (`ArchiveManager_Init` from `NitroStartUp`) | inferred |
| `0x020B04D0` | 4 | The graphics manager's (`0x020249FC` on) | `bx lr`: `gGraphicsManager` (0x10A4 bytes: `GraphicsManager_Init` clears 0x140, 0xC0, 0xEA0 = 0x48 entries of 0x34 from 0x204) | inferred |
| `0x020B04D4` | 0x18 | `src/MainHeap.cpp` (`0x0202678C`, `operator new`, to `0x02026A1C`) | `gMainHeap` (0x3C bytes): stores 0 at 0x1C and 0x20, the frame heap and its memory (`MainHeap::CreateFrameHeap` sets them, `MainHeap::DestroyFrameHeap` clears them) | measured |
| `0x020B04EC` | 4 | `src/PadInput.cpp` (`0x02026A1C` to `0x02026B48`) | `bx lr`: `data_0211F9D4` (`PadInput`, 0x24 bytes), whose constructor is empty | measured: the file compiles to it |
| `0x020B04F0` | 0x14 | The sprites' and OAM's (`0x02026B48` to `0x02027528`) | `gObjResourceManager` (0x19A8 bytes): tail call of its constructor `ObjResourceManager_Construct` (`0x0202709C`, out of line, in the middle of the file); no destructor | measured |
| `0x020B0504` | 0x14 | `src/PaletteLoader.cpp` (`0x02027528` to `0x02027A08`) | `data_021213B8` (`PaletteLoader`, 0x420 bytes): its inline constructor clears the queue's count at 0x400 | measured |
| `0x020B0518` | 4 | `src/OwnerInfo.cpp` (`0x02027A08` to `0x02027A7C`) | `bx lr`: `gOwnerInfo` (0x5C bytes: the `OSOwnerInfo`, the favourite colour at 0x54, the MAC address at 0x56), whose constructor is empty | measured: the file compiles to it |
| `0x020B051C` | 0x14 | The screen loader's (`0x02027A7C` to `SoundSystem.cpp`, `0x02028630`) | `data_02121834` (0x624 bytes): stores 0 at 0x480 | measured |
| `0x020B0530` | 4 | After `SoundSystem.cpp`: the touch screen's (`0x020287F8` on), or the file of `gRandom` or of `data_02121EB8` | `bx lr`: most likely `gTouchInput` (0x54 bytes; `TouchInput_Init`). `gRandom` is seeded by `NitroMain` itself | inferred |
| `0x020B0534` | 0x14 | The character loader's (`0x02028F20` on) | `gVramTransferQueue` (0x4F4 bytes): stores 0 at 0x4F0 | measured |
| `0x020B0548` | 0x84 | The models' (to `0x0202B0A8`) | `data_021223E4` (`ModelResources_Init`): an array of 0x20 entries of 0x1C (`__cxa_vec_ctor` with `ModelResource_Construct`, `ModelResource_Destruct`), `func_0202a640` on the camera at +0x380 (`data_02122764`), `func_0202a96c` on four lights of 0xC at +0x3E8 (`data_021227CC`, a loop, no destructor). Registers `ModelResources_Destruct` (`__cxa_vec_cleanup` of the 0x20 entries) with the record `data_021223D8` | measured |
| `0x020B05CC` | 4 | The 3D sprites' (`0x0202B0A8` or `0x0202B130` to `0x0202B6D0`) | `bx lr`: `data_021232E4` (the 3D sprites' texture list, 0x150 bytes: 12 entries of 0x1C, indexed by `G3Sprite::mTexture`) | inferred |
| `0x020B05D0` | 4 | `src/LcdPower.cpp` (`0x0202B6D0` to `0x0202B840`) | `bx lr`: `gLcdPower` (0xC bytes: `LcdPower::Update` turns the LCDs off after 20 frames of the lid closed and on after 10 open), whose constructor is empty | measured: the file compiles to it |
| `0x020B05D4` | 4 | `GameData`'s (`0x0202B840` to `Font.cpp`, `0x0202D5A4`) | `bx lr`: `gGameData` (`func_0202b840` from `NitroMain`) | inferred |
| `0x020B05D8` | 0x2C | `src/text/Font.cpp` | `gFonts`: three `Font`s through `__cxa_vec_ctor` | measured |
| `0x020B0604` | 4 | `src/CardPullOutMonitor.cpp` | `bx lr`: `gCardPullOutMonitor` | measured |
| `0x020B0608` | 0x48 | `src/text/MessageText.cpp` | `gMessageFiles` (an array of one `MessageFile`), destructor registered | measured |
| `0x020B0650` | 0x24 | MessageWindow's (at least `0x02032FB8` to `0x02034F94`) | A `const` pair at `data_021102D8` (`.data`, zero in the ROM) copied from words 0x30 and 0x34 of `data_020B5AF4`, a table of the file's `.rodata` | measured |
| `0x020B0674` | 0x2C | `src/sound/ValueRamp.cpp` | `gSoundManager`, destructor registered | measured |
| `0x020B06A0` | 0x2C | `src/scene/SceneManager.cpp` | `gSceneManager`, destructor registered | measured |
| `0x020B06CC` | 0x14 | `src/scene/EmptyScene.cpp` | `gSceneMemory` (`SceneMemory`'s constructor) | measured |
| `0x020B06E0` | 0x14 | `src/scene/WirelessTestScene.cpp` | `sCharacter` (`SavedCharacter`'s constructor) | measured |
| `0x020B06F4` | 0x88 | `src/scene/MiiListMenuScene.cpp` | `const` positions filled from other constants (`sMiiMenuTextPositions`) | measured |
| `0x020B077C` | 0x64 | `src/scene/MiiListScene.cpp` | `const` tables filled from other constants (`sTabSequences`) | measured |
| `0x020B07E0` | 0x30 | `src/keyboard/KeyboardCore.cpp` | `const` kana pairs filled from other constants (`sKanaPairDakuten`) | measured |

The first fifteen initialisers are the files of `0x0202320C` to `0x0202D5A4`. That is the gap after the run-time
library (`_ExitProcess`, `0x02023200`, is the library's last function) and before `Font.cpp`. Each of those files
except `SoundSystem.cpp` has its own part of `.bss`, in the same order (measured, from the addresses the code
uses):

| `.bss` | Size | Used by | |
| --- | --- | --- | --- |
| `0x0211D604` | 0xF8 | `ScreenControl_Init` (`0x0202320C`) and the functions up to `0x02024370`, which take it as `this` | ScreenControl |
| `0x0211D6FC` to `0x0211E8F4` | | `0x02024370` to `0x020249FC` (`data_0211D6FC`, `data_0211DFF4`, ..., and `gArchiveManager` as `this`) | Archives |
| `0x0211E8F4` | 0x10A4 | `GraphicsManager_Init`. The sprites use its 0x34-byte entries at their own addresses (`data_0211EAF8`, ...) | Graphics |
| `0x0211F998` | 0x3C | `0x0202678C` to `0x02026A1C`: an expanded heap at 0, a mutex at 4, a frame heap and its memory at 0x1C and 0x20, a mutex at 0x24 | Main heap |
| `0x0211F9D4` | 0x24 | `PadInput::Init`, `PadInput::Update`; +0x1C is the lid's state | `PadInput.cpp` |
| `0x0211F9F8`, `0x0211FA10` | 0x18, 0x19A8 | The OAM functions (`0x02026EEC` to `0x02027518`); `gObjResourceManager` (`func_02027480` uses +0x198C), with `data_02120948`, `data_02120A10` and `data_021212A8` inside it | Sprites and OAM |
| `0x021213B8` | 0x420 | `PaletteLoader_Init` (`0x02027528`, clears 0x414 to 0x41C) | `PaletteLoader.cpp` (also `.rodata` `0x020B599C` to `0x020B59EC`, the table of 20 loaders, and `.data` `0x0210F628` to `0x0210F630`, `".NCLR"`) |
| `0x021217D8` | 0x5C | `gOwnerInfo` (`OwnerInfo::Init`) | `OwnerInfo.cpp` |
| `0x02121834` | 0x624 | `ScreenLoader_Init` (clears 0x484 and 0x5C4, and eight 0x1C entries at 0x4F8) | Screens |
| `0x02121E58`, `0x02121EAC`, `0x02121EB8` | 0x54, 0xC, 0x2C | `gTouchInput` (`TouchInput_Init`, `0x02028880`); `gRandom`; the path buffer of `func_02028af4` | Touch, random numbers, paths (two or three files) |
| `0x02121EE4` | 0x4F4 | `gVramTransferQueue` (`VramTransferQueue_Init` is a `bx lr`) | Characters |
| `0x021223D8` to `0x021232E4` | | The destructor record, `data_021223E4` with the camera and lights inside it (`ModelResources_Init` clears +0x418 and +0x41C), and `data_02123024` (`0x020299E8`, `0x02029D40`) | Models |
| `0x021232E4` | 0x150 | `func_0202b2e8`, `func_0202b4dc` (at their own addresses `data_021232EC` to `data_021232FC`: fields of the entries, which it indexes with a stride of 0x1C) | 3D sprites |
| `0x02123434` | 0xC | `gLcdPower` (`LcdPower::Init`, `LcdPower::Update`) | `LcdPower.cpp` |
| `0x02123440` to `0x02138E1C` | | `data_02123440`, `data_02123444`, `gGameData` | GameData |

Inferred from this:

- Twelve file boundaries in that gap. The game code begins at `0x0202320C`. The archive manager is `0x02024370` to
  `0x020249FC`. The graphics code ends at `0x0202678C`: its last two functions, `0x02026754` and `0x02026770`, are
  deleting destructors, which come at the end of a file. The heap is `0x0202678C` to `0x02026A1C` (cut and decompiled: `src/MainHeap.cpp`). The pad is
  `0x02026A1C` to `0x02026B48` (cut and decompiled: `src/PadInput.cpp`). The sprites and OAM are `0x02026B48` to `0x02027528`. The palettes are `0x02027528`
  to `0x02027A08` (cut and decompiled: `src/PaletteLoader.cpp`). The owner information is `0x02027A08` to `0x02027A7C` (cut and decompiled: `src/OwnerInfo.cpp`). The screens are `0x02027A7C` to
  `0x02028630`. Touch starts at `0x020287F8`. The characters start at `0x02028F20`. The models end at `0x0202B0A8`:
  its last functions are the destructors and the constructor that the initialiser uses, and the implicit destructor
  `ModelResources_Destruct` is the file's last function. The screens' power is `0x0202B6D0` to `0x0202B840` (cut and decompiled: `src/LcdPower.cpp`).
  GameData is `0x0202B840` to `0x0202D5A4`. The graphics code between `0x020249FC` and `0x0202678C` may be more than
  one file: only one part of `.bss` belongs to it.
- `data_021223E4` is one object of a class (called ModelResources here) that holds the 0x20 resource entries, the
  camera at +0x380 and four lights at +0x3E8. The initialiser computes the lights' end from `data_021223E4`, and
  `ModelResources_Init` writes +0x418 and +0x41C. The registered destructor destroys only the array, because the
  camera and the lights have trivial destructors. `func_0202a640` and `func_0202a96c` set a camera's and a light's
  defaults: either they are the constructors, out of line, or inline constructors call them.
- MessageWindow's file has `.init` `0x020B0650` to `0x020B0674`, `.ctor` `0x020FC4DC` to `0x020FC4E0`, `.rodata`
  `0x020B5AEC` to `0x020B5C44` (every object there is used by `func_02032fb8` to `func_020341d4`), `.data`
  `0x021102D8` to `0x021102F0` (the pair, then `MessageWindow`'s virtual table at `0x021102E0`, whose two functions
  are `0x02034A94` and `0x02034BC4`), and no `.bss`. `wh.cpp`'s `.data` ends at `0x021102D8` and `TextUtil.cpp`'s
  begins at `0x021102F0`. Its initialiser is the pattern of `MiiListMenuScene.cpp`: a `const` aggregate initialised
  from another constant, `{data_020B5AF4[k].a, data_020B5AF4[k].b}` or similar.
- When one of these files is cut, its `.init` and `.ctor` ranges are in the table above, and the initialiser's name
  becomes `__sinit_<File>.cpp`. Until then the initialisers keep their addresses as names.

## The apartment scene

Overlay 74 is the apartment: the scene `ScenePlayRoom2` with the messages of `tmd_mainroom_msg_JP.bmg`. Measured:
254 functions, 146 KB, of which 104 KB is 15 functions of 2 to 13 KB each. Decompiled in
`src/overlays/ov074/RoomScene.cpp`, one unit that claims every section of the overlay; 250 functions match.

| Range | Holds |
| --- | --- |
| `0x02149A40` to `0x0214E5F8` | `RoomScene` (0x1D7C8 bytes, scene ids 0x11 to 0x13): loading, input, the building front's buttons, room changes, and `UpdateEvent`, the 17 states of what happens in the room over time |
| to `0x02152690` | What two characters in a room do to each other, and `RoomCharacter`'s constructor, state setter and `Update` |
| to `0x02168D24` | `RoomCharacter`'s states 0x01 to 0x117, in 15 functions by range of states, then its walking, animation and floor-area helpers |
| to `0x0216D400` | The rest of `RoomCharacter` (head turning, walking steps, held items, sounds), `HeldItem`, `ApartmentRoom`, `RoomCamera`, `RoomLight`, `ApartmentFront` with its cursor and buttons, `SkyBackground`, `NamePanel`, `TouchButton`, `Clock`, `ClockDisplay`, `TimedMessage`, and the compiler's implicit destructors (`HeldItem`'s deleting one, the scene's two) |
| `.init`, `.ctor` | `__sinit_RoomScene.cpp`, which computes the walk area's size and corners |

- A `RoomCharacter` (0x2D98 bytes, two per scene) is a state machine of 0x118 states; `RoomCharacter::Update`
  dispatches on the state to the functions above, each a `switch` over a range of states. In the states for two
  characters, the one in slot 0 decides for both.
- The state functions are named by their range of states where the code does not show what the activity is.
- The classes are in the headers beside the source (`RoomScene.h`, `RoomCharacter.h`, `Character.h`, `Model.h`,
  `HeldItem.h`, `Room.h`, `RoomCamera.h`, `FloorPos.h` and one for each of the scene's other objects).
- Measured: the scene's speech player is a class derived from `TtsPlayer` with nothing added. The scene's
  constructor (main program, `0x0203ED10`) builds it with `TtsPlayer`'s base-object constructor, the only call of
  that copy in the game, and the scene's destructor destroys it with the base-object destructor.
- Measured: the implicit destructors of `RoomCharacter`, `HeldItem` and `RoomPart` that the scene's destructor calls
  are the main program's copies (`0x0203EEE4`, `0x0203EF44`, `0x0203EF80`), after the scene's constructor; only
  `HeldItem`'s deleting destructor is in the overlay.

Overlay 73 is the tutorial's apartment (`ScenePlayRoomTut`, scene id 0x6B): 133 functions, 29 KB. Decompiled in
`src/overlays/ov073/` as one file, `TutorialRoomScene.cpp`, which `delinks.txt` gives every section: 131 of 133
match.

- 78 of its functions are byte for byte overlay 74's, but it is not the same code throughout: its character
  (`TutorialRoomCharacter`) is 0x2B94 bytes with 18 states and no held items, its camera 0x5C bytes without the
  close-up mode, its `TouchSprite` and `TouchButton` four bytes smaller, and the building front cannot be touched.
- The program is linked as one, so two classes cannot share a name: overlay 73's are named with the prefix
  `Tutorial` in the source.
- Measured: the compiler heap-sorts a file's data objects by size and pools the four-byte-aligned constants of
  `.rodata` in that order (details in `docs/notes/ov073.md`). Overlay 73's file gives the constants that the code reads
  off the pool's start (its 8-byte points) the ROM's offsets, but the rest of its `.rodata` and its `.data` are in
  the ROM in an order that is not by size, which our compiler does not give: the data does not match, and the unit
  is not complete.

Inferred, from several measurements that agree: each of the two overlays is ONE source file. Overlay 73's small
constants are in one pool that functions throughout it address by offset; overlay 74 has one static initialiser,
whose objects sort into one `.data` pool with `HeldItem`'s table; and each scene's implicit destructor is at the
very end. Overlay 74 is now one file, and its two implicit destructors come out of the compiler there by themselves.

The data of overlay 74, measured (by compiling test files with the game's flags, and from the ROM):

- `.init` and `.ctor` come out as the ROM has them. In `.data` the virtual tables, the construction tables and VTT of
  `TimedMessage` (whose `TextCanvas` is a virtual base), the walk area and `sHeldItemDigits` are where the ROM has
  them; the linker script aligns every section to 4 (`ALIGNALL(4)`), which the 3-byte strings need.
- What is not reproduced: the order of the anonymous strings `"00"` to `"10"` of `sHeldItemDigits` (the ROM has
  10, 02, 08, 06, 04, 09, 01, 03, 07, 05, 00; ours depends on the numbers the compiler gives anonymous objects, and
  no simple hash of them explains either order), and `.rodata`.
- `.rodata`: a file's 4-aligned constants go into one section sorted by size, read at the section's address plus an
  offset; a 1-aligned array gets a section of its own. In the ROM, `0x0216D404` to `0x0216D484` is sixteen 8-byte
  objects at the start of such a pool (what dsd took for a 13-entry table `gRoomPoints`, `sButtonSequences`, and two
  more points), then the 10-, 12-, 16-, 20-, 22- and 44-byte ones in order of size up to `0x0216D5C8`; after them
  come `sButtonRectOther`, `sButtonRect0`, `sButtonRect1`, `sHeldItemHasPattern`, `sSkyPictures` and
  `sClockPositions`, outside that order. Most of the small ones are local initialisers (`FloorPos p = { x, z };`)
  in the original: `StartPairEvent` matches but for the offset of such a constant.

Not done:

- 5 functions that are written and differ, each with a comment saying how: in overlay 74 `ChooseReactions` (the
  callee-saved registers named differently), `StartPairEvent` (the offset of a pooled constant), `GetAngleAround`
  (one copy and two registers of one vector, 95%) and `ApartmentFront::UpdateCursorTarget`; in overlay 73
  `TutorialApartmentFront::UpdateCursorTarget`. Both copies of `ClockDisplay::Refresh` match since the hour's ones
  digit is subtracted in place (`hour -= tens * 10;`, as overlay 72's `MapClockDisplay::Refresh`). Both copies of
  `NamePanel::Init` match: the character counts read through a pointer to the canvas, and both text positions set
  with `SetPositionFromVec`. `StartMeeting` matches with the distances taken straight off `GetFloorPos`'s results.
- The data of both files, above: the order of the objects after the pool of small constants, and of overlay 73's
  `.data`, is not one our compiler gives, so neither unit is complete. Overlay 73's notes have the rule for the
  order the compiler does give, by which its pool is reproduced.
- `tools/check_relocs.py` lists, for overlay 74, only string literals and `data_020f7300`, which is
  `data_020f72f8[0].mLightColor` (other overlays refer to it by that name too); for overlay 73, string literals and
  three anonymous vectors.
- `docs/notes/ov074.md` and `docs/notes/ov073.md` keep the working notes, with the evidence for names and layouts.

## The wedding announcement

Overlay 96, `src/overlays/ov096/WeddingScene.cpp`: all 37 of its functions match, and its data is the ROM's:
complete.

Measured: the scene `WeddingScene` (0x1CEE0 bytes, constructed in the main program at `0x02048600` after
`FS_LoadOverlay(0x60)`; its virtual table at `0x0214C860`) shows two characters in front of the camera. Its messages
are trouble messages 0x24F to 0x259: the first says they are getting married, the second thanks the player, the first
promises a happy life, a window says that the two got married (0x256), and the second hands the player "the wedding
album" (item 0xAA of the item messages) as a keepsake. The album's model grows in the character's hands over the
second half of the `i_give01` animation; then `InventoryItem::Add(1)` puts it in the inventory and the scene asks for
scene 0xC0. Coming back from scene 0xC0 (the characters' indices are kept at `data_0213ba74 + 0x20`), the first says
they are off on their honeymoon; each character's `0x230` gets the time the honeymoon ends (now plus minutes from an
event table, `data_02114d78`) and its `0x23A` is set to 3, and the scene asks for scene 0x77.

The overlay's classes are its own versions of what the Mii-touch scenes (overlays 75 and 101 to 104) share: several
of its functions are byte for byte the same there. The main program's classes it uses are named from it:
`SpeechWindow` (a message window that hides itself when the speech engine is done, or after a number of frames),
`Inventory` and `InventoryItem` (236 item counts read from and written back to the save data, at most 99 of each) and
`ItemModel` (the model `Mdl/Item/item%03d_3D_LZ.bin` of an item, scaled by the item table's size).

The data: `.rodata` starts with an 8-byte object `{25, 128}` (0x0214C80C) that nothing in the ROM refers to; it is
in the source as `data_ov096_0214c80c`, kept by an uncalled stand-in that the link strips, and the four 12-byte
vectors are declared in the order that sorts them as the ROM has them. The scene's four message files are opened as
`"TCM:tmd_common_msg_JP.bmg"` and so on, without the slash after the colon that the source had (the strings are
the ROM's now, and with them the offsets of everything after them in `.data`).

## The message window's buttons

Measured (fix-link) on the window's update, `func_020341d4`: it sets +0xAF7C, +0xAF80 and +0xAF84 to -1 at the
start of each frame, then three helpers walk the window's sprite buttons (count +0xAD58, the numbers they were
added with at +0xAD68) and text buttons (count +0xAD5C, numbers at +0xAD98) and store the number of the first one
that reports:

| Field | Set by | Means | Name |
| --- | --- | --- | --- |
| +0xAF7C | `func_02033f6c` | the stylus came down on it this frame (`func_02031294`, the touch point inside the button's rectangle, and `gTouchInput.mTouched`) | `mTouchedButton` (overlay 76's choices) |
| +0xAF80 | `func_0203404c` | its update, `func_0203133c`, returned TRUE: it was pressed. Open (`func_02034720`) also clears it | `mResult` |
| +0xAF84 | `func_02034110` | `func_02031ccc`: the button's +0xB4 flag, which its draw (`func_02031d7c`) sets when the pressed sequence has ended (kinds 0, 4, 5) | `mFinishedButton` (the main program's list scenes) |

So a scene that reacts at once reads +0xAF80, one that waits for the button's animation +0xAF84 (inferred from
the flags; the names are descriptive).

## The save screens

Three small overlays put a message window (the main program's, 0xAFA4 bytes, the same stand-in as overlay 73's, now
`include/MessageWindow.h`) on the lower screen to say something about the save data. Each is one source file under
`src/overlays/ovNNN/`, every function matches, and each unit is complete: every section is placed and linked from
source.

| Overlay | Scene | Class | Functions | What it shows (etc messages) |
| --- | --- | --- | --- | --- |
| 69 | 0x2C, 0x2D | `SavePlayScene` (0x6DB84 bytes) | 20 | 0x17C to 0x180: save what has happened so far? / saving / saved, go on playing? / save again when you stop / turn the power off now. Scene 0x2D comes before wireless play: 0x181, 0x182 |
| 106 | 0x39 | `PreStartScene` (0xB13C bytes) | 17 | 0x1A4 the data could not be read; 0x1A7, 0x17D, 0x1A8 the data is broken and is initialised; 0x1A2, 0x1A3 this is another console than last time |
| 107 | 0xC2 | `SaveErrorScene` (0xAFEC bytes) | 10 | 0x1A6 the data could not be written: turn the power off and put the card back in |

Measured:

- The scene factory (`CreateScene`) loads each overlay and calls a constructor in the main program
  (`_ZN13SavePlaySceneC1Ei`, `_ZN13PreStartSceneC1Ei`, `_ZN14SaveErrorSceneC1Ei`), which stores the overlay's virtual
  table. Two of them call a constructor that is the overlay's own: overlay 69's camera (0x0214AB6C) and overlay 106's
  clock (0x0214A624). The compiler emits such a constructor as C1 and C2; the link keeps the one the main program
  calls, so the ROM has one copy.
- Overlays 69 and 106 ask for scene 0xC2 when the main program's write (`func_0202cd48`) fails.
- `PreStartScene` reads `gGameData+8` as the outcome of loading the save data (0: it goes on; 1: it goes on as on
  a first start, clearing the flag below; 2: the data is broken and initialised; 3 and 4: it stops), and
  `gGameData+0xB1` as a flag: when it is set, it lists the residents and runs the main program's functions around
  `EncounterJudge`'s (0x020ad294 to 0x020aef80) on them with the clock's time, then asks for scene 0x6A; when it is
  not, it sets it and asks for scene 0x69. The warning about another console comes when the game clock's +0x34
  (`data_02123440`) is set.
- `SavePlayScene` shows one resident, picked at random, with a random animation of `Mdl/Ica/ica_mii_save_LZ.bin`.
  The character is an array of one: the scene's destructor calls `__cxa_vec_cleanup` with a count of 1 and the main
  program's `_ZN17SavePlayCharacterD1Ev`. 0x60000 bytes of the scene, from 0xDB14, are touched by nothing in the
  overlay or the constructor.
- The three files' six constants (window rectangle, button positions and size, text position, the yes and no
  messages) are the same objects, and come out in two orders in the ROM: overlays 69 and 107 have one, overlay 106
  another. Our compiler gives both, from two declaration orders (the size sort permutes objects of one size by
  their place in the list, `docs/notes/ov073.md`).

Inferred: the three are one family, written from the same code. Their `Init`s set the window up alike, their window
helpers are byte for byte the same (`ShowMessage` in all three, `ShowOk` and `Ask` in 69 and 106), and overlay
107's `Init` gives the window overlay 106's first message, 0x1A2, which it replaces before the window is shown.

objdiff reports the data of all three a little under 100% although the ROM is identical: it counts the
padding at the end of `.rodata` and `.data` that the delinked original has and the compiled object does not.

## The Nintendo logo (overlay 105)

Overlay 105, `src/overlays/ov105/NintendoLogoScene.cpp`: all 8 of its functions match, and its data is the ROM's
(the virtual table and six strings): complete.

Measured:

- `NintendoLogoScene` (0x1C bytes; its constructor is the main program's, `0x0203DE78`) mounts
  "Cmn/Nin/NintendoLogo_LZ.bin" as "MNU" and sets both screens' fade speed to 0x19C2 of 0x10000 a frame, so that
  its fades take ten frames (`func_02023c54(&data_0211d604, 0x3F3F, 0x19C2)`: it writes the screens' +0x0C and
  +0x10, the steps `func_02023cd4` and `func_02023ddc` add to their brightness, +0x04 and +0x08, each frame). `Init`
  loads `MNU:/Gray_White.nsc`, `.ncg` and `.ncl` into plane 2 (one of the main engine's, whose planes are 0x3F in
  the screens' masks), shows that plane alone, and has `func_020278f8` set colour 0x7FFF with 0x101, as
  `PreStartScene` does. `Update` counts frames in its one state and on the 61st asks for scene 0x39
  (`PreStartScene`, overlay 106). `Exit` unmounts "MNU", `Draw` is empty, and the field at +0x14 is cleared by
  `SetState` and read by nothing.
- Scene ids 0x00, 0x0D, 0x28 and 0xB6 all construct it, and nothing differs between them: in `CreateScene` each case
  sets sound group -1, loads overlay 105 and constructs the same class, `UnloadSceneOverlay` unloads overlay 105
  for each, and the scene never reads `mSceneId`: whichever id started it, it goes on to 0x39. They differ only in
  who asks for them:
  - 0xB6: `SceneManager::Init` constructs it, as the game's first scene.
  - 0x00: overlays 101 (`SceneAF`) and 103 (`SceneB3`) ask for it when they finish, unless the scene manager's
    previous scene (`mPreviousSceneId`) is 0xAF (overlay 101) or 0x40 (overlay 103), when they ask for 0x40
    instead. Overlay 90 (`RomanceScene`, 0x0214B5A4) calls `RequestScene(0)` too, but with an address in its own
    `.rodata` (0x0214C478) as the scene manager.
  - 0x0D and 0x28: no call of `RequestScene` in the delinked or compiled code passes either as a constant.

Inferred: scene 0 is the way back to the start of the game, through the logo and `PreStartScene`'s check of the
save data.
## The tutorial's steps (overlay 108)

Overlay 108, `src/overlays/ov108/TutorialStepScene.cpp`: all 7 of its functions match, and the unit is complete
(its `.data` is the virtual table, 0x24 bytes, and alignment padding to 0x40).

Measured:

- `TutorialStepScene` (scene 0x69; `CreateScene` loads overlay 108 without setting a sound group and allocates
  0x10 bytes; constructor `_ZN17TutorialStepSceneC1Ei`, 0x02045B44, in the main program; virtual table at
  0x02149DC0) has no members of its own. `Init`, `Update`, `Draw` and `Exit` are empty: everything is in
  `StartLoading`, which adds one to the tutorial's step (`gSceneMemory.mTutorialStep`, the byte at
  `data_0213ba74 + 0x1E`) while it is below 24 and asks for the scene of the new step:

  | Steps | Scene | What it is |
  | --- | --- | --- |
  | 1 | 0xAD | the first start's settings (overlay 98, `OptionScene`: the clock) |
  | 2, 7, 15, 18, 20, 23 | 0x6D | the island (overlay 72, `MapScene`) |
  | 4 | 0x6E | the island, without setting the sound group |
  | 3 | 0x72 | overlay 93 |
  | 5, 10 | 0x6F | the Mii maker (overlay 76, `MiiEditScene`) |
  | 6, 11 | 0x7E | overlay 94 |
  | 8, 13, 16, 21 | 0x6B | the tutorial's apartment (overlay 73, `TutorialRoomScene`) |
  | 9, 12, 17, 22 | 0x6C | overlay 75 |
  | 14 | 0x7D | overlay 77 |
  | 19 | 0x71 | overlay 85 |

  The step after 23 stays 24: it ends the tutorial. Of the saved characters that exist (`func_0202c3dc`, indices 0
  to 99), the first two are read (`func_0202c264`), have `ClearLongAct` and `ClearTrouble` applied and are written
  back (`func_0202c28c`). Then saved character 0 is read, cleared of trouble and given trouble id 0 of category 9 in
  state 1, ending at 0x7FFFFFFFFFFFFFFF (never), and written back; the scene asks for scene 0x5F, the island.
  Step 0 cannot occur (the jump table's entry 0 returns).
- The scenes that ask for scene 0x69 are those of the tutorial: `PreStartScene` (overlay 106) on a first start,
  `OptionScene` after the first start's clock (scene 0xAD), `MapScene` at the end of its tutorial visits,
  `TutorialRoomScene` and `MiiEditScene`.

Inferred: the steps the island handles (2, 4, 7, 15, 18, 20, 23) and those overlay 73 handles (8, 13, 16, 21) are
the same numbers these scenes switch on, so each scene of the tutorial plays the step this scene chose and comes
back here. What trouble category 9, which saved character 0 is left with, is, is not known.

## The name sort test

Overlay 1, `src/overlays/ov001/NameSortTestScene.cpp`: all 10 of its functions match, and its data is the ROM's:
complete.

Measured: scene 0x0C, `NameSortTestScene` (0x2434 bytes, constructed in the main program at `0x0203E6F0`; its
virtual table at `0x02149F00`), is a test of the resident list's name orders (`CharacterSort`, main program). It
loads nothing and draws nothing. In `Init` it copies the string "かー～ヴヶa　" (the only other thing in `.data`,
after the table) into the scene and gives its eight `SavedCharacter`s empty Mii names. Then:

- A: `CharacterSort::ToUpperCase` on the string.
- Y: each character gets a random name and family name (ten characters 0..255 from `gRandom`, the name starting
  "あ!" or, for odd characters, just "あ"), and `MATH_QSort` sorts them with `CharacterSort::CompareByName`, its
  stack in the scene (`+0x1654`, 0x320 bytes).
- B: each character gets a random Mii name (`MiiData_SetName`) and its `mIndex`, and a thread of the scene's own
  (stack 0xA00 bytes, priority 26) sorts them with `CharacterSort::CompareByMiiName` and no stack buffer; `Update`
  waits until the thread has terminated.

Everything it would show goes to two member functions that are empty in this build (one keeps a `wcslen` of its
string): what is left of debug prints, called with a line (0, 1), a name and 10. `Init` also keeps the `clz` of an
unused `MATH_QSortStackSize(8)`, which the compiler does not remove because the SDK writes it as inline assembly.

Inferred: the 0x320-byte sort stack is far more than `MATH_QSortStackSize(8)` (32 bytes); its size is what the
layout leaves between the characters and the thread's stack, assuming the stack ends at the thread as in
`TtsPlayer`.

## Three scenes of the main program

Not every scene is an overlay. Three are in the main program, `0x0204FC74` to `0x02051640`, between a family of
helpers that many overlays call and a fourth scene of the main program (`0x02051640`, which mounts the sky, vote and
apartment models; not done). Each is one source file under `src/scene/`, and each is reached only through its
virtual table: the scene factory (`CreateScene`) calls a constructor in the main program's file of scene
constructors, which stores it. No overlay is loaded for them.

| Scene | Class | Size | Constructor | File | Functions |
| --- | --- | --- | --- | --- | --- |
| 0x24 | `MiiListMenuScene` | 0x2C0C0 | `func_02040288` | `MiiListMenuScene.cpp`, `0x0204FC74` to `0x02050884` | 13 of 13, complete |
| 0x25 | `G3SpriteTestScene` | 0x48B4 | `func_0204036c` | `G3SpriteTestScene.cpp`, to `0x02050D70` | 9 of 9, complete |
| 0x26 | `MiiListScene` | 0x2AA44 | `func_020403e8` | `MiiListScene.cpp`, to `0x02051640` | 10 of 10, complete |

Measured:

- They are three files, not one. Each file's `.data` is its data objects, then its virtual table, then the strings
  its code uses, in the order it uses them; scene 0x24's strings come before scene 0x25's table, and scene 0x25's
  before scene 0x26's data, which one file does not give. Scenes 0x24 and 0x26 have a static initialiser each
  (`0x020B06F4`, `0x020B077C`), and a file has at most one. Their `.rodata` follows the same order.
- Scene 0x24 shows two buttons at the top of the touch screen. The left one opens a menu of what to do with a Mii
  (etc messages 0x75 edit, 0x77 change its clothes, 0x79 erase, and a cancel button); "erase" asks whether the Mii
  is to move off the island (0x7A, with the placeholder name "「削除したいMii」") and then says that it has gone
  (0x7E, "「削除したMii」"). The right one opens a menu of the seven orders the list can be sorted in (0x7F to
  0x85: name, nickname, sex, year of birth, birthday, blood type, personality). Nothing is done with any choice:
  inferred, a mock-up of the Mii list's two menus.
- Scene 0x26 is the list of Miis (the main program's list, panel and five tabs at `0x020A03E8` on, which this scene
  only calls) with the sort menu of scene 0x24. It sorts the list by `gGameData+0x4D` (`mMiiListSortKey`) and writes
  the order chosen back there on leaving when it changed.
- Scene 0x25 mounts `Scn/Scene/SceneTestG3Sprite_LZ.bin` and a body model, puts a hundred 3D sprites
  (`G3Sprite`, 0x4C bytes, functions `0x0202B0E8` to `0x0202B6D0`) of `M00:/Face.tga` at random places and sizes in
  front of it, and moves one of them each frame: a test.
- No call of `func_0203c02c` (which asks for the next scene) has 0x24 to 0x27 put in its argument by an
  instruction just before it. Whether anything reaches these scenes, by a table or a computed id, is not known.
- These scenes act on the message window's +0xAF84 (measured on all four of scene 0x24's windows, which
  start at +0x230 where the constructor stores the window's table), and keep what to do after a window closes at
  +0xAF98. What +0xAF84 is: see "The message window's buttons" below.
- `_ZTV5Model` and `_ZTV9AnimModel` were named 8 bytes into their tables; they are at `0x0210F640` and `0x0210F65C`
  now, and every reference to them is `add:0x8`. Linking scene 0x25's destructor from source showed it.

Two things the compiler does that these files needed:

- **Text positions that are not constants.** `.data` of scenes 0x24 and 0x26 holds arrays of points that are zero
  in the ROM and filled by the static initialiser, in one run of stores from one base. They are the menu buttons'
  positions moved by a fixed amount (`{sPos[0].x - 0x20, sPos[0].y - 6}`): an initialiser that reads another
  constant object is not a constant expression, so the compiler initialises it at run time, and as `const` it goes
  in `.data` (a non-`const` one goes in `.bss`). That this is how the source had it is inferred: it gives the bytes.
- **Two files opened through one pointer.** `MiiListMenuScene::Init` opens its two message files (members at
  +0x200 and +0x218) through one pointer to the first, as `files[1]` and then `files[0]`. Written as two members,
  the second `Open` computes the file's address before it loads the path, where the ROM does the reverse; an inline
  helper that takes the path first fixes that call but reschedules the code before it. That the source had a
  pointer or an array here is inferred: it is the form found that gives the bytes (an array member changes the
  destructor).
- **A local copied inside a loop is hoisted.** Scene 0x26's sort texts take an offset by value; written as a local
  initialised inside the loop, the compiler loads it once before the loop and gives its stack slot the place the
  ROM has. Declared before the loop it gets another slot.

Not done:

- The constructors are in the scene factory's file (`src/scene/SceneManager.cpp`); see "The scene constructors".

## The main program from 0x0203671C to 0x02040578

Cut into files by gap38-a. Measured, from the decompiled code and the delinked sections:

| File | `.text` | Other sections | |
| --- | --- | --- | --- |
| `src/sound/ValueRamp.cpp` (end) | `0x0203671C` | `.init`, `.ctor`, `.bss` `0x0213B5C0` to `0x0213B6B0` | `gSoundManager` (0xE4 bytes) is defined in the file of `ValueRamp`: the static initialiser constructs it (`SoundSystem`'s inline constructor) and registers its destructor `0x0203671C` |
| `src/text/TextUtil.cpp` | `0x0203673C` to `0x02039660` | `.data` `0x021102F0` to `0x0211085C` | Wide-string helpers (`include/text/TextUtil.h`): numbers in full-width digits, and text made ready for the speech engine |
| `src/text/SpeechWindow.cpp` | `0x02039660` to `0x020398E4` | `.rodata` `0x020B5C54` (`sTextPosition`, {16, 162}) | The window of what a character says, on sprites, hidden when the speech engine is done or after a number of frames |
| `src/save/SaveBlocks.cpp` | `0x020398E4` to `0x0203B748` | `.rodata` `0x020B5C5C` to `0x020B5CFC` | The blocks of the save data, members of `gGameData` |
| `src/wireless/WirelessSession.cpp` | `0x0203B748` to `0x0203BCE0` | `.rodata` `0x020B5CFC` to `0x020B5D04`, `.bss` `0x0213B6B0` to `0x0213B700` | The game's layer over WH for finding a parent DS and connecting as a child (overlays 109 and 110) |
| `src/scene/SceneManager.cpp` | `0x0203BCE0` to `0x0204AF20` | `.init` `0x020B06A0`, `.ctor` `0x020FC4E4`, `.data` `0x0211085C` to `0x02110B94`, `.bss` `0x0213B700` to `0x0213BA74` | The scene manager, the scene factory and every scene's constructor |

- **Text for the speech engine** (`TextUtil.cpp`, complete). `SplitSyllables` (`0x02037568`, 8,300 bytes) cuts a
  katakana text into what the speech engine counts as syllables, morae in effect: a kana with the small kana that go
  with it, three characters first (20 of them, `スゥィ`, `ドゥァ`, `ヴィェ`; `スゥィ` is listed twice), then two (145,
  `キャ` to `ヅィ`, some twice: the rows of `ギャ`, `ビャ`, `ヒャ`, `ジャ`, `チャ` and `ウィ`), else one character, so that a
  lone small kana, `ッ`, `ン` and `ー` are syllables of their own. Measured from its compare chains. Its callers
  outside the file are overlays 8 (`0x0214A54C`) and 92 (`0x02150A74`, `0x0215190C`), with a buffer;
  `CountSyllables` calls it without.
- **Three tables the link left out.** The file's `.data` holds, besides the readings of the letters and digits, 219
  wide strings that nothing in the ROM refers to: the 73 kana (gojūon order, `ゐ` and `ゑ` included, the voiced rows
  after `を`, `ん` last) in hiragana, in katakana, and in letters of two at most (`KA`, `SH` for `し`, `CH`, `TS`, `FU`,
  `JI` for both `じ` and `ぢ`). Measured: their order in the pool, which is the compiler's heap sort of the file's
  data list (decompiling.md, "How a file's data is laid out"), is reproduced exactly, all 255 strings, only by three
  arrays of 73 pointers declared in that order between `sLetterReadings` and `sDigitReadings`; as objects of 292
  bytes each is in a section of its own, which the link dropped with whatever used them, while the strings, in the
  pool, stayed. Inferred: the game once converted kana to letters, or the reverse, in this file.
- **Save blocks.** Each block is its data as it is in the card's backup memory (a 16-byte magic string first, a
  checksum, `GameData::Checksum` at `0x0202CD00`, last), followed by two flags: read, and valid. Each has its place
  in each of two slots of 0x40000 bytes. The ten magic strings are `.rodata` in an order that is not the order of
  the functions that use them; the source declares them in the ROM's order. Two of them have hyphens:
  "TOMODACHI-FAIRY" and "N-EAST-WEST-S". The card is locked around each access (`CARD_LockBackup`,
  `CARD_UnlockBackup` at `0x02010594`, `0x020105A4`, with `gGameData.mCardLockId`).
- **Wireless session.** A parent is one of this game only if its user game information (the owner's nickname, 15
  characters) ends with the same 16-bit id as this DS's. The scene counts each parent's timer down: 600 when it is
  found, 599 when it is seen again. `gWirelessSession` is defined in the scene manager's file: it is the first
  object of that file's `.bss` (`0x0213B700`), before `gSceneManager`'s destructor record, and the scene
  manager's static initialiser constructs it.
- **The scene manager** (`gSceneManager`, 0x364 bytes, `include/SceneManager.h`): `Update` runs the scene; when it
  is done, deletes it, unloads its overlay (`UnloadSceneOverlay`), constructs the next (`CreateScene`, which loads
  the scene's overlay and sound group) and restores the graphics managers to the state `Init` recorded (three
  buffers of 0x140, 0x140 and 0xD4 bytes). `RequestScene` (`0x0203C02C`) is what the overlays call. `CreateScene`'s
  overlay ids are `overlay_id` relocations (`include/Overlay.h`).
- **Scene constructors 0x00 to 0x26** follow `CreateScene` in id order, as those after them do (next section).
  The class tables of `Scene` (`0x02110970`), `ObjTextBox` (VTT `0x02110864`, table `0x021108AC`) and `ObjCharBox`
  (VTT `0x0211086C`, table `0x02110900`) are this file's, as are the main program's `TestDrawMiiAnimModel`'s
  (`0x02110954`, D1 `0x0203E2E4`, D0 `0x0204AD88`).
- Every scene's constructor takes the scene id (`r1`, stored at +4), including `RoomScene`, `TutorialRoomScene` and
  `WeddingScene`'s, which the symbols had as taking nothing.

Inferred: scene 0x04's overlay tests a character's body (`CharacterBodyTestScene`, the name overlay 4's agent gave
from its code); scenes 0x1F to 0x22 are the right-job scenes after their archives `SceneRightJob` and
`SceneRightJob2`.

Not done in this range: `SceneManager::Init` (92.6%: six loads and constants scheduled differently at its end).
Every scene constructor of 0x00 to 0x26 matches: `Scene1E`'s since its part at +0x1C8 is the shops' `ShopCounter`,
`RoomScene`'s, `TutorialRoomScene`'s and `VoteScene`'s since overlays 73 and 74 moved onto the shared headers and
`G3Sprite`'s destructor became inline (below).

- **The drawing canvas** (`DrawingCanvas`, `include/DrawingCanvas.h`, measured): `0x02025A9C`/`0x02025B5C` are C2
  and C1 of a 0x2B4-byte class that draws lines into a 128 by 96 pixel rectangle of a background's 4-bit
  characters with the touch pen: `Update` (`0x02025C8C`) draws from the previous touch point to the current one
  while +0x5C is set and the pen is down, `SetBrushSize` (`0x02025D00`) clamps to 0..7 and fills the brush's
  rows from the sine table, `SetColor` (`0x02025DC0`) clamps to 0..15 and repeats the pixel through a word. Its
  part at 0 (`0x0202575C`) holds the background's screen and character buffers (tables `0x0210F5C8`,
  `0x0210F59C`), which flush to VRAM through the screen loader and `gVramTransferQueue`. Scenes 0x23
  (`VoteScene`), 0x3A and 0x8E hold one; 0x3A's is a class derived from it (built with C2).
- **The apartment scenes' members built with C2.** `RoomScene` and `TutorialRoomScene` build their particle
  system and head icons with `ParticleSystem`'s and `HeadIcon`'s C2 and their speech player with `TtsPlayer`'s
  C2 (`0x02083DC4`, its only caller): classes derived from those with nothing of their own
  (`RoomParticleSystem`, `RoomHeadIcon` in `include/HeadIcon.h`, overlay 74's `RoomTtsPlayer`).
- **The shops' counter.** Scenes 0x1E, 0x2F, 0x35 and 0x68 hold `ShopCounter` (`include/shop/Shop.h`) at the
  start of a part that the constructor builds from the counter's address together with what follows it
  (`Scene1EPart`, ...): the mannequin, models, particles, head icons.

- **Text canvases of the scene constructors.** Three classes derived from `ObjTextCanvas` have their tables in
  this file's data, told apart by where their `TextCanvas` virtual base is (measured, from their tables' first
  words and the constructors): 0x150 (nothing of their own), 0x174 (0x24 bytes of their own), 0x16C (0x1C bytes,
  the first cleared). Each table is a triple pointed at at its end (+0xC), each VTT passed from its second word
  (+4) to `ObjTextCanvas`'s C2. Scene 0x0F holds all three (`Scene0FTextCanvas`, `Scene0FTextCanvas2`,
  `Scene0FTextBox`).
- **The mannequin's body is a base.** The shops' constructors build `Mannequin`'s `CharacterModel` with C2
  (`0x0208EB18`), as `Character` builds its own; members are built with C1 (`0x0208EBA0`). Measured at
  `0x0203F9D0`, `0x0203FA50`, `0x020411B0`, `0x02041AD4`, `0x020457F0`.

## The scene constructors (0x02040578 to 0x0204AF20)

The constructors of scenes 0x27 to 0xC2, and the scene manager's `UnloadSceneOverlay` and `SetSoundGroup` after
them, are the second half of `src/scene/SceneManager.cpp` (the scene factory's file, which gap38-a cut; see its
header). The classes are declared in the overlays' directories (`src/overlays/ovNNN/<Class>.h`, with the layout the
constructor shows) or, for scenes of the main program, in `src/scene/`.

Measured:

- **One file, constructors out of line in id order.** `CreateScene` calls every constructor, even 0x28-byte ones,
  and they follow one another in the order of the scene ids, each followed by the implicit constructors and
  destructors of the member classes it gives `__cxa_vec_ctor`. Defined out of line in the file
  (`X::X(int sceneId) : Scene(sceneId) {...}`), they compile to the ROM's code; as inline constructors in the class
  headers `CreateScene` inlines them. The C2 copies the compiler also emits are stripped by the link.
- **What the code shows of a member.** An array of a class with a destructor is built by `__cxa_vec_ctor`; one
  without (message windows, buttons, texts, saved characters) by a loop, and an array of one still by a loop,
  so `MessageWindow mWindow[1]` and `MessageText mText[1]` where the ROM loops once. An inline constructor
  (MessageWindow, AnimModel, a Character's) is expanded in place. A run of members that the constructor addresses
  from the end pointer of the previous loop is a part of its own (`Scene74Part`, `RankingPart`): written as plain
  members, the compiler addresses them from `this`.
- **Model classes of the overlays.** Several scenes hold arrays of a class of their overlay that holds an
  AnimModel (`TestNg69Model`, `Scene3CModel`, `Scene3EModel`: constructor in the overlay, destructor the main
  program's copy) or derives from it (`Scene75Model`, `Scene91Model`: implicit constructor and D1 here, the table
  and D0 in the overlay, which refers to this file's D1; the class needs its overlay key function declared, or
  this file emits the table too). A model class with nothing virtual of its own (`TestNg134Model`) has its table
  and D0 in this file (`0x0211091C`, `0x0204AE10`).
- **Twins are C2 and C1.** `ParticleSystem`'s and `HeadIcon`'s constructors are each emitted twice, byte for
  byte (`0x02080E80`/`0x02080E8C`, `0x02081034`/`0x02081074`); the C2 comes first, and a head icon that is a member
  (scene 0xAF's character) is built with the second. Scene 0x76's icon calls both C2s: a class deriving from both.
  CharacterModel's (`0x0208EB18`/`0x0208EBA0`) are alike; a `Character` builds it as its second base with C2.
- **Sizes.** `CreateScene` allocates `sizeof` of each class, so a wrong size in a scene header shows in its
  literal pool; the headers' sizes all match it.
- `UnloadSceneOverlay` (0x02049F0C) unloads the overlay of each scene with an overlay, its 111 literal ids
  `overlay_id` relocations as `CreateScene`'s are; `SetSoundGroup` (0x0204AB04) keeps the sound group loaded in
  `SceneManager::mSoundGroup` (+0x360).
- Shared classes these constructors needed, now one definition each: `MessageWindow` (its inline constructor,
  two virtual functions: `SetVisible`, 0x02034A94, and one of more than four arguments), `G3Sprite`, `DayPhase`,
  `HeadIcon` with `ParticleSystem`, `ShopWindow` and `HalfPriceShopWindow` (the shops' price window, a message
  window with two buttons and three text canvases; tables `0x02115368`, `0x02115380`); and the shared
  character, model, item, room and list headers of the main program's owners.

Named classes of scenes whose overlay is not decompiled: after the scene's archive
`Scn/Scene/Scene<Name>_LZ.bin` when no other overlay mounts it (`RankingScene`, `CityHallScene`, `RomanceScene`,
`Marriage2Scene`, the `TestNg<n>Scene`s), otherwise after the lowest scene id (`Scene2B`, ...). Scene 0x27 of the
main program mounts the island map's sky, vote and apartment models (`MapModelsScene`), scene 0x2E shows a room
stepped through the 78 interiors (`InteriorViewScene`).

Done since (scene-ctors): every constructor of this range, the implicit members after them, the item classes'
inline getters and `Scene`'s D1 (`0x0204AD84`, the lone `bx lr` of `Scene`'s table), except `AlbumScene`'s (scene
0xB4, 97.9%: `ItemListView`'s three inline loops take r5/r6/r7 in rotation where the original alternates r5 and
r6; the same inline constructor matches in `ChangeRoomScene`'s, and no pragma around the constructor changes it).
Measured on the way:

- **Classes with two item bases.** Several scenes hold a class derived from `InventoryItem` and `ClothesItem`
  together (`AlbumItems`, `Scene40Items`, overlay 110's `ItemTradeSelection`): the constructor calls both bases'
  constructors and only then stores both of the class's tables (this file's data, one object, the second table
  at +0x24, `relocs.txt` `add:0x8` and `add:0x2c`). With RTTI off, MW writes 0 for a secondary table's offset to
  the top.
- **Item models of the scenes** (`Scene40ItemModel`, `WeddingGiftModel`, `AlbumItemModel`,
  `ItemTradeItemModel`) derive from `ItemModel` without a virtual function of their own: their tables
  (`ItemModel`'s with the `InventoryItem` part at +0x24) and D0s are this file's, their D1s follow the scene's
  constructor, and with them this file emits `InventoryItem`'s and `ClothesItem`'s inline getters
  (`0x0204ABAC` to `0x0204AD7C`), which the ROM keeps from here.
- **Lists derived from `ItemListView`.** `ChangeRoomScene`'s and `Scene40`'s lists override all five of its
  virtual functions in their overlays (tables `ov086 0x0214BA84`, `ov075 0x02182CD0`), scene 0x35's derives from
  `ShopItemListView` and overrides `DrawRow` alone (`ov084 0x0214C2C8`; the constructor stores `ItemListView`'s
  table and then the overlay's), and `AlbumScene`'s overrides none and has its table here (`0x02110938`).
- **Main-program copies the overlays refer to.** Overlays 73, 74, 96, 109 and 110, now on the shared headers,
  emit copies of `SceneCharacter`'s, `WeddingCharacter`'s, `RoomCharacter`'s, `TutorialRoomCharacter`'s and
  `RoomPart`'s D1 and of `InteriorItem`'s getters; the originals refer to the main program's (listed in
  `config/jpn_rev1/arm9/inline_symbols.txt`).

## The main program's test scenes and the scenes' helpers

From `0x0204AF20` (where the scene factory's file, `SceneManager.cpp`, ends) to the list scenes at `0x0204FC74`: the scene
base class's runner, ten test scenes of the main program and the helpers that the scene overlays share. Each test
scene is one file under `src/scene/`, its class in a header of its own; their constructors are in the scene
factory's file, the destructors (implicit) in theirs.

| Scene | Class | Size | File's `.text` | Functions | What it does |
| --- | --- | --- | --- | --- | --- |
| | `Scene::Run` | | `Scene.cpp`, `0x0204AF20` | 1 of 1 | The phase runner (see `include/Scene.h`): the scene manager's update calls it |
| 6 | `DrawManTestScene` | 0x272C | `0x0204B090` | 7 of 8 | "SceneTestDrawMan": a head model whose render callback keeps the matrix of the joint `head_h2`, and a character's face drawn with it |
| 16 | `G3dTestScene` | 0x5C2C | `0x0204B504` | 6 of 7 | "SceneTestG3d": two characters, a body model and glasses (`Glass6.imd`) at the first one's hand, edge marking by polygon id |
| 7 | `HumanTestScene` | 0x1D8 | `0x0204BC78` | 6 of 7 | "TestHuman00": a head and a body model, their textures loaded later, a palette swapped in |
| 8 | `ParticleTestScene` | 0x53C | `0x0204C174` | 8 of 8 | The particle library with `SplRoomEffect.spa`, the particles kept at a body's neck |
| 9 | `BgTextTestScene` | 0x1D4 | `0x0204C6F4` | 7 of 7 | Sets a text canvas up on background 2 and nothing else |
| 10 | `MessageTestScene` | 0x2D84 | `0x0204C860` | 6 of 7 | Etc messages on sprites and on background 2, with a string and item messages in their tags |
| 11 | `KeyboardTestScene` | 0x6AF8 | `0x0204CE28` | 6 of 7 | The on-screen keyboard (`SoftKeyboard`), typing ten characters into a text box |
| 14 | `TtsTestScene` | 0x6A858 | `0x0204D678` | 7 of 7 | The speech synthesizer: a weather report straight to the stream, a test sentence into the capture buffer |
| 22 | `EmptyScene` | 0x10 | `0x0204D7F8` | 9 of 9 | Does nothing; the file also defines `gSceneMemory` and `data_0213bab0` |
| 27 | `WirelessTestScene` | 0xCC | `0x0204D880` | 13 of 14 | WH: a parent sends a saved character in four packets, a child lists parents and stores what it receives |
| | (helpers) | | `SceneHelpers.cpp`, `0x0204E4B4` | 39 of 42 | See below |

Measured:

- **The files' bounds.** Each test scene has its own virtual table in `.data` (`0x02110B94` to `0x02111424`),
  followed by the strings its code uses in the order of its code, and the static initialisers in `.init` follow
  the files' order (`0x020B06CC` constructs `gSceneMemory`, `0x020B06E0` the wireless test's `SavedCharacter`).
  The tables are named `_ZTV<Class>` at their start, and the constructors refer to them with `add:0x8`.
- **`gSceneMemory`** (`0x0213BA74`, `include/SceneMemory.h`): what the scenes leave for each other. Its constructor
  (`0x0204D824`, not inlined into the initialiser: `__attribute__((never_inline))` gives that) zeroes the camera
  position and target and the orbit, and fills the twelve character indices and 16 more bytes with 0xFF.
- **Scene 9 is not overlay 0's `TextTestScene`**, whose constructor is the scene factory's first; scene 9 only
  sets a canvas up.
- **The wireless test** (scene 27) shows WH's states with its own texts ("通信モード－親（送信）", "接続中  親を選択して
  スタート", "送信成功(タッチで終了)"...), scans with a broadcast MAC address (`sAnyParent`, `.rodata`
  `0x020B5D3C`), keeps up to three parents (`sParents`, `WMBssDesc`), and puts the owner's nickname in the
  parent's user game info. It sets the game clock to 2011-02-03 12:13:14 at its start. It is not a copy of WH;
  the child-only copy is elsewhere (see "WH, the wireless wrapper").
- **`SceneHelpers.cpp`** is one file: its tables (`0x02111648` on) are read by the number reading and by
  `SceneCast` alike, its strings follow the code's order, and its `.rodata` pool (`0x020B5D44` to `0x020B5DAC`)
  is sorted by size across the classes (six 8-byte pairs that the code folds: the buttons' positions and touch
  sizes; the two 12-byte light directions; the two 16-byte `ButtonSounds`). In it:
  - `MakeCharacterSpeechTexts` fills `CharacterSpeechTexts` (`include/character/CharacterSpeechTexts.h`, 0x294
    bytes) from a saved character: the Mii's name, the family and given names, and the character's phrase
    (`mSpeechDefault`, "ミニュウリョク" when empty), each turned into the synthesizer's notation by `func_020372d0`,
    the readings also in Shift JIS. Overlay 76's voice page and ten other overlays call it.
  - `MakeNumberReading` reads a number from 0 to 9999 out in the synthesizer's notation ("サ’ンゼン/ヨ’ンヒャク/...",
    "レイ" for 0); `MakeNumberText` writes it as digits.
  - `SceneCast` (called by overlays 7 to 63): up to ten characters with bodies, chosen among the saved ones,
    `gSceneMemory.mCharacters` first and the rest at random, each given an archive "B0I" to "B9I" and the room
    animations "BIC" (resource 31).
  - `FaceCast` (overlays 14, 15, 23, 27, 71, 80, 87, 95, 97): up to eleven characters shown as their Mii faces
    only (`CharacterBase`), each placed by a `FaceTransform` (x, y, a scale, three turns), the first turned by
    dragging the stylus.
  - `ReturnButton` ("Scn/Scene/ReturnBtn_LZ.bin", asks for a scene when pressed), `SceneRoom` (a `Room` with its
    archives "CO0" and "RM", which hides its parts by the room style's table) and `NewsButton` (the Mii news
    screen's lower background, "SceneMiiNews", and its button, which asks for scene 0x49).
- **NitroSystem's `NNS_G3dMdlSetMdl*All`** (`0x0206597C` to `0x02065B3C`, the seven "for every material" setters of
  `model.c`) were not matched by the library matcher; they are now in `model.c`'s unit, linked from source.

Not done (each a near-miss of instruction order or registers, not of what the code does):

- The `Init`s of scenes 6, 7 and 16 set light 0 to point along x with an x stored, reloaded and stored again, and
  the colour loaded and stored back; no source form tried gives the original's registers (local copies, struct
  copies, volatile reads, accesses through the array).
- `MessageTestScene::Init` keeps 0 and 1 in a callee-saved register as `#pragma opt_propagation off` does; with the
  pragma the sprite positions get one dead copy too many. `KeyboardTestScene::Init`: two dead position copies
  stored in another order (none of the 25 combinations of the five position setters for the two boxes gives it).
  `WirelessTestScene::Update` (78%): its loops are not rotated as at optimisation level 2, but level 2, with
  `opt_common_subs` off or the eight IR passes off, still differs in 130 lines (shared epilogue, registers).
  `WirelessTestScene::Init` matched once the clock setter `func_02035460` got its fourth argument (it reads r3; overlay
  98 passes it): the 0 in r3 is also the `date.week` stored. `Redraw` matched at level 2 with `opt_propagation` off.
- `SceneHelpers.cpp`: `ApplyTransform` (two registers), `SetTransform(int,
  FaceTransform)` (a copy of the position the original does not make) and `SceneRoom::ShowParts` (registers).

## The face test scene (overlay 2)

Overlay 2 is scene 2, `FaceTestScene` (0x4DDC bytes; its constructor is the main program's `_ZN13FaceTestSceneC1Ei`,
0x0203df24): one source file, `src/overlays/ov002/FaceTestScene.cpp`, all seven functions match and the unit is
complete.

Measured:

- It mounts "Scn/Scene/SceneTestG3d_LZ.bin" as TG3, as the main program's `G3dTestScene` (scene 16) does, and
  "Mdl/Ica/head_ICA_LZ.bin" as TGH. Resource 0 is TG3's glasses (`Glass6.imd`), drawn by a plain `Model` at +0x10
  (rotated 0x6000 about y, at y -0x1199); resource 1 is TGH's `head.imd` and `wait00.ica`, the head model of a
  `CharacterHead` at +0x26AC (at x 1.0), whose animation 0 starts at a random frame.
- Beside it a bare `CharacterBase` at +0xC8: a face drawn with no head model, at its own head matrices.
  `Init` sets its `mHeadRoll` to 0x2000. `Update` changes it alone: with A held the pad moves its `mFaceOffset` by
  0x40, with B it changes `mHeadRollNow` and `mHeadPitchNow` by 0xB0, with Y `mHeadYawNow` by 0xA0. X with up and
  down counts +0x4DD4 through 0 to 26, and left or right gives it to `SetFace`; L does the same with +0x4DD8 (0 to
  7) for `mHeadTurn`. With R held the pad moves the camera (`Camera_Translate`, 0x0202a6fc: adds to its position
  and target alike). Every frame it runs the face's `UpdateFace` and `UpdateHeadTurn`.
- Touching the screen sets the touch line (`ModelResources_SetTouchLine`, 0x0202af00: `NNS_G3dScrPosToWorldLine` of
  the point into `data_021223e4 + 0x428`, then the normalised direction from the camera's eye, `data_02122764 + 0x20`,
  into `data_021223e4 + 0x434` = `data_02122818`, which `CharacterBase::HitTestFace()` reads); when the line hits
  the face it gets a random default Mii of 6 (`SetDefaultMii`).
- `Init` resets lights 0 and 1 with `func_0202a96c` (direction (0, 0, -0xFFF), colour 0x7FFF, on) and has none of
  `G3dTestScene`'s light-0 code.
- `AnimModel_SetAnimationFrame` (0x0202a5d8) finds a model's animation by type and id and sets its `NNSG3dAnmObj`'s
  frame.

Inferred: a test of the Mii face's placement on a head, from the same test archive as `G3dTestScene`.

## The Mii drawing test scene (overlay 5)

Overlay 5 is scene 5, `TestDrawMiiScene` (0x28F8 bytes; its constructor is the main program's
`_ZN16TestDrawMiiSceneC1Ei`, 0x0203e1e8): one source file, `src/overlays/ov005/TestDrawMiiScene.cpp`.

| | Where | |
| --- | --- | --- |
| `.text` | `0x02149A40` to `0x0214A4A0` | The scene's five virtual functions and `UpdateModelFades`, `TestDrawMiiModel`'s constructor, `Init` and `Update`, the camera's five functions, then the implicit destructors (the model's D0, the scene's D1 and D0) |
| `.rodata` | `0x0214A4A0` to `0x0214A4D0` | Four constants of `Init` and the camera's `Init`, sorted by size: the models' rotations (`u16[2]`), their model indices (`int[2]`), the camera's first angles, the models' places (`VecFx32[2]`) |
| `.data` | `0x0214A4E0` to `0x0214A560` | `TestDrawMiiModel`'s table, the scene's table, the strings |

Measured:

- It mounts "Scn/Scene/SceneTestDrawMii_LZ.bin" as SME; resource 0 is `SME:/room.imd`. The room is model 0 of it,
  drawn by a `TestDrawMiiAnimModel` at +0x27E4; two `TestDrawMiiModel`s at +0x25F4 (0xF8 bytes each, derived from
  `AnimModel`, with their own table in the overlay) are models 1 and 2 of the same file, at (0x5000, 0, 0) and
  (0, 0, -0x5000), turned 0 and 0x4000 about y. A `CharacterBase` at +0x10 is the Mii: `InitFace(1)` and then a
  random Mii (`SetRandomMii(0, 0, 2, TRUE)`), drawn with `CharacterBase::Draw`.
- `Init` also turns on antialiasing, toon shading and W buffering, clears to `GX_RGB(21, 21, 21)` and sets light 0
  to (0xFFF, 0, 0) and white.
- The camera at +0x28C4 (`TestDrawMiiCamera`, 0x34 bytes, no constructor) circles a point: its eye is the target
  shifted right by 3, plus `radius.z * sin(yaw)` on x and `radius.x * cos(yaw)` on z, and `radius.y * sin(pitch)`
  on y, the angles being sine table indices in fx32 (yaw 0x1600, pitch 0x1200 at first; radius (0x12800, 0x9000,
  0x12800), target (0, 0x1CCC, 0)). The code is overlay 74's `RoomCamera::GetOrbitPosition`, instruction for instruction.
  Left and right turn it by 0x100 a frame, up and down raise it by 0x30; with X held, right moves the target to a
  random place (each of x and z from 0 to 3.9, either sign), left gives x and z of the radius random values from
  9.0 to 17.9 and up a random yaw. The steps are `FX32_ONE / 10` (0x199), not `FX32_CONST(0.1)` (0x19A).
- Each frame a model whose distance from the eye on the floor (x and z) is less than the eye's from the origin
  fades out (its alpha down by 5 a frame to 0, `func_02029f50`), and the others fade back in to 31: the models
  between the camera and the room's middle get out of the way.
- `TestDrawMiiModel::Init` is empty; the scene calls it after `Load`. Its complete-object destructor is the main
  program's copy (`0x0203E2A8`), which the scene's destructor and the model's table refer to; the overlay has only
  its deleting destructor.

Inferred: a test of how a Mii looks in a room under a moving camera.
## The character body test scene (overlay 4)

Overlay 4 is scene 4, `CharacterBodyTestScene` (0x2B38 bytes; its constructor is the main program's
`_ZN22CharacterBodyTestSceneC1Ei`, 0x0203e11c): one source file, `src/overlays/ov004/CharacterBodyTestScene.cpp`.

| | Where | |
| --- | --- | --- |
| `.text` | `0x02149A40` to `0x0214A2B4` | The five scene functions, `UpdateTurnTest`, and the implicit destructor (D1, D0) |
| `.rodata` | `0x0214A2B4` to `0x0214A2BC` | `sTurnTarget`, the point (1.0, 0) on the floor |
| `.data` | `0x0214A2C0` to `0x0214A3E0` | `sLightColor` (a `GXRgb`, red 30), the virtual table, the strings in the order of use |

Measured:

- It mounts "Scn/Scene/SceneTestG3d_3_LZ.bin" as CA3, as the main program's `DrawManTestScene` (scene 6) does, and
  "Mdl/Body/body000_3D_LZ.bin" as TG3. Resource 0 is TG3's `body.imd`, the body of one `SceneCharacter` at +0x10
  (`Character::Init(0, 0, 0)`, its joints tracked and turnable: `mTrackLimbs`, `mTurnJoints`).
- `Init` sets the clear colour to grey (15, 15, 15) with alpha 15, light 0 to point along x in `sLightColor`, and the
  camera's y angle to 0x4000.
- B steps +0x2B28 (`mBodyStep`) through four changes: TG3 mounted again with body000 and the model made again
  (`CharacterModel::MakeModel`), the palette file `TG3:/body_c1.ntfp` sent to the body's palette in VRAM
  (`func_02029d40`), TG3 mounted with body001 (`Mdl/Body/body001_3D_LZ.bin`) and the model made again, the palette
  again.
- With A held the pad rotates the body model about z and x (by 0x80) and turns the character (`Character::Turn`,
  ±0x100); with X it scales the body in y and z (by 0x40, `mInvScale` kept as `FX_Inv` of `mScale`); with Y right
  moves it along x, left along y, up and down scale it in x; with R held the pad moves the camera (`Camera_Translate`).
  L starts the turn test (+0x2B34, `mTurnTest`): a frame later the character's `mTargetAngle` is set to the direction
  of `sTurnTarget` from where it stands, and it turns towards it by `CharacterBase::StepAngle(mAngle, mTargetAngle,
  3)` and `SetAngle` a step a frame until it faces it. Otherwise the pad turns the camera.
- Touching the screen sets the touch line (`ModelResources_SetTouchLine`) and hit-tests two of the character's
  joints, the body (`CharacterModel::HitTestJoint(int)`) and the left knee (`HitTestJoint` with the
  camera's eye and the line's direction, `data_02122818`); both results are thrown away.
- +0x2B2C is only cleared, in `Init`.

Not done: `Init` (97%), the light-0 block of scenes 6, 7 and 16: with `#pragma opt_propagation off` every register
matches and only the stores of x and `mEnabled` are in the other order (see decompiling.md, "Independent statements
scheduled in another order"). Seven of the eight functions match; the unit is not complete.

Inferred: a test of the character's body (its two models, its palette, its scale, its turning and its joints), from
the same test archive as scene 6.

## WH, the wireless wrapper

WH is the wireless code of the SDK's sample programs (`build/demos/wireless_shared/wh`), a state machine over the
WM driver's asynchronous calls, which games copied into their own source. The game has it in the main program, in
`lib/WH/wh.cpp` here: all 66 functions and all its data match, and the unit is complete and linked from source.

| | Where | |
| --- | --- | --- |
| `.text` | `0x0202F42C` to `0x0203115C` | `WH_GetWMErrCodeName` to `WH_End`, after `MessageText.cpp`'s last function; `0x0203115C` on is the game's sprite code (`0x0203119C` is the one that some 40 overlays call) |
| `.data` | `0x0210F79C` to `0x021102D8` | The three name tables' strings and the tables, sorted by size, then the strings of the code in the order it uses them |
| `.bss` | `0x02138F34` to `0x0213B5C0` | A pool of the statics under 64 bytes (`0x02138F40` to `0x02138FE0`), then each larger one on its own, the WM buffer last |

Measured:

- The names are WH's own: its debug strings name its states (`WH_StateInStartParentKeyShare failed`,
  `WH_StateOutStartScan : MAC=...`, `WH_Finalize, state = %d`), and each state is the WM call it makes and the
  callback it passes. Its tables name the WH states (`WH_SYSSTATE_STOP` to `WH_SYSSTATE_ERROR`, 10), WM's error
  codes with WH's three after them (23) and WM's state codes (27).
- This revision has key-sharing and data-sharing (it calls `WM_StartKeySharing`, `WM_StartDataSharing`), scans with
  `WM_StartScan` and connects with `WM_StartConnectEx`; a parent takes one child (`maxEntry` 1, the data-sharing
  AID bitmap 3), data-sharing is on port 13 with 200 bytes a device, and `WH_SendData` sends on port 14.
- The game's build is a final one: the trace hook `wh_trace` is a null pointer in `.bss` that nothing sets, so
  none of the strings is ever printed, and a failed check calls `OS_Terminate`.
- 66 functions are in the ROM; the sample's others (`WH_GetConnectMode`, `WH_GetLastError`, the WEP key
  generator setters, ...) were stripped by the link, as nothing called them. Their statics are still in the pool:
  `sParentWEPKeyGenerator`, `sJudgeAcceptFunc` and the others are read and never written.
- One variable is the game's, not the sample's: when a child connects, the parent's callback copies the child's
  24-byte SSID (the user data of its connection) into `gWHChildSsid` (`0x02138FB4`), which overlays 109 and 110 read.
- Callers: overlays 109 and 110 (wireless play) call 11 of the functions, and the main program at `0x0203B7EC` to
  `0x0203BB8C` and the wireless test scene (`src/scene/WirelessTestScene.cpp`, `0x0204D880` to `0x0204E4B4`). A second, child-only copy of WH with no debug strings is at
  `0x02078F58` to `0x02079DFC` (another unit).
- Named from WH's calls, by comparison with the SDK's sources: `WM_Initialize` (`0x02012604`),
  `WM_StartKeySharing` (`0x02013850`), `WM_EndKeySharing` (`0x0201386C`), `CHT_IsPictochatParent` (`0x020144E4`).

What it showed about the compiler, measured (see "Compiler"):

- **`-ipa file`.** The small statics are one pool, addressed from its start: `sSysState` is the pool's address
  plus 0x2C everywhere. The compiler pools a file's statics only with `-ipa file`, so WH was compiled with it.
- **Objects of 64 bytes and more are not pooled.** The 32-byte `WMScanParam` is in the pool and reached from its
  start; the 64-byte `WMParentParam`, the scan's `WMBssDesc` and the buffers are addressed each on its own (by test
  files: 60 bytes is still pooled, 64 is not). This is what told the compiler's version; with build 138 and
  `-ipa file` all 66 functions match, with build 137 58 (the others reach the parent parameters).
- **Arrays of `char` are not pooled either**, whatever their size: `gWHChildSsid`, in the pool, is an array of
  `u16`, and `sConnectionSsid`, apart, an array of `u8`.
- **A file's `.bss` objects are sorted with its `.data`**, in the compiler's size sort (`docs/notes/ov073.md`), so
  the statics' declarations decide the order of the tables' strings of equal length in `.data`. In C a static
  without an initialiser is a tentative definition, created at the end of the file; in C++ every static is created
  where it is declared. The ROM's order of the 60 strings comes out of C++ (every static created in turn, then the
  error names, the state code names and the WH state names, each a static of the function that uses it), and of no
  arrangement of WH's statics in C that a search found (56 at best). So WH was compiled as C++, like the game's
  own code (inferred: the order needs every static to be created where it is declared, which C gives only to the
  ones with an initialiser, and the sample declares most without). `-lang=c++` and `-lang=c99` give the same code
  for it; its functions are kept under their C names (`extern "C"`), since nothing outside refers to them by name
  but the game's code, by address. `docs/notes/wh.md` has the details.
- **The pool's order** follows from that sort: the statics are declared in an order that gives the ROM's pool and
  strings, which says nothing of the original's order but sizes.

## A Mii's trip, and the "Ng" scenes

Overlay 39, `src/overlays/ov039/TravelScene.cpp`: all 11 functions and all of its data match (the data compared byte
for byte, and the relocations of `.data`, with the original). The unit is not marked complete only because the
main program's character functions it calls are still under their old names on main (see the end of this section).

Measured:

- **What it is.** Scenes 0x77, 0x78 and 0x79 construct `TravelScene` (0x1EE84 bytes; the main program's
  constructor `0x020460B0`, virtual table `0x0214BA70`). Its message 0x4B5 of `Msg/TmdMiniContentsMsgJP_LZ.bin`
  (archive "TBM", file `TBM:/tmd_minicontents_msg_JP.bmg`) introduces a Mii's trip on which the player photographs
  the travellers at the sights. `StartLoading` draws one of nine destinations, whose archive
  `Scn/Travel/<name>_LZ.bin` is mounted as "N01" (the table is in the order of the names: France, Hawaii, Hongkong,
  Italia, Kyoto, Okinawa, Shanghai, Spain, Thai), and shuffles the destination's 4 to 8 sights to take three.
  Sight n of a destination has its name at message `base + 2n` and its reading at `base + 2n + 1`, the bases from
  0x4B6 (France) to 0x51A (Thai); 0x522 to 0x531 are the destinations' names and readings.
- **The three scene ids** give two, one and two travellers (`SceneCast::Choose`). Scene 0x77 also loads the
  `N00:/*_hm.*` graphics (the scene archive is `Scn/Scene/SceneTestNg116_LZ.bin`, "N00") and dresses the pair for
  the destination: a table of nine pairs of clothes ids, the first for a man and the second for a woman, by the
  first traveller's sex (`MiiData_GetSex`), stored in `CharacterRecord::mClothesId`. Its departure plays sequence
  archive entry 14 where the others play 13.
- **The flow** (`SetState`, `Update`, 21 states): a start button; the upper screen (`BG_airplane`) fades out; then for
  each photograph the sight's picture (`N01:/BG_0<n>.ncg/.ncl/.nsc`, the digit put into a copy of the name) fades
  in, the first traveller says "<sight> desu" (message 0x4AE, the sight's name in the speech window and its reading
  in the spoken text), the shutter button appears and the player's "say cheese" (0x4B0) is spoken with the voice
  of a resident drawn at random; when its frame comes (from that resident's `mVoice[4]`: `(v - 50) * 220 / 250 +
  40`) the travellers blend into random poses ('BCA0' animations 1-4 and 10-13 together, or 5-9 each), and the
  shutter plays sequence archive entry 15 and a white flash (`func_02023f4c` from 16 down), with faces 6 or 1. A
  "next" button follows the first two photographs, a wait the third; then a return button, and the scene fades and
  asks for scene 0x40. It also stores a per-destination halfword (0xB6, 0xC3, 0xD9, 0xC2, 0xC4 to 0xC8) at
  `gSceneMemory` +0x2C (not identified): overlays 68, 75, 84 and 85 read that halfword (84 when the previous scene
  was 0x40), and 28, 52, 53 and 75 write it too.
- **The caption** (`Scn/Scene/NgCaption_LZ.bin`, archive "SFL", `SFL:/caption.ncg/.nsc`, loaded for plane mask 2) is a
  MessageText centred at (128, 14) with message 0x4B2 ("<name>'s") or 0x4AD ("<name> and <name>'s"), the names from
  `SceneCast`'s speech texts, with a shadow on four sides (0xAA).

Inferred, for the other scenes that mount `NgCaption` and the mini-contents messages (about sixty overlays, 7 to
63 among them): they share this shape, which the constructors in the main program show (overlay 40's has the
same members in the same order): the characters (`SceneCharacter[n]`), a `SceneCast` right after them, a few ints
and a `char[0x82]` Shift JIS buffer, the `TtsPlayer`, buttons, one `MessageFile`, a `SpeechWindow` with 0x84 bytes
after it, a `MessageText` that only expands the spoken message, and the caption. Overlay 39's `Speak(speaker,
message, mode)` (show the message in the speech window for 120 frames, set `SceneCast`'s speaker and face 1, give
the speech engine the speaker's voice unless it is -1, expand the message, convert it to Shift JIS and speak it)
and the `SetState`/`mState`/`mFrame` pair are likely to recur. Every caller of `SceneCast::UpdateCharacters`
passes 1 (overlays 7, 20, 39, 40, 50 and 63 checked), so it takes an int.

Not done: marking the unit complete. It calls `CharacterBase::SetFace`, `Character::SetAngle`, `SetAnimation`,
`BlendToAnimation`, `Draw`, `Update`, `SetVisible`, `CharacterModel::GetJointMatrices`,
`SpeechWindow::SetMessageArg` and `SceneManager::RequestScene` by the names gap66-a's and gap38-a's branches give
them; main's `symbols.txt` still has `Character_SetFace` and the like, which complete overlays call. Once those
renames are on main the unit can be marked `complete`; nothing else is missing.
## The body palette test scene (overlay 3)

Overlay 3 is scene 3, `BodyPaletteTestScene` (0x1BC bytes; its constructor is the main program's
`_ZN20BodyPaletteTestSceneC1Ei`, 0x0203e0c4): one source file, `src/overlays/ov003/BodyPaletteTestScene.cpp`, with
the whole `.text` (seven functions, the two destructors implicit) and `.data` (the virtual table at `0x02149EC0`,
then the strings in the order of the code). Six of the seven functions match; `Init` is at 79% (scheduling only, see
decompiling.md on the light-0 block), so the unit is not complete and its data not yet placed.

Measured:

- `StartLoading` mounts "Mdl/Body/body000_3D_LZ.bin" twice, as TG3 and as TG4 (the path is two strings in the
  ROM: `-str noreuse`). `Init` binds TG3's `body.imd` to resource 0 and TG4's to resource 1 and loads them into two
  plain `Model`s, `mBody` (+0x10) and `mSecondBody` (+0xC4, at x -2.0), clears to grey (`GX_RGB(15, 15, 15)`), and
  points light 0 along x with the code of the main program's `G3dTestScene` and `HumanTestScene` (x stored,
  reloaded and stored again, the colour loaded and stored back).
- `Update`, by the button pressed this frame: A gets the file "TG3:/body_c1.ntfp" (a palette), makes its colour 1
  red and gives it to `func_02029d40` with offset and size -1 (the whole of `mBody`'s palette, from its VRAM key);
  X makes colour 1 of `mBody`'s own palette data (`mResTex->plttInfo.ofsPlttData`) cyan and transfers 4 bytes;
  B releases the model resource (`func_02029758`, 'BMD0'), remounts TG3 on "Mdl/Body/body001_3D_LZ.bin", binds its
  `body.imd` to resource 0 again and rebinds `mBody`'s model (`func_02029c20`); Y walks the palette-to-material
  dictionary of `mBody`'s materials and looks each name up in its textures' palette dictionary
  (`NNS_G3dGetPlttDataByName`), with nothing done with the result. Otherwise the pad turns the camera, as in
  `HumanTestScene`.
- `Model` +0x10 is the model's `NNSG3dResTex` (`mResTex`): `func_02029d40` (0x02029d40, `Model*`, palette, offset,
  size) takes the palette's VRAM address and size from its `plttInfo.vramKey` when offset or size is negative, and
  queues the transfer with `func_0202b108(data_02123024, 1, ...)`, which passes it on to
  `NNS_GfdRegisterNewVramTransferTask` (type 1, `NNS_GFD_DST_3D_TEX_PLTT`). `func_02060c1c` passes the linked VRAM managers'
  state (`data_021423d8`) to `NNSi_GfdDumpLnkVramManFreeListInfo`.

Inferred: a test of colouring the body's clothes by palette (the clothes colours of the game), with an untouched
copy beside it to compare.

## The settings and the mansion information

Overlay 99, `src/overlays/ov099/InformationScene.cpp`: all 10 functions and its data match, and the unit is
complete. Overlay 98, `src/overlays/ov098/OptionScene.cpp`: all 15 of its functions and its data match, and the
unit is complete.

Measured:

- `InformationScene` (0x51BC bytes, scene 0xAE; constructed in the main program at `0x02048918` after
  `FS_LoadOverlay(0x63)`, virtual table at `0x0214AA20`) shows statistics over the 100 saved characters with the
  etc messages 0x185 to 0x1A1: the number of residents, their average age to a tenth (the birth year is 12 bits at
  0x50 of a saved character, 0xFFF when not given; month and day are func_02098dac's properties 0x26 and 0x27; the
  age is taken against `Clock_GetDate`), the ratio of men to women (func_0202c604 is set for women, by the
  numbers' order in "男女比"), reduced by their greatest common divisor, zero to five stars of comfort from the byte
  at 0xFB of each character, the troubles solved (`gGameData` 0xC2, "悩み事解決数"), and one of 17 types of
  mansion (0x18F + type): the summed personalities (0 to 15) give a point on two axes, its direction one of eight
  types, its length against the number of residents one of two strengths, and the origin "完全平均". Its one button
  goes back to scene 0x11.
- `OptionScene` (0x26394 bytes, scene ids 0xAC and 0xAD; constructed in the main program at `0x020487E4` after
  `FS_LoadOverlay(0x62)`, virtual table at `0x0214C700`) is the settings, with the etc messages 0xD7 to 0xF4 and
  0x150/0x151: the sound settings (stereo or mono, and the volumes of music, effects and voice, 0 to 3, in
  `gGameData` 0xC8 to 0xCB; the voice's volume is spoken, "ボリュームゼロ" to "ボリュームサン"), the clock (date
  and time on sprite digits, written to the game's clock by func_02035460 and saved), and erasing all the data after
  three questions. Scene 0xAD is the first start ("時刻を設定してください"), which goes on to scene 0x69.
- Both scenes are built from two main-program classes, now stand-ins in `include/SpriteButton.h`: a sprite button
  of 0xF4 bytes (constructor `0x0203117C`, update `0x0203133C`, draw `0x02031D7C`) and a button with a message
  text of 0x6F0 bytes (constructor `0x02031F1C`), the same that the message window holds 12 and 16 of.

Inferred, and measured with the compiler: overlay 98's `.rodata` is one file's constants, heap-sorted by size,
with the four-byte-aligned ones in one pool read by offset from its start. Up to `0x0214C384` our compiler gives
the ROM's layout from the declaration order in the source. After it the ROM has the string
"N83:/Window_down.nsc" and then the six tables of 64 bytes and more, which our compiler (2.0 SP2p2) puts in the
pool, before the string; mwccarm 3.0 build 138 (2.0 SP2p3) keeps objects of 64 bytes and more out of the pool and
gives the ROM's layout from the same file, and compiles all of both overlays' functions as ours does. Overlay 99's
data come out the same with either.

Not done:

- The pool's first 36 bytes of overlay 98 (0, {2,0}, {0,4}, {2,2}, {2,2} as words), which nothing refers to, are
  not identified: the source keeps them as `sUnknown0` to `sUnknown4` with the ROM's values, and the unit is
  complete with them. The placement model (`docs/notes/ov072.md`) admits the five Show functions' digit counts
  (4, 2, 2, 2, 2) as local constants for the last five words, but nothing in the code shows them, and the other
  four words (0, 0, 0, 2) have no candidate. Function-local `const`
  scalars give such entries, and nine 4-byte objects would fit (three zeros, one 4, five 2s), which is not
  established. `docs/notes/ov098.md` has the details.

## The island map

Overlay 72, `src/overlays/ov072/MapScene.cpp`, one unit with `.text`, `.rodata` and `.data`: 56 of its 59 functions
match (61 in the compiled object, with the base-object constructor and destructor that the link strips); not
complete.

Measured:

- `MapScene` (0x1D4FC bytes; `CreateScene` makes it for scene ids 0x5F to 0x66, 0x6A, 0x6D and 0x6E after
  `FS_LoadOverlay(0x48)`; constructor `0x0204534C`, virtual table at `0x02150AD4`) is the island: 13 models (the
  ground, by season, and twelve buildings, each a "board" model until it is built), the apartment building's lit
  windows (up to 150 `G3Sprite`s, one per room, textured by what the resident is doing: "w_live.tga" to
  "w_game.tga"), "new" marks over buildings, and two message windows. Touching a building flies the camera to it
  (`MapCamera`: 40 frames, eased by t^4) and opens its window ("Scn/Map/SceneMap2Wnd<Name>_LZ.bin") on the lower
  screen; touching it again, or the window's button, enters it (`EnterBuilding`: the scene of each building, the
  fountain's by the time of day from a table of 14 times). The back button goes back to the apartment (scene 0x11)
  from the island's view, and to that view from a building.
- Scene 0x6A starts in a title mode (`TitleCamera`: ten moving views of the island, chosen at random, until a
  touch); scenes 0x6D and 0x6E play the tutorial's visits (steps 2, 4, 7, 15, 18, 20 and 23 of `data_0213ba74`
  +0x1E, the messages 0x152 to 0x17A); scenes 0x62 to 0x66 show the island at sizes 0 to 4. When a building or a
  feature has just been built (func_020af4d8), the scene shows it appearing with effects and goes to the news
  (scene 0x46, overlay 87), after adding a news item of it (`AddNews`, `NewsItem_Make` of the main program).
- `MapSound` plays the music of the day or the night (sequences 0x1A and 0x1B, switching at the month's times
  from func_020a6390/func_020a6480) and three sounds of the surroundings by season and time of day; each building
  has its own volume, pitch, tempo and pan for the music, ramped to when it is selected.
- The news stand's texture animation shows whether a new issue is due (`IsNewsDue`: the newest item is from before
  the last 7:00 or 19:00 edition by half a day); entering it then makes the day's news (`MakeDailyNews`: a
  resident's birthday, else a random kind of news of the month, `data_020fa1d0`).
- `MapClock` and `MapClockDisplay` are overlay 74's `Clock` and `ClockDisplay` but for the sprite bank;
  `MoneyDisplay` counts the shown money up to `gGameData`'s.

The data, measured with the compiler: the unit's `.rodata` (0x89C bytes) comes out byte for byte as the ROM's with
SP2p3 (see "Compiler"). How the compiler orders a file's data, which reproduces every object's
place in our object: it keeps a list, newest first, of the file's statics in declaration order, each preceded by the
strings of its initializer; then the class's virtual table; then the functions' local `const` objects. It heap-sorts
the list by size; the four-byte-aligned objects of `.rodata` and of `.data` are each pooled, at the place of the
first. Unreferenced statics are not on it; local `const` scalars are, even when their uses are folded, which is
what the first 0x14 bytes of the pool are (four constants of `Init` and two one-element arrays). The declaration
order of the source was solved with this model (`docs/notes/ov072.md`).

Not done:

- Two functions differ, each with a comment saying how: `Update` (the frame is the original's since the touch loop
  reaches the camera through a pointer, the apartment mark is set with `SetPositionFromVec`, which orders its dead
  copies as the ROM does, and `ResetCamera` passes `mCamera.mCamera` to `func_0202a640`; the registers and order of
  the touch loop's rectangle arithmetic differ) and `MakeDailyNews` (the registers of the loop that picks the
  month's kind of news). `Init` matches with the second light's colour through a local. `Exit` matches since
  `data_0213bab0` is a byte buffer (0x70 bytes to the next symbol; overlays 68 and 75 clear 100 of them with
  `MI_CpuFill8`, overlays 31 and 90 read bytes of it) and the map writes its three colours through a `GXRgb *` cast
  from it: the cast pointer is kept in a register, the array's own address is loaded again before each store.
- `.data`: its pool is the ROM's, but of the 112 sorted strings and the virtual table after it only some are in the
  ROM's order yet; the strings of the code are named `gStr_*` in `symbols.txt`, not by the compiler's numbers.
- What the date at +0x6C of `gGameData` is. `MapScene::IsNewDay` (func_0202cb08) is TRUE when it is not today's (a
  day starting at 5:00) or not set, which gives the fountain its building window and its entry to scene 0x30; when
  it is today's, the window asks a question (message 0xA4) and entering goes to the fountain's scene of the time of day.

## The Mii transfer

Wireless play (overlay 109, scene 0x29) takes Miis from another machine through code of the main program,
0x020776E4 to 0x02079FA0, which nothing else calls. It is three files under `lib/MiiTransfer/` (the name and the file
names are the project's own: none survives), all built with 2.0 SP2 patch 2 (see "Compiler"):

| File | Code | Data | Functions |
| --- | --- | --- | --- |
| `MiiTransfer.cpp`, the protocol | `0x020776E4`-`0x02078F58` | .bss `0x02145380`-`0x02145C00`, .rodata `0x020B7BE4`-`0x020B7E1C` | 53 of 54 match |
| `MiiTransferWH.cpp`, a second WH | `0x02078F58`-`0x02079DFC` | .bss `0x02145C00`-`0x02148680` | 45 of 45, complete |
| `MiiTransferList.cpp`, the Mii records | `0x02079DFC`-`0x02079FA0` | .bss `0x02148680`-`0x02148688` | 7 of 7, complete |

Measured:

- **The bounds.** The code before 0x020776E4 belongs to the code around 0x0206CF6C to 0x020776E4 that works on Miis
  from one pointer at 0x02145360; the functions after 0x02079FA0 are accessors of a Mii's bit fields, called through a
  table at 0x020FBC2C. Each of the three files reads its statics from its own block of .bss, and the blocks follow one
  another in the order of the code.
- **`MiiTransferWH.cpp` is WH again**, function for function in WH's order (`WH_ChangeSysState`, `WH_SetError`, ...,
  `WH_Finalize`, `WH_End`), with WH's system states, connect modes and error codes (`WH_ERRCODE_DISCONNECTED` 0x14,
  `WH_ERRCODE_NO_RADIO` 0x16). It differs from the main copy (`lib/WH`): it scans with `WM_StartScanEx` and connects
  with `WM_StartConnectEx`, a later revision; it has no strings; and it has only the child's side: of the parent's
  functions only those that end a parent's session are there, which `WH_Finalize` calls. One function is not in WH as
  the SDK has it, `WH_FindParentInScan` (`0x020791C4`), which picks the parent to connect to from a scan's results (by GGID, and the entry
  flag). Its statics are where WH's are, by size: the parent parameters (0x40 bytes) at +0x80, the scan parameters at
  +0xC0, `sBssDesc` +0x120, 0x200 bytes of a data set used by nothing in the ROM at +0x1E0, the scan buffer, the key
  set and data-sharing buffers, the send, receive and system buffers.
- **The protocol** (`MiiTransfer.cpp`) is a child only: it starts WH, scans for parents of GGID 0x412, keeps up to eight
  of them (a timer of 20 frames and an age each), connects to the one overlay 109 picks as an MP child, and exchanges
  packets of up to 0x60 bytes, one at a time, numbered by a sequence byte that is odd while one is unanswered. In them
  go messages: 0x10 a Mii record, 0x20 the answer to one, 0x30 to 0x50, 0xD0 idle, 0xE0 cut, 0xF0 the owner's nickname
  (`OS_GetOwnerInfo`). A Mii received is stored in a free slot of the list, or over the Mii with the same create ID.
- **The Mii records** are 0x4A bytes with the create ID at 0x18, copied by halfwords. That is the size and layout of
  the Wii's Mii data (`RFLCharData`). The list is the caller's (overlay 109 passes 100 slots).
- Its .rodata: the six bytes FF FF FF FF FF FF of the address `WH_StartScan` is given, copied byte by byte to the
  stack, then, with no padding, 28 names of 20 characters that nothing refers to ("Off", "Scanning", "Connecting", ... "DispRecv",
  ... "DispSend", ... "CuttingForSaveErr", "DispSaveErr").

Inferred: this is not the game's own code but a library built apart from it and linked in, as the Mii transfer that
Nintendo's DS games take Miis from the Wii with: it is built with another compiler than the game around it (below),
carries its own WH of another revision, has no debug strings, uses the Wii's Mii format, and only overlay 109 uses it.

Not done: `MiiTransfer_UpdateSession` (the session's update) differs only in five branch offsets of a jump table (the unlisted
states of case 3 branch to the function's end, where the original branches to the inner switch's break), with a
comment above it, so `MiiTransfer.cpp` is not complete. The same output comes from every build from 114 to 137
and under every one of the 78 `opt_*` pragmas that build knows, each on or off. Measured with the compiler:
`MiiTransfer_OnPacketReceived` (WH's receiver) is compiled with `#pragma opt_common_subs off` (with it on, sSeq and its low bit swap registers in
every form of the source), and `MiiTransfer_UpdateSession` is compiled at `#pragma optimization_level 2` with `opt_propagation off`
(its loops are neither rotated nor strength reduced, and the 0 it passes to `WH_StartScan` stays in r4); the
address is copied from a local array's initialiser, the 6-byte compiler object at 0x020B7BE4, and the 28 names are
another local array's initialiser that nothing uses, which the compiler keeps and puts in the same section right
after it, at 0x020B7BEA with no padding, as the ROM has them. Which statics of one size go where in .bss follows
from the order of all of the file's data objects, those two included; the declarations are ordered so that every
static is at its ROM offset.

## The Mii data

Right after the Mii transfer, `0x02079FA0` to `0x0207A9C0`, are the accessors of a Mii's data: `lib/MiiData/MiiData.cpp`,
97 functions, all matching, and its `.rodata` (`0x020B7E1C` to `0x020B7EE4`) placed: complete. The file name and the
struct name `MiiData` are the project's own.

Measured:

- **The bounds.** The code before it is the Mii transfer's last file; the code after it, from `0x0207A9C0`, is a
  linked list that the particle code (`0x0207AB20` on) and three other users call, and none of this file's functions
  do. Its `.rodata` follows the Mii transfer's, and the next object (`0x020B7EE4`) is read by code at `0x0207F7D4`.
- **A get and a set function for each bit field**, 88 in all, in pairs. The character code calls them through two
  tables of 44 (`data_020FBB7C` the setters, `data_020FBC2C` the getters) by a parameter number:
  `func_02098c40`, `func_02098cb4`, `func_02098dac` and `func_02098e50`. Parameters 0x26 and 0x27 are the birth
  month and day, which `func_02098dac` takes from the character's own record when the Mii has none.
- **The layout is the Wii's Mii data (`RFLCharData`)**: the record is 0x4A bytes with the create ID at 0x18 (as the
  Mii transfer measured), and each of the 44 fields has the place and width that the Wii's field of the same name
  has in its 16-bit word. Hence the names: sex, birth month and day, favourite colour (0x00), the name (0x02), height
  and build (0x16, 0x17, both read capped at 127), face, hair, eyebrows, eyes, nose, mouth, glasses, beard and
  mustache, mole (0x20 to 0x34), the creator's name (0x36). mwccarm allocates bit fields from the low bit, the
  reverse of the Wii's compiler, so they are declared in the reverse order of the Wii's.
- **0x36 is the creator's name here too**: the Mii maker (`MiiEditSexPage::Update`) gives a new Mii the 0x36 of saved
  character 0, or that character's name when `MiiEditScene::GetNameLength` rejects it. (Overlay 109's source calls it a
  nickname.)
- `MiiData_AssignCreateId` writes the create ID from a counter that it advances, six bytes beside it (the Mii maker
  passes `data_0212349c`, through `func_0209859c`) and a random number from the main Mii code (`NFL_Random`):
  byte 0 is `0xC0`, the random number times 4 and bits 12-13 of the counter.
- Two setters, of bit 2 of 0x20 (`MiiData_SetLocalOnly`) and bit 0 of 0x00 (`MiiData_SetFavorite`), are called through
  `func_02098cdc` and `func_02098ce8`. Inferred: they are the Wii's `localOnly` and `favorite`, which `RFLCharData` has
  at those places, the correspondence that names the other fields; the Mii transfer sets the first and clears the
  second on every Mii it receives. What the game does with them is not known.
- **Its `.rodata`** is the days of each month (`sDaysInMonth`, 13 words from month 0, February 29), which
  `MiiData_GetDaysInBirthMonth` reads, and right after it two Mii records that nothing in the ROM refers to: a male
  and a female one (sex 0 and 1), blank but for the Wii's defaults (height and build 64, hair 33 for him, 12 for her).

Inferred:

- **It is built with 2.0 SP2 patch 2, as the Mii transfer is, not with the game's compiler.** The two records survive
  only as part of one pool with the days table: so compiled with patch 2, the 0x94 bytes go at 0x34 in the file's
  one `.rodata` section. With the game's patch 3 they get a section of their own (they are 64 bytes or more, see
  "Compiler"), which nothing would refer to and the link would leave out. Some function of the file read them and was
  left out by the link, as nothing called it; the source has a stand-in for it (`MiiData_UnusedCopyDefault`), which
  the link leaves out too. Hence `lib/MiiData/`, beside the Mii transfer, with its compiler.

## Wireless play with items

Overlay 110, `src/overlays/ov110/ItemTradeScene.cpp`, one unit with `.text`, `.rodata`, `.data` and `.bss`: all 75
of its functions match, and its data is the ROM's: complete.

Measured:

- `ItemTradeScene` (scene 0xB1; `CreateScene`, case 177, loads overlay 110 and allocates 0x28E0C bytes; the
  constructor is the main program's, `0x02048B78`; virtual table at `0x0214F724`) is the other branch of wireless
  play from overlay 109's: the two DS exchange things the player owns, not Miis. Etc message 0xF9, "たべもの・服・
  お宝など" (food, clothes, treasure and so on), names it on the first window, beside "相手を募集する" (look for
  a partner, 0xFE) and "相手を見つける" (find one, 0xFF). It mounts the same scene archive as overlay 109
  (`STS`, "Scn/Scene/SceneTushin_LZ.bin") and its own list, "Scn/List/List_TushinItemList_LZ.bin". Leaving, or
  a lost connection once acknowledged, asks for scene 0x29 (overlay 109); a failed write of the save data for
  scene 0xC2 (overlay 107).
- The scene is a state machine of 20 states, each with a window kind (`sWindowKinds`) and an etc message
  (`sStateMessages`, 0xFD to 0x127): choose a role, recruit or search, choose a partner among up to three found,
  connect, then the list (no window), ask, send, receive, save, and the messages for sent, received, too many (99
  of a thing at most), disconnected and error. The window is the main program's message window (`MessageWindow.h`)
  with up to three buttons and OK, or yes and no.
- What is sent: WH (`WH_SetReceiver`, `WH_SendData`) carries four bytes for a thing, its id, with 0x800 added for a
  costume and 0x1000 for a room interior; and two bytes for a command: 2 a thing is coming, 3 ready for it, 4
  received, 5 not received (too many). A receive without data means the partner has gone. When both send at once,
  the child gives way and receives. The sender takes the thing out of its inventory and saves before it sends, and
  puts it back if the partner had too many. The partner's name shown on connection is `gWHChildSsid` (the parent)
  or the name of the parent found (the child); the parents found are kept by the main program's session layer over
  WH (`data_0213B700`, functions `0x0203B748` to `0x0203BCD4`), in the overlay's `sPartners` (3 × 0xC0 bytes).
- `ItemTradeList` (0x15A3C bytes at 0x10 of the scene) is the list of the player's things on the lower screen: the
  main program's scrolling list (`ItemListView` here, virtual table `0x021153BC`, functions `0x020A80F0` to
  `0x020A9E94`), five tabs (items, costumes, room interiors and two more item lists, each fed by an inventory
  function of the main program: `0x0209F0F0`, `0x0209E4AC`, `0x020A5174`, `0x0209F208`, `0x0209F28C`), a row of
  four filters under the items tab and five under the costumes tab, OK and back. On the upper screen it shows what
  is chosen: an item's model, a costume on a mannequin ("Mdl/Misc/headManikin_3D_LZ.bin" for the head), or a room
  interior, each sliding in and bouncing, and its name (explain messages).
- The list is not overlay 110's alone: its filter rows (`0x0214A1C8` to `0x0214A4CC`) are byte for byte functions of
  overlays 75, 85 and 100, and several of its own functions are overlay 100's (`0x0214AE10` to `0x0214B144`,
  `0x0214BB48`, `0x0214C9CC` there); the item model's `ChangeItem` is overlay 75's and 96's. Only four small
  functions are byte for byte overlay 109's (`SetEntry`, `WriteSave` and two empty callbacks): the two scenes do not
  share their code otherwise.
- The data, all at the ROM's addresses in our object (`tools/name_unit_data.py` lays `.rodata`, `.data` and `.bss`
  out where the ROM has them, and names them): `.rodata` is two 1-aligned arrays, the pool of the constants under
  64 bytes (0x2D0 bytes, in the ROM's order), an array, the window's screen name, and the five tables of 80 and 96
  bytes each on its own; `.data` the lights' colours, the virtual table and the strings in the order of the code;
  `.bss` the communication variables, sorted by size with WH's 32-byte aligned send buffers among them, then the
  partners' timers and the 0x240-byte list of parents found.
- Reads of a file-scope `static const` object with a constant index are folded into the code: the tabs' and
  filters' touch sizes, the buttons' positions and the cameras' vectors are in the pool though no instruction loads
  them. Local `const` scalars that nothing uses are in it too (0x3000 and 0x4000 here), and so is the initialiser
  of a local `char` array ("IM0"). In the pool's size sort, the `.bss` statics' declaration places count as well:
  adding a constant moves `.bss` objects of equal size.

Inferred: overlays 109 and 110 are the two halves of wireless play, written from common pieces (the session layer,
WH, the save-and-send sequence), but each from its own source; overlay 110's list is the code of the scenes that
list the player's things (75, 85, 100), copied in.

How the last functions matched:

- `SetupWindow` matches with the text's vertical alignment as an if/else and the buttons reading their tables
  directly (with a ternary, or the positions through a local pointer, `this` stays in r11 where the original
  spills it and keeps the positions there), `Update` with the lost connection's two ways as an if and its else.
  `SetItemCamera` and `SetRoomCamera` match under `#pragma opt_propagation off` with the vectors as struct copies,
  and `UpdateWireless` with its partner loop written as overlay 109's (`continue`, `mWindow.mTexts[i + 1]` at each
  use). Marking the unit complete took three dsd symbols out of `symbols.txt` that are fields inside
  `sWindowButtons` and `sWindowButtonMessages` (their references are now `add:` offsets in `relocs.txt`), the
  placeholders of the `.bss` blocks and one inside a string, `local` on the three static WH callbacks, and the two
  local constants of `ItemTradeList::UpdatePreviewStates` declared the other way round.

Not done:

- `tools/check_relocs.py` lists one reference: `UpdateRoom` reads the room style's light colour as
  `data_020f72f8[style].mLightColor` where the original refers to `data_020f7300`, as overlay 74 does.
- What the two item tabs besides the first list (`0x0209F208`, `0x0209F28C`), and what the selection's category and
  value (0x30, 0x34 of `ItemTradeSelection`) are for: nothing in the overlay reads them.

## Wireless play: the Mii exchange

Overlay 109, `src/overlays/ov109/TushinScene.cpp`: 71 of its 77 functions match, and every section of the
overlay is claimed by the one file. The unit is not complete. `docs/notes/ov109.md` has the working notes.

Measured:

- The scene factory (`CreateScene`, case 41: scene 0x29) loads overlay 109 (0x6D) and allocates 0x3F0B4 bytes; the
  constructor is the main program's (`_ZN11TushinSceneC1Ei`, `0x02040644`), which stores the overlay's virtual table
  (`0x02152ED0`, referred to with `add:0x8`). The save screen (overlay 69) asks for scene 0x29 after saving before
  wireless play. The scene mounts `Scn/Scene/SceneTushin_LZ.bin`, `Scn/List/List_TushinList_LZ.bin`, the etc and
  common messages, `Mdl/Ica/ica_mii_contents_LZ.bin` and `Mdl/Spl/SplTushinEffect_LZ.bin`.
- Its pages are etc messages 0xF7 to 0x125. The first asks what to exchange: Miis, or "food, clothes, treasure",
  which asks for scene 0xB1 (overlay 110, the goods exchange, which loads the same archive). Then with what:
  another Tomodachi Collection, through WH, or the Wii's Mii Channel ("look for a Wii to talk to... press A, B, 1, 2
  in the Mii Channel"), through the main program's Mii transfer code (`0x02077000` to `0x0207A000`, which only this
  overlay calls). Then, on a DS, whether to recruit a partner (the parent, `WH_ParentConnect` on the channel
  `WH_GetMeasureChannel` measured) or to find one (a child; the parents found are shown with their names for 600
  frames, `sParents` and `sParentTimers`).
- A resident is chosen in the list on the lower screen (the main program's resident list, whose sort order is
  `GameData::mMiiListSortKey`) and sent with `WH_SendData`: the 0x2C0-byte character (`sTransfer`) in six 0x80-byte
  chunks, then the songs of its six slots, 0x40 bytes each (`sSongs`). The receiver collects the chunks in its
  `WH_SetReceiver` callback, checks the character (`IsInvalid`: a broken Mii or a blank name or nickname; `HasNgWord`:
  any of its texts, nickname and name equal to one of the 41 words of `sNgWords` once trailing spaces and
  exclamation marks are taken off; a song with such a word), and stores it as a new resident or updates the one with
  the same Mii, then answers with a 2-byte code that the sender shows: 5 received, 6 and 7 the receiver's list is
  full (7: sent back), 8 it has the same special Mii, 9 it has the same Mii with another sex, 10 it could not be
  received (messages 0x10A, 0x11C, 0x11B, 0x118, 0x120, 0x123; the receiver's own are 0x10E to 0x122).
- The Wii only sends Miis: the scene gives the transfer code the Mii data (0x4A bytes) of all 100 residents
  (`sMiiData`), and `ReceiveWiiMii` adds or updates a resident from the one received.
- The first exchange of each kind gives a T-shirt: item 0x113 (DS) or 0x114 (Wii) of the clothes (`GiveItem`,
  messages 0x124 and 0x125). `ClothesItem` (`0x0209E168`) is named from it; that these 300 items are the clothes is
  inferred from those two messages only.
- `gWHChildSsid` (WH's copy of the connecting child's SSID) is shown as the partner's name when a DS connects
  (message 0x104).
- The scene's characters are three of the main program's 0x2B18-byte class that several scenes hold
  (`SceneCharacter`, constructor `0x0203E168`, destructor `0x0203E188`); a resident's first 0x2A4 bytes
  (`CharacterRecord`, the record `func_0202c264` reads) are copied into them by the record's implicit assignment,
  whose member-by-member copy shows the record's members (arrays of 5 and 6 bytes, s64 at 0x21C, 0x228, 0x230,
  0x23C, 32- and 40-byte structures).

Measured, about the compiler:

- **Some functions were compiled at a lower optimisation level.** Their loops are neither rotated nor strength
  reduced (a branch to the test first, indexed addressing), and `#pragma optimization_level 2` around the function
  gives them exactly: `SendSongs` and `HasNgSongReceived` match that way. None of the -O4 forms tried gives them
  (a volatile counter or buffer, `u8`/`s16`/unsigned counters, `while (1) { if (!cond) break; }`, `goto`,
  `opt_rotateloops off`, `opt_strength_reduction off`, `opt_loop_invariants off`), in any compiler version from 2.0
  SP1p2 to SP2p3. They are interleaved with -O4 functions (`HasNgSongReceived` is between two), so they are not a
  file of their own. `ReceiveCharacter`, `UpdateCharacters` and `Update` are compiled the same way here.
- **A `const` object initialised from another constant** is filled in by the static initialiser and stays in
  `.data` as zeros (the list's text origins, from the item positions). The original's initialiser loads the item
  positions from the `.rodata` pool instead of folding them; it matches with `#pragma opt_foldconstants off` in
  effect at the end of the file (inferred: the original set it somewhere and left it on).
- **`UpdateCharacters`** matches its hoisting and registers only without common subexpression elimination
  (`opt_common_subs off` at level 2), to one block.
- **`.rodata`**: the objects under 64 bytes are one pool sorted by size (8 to 56 bytes), the arrays of `u8` and the
  strings before and after it, and the 0x90- and 0x110-byte tables on their own after it. The compiled `.rodata` is
  in the ROM's order: same-size declarations were permuted until it was. Two of the pool's constants are folded
  wherever they are used and kept anyway (`sCameraPosition`, `sCameraTarget`, and the signal sprite's
  `sSignalPosition`).

- **The destructors** at `0x02152460` and `0x021524A8`, after the file's last function, are `TushinScene`'s `D1` and
  `D0`: the implicit destructor compiles to them, and the scene's virtual table refers to both.
- **`.bss`** is a pool of separate statics sorted by size (a `u8`, 20 `u16`, ten `int`, then `sParentTimers` and
  `sReceivedName`), which the code reaches from the pool's start; the source declares them so, named by address.
  13 of the `u16` (pool+0x06 to +0x1F) are not referred to by anything in the overlay. Inferred: functions that
  used them were stripped by the link, which keeps the pooled statics they used; a stand-in function keeps them
  in the compiled object.

Not done:

- 6 functions differ, each with a comment saying how: `Update` (99.9%), `UpdateCharacters` (99.3%),
  `ReceiveCharacter` (one `orr`'s operand order), `UpdateListWindow` (one register copy), `ReceiveWiiMii` (a -1
  derived from a register holding 0) and `InitParticles` (registers of the last block). All are register choice
  or order; no `opt_*` pragma or compiler build changes them.
- `.data`: the three 4-byte strings of `sCharacterArchives` and the 12-byte group of its pool come out in another
  order than the ROM's.

## The keyboard's object

`src/keyboard/Keyboard.cpp` (`0x02082EAC` to `0x02083DC4`, 34 functions) is complete: every function matches and the
unit is linked from its object. It is the object a scene holds for the on-screen keyboard (`SoftKeyboard`,
`include/keyboard/SoftKeyboard.h`, 0x5FF8 bytes), and sets up and draws the keyboard's core
(`KeyboardCore.cpp`) through it. Every function is a member of `SoftKeyboard` or `KeyboardButton`; the two that
the core calls back are static members. Measured:

- **Callers.** The main program's scene constructors construct it (`_ZN12SoftKeyboardC1Ev`, six calls), and
  `func_0204ce88`/`func_0204d178` of the main program and overlays 43, 75, 76, 92 and 93 use it. The set-up, the
  same in all six: `SetArchiveName("KBD")`, `InitBg(0x100, 0)` (the background plane, and the offset of the palette
  in `fs_keyboardColor.ncl`), `SetMode(0, FALSE)`, `InitObj` (the sprite resource, a message file, a character
  offset, an argument of `LoadObjCells`, the layout and the sprite palette: `0, file, n, -1, 0, 13` in the main
  program, `0, file, n, 0x14000, 4, 13` in overlay 43), `EnableTabs` and `GetButtonCharCount`. Each frame they call
  `Update` with a pad of 0 and `gTouchInput`'s touch data, then `KeyboardCore_GetChar`. `func_0204d178` loads the
  `MNU:BG_down` background onto the plane when A is pressed (`data_0211f9d4`'s trigger), and when B is, the
  keyboard's again with `InitBg` and `LoadObj`.
- **Background.** `InitBg` is `InitBgGraphics` (the core's buffers and the five NCGR paths, `KeyboardCore_InitGraphics`;
  the plane), `InitCore` (`KeyboardCore_Init` at the keys' origin (45, 59) with `PlayKeySound` as the sound
  callback, the screen made by `KeyboardCore_MakeScreen`, the plane scrolled to the origin), then
  `TransferCharData`, `TransferScreen` and `LoadBgPalette`.
- **Layout.** The plane, the palette number (13), the sound handle of the key sounds, the keys' screen data
  (0x800 bytes, which `func_02027f3c` sends) and two buffers of 0x2280 bytes that the core is given (`mBaseCharData`,
  `mCharData`; the second is what `TransferCharData` queues to VRAM), the six background paths (five of 0x30 bytes
  and the palette's of 0x20), five tab sprites, two buttons of 0x6B8 bytes, a sprite (`mLayoutSprite`, cell 7, at
  the one of five positions that `mLayout` selects: `PlaceLayoutSprite`), its resource and layout, and the three
  sprite paths. Every path starts `xxx:/`, which `SetArchiveName` overwrites with the archive's mount name.
- **The buttons** (`KeyboardButton`, `0x0208329C` to `0x02083740`) are a sprite with a `MessageText`: they show
  messages 6 and 5 of the file passed to `InitObj`, at (0x80, 0xAD) and (0, 0xAD), and play a sound of
  `gSoundManager` as they are touched and as they are released. `Update` returns TRUE when the stylus is lifted on the
  button. `mPressed` is set as it is touched and `mHeld` while the stylus stays down; `Reset` (with the button's
  index) and `Release` (which also shows frame 0) clear both. Overlay 43 calls `Release` before giving a button a new
  message.
- **The tabs.** The five areas of `sTabAreas` (`0x020B7F78`) are the core's extra touch areas
  (`KeyboardCore_SetAreas`): `InitCore` registers only the first two, without a callback, and `EnableTabs` all five
  with `OnTabArea`, which the core calls with an action (1 pressed, 3 lifted, 4 left), the code pressed and the code
  under the pen. On a press it lights that tab and selects keyboard mode 0 to 4 (`KeyboardCore_SetMode`).
- **Data.** `.rodata` is one pool, `sButtonPositions`, `sButtonRect`, the local table of the five modes,
  `sTabAreas`, `sLayoutPositions`; `.data` is the nine paths. The `gText_Yu` and `gStr_F` that the string tool had
  named here were `sButtonPositions` + 4 (the code reads the y column off it) and `sLayoutPositions`. The object has
  the nine paths twice, once for each of the two constructors the compiler emits (`C2`, then `C1`); the ROM keeps
  `C1`'s, the second set, so `tools/name_unit_data.py`, which lays out every set, names the wrong ones and runs past
  the range.

Measured: the object is 0x5FF8 bytes. The Mii maker's profile page keeps a text of its own from +0x5FF8 of its
keyboard (it clears 0x16 bytes there and counts the characters at +0x6010), and two scene constructors of the main
program construct their next member at +0x5FFC from the keyboard's start. Nothing in this file touches anything
past +0x5FF8.

## The keyboard's core

`src/keyboard/KeyboardCore.cpp` (`0x0208179C` to `0x02082EAC`, 35 functions and a static initialiser): the keys, the
touch and pad handling, and the drawing of the key labels into character data. It is complete: every function and
all its data match, and the unit is linked from its object. Measured:

- **State.** Every function works on one static of 0x50 bytes, `sKeyboard`: the key touched first and the key
  under the pen (`pressed`, `current`), the layout (+0x8), a drop callback and its argument (`dropCallback`,
  `dropArg`: called as the pen is lifted after leaving a character key, with the character and the point; nothing
  in the ROM sets it), a sound callback and its argument (+0x14, +0x18, called through `KeyboardCore_PlaySound` with
  sound numbers 0 to 10), the layout's characters (+0x1C), a key repeat of the input code (+0x20, which `relocs.txt`
  refers to as `sKeyboard` + 0x20), the code of this frame's key (+0x28) and the character it gives (`character`,
  +0x2A, which `KeyboardCore_GetChar` returns to the callers), the character dragged off its key (`dragChar`,
  `KeyboardCore_UpdateDrag`), the modifier (+0x2E: 0, 3 or 4, the code of the modifier key that is on), the layout
  number (+0x30), the pad cursor (+0x34), a flag that makes the next update give the cursor's key
  (`pressCursorKey`; nothing in the ROM sets it), the flag that makes `sExtraKeys` keys (`extraKeys`, which
  `SoftKeyboard` passes as 0), and the extra touch areas with their callback (+0x40 to +0x4C: `SoftKeyboard`'s
  tabs).
- **The .bss pool**: `sCharData` (the buffer `SoftKeyboard::TransferCharData` sends), `sOriginX`, `sInitialized`,
  `sBaseCharData` (the layout's graphics with the labels drawn, copied over `sCharData` before the pressed key and
  the modifier are lit by `KeyboardCore_LightKeys`), `sOriginY`, then 0x18 bytes no function in the ROM refers to
  (`data_021486d8`, unnamed for that reason).
- **Keys** are 8 bytes: x, y, width and height as bytes, read as u16 bit fields (one `ldrh` for x and y), then a
  signed link to the next key of the same group (the keys lit together), then the code. Codes 1 to the layout's
  count of special keys (`numSpecialKeys`, +0xC of a layout) are special keys (1 delete, 2 space, 3 and 4 the
  modifiers, 20 enter); above it, a code indexes the layout's characters (`KeyboardCore_CodeToChar`); 0x100 and above
  are the extra areas'. `sExtraKeys` are two more keys of code 0 at the bottom, searched only with `extraKeys`, and
  START puts the cursor on the first.
- **Layouts**, `sLayouts`: five of 0x10 bytes (keys, NCGR file, a function called when a key gives a code, the count
  of special keys, the count of keys). They are English, kana, European, signs and pictograms
  (`KeyboardCore_UseEnglishLayout` to `KeyboardCore_UsePictoLayout`, chosen by `KeyboardCore_SetLayout`). Their
  functions are `KeyboardCore_OnEnglishKey` (the modifiers choose lower case, symbols or upper case,
  `KeyboardCore_ApplyModifier`), `KeyboardCore_OnKanaKey` (3 hiragana, 4 katakana) and three empty ones. Their files
  are `SoftKeyboard`'s five background paths, in the order jp, English, euro, sign, picto. `KeyboardCore_SetMode`
  takes the tabs' order (kana, English, euro, sign, picto) and maps it to a layout.
- **Kana**: four tables of 60 characters (plain kana, then what dakuten, handakuten and the small form make of each,
  a full-width space for none) and three pairs (plain, other) that the static initialiser fills from
  `sKanaTables`. Nothing in the ROM reads the pairs. Inferred: code that read them was dropped by the link, as
  were the users of the 0x18 bytes of `.bss` and of two 20-byte local tables in `.rodata` (`{2,0,3,4,1}`,
  `{2,3,1,4,0}`, after `KeyboardCore_SetMode`'s `{0,2,1,3,4}`). The source keeps them through an uncalled function.

The character and key tables of the layouts, `0x02112220` to `0x02112DC4`, are left out of the unit and stay in the
gap (`KeyboardChars*`, `KeyboardKeys*`). Measured: their sizes in memory order are 0x5C 0x5C 0x5C 0x70 0x68 0x68
0x70 0x68, then 0x188 0x1B0 0x1D0 0x1A8 0x1C8. That is not the size order the compiler gives one file's data,
measured for C and C++, with and without `-ipa file`. The code takes a key table's count as the bytes between its
start and its end (two literals per table, the end being the next table's address). It reads a character table
through a pointer cast with a separate `add` (mwcc folds `&table[1]` into the literal for an object of its own
file, and does not fold it after a cast). The character tables are UTF-16 text with a byte-order mark (`0xFEFF`,
skipped) and no terminator. Inferred: they are binary files linked as objects of their own, between NFL's data and
this file's.

## The Mii maker

Overlay 76, `SceneMiiEdit2`: 83 functions, `.text` `0x02149A40` to `0x02158748` (60 KB). It is cut as one unit,
`src/overlays/ov076/MiiEditScene.cpp`, which claims every section. 24 of its 83 functions are written and match
(the scene's StartLoading, Draw, Exit, camera functions and destructors, and the pages' Draw and Exit); the header
holds the scene's layout and its seven pages (`mPage`, +0x150: 0 the sex choice, etc message 0x1A; 1 the face, 0x1C;
2 the profile, 0x1D; 3 the voice, 0x21; 4 the personality, 0x22; 5 and 6 the questions of finishing).

Measured:

- The scene factory (`CreateScene`, cases 66, 67, 68 and 111: scenes 0x42, 0x43, 0x44 and 0x6F) loads overlay 76 (0x4C)
  and allocates 0x983F8 bytes. The constructor is the main program's (`func_0204332c`), which stores the overlay's
  virtual table at `0x0215A120`. Its entries are StartLoading `0x02149BAC`, Init `0x02149C98`, Update `0x0214A9B0`,
  Draw `0x0214ADE8` and Exit `0x0214B060`, then the two destructors `0x02158618` and `0x021586AC`.
- StartLoading mounts `Scn/Scene/SceneMiiEdit2_LZ.bin` (`E00`), `Cmn/Kbd/SoftKeyboard_LZ.bin` (`KBD`), the etc and
  common messages (`TBM`, `T02`), `Mdl/Spl/SplEditEffect_LZ.bin` and `Mdl/Ica/ica_mii_edit_LZ.bin`.
- Init switches on the scene id. For 0x42 and 0x44 it takes a resident's index from `data_0213ba74`+0x20 and reads
  that saved character into the scene's character at +0x710 (`func_0202c264`, through `0x0214B670`). For 0x43 and 0x6F
  it takes the first of the 100 save slots that `func_0202c354` reports free, and stops (`OS_Terminate`) when none
  is. It then sets the character up with a fresh Mii and today's date. **Inferred**: 0x43 and 0x6F make a new Mii,
  and 0x42 and 0x44 edit an existing one. How 0x42 and 0x44, and 0x43 and 0x6F, differ is not established.
- The layout, from the constructor and the destructor: six sprite buttons at +0x158; a character (`CharacterBase`
  and its `CharacterModel`, destroyed inline) at +0x710; a second `CharacterBase` at +0x3228, constructed by
  `func_0208fd18` and destroyed by `_ZN13CharacterBaseD1Ev`; a `TtsPlayer` at +0x5920; nine `ObjTextBox`es at
  +0x1A368; two message files at +0x1C0F8; ten message windows (2 at +0x1C138, 6 at +0x354B4, 1 each at +0x824A4 and
  +0x8D454); the on-screen keyboard at +0x77D6C (`func_02082eac`); and many sprite and text buttons
  (`MiiEditScene.h`).
- `func_0208fd18` has the same body as `_ZN13CharacterBaseC2Ev` and builds the complete object that the destructor
  destroys with `D1`, so it is now `_ZN13CharacterBaseC1Ev`. Both call `func_020979a0`, which has the same body as
  `_ZN14SavedCharacterC1Ev` just after it, so that is now `_ZN14SavedCharacterC2Ev`.
- None of the 83 functions has the same bytes as a function already decompiled elsewhere (compared with every
  overlay's disassembly, the overlays' own symbols masked).
- `.data` holds a 0x120-byte `const` table at `0x0215A13C` that is zero in the ROM. The static initialiser fills it
  from two pairs of words of the `.rodata` pool (offsets 0xBC and 0x74), as overlay 109's text origins are filled.

Measured, from the last third of the file (`0x02153CF4` on):

- **The pages' Init and Update.** The scene's page switch (`MiiEditScene::SetPage`) calls the old page's Exit and then
  the new page's first function with the scene (pages 5 and 6 also get the page that was open). The scene's Update
  calls one function of the page shown. Each page keeps the scene in a member.
- **Pages 5 and 6** are a question in a window with two buttons, common messages 2 and 3 ("はい", "いいえ"), under
  etc message 0x2E (5) or 0x2D (6). The first button leaves the scene: page 6 first calls `MiiEditScene::SaveCharacter` and
  passes the resident on in `data_0213ba74`+0x20 (scenes 0x74 or 0x69); page 5 asks for 0x1A or 0x5E. The second
  goes back to the page that was open.
- **The personality page (4)** has five sliders of eight steps. They set the five bytes at +0x4A of the record
  (`func_02098d5c`), 1 to 8, 0 while unset.
- **The profile page (2)** keeps its own text (11 `wchar_t` and a count) right after its keyboard. The keyboard
  object is therefore 0x5FF8 bytes. Its helpers show the birthday and a four-digit number with a window's plain
  sprites (MessageWindow +0xA9F4, five sprites that `func_020345d8` draws), and the profile in the scene's nine text
  boxes. That number is bits 17-28 of the record's +0x50, 0xFFF for none.
- **The voice page (3)** has six voice presets (`data_020FBAAC`, six `s16` each), a random voice (`MATH_Rand16`
  over each value's range, `data_020FB940`), five sliders and a choice of four for +0xEE..+0xF9 of the record (six
  `s16`, `func_02098d68`). It keeps two copies of the record: one to undo a change, one to restore on cancel.
- **The voice page speaks the profile** with the scene's `TtsPlayer`, one part at a time (`MiiEditVoicePage::SpeakProfile`,
  etc messages 0x2F to 0x36) through `CharacterSpeechTexts`, 0x294 bytes of texts that `func_0204E4B4` makes from a
  record. Each part goes on once the engine is idle.
- **The star signs.** One part speaks the star sign of the birthday: `data_ov076_02158D34` holds the first day of
  each sign in its month, from January's (21, 19, 21, 21, 22, 22, 23, 23, 24, 24, 23, 22), and etc message 0x38 is
  the sign that begins in January.
- **The record's members.** The voice page copies the 0x2A4-byte record with its implicit assignment. That copy
  goes member by member and shows overlay 109's layout of `CharacterRecord` exactly, down to twelve single bytes at
  +0x244 and single bytes from +0x294.

Measured, from the first third of the file (to `0x0214EDB8`):

- **The scene's own functions.** `MiiEditScene::LoadCharacter` reads the saved character into the scene's character and
  `MiiEditScene::LoadModel` loads its model ("B0I"), by sex. `MiiEditScene::SaveCharacter` writes the character back
  (`func_0202c28c` with the index at +0x5824): it sets one of eight flag bits of the record's +0x210 and the 7 bits
  at +0x54 (overlay 109's `mInterior`) from the personality's row (`data_020fba28`) and the sex, and for scenes 0x43,
  0x44 and 0x6F resets some of the record's fields. The four functions beside `MiiEditScene::ShowControls` show or hide the
  scene's buttons and texts with one of the lower screen's backgrounds ("BG_down", "BG_down_editface",
  "BG_down_nobar").
- **mSprite582C** is a pointer at one of five places along the lower screen (`sSpriteB0Places`, under the gaps
  between the scene's five page buttons). Init puts it at 4 for scene 0x42, 2 for 0x44 and 0 for 0x43 and 0x6F;
  `MiiEditProgressSprite::AdvanceTo` only moves it to a higher place.
- **Page 0** is the sex choice: two windows (etc messages 0x1A, 0x1B), with a third button for scene 0x43 and other
  button positions for 0x6F. Its Update sets the sex, the default colours of the sex (`data_020fbaac`), a blank
  name, and the creator's name from saved character 0 (its name if `MiiEditScene::GetNameLength` finds the creator's name
  empty), and asks for scene 0x5E when the window's third answer is chosen.
- **The face page** has eight parts (`MiiEditFacePage::ShowPart`): 0 the body, with the two sliders for parameters 0x29 and
  0x2A (inferred: height and build; the camera is raised with 0x29); 1 the face (`MiiEditFaceTab`); 2 hair, 3 eyebrows, 4 eyes,
  5 nose and 6 mouth, each with its graphics ("E00:/Hair.nce" and so on), a colour palette and colour buttons where
  the part has a colour, and pages of twelve parts (6, 2, 4 and 2 pages for hair, eyebrows, eyes and mouth); 7 the
  other parts (`MiiEditEtcTab`). The type parameters are 0x19 (hair), 6 (eyebrows), 0 (eyes), 0x1C (nose), 0xC
  (mouth), the colours 0x1A, 7, 1 and 0xD. The camera goes to a point raised by `(parameter 0x29 * 0x1130 >> 7) - 0x898` for
  all parts but the body.
- **The data.** All 122 objects of `.rodata` are written out and come out at their ROM addresses
  (`MiiEditScene.cpp`, between the BEGIN and END markers); see decompiling.md, "How a file's data is laid out".

Measured, from the middle of the file (`0x0214EE7C` to `0x02153A8C`) and the etc messages (decoded from
`Msg/TmdEtcMsgJP_LZ.bin`: LZ10, a NARC holding the BMG):

- **The face page's parts.** `GetPartType` maps a part button to a Mii part type through one table per part
  (`sHairTypes`, 72; `sEyebrowTypes`, 24; `sEyeTypes`, 48; `sNoseTypes`, 12; `sMouthTypes`, 24), and
  `GetPartButton` is its inverse. `SetParameter` sets a Mii parameter through the main program's setter. For the eye
  and eyebrow types it also turns the eyes (parameter 3) or eyebrows (9) by the difference of two entries of
  `sEyeRotations` or `sEyebrowRotations`, then plays the particle effect at the head, raised by a height for the part.
  It keeps the parameter's old value, and button 27 swaps it back and forth: undo and redo (`SetUndoMode`, the
  button flipped for redo).
- **Two tabs of the face page** are objects of their own at +0x3410 and +0x341C: the face (`MiiEditFaceTab`, Mii
  parameters 0x23 to 0x25: type, colour, texture) and the other parts (`MiiEditEtcTab`: glasses 0x1F/0x20,
  mustache 0x12/0x11, mole 0x15, beard 0x10/0x11).
- **The profile page's states**: 1 the ten buttons, 3, 5, 7 and 9 the keyboard for the name (etc message 0x1F, the
  record's +0x58), the family name (0x1E "苗字", +0x6E), the nickname (0x20, the Mii data's name) and the creator
  (0x24). 10 to 16 open its six windows: blood type (+0x50 bits 29-31: Ａ, Ｂ, Ｏ, ＡＢ, 4 for none), favourite colour
  (parameter 0x28, etc messages 0x44 on), the birth year (+0x50 bits 17-28, 1900 to 2099, 0xFFF for none; the voice
  page speaks it as 0x32 "{n}ネンウマレ"), the birthday (0x26, 0x27), the relation to the player (+0x298: etc messages
  0x54 + 0 other, 2 spouse, 3 parent, 4 child, 5 sibling; page 0 sets 1, 本人, for the player's own Mii) and the sex.
  The windows' choices are read from +0xAF7C of the message window (`mUnknownAF7C`), and the original reads it and
  `mResult` anew at each use.
- **Pages 5 and 6** ask "保存しないで終了します。よろしいですか？" (0x2E, finish without saving) and
  "登録します。よろしいですか？" (0x2D, register), hence `MiiEditQuitPage` and `MiiEditRegisterPage`. Page 0 is
  `MiiEditSexPage` (0x1A "性別をえらんでください", then 0x1B "顔を作りますか？").

Not done in the middle: `MiiEditProfilePage::SetState` (99.97%: six stack slots of the WindowPoints held across its
windows' set-up).

Not done in the first third: `MiiEditScene::Init` (95.8%: the registers of the index and the character offset in the
loops over the buttons and the text boxes).

## Items, the mannequin, the room and the Mii list

Four files of the main program after the character code (`include/item/`, `include/room/`, `include/list/`):

| File | Range | Holds |
| --- | --- | --- |
| `src/item/Item.cpp` | `0x0209E140`..`0x020A00BC` | the item classes, the clothes list, the inventory, ItemModel, the random choice of goods and clothes |
| `src/item/Mannequin.cpp` | `0x020A00BC`..`0x020A03E8` | the mannequin that shows clothes |
| `src/list/MiiList.cpp` | `0x020A03E8`..`0x020A3C68` | the list of saved characters, its panel and tabs |
| `src/room/Room.cpp` | `0x020A3C68`..`0x020A5CC4` | a resident's room: door, furniture, interiors |

Measured:
- The character file ends at `0x0209E140`: `0x0209E0E8` and `0x0209E114` are one clamp template instantiated for
  `s16` and `u8`, called only by `0x02097D8C`, and such instances follow the last function of the file that uses
  them. The item file ends with ItemModel's implicit deleting destructor (`0x020A0078`, entry 1 of its table).
- `Item` (virtual table `_ZTV4Item`, `0x02114F78`) has seven entries, all 0: pure virtual functions. Its three
  kinds read their tables: `ClothesItem` the clothes (`data_020f3ff0`, 0x14 bytes a row, 300 rows), `InventoryItem`
  the goods (`data_020f5f0c`, 0x14 bytes, 255 rows, 236 of them saved), `InteriorItem` the interiors
  (`data_020f72f8`, 0x54 bytes, 78 rows). The seven virtual functions return the price field twice, the model name
  (+8 for goods and clothes, +0xA for interiors) and four weights, one of which `gGameData+0x4E` selects; the random
  choices of goods, clothes and interiors on offer sum those weights. The goods' and clothes' getters are emitted
  at `0x0204ABAC`..`0x0204AD84` (in the scene constructors' file), the interiors' at the end of Room.cpp.
- `ItemModel` derives from `AnimModel` and `InventoryItem`: its constructors build the item base after the model
  and then store both halves of `_ZTV9ItemModel` (the second at `0x02115000`).
- `Room` derives from `InteriorItem` and `RoomModels`, which holds the room's own model with its door (`RoomDoor`,
  a node callback shows the node "entrance" while the door moves; node description 0 gives the matrix the
  furniture is placed by) and six pieces of furniture (`RoomPart`, faded out between the camera and the room).
- The list's `.rodata` ends at `0x020FBFD4`; the room's begins there with an unreferenced halfword `0xFFFF` (its
  objects sort by size: 2, 0x10, 0x14 bytes) and ends at `0x020FBFFC`.

The Mii list (`MiiList.cpp`, complete, 66 of 66), measured:
- `MiiList` holds the 100 saved characters in the order of `mSortKey` and in up to 17 groups of that order (the
  kana rows of the names, the sexes, months, blood types, personalities); `MiiListTabs` shows one of five tabs of
  group buttons; `MiiListPanel` a page of buttons with the names. The panel has one button more than a page and uses
  them round (`GetButtonEntry`): scrolling a row writes only the button that comes into view (`ShowEntryAbove`,
  `ShowEntryBelow`), and a button's text is written again only when its character or the mode changed. A mark beside
  a name (`mIcons`) shows that the names, the creator's name, the birthday and the blood type are all set.
- The name sorts move the names that start with no kana (`CharacterSort::GetNameInitialGroup` 10) after the kana
  ones by exchanging the two runs (`SwapRuns`), and fold groups 11 and 12 into 10.
- `RemoveSelected` will not remove the first saved character (index 0); otherwise it removes the selected one from
  the list and `EraseCharacter` from the saved game: the other characters' relations with it are reset (meter 100,
  rank 0) and their long acts and troubles with it ended (`ForgetCharacter`), the news items about it dropped
  (`RemoveNewsOf`), the pairs naming it dropped from the two 100-entry tables of character pairs at
  `gGameData+0x155E4` and `+0x154B8` (`RemovePairsOf`), the saved songs' notes for it changed (`RemoveFromSongs`) and
  its record reset (`ClearRecord`).
- The list copies whole characters as 0x2A4 bytes in one block and then `SavedCharacter`'s own fields. A derived
  class's implicit assignment copies its base field by field (measured with the game's compiler), so the original
  `SavedCharacter` held the record as a member, not as a base (inferred); the source copies through a mirror
  structure.
- Overlays 70, 78, 95 and 109 and the main program's `MiiListScene` use the list.

Inferred: the classes' names are descriptive; none survives in the ROM. That the first virtual function is a price
rests on the values (250, 480, 5800).

## The Mii library (NFL)

Code of the main program from `0x02069BF4` (right after NitroSystem) to `0x020776E4` (the Mii transfer), 144
functions, in `lib/NFL/NFL.c`.

**The names are the project's own.** Nothing in the ROM gives the library's names (no string, no assert, no object
to match), so every function, datum, type and field of `lib/NFL` is named for what its code does: `NFL_` for the 33
functions called from outside the library (by `main`, the character code, `lib/MiiData`, `lib/MiiTransfer` and the
overlays, found in their callers), `NFLi_` for the others, `sNFL` for its data.

Measured:

- **Its entry points.** `main` mounts `Cmn/Nfl/NFL_Res_LZ.bin` as `NFL`, gets `NFL:/NFL_Res.dat` and passes it to
  `NFL_Init` with a work area (`data_02115480`), `data_02115574`, `0x4000` and `data_020fc504`. That function returns 8
  when a pointer is null or the size is not a multiple of 4, stores the RTC time in seconds at +0x58 of the work area
  (`randState`) and the constants of an LCG (`0x5D588B65`, `0x269EC3`) at +0x5C/+0x60, and publishes the work area in
  `sNFLWork` with interrupts disabled. Every frame `main` calls `NFL_FillBuffer`, and after `OS_WaitIrq`
  `NFL_LoadVram(0x4000)`: both are state machines on +0x8C and +0x90 of the work area (`phase` and `stage`) that return
  small codes (0, 2, 3, 4, 8, 0x13, 0x14).
- **What it does.** `0x0206CF6C` to `0x020776E4` reads its state through `sNFLWork` (38 references), builds
  display lists (`G3_BeginMakeDL`, `G3C_Vtx`, `G3C_TexCoord`, ...), loads textures and palettes (`GX_BeginLoadTex`),
  and reads `OS_GetMacAddress` and `RTC_GetDateTime` (as a Mii's create ID is made of). `0x02069BF4` to `0x0206CF6C`
  is a byte-stream reader and display-list drawing code that calls back into `0x0206CF6C`, `0x020743E4` and
  `0x02074444`, and is called from the other part.
- **One library, by its table.** The function table `sNFLPartPreparers` (.data) holds `NFLi_PrepareFaceParts` and six
  functions of `0x0206AAEC` to `0x0206B024`, so the two parts are linked together. Its .data is inferred to run from
  `0x02112140` to `0x0211220C`, where the keyboard's tables start; its .rodata from `0x020B6524`, read by both parts.
- **Callers.** `main`, the main program's character code (`CharacterBase`'s destructors call `NFL_RemoveFaceSlot` and
  `NFL_ResetFace`), overlays 70, 76, 78 and 79 (`NFL_GetFaceStatus`), and 48, 51, 68, 75, 78 and 100
  (`NFL_GetFavoriteColor` to `NFL_IsMiiCreateIdUnassigned`).
- **One static.** The library's only .bss is the pointer `sNFLWork`: every one of its 38 references loads
  the address and reads or writes at offset 0, and nothing in `0x02069BF4..0x0206CF6C` touches .bss. The 0x1C bytes
  after it, to the Mii transfer's .bss at `0x02145380`, are not referred to. So .bss cannot show where its files
  begin and end, and the compiler's pooling threshold (137 against 138) cannot be read from its statics; its .rodata
  is the place left to look (`sNFLFacePlanes` and its words at +4, +8 and +0xC are each loaded by their own
  address).
- **No identity string.** The main program holds only two middleware strings, `[SDK+NINTENDO:BACKUP]` and
  `[SDK+SHARP:SHTTS-1_2_0]`; nothing names NFL's version. No objects or sources of NFL (or the Wii's RFL) were found
  on the machine to match against.

Started in `lib/NFL/NFL.c`, one unit for the library's whole range (.text `0x02069BF4`-`0x020776E4`, .rodata
`0x020B6524`-`0x020B7BE4`, .data `0x02112140`-`0x0211220C`, .bss `0x02145360`-`0x02145380`), not yet divided into files.
29 of 144 functions match: the byte-stream reader, the palette makers (16-colour ramps from tables of 8-bit colours,
`GX_RGB` of each component shifted down 3) and the create-ID and work-area helpers. Measured with them: the code
compiles the same with 2.0 SP1p2, SP2, SP2p2 and SP2p3, as C and as C++, so the compiler is not settled. The Mii records
it reads have the Wii's layout: the birthday check `NFLi_IsValidCharData` reads the month from bits 10-13 and the day
from bits 5-9 of the first halfword and checks them against a table of days a month, and the favourite colour in bits
1-4 against 11, as `RFLCharData` places them.

Measured from the functions of `0x0206AAEC`-`0x0206EBB0` (the display-list builder, matched but for four):

- **`NFL_MakeFace(index, mii, partDepth)` builds a face's two display lists.** It rejects a Mii whose fields are out of
  range (`NFLi_IsFaceDataInRange`, a bound for each of 37 fields) or whose birthday is invalid, takes the face
  (`NFLFace`) of `index` (`NFLi_GetFace`), gives it VRAM from the work area's bases at 0x8 and 0xC (`faceTexBase`,
  `facePlttBase`: 0x1A10 bytes of texture and 0x130 of palette per index, 0x6880 of texture in mode 7, the mode being
  the work area's 0x68, `texFmt`), runs the seven part functions of `sNFLPartPreparers` on a context on its stack, and
  makes the display list at 0x2A4 of the face (`dl`, 0x1AE0 bytes) from eight mesh functions and the one at 0x1D84
  (`partsDl`, 0x494 bytes) from `NFLi_MakeFacePartsDL`. `NFL_DrawFace` sends them to the geometry FIFO under the face's
  matrices (`mtx`, a 4x3, and `nrmMtx`, a 3x3 loaded as the vector matrix).
- **The resource is a table of 20 categories** (`NFLi_GetResourceItem(category, index)`: two halfwords, 20 offsets,
  then in a category a count, a halfword and the offsets of its items). A mesh item is vertices and normals (three
  halfwords each), texture coordinates (two), then strips of index triples. The meshes read are: 0 the beard (by its
  type), 4 the face (by the face type; it also holds the three points the other meshes are placed at), 6, 9 and 0x12 the
  hair (by the hair type), 7 the glasses, 0xF and 0x10 the nose. The textures are categories 1 (beard), 2 (eyes), 3
  (eyebrows), 5 (face), 8 (glasses), 10 (hair), 11 (mole), 13 (mouth), 14 (mustache) and 17 (nose), by the streams that
  load them (`sNFLFaceStreamSources`, `sNFLSharedStreamSources`), and 12 is the shared palette.
- **A face keeps the fields it was built from** (0x180, `keptFields`, 18 halfwords) and a value and a state for each of
  21 textures (0x1A4 and 0x228, `texStreams` and `plttStreams`, 12 bytes each): the part functions compare the Mii with
  the kept fields and set a state of 0 where a texture must be redrawn, 7 where there is none; 1 to 6 are states in
  progress.
- **Hair types** 5, 7, 0x18, 0x1D, 0x2A, 0x35, 0x3A, 0x45 and 0x47 have a texture of their own, and 0x22 and 0x39 are
  drawn in the favourite colour; hair flip mirrors the hair meshes with a scale of -1 on the X axis.
- **The colour texture.** `NFLi_MakeColorTexture` makes an 8x8 image of 2-bit pixels, four 4x4 quadrants of colours
  0 to 3, and `NFLi_MakeColorTexturePltt` its palette: the hair, face, beard and favourite colours (`sNFLHairGXColors`,
  `sNFLFaceGXColors`, `sNFLFavoriteGXColors`). The untextured meshes take their colour from it by their texture
  coordinate: (8, 0) the face colour for the hair's category 6 and the nose's 0xF, (0, 0) the hair colour for
  category 9, (8, 8) the favourite colour for 0x12, (0, 8) the beard colour for a beard without texture. Hence the
  meshes' names (`NFLi_MakeHairSkinMesh`, `NFLi_MakeHairMesh`, `NFLi_MakeFavoriteColorHairMesh`, ...).
- **The colour tables** at `0x020B6558`, `0x020B65A0`, `0x020B65E8`, `0x020B6630`, `0x020B6678` and `0x020B66D8` are
  arrays of {r, g, b} words (`sNFLGlassesColors`, `sNFLFaceColors`, `sNFLEyeColors`, `sNFLMouthColors`,
  `sNFLHairColors`, `sNFLFavoriteColors`, by the palette makers that read them and the fields their callers pass), and
  `0x020B676C` four planes of {a, b, c, d} (`sNFLFacePlanes`: `NFLi_GetPartCorner` takes a part's Z from the
  plane of its quadrant). The symbols that sat on their inner words were fields, now addends in `relocs.txt`.

Measured on the compiler: `NFL_MakeFace` matches only under `#pragma optimization_level 2` with
`#pragma opt_common_subs off`. The three hair meshes (`NFLi_MakeFavoriteColorHairMesh`, `NFLi_MakeHairMesh`,
`NFLi_MakeHairSkinMesh`) and `NFLi_DLScale` load every call's arguments from the last to the first, keep their loops
unrotated and test the hair type with a chain of materialised booleans. Inferred: they were compiled with eight of the
IR optimiser's passes off (`opt_common_subs`, `opt_propagation`, `opt_loop_invariants`, `opt_strength_reduction`,
`opt_rotateloops`, `opt_scalarize`, `opt_rebuildlogicals`, `opt_collapselogicalselectionintobittest`) and their
display-list commands go through inline functions, whose arguments that mode binds last to first; with that,
`NFLi_MakeHairSkinMesh` and `NFLi_DLScale` match. No other function of NFL loads its arguments in that order.

The last third, `0x02073514` to `0x020776E4`, measured from its code (in `lib/NFL/NFL.c`):

- **A random Mii.** `NFLi_MakeRandomMii` fills a Mii's data from the library's LCG (`NFL_Random`, state at +0x58 of the
  work area, the constants at +0x5C/+0x60: `randState`, `randMul`, `randAdd`): each part from a picker
  (`NFLi_RandomFaceType` to `NFLi_RandomEyeColor`) that takes one of the choices of row `sex * 9 + b * 3 + c` of a table
  (`sNFLRandomFaceTypes`, `sNFLRandomFaceTextures`, ..., and tables that are local arrays copied to the stack), the
  face's parts set lower by up to 4, a beard on one man in five; height and build 64, names, create ID and birthday
  cleared, favourite colour random. `NFL_MakeRandomMii` checks its arguments (it tests the first one where the second
  and third are meant), `NFLi_MakeRandomMiiOf` picks 2 (sex) and 3 (b, c) at random. The tables of the pickers that are
  local arrays are named after them (`sNFLRandomHairTypeChoices`, `sNFLRandomHairTypeCounts`, ...); the compiled code
  refers to its own anonymous copies. `NFL_CopyDefaultMii` copies one of six default Miis (`sNFLDefaultMiis`, 0x4A bytes
  each).
- **Streams into VRAM.** The work area holds a ring buffer (+0x78 `buf`, +0x7C `bufSize`; write offset +0x84, read
  offset +0x80, bytes pending +0x88) and seven streams of 12 bytes at +0xA0 (`NFLStream`: buffer offset, VRAM address,
  state 0 to 7). The first six load the shared texture slots at +0x10 (`NFLSharedTex`): the eyes and mouths of the
  expressions (slots 0 to 4, eye 26, mouth 24, eye 48, mouths 10 and 12 by `sNFLSharedStreamSources`) and the mole's
  texture (5); the seventh the shared palette. Every frame `NFL_FillBuffer` copies resources into the buffer
  (`NFLi_CopySharedTextures`: six from the table `sNFLSharedStreamSources`, then `NFLi_CopySharedPltt`, the seventh,
  then the faces' streams, `NFLi_CopyFaceTextures` and `NFLi_CopyFacePltts`, eleven each at +0x1A4 and +0x228 of a face,
  with palettes made from the face's kept fields at +0x180); after the interrupt `NFL_LoadVram` loads them into texture
  and palette VRAM (`NFLi_LoadSharedTextures`, `NFLi_LoadSharedPltt`, `NFLi_LoadFaceTextures`, `NFLi_LoadFacePltts`)
  through the LCDC (`NFLi_CopyToTexBank`, `NFLi_CopyToTexPlttBank`, which check the bank's size first), up to the budget
  main passes (0x4000). A copy that wraps around the buffer is made in two parts, and one cut short by the budget is
  resumed the next time (states 3 to 5). Mode 7 (+0x68) converts 4-bit images through a palette
  (`NFLi_Convert4BitToDirect`), so its sizes are four times the others'.
- The coordinate helpers `NFLi_MapVecFx16` to `NFLi_DLNormal` map x, y and z to the axes of the drawing context
  (indices at +0xD0..+0xD2, `yAxis`, `zAxis`, `xAxis`; negations at +0xC4..+0xCC, `negate`) before `G3C_Translate`,
  `G3C_Scale`, `G3C_Vtx`, `G3C_Normal` and `MTX_RotAxis43`.
- Two disagreements in the original: `NFLi_LoadFacePltts` loads ten of the eleven palette streams that
  `NFLi_CopyFacePltts` fills, and the eleventh entry of the array at +0x228 lies over the pointer at +0x2A0 that
  `NFL_AddFace` links its list through.

The middle third, `0x0206ED94` to `0x02073514`, measured from its code (in `lib/NFL/NFL.c`):

- **The work area is 0xF4 bytes.** `main` calls `NFL_Init(work, resource, buffer, 0x4000, desc, 0x7F, 0)` with the work
  area at `data_02115480` and the buffer at `data_02115574`, right after it. The function copies the three words of
  `desc` (`data_020fc504`) to +0x0, +0x4 and +0x68 (`texAddr`, `plttAddr`, `texFmt`), keeps the buffer and its size at
  +0x78 and +0x7C, sets the state of each of the first five streams at +0xA0 to 0 or 7 by the bits 2, 4, 8, 0x20, 0x40
  of the flags (`sNFLSharedStreamFlags`), then the first's to 0 and the seventh's to 7 in mode 7 (else 0), and the
  texture base at +0x8 to `desc[0] + 0x200 + 0x800` per enabled stream (a quarter of that in mode 3, where the palette
  base at +0xC gets 0x20 more).
- **Its faces.** `NFL_AddFace` appends a face to the list at +0x6C, at most +0x70 of them (`faces`, `maxFaces`, which
  `NFL_AddFaceSlot` counts up and `NFL_RemoveFaceSlot` down); `NFL_RemoveFaceSlot` also takes the last one off that list
  and off the second list at +0x74 (`loadList`, linked through +0x17C of the faces, `nextLoad`). `NFLi_GetFace` returns
  the n-th, `NFL_ResetFace` sets its +0x170 (`state`) to 4 and `NFL_FindResetFace` finds the first that has 4 there. The
  character code calls `NFL_AddFaceSlot`, `NFL_AddFace` and `NFL_FindResetFace` in turn and keeps the index it gets,
  which it then passes to `NFL_MakeFace` and `NFL_SetExpression`.
- **Expressions.** `NFL_SetExpression(index, kind)` gives the n-th face one of seven expressions: kind 0 is
  `NFLi_ApplyExpression0`, 1 to 6 `NFLi_ApplyExpression1`, `NFLi_ApplyExpression2`, `NFLi_ApplyExpression3`,
  `NFLi_ApplyExpression4`, `NFLi_ApplyExpression5` and `NFLi_ApplyExpression6`, each only if its bit of +0x16C
  (`exprFlags`) is set (kinds 1 to 6: 0x2, 0x4, 0x8, 0x10, 0x20, 0x40); the kind is kept at +0x168
  (`NFL_GetExpression`). Each turns the eyes (the patches at 0x110 and 0x128 of the face, `eyePatch` and
  `eyeMirrorPatch`) and the eyebrows (0xE0, 0xF8, `browPatch` and `browMirrorPatch`) through `NFLi_PatchFacePart`, which
  rewrites the matrix, texture and palette commands that the display list was built with places for, and writes the
  eyebrows' height into the two translation commands at 0x158 and 0x160. The angles are `rotation + table[type] - 32` in
  32nds of a turn, with `sNFLEyeTypeRotations` for the eyes and `sNFLEyebrowTypeRotations` for the eyebrows, from the
  face's halfwords at 0x80 (`exprFields`: eyebrow Y, eye rotation, eye type, face type, eyebrow rotation, eyebrow type,
  as `NFLi_PrepareFacePartTextures` stores them); face types 2, 6 and 7 move the eyebrows by 0x1E60. The other
  expressions change the rotations by up to two steps (clamped to 0..7 and 0..11) and take some textures from the work
  area's six slots at +0x10: kinds 5, 6, 1, 2 and 3 take slots 0 to 4, which `NFLi_AllocExpression5Texture` and the four
  after it give VRAM, and kind 4 its own eye texture of type 0x31 (stream 5). The expressions keep their numbers in
  their names (`NFLi_ApplyExpression0` to `6`): nothing shows which face each is. `sNFLEyeTypeRotations` is also indexed
  by the eye rotation in three of them, so what it is per index is not settled.
- `NFL_MakeCreateId` makes a create ID: bytes 0 and 1 zero, byte 2 a random number below 4 times 4 with bits 16-17
  of the RTC time in seconds shifted down 5, bytes 3 and 4 that value's bits 8-15 and 0-7, bytes 5 to 7 the last
  three of the MAC address. `NFL_GetCharInfo` copies a valid Mii's names (as strings of 11 halfwords), create ID and
  fields into another record, with height and build capped at 127 and the skin colour of `NFLi_GetFaceColor`.
- **The create-ID bits.** `NFLi_SetUnassignedCreateId` gives the default and random Miis the ID `E0 00 00 ...`, and
  `MiiData_AssignCreateId` gives a Mii an ID only if bit 5 of its first byte is set (`NFL_IsMiiCreateIdUnassigned`), and
  the Mii transfer stops with an error on a record that has it (`func_02078db8`): hence "unassigned". Bits 7 and 6 keep
  their numbers in the names (`NFL_IsCreateIdBit7Clear`, `NFLi_GetCreateIdBit6`). `NFL_IsSameCreateIdSource` compares
  the low nibble of byte 2 and bytes 3 to 7, the six bytes that `MiiData_AssignCreateId` takes from its source.
- **The second display list.** `NFLi_MakeFacePartsDL` builds it from the seven 0x30-byte records of the context
  (0x78..0x1C8): `NFLi_MakeMouthMolePart` for records 4 and 5 (mouth and mustache), and 6 when the Mii has a mole;
  `NFLi_MakeEyeBrowPart` for 0 to 3 (the eyes and eyebrows, kind 4 the mirrored side). Those two (6 KB and 4 KB, both
  matching) place a part as a quad on the face: a 2x2 grid of points whose Z is taken from the
  planes at `0x020B676C`, normals interpolated bilinearly over the 3x3 grids of points (`0x020B6818`,
  `sNFLFaceGridPoints`) and normals (`0x020B67AC`, `sNFLFaceGridNormals`), their sum normalised and the quad rotated
  onto it (`MTX_RotAxis43` about the cross product with (0, 0, 1)); records 0 to 3 can share a state (`NFLMakeState`)
  whose +0x20 (`set`) says the first of a pair has set it.
- **`NFLi_MakeMouthMolePart` transforms its vertices on the CPU.** Where `NFLi_MakeEyeBrowPart` emits matrix
  commands (which the expressions patch), it builds the matrix itself (identity, translation, the rotation onto the
  normal, scale) and sends each corner through `MTX_MultVec43` and each normal through `MTX_MultVec33` (the
  rotation's 3x3 part) before `G3C_Vtx` and `G3C_Normal`, and clears the patch's matrix command, so the expressions
  rotate no mouth. It looks the face up at |x| (the face is symmetric) and mirrors the normals of points left of the
  centre; the mole (kind 0) takes the mean of its own Z and its four corners' Zs. The mouth (kind 1) is a quad strip
  of three columns whose texture runs from s = width at its right edge through the middle to 0, its side normals
  tilted by 0x197 in X; the mustache (kind 2) a strip whose texture runs 0, width, 0, so one half mirrors the other
  (its last normal is tilted in Y where the mouth's is in X); the mole a plain quad. The second of a pair (the
  mustache) is put at the first's depth only if its centre is within a third of their summed sizes of the first's,
  else at depth offset 0.

On the compiler: `NFLi_ApplyExpression0` and `NFLi_ApplyExpression6` match only under `#pragma optimization_level 2`
with `#pragma opt_common_subs off`, as `NFL_MakeFace` does, and with each sine and cosine assigned through an `(fx16)`
cast; without them the instructions are the same and the callee-saved registers are rotated. `NFLi_ApplyExpression2`
and `3` match with `#pragma opt_propagation off` added and the eyebrow's clamp written on a copy of the rotation (the
compares on the copy, the else adding `d` to the variable): the original adds `d` twice, which every level from 1 up
folds into one sum when the clamp reads and writes one variable. `NFLi_ApplyExpression5` matches with
`#pragma opt_dead_assignments off` instead of propagation off and its clamp in the same copy form: the original keeps
the adds, branch and compare of a clamp whose result nothing uses (the eyes take the Mii's own rotation), and that
pragma is the only setting found that keeps them with the level-2 scheduling (level 1 keeps them without it).
Inferred from these: the expressions were not compiled under one setting. `NFLi_ApplyExpression1` and `4` still differ
by the register of the clamped rotation (r3 where the original reuses the field's r4): no statement or declaration
order, single `opt_*` pragma, clamp form (direct, copy, inline helper) or sine/cosine form tried moves it.
`NFLi_MakeMouthMolePart` and `NFLi_MakeEyeBrowPart` need none of this: they match at the file's level.

Inferred: this is NFL, the face library that `NFL_Res.dat` is the resource of (the file name, the Mii-face drawing
and the create-ID ingredients). Not established: its files' bounds, which compiler built it (the Mii transfer after
it is 2.0 SP2p2, the game SP2p3), and Nintendo's names for its functions.

## The end of the main program's code

Measured: the main program's code from `0x020a5cc4` to its end at `0x020B04C8` is seven files of the game and, last,
the anti-piracy code. None of them has a static initialiser (the `.init` entries end with `KeyboardCore.cpp`'s), and
their data is the last of each section: `.rodata` from `0x020FBFFC`, `.data` from `0x02115140`, `.bss` from
`0x02149A20`.

| File | Code | What it is |
| --- | --- | --- |
| `src/shop/ShopStock.cpp` | `0x020A5CC4` | The shops' stock of the day |
| `src/island/Apartment.cpp` | `0x020A6074` | The apartment building's stage, rooms and windows |
| `src/island/Sky.cpp` | `0x020A612C` | The time of day and the upper screen's sky |
| `src/shop/Shop.cpp` | `0x020A80F0` | The scrolling item list, the shop's window, the shop staff |
| `src/resident/ResidentLife.cpp` | `0x020ACFE4` | What the residents do by themselves, and their encounters |
| `src/song/Song.cpp` | `0x020AF058` | The saved songs |
| `src/island/IslandProgress.cpp` | `0x020AF224` | What decides the island's next facility |
| `lib/DSProtect/DSProtect.c` | `0x020AF9F0` | DS Protect |

How the bounds were found: the apartment's `.rodata` must start at `0x020FBFFC` (a section of its own at
`0x020FC002` is aligned to 4 by dsd, which moves every symbol after it; and the three `u16` there are the stage
thresholds its code compares with); the sky's `.rodata` objects interleave the time of day's tables with the sky's
own, so the two are one file; the shop file's `.rodata` pool at `0x020FC27C` (see below) is read by the list, the
window and the staff code alike, and its `.data` holds a static of the counter's code before the four virtual
tables, then the list's and the staff's strings. The other boundaries are where the callers and the subject change:
the residents' functions, the songs' and the island's progress have no data of their own to tell them apart
(inferred).

### The item list, the price window and the shop counter

Measured, `src/shop/Shop.cpp` (95 of its 98 functions match; classes in `include/shop/Shop.h` and `ShopWindow.h`):

- `ItemListView` (0x13F64 bytes, table `0x021153B4`): the scrolling list of the lower screen that the shops, the
  change room (overlay 86), the album and the item trade (overlay 110) use. Up to seven rows (one more than shown),
  each a button with the item's name (`MessageText`), a price with an icon, and a count canvas; arrow buttons, a
  scroll bar and a touch area scroll it 4 pixels a frame or by dragging, and a row is 20 pixels. The rows are reused
  round (`GetRowItem`), and only rows whose item, palette, value or count changed are drawn again. `mMode` says what
  a row shows beside the name: nothing (0), the price, greyed when `*mMoney` is less (1), "ｘ" and the count (2), or
  `mUnknown0C` " / " the count (3), greyed in 2 and 3 when `mUnknown0C` is 0. `ShopItemListView` (table
  `0x02115398`) adds a mark beside each row whose item was never owned (`Item::mOwned`).
- `ShopWindow` (0xB6C8 bytes, table `0x02115368`) is a `MessageWindow` that asks how many to buy (message 0xC3) or
  sell (0xC7): its plus and minus buttons change `mCount` within the money, the 99 an item can be owned and, when
  selling, what is had; it shows the count, the total and the money. Totals are `GetBuyTotal`/`GetSellTotal`, its
  third and fourth virtual functions: the price (Item's first virtual) or the second price (its second) times the
  count; `HalfPriceShopWindow` (`0x02115380`, overlay 56) buys at half and sells at 11/10. MessageWindow's second
  virtual, `0x02034BC4`, is `SetPosition(WindowPoint)`: the window and what is on it move there.
- `ShopCounter` (0x18FBC bytes) holds the speech player, the speech window, the staff and an item panel, and sets up
  the 3D view (camera, two white lights, clear colour 0x3DEF). The panel (`ShopItemPanel`, 0xE44 bytes) shows an
  item's name and either the number owned ("所持数") or the price ("売値", "円"), slides in and out, and hides while
  the speech window is shown. Its kind gives the first name's message: 0 for goods, 0xFF for clothes, 0x22B for
  interiors (TmdItemMsgJP has the goods, then the 300 clothes, then the interiors).
- `ShopStaff` (0x321C bytes) is a resident whose event (0x14) takes place at the shop, if there is one
  (`FindResidentAt`), or else the head staff: a `Mannequin` with the head staff's head ("3CH:/head.imd") and the
  shop's uniform, clothes 0x87, 0x73, 0x97 or 0xC0 by shop (`GetUniform`). It greets (a bow, animation 0x22, 0x2A in
  shop 3, and a turn), says two messages (the speech window shows the first, the voice speaks the second, animation
  29) and turns back; `mHeldModel` is put in its right hand every frame.

The `.rodata` is two `u8[2]` and one pool of 4-aligned statics sorted by size: an int, 31 points, two `u16[5]`,
three 12-byte, five 16-byte (the last a local array of `SetKind`), an `int[5]`, two `ScreenPoint[3]`, two
`ButtonSounds` and a `ScreenPoint[5]`. Five of the points are only read folded, and a set of five tab buttons
(sequences 0x17 to 0x20 in pairs, kinds 1 to 16, positions 0x2E apart, size 42×24) is read by no linked code
(inferred: by a function the link stripped).

### The shops' stock

Measured: the saved game holds two records of 0x34 bytes (func_0202c864 reads one, func_0202c878 writes one): a
date, then five clothes, five goods of kind 1, five of kind 4, three interiors and one more good, which the item
files' pickers choose (gap66-b's ChooseClothes, ChooseItemsNot1000, ChooseItems, and 0x020A5A28 for the interiors;
on the very first day the first five goods are a fixed list). `UpdateShopStocks`, called only by the shop scenes
(overlays 56, 68, 83, 84 and 85), moves record 1 into record 0 and makes a new record 1 when the game's day has
changed, not when the clock went back one day (the turn of 2099 into 2000 included), and not while func_02035aa4
says so. **Inferred:** record 0 is today's stock and record 1 tomorrow's. A game day starts at 5 in the morning
(`GetGameDate`).

### The apartment building

Measured: the building has stage 1, 2 and 3 from 20, 45 and 70 residents (`GetApartmentStage`), 24, 48, 72 or 100
rooms by stage (`GetApartmentStageRooms`; gGameData's byte 0x4C is the saved stage), and its front has twelve floors
of eight windows and a thirteenth of four (`GetWindowCell` gives a room's column and floor). Overlay 72 uses them.

### The time of day and the sky

Measured, `DayPhase` (0x28 bytes): sunrise and sunset come from two tables by month (`GetSunrise`, `GetSunset`:
sunrise from 5:00 in June to 7:00 in December, sunset from 17:00 in December to 19:00 in June), in seconds since
midnight. The phase is worked out again every 0x1C42 frames (about two minutes): within 64 minutes of sunrise or
sunset it steps from 0 to 15 every 512 seconds, in between it is 15, with `mDaytime` telling day from night;
`GetPhase` gives 0 to 31, the night's 16 higher. `GetSkyLightColor` and `GetSkyLightColor2` turn it into the colours
of the 3D scenes' two lights (overlay 72). The seconds tables are function-local statics with dynamic initialisers,
filled on first use (their guards are the `.bss` at `0x02149A20`), which leaves them as zeros in `.data`.

`SkyPalette` (overlays 84, 85, 90, 103) is a DayPhase that loads one palette, `XXX:/Fw_BG_up_<when>.ncl`, where
`<when>` is `DAY_Chg`, `AM_Fine`, `AM_Cloud` by day and `NIGHT_Chg`, `PM_Fine`, `PM_Cloud` by night, chosen by the
flags at 0x24 (`Chg`; nothing in the main program sets it) and 0x20 (`Fine`; 1 from the start), at the phase's 32
bytes of the palette. `Sky` (0xEF8 bytes; overlays 20, 34, 57, 72, 79) loads the `Fw_Obj_up` and `Fw_BG_up`
palettes alike and moves its objects, each turned on by an argument of `Sky::Init`: eight clouds in eight rows,
drifting left at speeds and with gaps that the one `CloudSettings` entry (`0x020FC10C`) gives; a flock of one to four
birds (`SkyBirds`) every 1200 to 2400 frames, with sequence archive 3 entry 6 or 7 on handle 2 as its sound, which
fades as the flock crosses; an aeroplane every 18000 to 36000 frames (sometimes much sooner at the start), with entry
8 on handle 3; and the moon, whose picture is its age (`((year + 10) % 19 * 11 + month + day) % 30`, two more in
January and February) and whose x follows the time of day.

### What decides the island's next facility

Measured, `IslandProgress` (0x2C bytes): `Count` counts the saved characters by sex (func_0202c604 is the
character's `MiiData_GetSex`), those with a full date of birth, the residents in all, the items of kinds 4 to 6 the
player owns, the money (gGameData's capped counter, which this settles as the money), the used entries of two lists
of ten in the saved game and the troubles solved. `GetNewFacility` goes through the island's state bytes
(func_02039c00) in a fixed order: one in state 3 opens at once, one in state 0 when its counts are reached (for
facility 2 two men, two women and 15 troubles solved; for 11 20000 in money; for 20 to 24 100, 200, 400, 700 and
1000 troubles; for 25 to 28 10, 30, 50 and 100 residents), and the apartment building (7) when the residents call for
a stage above the saved one. Overlay 72's map asks the four `Has*` tests before a building can be entered.

### The saved songs

Measured: `SongEntry` is a song of 0x40 bytes as the saved game keeps it, with the number it is kept under;
func_0202c934 stores one (`Save`), func_0202c920 copies one out (`Load`). Its 21 notes at 0x16 are 16 bits each, a
kind in the low three bits and a 9-bit value; `SetNotesFromSlot` (overlay 109, for songs received over the air)
replaces the values of the notes of kinds 1 to 4 from the table of the song's slot (`0x020F9414`, 0xE0 bytes a
slot) through `data_020f9408`.

### The residents' life

Measured: overlay 106's `PreStartScene::UpdateResidents` calls, in this order, `ShiftEventTimes` (the change of the
clock's offset, func_02035ae0, brought into range by steps of 100 years of 365.25 days, moves every event end and
outing; on another console they are reset), `LimitEventTimes`, `EncounterJudge::MeetWhileAway` (unless the clock says
otherwise), `UpdateResidents`, `StartNextEvents` (which first calls `StartRandomEvents`), `PickItemsWhileAway`,
`ChangeClothesWhileAway`, `ClearUnknown24E` and `SetRandomUnknown24E`; overlays 72 and 74 call the middle ones when
they load. While the game was off, two residents meet every `3000 / pairs` minutes (at most 500 meetings); each
meeting changes both affinities by a random entry of the relation's row of `data_021135a0`, doubled, and
`EncounterJudge::Judge` (when both like each other, affinity 200) or `JudgeEnd` (when neither does) may change their
relation, by their relation and by how many of the others are in which relation to them (`CountAcquaintances`).
`ChangeClothes` gives a resident one of the clothes it has seen, three times as likely when it likes them most
(func_0209c89c at least 13), twice at 10 and once at 6. **Inferred:** the result codes of Judge (1, 2, 4, 9) and the
relations GetRelationAfter maps them to are steps in friendship and love; what each number means is not established.

### DS Protect

Measured: the last 2,776 bytes of code, `0x020AF9F0` to `0x020B04C8`, hold seventeen functions of which seven contain
ranges that the ROM keeps encrypted (dsd's `dsprot_range`; it decrypts them on extraction and encrypts them again
when building, `enable_dsprot: true`). The ten before them are the decryption: `DSProtect_DecryptRange` takes the
word before a range as its marker, makes a 16-byte key of it, finds the range's end by the same word and decrypts the
range in place (`DSProtect_DecryptWithKey`); `DSProtect_EncryptRange` does the reverse from the end. The cipher is
RC4 (`DSProtect_RC4Init` is the key schedule over 256 bytes, walked from the end, `DSProtect_RC4NextByte` the
generator) applied to each instruction's first two bytes, while the third goes through a table that flips its low bit
and the fourth (the condition and the instruction class) is kept (`DSProtect_CryptInstructions`);
`DSProtect_SyncCaches` writes back the data cache and disables or enables the instruction cache around it. They call
`OS_GetMacAddress`, `OS_GetOwnerInfo`, `CARD_LockRom` and `CARD_UnlockRom`, and their 8 bytes of `.rodata` are the
last of the section. **Inferred:** this is the DS Protect library the game is known to carry, linked after the game's
files; it is cut as `lib/DSProtect/DSProtect.c`, one unit, as no file names of it are known.

The seven, read decrypted (measured):

| Address | Name | What it does |
| --- | --- | --- |
| `0x020AFDD8` | `DSProtect_CheckMacAddress` | TRUE when the MAC address is 00:09:BF:00:00:31 (`sMacAddress`, stored inverted) with the owner's birthday January 1 and an empty nickname, or all zeros |
| `0x020AFEF8` | `DSProtect_ReadRom` | Reads the card's ROM with command 0xB7 through the card's registers, 512 bytes at a time |
| `0x020B009C` | `DSProtect_CalcChecksum` | A CRC-32 (0xEDB88320) with the test of the low bit inverted |
| `0x020B0140` | `DSProtect_CheckRom` | Checksums six blocks (0, 0x200, 0x400, 0x8000, 0x8200, 0x8400); TRUE when one of the first three differs from 0x8000's, or the last three are equal |
| `0x020B0250` | `DSProtect_RunRomCheck` | Runs `DSProtect_CheckRom` (from a list of addresses plus 800), and calls its argument when it returns TRUE |
| `0x020B0328` | `DSProtect_RunMacCheck` | The same with `DSProtect_CheckMacAddress` |
| `0x020B0400` | `DSProtect_RunNoCheck` | The same with an empty list: always FALSE |

`main` calls the three runners (`0x02000DB8`, `0x02000E6C`, `0x02000F5C`). Inferred: `DSProtect_CheckRom` tells a
copy from a genuine card, which reads its first 32 KB as copies of what follows; `DSProtect_CheckMacAddress` an
emulator's defaults.

**How the build handles the encryption** (measured): dsd decrypts the ranges when it extracts the ROM, so the
delinked objects, and what objdiff compares a source with, hold plain code; after linking it encrypts the ranges of
the symbols that have `dsprot_range` in `symbols.txt` again (the build's "Encrypting DS Protect in ARM9 program"). So
an encrypted function is written as it runs. Its ranges are inline assembly in C, the library's own way (the
compiler's quirks around `asm` blocks, a constant loaded from the pool for a variable live across one, show the
original was so too): `DSPROTECT_BEGIN(marker)` pushes r0-r3, calls `DSProtect_DecryptRange` on the address after
the marker word and branches over it; `DSPROTECT_END(marker)` branches over the second copy and calls
`DSProtect_EncryptRange`. Linked from source (the unit marked complete as a test), the plain ARM9 program was the
original's word for word over `DSProtect_CheckMacAddress`'s two ranges.

What is left (measured): 15 of the 17 functions compile to the original's bytes; `DSProtect_RC4Init` differs in
registers and `DSProtect_CalcChecksum` keeps its polynomial in a register where the original XORs four immediates.
objdiff scores the six matched encrypted functions at 96.7 to 98.9%: each marker word is `dcd` in the source, data
with a `$d` mapping symbol, where the delinked object, having no mapping for it, shows a `bl`; the bytes are equal.

## The characters

The main program's character code, `0x0208EB18` to `0x0209E140`, which every scene with a resident uses: four units
under `src/character/`, with the classes in `include/character/Character.h`, `CharacterModel.h` and `FloorPos.h`.
The class names are the project's own; the field names in quotes below are the game's.

| Unit | Code | What |
| --- | --- | --- |
| `CharacterModel.cpp` | `0x0208EB18` to `0x0208FCF8`, 30 functions, all matching | The body: `CharacterModel` |
| `Character.cpp` | to `0x02092178`, 73 functions, 69 matching | `CharacterBase` (the Mii face), `CharacterHead`, `Character` |
| `CharacterSort.cpp` | to `0x020979A0`, 23 functions, all matching, complete | The resident list's orders: name normalization, collation, comparators |
| `SavedCharacter.cpp` | to `0x0209E140`, 183 functions, 178 matching (Validate's string labels aside) | `SavedCharacter`: the record, its parameters, relations, long activities, troubles, tastes, reactions, birthday and biorhythm |

Measured:

- **The bounds.** `.rodata` `0x020FB768` to `0x020FB864` is one size-sorted list (a 12-byte vector, then twelve 20-byte
  joint names) read only by the model's code; the 12-byte object at `0x020FB864` (read at `0x0209005C`) and the two
  40-byte animation tables after it, which only overlays read, are the next file's. `0x020FB8C0` to `0x020FBCDC` is
  one list read from `0x02098EF0` to `0x0209E00C`; `0x020FBCDC` begins the item file, whose `ClothesItem` constructor
  is at `0x0209E140`. The functions `0x02092178` to `0x020979A0` call only each other and are called only by the
  resident list (`0x020A1xxx`) and overlays. The strings `0x02114DA0` to `0x02114E3C` are the model's,
  `0x02114E3C` to `0x02114F78` the saved character's. What lies between SHTTS and the model in `.rodata`
  (`0x020F3FF0` to `0x020FB768`: the clothes table, the goods, room and trouble tables) and in `.data`
  (`0x021135A0` to `0x02114DA0`) has no code: data-only files, not cut.
- **`CharacterModel` (0x508 bytes)** holds an `AnimModel` as a member and has no virtual table of its own. It is the
  second base of the 0x2B18-byte `Character` (`class Character : public CharacterBase, public CharacterModel`): the
  scenes' constructors call `0x0208EB18`, the first of the two identical constructors the compiler emits, which is the
  base constructor C2, while a model that is a member (the G3D test scene's) is built with `0x0208EBA0`, C1. It keeps
  the matrices of ten joints, found by name with `NNS_G3D_GET_JNTID` in its render callbacks (neck, `L_hand1`,
  `R_hand1`, body; with `mTrackLimbs` the elbows, knees and ankles), scales the body from the Mii's height and build,
  turns joints by angles, and shows or places the shadow. It wears a `ClothesItem` (+0x4B0): the archive is
  `Mdl/Body/body<model name>[F]_3D_LZ.bin`, `F` for a female Mii when the clothes table has a female body, and the
  texture `body_c<colour>.ntfp`.
- **`CharacterBase` (0x25E4)** is `SavedCharacter` with its Mii face: an `NFLFace` of the Mii library (+0x2DC), its
  draw parameters (+0x2C0, NFL's `NFLDrawParam`), the head joint's matrices that the body leaves it, and the face's
  moods: `UpdateFace` runs one of 26 kinds (show an expression once, blink, wink, alternate expressions on timers).
  `LipSync` opens the mouth while the phoneme is a vowel or `w`.
- **`CharacterHead` (0x2728)** is a `CharacterBase` with an `AnimModel` head instead of a body: overlays 2, 77 and 79.
- **`Character`** adds walking (`WalkToTarget`, `TurnTo...`, `MoveAlongAngle`; its speeds scale with the Mii's height
  and the animation speed), the point tests (`IsBeside`, `IsInFront`, `IsAhead`, `IsPointWithinReach`), and sounds at
  quarters of an animation. `SetWalkTargetAwayFrom` is NitroSDK's `VEC_Normalize` written out in two dimensions.
- **The record's fields** are named by `SavedCharacter::Validate`, which checks each field and passes its name to the
  clamping template: "character" (the personality answers, +0x4A), "voice", "motto1" and "motto" (the texts),
  "RelationMeter" (+0xFC, 0 to 200), "RelationRank" (+0x160, 0 to 10), "LongActState" (+0x224, 0 to 0x2E),
  "TroubleState" (+0x23A, 0 to 5), "TroubleID", "TroubleCategory", "SatisfyMeter", "SatisfyLevel", "EatFullness",
  "EatLastFood", "MeetYear"/"Month"/"Day", "PatYear"/"Month"/"Day", "TouchTalkMonth"/"Day"/"Hour", "RelationBlood".
- **The Mii's parameters** (`GetParameter`, `SetParameter`) are numbered as the Mii data's getter and setter tables
  (`MiiParameter`); for a Mii made on another console the birth month and day are the record's own.
- **The long activities** (`mLongActState`): odd states 0x17 to 0x2D are done with a partner, the even ones after them
  are the partner's side, 0x13 to 0x16 are away; `ChooseLongAct` draws one by weights for the personality's kind and
  the mood (`data_020F8CC0`: minutes, weight, personality bits, mood bits).
- **The relations** (`mRelationRank`, by the other's index): 0 none, 1 friends, 2 lovers, 3 former lovers, 4 spouses,
  5 divorced, 6 to 8 friends, lovers and spouses fighting, 9 best friends. The order is the game's list of relation
  names (`TmdEtcMsgJP`), and the code agrees: a wish to break up looks for a lover liked no more than 50 or one it is
  fighting with, a divorce for a spouse, a fight that nobody settles turns friends into strangers and lovers and
  spouses into ones who want to leave each other.
- **The troubles** (`data_02114D78`: ten tables by category, 12 bytes a trouble: kind, length in minutes, weight;
  a mood, the minutes of cooldown, whether it has a target; flags). The kinds the code tells apart: 1 wants food
  (the flags are kinds of food), 3 wants clothes (kinds of clothes: bit 0 warm, bit 1 cool, which are not asked for
  out of season), 4 wants to make a friend, 5 is in love, 7 and 8 have fought (8 angry), 9 and 10 want to break up and
  divorce, 11 is heartbroken (category 4, with a progress meter), 19 wants an interior. `StartTrouble` chooses the
  target (`mTroubleTarget`: a character's index, or a food or clothes); `ExpireTrouble` runs when its time is over:
  an open one fails (`FailTrouble`: the character eats something by itself, the break-up or divorce happens and
  leaves the other heartbroken, ...), a finished one is cleared.
- **Tastes**: goods, clothes and interiors have a taste (0 to 15) for each of four kinds of character; the
  character's liking is that, give or take three by its create ID, and less for things of the other sex.
- **Reactions** (overlay 74): `ChooseTalkReaction` draws one of 12 reactions to a partner in a talk by the relation and
  the partner's last reaction, `ChooseNextReaction` one of 15 by the last one, both weighted by personality, mood and
  the personality answers.
- **The calendar**: `GetBirthday` makes a date of the 20th century from the create ID when there is no year of birth;
  `GetBiorhythm` adds the physical (23 days), intellectual (33) and emotional (28) cycles at the age in days;
  `GetBirthDayCycle` is the day of the 60-day calendar cycle the character was born on (1900-01-01 counted as day 10),
  times two, plus the sex.
- **The file's `.rodata`** (`0x020FB8C0` to `0x020FBCDC`) comes out of the source at the ROM's addresses: a pool of the
  4- and 2-aligned tables under 64 bytes in size order, then the byte array and the larger tables each on their own.
  The five answer-score tables are one `s8[5][9]` (separate `s8` arrays are never pooled and would come before the
  16-byte tables), and the intellectual cycle has a 34th entry, which puts the voice presets on a 4-byte boundary.
- **The collation**: `CharacterSort::GetCollationKey`, a switch of 522 characters (punctuation, Latin, Greek, kana,
  ...) to ranks, times ten plus five for a full-width letter; names are compared after normalizing in up to five
  rounds of decreasing strength.

Inferred:

- The `func_0209E0E8`/`func_0209E114` pair at the end of `SavedCharacter.cpp` is one template instantiated for `s16`
  and `u8`: out-of-line template code is emitted after the file's last function.

## Compiler

**The game's code: mwccarm 2.0 SP2 patch 3 ("Version 3.0 build 138"), or patch 4 (build 139).** Measured:

- **What tells the builds apart is where a file's statics go.** Every 2.0 release up to SP2 patch 2 (build 137) puts
  all of a file's 4-aligned statics into one pool, read at the pool's address plus an offset. The ROM keeps those of
  64 bytes and more out of the pool, each in a section of its own: overlay 72's tables of 88 to 320 bytes follow its
  pool one by one, overlay 98's six button and sprite tables come after its last string, and WH's 64-byte parent
  parameters are addressed on their own while its 32-byte scan parameters are pooled. Of the 3.0 builds (114 to 139,
  compiled with test files under the game's flags), the threshold first appears in 138; no flag of the earlier ones
  (`-[no]constpool`, `-sdatathreshold`, other `-O` and `-opt` settings, without `-ipa file`) gives it. The DSi
  compilers (4.0, builds 1018 to 1051) have it too, but compile the game's code differently: `TtsPlayer` drops from
  28 matching functions to 10, overlay 74 from 247 to 107, WH from 66 to 38.
- **Nothing else changes.** Every game source of `src/`, and the speech engine's four, compiles to byte-identical code
  with builds 137, 138 and 139, and with 138 as the game's compiler every complete unit links and the ROM is
  identical; the per-function report of the whole program is the same with 137 and 138 (1,959 of 7,357 matching).
  So the evidence for the version is the data, and the matched functions are no evidence against it. That was
  measured on the files of `src/` as they were at the switch; the Mii transfer, below, is code the same build does
  not give.
- **Overlay 72's `.rodata` comes out byte for byte with 138** (0x89C bytes: the pool of 0x3B0, then the 13-, 21- and
  22-byte arrays and the seven tables of 88 to 320 bytes, each in a section of its own), and its functions compile
  as with 137. Of the DSi compilers, 1.1 to 1.3p1 pool objects of up to 60 bytes in C++ (and everything in C);
  1.6 SP1 and SP2 also leave a 4-byte object out of the pool.
- **138 and 139 are not told apart.** They give identical output on every game file and on WH, and on all 163 C
  files of NitroSDK and NitroSystem that the build compiles, each with its own flags. 138 is used, as the first
  build that fits; 139 is the last 3.0 build there is.
- **WH is compiled like the game's C++**: `-ipa file` (its pooled statics need it), and as C++ (the order of its
  data needs it), see "WH". The speech engine, C, matches only without `-ipa file`: it was built apart.
- **The Mii transfer (`lib/MiiTransfer`) is built with an earlier build than the game's.** Its files read statics of
  64 bytes and more at the pool's address plus an offset: `WH_SetGgid` stores at +0x88 (the GGID of the 0x40 bytes of
  parent parameters at +0x80, whose first two members `WH_Initialize` clears at +0x80 and +0x84), and the protocol
  reads the valid flags of its three 0x60-byte packets at +0x1B0, +0x210 and +0x270. With build 137 (2.0 SP2 patch 2)
  `MiiTransferWH.cpp` matches 45 of 45 and `MiiTransfer.cpp` 52 of 54 (53 since, with a pragma); with 138 they drop to 40 and 43, the functions growing
  by the loads of the statics' own addresses. The main WH (`lib/WH`), the same source, has its parent parameters on their own, as 138 does. So any build up to 137 fits, and patch 2 is the latest that does, not
  proven to be the one. `-ipa file` is part of it: without it (C++ alone) statics are not pooled at all and they drop
  to 40 and 9.
- **The linker** stays patch 2's, with which the ROM links identical; `configure.py` names it on its own
  (`MWLD_VERSION`).

The earlier evidence still holds: NitroSDK 4.2 is from 2008 and the game came out in June 2009, and
`src/voice/TtsPlayer.cpp`, 28 functions of C++ that set up streams and threads and pass up to ten arguments, matched
in full with build 137 and does with 138.

The libraries are another matter, and settled: NitroSDK is built with 2.0 SP1 patch 2 and NitroSystem with 2.0 SP2,
as in Pokémon Platinum, and 1,148 of their functions come out identical to the game's. That says what Nintendo built
its libraries with, not what the game's own code was built with.

The flags in `configure.py` are the usual ones of games of this SDK, adjusted to what is measured above. Each is a
guess until a function that depends on it matches. One that is now measured: `-str noreuse`. The speech engine's
command function has "Pitch" three times, and the original holds three separate strings for them.

## Linking

The original was linked with dead-stripping, as a game built on libraries is: an object of the SDK holds functions
that the game never calls, and of `os_cache.c`'s 25 only ten are in the ROM. The build links with `-dead` too.

On its own that strips code that is in the ROM, because `relocs.txt` does not know every reference to it (a pointer
in a table that the analysis took for a number): the first try lost code from the main program and from a number of
overlays. So the linker script names every symbol of the delinked objects in a `FORCE_ACTIVE` list, which keeps the
original whole, and stripping only acts on compiled objects. With that, the ROM matches, and a library file linked
from its compiled object comes out with exactly the functions the game has.
