"""Register a generated riding sprite in the server's ridePetTable.

The client packer (pack.py) creates the sprite data under a new GraphNo. The
server still needs a row {rideNo, charNo, petNo, petId} in ridePetTable
(server/gmsv/char/char_base.c) so CHAR_PET_GetRideNo() can resolve it.

petId is the pet's id in data/enemy1.txt (the same number GM `petmake` uses).
It can be resolved automatically from B's GraphNo:
  B -> enemybase1.txt column 37 (E_T_IMGNUMBER) -> tempno (column 7)
    -> enemy1.txt column 5 (ENEMY_TEMPNO)   -> petId (column 4)

Dry-run by default; pass --write to modify char_base.c (a .bak backup is
created first). The file is edited byte-wise so CRLF line endings survive.
"""

from __future__ import annotations

import argparse
import re
from pathlib import Path

DECL_RE = re.compile(rb'tagRidePetTable\s+ridePetTable\[(\d+)\]\s*=\s*\{')
ENTRY_RE = re.compile(r'\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\}')
# Macros that are known OFF in server/gmsv/include/version.h. Entries inside
# `#ifdef OFF_MACRO` blocks do not count against the declared array size.
OFF_MACROS = {'_GM_METAMO_RIDE'}

REPO = Path(__file__).resolve().parents[2]
DEFAULT_CHAR_BASE = REPO / 'server/gmsv/char/char_base.c'
DEFAULT_DATA_DIR = REPO / 'server/build/windows/bin/data'


def load_text(path: Path) -> str:
    raw = path.read_bytes()
    for encoding in ('utf-8', 'gbk'):
        try:
            return raw.decode(encoding)
        except UnicodeDecodeError:
            continue
    return raw.decode('utf-8', errors='replace')


def find_pet_candidates(data_dir: Path, name: str | None = None,
                        imgno: int | None = None) -> list[dict]:
    """Cross-reference enemybase1.txt and enemy1.txt.

    enemybase1.txt: col 1 name, col 7 E_T_TEMPNO, col 37 E_T_IMGNUMBER.
    enemy1.txt:     col 1 name, col 4 ENEMY_ID,  col 5 ENEMY_TEMPNO.
    """
    tempno_to_base = {}
    for line in load_text(data_dir / 'enemybase1.txt').splitlines():
        row = line.split(',')
        if len(row) > 37 and row[6].strip().isdigit():
            tempno_to_base[row[6].strip()] = (row[0], row[36].strip())
    candidates = []
    for line in load_text(data_dir / 'enemy1.txt').splitlines():
        row = line.split(',')
        if len(row) <= 4 or not row[3].strip().isdigit():
            continue
        tempno = row[4].strip()
        base = tempno_to_base.get(tempno)
        if base is None:
            continue
        base_name, base_imgno = base
        if name and name not in row[0] and name not in base_name:
            continue
        if imgno is not None and base_imgno != str(imgno):
            continue
        candidates.append({'pet_id': int(row[3]), 'name': row[0],
                           'tempno': int(tempno), 'imgno': base_imgno})
    return candidates


def analyze_table(raw: bytes, decl: re.Match):
    """Return (entries, active_count, closing_brace_pos)."""
    end = raw.index(b'\n};', decl.end())
    body = raw[decl.end():end].decode('ascii', errors='replace')
    entries = []
    active = 0
    skip_stack = []
    for line in body.splitlines():
        stripped = line.strip()
        directive = re.match(r'#\s*(ifdef|ifndef|else|endif)\s*(\w*)', stripped)
        if directive:
            kind, macro = directive.groups()
            if kind == 'endif':
                if skip_stack:
                    skip_stack.pop()
            elif kind == 'else':
                if skip_stack:
                    skip_stack[-1] = not skip_stack[-1]
            else:
                off = macro in OFF_MACROS
                skip_stack.append(off if kind == 'ifdef' else not off)
                if macro and macro not in OFF_MACROS:
                    print(f'note: #{kind} {macro} assumed ON; verify against version.h')
            continue
        found = ENTRY_RE.findall(stripped)
        if not found:
            continue
        entries.extend((int(a), int(b), int(c), int(d)) for a, b, c, d in found)
        if not stripped.startswith('//') and not any(skip_stack):
            active += len(found)
    return entries, active, end


