#!/usr/bin/env python3
"""Generates build.ninja for the Tomodachi Collection decompilation.

Run this once after cloning, and again after adding a source file to a delinks.txt, then run `ninja`.
Only the standard library is used, so the system's python3 is enough.
"""
import argparse
import hashlib
import io
import json
import os
import platform
import shlex
import shutil
import stat
import subprocess
import sys
import urllib.request
import zipfile
from pathlib import Path

ROOT = Path(__file__).parent.resolve()
GAME = "tomocolle"

# Each version is one base ROM. The name is also the name of the ByteMatch report artifact, <name>_report.
VERSIONS = {
    "jpn_rev1": {
        "title": "Tomodachi Collection (Japan) (Rev 1)",
        "game_code": "CCUJ",
        "sha1": "f37d5ff0e3444cf3fab38890efb71f4f14c96994",
        "defines": ["VERSION_JPN_REV1"],
    },
}
DEFAULT_VERSION = "jpn_rev1"

# ---------------------------------------------------------------------------------------------------------------------
# External tools, pinned by version and SHA-256. A new version means new hashes: take them from the release's assets.
# ---------------------------------------------------------------------------------------------------------------------
DSD_VERSION = "v0.12.1"
DSD_URL = "https://github.com/AetiasHax/ds-decomp/releases/download/{version}/{asset}"
DSD_ASSETS = {
    ("linux", "x86_64"): ("dsd-linux-x86_64", "256edde804dd8e5d7bc4c0e4187f85244ee4cd1f42bb427502e01fda6af43ceb"),
    ("darwin", "arm64"): ("dsd-macos-arm64", "1bbb57fe66a37123a7e9a64459b174956ea69ea103ad3dfd2766f18ca63057c7"),
    ("darwin", "x86_64"): ("dsd-macos-x86_64", "21adcef119d6e9b6bb853b0e5cc626bc4c086d28d45abc9a91b715c78631a044"),
    ("windows", "x86_64"): ("dsd-windows-x86_64.exe", "5917df26bffc05d4752276a2e0b1edd21bd6e920aabe1992be8a50efc9d10932"),
}

OBJDIFF_VERSION = "v3.8.3"
OBJDIFF_URL = "https://github.com/encounter/objdiff/releases/download/{version}/{asset}"
OBJDIFF_ASSETS = {
    ("linux", "x86_64"): ("objdiff-cli-linux-x86_64", "e823ae932a56c68dcd86b68d12f2268c953e0c027402162c8a6dfa6a00d65008"),
    ("linux", "arm64"): ("objdiff-cli-linux-aarch64", "0e630457e2f33a43da3723e195ecd0b39b683a6a83122955fdc68c5b17183cbc"),
    ("darwin", "arm64"): ("objdiff-cli-macos-arm64", "27dbd79196394c246dde77ef0a6ac855ff6c568069171dd7db0093f39c03ee8d"),
    ("darwin", "x86_64"): ("objdiff-cli-macos-x86_64", "4a0f5079600cb9ddf2369414e712f96838bc0ae05cd1a98c92f6131078ec4b0e"),
    ("windows", "x86_64"): ("objdiff-cli-windows-x86_64.exe", "dd12dfb5e9e94a7f6c949a37e1815963cf2cffe4c87b4999b7ebc8c15b398a1f"),
}

# wibo runs the Windows compiler on Linux. This is the last release before the 1.0 rewrite: 1.2.0 cannot load
# sjiswrap (GetProcAddress fails), on Linux and macOS alike, so macOS uses wine until a release that can.
WIBO_VERSION = "0.6.16"
WIBO_URL = "https://github.com/decompals/wibo/releases/download/{version}/{asset}"
WIBO_ASSETS = {
    ("linux", "x86_64"): ("wibo", "8a8490a6172aa4f0f6ddcadb144ca96f51da6e90e6648ce9adaf4f6babb6e00b"),
}

