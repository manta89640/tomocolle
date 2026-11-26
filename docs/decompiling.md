# Decompiling

## How the build knows what is decompiled

[dsd](https://github.com/AetiasHax/ds-decomp) cuts each module of the ROM into object files, as its `delinks.txt`
says. A module is the main program, an autoload or an overlay, and its config is under `config/jpn_rev1/arm9/`:

| File | What it holds |
| --- | --- |
| `delinks.txt` | The module's sections, then each source file and the address ranges that are its own |
| `symbols.txt` | The name, kind and address of every function and variable |
| `relocs.txt` | Every place that holds an address, and what it points at |

What no source file claims is delinked as the module's "gap", and linked back as it was. So the ROM always builds,
however little is decompiled, and a source file takes over a range of addresses by claiming it.

## Adding a source file

`src/voice/TtsPlayer.cpp` is the worked example of a finished file: a class, with its functions under their
mangled names in `symbols.txt`, its static data placed, and marked complete. Overlay 0 is one that is still in
progress: `src/overlays/ov000/ov000.cpp`, claimed at the end of `config/jpn_rev1/arm9/overlays/ov000/delinks.txt`.

1. Read the code. `ninja dis` writes a listing of every module to `build/jpn_rev1/asm/`.
2. Find where the file begins and ends. A file's functions are contiguous in `.text`, and its data is contiguous in
   each of the other sections, in the same order as the files around it.
3. Claim the ranges in the module's `delinks.txt`:
   ```
   src/overlays/ov000/ov000.cpp:
       .text       start:0x02149a40 end:0x02149c50
       .data       start:0x02149c60 end:0x02149ca0
   ```
4. Write the file, run `python3 configure.py` so that the build knows of it, and `ninja`.
5. Open the repository in [objdiff](https://github.com/encounter/objdiff) and pick the file. objdiff rebuilds it as
   you save, and shows each function beside the original.

A function of the original and yours are compared when they have the same name. A function that is not understood yet
keeps the name dsd gave it, such as `func_ov000_02149c10`, declared `extern "C"` so that the compiler leaves the name
alone. Once it is understood, give it its real name in the source, and the mangled form of that name in
`symbols.txt`.

## Finishing a file

When every function and all the data of a file match, mark it `complete` in `delinks.txt`:

```
src/overlays/ov000/ov000.cpp:
    complete
    .text       start:0x02149a40 end:0x02149c50
    .data       start:0x02149c60 end:0x02149ca0
```

From then on the ROM is linked from your object instead of the original's bytes, and `ninja` fails if the module no
longer matches. Until then the file is compiled and compared, and does not go into the ROM.

## Naming symbols as you go

`symbols.txt` is the project's record of what each address is, and the name is the documentation. So a name goes in
the moment the code shows what a thing is, whether or not the function is decompiled, and never waits for the file to
be finished:

- Rename the symbol in the module's `symbols.txt`, and nothing else: relocations refer to addresses, not names.
- For a library function, use the library's own name (`DC_FlushRange`). For C++ game code, the name in `symbols.txt` is
  the mangled one, since that is what the compiler emits.
- Say in the commit message what the evidence was. "Named the cache functions of os_cache.c: each is identified by
  the CP15 operation it performs" lets the next person trust the name, or check it.
- What is not understood keeps the name dsd gave it: `func_<address>`, `data_<address>`, and `unk_<offset>` for a
  field. A guessed name reads as knowledge, and is worse than none.
- `dsd sig apply --all` names the functions dsd has signatures for. Run it again when dsd is updated.

### Strings, in bulk

`tools/strings.py` writes a table of every string the game's code refers to, `build/jpn_rev1/strings.tsv` (game data,
so it is generated and not committed), and `tools/name_strings.py --apply` names each of them in `symbols.txt` from
its text:

- `gStr_M00_sky_imd` for `"M00:/sky.imd"`
- `gText_Watashi` for `ワタシ`, and `gText_NenreiFushou` for `年齢不詳`: Japanese text is named by how it is read. The
  readings of kanji come from the system's Japanese tokenizer (`tools/romaji.swift`), so this step runs on macOS.
- A sign with no reading is named by what it is: `gText_Question` for `？`.

No name is an address. Equal strings in several places are told apart by their overlay and then by number
(`gStr_appear_ov075`, `gStr_appear_ov075_2`). An entry whose kanji the tokenizer cannot read is taken to be data
that decodes as text, not a string, and is left unnamed. The tool also leaves alone what already has a name, the
strings a source file owns, and any symbol the sources still refer to by its placeholder name. Run both again after
dsd's analysis finds more references.

### Library code, in bulk

Library functions are not named one at a time. `tools/match_objects.py` finds them by comparing the game with compiled
objects of the library, which a built checkout of another decompilation provides, and both names them and cuts their
files out as units:

```sh
P=~/pokeplatinum
python3 tools/match_objects.py \
    NitroSDK=$P/build/subprojects/NitroSDK-4.2.30001/libraries \
    NitroSystem=$P/build/subprojects/NitroSystem-071126.1/libraries \
    MSL_C=$P/build/MSL_C_NITRO_Ai_LE.a \
    --sources NitroSDK=$P/subprojects/NitroSDK-4.2.30001/libraries \
    --sources NitroSystem=$P/subprojects/NitroSystem-071126.1/libraries
```

Without `--apply` it reports what it would do. It only reads those objects: nothing of them is copied here but the
names. Any library that exists as objects with symbols can be matched the same way. An archive's members are named
after their objects, not their sources, so their files are only cut with `--member-sources NAME`, for an archive whose
members are named after C sources of the same name, as MSL's are (`alloc.o` is `alloc.c`):

```sh
python3 tools/match_objects.py MSL_C=$P/build/MSL_C_NITRO_Ai_LE.a --member-sources MSL_C --apply --units
```

## Subdividing units

A module starts as one unit, its gap. Cut a file out of it as soon as the file's bounds are known, before any of its
code is written: add it to `delinks.txt` under the path its source will have. dsd then delinks it as a unit of its own,
and the gap shrinks to what is still unassigned.

```
lib/NitroSDK/os/os_cache.c:
    .text       start:0x02007ff4 end:0x02008114
```

That unit appears on the progress map under its own name and counts toward its category, so the map shows how much
of the game is understood as well as how much is matched. Which directory a file goes in says what it is:

| Directory | For | Category |
| --- | --- | --- |
| `src/` | The game's own code, by overlay or by subsystem | Game, and the subsystem's (`sound`, `text`, `voice`) |
| `lib/<library>/` | Code of the SDK and of middleware, under the library's own file names where known | Libraries, and the library's (`nitrosdk`, `nitrosystem`, `shtts`, `msl`) |

Cut where the evidence is: a library's known file, a run of functions that only refer to each other and to their own
data, a static initializer (each belongs to one file), a table of virtual functions. Do not cut on a guess: a wrong
boundary makes two files that can never match, and costs more than a large gap does.

## Library sources

The SDK's files are not written here but fetched (see the README), and take part in the build like any other: a
`delinks.txt` names the file by its path under `lib/NitroSDK/` or `lib/NitroSystem/`, and the build compiles it with
its library's compiler and flags, from the `LIBRARIES` table in `configure.py`.

`tools/complete_units.py` then marks the units `complete` whose every function matches, a batch at a time, and keeps
each only if the ROM still matches when it is linked from the compiled object. It leaves a unit alone whose object
has data that its entry in `delinks.txt` does not place yet.

`tools/place_data.py` works out where a file's data is from the addresses in its matched functions, and with
`--unit <source> --complete --apply` places one file's data, names its objects and marks it complete; the build then
decides (see its header). One unit at a time, so, 38 library files were completed in round 5; what kept the others
back is in binary.md, "Libraries".

A library function that objdiff scores 0% is often already right and only has dsd's name: objdiff pairs functions by
name, and dsd gives a file's statics address names. Before changing anything, compare the compiled object with
`build/<version>/delinks/<unit>.o` in object order, by size and by the bytes outside relocations; a run of equal
functions between two named ones takes the compiled object's names (104 functions of the SDK and NitroSystem were
named so). The same comparison finds a range that ends a function or two early (`cgtool/3dsmax.c`), and a relocation
dsd got wrong shows as a near-miss: a constant it took for an address, or a call it missed after a `bx`. Then:

- a static is `local` in `symbols.txt`, which a complete unit needs; one that a table in the gap still refers to has to
  stay global until that table's data is placed with its unit (dsd refuses a local symbol referred to from another
  unit);
- a structure that dsd cut into its fields (it puts a symbol on every address the code loads) is folded back: one
  symbol, and every reference in every `relocs.txt` as `to:<structure> add:<offset>` (negative addends are accepted,
  `add:-0x4`, for `table - 1`);
- `tools/name_unit_data.py --apply` writes `kind:bss` for `.bss` objects (it wrote `kind:data(any)` before, which
  makes `dsd dis` panic) and can leave a symbol twice: check `symbols.txt` for duplicate addresses afterwards.

### Where the game's release of a file differs (awaiting the user's review)

The fetched sources are another release than the game's (4.2.30007), and a few files differ in code, not only in
names. Such a file is not copied here, corrected or patched: that would put the SDK's own text in this repository.
Instead `lib/overrides/` holds, at the fetched file's path (`lib/overrides/NitroSDK/libraries/spi/src/pm.c` for
`lib/NitroSDK/libraries/spi/src/pm.c`), only the project's own code for what differs, written from the game's
disassembly, and directives that say where it goes:

```c
/* override: replace PM_Init */      the block takes the place of the fetched definition of PM_Init
/* override: remove PM_GoSleepMode */ the definition (and any declaration) goes
/* override: after MI_StopDma */     the block goes after the definition; also "before NAME", and "end"
```

`tools/override_functions.py` writes the fetched file with those changes into `build/<version>/override/`, and
`configure.py` compiles that in place of the fetched file, with the same compiler, flags and include directories, to
the same object; the unit and its name are unchanged. The tracked file never holds lines of the fetched one. This is
the coordinator's design of round 3, kept for the user to approve or replace. It is used by `mi/src/mi_dma.c` (one
function the fetched release lacks) and `spi/src/pm.c` (a later release, see binary.md, "Libraries"); no later commit
of the fetched repository has either.

Before writing an override, look for the game's release among later commits of the fetched repository: its history
is small (`git clone --filter=blob:none` of it, then `git log -- <path>`). Where one has the file as the game has it,
nothing is written here: `FETCHED_FILES` in `configure.py` names the file, the commit and the file's SHA-256, and
`configure.py` downloads that one file at that commit into `lib/fetched/<repository>/<commit>/` (ignored), checks the
hash, and compiles it in place of the repository's file, with the same compiler, flags and include directories (the
repository's directory of the file is searched first, as for an override, and an override of the file applies to the
fetched one). The repository's own pin stays: a later commit changes many other files, so moving it is a decision for
all of them. `math/src/qsort.c` is built so, from `ccbbcc8`, the commit that writes `MATH_QSort`'s conditional
instructions out as branches under `SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS` (binary.md, "Libraries"). So is
`card/src/card_spi.c`, from the same commit, compiled with `SDK_PATCH3`, the define under which that commit adds the
game's 128 KB EEPROM: an entry of `FETCHED_FILES` may name defines for its file alone.

The fetched file's other definitions stay. A static that only a removed or replaced function used is no longer
emitted, which matters: with `-ipa file` the statics are pooled, so the file's pool is only right once every function
that uses one of them is the game's. Remove what the game's release does not have.

A file whose code is partly in the ITCM (`os_irqHandler.c`, `mi_dma.c`) is one unit: its entry in the main program's
`delinks.txt` names an `.itcm` range beside its `.text`, and dsd's section migration takes that range out of the ITCM
module (`.dtcm` works the same way for the DTCM's `.bss`). The SDK's own linker script defined symbols that its files refer to and dsd's script does not
(`SDK_AUTOLOAD_DTCM_START`, `SDK_IRQ_STACKSIZE`, `SDK_OVERLAY_DIGEST` and others): `LINKER_SYMBOLS` in `configure.py`
has them with the original's values, and `tools/force_active.py` writes them at the end of the script, with
`AutoloadCallback`, which `dsd rom config` looks up, as an alias of crt0's `_start_AutoloadDoneCallback`.
`tools/check_relocs.py` takes a reference to such a symbol for the original's reference to the symbol at its address.
`dsd json delinks` and `dsd objdiff` do not migrate and so
still list the ITCM's old gap; `tools/migrated_gaps.py` finds it, and `configure.py` and `tools/objdiff_config.py`
leave it out.

Linking strips what nothing refers to, as the original link did: a compiled file of a library holds many functions
the game never calls. The objects that dsd delinks are exempt, since only as much of them is known to be referred to
as `relocs.txt` knows: `tools/force_active.py` lists every symbol of theirs in the linker script as one to keep.

## Conventions

- **Source files are UTF-8.** The compiler receives them as Shift JIS, so Japanese text in a string literal is written
  as it is, not as escapes.
- **The game is C++** (see [binary.md](binary.md)), so the aim for game code is the classes it was written as. Plain
  functions that take the object, as in overlay 0 today, are a first step for code whose class is not recovered yet.
- **Never commit anything taken from the ROM**: no assembly, no data dumps, no assets. The configs under `config/`
  hold addresses and names only.

## Where to start

The order that the code itself suggests:

1. **Finish mapping the libraries in the main program.** NitroSDK, NitroSystem and part of MSL are named and cut into
   units under `lib/` (see [binary.md](binary.md)). Still to find: the rest of MSL. WH, the text-to-speech
   middleware, the Mii transfer and the Mii data are found and decompiled; NFL is cut as one unit (`lib/NFL`) but not
   yet divided into its files. Every library function named makes the game code that calls it readable, and what is
   left when the libraries are out is the game.
2. **Recover the classes that every scene is built from.** Each overlay holds classes that derive from ones in the
   main program, so little of an overlay can be finished before its base classes are.
3. **Then the overlays**, smallest first: each is a self-contained piece of the game and a short path to a complete
   file.

## What the game's C++ taught us

- **`-ipa file`.** The game's C++ files were compiled as a whole. Without the flag every object carries the static
  counter of the SDK's inline `GX_SetVCount` in its `.bss`, and the ROM has no room for it. The speech engine (C)
  matches only without it.
- **Vectors are classes with constructors, passed by value.** A setter such as `SetPosition(FxVec2 position)`
  called as `SetPosition(FxVec2(x, y))` leaves two dead copies on the caller's stack; that is the natural source of
  them, not hand-written copies.
- **A file's `.bss` order tells its bounds.** `gFonts` precedes `gTextObjResource` in the ROM, which one file
  defining both does not produce, so the fonts are a file of their own.
- **Static initialisers** are `__sinit_<file>.cpp` in `.init`, with a pointer in `.ctor`; both are local symbols and
  both sections have to be in the unit's `delinks.txt` entry before it can be complete. An array of objects is
  constructed through `__cxa_vec_ctor`, a single one registered with `__register_global_object`. An array of ONE is
  compiled as a single object (by 2.0 SP1p2, SP2 and SP2p2, and DSi 1.1 and 1.6 SP2); `__cxa_vec_ctor` with a count
  of 1, and a registered destructor that calls `__cxa_vec_cleanup`, come from a class whose member is the array: its
  implicit destructor is that function, emitted after the file's last function (`gMessageFiles`,
  `MessageFileList`). A global of an empty class with an empty inline constructor gives an initialiser that is a
  lone `bx lr` (`gCardPullOutMonitor`).
