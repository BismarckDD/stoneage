"""Pack corrected PNG frames into a separate playable client data directory."""

import argparse
import json
from pathlib import Path
import shutil
import struct

from png_codec import read_indexed
from sprite_data import AssetLibrary


FILES = ('adrn_136.bin', 'real_136.bin', 'spradrn_115.bin', 'spr_115.bin')


def pack(project: Path, client: Path, output: Path, graphno: int | None):
    manifest = json.loads((project / 'manifest.json').read_text(encoding='utf-8'))
    if manifest.get('format') != 1 or not manifest.get('complete'):
        raise ValueError('project is incomplete; prepare all animations before packing')
    if not manifest['animations'] or any(not a['frames'] for a in manifest['animations']):
        raise ValueError('project has an empty animation')
    if output.resolve() == client.resolve() or output.resolve() == project.resolve():
        raise ValueError('output must be a separate directory')
    data_out = output / 'data'
    if data_out.exists() and any(data_out.iterdir()):
        raise FileExistsError(f'{data_out} is not empty; choose a new output directory')

    library = AssetLibrary(client)
    if graphno is None:
        graphno = max(library.sprite_index) + 1
    if graphno in library.sprite_index or not 100000 <= graphno < 140000:
        raise ValueError(f'GraphNo {graphno} is used or outside client sprite range')
    total_bytes = sum((client / 'data' / name).stat().st_size for name in FILES)
    free_bytes = shutil.disk_usage(output.parent if output.parent.exists() else client).free
    if free_bytes < total_bytes + 100_000_000:
        raise OSError(f'need at least {total_bytes + 100_000_000} free bytes for a full client data copy')

    data_out.mkdir(parents=True, exist_ok=True)
    for name in FILES:
        shutil.copy2(client / 'data' / name, data_out / name)
    shutil.copytree(client / 'data' / 'pal', data_out / 'pal', dirs_exist_ok=True)

    image_count = (data_out / 'adrn_136.bin').stat().st_size // 80
    if image_count >= 500000:
        raise ValueError('client image index is full')
    sprite_offset = (data_out / 'spr_115.bin').stat().st_size
    next_image = image_count
    quality_warnings = 0
    with (data_out / 'real_136.bin').open('ab') as real, \
         (data_out / 'adrn_136.bin').open('ab') as address, \
         (data_out / 'spr_115.bin').open('ab') as sprite:
        for animation in manifest['animations']:
            frames = animation['frames']
            sprite.write(struct.pack('<HHII', animation['direction'], animation['action'],
                                     animation['duration'], len(frames)))
            for frame in frames:
                path = project / frame['file']
                width, height, pixels = read_indexed(path, library.colors)
                if not 0 < width <= 2048 or not 0 < height <= 2048:
                    raise ValueError(f'{path}: invalid frame dimensions')
                if next_image >= 500000:
                    raise ValueError('client image index is full')
                image_id = next_image
                next_image += 1
                payload = struct.pack('<2sBBIII', b'RD', 0, 0, width, height,
                                      16 + len(pixels)) + pixels
                image_offset = real.tell()
                if image_offset > 0xffffffff:
                    raise ValueError('real file offset exceeds 32-bit limit')
                real.write(payload)
                attr = bytearray(52)
                struct.pack_into('<I', attr, 48, image_id)
                address.write(struct.pack('<IIIiiII52s', image_id, image_offset,
                                          len(payload), int(frame['x']), int(frame['y']),
                                          width, height, bytes(attr)))
                sprite.write(struct.pack('<IhhH', image_id, 0, 0, int(frame['sound'])))
                quality_warnings += bool(frame.get('warning'))
    with (data_out / 'spradrn_115.bin').open('ab') as sprite_address:
        sprite_address.write(struct.pack('<IIHxx', graphno, sprite_offset,
                                         len(manifest['animations'])))
    metadata = {
        'graphno': graphno,
        'a': manifest['a'], 'b': manifest['b'], 'c': manifest['c'],
        'animations': len(manifest['animations']),
        'images': next_image - image_count,
        'frames_flagged_during_prepare': quality_warnings,
    }
    (output / 'generated.json').write_text(json.dumps(metadata, ensure_ascii=False, indent=2),
                                           encoding='utf-8')
    library.close()
    return metadata


def main():
    parser = argparse.ArgumentParser(description='Pack an edited riding sprite project into client data files.')
    parser.add_argument('--project', type=Path, required=True)
    parser.add_argument('--client', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--graphno', type=int, help='new sprite number; defaults to first unused GraphNo')
    args = parser.parse_args()
    print(json.dumps(pack(args.project, args.client, args.output, args.graphno),
                     ensure_ascii=False, indent=2))


if __name__ == '__main__':
    main()
