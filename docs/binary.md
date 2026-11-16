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

So 1,293 of the main program's 4,205 functions are library code with known names, and 181 library files are cut
out as units under `lib/`: 155 of NitroSDK and NitroSystem, and 26 of MSL's C library (`lib/MSL_C/`; `math_api.c`
and `ansi_fp.c` are left in the gap, as their ranges would hold more functions than the objects have).

MSL's own files are compiled with 2.0/sp2 (measured: its objects say "Metrowerks C/C++ for ARM 3.0.0.136", which is
what that compiler writes) and the headers of that MSL, internal ones included. `lib/MSL_C/wstring.c` is the first:
its seven functions match and it is linked from source. `lib/MSL_C/e_pow.c`, fdlibm's `pow` as MSL changed it (see its
header), has its data matching and its one function at 99.88%: three values are given registers in another order.
Measured from the original objects (in the compiler's MSL archive): MSL was compiled with exception tables, which the
game's link leaves out (its `.exceptix` is empty), and the soft floating-point helpers it calls are `_dadd`, `_dsub`,
`_dmul`, `_ddiv`, `_dsqrt`, `_dgr`, `_dleq`, `_dflt` and `_f2d`, now named. What is left in the main program is two stretches: `0x0201F4DC` to `0x02053C64`, and from
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

## The apartment scene

Overlay 74 is the apartment: the scene `ScenePlayRoom2` with the messages of `tmd_mainroom_msg_JP.bmg`. Measured:
254 functions, 146 KB, of which 104 KB is 15 functions of 2 to 13 KB each. Decompiled in
`src/overlays/ov074/RoomScene.cpp`, one unit that claims every section of the overlay; 247 functions match.

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

- 7 functions that are written and differ, each with a comment saying how: in overlay 74 `ChooseReactions` and
  `StartMeeting` (the callee-saved registers named differently), `StartPairEvent` (the offset of a pooled
  constant), `GetAngleAround`, `ApartmentFront::UpdateCursorTarget`, `NamePanel::Init`; in overlay 73
  `TutorialApartmentFront::UpdateCursorTarget`. Both copies of `ClockDisplay::Refresh` match since the hour's ones
  digit is subtracted in place (`hour -= tens * 10;`, as overlay 72's `MapClockDisplay::Refresh`). Overlay 73's
  `NamePanel::Init` matches, and its fix (the character counts read through a pointer to the canvas, and both text
  positions set with `SetPositionFromVec`) is still to be carried over to overlay 74's byte-identical copy.
- The data of both files, above: the order of the objects after the pool of small constants, and of overlay 73's
  `.data`, is not one our compiler gives, so neither unit is complete. Overlay 73's notes have the rule for the
  order the compiler does give, by which its pool is reproduced.
- `tools/check_relocs.py` lists, for overlay 74, only string literals and `data_020f7300`, which is
  `data_020f72f8[0].mLightColor` (other overlays refer to it by that name too); for overlay 73, string literals and
  three anonymous vectors.
- `docs/notes/ov074.md` and `docs/notes/ov073.md` keep the working notes, with the evidence for names and layouts.

## The wedding announcement

Overlay 96, `src/overlays/ov096/WeddingScene.cpp`: 35 of its 37 functions match.

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

Not done:

- `WeddingCamera::Reset` (the stack slots of three vector copies) and `WeddingCamera::Approach` (two loads in the
  other order), each with a comment.
- The data: `.rodata` starts with an 8-byte object `{25, 128}` that nothing refers to, which the source does not
  give; the strings of `.data` are named `gStr_*` in `symbols.txt`, not by the compiler's numbers.

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
- The message window's result is at +0xAF84 in these scenes (measured on all four of scene 0x24's windows, which
  start at +0x230 where the constructor stores the window's table), and what the scenes keep for after a window
  closes at +0xAF98.
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

- The constructors are in the main program's file of scene constructors, which is not cut, and are named only by
  their addresses.

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
  `0x0203BB8C` and `0x0204D890` to `0x0204E38C`. A second, child-only copy of WH with no debug strings is at
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

- Three functions differ, each with a comment saying how: `Init` (one instruction placed differently: the back
  button's base address; the same length), `Update` (the frame is the original's since the touch loop reaches the camera
  through a pointer; the stack slots of the clock's date and time and the order of the rectangle arithmetic
  differ), `MakeDailyNews` (the registers of the loop that picks the month's kind of news). `Exit` matches since `data_0213bab0` is a byte
  buffer (0x70 bytes to the next symbol; overlays 68 and 75 clear 100 of them with `MI_CpuFill8`, overlays 31 and
  90 read bytes of it) and the map writes its three colours through a `GXRgb *` cast from it: the cast pointer is
  kept in a register, the array's own address is loaded again before each store.
- `.data`: its pool is the ROM's, but of the 112 sorted strings and the virtual table after it only some are in the
  ROM's order yet; the strings of the code are named `gStr_*` in `symbols.txt`, not by the compiler's numbers.
- What `func_ov072_0214ef3c` (func_0202cb08 of `gGameData`, which decides whether the fountain can be entered) is.

## The Mii transfer

Wireless play (overlay 109, scene 0x29) takes Miis from another machine through code of the main program,
0x020776E4 to 0x02079FA0, which nothing else calls. It is three files under `lib/MiiTransfer/` (the name and the file
names are the project's own: none survives), all built with 2.0 SP2 patch 2 (see "Compiler"):

| File | Code | Data | Functions |
| --- | --- | --- | --- |
| `MiiTransfer.cpp`, the protocol | `0x020776E4`-`0x02078F58` | .bss `0x02145380`-`0x02145C00`, .rodata `0x020B7BE4`-`0x020B7E1C` | 52 of 54 match |
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
  the SDK has it, `func_020791C4`, which picks the parent to connect to from a scan's results (by GGID, and the entry
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

Not done: `func_02077858` (the session's update) differs only in five branch offsets of a jump table (the unlisted
states of case 3 branch to the function's end, where the original branches to the inner switch's break), and
`func_02077F48` in two registers the other way round, with comments above them, so `MiiTransfer.cpp` is not complete.
Measured with the compiler: `func_02077858` is compiled at `#pragma optimization_level 2` with `opt_propagation off`
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
- **0x36 is the creator's name here too**: the Mii maker (`func_ov076_0214c2e4`) gives a new Mii the 0x36 of saved
  character 0, or that character's name when `func_ov076_0214bf20` rejects it. (Overlay 109's source calls it a
  nickname.)
- `MiiData_AssignCreateId` writes the create ID from a counter that it advances, six bytes beside it (the Mii maker
  passes `data_0212349c`, through `func_0209859c`) and a random number from the main Mii code (`func_020743a0`):
  byte 0 is `0xC0`, the random number times 4 and bits 12-13 of the counter.
- Two setters, of bit 2 of 0x20 (`func_0207a6f0`) and bit 0 of 0x00 (`func_0207a7bc`), are called through
  `func_02098cdc` and `func_02098ce8`; what those bits are is not known.
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

Overlay 110, `src/overlays/ov110/ItemTradeScene.cpp`, one unit with `.text`, `.rodata`, `.data` and `.bss`: 73 of
its 75 functions match; not complete.

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

Not done:

- Two functions differ, each with a comment saying how: `SetupWindow` (the original keeps the button positions'
  table in r11 and so spills `this`) and `Update` (one instruction's place). `SetItemCamera` and `SetRoomCamera`
  match under `#pragma opt_propagation off` with the vectors as struct copies, and `UpdateWireless` with its partner
  loop written as overlay 109's (`continue`, `mWindow.mTexts[i + 1]` at each use). The unit cannot be complete until they match.
- `tools/check_relocs.py` lists one reference: `UpdateRoom` reads the room style's light colour as
  `data_020f72f8[style].mLightColor` where the original refers to `data_020f7300`, as overlay 74 does.
- What the two item tabs besides the first list (`0x0209F208`, `0x0209F28C`), and what the selection's category and
  value (0x30, 0x34 of `ItemTradeSelection`) are for: nothing in the overlay reads them.

## Wireless play: the Mii exchange

Overlay 109, `src/overlays/ov109/TushinScene.cpp`: 68 of its 77 functions match, and every section of the
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
  positions from the `.rodata` pool; ours folds them into immediates, and every compiler version tried does, so
  `__sinit_TushinScene.cpp` does not match.
- **`.rodata`**: the objects under 64 bytes are one pool sorted by size (8 to 56 bytes), the arrays of `u8` and the
  strings before and after it, and the 0x90- and 0x110-byte tables on their own after it. The compiled `.rodata` is
  in the ROM's order: same-size declarations were permuted until it was. Two of the pool's constants are folded
  wherever they are used and kept anyway (`sCameraPosition`, `sCameraTarget`, and the signal sprite's
  `sSignalPosition`).

Not done:

- 9 entries differ: 6 functions and the static initialiser, each with a comment saying how, plus the two
  destructors at `0x02152460` and `0x021524A8`, which are still under their address names in `symbols.txt` (ours
  are `D1`/`D0`). Most of the 6 differ in the register choice or scheduling of one or two instructions; `Update`, `UpdateSending` and `UpdateCharacters` also read the selected resident's `mIndex` and a
  model's `mVisible` with folded offsets, where the original goes through the element's or the model's address.
- `.data`: the three 4-byte strings of `sCharacterArchives` and the 12-byte group of its pool come out in another
  order than the ROM's.
- `.bss`: the state of the exchange is a structure (`sWireless`) in the source. In the ROM it is a pool of separate
  statics sorted by size (a `u8`, 20 `u16`, ten `int`, then `sParentTimers` and `sReceivedName`), which the code
  reaches from the pool's start. 13 of the `u16` (pool+0x06 to +0x1F) are not referred to by anything in the
  overlay. Inferred: functions that used them were stripped by the link, which keeps the pooled statics they used.

## The keyboard's object

`src/keyboard/Keyboard.cpp` (`0x02082EAC` to `0x02083DC4`, 34 functions) is complete: every function matches and the
unit is linked from its object. It is the object a scene holds for the on-screen keyboard (`SoftKeyboard`,
`include/keyboard/SoftKeyboard.h`, 0x6014 bytes), and sets up and draws the keyboard's core
(`KeyboardCore.cpp`) through it. Measured:

- **Callers.** The main program's scene constructors construct it (`_ZN12SoftKeyboardC1Ev`, six calls), and
  `func_0204ce88`/`func_0204d178` of the main program and overlays 43, 75, 76, 92 and 93 use it. The main program's
  set-up is `SetArchiveName("KBD")`, `func_02083078` (the background plane 0x100 and a palette),
  `func_02083288(0, 0)`, `func_0208382c` (the sprite resource, a message file, a character offset, the layout),
  `func_02083c30` and `GetButtonCharCount`; the overlays call the same functions.
- **Layout.** The plane, the palette number (13), the sound handle of the key sounds, the keys' screen data
  (0x800 bytes, which `func_02027f3c` sends) and two buffers of 0x2280 bytes that the core is given
  (`func_02081d68`; the second is what `TransferCharData` queues to VRAM), the six background paths (five of 0x30 bytes
  and the palette's of 0x20), five tab sprites, two buttons of 0x6B8 bytes, a sprite, its resource and layout, and the
  three sprite paths. Every path starts `xxx:/`, which `SetArchiveName` overwrites with the archive's mount name.
- **The buttons** (`KeyboardButton`, `0x0208329C` to `0x02083740`) are a sprite with a `MessageText`: they show
  messages 6 and 5 of the file passed to `func_0208382c`, at (0x80, 0xAD) and (0, 0xAD), and play a sound of
  `gSoundManager` as they are touched and as they are released. `Update` returns TRUE when the stylus is lifted on the
  button.
- **The tabs.** The five areas of `sTabAreas` (`0x020B7F78`) are the core's key areas (`func_02082e70`): the core
  calls `func_02083c54` when one is touched, which lights that tab and selects keyboard mode 0 to 4
  (`func_02081ff0`). `func_02083128` registers only the first two at first.
- **Data.** `.rodata` is one pool, `sButtonPositions`, `sButtonRect`, the local table of the five modes,
  `sTabAreas`, `sLayoutPositions`; `.data` is the nine paths. The `gText_Yu` and `gStr_F` that the string tool had
  named here were `sButtonPositions` + 4 (the code reads the y column off it) and `sLayoutPositions`.

Inferred: the 0x1C bytes from +0x5FF8 to the end are part of the object (the Mii maker's next member starts there),
but nothing in this file touches them.

## The Mii maker

Overlay 76, `SceneMiiEdit2`: 83 functions, `.text` `0x02149A40` to `0x02158748` (60 KB). It is cut as one unit,
`src/overlays/ov076/MiiEditScene.cpp`, which claims every section. 24 of its 83 functions are written and match
(the scene's StartLoading, Draw, Exit, camera functions and destructors, and the pages' Draw and Exit); the header
holds the scene's layout and its seven pages (`mPage`, +0x150: 0 the sex choice, etc message 0x1A; 1 the face, 0x1C;
2 the profile, 0x1D; 3 the voice, 0x21; 4 the personality, 0x22; 5 and 6 window pages not yet identified).

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

Not done: the other 59 functions (the scene's Init and Update, the pages' Init and Update, and the large edit
functions), and the data.

## The Mii library (NFL), not cut yet

Code of the main program from `0x02069BF4` (right after NitroSystem) to `0x020776E4` (the Mii transfer), 144
functions; started, not decompiled, no unit cut yet. Measured:

- **Its entry points.** `main` mounts `Cmn/Nfl/NFL_Res_LZ.bin` as `NFL`, gets `NFL:/NFL_Res.dat` and passes it to
  `func_02072a58` with a work area (`data_02115480`), `data_02115574`, `0x4000` and `data_020fc504`. That function
  returns 8 when a pointer is null or the size is not a multiple of 4, stores the RTC time in seconds at +0x58 of the
  work area and the constants of an LCG (`0x5D588B65`, `0x269EC3`) at +0x5C/+0x60, and publishes the work area in
  `data_02145360` with interrupts disabled. Every frame `main` calls `func_020749e4`, and after `OS_WaitIrq`
  `func_02076448(0x4000)`: both are state machines on +0x8C and +0x90 of the work area that return small codes
  (0, 2, 3, 4, 8, 0x13, 0x14).
- **What it does.** `0x0206CF6C` to `0x020776E4` reads its state through `data_02145360` (38 references), builds
  display lists (`G3_BeginMakeDL`, `G3C_Vtx`, `G3C_TexCoord`, ...), loads textures and palettes (`GX_BeginLoadTex`),
  and reads `OS_GetMacAddress` and `RTC_GetDateTime` (as a Mii's create ID is made of). `0x02069BF4` to `0x0206CF6C`
  is a byte-stream reader and display-list drawing code that calls back into `0x0206CF6C`, `0x020743E4` and
  `0x02074444`, and is called from the other part.
- **One library, by its table.** The function table `data_02112140` (.data) holds `func_02071578` and six functions of
  `0x0206AAEC` to `0x0206B024`, so the two parts are linked together. Its .data is inferred to run from `0x02112140`
  to `0x0211220C`, where the keyboard's tables start; its .rodata from `0x020B6524`, read by both parts.
- **Callers.** `main`, the main program's character code (`CharacterBase`'s destructors call `func_02072dc0` and
  `func_02072f34`), overlays 70, 76, 78 and 79 (`func_0206dc84`), and 48, 51, 68, 75, 78 and 100
  (`func_02073514` to `func_02073550`).
- **One static.** The library's only .bss is the pointer `data_02145360`: every one of its 38 references loads
  the address and reads or writes at offset 0, and nothing in `0x02069BF4..0x0206CF6C` touches .bss. The 0x1C bytes
  after it, to the Mii transfer's .bss at `0x02145380`, are not referred to. So .bss cannot show where its files
  begin and end, and the compiler's pooling threshold (137 against 138) cannot be read from its statics; its .rodata
  is the place left to look (`data_020b676c`, `...6770`, `...6774`, `...6778` are each loaded by their own address).
- **No identity string.** The main program holds only two middleware strings, `[SDK+NINTENDO:BACKUP]` and
  `[SDK+SHARP:SHTTS-1_2_0]`; nothing names NFL's version. No objects or sources of NFL (or the Wii's RFL) were found
  on the machine to match against.

Started in `lib/NFL/NFL.c`, one unit for the library's whole range (.text `0x02069BF4`-`0x020776E4`, .rodata
`0x020B6524`-`0x020B7BE4`, .data `0x02112140`-`0x0211220C`, .bss `0x02145360`-`0x02145380`), not yet divided into
files. 29 of 144 functions match: the byte-stream reader, the palette makers (16-colour ramps from tables of 8-bit
colours, `GX_RGB` of each component shifted down 3) and the create-ID and work-area helpers. Measured with them: the
code compiles the same with 2.0 SP1p2, SP2, SP2p2 and SP2p3, as C and as C++, so the compiler is not settled. The Mii
records it reads have the Wii's layout: the birthday check `func_020731e0` reads the month from bits 10-13 and the day
from bits 5-9 of the first halfword and checks them against a table of days a month, and the favourite colour in
bits 1-4 against 11, as `RFLCharData` places them.

Inferred: this is NFL, the face library that `NFL_Res.dat` is the resource of (the file name, the Mii-face drawing
and the create-ID ingredients). Not established: its files' bounds, which compiler built it (the Mii transfer after
it is 2.0 SP2p2, the game SP2p3), and its function names.

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
  `MiiTransferWH.cpp` matches 45 of 45 and `MiiTransfer.cpp` 52 of 54; with 138 they drop to 40 and 43, the functions growing
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
