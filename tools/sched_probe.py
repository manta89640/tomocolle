#!/usr/bin/env python3
"""Shows what mwccarm's instruction scheduler saw and decided, block by block, and replays it with a model.

    python3 tools/sched_probe.py show src/scene/SceneManager.cpp [--pre|--post] [--grep TEXT]
    python3 tools/sched_probe.py check src/scene/SceneManager.cpp

It runs the compiler under wibo: set WIBO to a wibo executable (1.2.0 works; the source must be ASCII or Shift JIS,
since wibo cannot run sjiswrap). Under the build's wine the patched compiler crashed (not investigated).

`show` compiles the file with a patched copy of the game's compiler (2.0 sp2p3, build/sched_probe/mwccarm.exe,
made from tools/bin on first use). The copy behaves exactly like the original and also appends to
build/sched_probe/sched.log, for every block it schedules, the dependence graph (each instruction in block order with
its latency, rank, height, latest cycle, number of predecessors and its successor edges with their latencies) and
then each choice it makes (cycle, instruction, and the register-pressure counter at that moment). Virtual registers
are numbered as the code generator created them (r0-r15 are physical, v16 and up virtual).

`check` compiles the same way and replays every logged block with the model in docs/decompiling.md ("How the
compiler orders and colours"), and counts the blocks the model reproduces cycle for cycle.

The patch: two calls in the scheduler's schedule_block (0x4ff95b, 0x4ff9c4) are redirected to a stub that calls the
original function and writes what it sees; the stub is placed over the ARM11 machine model (0x4f5690), which the
arm946e target never uses. The stub is STUB below, assembled from tools/sched_probe_stub.s.
"""
import argparse
import os
import re
import shlex
import struct
import subprocess
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SOURCE_EXE = ROOT / 'tools/bin/mwccarm/2.0/sp2p3/mwccarm.exe'
OUT_DIR = ROOT / 'build/sched_probe'
EXE_SHA1 = '44e2f4ed40effd7d5a1dfcd6864377adc3432374'

CODE = 0x4f5690
STR = 0x4f5b00
STUB2 = 302
STRINGS = [b'a\0', b'B pre=%d\n\0', b'N %x i=%x op=%x lat=%d rank=%d earl=%d latest=%d h=%d np=%d\0',
           b' o%d/%d/%x/%d\0', b' E%x:%d\0', b'\n\0', b'S %d %x live=%d lim=%d\n\0']
STUB = bytes.fromhex(
    'b9d0f64f00ffd160833d002f5f0000751968005b4f0068785b4f00b990f34000ffd183c408a3002f5f00a16ca363005089e0'
    '5068025b4f00ff35002f5f00b9207f4000ffd183c4108b35ba66610085f60f84c60000008b7e0c0fb7461a500fb74618500f'
    'b74616500fb74614500fb74612500fb74610500fbf472850575689e050680c5b4f00ff35002f5f00b9207f4000ffd183c430'
    '0fbf6f2c8d5f2e85ed7e310fbf4304500fb74302500fbe4301500fb6035089e05068485b4f00ff35002f5f00b9207f4000ff'
    'd183c41c83c30e4debcb8b5e0885db74240fbf430850ff730489e05068565b4f00ff35002f5f00b9207f4000ffd183c4148b'
    '1bebd889e050685e5b4f00ff35002f5f00b9207f4000ffd183c40c8b36e932ffffffff35002f5f00b990f24000ffd183c404'
    '61c3ff742408ff742408b930f74f00ffd183c40860833d002f5f0000751968005b4f0068785b4f00b990f34000ffd183c408'
    'a3002f5f008b44241c8b4c24288b15806661008b1d7c6661005352505189e05068605b4f00ff35002f5f00b9207f4000ffd1'
    '83c41c61c3')


class PE:
    def __init__(self, data):
        self.d = data
        pe = struct.unpack_from('<I', data, 0x3c)[0]
        nsec = struct.unpack_from('<H', data, pe + 6)[0]
        opt = struct.unpack_from('<H', data, pe + 20)[0]
        self.base = struct.unpack_from('<I', data, pe + 24 + 28)[0]
        self.secs = []
        for i in range(nsec):
            o = pe + 24 + opt + 40 * i
            vs, va, rs, ra = struct.unpack_from('<IIII', data, o + 8)
            self.secs.append((va, vs, ra, rs))

    def off(self, v):
        v -= self.base
        for va, vs, ra, rs in self.secs:
            if va <= v < va + max(vs, rs):
                return ra + v - va
        raise ValueError(hex(v))

    def cstr(self, v):
        o = self.off(v)
        return self.d[o:self.d.index(b'\0', o)].decode('latin1')


def opcode_names(pe):
    """The ARM opcode table: 0x1c-byte entries, the mnemonic's format string first."""
    names = {}
    v = 0x5e7794
    for i in range(600):
        try:
            names[i] = pe.cstr(struct.unpack_from('<I', pe.d, pe.off(v + 0x1c * i))[0]).replace('%s', '')
        except (ValueError, UnicodeDecodeError):
            break
    return names


