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

Placing that data is the next step for the library files, and is not done. `tools/place_data.py` works out where a
file's data is from the addresses in its matched functions, and reports it, but what it writes is not usable yet:
see its header.

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
- `tools/name_unit_data.py --apply` writes `kind:data(any)` for `.bss` objects and can leave a symbol twice: make
  them `kind:bss` and check `symbols.txt` for duplicate addresses afterwards.

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
function the fetched release lacks) and `spi/src/pm.c` (a later release, see binary.md, "Libraries").

The fetched file's other definitions stay. A static that only a removed or replaced function used is no longer
emitted, which matters: with `-ipa file` the statics are pooled, so the file's pool is only right once every function
that uses one of them is the game's. Remove what the game's release does not have.

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
  into `symbols.txt` (the save screens, overlays 69, 106 and 107, were completed with it). A constructor's string
  literals come twice, once for each constructor the compiler emits (`C2`, then `C1`); the ROM keeps only `C1`'s,
  the second set, which the tool does not know: name those by hand (`Keyboard.cpp`).
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

## Matching techniques

What made near-misses match, by symptom, with where each was found. Most are about the order in which the compiler
sees things: declarations, statements, and how an address is formed. Work by script: compile one object with source
variants and compare one function (`tools/compare_object.py --diff`), then confirm with `ninja report`:
`compare_object.py` masks the bytes of calls and has called a function equal that objdiff did not.

**An address reloaded at each use, where the original keeps it in a register**
- Write through a pointer made by a cast: `GXRgb *colors = (GXRgb *)data_0213bab0;`. A plain local pointer to the
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
- Slots follow the order of the caller's locals; an inline's locals and by-value copies sit below them. Declare them
  in the caller to move them up (`MapScene::Update`), or use an inline to move a copy down (`OptionScene::OpenWindow`).
- A struct made only to pass by value costs a slot the original lacks: pass the `static const` itself (overlay 109's
  `SetUpWindow`, `SetUpList`). Where the original has the slot, make the local and pass its address, point first
  (`ScreenPoint point = *touch; ScreenRect rect = GetAmPmRect(); func_02028d88(&rect, &point);`,
  `OptionScene::Update`).
- Two dead copies of a vector are the by-value `FxVec2` of `SetPosition(FxVec2(x, y))`; which of
  `SetPosition`/`SetPositionFromVec` is called decides their order. All three forms (`SetPosition(FxVec2(...))`,
  `SetPosition(x, y)`, `SetPositionFromVec(x, y)`) give different store orders or slots, and overlay 76's face page
  uses each in a different loop of one function: try all three.