- **Virtual tables of classes without a virtual function of their own** (measured on `TextCanvas.cpp` and
  `MessageText.cpp`, classes with a virtual base and nothing virtual) are emitted in every file that needs them, as
  symbols of MW's binding 13 (shown as "processor specific"). The ROM has one copy of each, at its place in the
  `.data` of the first file in link order that has it (that the linker keeps the first is inferred; another class
  in the main program also uses `_ZTV13ObjTextCanvas`, from `0x02110880`): `TextCanvas.cpp` has `_ZTV12BgTextCanvas` and
  `_ZTV13ObjTextCanvas` (its complete-object constructors store them), `MessageText.cpp` the two construction tables
  `_ZTV12BgTextCanvas__10TextCanvas_at_1080` and `_ZTV13ObjTextCanvas__10TextCanvas_at_1416`, the VTT
  `_ZTT11MessageText` and `_ZTV11MessageText`, in that order, then its string literals. Such a table is three
  words (the offset of the virtual base, the offset to the top, a null type) and an object points at its END, so
  `relocs.txt` refers to it as `to:<table> add:0xc` (the VTT's entries too); a symbol at the address the code loads
  names the wrong thing.
- **Declaration order is emission order, reversed.** The compiler emits those tables in the reverse order of the
  classes' declarations (`ObjTextCanvas` is declared first so that `BgTextCanvas`'s table comes first, as in the
  ROM), and a file's `static const` arrays in the reverse order of their definitions.
- **A complete unit's anonymous objects are named by their compiler names** in `symbols.txt` (`@13766`, local):
  `dsd check symbols` wants every symbol of the unit's range by the name the linked program has. The numbers
  change when the file's code does, or a header it includes; the check names the new one.
  `tools/name_unit_data.py` lays a compiled object's data out from the start of each range and writes those names
  into `symbols.txt` (the save screens, overlays 69, 106 and 107, were completed with it). It lays out only the
  sections the link keeps: those reachable through the object's relocations from a function `symbols.txt` has, or
  from the static initialiser. So a constructor's string literals, which come twice, once for each constructor the
  compiler emits (`C2`, then `C1`), are laid out once, as the ROM has them (`Keyboard.cpp`), and so are the objects
  of a function the link strips. A header change renumbers the names of every complete unit that includes the
  header: `python3 tools/stale_names.py --apply` (after `ninja`) renames them in `symbols.txt`; neither the ROM
  nor `dsd check symbols` notices stale ones, `tools/check_relocs.py` does.
- **What else `dsd check symbols` asks of a complete unit** (overlays 96 and 110): a symbol dsd made inside an
  array or a string (a field the code addresses directly, `data_ov110_0214f654` in `sWindowButtons`) has to go,
  with its references in `relocs.txt` rewritten as `to:<array> add:<offset>`; the `kind:bss` placeholders of
  `.bss` blocks go; `static` functions are `local`. The linked module is then compared byte for byte, which finds
  data the functions do not show: two local `const` scalars in the wrong order, a string with a `/` too many.

## Which compiler, which flags

Not all of the game was built alike. Measured (details in [binary.md](binary.md), "Compiler", and the sections of
each library):

| Code | Compiler | Flags | How it was told |
| --- | --- | --- | --- |
| The game's own code (`src/`), WH | 2.0 SP2 patch 3 (build 138) | C++, `-ipa file` | Statics of 64 bytes and more are kept out of the file's pool; only 138 and 139 do that |
| The Mii transfer | 2.0 SP2 patch 2 (build 137) or earlier | C++, `-ipa file` | Its code reads statics of 64 bytes and more off the pool's start |
| The Mii data | 2.0 SP2 patch 2 or earlier | C++, `-ipa file` | Its 0x94 bytes of unreferenced records sit in one `.rodata` pool with the days table; 138 would give them a section of their own |
| The speech engine (SHTTS) | the game's | C, without `-ipa file` | Matches only that way |
| NitroSDK / NitroSystem / MSL | 2.0 SP1p2 / SP2 / SP2 | their own | As in other decompilations of the SDK |
| NFL | not measured | C, `-ipa file`, as a hypothesis | Its code compiles alike under every 2.0 build tried |

So when a file will not match, try the other build before the source (CLAUDE.md says as much). Every mwccarm build
is at `~/Projects/pokesxf/subprojects/metroskrew/bin/mwccarm-<version>.exe` (2.0 builds 72 to 87, 3.0 builds 114 to
139, 4.0 builds 1018 to 1051) for testing; they run under `~/Projects/pokesxf/tools/wibo/wibo`, without `-enc SJIS`.
Judge a match by the build, not by such a run: the project's own `mwccarm.exe` of 2.0/sp2, run under wibo, divided
MSL's `__pformatter` switch into jump tables differently from the same compiler in the build (which runs it under
wine), where metroskrew's `mwccarm-3.0-136.exe` under wibo agreed with the build.
The project itself fetches its compilers from the archive in `configure.py`.

Within a file, the original also set the optimisation per function: overlay 109's and the Mii transfer's functions
with loops that are neither rotated nor strength-reduced sit between ordinary ones, and match under
`#pragma optimization_level 2` around the function (then `#pragma optimization_level reset`). Nothing at the
default level gives that shape: not `volatile`, narrower or unsigned counters, other loop forms, or the `opt_*`
pragmas.
NFL's `NFL_MakeFace` has that shape and also reloads a list link that level 2 alone keeps in a register: it matches
with `#pragma opt_common_subs off` added (reset it after the function too: `optimization_level reset` does not).
NFL's hair meshes have that shape too, but are not level 2: they are the default level with eight of the IR
optimiser's passes off together, under `#pragma push`/`pop` (see "Arguments evaluated in another order"); level 2
gave their loops but not their argument order or their boolean chains.

Other per-function settings found the same way (overlay 109):

- `#pragma opt_common_subs off` at level 2 (then `#pragma opt_common_subs reset`) gave `UpdateCharacters` the
  original's hoisting and registers (72% to 99.3%), where level 2 alone hoists more and level 1 does not schedule.
- The static initialiser is compiled with the settings in effect at the end of the file. Overlay 109's original
  reads a `const` table from the `.rodata` pool instead of folding it, and matches with `#pragma opt_foldconstants
  off` as the file's last line; at the definition the pragma has no effect, and for the whole file it breaks other
  functions.
- The build passes `-w off`, so a pragma the compiler does not know is ignored without a word (`#pragma scheduling
  off` is one). Check a pragma on a small file with `-w all` first; the `opt_*` names it knows are in the
  executable's strings (`strings mwccarm.exe | grep ^opt_`).
- Builds 137, 138 and 139 compile overlay 109's near-misses identically, so they are not a version question.
- A group of functions that are one design may need one setting, not one each: NFL's seven expression functions
  had been matched with three settings (level 2 with common_subs off; with propagation off added; with
  dead_assignments off added), each the part of `optimization_level 2` + `opt_common_subs`, `opt_propagation` and
  `opt_dead_assignments` off that the function's code happened to show. All five that matched still match under the
  union. Try the union of a family's settings before contorting one member's source.
- Look for an SDK function the code was copied from: NFL's `NFLi_CopyToTexBank` matched as soon as it was written
  as NitroSDK's `GX_LoadTexEx`, line for line (its three bases read and shifted in the SDK's order), where no
  statement order of an independent form gave the literal pool's order and registers.
- `#pragma opt_common_subs off` alone, at the default level, gave overlay 76's `MiiEditFacePage::SetParameter` its
  clamp `CLAMP(get(p) + delta, min, max)` as the original's: the range's ends reloaded from the stack after each call
  of the getter, where the default hoists one of them into a callee-saved register.

## How a file's data is laid out

Measured by compiling test files and checked against whole compiled files (overlays 72, 98 and 99, WH); the
notes ([ov072](notes/ov072.md), [ov098](notes/ov098.md), [wh](notes/wh.md)) have the details:

1. The compiler keeps one list of the file's data objects, `.rodata`, `.data` and `.bss` alike, newest first. A
   static goes on it where it is declared, and the anonymous strings of its initialiser just before it (so in the
   list the object precedes its strings). In C, a static without an initialiser is a tentative definition and is
   only created at the end of the file; with `= 0` it is created where it is declared. In C++ every static is created
   where it is declared.
