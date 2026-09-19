"""Read the StoneAge client sprite and indexed image formats."""

from dataclasses import dataclass
from pathlib import Path
import struct


@dataclass(frozen=True)
class Frame:
    image: int
    x: int
    y: int
    sound: int


@dataclass(frozen=True)
class Animation:
    direction: int
    action: int
    duration: int
    frames: tuple[Frame, ...]


@dataclass(frozen=True)
class IndexedImage:
    number: int
    x: int
    y: int
    width: int
    height: int
    pixels: bytes


class AssetLibrary:
    def __init__(self, client: Path):
        data = client / 'data'
        self.sprite_index = {
            number: (offset, count)
            for number, offset, count in struct.iter_unpack(
                '<IIHxx', (data / 'spradrn_115.bin').read_bytes()
            )
        }
        self.sprite_file = (data / 'spr_115.bin').open('rb')
        self.image_index = (data / 'adrn_136.bin').open('rb')
        self.image_file = (data / 'real_136.bin').open('rb')
        palette = (data / 'pal/PALET_1.SAP').read_bytes()
        self.colors = [(0, 0, 0)] * 256
        for index in range(16, 240):
            self.colors[index] = tuple(palette[(index - 16) * 3 + c] for c in (2, 1, 0))

    def sprite(self, number: int) -> tuple[Animation, ...]:
        offset, count = self.sprite_index[number]
        self.sprite_file.seek(offset)
        animations = []
        for _ in range(count):
            direction, action, duration, frame_count = struct.unpack(
                '<HHII', self.sprite_file.read(12)
            )
            frames = tuple(
                Frame(*value)
                for value in struct.iter_unpack(
                    '<IhhH', self.sprite_file.read(frame_count * 10)
                )
            )
            animations.append(Animation(direction, action, duration, frames))
        return tuple(animations)

    def image(self, number: int) -> IndexedImage:
        self.image_index.seek(number * 80)
        record = self.image_index.read(80)
        image_number, offset, size, x, y, width, height = struct.unpack_from(
            '<IIIiiII', record
        )
        if image_number != number or not width or not height:
            raise KeyError(number)
        self.image_file.seek(offset)
        source = self.image_file.read(size)
        magic, flag, _, decoded_width, decoded_height, payload_size = struct.unpack_from(
            '<2sBBIII', source
        )
        if magic != b'RD' or (decoded_width, decoded_height) != (width, height):
            raise ValueError(f'image {number}: invalid RD header')
        if flag == 0:
            pixels = source[16:16 + width * height]
        elif flag == 1:
            pixels = self._decode(source, payload_size, width * height)
        else:
            raise ValueError(f'image {number}: unsupported compression {flag}')
        if len(pixels) != width * height:
            raise ValueError(f'image {number}: wrong decoded size')
        return IndexedImage(number, x, y, width, height, bytes(pixels))

    @staticmethod
    def _decode(source: bytes, end: int, expected: int) -> bytes:
        output = bytearray()
        position = 16
        while position < end and len(output) < expected:
            tag = source[position]
            position += 1
            if tag & 0x80:
                value = 0
                if not tag & 0x40:
                    value = source[position]
                    position += 1
                if tag & 0x20:
                    count = ((tag & 15) << 16) | (source[position] << 8) | source[position + 1]
                    position += 2
                elif tag & 0x10:
                    count = ((tag & 15) << 8) | source[position]
                    position += 1
                else:
                    count = tag & 15
                output.extend(bytes([value]) * count)
            else:
                if tag & 0x10:
                    count = ((tag & 15) << 8) | source[position]
                    position += 1
                else:
                    count = tag & 15
                output.extend(source[position:position + count])
                position += count
        return bytes(output)

    def render_frame(self, frame: Frame) -> tuple[IndexedImage, int, int]:
        image = self.image(frame.image)
        return image, frame.x + image.x, frame.y + image.y

    def close(self) -> None:
        self.sprite_file.close()
        self.image_index.close()
        self.image_file.close()
