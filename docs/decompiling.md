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
   units under `lib/` (see [binary.md](binary.md)). Still to find: the rest of MSL, WH, NFL and the text-to-speech
   middleware. Every library function named makes the game code that calls it readable, and what is left when the
   libraries are out is the game.
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
  change when the file's code does; the check names the new one.

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
