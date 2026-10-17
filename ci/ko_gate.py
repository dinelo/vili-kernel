#!/usr/bin/env python3
"""Verifica modulele externe (ath9k_htc & co) contra kernelului STOCK, fara a-l inlocui.
usage: ko_gate.py stock_kernel_crcs.txt expected_vermagic report.txt mod1.ko [mod2.ko ...]
Fiecare simbol importat (__versions) trebuie sa existe in kernelul stock cu acelasi CRC,
sau sa fie exportat de unul din modulele din set. module_layout si vermagic trebuie sa fie identice."""
import sys, struct, os
def sections(d):
    shoff, = struct.unpack_from('<Q', d, 0x28); shentsize, shnum, shstrndx = struct.unpack_from('<HHH', d, 0x3A)
    secs = [struct.unpack_from('<IIQQQQIIQQ', d, shoff + i * shentsize) for i in range(shnum)]
    stro = secs[shstrndx][4]
    return {d[stro + s[0]: d.index(b'\0', stro + s[0])].decode(): d[s[4]: s[4] + s[5]] for s in secs}
def info(path):
    d = open(path, 'rb').read(); s = sections(d)
    v = s.get('__versions', b'')
    vers = {v[i + 8:i + 64].split(b'\0')[0].decode(): struct.unpack_from('<Q', v, i)[0] for i in range(0, len(v), 64)}
    mi = dict(x.decode().split('=', 1) for x in s.get('.modinfo', b'').split(b'\0') if b'=' in x)
    exp = set(x.decode() for x in s.get('__ksymtab_strings', b'').split(b'\0') if x)
    return vers, mi, exp
stock_f, want_vm, rep = sys.argv[1:4]; kos = sys.argv[4:]
stock = {l.split('\t')[1].strip(): int(l.split('\t')[0], 16) for l in open(stock_f) if '\t' in l}
mods = {os.path.basename(k)[:-3]: info(k) for k in kos}
internal = set().union(*[m[2] for m in mods.values()])
out = []; bad = 0; total = 0; matched = 0
for name, (vers, mi, exp) in mods.items():
    vm = mi.get('vermagic', '')
    deps = [x for x in mi.get('depends', '').split(',') if x]
    nb = 0
    for sym, crc in sorted(vers.items()):
        total += 1
        if sym in internal and sym not in stock:
            matched += 1; continue
        if stock.get(sym) == crc: matched += 1; continue
        nb += 1; out.append(f'  {name}: {sym} ours={crc:#010x} stock={stock[sym]:#010x}' if sym in stock else f'  {name}: {sym} NOT EXPORTED by stock kernel')
    ok_vm = vm == want_vm
    ok_deps = all(d in mods for d in deps)
    out.insert(0, f'{name}.ko: imports={len(vers)} bad={nb} module_layout={vers.get("module_layout", 0):#x} vermagic_ok={ok_vm} depends={deps} deps_in_set={ok_deps}')
    bad += nb + (0 if ok_vm else 1) + (0 if ok_deps else 1)
    if vers.get('module_layout') != stock.get('module_layout'): bad += 1
out.insert(0, f'TOTAL: {matched}/{total} imported CRCs match the STOCK kernel ({len(stock)} stock exports); problems={bad}')
txt = '\n'.join(out) + '\n'; open(rep, 'w').write(txt); print(txt)
sys.exit(1 if bad else 0)