def build_row(ride: int, a: int, b: int, pet_id: int, comment: str | None,
              eol: bytes) -> bytes:
    row = f'    {{{ride}, {a}, {b}, {pet_id}}},'
    if comment:
        row += f'  /* {comment} */'
    return row.encode('ascii') + eol


def main():
    parser = argparse.ArgumentParser(
        description='Register a packed riding sprite in the server ridePetTable.')
    parser.add_argument('--ride', type=int, help='new ride GraphNo (from pack.py)')
    parser.add_argument('--a', type=int, help='A character GraphNo')
    parser.add_argument('--b', type=int, help='B pet GraphNo')
    parser.add_argument('--pet-id', type=int,
                        help='pet id in enemy1.txt; resolved from --b when omitted')
    parser.add_argument('--comment', help='short ASCII-free comment, e.g. the pet name')
    parser.add_argument('--char-base', type=Path, default=DEFAULT_CHAR_BASE)
    parser.add_argument('--data-dir', type=Path, default=DEFAULT_DATA_DIR)
    parser.add_argument('--find-pet', metavar='NAME',
                        help='only search pet id candidates by name substring')
    parser.add_argument('--write', action='store_true',
                        help='apply the change (default is dry-run)')
    args = parser.parse_args()

    if args.find_pet:
        hits = find_pet_candidates(args.data_dir, name=args.find_pet)
        if not hits:
            parser.error(f'no pet matching {args.find_pet!r}')
        for hit in hits:
            print(f"pet_id={hit['pet_id']:<6} imgno={hit['imgno']:<8} "
                  f"tempno={hit['tempno']:<6} {hit['name']}")
        return

    for field in ('ride', 'a', 'b'):
        if getattr(args, field) is None:
            parser.error(f'--{field} is required')

    pet_id = args.pet_id
    if pet_id is None:
        hits = find_pet_candidates(args.data_dir, imgno=args.b)
        if not hits:
            parser.error(f'cannot resolve petId for GraphNo {args.b}; pass --pet-id '
                         f'(use --find-pet NAME to search)')
        if len(hits) > 1:
            print(f'multiple pets use GraphNo {args.b}; pick one with --pet-id:')
            for hit in hits:
                print(f"  pet_id={hit['pet_id']:<6} tempno={hit['tempno']:<6} {hit['name']}")
            raise SystemExit(1)
        pet_id = hits[0]['pet_id']
        print(f"resolved petId={pet_id} ({hits[0]['name']}, tempno={hits[0]['tempno']})")

    raw = args.char_base.read_bytes()
    decl = DECL_RE.search(raw)
    if not decl:
        parser.error(f'ridePetTable declaration not found in {args.char_base}')
    capacity = int(decl.group(1))
    entries, active, end = analyze_table(raw, decl)

    for ride_no, char_no, pet_no, old_pet_id in entries:
        if ride_no == args.ride:
            parser.error(f'rideNo {args.ride} already used by '
                         f'(charNo={char_no}, petNo={pet_no})')
        if char_no == args.a and pet_no == args.b:
            parser.error(f'(A={args.a}, B={args.b}) already registered as '
                         f'rideNo {ride_no}')

    eol = b'\r\n' if b'\r\n' in raw else b'\n'
    row = build_row(args.ride, args.a, args.b, pet_id, args.comment, eol)
    patched = raw
    if active + 1 > capacity:
        new_capacity = active + 17
        patched = (patched[:decl.start(1)] + str(new_capacity).encode('ascii')
                   + patched[decl.end(1):])
        print(f'capacity bump: ridePetTable[{capacity}] -> [{new_capacity}]')
        # digit count may have changed; locate the closing brace again
        end = patched.index(b'\n};', DECL_RE.search(patched).end())
    patched = patched[:end + 1] + row + patched[end + 1:]

    print(f'table: {args.char_base}')
    print(f'entries: {len(entries)} total ({active} active) / capacity {capacity}')
    print(f'new row: {row.decode("ascii").rstrip()}')
    if not args.write:
        print('dry-run; re-run with --write to apply, then rebuild gmsv')
        return
    backup = args.char_base.with_suffix('.c.bak')
    if not backup.exists():
        backup.write_bytes(raw)
        print(f'backup: {backup}')
    args.char_base.write_bytes(patched)
    print('written; rebuild gmsv to activate (cmake --build, touch char_base.c not needed)')


if __name__ == '__main__':
    main()
