# Progress tracking on ByteMatch

[ByteMatch](https://bytematch.dev) shows how much of the game compiles back to identical bytes. It builds nothing
itself: it reads the report that [objdiff](https://github.com/encounter/objdiff) writes, `ninja report`, from this
repository's CI.

## What it reads

| ByteMatch | Here |
| --- | --- |
| A report per game version, as a CI artifact named `<version>_report` | `jpn_rev1_report`, uploaded by `.github/workflows/build.yml` from successful pushes to `main` |
| Units, drawn as a map by the directories in their names | `src/...` and `lib/...` for the files named in a `delinks.txt`, written or not, and `main/gap_<n>`, `overlays/ov<nnn>/gap_<n>` for what is left of each module |
| Progress categories | By module: `main`, `itcm`, `overlays`. By origin: `game` for `src/`, `libs` for `lib/` |
| A unit's source file, for "View source" | `metadata.source_path`, set by dsd |
| Complete units | Files marked `complete` in `delinks.txt` |

`tools/objdiff_config.py` writes `objdiff.json` in that shape, from what dsd generates.

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
