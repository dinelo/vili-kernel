#!/usr/bin/env python3
"""CRC gate: compare built Module.symvers (+ module_layout of built .ko) with stock vendor CRCs.
usage: crc_gate.py stock_crcs.txt Module.symvers report.txt [ko_dir] [stock_exports.txt]
exit 1 if module_layout differs or more than MAX_DIFF CRCs differ."""
import sys, os, struct, glob
MAX_DIFF = int(os.environ.get('CRC_MAX_DIFF', '5'))
ALLOW_MISSING = set(x for x in os.environ.get('CRC_ALLOW_MISSING', '').split(',') if x)
stock_f, symvers_f, report_f = sys.argv[1:4]
ko_dir = sys.argv[4] if len(sys.argv) > 4 else None
exp_f = sys.argv[5] if len(sys.argv) > 5 else None
stock = {}
for l in open(stock_f):
    p = l.split()
    if len(p) >= 2: stock[p[1]] = int(p[0], 16)
ours = {}
for l in open(symvers_f):
    p = l.rstrip('\n').split('\t')
    if len(p) >= 2: ours[p[1]] = int(p[0], 16)
missing = sorted(s for s in stock if s not in ours and s not in ALLOW_MISSING)
allowed = sorted(s for s in stock if s not in ours and s in ALLOW_MISSING)
diff = sorted(s for s in stock if s in ours and ours[s] != stock[s])
same = len(stock) - len(missing) - len(diff) - len(allowed)
out = []
out.append(f'stock CRCs: {len(stock)}  match: {same}  differ: {len(diff)}  missing in our kernel: {len(missing)}')
ml_stock = stock.get('module_layout'); ml_ours = ours.get('module_layout')
out.append('module_layout stock=%s ours=%s' % (hex(ml_stock) if ml_stock is not None else None, hex(ml_ours) if ml_ours is not None else None))
# module_layout as actually recorded in built modules' __versions
def ko_versions(path):
    d = open(path, 'rb').read()
    # minimal ELF64 LE section parser
    shoff, = struct.unpack_from('<Q', d, 0x28); shentsize, shnum, shstrndx = struct.unpack_from('<HHH', d, 0x3A)
    secs = [struct.unpack_from('<IIQQQQIIQQ', d, shoff + i * shentsize) for i in range(shnum)]
    stro = secs[shstrndx][4]
    for s in secs:
        name = d[stro + s[0]: d.index(b'\0', stro + s[0])].decode()
        if name == '__versions':
            v = d[s[4]: s[4] + s[5]]
            return {v[i + 8:i + 64].split(b'\0')[0].decode(): struct.unpack_from('<Q', v, i)[0] for i in range(0, len(v), 64)}
    return {}
ko_bad = []
if ko_dir:
    kos = sorted(glob.glob(os.path.join(ko_dir, '**', '*.ko'), recursive=True))
    for k in kos:
        v = ko_versions(k).get('module_layout')
        if v is not None and v != ml_stock: ko_bad.append(f'{os.path.basename(k)}={v:#x}')
    out.append(f'built .ko checked: {len(kos)}  with module_layout != stock: {len(ko_bad)}')
    out += ['  ' + x for x in ko_bad[:50]]
out.append('--- differing CRCs (symbol stock ours) ---')
out += [f'{s} {stock[s]:#010x} {ours[s]:#010x}' for s in diff]
out.append('--- allowed-missing (known, non-critical): ' + ' '.join(allowed))
out.append('--- stock-imported symbols NOT exported by our kernel ---')
out += missing
if exp_f and os.path.exists(exp_f):
    se = set(x.strip() for x in open(exp_f) if x.strip())
    lost = sorted(se - set(ours))
    out.append(f'--- info: {len(lost)} of {len(se)} stock kernel exports absent from our Module.symvers ---')
    out += lost
txt = '\n'.join(out) + '\n'
open(report_f, 'w').write(txt)
print('\n'.join(out[:12 + len(ko_bad[:50])]))
print('(full list in', report_f, ')')
fail = []
if ml_ours != ml_stock: fail.append('module_layout mismatch in Module.symvers')
if ko_bad: fail.append('module_layout mismatch in built .ko')
if len(diff) + len(missing) > MAX_DIFF: fail.append(f'{len(diff)} differing + {len(missing)} missing CRCs > {MAX_DIFF}')
if fail:
    print('CRC GATE FAILED: ' + '; '.join(fail)); sys.exit(1)
print('CRC GATE PASSED')