def build_probe():
    exe = OUT_DIR / 'mwccarm.exe'
    log = OUT_DIR / 'sched.log'
    if exe.exists():
        return exe, log
    import hashlib
    data = SOURCE_EXE.read_bytes()
    if hashlib.sha1(data).hexdigest() != EXE_SHA1:
        sys.exit(f'{SOURCE_EXE} is not the compiler this probe was written for')
    pe = PE(data)
    b = bytearray(data)
    blob = b''.join(STRINGS) + str(log.relative_to(ROOT)).replace('/', '\\').encode() + b'\0'  # the compiler runs in ROOT
    assert STR + len(blob) < 0x4f5c70, 'log path too long'
    b[pe.off(CODE):pe.off(CODE) + len(STUB)] = STUB
    b[pe.off(STR):pe.off(STR) + len(blob)] = blob
    for at, target in ((0x4ff95b, CODE), (0x4ff9c4, CODE + STUB2)):
        o = pe.off(at)
        assert b[o] == 0xe8
        b[o + 1:o + 5] = struct.pack('<i', target - (at + 5))
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    for f in SOURCE_EXE.parent.iterdir():
        if f.suffix.lower() == '.dll':
            (OUT_DIR / f.name).write_bytes(f.read_bytes())
    exe.write_bytes(bytes(b))
    return exe, log


def ninja_vars():
    v = {}
    for line in (ROOT / 'build.ninja').read_text().splitlines():
        m = re.match(r'^(\w+) = (.*)$', line)
        if m:
            v[m.group(1)] = m.group(2)
    return v


def compile_logged(source, extra=()):
    exe, log = build_probe()
    if log.exists():
        log.unlink()
    v = ninja_vars()
    lang = '-lang=c' if str(source).endswith('.c') else '-lang=c++'
    out = OUT_DIR / 'probe.o'
    wibo = os.environ.get('WIBO')
    if not wibo:
        sys.exit('set WIBO to a wibo executable (see the header of this file)')
    prefix, flags = f'timeout 60 {wibo}', v['cc_flags'].replace('-enc SJIS', '')
    cmd = (f"{prefix} {exe.relative_to(ROOT)} {flags} {lang} -ipa file "
           f"{v['defines']} {v['includes']} {' '.join(shlex.quote(e) for e in extra)} -c {shlex.quote(str(source))} "
           f"-o {out.relative_to(ROOT)}")
    r = subprocess.run(cmd, shell=True, cwd=ROOT, capture_output=True, text=True)
    if r.returncode:
        sys.exit(r.stdout + r.stderr)
    return parse(log.read_text())


def parse(text):
    blocks = []
    cur = None
    for line in text.splitlines():
        if line.startswith('B '):
            cur = {'pre': int(line.split('=')[1]), 'nodes': [], 'sel': []}
            blocks.append(cur)
        elif line.startswith('N '):
            m = re.match(r'N (\w+) i=\w+ op=(\w+) lat=(\d+) rank=(\d+) earl=\d+ latest=(\d+) h=(\d+) np=(\d+)(.*)', line)
            cur['nodes'].append({
                'addr': m.group(1), 'op': int(m.group(2), 16), 'lat': int(m.group(3)), 'rank': int(m.group(4)),
                'latest': int(m.group(5)), 'h': int(m.group(6)), 'np': int(m.group(7)),
                'ops': [tuple(int(x, 16) if k == 2 else int(x) for k, x in enumerate(o.split('/')))
                        for o in re.findall(r' o([\d/a-f-]+)', m.group(8))],
                'edges': [(a, int(x)) for a, x in re.findall(r' E(\w+):(-?\d+)', m.group(8))]})
        elif line.startswith('S '):
            f = line.split()
            cur['sel'].append((int(f[1]), f[2], int(f[3][5:])))
    return blocks


NAMES = {}


def render(n):
    ops = []
    for kind, cls, flags, reg in n['ops']:
        if kind == 0:
            ops.append(('*' if flags & 2 else '') + (f'r{reg}' if reg < 16 else f'v{reg}'))
        elif kind == 2:
            ops.append(f'#{flags:x}')
        elif kind != 4:
            ops.append(f'k{kind}:{reg}')
    return f"{NAMES.get(n['op'], hex(n['op']))} {','.join(ops)}"


def show(b):
    idx = {n['addr']: i for i, n in enumerate(b['nodes'])}
    print(f"--- block ({'pre-RA' if b['pre'] else 'post-RA'}), {len(b['nodes'])} instructions")
    for i, n in enumerate(b['nodes']):
        e = ' '.join(f"{idx.get(a, '?')}:{v}" for a, v in n['edges'])
        print(f"  {i:3d} {render(n):36s} lat={n['lat']} rank={n['rank']} h={n['h']:2d} latest={n['latest']:2d} "
              f"preds={n['np']}  -> {e}")
    print('  issue (cycle:instruction[pressure]): ' +
          ' '.join(f'{c}:{idx[a]}[{lv}]' for c, a, lv in b['sel'] if a != '0'))


