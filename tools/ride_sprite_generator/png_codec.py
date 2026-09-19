"""Small dependency-free PNG reader/writer for editable sprite frames."""

import struct
import zlib


SIGNATURE = b'\x89PNG\r\n\x1a\n'


def _chunk(kind: bytes, data: bytes) -> bytes:
    return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data))


def write_indexed(path, width: int, height: int, pixels: bytes, colors):
    if len(pixels) != width * height:
        raise ValueError('wrong indexed pixel count')
    palette = bytes(channel for red, green, blue in colors
                    for channel in (red, green, blue))
    rows = b''.join(b'\0' + pixels[y * width:(y + 1) * width]
                    for y in range(height))
    data = SIGNATURE
    data += _chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 3, 0, 0, 0))
    data += _chunk(b'PLTE', palette)
    data += _chunk(b'tRNS', bytes([0]) + bytes([255]) * 255)
    data += _chunk(b'IDAT', zlib.compress(rows, 6))
    data += _chunk(b'IEND', b'')
    path.write_bytes(data)


def read_indexed(path, colors):
    raw = path.read_bytes()
    if not raw.startswith(SIGNATURE):
        raise ValueError(f'{path}: not a PNG')
    pos = len(SIGNATURE)
    width = height = depth = kind = interlace = None
    palette = []
    transparency = b''
    compressed = bytearray()
    while pos + 12 <= len(raw):
        size = struct.unpack_from('>I', raw, pos)[0]
        name = raw[pos + 4:pos + 8]
        payload = raw[pos + 8:pos + 8 + size]
        stored = struct.unpack_from('>I', raw, pos + 8 + size)[0]
        if zlib.crc32(name + payload) != stored:
            raise ValueError(f'{path}: PNG CRC mismatch')
        pos += 12 + size
        if name == b'IHDR':
            width, height, depth, kind, _, _, interlace = struct.unpack('>IIBBBBB', payload)
        elif name == b'PLTE':
            palette = [tuple(payload[i:i + 3]) for i in range(0, len(payload), 3)]
        elif name == b'tRNS':
            transparency = payload
        elif name == b'IDAT':
            compressed.extend(payload)
        elif name == b'IEND':
            break
    if depth != 8 or kind not in (2, 3, 6) or interlace != 0:
        raise ValueError(f'{path}: only non-interlaced 8-bit RGB/RGBA/indexed PNG is supported')
    channels = {2: 3, 3: 1, 6: 4}[kind]
    scanline = width * channels
    decoded = zlib.decompress(compressed)
    previous = bytearray(scanline)
    pixels = bytearray()
    offset = 0
    lookup = {tuple(rgb): index for index, rgb in enumerate(colors) if index != 0}
    nearest_cache = {}
    for _ in range(height):
        filtering = decoded[offset]
        offset += 1
        line = bytearray(decoded[offset:offset + scanline])
        offset += scanline
        for i in range(scanline):
            left = line[i - channels] if i >= channels else 0
            above = previous[i]
            upper_left = previous[i - channels] if i >= channels else 0
            if filtering == 1:
                line[i] = (line[i] + left) & 255
            elif filtering == 2:
                line[i] = (line[i] + above) & 255
            elif filtering == 3:
                line[i] = (line[i] + (left + above) // 2) & 255
            elif filtering == 4:
                p = left + above - upper_left
                distances = (abs(p - left), abs(p - above), abs(p - upper_left))
                predictor = (left, above, upper_left)[distances.index(min(distances))]
                line[i] = (line[i] + predictor) & 255
            elif filtering != 0:
                raise ValueError(f'{path}: unsupported PNG row filter {filtering}')
        previous = line
        for x in range(width):
            if kind == 3:
                index = line[x]
                alpha = transparency[index] if index < len(transparency) else 255
                rgb = palette[index]
            else:
                start = x * channels
                rgb = tuple(line[start:start + 3])
                alpha = line[start + 3] if kind == 6 else 255
            if alpha < 128:
                pixels.append(0)
                continue
            if rgb not in nearest_cache:
                nearest_cache[rgb] = lookup.get(rgb)
                if nearest_cache[rgb] is None:
                    nearest_cache[rgb] = min(range(1, 256), key=lambda i: sum(
                        (rgb[c] - colors[i][c]) ** 2 for c in range(3)))
            pixels.append(nearest_cache[rgb])
    return width, height, bytes(pixels)
