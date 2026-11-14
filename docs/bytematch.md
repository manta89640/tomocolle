# Progress tracking on ByteMatch

[ByteMatch](https://bytematch.dev) shows how much of the game compiles back to identical bytes. It builds nothing
itself: it reads the report that [objdiff](https://github.com/encounter/objdiff) writes, `ninja report`, from this
repository's CI.

## What it reads

| ByteMatch | Here |
| --- | --- |
| A report per game version, as a CI artifact named `<version>_report` | `jpn_rev1_report`, uploaded by `.github/workflows/build.yml` from successful pushes to `main` |
| Units, drawn as a map by the directories in their names | `src/...` and `lib/...` for the files named in a `delinks.txt`, written or not, and `main/gap_<n>`, `overlays/ov<nnn>/gap_<n>` for what is left of each module |
| Progress categories | By module: `main`, `itcm`, `overlays`. By origin: `game` and `libs`, which between them hold all the code (below). By library: `nitrosdk`, `nitrosystem`, `shtts`, `msl`. By the game's subsystem: `sound`, `text`, `voice` (the directories under `src/`) |
| A unit's source file, for "View source" | `metadata.source_path`, set by dsd |
| Complete units | Files marked `complete` in `delinks.txt` |

`tools/objdiff_config.py` writes `objdiff.json` in that shape, from what dsd generates.

`game` is all of the game's own code and `libs` all of the libraries', so that "Game" on ByteMatch is how much of the
game itself matches. A file counts by its directory (`src/` or `lib/`). A gap, which no file claims yet, counts by the
address of its first function: the main program's library ranges (NitroSDK and MSL from `0x02000000` to
`0x0201F4DC`, NitroSystem from `0x02053C64` to `0x02069BF4`, measured in [binary.md](binary.md)) and the ITCM are
`libs`; the rest of the main program and every overlay are `game`. That is right but for the libraries not found yet
(WH, NFL, a few files of the SDK), which are in the main program's game ranges and count as `game` until they are
found and cut out. A gap without code (data alone) counts toward neither.

The library and subsystem categories come from the `LIBRARIES` and `SUBSYSTEMS` tables of the same script: a new
directory under `lib/` or `src/` gets a category of its own by adding it there. They count only files, so they say
how much of what has been cut out matches.

## Setting it up

1. **Register the project.** Sign in at [bytematch.dev](https://bytematch.dev) with the GitHub account that owns the
   repository, then Manage → Add project. Name the game "Tomodachi Collection", so that it shares a page with any
   other project for the same game.
2. **Give CI the base ROM.** A report needs the original code to compare with, so the job that writes it needs the
   ROM, which cannot be in this repository. The workflow reads it from a private repository:
   - Create a private repository that holds `baserom_jpn_rev1.nds.xz` (`xz -9 baserom_jpn_rev1.nds`).
   - Add a read-only deploy key to it, and its private half to this repository as the secret `BASEROM_DEPLOY_KEY`.
   - Set this repository's variable `BASEROM_REPO` to the private repository's `owner/name`.

   Until then the build job is skipped, and only the job that compiles the sources runs.

### Without the ROM in CI

A report can be sent from your own machine instead. Create a token on the project's Manage page, put it in the file
`.bytematch-token` in this directory (git ignores it) or in the environment as `BYTEMATCH_TOKEN`, and run:

```sh
ninja upload
```

That builds the report and sends it as the progress of the commit that is checked out, which has to be pushed and
without uncommitted changes: ByteMatch reads the source it links to from GitHub at that commit.