- A `static const` point read field by field is folded into immediates; the original loading it from the pool, with
  three dead pairs, is the point passed by value to an inline that does the shift and `SetPosition`
  (`SetCanvasPosition` in overlay 76's `MiiEditScene::Init`).
- A local copied inside a loop is hoisted before it and takes the original's slot; declared before the loop it gets
  another (the main program's list scenes).
- Spill slots of a function too large to keep its variables in registers (MSL's `__sformatter`, 3.4 KB) follow the
  order of their declarations, and a 64-bit variable can be given two slots apart. One variable that is saved in two
  unrelated places got two slots where the original had them elsewhere: two variables, one per place, matched. A
  script that permutes the declarations and counts the differing lines gets close fast; the rest took that split.
- A constant vector copied through a local and stored field by field (`VecFx32 target = sConst; mTarget.x =
  target.x; ...`) where a by-value setter, or the same code in an inline, gave three vector copies one another's
  slots (`WeddingCamera::Reset`, overlay 96).
- A value used once, taken straight off a call's result (`f().LengthSq()`) rather than through a local: its copies
  become temporaries below the function's locals, which let a later block's local take the slot above them
  (`RoomScene::StartMeeting`, overlay 74).
- Not solved: `MiiEditProfilePage::SetState` (7 KB) keeps the WindowPoint copies passed by value to its windows'
  buttons in six slots below every other local (two of them copies of one size, one never read). Fresh copies per
  window move them to the bottom but in another order and without the unread one; reusing one copy across windows
  gives it a slot among the function's own locals. A search over 128 combinations of fresh and reused copies and over
  the declarations' places did not find the original's order.

**Registers allocated differently**
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
- A loop counter and a value it is compared with in the original's registers: `int i; int choice;` at the block's
  top and `for (i = 0; ...)`, where `for (int i ...)` swaps them (the four choice loops of
  `MiiEditProfilePage::Update`).

**Fields read off an element's address, where ours folds the offsets**
- At optimisation level 2 the compiler stops folding offsets through inline calls: `GetSelectedResident()->
  GetIndex()` reads `mIndex` off the element's address, as the original does (overlay 109's `UpdateSending`).

**Arguments evaluated in another order**
- At a call whose argument is a load and whose other argument an address, mwcc sometimes forms the load first and
  sometimes the address, and which depends on the code around the call, not on the call: overlay 109's
  `AnimModel_FindAnimation(&mCharacters[i].mModel, ..., mCharacters[i].mModel.mAnimId)` comes out in the original's
  order alone in a test function, and in the other once a switch that reassigns a local follows it. Neither
  temporaries, inlines, parameter types nor `opt_*` pragmas change it. Not solved.
**Two registers holding one counter**
- A second counter, incremented in a statement of its own after the call (`int number = 0; for (int i = 0; i <= 1;
  i++) { f(number, table[i]); number++; }`), gives the original's two registers; `number++` in the argument list
  does not (overlay 76's question pages).

**A function inlined where the original calls it**
- An inline constructor that the original's static initialiser calls out of line, emitted after the file's
  implicit destructors: `__attribute__((never_inline))` on the inline definition (`SceneMemory`, `EmptyScene.cpp`).
  A non-inline definition instead comes before the destructors, as C1 and C2.

**Branches and switches laid out differently**
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
- Try every order of the independent statements at the top of the function by script: one of the 24 orders of four
  statements matched NFL's `NFLi_MakeGlassesMesh` exactly, where the others moved loads and kept a value the original
  leaves in r0.
- An array walked by a pointer whose increment comes before the loop's compare is incremented in the body
  (`...; character++; }`); in the `for` header (`i++, character++`) it comes after the compare (overlay 1).

**A value read once where the original reads it twice**
- A volatile cast at the one read that must stay, `*(volatile int *)&mWindows[0].mResult != -1`; the field itself
  made volatile in `MessageWindow.h` changed `PreStartScene::Ask` (overlay 76's page 0 Update).
- Two ways out of a block written as an if and its else followed by one `return`, where a `return` in each branch
  scheduled the else's code differently (`ItemTradeScene::Update`, overlay 110).

**Constants and arithmetic**
- A truncation to 16 bits at a call (`lsl #16; lsr #16`) where the parameter is `u16` in the original but `int` in
  our declaration: write the cast at the call (`Sprite_Init(..., (u16)(i * 2), ...)`).
- `MATH_Rand16(...) & 3` where `% 4` adds sign fix-ups; an inline `Rand(int max)` around `MATH_Rand16` gives the
  original's registers.
- A random value in a range as an inline taking ints, `return MATH_Rand16(&gRandom, max - min) + min;`, with the
  random number first: the generator's state is stored before the range is read (overlay 76's voice page).
- `x * 5 * FX32_CONST(0.01)` keeps the shift-add and the multiplication by 41, where `x * 5 * 41` folds to 205.
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

**Classes, copies and tables**
- A class whose constructor stores only another class's tables holds that class as a member rather than deriving
  from it: deriving makes the compiler emit its own table and destructors (`RoomDoorModel` in overlay 74, the
  character models).
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
  subobject C2 (`HeadIcon`, `ParticleSystem`, `CharacterModel`).

**Scenes**
- A scene's virtual table symbol goes at the table's start, and the main program's constructor's reference to it is
  `to:<table> add:0x8 module:overlay(<n>)`.
- A constructor of an overlay's class called from the main program's scene constructor is emitted as C1 and C2; the
  link keeps the one called, so the unit can still be complete.

**Tools to mind**
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
- A switch of hundreds of cases (`CharacterSort::GetCollationKey`) can be written from the original's compare tree
  by running the tree for every input value (a small interpreter of its `cmp`/branch/jump-table code) and listing one
  case per body in the bodies' address order; the compiler then rebuilds the same tree.
- Functions whose body is a table of character conversions are best understood by running them in an emulator over
  every character (unicorn, on `extract/.../arm9.bin`): that is how the normalization steps were named.
- A unit whose constructor builds an `AnimModel` member after another member with an out-of-line constructor emits
  `~AnimModel` (binding 13); it cannot be linked while the gap holds the global `0x0202AF98`, and marking that symbol
  weak makes the linker keep the new copy instead (`CharacterModel.cpp`, matched but not complete).
- The order of a file's static `char` arrays (the joint names) follows no declaration order of the functions that use
  them; declaring them at file scope in an order found by one compile (read which slot each lands in, give each slot
  the ROM's name) placed all twelve.

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