# sjiswrap hands the compiler its sources converted from UTF-8 to Shift JIS, the encoding of the game's own strings.
SJISWRAP_VERSION = "v1.2.2"
SJISWRAP_URL = "https://github.com/encounter/sjiswrap/releases/download/{version}/sjiswrap-windows-x86.exe"
SJISWRAP_SHA256 = "01d50fa1148cfc3c3b16658c6c9b1c0d9f5324cde79ed6c09ef8353af9d63adf"

# The archive holds every mwccarm version and is updated in place, so the executables are what is pinned.
MWCCARM_URL = "https://decomp.aetias.com/files/mwccarm.zip"
# CodeWarrior for NDS 2.0 SP2 patch 2, which reports itself as "Version 3.0 build 137". See docs/binary.md for why.
MWCC_VERSION = "2.0/sp2p2"
MWCC_SHA256 = {
    "mwccarm.exe": "b1da5a2f191d1cdea3fd078cedc9c504de46609642ae8e44c015014cef5419a7",
    "mwldarm.exe": "a41aea5f282041e16dcd6c3570328009cb1eefb32b0cd05f60a8fdb365aec47c",
}
# The same compiler's name on decomp.me, for objdiff's "create scratch" button
DECOMP_ME_COMPILER = "mwcc_30_137"

# ---------------------------------------------------------------------------------------------------------------------
# Compiler and linker flags
# ---------------------------------------------------------------------------------------------------------------------
# The compiler reads its sources as Shift JIS (from sjiswrap). In the default encoding, a character whose second byte
# is 0x5c, such as 表, would end in a backslash that escapes a closing quote.
ENCODING_FLAGS = ["-enc SJIS"]

CC_FLAGS = [
    "-O4,p",                # Optimize for speed
    "-proc arm946e",        # The ARM9 of the DS
    "-interworking",        # ARM/Thumb interworking
    "-enum int",            # Enums are int-sized
    "-char signed",         # char is signed
    "-fp soft",             # Software floating point
    "-Cpp_exceptions off",  # The exception table of every module is empty
    "-RTTI off",            # No runtime type information
    "-inline on,noauto",    # Only inline functions marked inline
    "-gccext,on",           # GCC extensions
    "-gccinc",              # #include "..." and <...> search the same paths
    "-sym on",              # Debug info, which objdiff shows as line numbers
    "-w off",
    "-nolink",
    "-msgstyle gcc",        # Messages an editor can jump from
]
LANG_FLAGS = {".c": "-lang=c99", ".cpp": "-lang=c++"}
SOURCE_SUFFIXES = tuple(LANG_FLAGS)

LD_FLAGS = [
    "-proc arm946e",
    # Keep everything. The original link stripped dead code, but -dead only reproduces that once relocs.txt knows
    # every reference: today it strips real code out of the main program and a number of overlays.
    "-nodead",
    "-nostdlib",
    "-interworking",
    "-map closure,unused",
    "-msgstyle gcc",
    "-m Entry",             # The entry function
]

INCLUDE_DIRS = ["include"]


def host() -> tuple[str, str]:
    system = {"Linux": "linux", "Darwin": "darwin", "Windows": "windows"}.get(platform.system())
    machine = {"amd64": "x86_64", "x86_64": "x86_64", "arm64": "arm64", "aarch64": "arm64"}.get(platform.machine().lower())
    if system is None or machine is None:
        sys.exit(f"Unsupported host: {platform.system()} {platform.machine()}")
    return system, machine


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as f:
        for block in iter(lambda: f.read(1 << 20), b""):
            digest.update(block)
    return digest.hexdigest()


def fetch(url: str) -> bytes:
    print(f"Downloading {url}")
    with urllib.request.urlopen(url) as response:
        return response.read()


def download_binary(path: Path, url: str, expected: str):
    """Downloads an executable to path, unless the one there already has the expected hash."""
    if path.exists() and sha256(path) == expected:
        return
    data = fetch(url)
    actual = hashlib.sha256(data).hexdigest()
    if actual != expected:
        sys.exit(f"{url}\n  has SHA-256 {actual}\n  expected    {expected}")
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)
    path.chmod(path.stat().st_mode | stat.S_IEXEC)


