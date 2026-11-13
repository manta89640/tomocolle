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

So 1,293 of the main program's 4,205 functions are library code with known names, and 155 library files are cut
out as units under `lib/`. What is left in the main program is two stretches: `0x0201F4DC` to `0x02053C64`, and from
the end of NitroSystem to the end of the code at `0x020B04C8`. They hold the game's own code and the libraries not
matched yet: WH, NFL, the text-to-speech middleware, and whatever of the SDK differs between release 30001 and the
game's 30007.

## Voice synthesis

The Miis' voices come from Sharp's text-to-speech engine, SHTTS 1.2.0 (the version is in the SDK's list of library
tags). Measured:

| | Where | |
| --- | --- | --- |
| `TtsPlayer`, the game's wrapper | `0x02083DC4` to `0x0208473C`, 28 functions | Decompiled, `src/voice/TtsPlayer.cpp` |
| The engine | `0x0208473C` to `0x0208EB18`, 123 functions, 44 KB | Decompiled, `lib/SHTTS/`: 122 functions match and are linked from source |

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

- `shtts_2.c` is one function, `ShttsDuration_SetWordSyllables`, which does not match yet (the original's size, 59 instructions
  different), so the file it belongs to cannot be linked from source while it is in it.
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
| `CardPullOutMonitor` | `0x0202E3EC` to `0x0202E44C` | Turns the screens off and stops when the card is pulled out |
| `MessageFile`, `MessageText` | `0x0202E44C` to `0x0202F408`, 28 functions | A BMG message file (`gMessageFiles[0]` is `BMG:/tmd_msg_JP.bmg`), and a text box that is both kinds of canvas and expands, aligns and draws a message, all at once or a character at a time |

- `BgTextCanvas` and `ObjTextCanvas` have `TextCanvas` as a virtual base, and `MessageText` inherits from both: their
  tables hold only the offset of that base (4, 0x150, and 0x588 and 0x438 in a `MessageText`).
- A message's tags start with the character 0x1A, followed by the tag's length in bytes; `MessageText::ExpandTags`
  replaces them with up to 20 strings and 2 messages set beforehand.
- The fonts are in a file of their own: `gFonts` comes before `gTextObjResource` in memory, and the compiler puts it
  after when both are defined in one file.

## Compiler

**Inferred, and only weakly tested so far:** mwccarm 2.0 SP2 patch 2 ("Version 3.0 build 137").

- NitroSDK 4.2 is from 2008, and the game came out in June 2009. Pokémon Platinum (2008), also on NitroSDK 4.2, is
  matched by this version.
- The six functions decompiled so far match with it. They are too simple to tell compiler versions apart, so this
  shows that the pipeline works, not that the version is right.

The libraries are another matter, and settled: NitroSDK is built with 2.0 SP1 patch 2 and NitroSystem with 2.0 SP2,
as in Pokémon Platinum, and 1,148 of their functions come out identical to the game's. That says what Nintendo built
its libraries with, not what the game's own code was built with.

For the game's code the evidence is now better. `src/voice/TtsPlayer.cpp`, 28 functions of C++ that set up streams
and threads and pass up to ten arguments, matches in full with this compiler and these flags, 26 of them as first
written. That does not rule out a neighbouring patch release, which may differ only in code this file does not have.
The WH code is a good candidate, since its source is the SDK's own sample: compiled with the right compiler and flags,
it must come out as it is in the ROM. Compiling it needs the SDK's headers and the C library headers that those
include (`stdio.h`, `stdarg.h`), which do not come with the compiler.

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
