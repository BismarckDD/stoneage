from pathlib import Path
import json
import shutil
import struct
from pack import pack
from png_codec import write_indexed
from sprite_data import AssetLibrary

base = Path(__file__).resolve().parent / '_pack_smoke'
if base.exists():
    shutil.rmtree(base)
client = base / 'client'
project = base / 'project'
output = base / 'output'
(client / 'data/pal').mkdir(parents=True)
(project / 'frames').mkdir(parents=True)
palette = bytearray(bytes([0, 0, 0]) * 224)
palette[:3] = bytes([0, 0, 255])
(client / 'data/pal/PALET_1.SAP').write_bytes(palette)
(client / 'data/real_136.bin').write_bytes(struct.pack('<2sBBIII', b'RD', 0, 0, 1, 1, 17) + b'\0')
(client / 'data/adrn_136.bin').write_bytes(struct.pack('<IIIiiII52s', 0, 0, 17, 0, 0, 1, 1, bytes(52)))
(client / 'data/spr_115.bin').write_bytes(struct.pack('<HHII', 1, 3, 100, 1) + struct.pack('<IhhH', 0, 0, 0, 0))
(client / 'data/spradrn_115.bin').write_bytes(struct.pack('<IIHxx', 100000, 0, 1))
colors = [(0, 0, 0)] * 256
colors[16] = (255, 0, 0)
write_indexed(project / 'frames/frame.png', 2, 2, bytes((16, 0, 16, 16)), colors)
manifest = {'format': 1, 'complete': True, 'a': 100230, 'b': 100904, 'c': 100445,
            'animations': [{'direction': 1, 'action': 3, 'duration': 100,
                            'frames': [{'file': 'frames/frame.png', 'x': -5, 'y': -7,
                                        'sound': 0, 'warning': None}]}]}
(project / 'manifest.json').write_text(json.dumps(manifest), encoding='utf-8')
result = pack(project, client, output, 104032)
asset = AssetLibrary(output)
animation = asset.sprite(104032)[0]
image, x, y = asset.render_frame(animation.frames[0])
assert (image.number, image.width, image.height, x, y) == (1, 2, 2, -5, -7)
assert image.pixels == bytes((16, 0, 16, 16))
print(result)
asset.close()
shutil.rmtree(base)