2. After the file's statics come the virtual table of the class whose key function is in the file, then the local
   `const` objects of its functions. A local `const` scalar is emitted as an object even when every use of it is
   folded; so is a file-scope `static const` read only with constant indices. The initialiser of an unused local
   `const` array stays too, packed after the other local initialisers in one byte-aligned section (the Mii
   transfer's state names).
3. An unreferenced static is dropped before the sort. So an object in the ROM that nothing refers to was referred to
   by something the link stripped: a function nothing calls. A small uncalled function in the source that uses it
   keeps it, and the link strips the function again (`MiiData_UnusedCopyDefault`, `MiiTransferWH.cpp`).
4. The list is heap-sorted by size: a plain max-heap sort, ascending, swapping a parent with a child only when the
   child is strictly larger, the left one first. The permutation depends only on the sequence of sizes, so objects
   of one size can be put in any order: compile once, read which slot each declaration lands in, and reassign the
   declarations of that size. Swapping two same-size declarations swaps their places and nothing else.
5. The four-byte-aligned objects of `.rodata` are pooled into one section at the place of the first of them, and so
   are those of `.data` and of `.bss` (with `-ipa file` only: without it each static is a symbol of its own). Arrays
   of `u8`, `s8` and `char` are never pooled. Build 138 keeps objects of 64 bytes and more out of the pool, each in
   a section of its own at its sorted place; build 137 pools them.
6. The strings of the code (literals in function bodies) are not on the list: they follow it in `.data`, in the
   order the code uses them.

Measured on overlay 76: because the list is one for all three sections, adding a reference to one `.data`
object (a table of pointers to wide literals) moved 65 of the 122 `.rodata` pool objects. So a pool laid out
while some functions are missing has to be laid out again as their statics come in.

Measured on overlay 76, which put all 122 objects of its `.rodata` at their ROM addresses: an `int` scalar goes on
the list where it is first referred to, not where it is declared (its slot followed the order of the references,
and moving its declaration changed nothing), while arrays and structures go where they are declared. Declared as
`int[1]`, the 4-byte scalars follow their declarations like the rest, and one round of the rank permutation below
placed them, and the `u8[4]` that the ROM has before the pool, in the ROM's order.

Not settled: overlay 109's agent found that this model does not reproduce its compiled `.data`/`.bss`, and the
best searches leave overlay 72's strings (50 of 134 objects) and WH-in-C (56 of 60) short. Test files on overlay
109's shape are the next step. Two other things the data showed:

- A `const` aggregate whose initialiser reads another constant (`{sPos[0].x - 0x20, sPos[0].y - 6}`) is filled in by
  the static initialiser at run time but stays in `.data`, as zeros in the ROM; a non-`const` one goes to `.bss`
  (the main program's list scenes, overlays 76 and 109).
- Comparing a compiled object's data with the ROM's before completing a unit finds wrong table values in the source,
  which no function shows (overlay 109's `sLayouts`).
- A `.bss` pool of small scalars sorted by size (overlay 109: a `u8`, 20 `u16`, ten `int`, then two arrays) is the
  compiler's pool of separate statics, not a structure: declared as statics in the right order it matches, and the
  code reads them all off the pool's base. Statics nothing in the file uses are kept by an uncalled stand-in
  function that the link strips (`TushinScene_UnusedClear`).
- Adding or removing objects changes the sort's permutation of every size, `.rodata` included, so the same-size
  declarations have to be permuted again after any change to the file's statics. The permutation of a compile is
  fixed, so this is mechanical: read where each declaration landed in the compiled object, give each position the
  declaration whose ROM address has that rank among its size, and compile once more.
- The same compiler run under wibo instead of wine (with the source converted to Shift JIS) gives the same code but
  another order of same-size objects: judge data layout only with the build's own compile. This holds for a
  `.bss` pool too: four `int` statics came out in different slots under wibo and in the build
  (`WirelessTestScene.cpp`), so permute declarations by `objdump -t` of the build's object.
- A pool that looks out of size order is often made of smaller objects than it seems. The 0x28 bytes at
  `0x02112DC4` that the static initialiser fills (0x18 of pairs, then a 0x10 table, then a 0x10 key array) are three
  8-byte objects, not one 0x18 array, and then sort as the model says (`KeyboardCore.cpp`). Data that is in no
  size order at all may not be the file's: see binary.md, "The keyboard's core".
- Every object of the original has to be on the list before the same-size slots come out right: also the ones that
  only code the link dropped referred to. The source keeps them through an uncalled function, and an unused local
  `const` array stays in the `.rodata` pool (`KeyboardCore_Unused`, `KeyboardCore_SetMode`). With the full set, one
  compile shows which declaration lands in which slot of a size; giving each slot's declaration the ROM's object put
  all of KeyboardCore's `.rodata`, `.data` and `.bss` in place in one pass.
- Function-local statics can be placed the same way, by solving on which function each slot's object belongs to
  rather than on names: a size sequence is good when each slot of a size holds a declaration of the function that
  the ROM's object there belongs to, and names are then swapped within each function. A random search over the
  order of each function's statics (and over which tables are file statics) with the model found overlay 39's
  whole `.rodata` and `.data` (31 statics, two local consts, 21 strings of initialisers), with every static in its
  function but one table, which had to be the file's first static.
- Strings in a file's `.data` that nothing refers to are the initialisers of a table the link dropped: a static of 64
  bytes or more is in a section of its own and goes with the function that used it, but its strings are in the pool
  and stay. Running the heap-sort model over candidate declarations (which arrays, their element order, where they
  are declared) and scoring against the ROM's string order found `TextUtil.cpp`'s three kana arrays, all 255 strings
  in place, in one pass of a few hundred candidates; the model then held under the build. An uncalled function
  that reads the arrays keeps them in the object (`KanaTables_Unused`), and the link drops it.
- `tools/name_unit_data.py` leaves out the sections the link will drop (with `--all` it lays them out too, after the
  range). `--apply` rewrites `symbols.txt` in another order; to keep the order, take its printed names for the range
  and replace the range's lines in place.
- Anonymous strings of an initialiser (a table of pointers to literals) cannot be permuted, since the table's order
  is fixed: make them named arrays (`static wchar_t sNgWord05[] = L"...";`, non-`const` to stay in `.data`), declared
  where the literals were, and the same-size permutation applies to them. Overlay 109's 44 strings went into the
  ROM's order in one pass of a script that reads each compiled slot and finds the object the ROM has there by its
  contents and swaps the declaration lines; a pointer table among them (`sCharacterArchives`) is placed by
  its known address and may move after its strings' declarations.
- Room in `.bss` that nothing uses is the objects' own size or alignment, and the alignment shows in the code:
  `ATTRIBUTE_ALIGN(32)` on a large static changed how the functions that use it address it (overlay 109's
  `sParents`, `sTransfer`, `sMiiData`), except where it did not (`sSongs`). A static whose type is a class with a
  constructor is built by the static initialiser with C1; a larger buffer made by deriving from the class calls C2
  instead, one made by holding it as a member keeps C1.
- Local `const` scalars come after all the statics and the virtual table (with `-ipa file` the functions are compiled
  at the end), in the order of their declarations: overlay 39's two message numbers.
- When a same-size group cannot be solved by swapping names (a local `const` array, or virtual tables, sit at fixed
  places in the list), change the size sequence instead. `src/shop/Shop.cpp` declares all its statics in one block;
  the model that reproduced its compiled layout is the list *reversed* (newest first) with the virtual tables and the
  local array after the block, heap-sorted as above. A few compiles narrowed the tables' order to candidates, a random
  search over one or two moved declarations found a sequence that put `SetKind`'s local array last among the 16-byte
  objects and `ShopItemListView`'s table before `ItemListView`'s, and swapping names within each size did the rest.
  `u16` arrays are pooled too, at 2-byte alignment, between the 8- and 12-byte objects. A point read only field by
  field (folded into immediates) is still an object of the pool, so the ROM's pool has every folded static too.
- Mind the build: when a quick `ninja <object>` fails (compile error), the old object stays and a layout read from it
  is stale.
- A function-local static with a dynamic initialiser goes on the list where its declaration is parsed, not where the
  function is compiled: its table, then its guard (`.bss`). So file statics declared after the function come after
  it on the list, and the declarations' order *across* functions moves it. `Sky.cpp`'s two seconds tables
  (`GetSunrise`, `GetSunset`, 48 bytes each, with 4-byte guards) came out reversed in `.data` with any order of the
  seven file statics at the top that keeps the guards right, and both came right with `sSunsetTimes` declared between
  the two colour tables. The heap-sort model with the locals placed so (table, guard) reproduced every compile tried
  and found that order at once (a search over placements before, between and after the functions).
- An anonymous local aggregate initialiser (`FloorPos p = { 0, 0x3000 };`, `const` or not) goes on the list where
  its function is in the source, exactly as a named static declared just before that function would: measured with
  test files (one function's objects between two statics come out between them) and on overlay 74, where replacing
  all 20 of its point initialisers by statics declared before their functions gave the same layout. Not after all
  named objects, as overlay 73's early notes had it. (Local `const` *scalars*, overlay 39, are another matter.)
- The model holds for a whole large file: overlay 74's 12,800 lines, with its virtual tables, VTT and construction
  tables taken as created at the end of the file (in the order `RoomScene`, `TimedMessage`, the construction tables of
  `MessageText`, `ObjTextCanvas`, `BgTextCanvas`, the VTT, `ApartmentRoom`, `HeldItem`), reproduce the compiled object
  exactly, every group of one size included (fin-ovl: the file cut at each class boundary and compiled, each prefix
  checked against the model).
- What looks like objects out of size order after a pool may be ONE larger object: overlays 73 and 74 have three
  16-byte touch rectangles after their pools' 44- and 22-byte tables, which is where one `ScreenRect sButtonRects[3]`
  (48 bytes) sorts; the code takes `&sButtonRects[1]` and the like, three literals that dsd made three symbols.
- A pooled object that nothing in the ROM refers to was read by a function the link dropped, and the file had that
  function: overlay 73's `sButtonTouchRect` belongs to `BuildingButton::Contains`, which overlay 74 has and the
  tutorial never calls (`TutorialBuildingButton::Contains`, written and stripped). With it, overlay 73's `.rodata` is
  the ROM's byte for byte.
- The search over declaration orders is a needle in a haystack: a model run takes microseconds
  (scratchpad tool `lay.c`, simulated annealing over the creation order), but the heap's permutation changes
  wholesale with any move, so annealing does not even recover a known solution after three objects are moved. What
  works is to make every object of a group freely assignable (named, declared before all its users), solve only the
  objects whose order is fixed (an initialiser's strings, the tables), and then reassign names by one compile.

## How the compiler orders and colours

Read out of the game's compiler (2.0 sp2p3 `mwccarm.exe`: its scheduler at `0x4ff730`/`0x4ff8f0`, the ARM9 machine
model at `0x4f5dc0`-`0x4f6140`, the allocator at `0x513780`-`0x513cf0`), compared with the decompiled source of the
GameCube compiler (github `rayanht/mwcc`, `src/backend/Scheduler.c` and `Coloring.c`: the same algorithms), and
measured. `tools/sched_probe.py show FILE` compiles a file with a patched copy of the compiler that logs, for every
block it schedules, the dependence graph it built and every choice it made (with the pressure counter below);
`tools/sched_probe.py check FILE` replays the log with the rules here. They reproduce the compiler cycle for cycle on
more than 99% of the blocks of `SceneManager.cpp`, `SavedCharacter.cpp` and the scenes (what is left depends on which
registers are live out of a block, which the log does not show). So these are the rules, not a fit; when code is
scheduled differently from the original, the question is which input to them differs.

**The passes.** Instruction selection (virtual registers, `v16` up, numbered as they are created), coalescing of
copies, local optimisation, then the scheduler on virtual registers ("pre-RA"), register allocation, peephole, and
the scheduler again ("post-RA"). The second run only takes blocks the first did not schedule: in practice blocks
that were merged afterwards (with the epilogue, or by if-conversion). The unit is the basic block. A call does not
end a block: it is a barrier (everything before it comes before it, everything after after it), so a function
without branches is one block, and what happens in it depends on the whole function.

**What is ready, and what goes first.** Single issue. An instruction is ready when all its predecessors have issued,
their latencies have elapsed and the issue slot is free. Each cycle the scheduler takes the first ready instruction
in block order (the order instruction selection produced, i.e. the source's evaluation order), and replaces it by a
later ready one when, in this order of tests:
1. the later one is critical (its latest start cycle, longest path to the block's end subtracted from the block's,
   has passed) and the current choice is not; in a long block nearly everything is critical, so this rarely decides;
2. it frees more successors (successors for which it is the last unissued predecessor);
3. it is higher (longer latency path to the block's end);
4. pre-RA only: it has the lower rank: copies 0; stores, compares and branches 1; arithmetic with register operands
   (and calls) 2; loads 3; constants and addresses (`mov rX, #n`, `ldr rX, =sym`) 4.
Otherwise the earlier one stays: ties go to source order.

Example (`SceneManager::Init`, `ScreenLoader_SetPriority(&gScreenLoader, 0x1, 1)`, compiled `mov r1, #1; mov r2,
r1`): by rule 2 `mov r1, #1` goes before `ldr r0, =gScreenLoader`, both of height 21, because only it frees the copy
into r2. That is the original's order; ours has the address first because of the next rule.

**Pressure mode** (pre-RA only, an ARM addition). A counter is kept while scheduling: +1 at each definition whose
register still has a use ahead in the block, -1 when a register's last use in the block issues. Physical registers
count too, and once per definition: every argument set-up in r0-r3, and every clobber of an argument register a call
does not use, adds 1, and nothing takes it off until that register's last use in the whole block. So the counter
climbs with every call in a long block (in `SceneManager::Init`, by 3 per `ScreenLoader_SetPriority`, to 41). From
14 (the allocatable registers r0-r12, lr) on, the rules above are replaced: the ready instruction with the lowest
pressure delta wins (16, less 1 + max(1, counter - 14) for each use that is its register's last, plus max(1, counter
- 14) for each definition with uses ahead), and ties go to source order. Address and constant loads then all tie, and
come out in source order instead of by height. This is why scheduling "flips with unrelated code earlier in the
function": the counter at a statement depends on every call before it in the same block.

**Latencies** (ARM9 model): `ldr` and literal loads 2, `ldrb`/`ldrh`/`ldrsb`/`ldrsh` 3, `mul`/`mla` 6,
`smull`/`umull`/`smlal`/`umlal` 7, calls and branches 3, an operand shifted by a register +1, everything else 1. An
instruction occupies the issue slot for the same number of cycles, except loads (1) and `ldm`/`stm` (one per
register). A true or output dependence carries the producer's latency; an anti-dependence (a register read, then
written) carries 0 but still orders.

**Memory dependences** come from the optimiser's alias information, not from the registers: a store is ordered after
every earlier load or store that may alias it, and a load after every earlier store. Two accesses through the same
base expression at disjoint offsets are independent (`light->x` and `light->mEnabled`); through different base
expressions they may alias, even two different globals (the stores to `gScreenControl` are ordered before the light
stores in the test scenes). So writing a field through another pointer, or through the global instead of a local
pointer, adds ordering edges and changes the schedule.

**Evaluation order.** An addition generates its left operand, then its right, unless the right contains a call; there
is no weighting. The tree is the optimiser's, which reassociates address arithmetic, so spellings of `t[a][b]` all
come out alike. A register that a parent asks for (an indexed load's index, a call argument) is created before the
operands that compute it, so it gets the lower number: in `Use(sTable[kinds[p]][1])` the product `kind * 20` is
`v17` and `kind` is `v21`. A local survives coalescing (keeps its own number) only if it has another use. An inline
function's argument is substituted at its uses when it has no side effects (constants, addresses, reads of a local
whose address is never taken); other arguments are bound to temporaries, last argument first (fin-lib, from
`CInline.c` and measured).

**Colouring** is Chaitin's, as in the GameCube compiler: the graph is simplified in passes over the virtual registers
in increasing number, each pass removing every node with fewer neighbours than registers; when none can be removed,
the one with the lowest spill cost per neighbour is removed as a spill candidate and the passes start again. Nodes
are then coloured in the reverse order of removal, each taking the lowest-numbered free register among the scratch
registers and the callee-saved ones already taken; when none is free it takes the next callee-saved register, from r4
up. So among values removed in the first pass the highest-numbered (created last) is coloured first; values that
interfere with more (removed in a later pass) are coloured before those; a cheap spill candidate (a hoisted address
used rarely) is coloured last. Measured: `a = g(); b = g(); c = g(); h(); use(a); use(b); use(c);` gives c r4, b r5,
a r6 (and `a = g(); b = g();` b r4, a r5). Two values that swap registers with the same instructions were created in
the other order, or have other neighbour counts.

**A block scheduled twice** (overlay 103, inferred from `tools/sched_probe.py show`): `DivorceScene::Init`'s
blocks were logged both before and after register allocation, and the second pass reordered address and constant
loads that the first had put in the original's order. The block held a copy (`a = b = 1`, a chained assignment)
that coalescing removed; written as two statements, the block was scheduled once and the function matched. When a
function's first-pass order is the original's but the output is not, look for a copy that coalescing deletes.

**Applied.**
- The light-0 block of overlay 4's `CharacterBodyTestScene::Init` (x stored before `mEnabled` in the original). The
  original order needs, after the store of x, the store of `mEnabled` chosen before the two argument loads of
  `ModelResources_SetModel`, which are higher (14 against 12). In the compiled block the counter is 13 there, so the
  loads win; at 14 the store would. A replay of the block's real graph over 3,000 orders of its fifteen instructions
  never gives the original's order at our counter, and gives it at one more. A search over single added dependences
  (every pair, latencies 0 to 3, replayed) finds exactly one family that gives the original's order with the source
  order x-then-`mEnabled`: an edge of latency 0 from the `mEnabled` store to the `ldr r0, =gModelResources` (or to
  the path's `ldr r2`). Latency 0 is an anti-dependence: in the original's virtual code the store read the register
  that the argument load then writes, i.e. the 1 stored into `mEnabled` was already in the physical argument register
  before allocation (as the 0 of y and z is r1, shared with the call's second argument, in ours). Which source does
  that is not found yet; it is the question to answer for all six light blocks.
- The method (fin-lib, on NFL): replay the logged graph with one dependence added at a time and compare with the
  original's order; the edge that reproduces it names what the original's source had (there, a may-alias edge from a
  table load through a pointer, `const u8 *entry = table + type; return *entry;`, matched two functions).
- `SceneManager::Init` (four pairs of address and constant loads in the other order): ours is in pressure mode from
  the fifth `ScreenLoader_SetPriority` on, and pressure mode puts addresses and constants in source order. Replayed
  without pressure mode, the block gives the original's order everywhere but in the `gPaletteLoader` / `gPadInput`
  stores. So the original's counter stayed below 14 there: its block, or the count of definitions in it, was
  different.

## Matching techniques

What made near-misses match, by symptom, with where each was found. Most are about the order in which the compiler
sees things: declarations, statements, and how an address is formed. Work by script: compile one object with source
variants and compare one function (`tools/compare_object.py --diff`), then confirm with `ninja report`:
`compare_object.py` masks the bytes of calls and has called a function equal that objdiff did not.

**An address reloaded at each use, where the original keeps it in a register**
- A global's address loaded at the function's entry into a callee-saved register, before a switch, and used in
  several of its cases: a local pointer initialised at the top (`Camera *camera = &gCamera;`, then
  `camera->mEye.x`); the global written at each use is loaded in each case (overlay 75's character states).
- Write through a pointer made by a cast: `GXRgb *colors = (GXRgb *)gSceneScratch;`. A plain local pointer to the
  global is dropped and the address reloaded (`MapScene::Exit`).
- `#pragma opt_propagation off` around the function keeps constants and addresses in callee-saved registers across
  calls (overlay 110's camera setters, the Mii transfer's `MiiTransfer_UpdateSession`).
- In a one-iteration loop a large member offset (`this + 0x268`) is computed once and kept (`MapScene::Init`, the
  back button in two one-iteration loops around its `SetPosition`). A one-element static array read in a
  one-iteration loop is loaded, not folded.

**An address kept in a register, where the original reloads it**
- Reach the member through a local pointer in the loop body (`MapCamera *camera = &mCamera;`): it stopped the
  compiler holding a table's address for the whole loop and spilling (`MapScene::Update`).
- A local `char` buffer filled backwards: `p = buff + size; *--p = 0;` makes the compiler reload a local struct's
  fields after each store through `p`, as the original does, where `p = buff + size - 1; *p = 0;` lets it keep them
  in registers (`float2str`, `wprintf.c`).
- A loop count the original reads again at the end of every pass but not for the first test (`cmp r3,#0; ble`
  on the value just tested by the `if` before the loop, then `ldrh` at the bottom): write the first test out and
  read the count through a volatile cast at the end only,
  `i = 0; if (i < kb->numAreas) { do { ... } while (i < *(volatile u16 *)&kb->numAreas); }`
  (`KeyboardCore_FindKeyAt`). Loop forms, inlines, pragmas and other compiler builds kept the count in a register.
  `#pragma opt_common_subs off` gives the reload but reallocates the rest of the function.

**An address loaded and then offset by an `add`, where ours has the offset in the literal**
- mwcc folds `&table[1]` and `table + 1` into the literal (`table+0x2`), but not after a pointer cast:
  `(const u16 *)wideTable + 1`, and the same for an address of a string literal (the keyboard's character tables,
  `KeyboardCore_UseEnglishLayout` and the four after it).
  `func_02082b48` and the four after it).
- The same for a member: `(u8 *)&mKeyboard.mButtons + 0x6B8` keeps the original's `ldr` of the member's offset and
  `add 0x2b8; add 0x400`, where `&mKeyboard.mButtons[1]` folds the two (`MiiEditProfilePage::Update`).

- A one-element member array used in a one-iteration loop (`for (int i = 0; i < 1; i++) f(&mButtons[i]);`) keeps
  part of the member's offset in a register (`add r4, this, #0x348` then `add r0, r4, #0x1bc00` at each use); a
  single member, or a pointer to it, does not (overlay 39's shutter button and caption, which the main program's
  constructor shows as arrays of one). Static `const` arrays of one element read there with `[i]` are sometimes
  folded (the shutter's position and size) and sometimes loaded (its `u16` sequence, the caption's font and
  position), as the original has them.
- An element after the first reached through an inline accessor (`GetSpeechTexts(1)->mName`, returning
  `&mSpeechTexts[index]`) is formed as base, plus the element's size, plus the field (three `add`s), where
  `mSpeechTexts[1].mName` folds the offset into one (`TravelScene::Init`).

**Two literal-pool entries for one address**
- One is an object's end: `&keys[ARRAY_COUNT(keys)]` is the object plus its size, and the next object's start is
  another literal. In `relocs.txt` such a reference is `to:<object> add:<size>`, so that `check_relocs.py` names
  both alike (`KeyboardCore_Init`'s key counts, `((const u8 *)end - (const u8 *)start) / sizeof(KeyRect)`, which is
  also what gives the original's `lsr #3`).

**Two operands loaded in the other order**
- `f(b)` called before `f(a)` and compared `cmp r0, r5` (the later result first): `int sb = f(b); int sa = f(a);
  if (sa == sb)`; `f(b) == f(a)` gives the calls in that order but the compare the other way
  (`EncounterJudge::Judge`).
- A range check `(int)(now - start) >> 9` on an `s64` difference shifts only the low word: the cast is in the source
  (`DayPhase::Compute`).
- How an object's address is formed decides it. Two adjacent members opened through one pointer to the first,
  `MessageFile *files = &mEtcMessages; files[1].Open(...); files[0].Open(...);` (`MiiListMenuScene::Init`).
- Do arithmetic in a statement of its own before the call: `hour -= tens * 10; f(hour);` rather than
  `f(hour - tens * 10)` (`ClockDisplay::Refresh`, in overlays 72, 73 and 74).
- Pass the value just stored in a member (`f(mLights[0])`) rather than recomputing it (`MapScene::Init`).
- Declare a value before the call that might clobber it: `u8 t = others[i];` before `Rand` (`MakeDailyNews`).
- The type of an inline's parameter: `Shift(const VecFx32 &v, u32 shift)` loads the shift before the vector's x,
  where `int shift` loads it after (`WeddingCamera::Approach`, overlay 96).
- A call's result through a local before it is stored, `GXRgb color = f(phase); light->mColor = color;`, moved an
  unrelated address computation after the store, as the original has it (`MapScene::Init`).
- `int next; BOOL go; go = FALSE; next = 0x44;` gives another order than the initialised declarations
  (`UpdateConnection`).
- `hour * 3600 + minute * 60` computes the minutes first.
- A parameter passed on through a local copy (`ld = num; ... __num2dec(&form, ld, &dec);`) is loaded before the
  function's first early return, where passing the parameter loads it after (`double2hex`, `wprintf.c`).
- `x -= widths->charWidth >> 1; obj->field = x;` loads the object's pointer before the `ldrsb`, where
  `obj->field = x - (w >> 1)` loads it after (`MiiEditProfilePage::Update`).
- `value = get(); ... value += step;` gives `add r5, r0, r5` where both `get() + step` and `step + get()` give the
  operands the other way (`MiiEditFacePage::StepParameter`).

**Stack slots in the wrong order, or a frame too large**
- Dead copies of a by-value point that share ONE slot across the cases of a switch, beside a fresh slot per use:
  the point is a local of the function (`FloorPos target;` at the top, set field by field in each case, then
  `SetTargetFloorPos(target)`); the fresh slots are the by-value parameter's. An inline returning the point gives
  two fresh slots per use (overlay 75's character states, five functions: one shape for every "walk to the
  camera").
- A struct zeroed through a pointer into one slot and copied with `ldm`/`stm` into another, whose address is passed:
  an inline that returns it, `RTCTime time = Midnight();` with `RTCTime t = {0, 0, 0}; return t;` inside
  (`GetDayNumber`, `src/shop/ShopStock.cpp`); a named local, a block or a by-value parameter put the two slots the
  other way round.
- Two objects of a class with a constructor, constructed in address order with the first at the lower address:
  an array, `SavedCharacter characters[2]` (`EncounterJudge::MeetRandomPair`); two locals are constructed in
  declaration order but placed the first-declared higher.
- A local declared in a block sits below the function's own locals: `__strtold`'s result union, moved into the block
  that builds it, went below the other arrays and gave the original's frame. A union holding a `long double` is
  8-aligned.
- Dead copies of `FloorPos` points around an angle (`FX_Atan2Idx` of a target minus where a character stands): an
  inline taking the target by value, `FloorPos delta = target - character->GetFloorPos(); return
  FX_Atan2Idx(delta.z, delta.x);`, as overlays 74 and 96 have it (`AngleTo`); writing the same expression in the
  function, with `Angle(...)`, `Sub` or named locals, put the four copies in other slots (overlay 4).
- Slots follow the order of the caller's locals; an inline's locals and by-value copies sit below them. Declare them
  in the caller to move them up (`MapScene::Update`), or use an inline to move a copy down (`OptionScene::OpenWindow`).
- A struct made only to pass by value costs a slot the original lacks: pass the `static const` itself (overlay 109's
  `SetUpWindow`, `SetUpList`). Where the original has the slot, make the local and pass its address, point first
  (`ScreenPoint point = *touch; ScreenRect rect = GetAmPmRect(); ScreenRect_Contains(&rect, &point);`,
  `OptionScene::Update`).
- Two dead copies of a vector are the by-value `FxVec2` of `SetPosition(FxVec2(x, y))`; which of
  `SetPosition`/`SetPositionFromVec` is called decides their order. All three forms (`SetPosition(FxVec2(...))`,
  `SetPosition(x, y)`, `SetPositionFromVec(x, y)`) give different store orders or slots, and overlay 76's face page
  uses each in a different loop of one function: try all three.
- A `static const` point read field by field is folded into immediates; the original loading it from the pool, with
  three dead pairs, is the point passed by value to an inline that does the shift and `SetPosition`
  (`SetCanvasPosition` in overlay 76's `MiiEditScene::Init`).
- A point plus an offset put on a sprite with six dead pairs per sprite (the point, the offset, the sum twice, the
  shifted pair twice): `SetSpritePosition(&sprite, AddPoints(position, sOffset))` with
  `template <class A, class B> static inline A AddPoints(A a, B b) { A p; p.x = a.x + b.x; ...; return p; }`, the
  sprite helper taking the point by `const &` (its temporary then sits above the inline's copies, among the caller's
  locals) and calling `SetPosition(x, y)`; a canvas helper taking it by value. That matched `ShopWindow::SetPosition`
  (`src/shop/Shop.cpp`); a template lets one helper take both `WindowPoint` and `ScreenPoint`. The forms of the
  setters differ between functions of one file and even between sprites of one function: `Shop.cpp` has all three
  (search each call's form by script, one at a time, keeping the best).
- Not solved: the item list's two `Refresh` functions (`Shop.cpp`), with the same helpers per row, keep 2 more points
  of the original's frame; the original zeroes a point through a pointer inside the loop (`add r1, sp, #264;
  str r2, [r1, #4]; str r2, [r1]`, y first) and adds its x as `add fp, r3, #0`. An inline returning a zeroed point
  gets the zeroing, but hoisted out of the loop.
- A local copied inside a loop is hoisted before it and takes the original's slot; declared before the loop it gets
  another (the main program's list scenes).
- Spill slots of a function too large to keep its variables in registers (MSL's `__sformatter`, 3.4 KB) follow the
  order of their declarations, and a 64-bit variable can be given two slots apart. One variable that is saved in two
  unrelated places got two slots where the original had them elsewhere: two variables, one per place, matched. A
  script that permutes the declarations and counts the differing lines gets close fast; the rest took that split.
- Spill slots that no declaration order moves (lib/NFL's mesh builders, about 40 spilled values each): the types
  decide them. `u32 kind` (the type of the parameter it is passed to) where `int` put it below all the others;
  `int scaleX` where `fx32` (`s32`, a `long`) put the scales above two loop-hoisted products instead of below; `fx16`
  sine and cosine assigned through a same-type cast (`sin = (fx16)FX_CosIdx(i);`), where without it they went below
  the scales. Writing a product the loop hoists inline at its uses, instead of through a local set in the outer loop,
  gave the original's reload of the inner counter. Block locals moved to function scope helped as well.
- A constant vector copied through a local and stored field by field (`VecFx32 target = sConst; mTarget.x =
  target.x; ...`) where a by-value setter, or the same code in an inline, gave three vector copies one another's
  slots (`WeddingCamera::Reset`, overlay 96).
- A value used once, taken straight off a call's result (`f().LengthSq()`) rather than through a local: its copies
  become temporaries below the function's locals, which let a later block's local take the slot above them
  (`RoomScene::StartMeeting`, overlay 74).
- A point passed by value to an inline (`SetPixelPosition(Sprite *, ScreenPoint)`) puts its copy among the
  temporaries in the order of the source, where a block-local copy (`ScreenPoint p = table[i];`) goes above the
  temporaries of earlier blocks (`TravelScene::Init`, overlay 39). Searching the three position setters
  (`SetPosition(FxVec2(...))`, `SetPosition(x, y)`, `SetPositionFromVec`) for each of the function's sprites by
  script found the rest of its slots.
- Not solved: `MiiEditProfilePage::SetState` (7 KB) keeps the WindowPoint copies passed by value to its windows'
  buttons in six slots below every other local (two of them copies of one size, one never read). Fresh copies per
  window move them to the bottom but in another order and without the unread one; reusing one copy across windows
  gives it a slot among the function's own locals. A search over 128 combinations of fresh and reused copies and over
  the declarations' places did not find the original's order.

- Two dead copies of a returned point at the top of the frame, above the function's other locals
  (`Character::GetPointInFront`): the result declared first and ASSIGNED, `FloorPos result; ...; result =
  GetFloorPos() - offset; return result;`. The assignment copies the value to a temporary and then to the variable;
  an initialisation makes no copy. A local initialised from an inline's by-value return (`FloorPos offset =
  Polar(distance, angle);`) is placed below that temporary, where one filled field by field is placed above it.
  Found by the advisor (round 5).

**Registers allocated differently**
- Measured with fin-sched's probe compiler (pre-RA virtual registers): an indexed load numbers its index register
  before it generates the index's operands, so in `table[k][1]` (a 2D table, constant column folded into the
  base) the product `k * 20` is numbered before `k`; colouring takes the highest number first, so `k` gets the
  lower register (`ldr r2,[kinds]; mul r3,r2,r0; ldr r2,[t+4,r3]`). A local `k` with a second use keeps its own
  (lower) number and reverses the two. With `table[k][mood]` (mla) the row address is numbered after `k`. Not
  solved: overlay 75 has `ldr r3,[kinds]; mul r2,r3,r0` at four single-use sites, which no source form tried
  gives (pointer forms, flat index, getters, locals at any scope, casts, a file-static table, ten opt_ pragmas).
- A value written out at each use instead of kept in a named local: `(u16)(i * 2)` in both calls, not
  `int sequence = i * 2`, put the button offset and the sequence in the original's registers (overlay 76's
  `MiiEditFacePage::func_ov076_0214c6d0`). A member read at each use (`mScene->...`) instead of a local copy of the
  pointer put the constant TRUE in the original's register (`func_ov076_0214de94`).
- A loop that counts down a variable of its own and steps a second one (`for (k = n; k >= 1; k--, bit -= 4)`) gives
  the two the other pair of registers from `for (i = k, ...)` over a copy; and a byte swap
  (`t = cp[i]; cp[i] = cp[7 - i]; cp[7 - i] = t;`) through a `char *` declared in a block of its own keeps both
  accesses indexed, where one through `(char *)&num` is turned into a post-incremented pointer (`double2hex`).
- Search the declaration order by script rather than by hand: move one declaration line at a time to every other
  place, keep the best, repeat. NFL's mesh functions (`NFLi_MakeBeardMesh`) went from 18 differing instructions to none
  in four rounds; what mattered was declaring the inner loop's index variables at function scope, in an order no
  guess had tried. Keep arrays and address-taken structures where they are: they decide the stack slots.
- A fresh `for (int i ...)` in every loop (overlays 98, 99); C-style declarations at the top, in the order the
  original spills them.
- Write a loop the way another file writes the same loop: overlay 110's partner loop matched once it used overlay
  109's form (`continue`, and `mWindow.mTexts[i + 1]` at each use instead of a local).
- Pass a member's address again rather than a local pointer to it, or the reverse (`ReloadCharacter`).
- A loop over an array of objects that reads a member's member: `mButtons[i].GetCharCount()`, an inline of the
  element's class, keeps a pointer to the element; `mButtons[i].mText.GetCharCount()` keeps one to the member
  instead (`SoftKeyboard::GetButtonCharCount`).
- A pointer to a member array declared before a local `const` array is formed before the array's `ldm`/`stm` copy,
  declared after it, after (`SoftKeyboard::OnTabArea`, `Keyboard.cpp`).
- In C (lib/NFL, measured on functions whose instructions were already the original's): a pointer initialised from a
  global with a cast to its own type, `NFLWork *work = (NFLWork *)sNFLWork;`, takes r4 where the plain initialiser
  leaves it last (`NFLi_CopySharedPltt`, `NFLi_LoadSharedTextures`, `NFLi_LoadFacePltts`). An uninitialised local
  declared above the initialised pointers takes a lower register than they (`u32 dest;` before `stream = work->streams`
  in `NFLi_LoadSharedTextures`), and initialised declarations take registers in their order (`NFLi_CopyFacePltts`). A
  variable used in two exclusive paths is one register for both: giving the second path a variable of its own block
  moved two registers into place (`NFLi_CopySharedPltt`). The type of a block local counts: `u32 over, n` where `int`
  gave other registers (`NFLi_LoadSharedPltt`). So does how a value is returned:
  `if (result != 0) return result; return 1;`, not `if (result == 0) result = 1;` (`NFL_LoadVram`), and
  `result = f(); return result;` kept a tail call out of the shared epilogue (`NFLi_LoadFaceTextures`).
- The order of independent statements in a case decides the scheduling of their loads: `NFLi_LoadFaceTextures` and
  `NFLi_LoadFacePltts` matched only with one of the six orders of their three assignments, found by trying them all.
- Brute-force the order of the C-style declarations at the top, the loop counters included: a script that splices each
  permutation into a copy of the file, compiles it and counts the differing lines of the one function. It took
  `KeyboardCore_MoveCursor` from 153 differing lines to 0 (`int x, endY, endX, dy, dx, px, y;`), and an `int i;`
  declared first fixed `KeyboardCore_DrawKeyLabels`.
- A row pointer that the original recomputes from the outer counter each pass (`add r0, base, y, lsl #6`), where
  ours strength-reduces it: index as a two-dimensional array, `((u16 (*)[32])screen)[y][x]`
  (`KeyboardCore_MakeScreen`).
- A value the inner loop does not change, which the original computes after the inner loop's guard, where ours
  hoists it and tests the guard twice: write the expression again at each use instead of keeping it in a local
  (`y0 - ty * 8 < 0 ? 0 : y0 - ty * 8`, `KeyboardCore_LightRect`).
- How the colours are given (measured with MSL's compiler, 2.0/sp2, in C, on test files): per live range, the value
  defined last first, each taking the lowest register free of its neighbours. Declaration order, number of uses
  and length of the range change nothing. So to move a value to a lower register, define it later, or define the
  value that holds that register earlier.
- Index an array where the original walks it with a pointer, or the reverse: `nan_str[i] == toupper(c)` with the
  count `i`, instead of `*p` with `p` stepped beside it, put `__strtold`'s NaN state in the original's registers.
- Pass a member's address again rather than a local pointer to it, or the reverse (`ReloadCharacter`). An array
  element read again at each use (`pair[i]->...`) rather than through a local pointer gave the original's
  callee-saved registers (`RoomScene::StartMeeting`).
- A member set in an if/else rather than by a ternary, and constant tables indexed directly rather than through local
  pointers (`sButtonPositions[i]`, not `positions = sButtonPositions; positions[i]`), gave the original's choice of
  spilling `this` and keeping a table's address in a register for the loop (`ItemTradeScene::SetupWindow`).
- `#pragma opt_common_subs off` around the one function (and `#pragma opt_common_subs reset` after it: `#pragma
  optimization_level reset` does not reset it) where two values swap registers whatever form the source takes (the
  Mii transfer's `MiiTransfer_OnPacketReceived`; overlay 109's `UpdateCharacters` with level 2). Pragmas the compiler does not
  know are ignored without a word under the build's `-w off`; the `opt_*` names it knows are in its executable
  (`strings mwccarm.exe | grep '^opt_'`).

- A cast to the type a value already has, on an assignment (`sin = (fx16)FX_SinIdx(idx);`), moves that variable
  to the lower callee-saved registers; with `#pragma optimization_level 2` and `opt_common_subs off` it gave
  `NFLi_ApplyExpression0` (lib/NFL) the original's order, the parameter in r8. A variable assigned a second time is
  coloured as a new value: there the second pair of assignments had to keep the casts too.
- An element pointer taken from the index at the top of the loop body, `stream = &obj->texStreams[i];`, where a
  pointer stepped in the `for` beside the counter (`stream++, i++`) gave the same instructions with four
  callee-saved registers rotated (NFL's `NFLi_CopyFaceTextures`, 99.1% to 99.9%). In `NFLi_CopySharedTextures`
  the same change made things worse, and declaring `source` before `stream` was what helped.
- A loop counter and a value it is compared with in the original's registers: `int i; int choice;` at the block's
  top and `for (i = 0; ...)`, where `for (int i ...)` swaps them (the four choice loops of
  `MiiEditProfilePage::Update`).

- A list filled in a loop with its count kept through a pointer (`*count`) and an index of its own: `list[n] =
  item; (*count)++; n++;` gave the original's registers, where `list[n++] = item` swaps the two counters
  (Item.cpp's list getters).
- A row of furniture placed from a table, where the original computes the element's address from the counter at
  each pass and ours strength-reduces it: an inline member taking the index (`SetPartPosition(i, pos)`) of the
  class that holds the array (Room.cpp's `LoadParts`).
- Two members set from one value without a reload: `mAngle = mTargetAngle = 0;` (Mannequin.cpp).
- A constructor that stores its virtual table after a member's constructor call has that "member" as a second base
  class (`ItemModel : public AnimModel, public InventoryItem`; `Room : public InteriorItem, public RoomModels`).
- dsd gives each delinked unit's sections the module's alignment and takes no `align:` on a file's line: a unit (or
  the gap after one) whose `.rodata` starts at a two-aligned address moves everything after it. Look for a small
  object that lets the boundary be four-aligned (the room file's halfword at `0x020FBFD4`).
- A call's result through a local before a field is set (`ModelAnimation *a = AnimModel_FindAnimation(...);
  a->mFinished = FALSE;`) swapped two strength-reduced loop pointers into the original's registers, where the
  chained `f(...)->mFinished = FALSE` gave them the other way (`TravelScene::SetState`).
- A variable used in two cases of a switch, declared once at the top, got one callee-saved register; declared in
  each case's block, the original's two (`TravelScene::SetState`).
- In a shuffle, the element read before the random draw (`t = a[i]; j = Rand(n); a[i] = a[j]; a[j] = t;`) gave the
  original's registers and load order (`TravelScene::StartLoading`).

**Fields read off an element's address, where ours folds the offsets**
- At optimisation level 2 the compiler stops folding offsets through inline calls: `GetSelectedResident()->
  GetIndex()` reads `mIndex` off the element's address, as the original does (overlay 109's `UpdateSending`).

**Arguments evaluated in another order**
- A stack argument stored late, with the loads and constants after it hoisted before an unrelated value's last use
  (NFL's `NFLi_ApplyExpression1` and `4`): the store depended on an earlier load in the block. The scheduler orders a
  store after every earlier load that may alias it; a load from an array indexed directly does not (the IR knows the
  object), a load through a pointer does. Reading the table entry through an inline accessor that forms a pointer,
  `const u8 *entry = table + type; return *entry;`, added the original's dependence (found by replaying the logged
  block with one extra edge at a time in fin-sched's `replay.py`, which reproduced the original's order only with an
  edge from that load to that store). A pointer variable set in an earlier block also aliases, but keeps the
  address in a register where the original reloads it.
- Which arguments of an inlined call are evaluated at the call and which inside the body (read in CInline.c's
  `setup_inline_locals_and_arguments`, measured on lib/NFL): an argument free of side effects that is a constant, an
  address, or a plain read of a local NOT marked address-taken is substituted into the body and evaluated where the
  parameter is used. Every other argument is bound to a temporary first, last argument first (the initialisers are
  prepended into one comma expression). A local is marked when `&x` is parsed (CExpr.c), but an array passed by its
  name is not. So element 0 of an array (`v[0]`, offset 0, a plain read) is substituted while `v[1]` and `v[2]` are
  bound. The hair meshes matched with the vertex as `fx16 vertex[3]` passed as `(VecFx16 *)vertex`: x is then
  loaded after `&ctx->dl` is formed, where a `VecFx16` passed as `&vertex` binds all three before it.
- The same IR order can come out in two orders: in a block where the pre-RA scheduler's pressure counter reaches 14,
  it picks by register-pressure delta with ties in IR order, otherwise by height. One hair-mesh source gives the
  original's `z, y, add, x` in the two textured meshes (counter 14) and `z, y, x, add` in the skin mesh (counter 11
  or 12). fin-sched's probe compiler (`scratchpad/fin-sched/probe.py`) prints the counter and every choice; use it
  before searching source forms for an order.
- At a call whose argument is a load and whose other argument an address, mwcc sometimes forms the load first and
  sometimes the address, and which depends on the code around the call, not on the call: overlay 109's
  `AnimModel_FindAnimation(&mCharacters[i].mModel, ..., mCharacters[i].mModel.mAnimId)` comes out in the original's
  order alone in a test function, and in the other once a switch that reassigns a local follows it. Neither
  temporaries, inlines, parameter types nor `opt_*` pragmas change it. Not solved.
- Every argument of every call loaded last to first (`ldr r3; ldr r2; ldr r1; add r0; bl`), compares loading
  their left operand first, and an `a == 5 || a == 7 || ...` test that is a chain of materialised booleans (one
  register per `||`, all set to 1 up front) instead of a bit test: the function was compiled with the IR
  optimiser's passes off, and its calls go through inline functions. Turn the passes off together, around the
  function, with `#pragma push`, `#pragma opt_common_subs off`, `opt_propagation`, `opt_loop_invariants`,
  `opt_strength_reduction`, `opt_rotateloops`, `opt_scalarize`, `opt_rebuildlogicals`,
  `opt_collapselogicalselectionintobittest` (all `off`), then `#pragma pop`. One or two of them at a time show
  nothing; `opt_prelinearize off` is an internal compiler error. In that mode an inlined call binds its arguments
  to its parameters from the last to the first, so `static inline void NFLi_PutScale(GXDLInfo *dl, fx32 x, fx32 y,
  fx32 z) { G3C_Scale(dl, x, y, z); }` gives the reversed loads where a direct call does not, and an SDK inline
  (`MTX_Scale43`) does it by itself. With the passes on, the inlined arguments are put back in order, which is why
  inline wrappers alone never showed it. The same mode keeps dead stores to a local whose address is taken, and a
  vector's fields copied one by one on the stack (lib/NFL: `NFLi_MakeHairSkinMesh`, `NFLi_DLScale`).
- The operands of a `+` evaluated right first where the original evaluates the left first, the left one holding
  `x < 0 ? -x : x`: write the absolute value as a static inline function, `NFLi_Abs(x / 32)` (lib/NFL's mesh
  builders). Inline, the conditional seems to weigh less in the choice of which operand to evaluate first.
**Loop invariants**
- Two products of a loop-invariant sum rounded in the original's order (`RoomModels::Update`: the camera's x square
  rounded before its z square): the sum written into one local before the loop, `fx32 distSq = FX_Mul(d.x, d.x) +
  FX_Mul(d.z, d.z);`. Kept as two locals and added in the loop (`IsPartNear(xx + zz, i)`), the optimiser hoists the
  sum itself and schedules the two products' rounding in another order, whatever the order of the locals or of the
  operands.
**Loops (MiiList.cpp)**
- A search loop whose test is one predicated chain (`ldr; ldr; cmp; add; sub; cmple; addgt; bgt`), where `while
  (a > x || x > b + c - 1) n++;` is rotated: `for (;;) { count = c[n]; last = b[n] + c[n] - 1; if (!(b[n] > x || x >
  last)) break; n++; }` with `count` and `last` used after the loop (`RemoveSelected`, `Sort`).
- A row's `y << 12` recomputed each pass from `y += 20` (not strength-reduced to `+= 81920`): write
  `(position.y + i * 20) << FX32_SHIFT` (the panel's `InitButtons`, `InitTexts`, `InitIcons`).
- A by-value point's y loaded into a callee-saved register before the first call although used after it: `int y =
  position.y;` at the top (`InitScrollBar`).
- Dead copies of four sprites' positions per row in the original's slots: one of the 27 combinations of the three
  position setters, found by script (`PlaceButtons`).
**A value the original computes from the counter at each use, where ours strength-reduces it**
- Index the array with a `u32`: `gInteriorTable[style].mParts[i]` read through `static inline s16 GetPieceZ(u16 style,
  u32 i)` (or `[(u32)i]`) keeps the original's `mla i, #10` at each use, where an `int` index is strength-reduced
  into a pointer stepped by 10; it also brought the loop's three hoisted zeros into the original's registers
  (`SceneRoom::ShowParts`). `#pragma opt_strength_reduction off` gave the same `mla` but no longer reduced the
  loop's other pointer, which the original does reduce.

**Overlay 109's last register and argument-order near-misses (round 4)**
- `#pragma opt_dead_assignments off` (inside `#pragma push`/`pop`) is a setting the original used per function. With
  it, a local assigned 0 is kept rather than folded, so `int zero = 0; s = zero - 1; ...; t = zero;` gives the
  original's `mov r2, #0; sub r1, r2, #1` (`ReceiveWiiMii`); and an inline forwarder's arguments are bound last to
  first, as with the eight passes off: `FindAnimationOf(model, BCA0, model->mAnimId)` forms the id before the
  model's address (`UpdateCharacters`), `SetEmitterFlag(e, 2)` twice leaves the function's last `mov r0, #0` after
  the two `orr`s (`InitParticles`). Neither the forwarder nor the pragma alone does it.
- `#pragma opt_propagation off` with an inline forwarder does the same for a member call (`SortBy(&list, key, 0)`
  in overlay 109's `Update`: the key's offset in r0, the list's address formed through r2); a parent index read
  into a local before indexing the names gave the mla's registers there.
- A value the original loads into one register and copies into another (`ldr r1; mov r4, r1; cmp r1`): test it
  through a volatile read of the field and keep a plain local for the later use (`UpdateListWindow`). A load the
  original does first whose value is the second operand of an `orr`: read it through a volatile cast into a local,
  then `own | received` (`ReceiveCharacter`).

**Two registers holding one counter**
- A second counter, incremented in a statement of its own after the call (`int number = 0; for (int i = 0; i <= 1;
  i++) { f(number, table[i]); number++; }`), gives the original's two registers; `number++` in the argument list
  does not (overlay 76's question pages).

**A member loaded once where ours loads it again, or late**
- If the original loads a member early, stores it in another member, and uses the loaded value after the store, read
  it back from the member it was stored in. The compiler forwards the stored value and does not reload it:
  `mPrevHeld = mHeld; mTrigger = (mPrevHeld ^ held) & held;`. Using `mHeld` twice loads it again after the store.
  A local (`u32 prev = mHeld;`) loads it late, after the volatile reads (`PadInput::Update`).

**A function inlined where the original calls it**
- An inline constructor that the original's static initialiser calls out of line, emitted after the file's
  implicit destructors: `__attribute__((never_inline))` on the inline definition (`SceneMemory`, `EmptyScene.cpp`).
  A non-inline definition instead comes before the destructors, as C1 and C2.

**Branches and switches laid out differently**
- A jump table whose unlisted values go to a lone `b end` block, while the listed cases' own breaks go straight to
  the end (the Mii transfer's `MiiTransfer_UpdateSession`, case 3): the default's block held a statement when the IR
  chained jumps. mwcc retargets a label only when the first real statement after it is a `goto` (`chain_label` in
  IroJump.c and `COpt_Follow` in COptimizer.c of the decompiled GameCube compiler, the same front end): labels and
  nops are skipped, anything else stops it. A dead assignment there (`default: { int state = sSessionState; break; }`)
  is kept through those passes under `#pragma opt_dead_assignments off` and deleted by the backend, leaving the lone
  branch. Without the pragma every vanishing body (`;`, `(void)0`, `i = i`, empty loops, empty inlines) is gone
  before the chaining and changes nothing. The same pragma keeps stores into a local array nothing reads, which
  otherwise needs `volatile`. Found with the advisor agent.
- `cmp #0; beq A; cmp #1; beq B; <return>` over a step counter is a switch of two cases; `if (step == 0) ...
  else if (step == 1)` gives `cmp #0; bne`. A lone `cmp #1` left before a return with nothing using it is an empty
  `case 1: break;` (overlay 75).
- Two branches of an if/else that each end with a store to the same member and the function's return: the
  original stores in each branch and returns there; written with one store after the if/else (or as a common
  tail), the compiler merges the stores into one block with a branch. A `return;` at the end of the first branch
  keeps them apart (overlay 75's state 0x8F).
- A chain `cmp #0; cmpne #6; cmpne #5` in an order that is not sorted: a sequence of `if (x == 0) return;
  if (x == 6) return; ...`. A switch sorts the cases (or builds a jump table), and `||` turns adjacent constants into
  a range test (`MiiEditScene::Update`).
- Which operand a comparison names first decides whether the shift of an unsigned division is folded into the
  `cmp`: `if (page == mUnknown314C[0])` matched where `mUnknown314C[0] == page` folded it (overlay 76's face page
  Update).
- An if/else the original compiles as "then predicated, branch over else" is written the other way round
  (`if (i != n) x++; else ...`).
- An if/else assigning a member stores at once; a ternary lets the store be scheduled later. An if/else also gave the
  original's result register where the ternary took another (overlay 109's `Update`, the next page after a send).
- The same member assigned in both branches (`if (...) y = A << 12; else y = (A + 2) << 12;`) where a local set in
  each branch and stored after them schedules the shift and store differently (`KeyboardButton::Draw`).
- At level 2, `lost = TRUE; next = c ? a : b;` puts the `mov` of the flag between the two conditional moves, as the
  original has it; the flag after the ternary puts it after both.
- A switch with the cases in the order of the original's code: `default` first puts its code after the compare
  chain; a jump table's values that only go to the default must be written out as cases; the tutorial's cases
  first gave `MapScene::Init`'s compare chain. `case 0: case 9: case 10:` gives `cmp; cmpne; cmpne` where `||`
  gives a range test.
- `for (;;) { if (!cond) break; n++; }` where `while (cond) n++` gets rotated (the save screens). An `i < 100` loop
  that is rotated at level 2 is a `do { } while`.
- A retry loop whose "taken" case jumps straight back to the body without testing the count again:
  `for (; n < count; n++) { retry: ...; if (taken) goto retry; }` (`SceneCast::Choose`); `while` with `continue`
  or an `if (!taken) n++` re-test the count.
- Identical statements in an if and its else are merged into one: `++n` in one and `n++` in the other keeps both.
- A test whose second branch the original places first (`cmp #1; beq later; cmp #2; bne end; <case 2>`) is a
  switch with that case written first (overlay 76's voice page).
- A loop of one iteration kept as a loop: `for (u16 i = 0; i <= 0; i++)`; `i < 1` is folded away.
- A BOOL made of an `&&` chain of calls, with every failure branching to one `mov r0, #0` and the last test
  `movne r0, #1`: `if (a && b && ...) ok = TRUE; else ok = FALSE;`. `ok = a && b && ...`, or an inline returning the
  chain through a member pointer, materialise a flag per operand (`MiiEditProfilePage::SetState`).

**Independent statements scheduled in another order**
- A loop's compare scheduled after the counters' decrements in the join block (`sub; sub; cmp; add; bne`) where ours
  puts the `cmp` first: the decrements written in both branches of the if/else before it (`FormatDecimal`,
  `TextUtil.cpp`). The compiler merges them into the join block, behind the compare in its order.
- Try every order of the independent statements at the top of the function by script: one of the 24 orders of four
  statements matched NFL's `NFLi_MakeGlassesMesh` exactly, where the others moved loads and kept a value the original
  leaves in r0.
- An array walked by a pointer whose increment comes before the loop's compare is incremented in the body
  (`...; character++; }`); in the `for` header (`i++, character++`) it comes after the compare (overlay 1).
- The test scenes' light-0 block (x stored, reloaded into a local and stored again; scenes 6, 7, 16, overlays 3
  and 4), measured with the quick compiler: the compiler forwards the stored x to the read when the read is an
  inline's argument, `dir.x = dir.x`, or when y or z is stored between, and keeps the reload only for a local
  `fx16 x = dir.x;`. `#pragma opt_propagation off` around `Init` (found by ov003) gives the original's registers
  (0 materialised early in r1); overlay 4 then differs only by the stores of x and `mEnabled` swapped, and an
  exhaustive search of the block's statement orders, inline setters, global or pointer accesses and local types
  found no source with x's first.

- The test scenes' light-0 block (x stored, reloaded and stored again, the colour loaded and stored back:
  `G3dTestScene`, `HumanTestScene`, overlays 3, 4 and 5) gets the original's registers (the light in ip, the colour
  in r3, 0 in r1 before the reload) only with `#pragma opt_propagation off` around `Init` (inside `#pragma push` /
  `#pragma pop`). A store between the x store and its reload (`mEnabled = TRUE;` or y) is what keeps the reload;
  `x = x` straight after it is dropped. In overlay 3 a `const char *path` local for `ModelResources_SetModel`'s path
  gave the original's early `ldr r2` of it. Measured there too: what is left of such an `Init` is scheduling that
  flips with unrelated code earlier in the function (removing the first call, `VramManagers_Reset`, puts the virtual
  `Load` calls' argument set-up in the original's order), so search the light statements' orders last, in the
  file as it will stay.

**A value read once where the original reads it twice**
- A value the original compares and returns from one load but reads again before a read-modify-write of the same
  element (`ldrsb r1, [r4, r9]` for the tests and the result, then `ldrsb r0, [r4, r9]; add; strb` for the step): read
  it through a const lvalue and write it through the plain one, `const s8 &phase = phases[slot]; if (phase == 0) {
  result = phase; ...; phases[slot]++; }` (`Character::PlaySoundAtAnimPhase`). The compiler takes a load through a
  `const` lvalue for another expression than the same load through a non-const one, so it does not forward it into the
  step; any change of type does the same (through `u8 *` the step reloads with `ldrb`), but only the const reference
  keeps the signed load. Locals, `+= 1`, a switch and the member instead of the pointer all forward or move the
  address.
- Not solved: light 0's setup in the test scenes (overlay 5's `TestDrawMiiScene::Init`, overlay 3, three of the main
  program's test scenes) stores the direction's x (and the colour), reads them back and stores them again with y and
  z. Written so, the compiler drops the read and the second store of x, a load followed by a store of the same value;
  a volatile read or `#pragma opt_dead_assignments off` keeps both but gives the light's pointer and x other
  registers. Statement orders, temporaries' types, inlines and struct copies did not do it.
  Measured later (fix-light, round 4, about 4,000 variants compiled; still not solved):
  - The read and the second store go only when nothing is stored between them: `x = p->x; p->x = x;` back to back,
    or `p->Set(p->x, ...)` with the x store first in the setter, is deleted. With any store between (the colour,
    `mEnabled`), both stay. This holds without any pragma.
  - The original is not a struct copy. A by-value `VecFx16` or `Light` (copy, getter, temporary or by-value parameter)
    always goes through the stack.
  - `#pragma opt_propagation off` matters only for the light pointer. Inside the pragma, writing the global directly
    loses the registers again. Outside it, passing the pointer once as `this` to an inline member
    (`light->Set(x, 0, 0, color)` on a struct derived from `Light`) gives the same registers with no pragma.
  - Every non-volatile form stops at the same place. Those forms include every setter body order with and without
    `mEnabled`, every caller order, pointer and global access per statement, `VEC_Fx16Set`, and the assignment as an
    argument (`Set(p->x = 0xFFF, ...)`). Overlay 4 always comes out `mov r0,#1; str r0,[r3,#8]; strh r2,[r3]`, against
    the original's x store first. No single `opt_*` pragma, `scheduling` or `peephole` moves it.
  - Volatile accesses keep their source order. Making the colour, x and `mEnabled` stores volatile gives overlay 4
    the original's store order. The call's first argument (`ldr r0,=gModelResources`) then moves above them, so 1
    goes to r2. In DrawManTestScene, volatile reads of x and stores of y and z leave only the x store and the
    constants 0 and 0xFFF out of place. Both look like the scheduler/allocator interplay that fix-nfl also left
    open (call arguments loaded before or after a value's last use).
- A volatile cast at the one read that must stay, `*(volatile int *)&mWindows[0].mResult != -1`; the field itself
  made volatile in `MessageWindow.h` changed `PreStartScene::Ask` (overlay 76's page 0 Update).
- Two ways out of a block written as an if and its else followed by one `return`, where a `return` in each branch
  scheduled the else's code differently (`ItemTradeScene::Update`, overlay 110).
- A field the original stores and reloads at once (`str r0,[r5,#4]; ldr r3,[r5,#4]`), with the constants of the
  compares that follow loaded together before them: the compares are conditionals assigned to variables,
  `pi = x >= 0x1B33 ? 1 : 0; pj = p->y <= 0x167B ? 1 : 0;`. The conditional splits the code into blocks, so the
  stored value is not forwarded and nothing is scheduled across it; `pi = x >= 0x1B33;`, an if/else on its own line
  elsewhere, or the compare written inside the index expression all let the compiler forward and reorder (lib/NFL's
  `NFLi_MakeMouthMolePart` and `NFLi_MakeEyeBrowPart`, 7 and about 30 differing instructions to none). A static
  inline `abs` (`return v < 0 ? -v : v;`) between the store and the reads does the same.
- A sum computed twice, once for a clamp's compares and again in its else (`adds r0, r7, r6; movmi r7, #0; bmi;
  cmp r0, #11; movgt r7, #11; addle r7, r7, r6`), where every level from 1 up folds the else into the compare's sum:
  the compares read a copy of the variable and the else adds to the variable itself,
  `browRot = obj->exprFields[6]; brow = browRot; ... if (browRot + d < 0) brow = 0; else if (browRot + d > 11)
  brow = 11; else brow += d;` (lib/NFL's `NFLi_ApplyExpression2` and `3`). With one variable in both places the
  compiler turns the clamp into `brow += d` and two compares, whatever the form (temporaries, ternaries, inline
  helpers, types, operand orders). The copy must be made at the top, with the field's load: a copy made just before
  the clamp is coalesced and folded again.

**A computation kept whose result nothing uses**
- A clamp whose moves are gone but whose `adds`, `bmi` and `cmp` remain (`adds r0, r1, r0; bmi L; cmp r0, #7; L:`),
  with its operands loaded: the clamp is dead, and the function was compiled with `#pragma opt_dead_assignments off`
  (lib/NFL's `NFLi_ApplyExpression5`, which computes the eye's rotation as kinds 1 and 4 do and then passes the
  Mii's own). Without the pragma the IR drops the whole clamp; with it and the clamp written on the variable itself,
  the backend drops the moves before if-conversion and leaves `cmppl`; written in the copy form above (a copy
  assigned at the top, the compares on the original, the else adding), the moves go after if-conversion and the
  `bmi`/`cmp` stay. Level 1 keeps the compares too, but not the original's scheduling.

**A call's arguments**
- A structure argument that the original loads with one `ldm` into r2 and r3, with the next word stored to the
  outgoing slot by `ldr`/`str`, where ours copies the structure to a temporary below the stack pointer first (`sub r3,
  sp, #8; ldm; stm; ldm r3, {r2, r3}`): the callee takes an 8-byte structure and a word, not a 12-byte one. The
  compiler copies any structure argument that is split between the registers and the stack through such a temporary,
  whatever the source form (a member of a by-value parameter, a parameter, a local, a class with a constructor); one
  that fits in the registers it loads straight from its place (`FaceCast::SetTransform`, whose `FaceTransform` holds
  an x, y position and a scale, not a VecFx32).
- A constant stored next to a call's argument set-up in r3 though the call takes three arguments: the callee takes a
  fourth (read its prologue for `mov rN, r3`), and the constant is that argument, shared with the store
  (`WirelessTestScene::Init`, the clock setter `Clock_SetDateTime`).
- A pointer passed through an inline cast helper (`ToG2dChars(mParentNames[i])`) scheduled its address computation
  before a store the original makes first; the plain cast gave the original's order (`WirelessTestScene::Redraw`, at
  level 2 with `opt_propagation` off).

**Constants and arithmetic**
- Read the shift after a division's magic multiply: `smull` by 0x2AAAAAAB then `asr #2` is `/ 24`, not `/ 6`
  (`ResidentLife`'s `PickItemsWhileAway`: a share of the residents for every day the game was off).
- A random number truncated to 16 bits twice (`lsl #16; lsr #16; lsl #16`, then the `lsr` folded into the use)
  where it is stored in a `u16`: the inline returns an `int`, `static inline int Rand(u16 max) { return
  MATH_Rand16(&gRandom, max); }` (`EncounterJudge::MeetRandomPair`). A shuffle whose swap took the original's
  registers only as `Swap(a[Rand(n)], a[Rand(n)])`, an inline taking two `u16 &`, with `Rand(int max)` (Sky's
  `ShuffleCloudFrames`, `ShuffleRandomEventCandidates`).
- Two draws of the generator chained with one store of its state (`mla; mla; str`) where ours stores it between
  them: nothing that may alias `gRandom` (a store to a member) may stand between the two calls in the source. Draw
  both into locals before the stores (`SkyBirds::Start`'s y and the step of x).
- A `u8` parameter on the stack read from its slot at each use (`ldrb r3, [sp, #24]` before each call) instead of
  kept in a register: the callee takes a `u8` too, so nothing needs truncating and the compiler reads the slot
  again. Declared `int` with an `int` callee, it is loaded once; a volatile read also gives the reloads
  (`EncounterJudge::StartOuting`, passing a trouble's category and id on to `SetTrouble(u8, u8, u8)`).
- A call result compared as 32 bits (`cmp r0, #0; popgt`) where ours compares 64 (`subs; sbcs`): the callee
  returns an `int`, not an `s64` (`SavedCharacter::GetFullness`, called by `PickItem`).
- A truncation to 16 bits at a call (`lsl #16; lsr #16`) where the parameter is `u16` in the original but `int` in
  our declaration: write the cast at the call (`Sprite_Init(..., (u16)(i * 2), ...)`).
- `MATH_Rand16(...) & 3` where `% 4` adds sign fix-ups; an inline `Rand(int max)` around `MATH_Rand16` gives the
  original's registers.
- A random value in a range as an inline taking ints, `return MATH_Rand16(&gRandom, max - min) + min;`, with the
  random number first: the generator's state is stored before the range is read (overlay 76's voice page).
- `MATH_Rand16(&gRandom, max)` through an inline taking `int` (`Rand(int max)`) truncates a `u8` count to 16 bits
  (`lsl #16; lsrs #16`) as the original does; a `u16` parameter or the call made directly drops the truncation
  (overlay 39).
- `x * 5 * FX32_CONST(0.01)` keeps the shift-add and the multiplication by 41, where `x * 5 * 41` folds to 205.
- Read the literal pool's constants, not only the instructions: a tenth of fx32 written `FX32_ONE / 10` is 0x199,
  `FX32_CONST(0.1)` rounds to 0x19A (overlay 5's `TestDrawMiiCamera::SetRandomRadius`).
- `(x == 1 || x == 2)` on a `u8` gives `add #255; and; cmp #1; bhi`, where `(u8)(x - 1) <= 1` gives a `sub`.
- A `wchar_t` set from a choice of two wide constants (`fill = zero ? L'0' : L' ';`) and then compared is compared
  in its register; with `'0' : ' '` the compiler reloads it from the stack, where its address is taken
  (`__wpformatter`, `wprintf.c`).
- A step assigned back to its own variable (`snum = (snum >> 21) & mask; exp = snum - 1023;`) keeps the
  intermediate in the original's register; computed straight into `exp`, the same instructions get other registers
  and another order (MSL's `double2hex`).
- A parameter loaded before an early return, though used only after it, is a copy made first (`ld = num;` at the
  top, and `ld` passed on) (`double2hex`).
- A constant compared from a register (`mov r0,#14; cmp r2,r0`) is a variable assigned the constant elsewhere,
  here in one state of a loop (`max_digits = 14;` when `__strtold` reads "0x"). A literal, a `const`, a
  `static const` or an initialised local all give `cmp r2,#14`.
- A `clz` whose result nothing uses (`ldr r1,=8; clz; rsb #31; cmp #0; bgt ...`, and a second `clz` on the other
  branch) is an SDK inline built on `MATH_CountLeadingZeros`, called for nothing: the compiler drops the arithmetic
  around it but keeps the inline assembly. `MATH_QSortStackSize(8);` as a statement gives it exactly (overlay 1's
  `NameSortTestScene::Init`).
- A shift to fx32 and back that the original keeps (`ldrh; lsl #12; asr #12; lsl #16; lsrs #16`, before
  `MATH_Rand16` reads its state) goes through an `int` variable: `int frames = (numFrame << FX32_SHIFT) >>
  FX32_SHIFT;` then `MATH_Rand16(&gRandom, frames)`. Passed straight to the call, as
  `FX_Whole(NNS_G3dAnmObjGetNumFrame(...))` or any other spelling, the shifts fold away (overlay 2's `Init`;
  overlay 74 has the same).
- A point passed by value in two registers with a dead copy of it on the stack (`sub sp,#8`, `str r1,[sp]`,
  `str r2,[sp,#4]` beside the call) goes through a `static inline` that takes the struct by value and passes its
  fields (`SetTouchLine(ScreenPoint point)` in overlay 2's `Update`). Passing the struct, or a local copy of it,
  to the function directly gives `ldm` or other registers.
- An `s64` constant built from a small constant held for another store (`mov r2,#1; sub ip,r2,#2; sub
  r3,r2,#0x80000002` for 0x7FFFFFFFFFFFFFFF): the compiler derives both halves from the constant assigned in the
  statement just before the `s64` one, so order the stores to make that the right one (`mTroubleState = 1;` last
  before `mTroubleEnd = ...;` in overlay 108's `TutorialStepScene::StartLoading`; three orders tried derived from 9,
  0 and 1).
- A counter incremented in a register and masked (`addcc r1,#1; andcc r1,#0xFF; strbcc`) and then switched on
  without a reload is a `u8` local: `u8 step = g.mStep; if (step < 24) { g.mStep = ++step; } switch (step)`.
  Incrementing the global and switching on it reloads it (overlay 108).

**Classes, copies and tables**
- A class whose constructor stores only another class's tables holds that class as a member rather than deriving
  from it: deriving makes the compiler emit its own table and destructors (`RoomDoorModel` in overlay 74, the
  character models).
- A `VecFx32` returned by value and stored in a member: `mEye = GetEye(...);` copies it field by field from the
  call's return slot, where `VecFx32 eye = GetEye(...);` then the fields makes a second copy and a larger frame. A
  vector member assigned through a pointer member, `mCamera->mPosition = mEye;`, loads the pointer once per vector;
  three field assignments reload it after each store (overlay 5's `TestDrawMiiCamera::Update`).
- A class's implicit assignment copies a base class field by field and a member structure in one block (`ldm`/`stm`
  loop). A copy that moves the first 0x2A4 bytes as a block and then `SavedCharacter`'s own fields is that of a class
  holding the record as a member: `MiiList.cpp` copies through a mirror structure (`SavedCharacterCopy`).
- A three-byte structure copied through stack temporaries (loaded, stored to a temporary, the signed byte read
  back) goes through static inline accessors taking or returning it by value (`GetPairA(i)`, `SetPairA(n, pair)` in
  `RemovePairsOf`).
- Record copies: `(CharacterRecord &)dst = src` (overlay 109). A copy of a `WMBssDesc` is field by field; a struct
  wrapping one is copied with `ldm`/`stm`.
- Bit fields are allocated from the low bit, so a structure laid out high bits first (the Wii's `RFLCharData`) is
  declared in reverse (`lib/MiiData`).
- `NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(...))` rather than writing the field directly.
- A literal copied byte by byte (`ldrb`/`strb` pairs) is `memcpy(p, "inf", 4)`; `strcpy` stays a call (MSL's
  `double2hex`, `float2str`).
- Pitfall: a zero-length padding array in a class (a `sizeof` that came out 0) made mwcc drop a member array's
  `__cxa_vec_cleanup` from the implicit destructor without a word (overlay 76).

**Constructors**
- Constructors called out of line from a function that would inline them are defined out of line (the scene
  factory's constructors, `src/scene/SceneManager.cpp`): an inline constructor in the class gets inlined into the
  caller, however many cases it has.
- An array of a class with a destructor is constructed by `__cxa_vec_ctor`; without one, by a loop. A loop that runs
  once is an array of one (`MessageWindow mWindow[1]`); a single member gets no loop.
- Members that the original addresses from the end pointer of the loop before them (`add r0, r5, #0x174` with r5
  the loop's end) are members of a part of their own that starts there; as plain members they are addressed from
  `this` (`Scene74Part`, `RankingPart`).
- A class whose table is another file's needs a non-inline virtual function declared (its key function), or a file
  that constructs it emits its table too (`Scene75Model::Draw`). A class with nothing virtual of its own gets its
  table emitted where it is constructed (`TestNg134Model`).
- Two byte-identical constructors one after the other are C2 then C1: members and array elements use C1, a base
  subobject C2 (`HeadIcon`, `ParticleSystem`, `CharacterModel`). Measured on test files: members, array elements,
  locals, `new` and static objects always call C1. So a C2 call where the layout has a member is a member of a class
  derived from that one with an implicit constructor (the derived class's constructor is inlined and calls the
  base's C2): the room scenes' head icons are `RoomHeadIcon : HeadIcon` (overlays 73, 74), and the main program's
  RoomScene constructor builds its particle system and speech player the same way. A C2 call at offset 0 of a part
  that has fields before it in the class is a second base (`Mannequin : MannequinHead, CharacterModel`).
- A stand-in that derives from a polymorphic class where the real one holds it as a member makes the file emit a
  weak virtual table the original does not have, and that object is on the data list: the size sort then puts the
  file's other data in another order. Overlay 73's `.rodata` constants had been ordered around such a table and had
  to be ordered again when `CharacterModel` got its real shape.

**Scenes**
- A scene's virtual table symbol goes at the table's start, and the main program's constructor's reference to it is
  `to:<table> add:0x8 module:overlay(<n>)`.
- A constructor of an overlay's class called from the main program's scene constructor is emitted as C1 and C2; the
  link keeps the one called, so the unit can still be complete.

**Moving an overlay onto the shared headers** (overlays 73, 74, 96, 109 and 110, measured)

- Work by comparison with the overlay's own compile from before the change, not with the original: compile the
  file once before (the quick compiler is enough), then after each step compare every function
  (`tools/compare_object.py new.o old.o`) and the bytes of `.data`/`.rodata`/`.bss` (`objdump -s`). Functions
  the new compile emits in addition (copies of inline members such as a character's D1) are the main
  program's in the original; list them in `inline_symbols.txt`.
- Keep the main-program declarations that have no shared header in a `MainProgram.h` beside the scene that
  only the `.cpp` includes; the scene's header must compile in `SceneManager.cpp` next to every other scene's.
- A member that a stand-in had (`mModel`) and the shared class has as a base: a pointer converted to the base
  gets a null test (`adds; addne`) where `&x.mModel` had none. Convert a reference instead
  (`CharacterModel &model = *c; return &model;`), and keep an inline accessor where the stand-in had one (the
  offsets are then not folded across it, as before).
- Names the derived class also declares hide the base's: `mRoom.mParts.SetState(...)` becomes
  `mRoom.RoomModels::SetState(...)`, not `mRoom.SetState(...)` (which is `ApartmentRoom::SetState`), and the same
  for fields (`RoomModels::mState`).
- Inline members that a stand-in had and the shared class lacks go into the overlay's own derived class or a
  static inline in `MainProgram.h`; adding inline members to a widely included shared header renumbers the
  `@` literals of every complete unit that includes it (a declaration with unnamed parameters does not).

**Tools to mind**
- A `.bss` object named `kind:data(...)` in `symbols.txt` (what `name_unit_data.py --apply` writes) makes `dsd dis`
  read bytes the module does not have and panic (`dis.rs:222`, `Option::unwrap()` on `None`; overlay 110's
  `sPartners`): `.bss` symbols are `kind:bss`.
- `tools/name_unit_data.py` reads only what `readelf` lists as OBJECT or NOTYPE: run it after any change to a
  complete unit or to a header it includes, since the compiler's `@` numbers move.
- A constructor emitted as C1 and C2 (no virtual base) has its string literals twice, the C2 copy first; the link
  strips C2 and its strings. `name_unit_data.py` lays both copies out, so name the strings of the C1 copy
  (`Keyboard.cpp`: the second nine `@` numbers).
- dsd puts a symbol on every address the code loads, so a table whose fields' addresses are formed apart (NFL's
  colour tables and planes, `sNFLFacePlanes[j][i].d`) shows as one object per field, and a source that indexes the
  table refers to the first with an addend. Remove the inner symbols from `symbols.txt` and write the references in
  `relocs.txt` as `to:<table> add:<offset>`; `check_relocs.py` is then clean.
- `tools/strings.py` has named some number tables as text (`gText_Ri`, `gStr_F`, ...). A "string" in a table is
  data; rename it when the code shows what it is.
- MSL's files are best checked against the archive's own object: `tools/compare_object.py build/.../x.o
  <member>.o` compares the functions of the same names, and `readelf` shows its data and relocations. A checkout
  configured before the fix in `download_msl` has the archive under `tools/bin/msl/` with every byte from 0x80 up
  turned into `?`, unreadable; delete `tools/bin/msl` and run `configure.py` again. Two members are both named
  `critical_regions` (names are cut to sixteen characters): extract by position, not by name.

**MSL**
- A loop that the original enters without testing its pointer is a `do`/`while` (`__flush_all`).
- A copy loop written `for (; i < ie; ++i, ++ib) *ib = *i;` and `*ib++ = *i++` compile to the same instructions but
  colour differently, and the difference reaches back over the whole function: in `__minus_dec` the second gave five
  pointers a rotation of the original's registers, which no declaration order undid (`ansi_fp.c`). Search a tail's
  forms by script (loop form, which pointer advances, whether a pointer is set again after) when the registers of an
  early part change with it.
- A double whose bits are read through a cast (`*(unsigned long long *)&frac`) lives in memory and is reloaded for
  the next call; the original kept it in registers and cast a copy (`__num2dec_internal`).
- A bitfield struct copied to a local (`__file_modes mode = file->mode;`) is given a stack slot; reading the field
  through the pointer again reuses the loaded word, as the original does (`__get_char`).
- For quick looks at code, a file compiles in about a second under `~/Projects/pokesxf/tools/wibo/wibo-macos-1.2.0`
  with the build's own command, `sjiswrap.exe` and `-enc SJIS` left out, and metroskrew's copy of the same compiler
  (`mwccarm-3.0-138.exe` for the game, `-3.0-137.exe` for the Mii transfer), against about twenty under wine. The
  code is the build's, byte for byte (measured on overlays 72, 74, 96 and 110 and the Mii transfer), but objects of
  one size can come out in another order, so data offsets, and with them a match, are for `ninja` to say.

**Found on DS Protect (`lib/DSProtect`)**
- In a function with inline assembly the compiler gives a variable live across an `asm` block its initial value with
  an `ldr` from the literal pool (`int r = 0;` gives `ldr r4, =0`), and does not fold a shift of a variable that holds
  a constant: the original's `ldr r7, =1; lsl r7, r7, #26` and `mov r4, #1; lsl r4, r4, #15` are `u32 base = 1;
  base <<= 26;` and `one << 15` with `u32 one = 1;`. Neither the `#pragma opt_*` switches nor the optimisation level
  changed anything in such functions.
- Data emitted by `dcd` in an `asm` block gets a `$d` mapping symbol, so objdiff shows it as data; a word dsd delinks
  inside a function without a mapping is shown as an instruction. The two cannot compare equal in objdiff even when the
  bytes are.

**Found on the character code (`src/character/`)**
- A chain of assignments, `mMoveAngle = mAngle = mTargetAngle = mWalkAngle = 0;`, gives the original's order of the
  stores and of the constants' registers where one statement per field schedules them otherwise (`Character::Init`,
  `CharacterBase::ResetFace`, `CharacterHead::Init`: `mPrevAnimId = mAnimId = -1`).
- Two dead copies of globals passed to a function by pointer are the by-value parameters of an inline overload:
  `BOOL HitTestJoint(VecFx32 origin, VecFx32 dir, int joint) { return HitTestJoint(&origin, &dir, joint); }`.
- `VEC_Set(&r->scale, s.x, s.y, s.z)` loads the three in reverse and stores them with one `stmib`, where a structure
  copy or three assignments do not (the model's joint callback).
- A template the file uses keeps an unused parameter only when it is not `static`: with `static` and `-ipa file`
  the compiler drops the argument at every call (`ClampField` in `SavedCharacter::Validate`, whose name strings
  otherwise vanish from the literal pools).
- `FX32_CONST(variable)` is a floating-point conversion; shift instead.
- A compare against a `u8` field or a `MATH_Rand16` result done signed in the original (`ge`, `lt` where ours has
  `cs`, `cc`): the value went through an `int` first, `int level = mSatisfyLevel;`, or an inline
  `static inline int Rand(int max) { return MATH_Rand16(&gRandom, max); }`; the same inline also gave the original's
  registers around the generator in five functions (`SavedCharacter.cpp`).
- Three byte loads given the scratch registers in another order: a field extracted as `(b & 0x0F) << 12` where
  `(u16)(b << 12)` gives the same instructions with the loads defined in another order (`GetCreateIdNumber`).
- A result kept in r1 where ours puts it in r0, with r0 unused afterwards: the function returns the value r0 holds
  (`GetCreateIdParts` returns the number it splits).
- `sum *= 1000; sum /= 32767 * 3;` where `sum = sum * 1000 / (32767 * 3)` schedules the whole function differently
  (`GetBiorhythm`, `UpdateBiorhythm`).
- A cascade of `x == 6 || x == 13 || x == 14` becomes a range test for 13..14; the original's `cmp; cmpne; cmpne` is a
  switch. The other way round, a switch over 1, 2, 4 gives a compare chain where the original's bit test
  (`sub; cmp #3; lsl; tst #11`) is `int state = mTroubleState; BOOL b = state == 1 || state == 2 || state == 4;`.
- An `if (...) return TRUE; return FALSE;` the original compiles with branches and two `mov r0` returns: a switch of
  one case (`switch (kind) { case 18: return TRUE; } return FALSE;`); for `ClampField`, an if / else if / else with
  one `return FALSE` after it.
- Byte arrays are never pooled, but a two-dimensional one is: `s8 sAnswerScores[5][9]` sat in the `.rodata` pool
  among the 16- and 46-byte tables, where five `s8[9]` came out before the pool.
- A switch of hundreds of cases (`CharacterSort::GetCollationKey`) can be written from the original's compare tree
  by running the tree for every input value (a small interpreter of its `cmp`/branch/jump-table code) and listing one
  case per body in the bodies' address order; the compiler then rebuilds the same tree.
- A chain of hundreds of compares against character constants, each constant hoisted out of the loop and spilled
  (`SplitSyllables`, `TextUtil.cpp`: 320 spill slots, a 0x508-byte frame): read the chain's order off the original
  by tracking which spill slot holds which constant (the prologue computes them all from one base, `0x30A3`, by
  `add`/`sub`), and write one `||` condition per length in that order. The compiler hoists and spills each occurrence
  as the original did, so the frame comes out right with no further work.
- Functions whose body is a table of character conversions are best understood by running them in an emulator over
  every character (unicorn, on `extract/.../arm9.bin`): that is how the normalization steps were named.
- A unit whose constructor builds an `AnimModel` member after another member with an out-of-line constructor emits
  `~AnimModel` (binding 13), though nothing in it calls it; see "Functions emitted in every file that uses them".
- The order of a file's static `char` arrays (the joint names) follows no declaration order of the functions that use
  them; declaring them at file scope in an order found by one compile (read which slot each lands in, give each slot
  the ROM's name) placed all twelve.

### Evaluation order and colouring rules measured on overlay 75's pages

Each was found on one function and checked on a second where the shape recurs:

- **An inlined call binds its arguments last to first, and a member call binds `this` last.** `PlaceMannequin`
  sets the body's size per axis through an inline: with the mannequin pointer as the helper's *last* parameter the
  pointer is read before the size is computed, as in the original; as a member function (`this` first) or with the
  size computed into a local first, the table is read before the pointer. The by-value position copy is the same
  helper style (`SetBodyPosition(Mannequin *, VecFx32)`).
- **`a | (b != 0)` with calls compares as soon as it can.** Written as one expression, the first call's result is
  compared before the second call; the original keeps the raw value across the call and compares after: the values
  are assigned to locals first (`Scene40ListTabs::UpdateAvailable`).
- **A switch gives branches where if/else gives conditional moves**, for three cases that each assign a value
  (`UpdateRelationAnswer`); and a switch case that only re-assigns the default keeps its own branch in the jump
  table (`UpdateTroubleAnswer`).
- **A redundant `else if (x >= N)` after `if (x < N)`** is the source of a conditional return (`poplt`) right after
  the branch target: the compiler reuses the first compare for the second test (`Scene40CallButton::UpdateCall`).
- **Register colouring follows variables, not values.** A second loop with a counter of its own colours its two
  registers as the original; reusing the first loop's counter swaps them (`Scene40OptionPage::UpdateDone`). A table
  read twice, rather than named in a local, gives the original's registers (`Scene40MenuBar::Init`).
- **A u16 field set from an int is cast**: `mClothesId = (u16)mChosenId` gives the original's `lsl #16; lsr #16`
  before the bitfield insert.
**Found on overlay 75's character, meters and pages (ov075-c, round 5)**
- A clamp whose instructions are right but whose registers are rotated is the SDK's `MATH_CLAMP` (a nested
  conditional expression whose result is a fresh temporary, so the loop's hoisted constants become its operands),
  where an if/else on the variable itself colours them otherwise (`Scene40SegmentMeter::Show`, found by the advisor).
  Try `MATH_CLAMP`/`MATH_MIN`/`MATH_MAX` before permuting declarations.
- A third test of an if/else-if chain on the same two values: `else if (mTarget < mValue)` keeps a compare of its
  own, `else if (mValue > mTarget)` reuses the flags of the test before (`popge` after `bge`) (`Scene40Meter::Update`).
  With one operand read through a pointer (`*mCounter`) the flags are reused either way.
- A member array read through a local pointer to it (`MtxFx43 *matrices = mJointMatrices; matrices[3]._30`) keeps
  the array's address apart from the element's offset (`add 0x860; add 0x2000; ldr [#0xbc]`), where indexing the
  member folds both into one offset (`Scene40Character::PlaceHeadIconAt*`). An inline accessor returning
  `&mJointMatrices[joint]` folds as well.
- One counter declared at the top and used by two loops keeps, in the second loop (which calls nothing), the
  callee-saved register the first loop needed; a fresh `int i` per loop gives the second a scratch register
  (`Scene40ClothesList::LoadOwnedBy`).
- A value added from one of two constants chosen in an if/else (`lineHeight = 28; ... else lineHeight = 14;
  height += lineHeight;`) hoists both constants into registers and moves one into a temporary, as the original;
  `height += 28` in each branch does not (`Scene40ProfilePage::FindRelations`).
- A by-value `VecFx32` setter that copies its parameter into a local before storing the fields gives three stack
  copies (the call's result, the parameter, the local), as `Scene40Character::ShowBed` has them.
- mwcc calls a direct member object's virtual function through its table (`ldr r3, [r0]; ldr r3, [r3, #4]; blx`):
  it does not devirtualise, so a member whose calls go through a table is still a plain member, not a pointer.
- A `while (x < table[i] && ++i < 7) {}` or `for (;;) { if (x >= table[i]) break; if (++i >= 7) break; }` search
  gives the original's predicated loop (`addlt; cmplt; blt`) where `while (x < table[i]) { if (++i >= 7) break; }`
  does not (`Scene40ProfilePage::GetMeterGrade`).
- Booleans materialised again and again (`cmp; moveq #1; movne #0; cmp #0; beq`, then after a call `movne #1;
  moveq #0` twice, then once more at the join) are nested inline functions returning BOOL, each `&&` inside one:
  `HasText(t) { return wcslen(t) != 0; }`, `HasMotto(c, mood, t) { return c->mMood == mood && HasText(t); }`,
  `WillSayMotto(...) { return HasMotto(...) && Random(100) < chance; }`, called as `if (WillSayMotto(...))`. The
  same conditions written inline in the `if` give branches only, and the eight IR passes off change nothing there
  (`Scene40Character::UpdateStates1ATo1C`, 0x02163758, matched so).
- A global's address loaded at a function's entry and kept in a callee-saved register for one late use is a local
  pointer to it at the top (`Camera *camera = &gCamera;`); and a local `FloorPos` whose dead copies sit above a
  block's locals is declared at function scope, not in its case (the same function).
- A text button's (or a page's) text cleared and drawn through inlines taking the button keeps a pointer to the
  button, with the field stores in between made through the indexed member (`ClearButtonText(&mButtons[i]);
  mButtons[i].mText.mMessage = m; DrawButtonText(&mButtons[i]);`, `Scene40ChoiceList::SetChoices`).

**Rules measured on overlay 75's character code (ov075-e)**
- Animation-by-name calls (`m.SetAnimation(type, ResourceSet_FindIndexByName(m.mUnknown04, type, "name"))`) match
  only through an inline member `PlayAnimation(type, name)`: an inline binds its arguments last to first, so the
  name's literal is loaded and pooled before the type. Two characters handed to the next scene loaded in reverse
  order came from the same rule (an inline `PrepareTogetherScene(first, second, mode)`).
- A range test on a `u8` field (`id == 2 || id == 3`) is done in u8 arithmetic (`add #254; and #255`); the
  original's `sub; cmp; movls` came from an inline accessor returning the field as an `int`.
- A BOOL-returning inline (`IsInTroubleOfKind`) materialises the BOOL (`moveq #1; movne #0`) where the original
  tests the condition directly.
- An extra field-by-field copy of a vector before a by-value call came from a function cut down from a sibling
  (`middle = rightPoint;` left over from the two-hand version of `PlaceHeldItem`).
- `x = gPersonalityKinds[x]` (a variable reassigned in place, or an inline converting its parameter in place) gives
  a constant-column table lookup the original's registers: a second definition keeps a local's virtual register as
  a second use does (found by the advisor).
- Ending a state's case with an if/else instead of early returns sends a switch's default to the function's common
  exit, as the original's.
**Found on overlay 75's scene and pages' controller (ov075-a, round 5)**
- **`T *const p = &gGlobal;` keeps the global's address; `T *p = &gGlobal;` is propagated.** A plain local pointer
  to a global becomes one literal per field address (`gCamera+0x2C`, `gCamera+0x20`); declared `* const`, it is
  one literal loaded at the top, held in a callee-saved register across calls, the fields formed by `add` or by
  the load's offset. Measured three times: `ProjectTouch` / `ProjectTouchBetween` (the camera), `SetGameFlags`
  (`GameData *const game`: one base for the if-converted orr/bic and the store) and `TakeGameResult` (the scratch).
  `#pragma opt_propagation off` keeps the base too but recolours other code in the function, so it is not the
  original's form; overlay 110's camera setters, which use the pragma, are candidates for the `const` form.
- **A struct assignment of a char-array struct** (`struct ArchiveName { char mName[4]; }; name = sTemplate;
  name.mName[2] = '0' + i;`) gives in-order `ldrb` of the template and `and rX, rY, #255` copies held in registers;
  char arrays copied element by element reload with `ldrsb` instead (`Scene40::StartLoading`). The destination
  declared before the template takes the higher stack slot.
- **A zero vector declared inside a loop body** re-stores the stack temporary every iteration and propagates only
  `.x` into the destination store; declared outside the loop, the stores are hoisted (`Scene40CharacterTouch::
  Clear`). A local `const VecFx32 up = {...}` is copied from `.rodata` with `ldm`/`stm`; a file `static const` of
  the same value is folded into immediates.
- **Locals are given stack slots by declaration order** (the first declared at the highest address): eight vectors
  declared in the original's slot order matched `ProjectTouch` at once.
- **A sparse switch needs its empty case to keep the jump table**: cases 0, 1 and 3 compile to compares, with
  `case 2: break;` to the original's four-entry table (`Scene40Pages::UpdateOptionPage`, `UpdateKeyboardPage`).
- **The scene's constructor is a probe of the original's member nesting.** Scene40's implicit constructor
  (main 0x020428E0) inlines every member's; with ItemListView's list view a member of the pages' controller
  directly it recolours the inlined loops (99.8%), flat in the scene or inside a list-page class it matches. Each
  class introduced over a range of members was checked so; the destructor (ov075 0x0217B040) matches with the
  final nesting.
- **A member's address is rematerialised, pointer arithmetic is not**: `&this->mPage` is formed again at each use,
  `(u8 *)this + 0x2A49C` is kept in a register (`Scene40Pages::Init` before its pages had classes).

## Functions emitted in every file that uses them

Measured (fix-link, test files under the game's compiler and flags, and the ROM):

- An **inline function** (a member defined in its class, an implicit or inline destructor or constructor) is emitted
  by every file that needs it, after the file's last function, under mwccarm's symbol binding 13 (readelf: "processor
  specific"). The linker keeps the copy of the first object in link order and drops the others, so the ROM has one
  copy, in the first file that emitted it. A virtual destructor written in the class comes out as D0 (deleting) then
  D1 (complete), the derived class's pair before its base's, with the base's body inlined into D1 and no D2:
  `AnimModel` D0 `0x0202AF54`, D1 `0x0202AF98` (0x3C bytes), `Model` D0, D1 after the last function of the 3D
  models' file. That is what an inline destructor looks like in the ROM.
- A **non-inline** destructor (`AnimModel::~AnimModel() { ... }` in a `.cpp`) is emitted once, global, at its place
  among the file's functions, as D1, D0 and D2, and a derived class's destructor calls the base's D2 instead of
  inlining it. So declaring an inline destructor non-inline in a header, to stop units emitting it, changes every
  derived destructor: it is not the original's source.
- dsd delinks every symbol as global. A complete unit whose object defines such a function (binding 13) then fails to
  link with "Multiply-defined" while the copy the game kept is in a unit that is not complete (the gap, or an
  incomplete file's delinked object): the linker takes global plus binding 13 as two definitions, and a weak (`STB_WEAK`)
  original makes it keep the new copy instead. **Fix**: name the function in `config/<version>/arm9/inline_symbols.txt`
  (with the address and file of the game's copy); the delink step (`tools/inline_symbols.py`) gives it binding 13 in
  the delinked objects, as the original object had, and the linker keeps the game's copy. Do not declare the
  function out of line and do not make the unit incomplete for it. Once the holder of the game's copy is complete
  the entry is harmless.
- The same applies across modules: an overlay's copy of `SceneCharacter::~SceneCharacter()` (overlay 39) against the
  copy `src/scene/SceneManager.cpp` emitted at `0x0203E188`, and to any inline member, not only destructors
  (`ClothesItem::GetModelName`, `0x0204AD20`, which `CharacterModel.cpp` emits too).
- So: before marking a unit complete, `readelf -s` its object for binding 13 functions; for each that the ROM has
  elsewhere (an earlier file), add the name to the list.

## A match is instructions and references

objdiff compares instructions, and counts a call or an address load as equal whatever it refers to. So after `ninja`,
run `python3 tools/check_relocs.py`: it compares, function by function, the symbol each relocation of the compiled
object refers to with the original's. What it reports is one of:

- **A source that calls something by an address name** (`func_ov074_0216a514`) that has since been named: declare and
  call it by its name.
- **A placeholder in `symbols.txt` that the compiled code names**: when a matching function of the SDK refers to
  `sDispMode` where the original has `data_02119574`, that is the symbol's name. 197 data symbols of the libraries
  were named this way.
- **A constructor or destructor variant named wrongly**: one copy survives in the ROM, and the call shows whether it
  is the complete (`C1`) or the base (`C2`) one.
- **A real mistake**: the wrong function or object.

The `@number` names of the speech engine's literals differ by design and can be ignored.