def check_compiler(directory: Path) -> list[str]:
    """Returns what is wrong with the compiler in a directory, as a list of problems."""
    problems = []
    for name, expected in MWCC_SHA256.items():
        exe = directory / name
        if not exe.exists():
            problems.append(f"{exe} is missing")
        elif sha256(exe) != expected:
            problems.append(f"{exe} is not the {MWCC_VERSION} one (SHA-256 {sha256(exe)})")
    return problems


def download_compiler(tools_dir: Path) -> Path:
    directory = tools_dir / "mwccarm" / MWCC_VERSION
    if check_compiler(directory):
        archive = zipfile.ZipFile(io.BytesIO(fetch(MWCCARM_URL)))
        prefix = f"mwccarm/{MWCC_VERSION}/"
        members = [m for m in archive.namelist() if m.startswith(prefix) or m == "mwccarm/license.dat"]
        if not members:
            sys.exit(f"{MWCCARM_URL} has no {prefix}")
        archive.extractall(tools_dir, members)
        problems = check_compiler(directory)
        if problems:
            sys.exit("\n".join(problems))
    return directory


def find_wrapper(choice: str | None, system: str, machine: str, tools_dir: Path) -> str:
    """Returns the command that runs a Windows executable on this host: "wibo", "wine", or a command of your own."""
    if system == "windows":
        return ""
    if choice is None:
        choice = "wibo" if (system, machine) in WIBO_ASSETS else "wine"
    if choice == "wine":
        wine = shutil.which("wine")
        if wine is None:
            sys.exit("No way to run the Windows compiler: install wine, or pass --wrapper")
        return wine
    if choice == "wibo":
        if (system, machine) not in WIBO_ASSETS:
            sys.exit(f"wibo {WIBO_VERSION} has no release for {system} {machine}: pass --wrapper wine")
        asset, digest = WIBO_ASSETS[(system, machine)]
        wibo = tools_dir / "wibo"
        download_binary(wibo, WIBO_URL.format(version=WIBO_VERSION, asset=asset), digest)
        return os.path.join(".", str(wibo.relative_to(ROOT)))
    return choice


class Writer:
    """A minimal writer of ninja files."""

    def __init__(self):
        self.out = io.StringIO()

    @staticmethod
    def escape(path) -> str:
        return str(path).replace("$", "$$").replace(" ", "$ ").replace(":", "$:")

    def comment(self, text: str):
        self.out.write(f"# {text}\n")

    def newline(self):
        self.out.write("\n")

    def variable(self, key: str, value):
        self.out.write(f"{key} = {value}\n")

    def rule(self, name: str, command: str, description: str | None = None, **kwargs):
        self.out.write(f"rule {name}\n  command = {command}\n")
        if description:
            self.out.write(f"  description = {description}\n")
        for key, value in kwargs.items():
            self.out.write(f"  {key} = {value}\n")
        self.newline()

    def build(self, outputs, rule: str, inputs=(), implicit=(), order_only=(), variables=None):
        line = f"build {' '.join(map(self.escape, outputs))}: {rule}"
        if inputs:
            line += " " + " ".join(map(self.escape, inputs))
        if implicit:
            line += " | " + " ".join(map(self.escape, implicit))
        if order_only:
            line += " || " + " ".join(map(self.escape, order_only))
        self.out.write(line + "\n")
        for key, value in (variables or {}).items():
            self.out.write(f"  {key} = {value}\n")
        self.newline()

    def default(self, targets):
        self.out.write(f"default {' '.join(map(self.escape, targets))}\n")


def config_files(config_dir: Path) -> list[Path]:
    """Returns every file of a version's dsd config, which the delinked objects and the linker script are made from."""
    names = {"config.yaml", "delinks.txt", "relocs.txt", "symbols.txt"}
    return sorted(p.relative_to(ROOT) for p in config_dir.rglob("*") if p.name in names)


