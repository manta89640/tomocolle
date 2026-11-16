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
  into `symbols.txt` (the save screens, overlays 69, 106 and 107, were completed with it).

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
The project itself fetches its compilers from the archive in `configure.py`.

Within a file, the original also set the optimisation per function: overlay 109's and the Mii transfer's functions
with loops that are neither rotated nor strength-reduced sit between ordinary ones, and match under
`#pragma optimization_level 2` around the function (then `#pragma optimization_level reset`). Nothing at the
default level gives that shape: not `volatile`, narrower or unsigned counters, other loop forms, or the `opt_*`
pragmas.

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

Not settled: overlay 109's agent found that this model does not reproduce its compiled `.data`/`.bss`, and the
best searches leave overlay 72's strings (50 of 134 objects) and WH-in-C (56 of 60) short. Test files on overlay
109's shape are the next step. Two other things the data showed:

- A `const` aggregate whose initialiser reads another constant (`{sPos[0].x - 0x20, sPos[0].y - 6}`) is filled in by
  the static initialiser at run time but stays in `.data`, as zeros in the ROM; a non-`const` one goes to `.bss`
  (the main program's list scenes, overlays 76 and 109).
- Comparing a compiled object's data with the ROM's before completing a unit finds wrong table values in the source,
  which no function shows (overlay 109's `sLayouts`).

## Matching techniques

What made near-misses match, by symptom, with where each was found. Most are about the order in which the compiler
sees things: declarations, statements, and how an address is formed. Work by script: compile one object with source
variants and compare one function (`tools/compare_object.py --diff`), then confirm with `ninja report`:
`compare_object.py` masks the bytes of calls and has called a function equal that objdiff did not.

**An address reloaded at each use, where the original keeps it in a register**
- Write through a pointer made by a cast: `GXRgb *colors = (GXRgb *)data_0213bab0;`. A plain local pointer to the
  global is dropped and the address reloaded (`MapScene::Exit`).
- `#pragma opt_propagation off` around the function keeps constants and addresses in callee-saved registers across
  calls (overlay 110's camera setters, the Mii transfer's `func_02077858`).
- In a one-iteration loop a large member offset (`this + 0x268`) is computed once and kept (`MapScene::Init`, the
  back button in two one-iteration loops around its `SetPosition`). A one-element static array read in a
  one-iteration loop is loaded, not folded.

**An address kept in a register, where the original reloads it**
- Reach the member through a local pointer in the loop body (`MapCamera *camera = &mCamera;`): it stopped the
  compiler holding a table's address for the whole loop and spilling (`MapScene::Update`).
- A local `char` buffer filled backwards: `p = buff + size; *--p = 0;` makes the compiler reload a local struct's
  fields after each store through `p`, as the original does, where `p = buff + size - 1; *p = 0;` lets it keep them
  in registers (`float2str`, `wprintf.c`).

**Two operands loaded in the other order**
- How an object's address is formed decides it. Two adjacent members opened through one pointer to the first,
  `MessageFile *files = &mEtcMessages; files[1].Open(...); files[0].Open(...);` (`MiiListMenuScene::Init`).
- Do arithmetic in a statement of its own before the call: `hour -= tens * 10; f(hour);` rather than
  `f(hour - tens * 10)` (`ClockDisplay::Refresh`, in overlays 72, 73 and 74).
- Pass the value just stored in a member (`f(mLights[0])`) rather than recomputing it (`MapScene::Init`).
- Declare a value before the call that might clobber it: `u8 t = others[i];` before `Rand` (`MakeDailyNews`).
- `int next; BOOL go; go = FALSE; next = 0x44;` gives another order than the initialised declarations
  (`UpdateConnection`).
- `hour * 3600 + minute * 60` computes the minutes first.
- A parameter passed on through a local copy (`ld = num; ... __num2dec(&form, ld, &dec);`) is loaded before the
  function's first early return, where passing the parameter loads it after (`double2hex`, `wprintf.c`).

**Stack slots in the wrong order, or a frame too large**
- Slots follow the order of the caller's locals; an inline's locals and by-value copies sit below them. Declare them
  in the caller to move them up (`MapScene::Update`), or use an inline to move a copy down (`OptionScene::OpenWindow`).
- A struct made only to pass by value costs a slot the original lacks: pass the `static const` itself (overlay 109's
  `SetUpWindow`, `SetUpList`). Where the original has the slot, make the local and pass its address, point first
  (`ScreenPoint point = *touch; ScreenRect rect = GetAmPmRect(); func_02028d88(&rect, &point);`,
  `OptionScene::Update`).
- Two dead copies of a vector are the by-value `FxVec2` of `SetPosition(FxVec2(x, y))`; which of
  `SetPosition`/`SetPositionFromVec` is called decides their order.
- A local copied inside a loop is hoisted before it and takes the original's slot; declared before the loop it gets
  another (the main program's list scenes).
- Spill slots of a function too large to keep its variables in registers (MSL's `__sformatter`, 3.4 KB) follow the
  order of their declarations, and a 64-bit variable can be given two slots apart. One variable that is saved in two
  unrelated places got two slots where the original had them elsewhere: two variables, one per place, matched. A
  script that permutes the declarations and counts the differing lines gets close fast; the rest took that split.

**Registers allocated differently**
- A loop that counts down a variable of its own and steps a second one (`for (k = n; k >= 1; k--, bit -= 4)`) gives
  the two the other pair of registers from `for (i = k, ...)` over a copy; and a byte swap
  (`t = cp[i]; cp[i] = cp[7 - i]; cp[7 - i] = t;`) through a `char *` declared in a block of its own keeps both
  accesses indexed, where one through `(char *)&num` is turned into a post-incremented pointer (`double2hex`).
- A fresh `for (int i ...)` in every loop (overlays 98, 99); C-style declarations at the top, in the order the
  original spills them.
- Write a loop the way another file writes the same loop: overlay 110's partner loop matched once it used overlay
  109's form (`continue`, and `mWindow.mTexts[i + 1]` at each use instead of a local).
- Pass a member's address again rather than a local pointer to it, or the reverse (`ReloadCharacter`).
- A loop over an array of objects that reads a member's member: `mButtons[i].GetCharCount()`, an inline of the
  element's class, keeps a pointer to the element; `mButtons[i].mText.GetCharCount()` keeps one to the member
  instead (`SoftKeyboard::GetButtonCharCount`).
- A pointer to a member array declared before a local `const` array is formed before the array's `ldm`/`stm` copy,
  declared after it, after (`func_02083c54`, `Keyboard.cpp`).

**Fields read off an element's address, where ours folds the offsets**
- At optimisation level 2 the compiler stops folding offsets through inline calls: `GetSelectedResident()->
  GetIndex()` reads `mIndex` off the element's address, as the original does (overlay 109's `UpdateSending`).

**Branches and switches laid out differently**
- An if/else the original compiles as "then predicated, branch over else" is written the other way round
  (`if (i != n) x++; else ...`).
- An if/else assigning a member stores at once; a ternary lets the store be scheduled later.
- The same member assigned in both branches (`if (...) y = A << 12; else y = (A + 2) << 12;`) where a local set in
  each branch and stored after them schedules the shift and store differently (`KeyboardButton::Draw`).
- A switch with the cases in the order of the original's code: `default` first puts its code after the compare
  chain; a jump table's values that only go to the default must be written out as cases; the tutorial's cases
  first gave `MapScene::Init`'s compare chain. `case 0: case 9: case 10:` gives `cmp; cmpne; cmpne` where `||`
  gives a range test.
- `for (;;) { if (!cond) break; n++; }` where `while (cond) n++` gets rotated (the save screens). An `i < 100` loop
  that is rotated at level 2 is a `do { } while`.
- Identical statements in an if and its else are merged into one: `++n` in one and `n++` in the other keeps both.

**Constants and arithmetic**
- `MATH_Rand16(...) & 3` where `% 4` adds sign fix-ups; an inline `Rand(int max)` around `MATH_Rand16` gives the
  original's registers.
- `x * 5 * FX32_CONST(0.01)` keeps the shift-add and the multiplication by 41, where `x * 5 * 41` folds to 205.
- `(x == 1 || x == 2)` on a `u8` gives `add #255; and; cmp #1; bhi`, where `(u8)(x - 1) <= 1` gives a `sub`.
- A `wchar_t` set from a choice of two wide constants (`fill = zero ? L'0' : L' ';`) and then compared is compared
  in its register; with `'0' : ' '` the compiler reloads it from the stack, where its address is taken
  (`__wpformatter`, `wprintf.c`).

**Classes, copies and tables**
- A class whose constructor stores only another class's tables holds that class as a member rather than deriving
  from it: deriving makes the compiler emit its own table and destructors (`RoomDoorModel` in overlay 74, the
  character models).
- Record copies: `(CharacterRecord &)dst = src` (overlay 109). A copy of a `WMBssDesc` is field by field; a struct
  wrapping one is copied with `ldm`/`stm`.
- Bit fields are allocated from the low bit, so a structure laid out high bits first (the Wii's `RFLCharData`) is
  declared in reverse (`lib/MiiData`).
- `NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(...))` rather than writing the field directly.
- Pitfall: a zero-length padding array in a class (a `sizeof` that came out 0) made mwcc drop a member array's
  `__cxa_vec_cleanup` from the implicit destructor without a word (overlay 76).

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
- `tools/strings.py` has named some number tables as text (`gText_Ri`, `gStr_F`, ...). A "string" in a table is
  data; rename it when the code shows what it is.
- MSL's files are best checked against the archive's own object: `tools/compare_object.py build/.../x.o
  <member>.o` compares the functions of the same names, and `readelf` shows its data and relocations. A checkout
  configured before the fix in `download_msl` has the archive under `tools/bin/msl/` with every byte from 0x80 up
  turned into `?`, unreadable; delete `tools/bin/msl` and run `configure.py` again. Two members are both named
  `critical_regions` (names are cut to sixteen characters): extract by position, not by name.

**MSL**
- A loop that the original enters without testing its pointer is a `do`/`while` (`__flush_all`).
- A bitfield struct copied to a local (`__file_modes mode = file->mode;`) is given a stack slot; reading the field
  through the pointer again reuses the loaded word, as the original does (`__get_char`).

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