# ---- the model ----------------------------------------------------------------------------------------------------

SPECIAL = {13, 15}      # sp and pc: never counted
LIMIT = 14              # allocatable GPRs: r0-r12 and lr
LOADS = {0x80, 0x81, 0x82, 0xa5, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xf6, 0xf7}


def issue_cycles(n):
    op = n['op']
    if 0x85 <= op <= 0xa4 or 0xce <= op <= 0xed:     # ldm, stm: a cycle per register, at least two
        c = len(n['ops']) - 2
        return 2 if c == 1 else c
    if op in LOADS:
        return 1
    return n['lat']                                   # otherwise latency and issue cycles are the same number


def regs(n, bit):
    return [reg for kind, cls, fl, reg in n['ops'] if kind == 0 and fl & bit and reg not in SPECIAL]


def schedule(nodes, pre, live, no_pressure=False):
    n = len(nodes)
    idx = {x['addr']: i for i, x in enumerate(nodes)}
    succ = [[(idx[a], v) for a, v in x['edges'] if a in idx] for x in nodes]
    npred = [0] * n
    for i in range(n):
        for j, _ in succ[i]:
            npred[j] += 1
    uses = Counter()
    for x in nodes:
        for r in regs(x, 1):
            uses[r] += 1
    earliest = [0] * n
    done = [False] * n
    out = []
    cycle = busy = 0

    def delta(i):
        x = nodes[i]
        e = 16
        for r in regs(x, 1):
            uses[r] -= 1
        for kind, cls, fl, r in x['ops']:
            if kind == 0 and fl & 1 and r not in SPECIAL and uses[r] == 0:
                e -= 1 + max(1, live - LIMIT)
        for kind, cls, fl, r in x['ops']:
            if kind == 0 and fl & 2 and not fl & 1 and r not in SPECIAL and uses[r]:
                e += max(1, live - LIMIT)
        for r in regs(x, 1):
            uses[r] += 1
        return e

    def frees(i):
        return sum(1 for j, _ in succ[i] if npred[j] == 1)

    while len(out) < n:
        if busy == 0:
            ready = [i for i in range(n) if not done[i] and npred[i] == 0 and earliest[i] <= cycle]
            if ready:
                if pre and live >= LIMIT and not no_pressure:
                    best = min(ready, key=lambda i: (delta(i), i))
                else:
                    best = ready[0]
                    for c in ready[1:]:
                        bc, cc = nodes[best]['latest'] <= cycle, nodes[c]['latest'] <= cycle
                        if bc != cc:
                            best = c if cc else best
                            continue
                        fb, fc = frees(best), frees(c)
                        if fb != fc:
                            best = c if fc > fb else best
                            continue
                        hb, hc = nodes[best]['h'], nodes[c]['h']
                        if hb != hc:
                            best = c if hc > hb else best
                            continue
                        if pre and nodes[c]['rank'] < nodes[best]['rank']:
                            best = c
                out.append((cycle, best))
                done[best] = True
                for j, v in succ[best]:
                    npred[j] -= 1
                    earliest[j] = max(earliest[j], cycle + v)
                x = nodes[best]
                for r in regs(x, 1):
                    uses[r] -= 1
                    if uses[r] == 0:
                        live -= 1
                for kind, cls, fl, r in x['ops']:
                    if kind == 0 and fl & 2 and not fl & 1 and r not in SPECIAL and uses[r] > 0:
                        live += 1
                busy = issue_cycles(x)
        cycle += 1
        if busy:
            busy -= 1
    return out


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('mode', choices=['show', 'check'])
    p.add_argument('source')
    p.add_argument('--pre', action='store_true', help='only the blocks scheduled before register allocation')
    p.add_argument('--post', action='store_true', help='only the blocks scheduled after it')
    p.add_argument('--grep', help='only blocks with an instruction whose rendering contains this')
    args, extra = p.parse_known_args()
    NAMES.update(opcode_names(PE(SOURCE_EXE.read_bytes())))
    blocks = compile_logged(Path(args.source).resolve(), extra)
    if args.mode == 'show':
        for b in blocks:
            if (args.pre and not b['pre']) or (args.post and b['pre']):
                continue
            if args.grep and not any(args.grep in render(n) for n in b['nodes']):
                continue
            show(b)
    else:
        ok = 0
        for b in blocks:
            sel = [(c, a, lv) for c, a, lv in b['sel'] if a != '0']
            idx = {x['addr']: i for i, x in enumerate(b['nodes'])}
            want = [(c, idx[a]) for c, a, lv in sel]
            ok += schedule(b['nodes'], b['pre'], sel[0][2] if sel else 0) == want
        print(f'{ok} of {len(blocks)} blocks reproduced cycle for cycle')


if __name__ == '__main__':
    main()