def main():
    parser = argparse.ArgumentParser(description="Generates build.ninja")
    parser.add_argument("version", nargs="?", default=DEFAULT_VERSION, choices=sorted(VERSIONS), help="game version")
    parser.add_argument("-w", "--wrapper", help='what runs the Windows compiler: "wibo" (downloaded, the default on '
                        'Linux), "wine" (the default on macOS), or a command of your own')
    parser.add_argument("--dsd", type=Path, help="use this dsd instead of downloading one")
    parser.add_argument("--objdiff", type=Path, help="use this objdiff-cli instead of downloading one")
    parser.add_argument("--sjiswrap", type=Path, help="use this sjiswrap.exe instead of downloading one")
    parser.add_argument("--compiler", type=Path, help=f"use the mwccarm in this directory, which holds {MWCC_VERSION}/")
    args = parser.parse_args()

    os.chdir(ROOT)
    version = args.version
    info = VERSIONS[version]
    system, machine = host()
    exe = ".exe" if system == "windows" else ""
    tools_dir = ROOT / "tools" / "bin"

    # Tools
    if args.dsd:
        dsd = args.dsd
    else:
        dsd = (tools_dir / f"dsd{exe}").relative_to(ROOT)
        if (system, machine) not in DSD_ASSETS:
            sys.exit(f"dsd has no release for {system} {machine}: build it and pass --dsd")
        asset, digest = DSD_ASSETS[(system, machine)]
        download_binary(ROOT / dsd, DSD_URL.format(version=DSD_VERSION, asset=asset), digest)
    if args.objdiff:
        objdiff = args.objdiff
    else:
        objdiff = (tools_dir / f"objdiff-cli{exe}").relative_to(ROOT)
        if (system, machine) not in OBJDIFF_ASSETS:
            sys.exit(f"objdiff-cli has no release for {system} {machine}: build it and pass --objdiff")
        asset, digest = OBJDIFF_ASSETS[(system, machine)]
        download_binary(ROOT / objdiff, OBJDIFF_URL.format(version=OBJDIFF_VERSION, asset=asset), digest)
    if args.sjiswrap:
        sjiswrap = args.sjiswrap
    else:
        sjiswrap = (tools_dir / "sjiswrap.exe").relative_to(ROOT)
        download_binary(ROOT / sjiswrap, SJISWRAP_URL.format(version=SJISWRAP_VERSION), SJISWRAP_SHA256)
    if args.compiler:
        compiler_dir = args.compiler / MWCC_VERSION
        problems = check_compiler(compiler_dir)
        if problems:
            sys.exit("\n".join(problems))
    else:
        compiler_dir = download_compiler(tools_dir).relative_to(ROOT)
    mwcc = compiler_dir / "mwccarm.exe"
    mwld = compiler_dir / "mwldarm.exe"
    wrapper = find_wrapper(args.wrapper, system, machine, tools_dir)

    # Paths
    config_dir = Path("config") / version
    arm9_config = config_dir / "arm9" / "config.yaml"
    build_dir = Path("build") / version
    extract_dir = Path("extract") / version
    extract_config = extract_dir / "config.yaml"
    baserom = Path("orig") / f"baserom_{version}.nds"
    rom = build_dir / f"{GAME}_{version}.nds"
    elf = build_dir / "arm9.o"
    report = build_dir / "report.json"
    bios = Path("arm7_bios.bin")
    configs = config_files(ROOT / config_dir)

    # What dsd delinks and what gets linked, known ahead of time from the configs
    result = subprocess.run([str(dsd), "json", "delinks", "--config-path", str(arm9_config)],
                            capture_output=True, text=True)
    if result.returncode != 0:
        sys.exit(f"dsd json delinks failed:\n{result.stderr.strip()}")
    delinks = json.loads(result.stdout)
    files = delinks["files"]
    lcf_file = Path(delinks["arm9_lcf_file"])
    objects_file = Path(delinks["arm9_objects_file"])
    delink_outputs = sorted({f["delink_file"] for f in files})
    link_inputs = sorted({f["object_to_link"] for f in files})

    # A source file takes part in the build once a delinks.txt names it
    listed = {f["name"] for f in files if f["name"].endswith(SOURCE_SUFFIXES)}
    for name in sorted(listed):
        if not Path(name).exists():
            sys.exit(f"{name} is in a delinks.txt but does not exist")
    for directory in ("src", "lib"):
        for path in sorted(Path(directory).rglob("*")):
            if path.suffix in SOURCE_SUFFIXES and path.as_posix() not in listed:
                print(f"warning: {path} is in no delinks.txt, so it is not built")

    n = Writer()
    n.comment("Generated by configure.py, do not edit")
    n.variable("ninja_required_version", "1.7")
    n.variable("python", shlex.quote(sys.executable))
    n.variable("dsd", f"{shlex.quote(os.path.join('.', str(dsd)))} --force-color")
    n.variable("objdiff", shlex.quote(os.path.join(".", str(objdiff))))
    # Silence wine, and the Vulkan layer it loads on macOS
    n.variable("wrapper", f"WINEDEBUG=-all MVK_CONFIG_LOG_LEVEL=0 {shlex.quote(wrapper)}" if wrapper else "")
    n.variable("sjiswrap", shlex.quote(str(sjiswrap)))
    n.variable("mwcc", shlex.quote(str(mwcc)))
    n.variable("mwld", shlex.quote(str(mwld)))
    n.variable("cc_flags", " ".join(CC_FLAGS + ENCODING_FLAGS))
    n.variable("ld_flags", " ".join(LD_FLAGS))
    n.variable("includes", " ".join(f"-i {d}" for d in INCLUDE_DIRS))
    n.variable("defines", " ".join(f"-d {d}" for d in info["defines"]))
    n.variable("config", arm9_config)
    # With the ARM7 BIOS, dsd can encrypt the secure area and so write its checksum, the last four bytes of a match
    n.variable("bios", f"--arm7-bios {bios}" if bios.exists() else "")
    n.newline()

    n.rule("configure", f"$python configure.py {subprocess.list2cmdline(sys.argv[1:])}", "Configuring", generator="1")
    n.rule("check_baserom", f"$python tools/check_rom.py base $in --sha1 {info['sha1']} --stamp $out",
           "Checking $in")
    n.rule("extract", "$dsd rom extract --rom $in --output-path $extract_dir $bios", "Extracting $in")
    n.rule("delink", "$dsd delink --config-path $config", "Delinking")
    n.rule("lcf", "$dsd lcf --config-path $config", "Generating the linker script")
    # mwccarm writes the dependency file next to the object, with Windows paths that fix_depfile.py converts
    n.rule("mwcc", "$wrapper $sjiswrap $mwcc $cc_flags $lang $defines $includes -gccdep -MD -c $in -o $out "
           "&& $python tools/fix_depfile.py $dep", "Compiling $in", depfile="$dep", deps="gcc")
    # The includes and code of a source file as one file, which objdiff sends along to a decomp.me scratch
    n.rule("ctx", "$wrapper $mwcc $lang -gccinc $defines $includes -EP $in | $python tools/clean_ctx.py > $out",
           "Preprocessing $in")
    n.rule("mwld", "$wrapper $mwld $ld_flags @$objects $lcf -o $out", "Linking $out")
    n.rule("rom_config", "$dsd rom config --elf $in --config $config", "Configuring the ROM")
    n.rule("rom_build", "$dsd rom build --config $in --rom $out $bios", "Building $out")
    n.rule("check_modules", "$dsd check modules --config-path $config --fail && touch $out", "Checking modules")
    n.rule("check_symbols", "$dsd check symbols --config-path $config --elf-path $in --fail --max-lines 20 "
           "&& touch $out", "Checking symbols")
    n.rule("check_rom", "$python tools/check_rom.py built $in --base $base --stamp $out", "Checking $in")
    decomp_me_flags = " ".join(CC_FLAGS)
    n.rule("objdiff_config", f"$python tools/objdiff_config.py --dsd {shlex.quote(str(dsd))} --config $config "
           f"--compiler {DECOMP_ME_COMPILER} --c-flags {shlex.quote(decomp_me_flags)} -o $out",
           "Writing $out")
    n.rule("report", "$objdiff report generate -p . -o $out", "Generating $out")
    n.rule("disassemble", "$dsd dis --config-path $config --asm-path $asm_dir --ual && touch $out", "Disassembling")

    n.build(["build.ninja"], "configure", implicit=["configure.py", *configs])

    # Base ROM -> extracted files -> delinked objects and linker script
    baserom_ok = build_dir / "baserom.ok"
    n.build([baserom_ok], "check_baserom", [baserom], implicit=["tools/check_rom.py"])
    n.build([extract_config], "extract", [baserom], implicit=[baserom_ok], variables={"extract_dir": extract_dir})
    n.build(delink_outputs, "delink", configs, implicit=[extract_config])
    n.build([lcf_file, objects_file], "lcf", configs, implicit=[extract_config])

    # Decompiled sources. Each is compiled whether or not its delinks.txt entry is marked complete: a complete file's
    # object is linked into the ROM, and an incomplete one's is only compared with the original by objdiff.
    compiled = []
    for name in sorted(listed):
        source = Path(name)
        obj = build_dir / source.with_suffix(".o")
        n.build([obj], "mwcc", [source], implicit=["tools/fix_depfile.py"],
                variables={"lang": LANG_FLAGS[source.suffix], "dep": obj.with_suffix(".d")})
        compiled.append(obj)
        n.build([obj.with_suffix(f".ctx{source.suffix}")], "ctx", [source], implicit=["tools/clean_ctx.py"],
                variables={"lang": LANG_FLAGS[source.suffix]})

    n.build(["compile"], "phony", compiled)

    # Link and build the ROM
    n.build([elf], "mwld", link_inputs, implicit=[lcf_file, objects_file],
            variables={"lcf": lcf_file, "objects": objects_file})
    rom_config = build_dir / "build" / "rom_config.yaml"
    n.build([rom_config], "rom_config", [elf], implicit=[extract_config])
    n.build([rom], "rom_build", [rom_config])

    # Checks: every module's bytes, every symbol's address, and the ROM as a whole
    modules_ok = build_dir / "modules.ok"
    symbols_ok = build_dir / "symbols.ok"
    rom_ok = build_dir / "rom.ok"
    n.build([modules_ok], "check_modules", [elf], implicit=configs)
    n.build([symbols_ok], "check_symbols", [elf], implicit=configs)
    n.build([rom_ok], "check_rom", [rom], implicit=[baserom, "tools/check_rom.py"], variables={"base": baserom})
    n.build(["check"], "phony", [modules_ok, symbols_ok, rom_ok])
    n.build(["rom"], "phony", [rom])

    # objdiff: the config for the GUI, and the progress report that ByteMatch reads
    n.build(["objdiff.json"], "objdiff_config", configs, implicit=["tools/objdiff_config.py", *compiled])
    n.build([report], "report", ["objdiff.json"], implicit=[*delink_outputs, *compiled])
    n.build(["report"], "phony", [report])

    # A listing of the whole game to read, not to assemble
    asm_ok = build_dir / "asm.ok"
    n.build([asm_ok], "disassemble", configs, implicit=[extract_config], variables={"asm_dir": build_dir / "asm"})
    n.build(["dis"], "phony", [asm_ok])

    n.default(["objdiff.json", "check", *compiled])

    Path("build.ninja").write_text(n.out.getvalue(), encoding="utf-8")
    print(f"Wrote build.ninja for {version}: {len(compiled)} source files, {len(delink_outputs)} delinked objects")


if __name__ == "__main__":
    main()
