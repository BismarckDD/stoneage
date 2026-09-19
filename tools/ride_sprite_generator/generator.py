"""Build a new riding sprite from an existing rider, mount, and ride reference.

The game stores riding frames as flattened indexed images. This tool finds a
second existing ride of A, extracts A from that ride, and transfers A into C's
ride of B. Every generated frame is previewable before installation.
"""

from __future__ import annotations

import argparse
from collections import Counter, defaultdict
from dataclasses import dataclass
import json
from pathlib import Path
import re

from sprite_data import Animation, AssetLibrary, Frame, IndexedImage
from png_codec import write_indexed


@dataclass
class Layer:
    pixels: dict[tuple[int, int], int]

    @property
    def bounds(self) -> tuple[int, int, int, int]:
        return (
            min(x for x, _ in self.pixels),
            min(y for _, y in self.pixels),
            max(x for x, _ in self.pixels) + 1,
            max(y for _, y in self.pixels) + 1,
        )


def layer_from_image(image: IndexedImage, x: int, y: int) -> Layer:
    return Layer({
        (x + px, y + py): color
        for py in range(image.height)
        for px in range(image.width)
        if (color := image.pixels[py * image.width + px])
    })


def find_rides_for_character(source: Path, character: int, library: AssetLibrary):
    text = source.read_text(encoding='utf-8', errors='replace')
    text = re.sub(r'//[^\n]*', '', text)
    matches = re.findall(r'\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*\d+\s*\}', text)
    return [(int(ride), int(mount)) for ride, person, mount in matches
            if int(person) == character and int(ride) in library.sprite_index
            and int(mount) in library.sprite_index]


def find_character_for_ride(source: Path, ride_number: int, mount_number: int,
                            library: AssetLibrary) -> int:
    text = re.sub(r'//[^\n]*', '', source.read_text(encoding='utf-8', errors='replace'))
    matches = re.findall(r'\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*\d+\s*\}', text)
    for ride, person, mount in matches:
        if (int(ride), int(mount)) == (ride_number, mount_number) and int(person) in library.sprite_index:
            return int(person)
    raise ValueError(f'cannot find the character riding B in C={ride_number}')


def animation_for(sprite: tuple[Animation, ...], direction: int, action: int) -> Animation:
    for animation in sprite:
        if animation.direction == direction and animation.action == action and animation.frames:
            return animation
    for preferred in (3, 0, 4):
        for animation in sprite:
            if animation.direction == direction and animation.action == preferred and animation.frames:
                return animation
    raise ValueError(f'no usable animation for direction {direction}')


def proportional_frame(animation: Animation, position: int, count: int) -> Frame:
    index = round(position * (len(animation.frames) - 1) / max(1, count - 1))
    return animation.frames[index]


def candidate_frames(sprite: tuple[Animation, ...], direction: int, action: int) -> list[Frame]:
    animation = animation_for(sprite, direction, action)
    seen = set()
    frames = []
    for frame in animation.frames:
        if frame.image not in seen:
            frames.append(frame)
            seen.add(frame.image)
    return frames


def match_mount(composite: Layer, candidates: list[Layer]) -> tuple[Layer, int, float]:
    """Find the source mount bitmap and translation sharing the most exact pixels."""
    composite_by_color = defaultdict(list)
    for position, color in composite.pixels.items():
        composite_by_color[color].append(position)
    composite_set = {(x, y, color) for (x, y), color in composite.pixels.items()}
    best = None
    for candidate in candidates:
        by_color = defaultdict(list)
        for position, color in candidate.pixels.items():
            by_color[color].append(position)
        votes = Counter()
        for color in composite_by_color.keys() & by_color.keys():
            if len(composite_by_color[color]) > 100 or len(by_color[color]) > 100:
                continue
            for x1, y1 in composite_by_color[color]:
                for x2, y2 in by_color[color]:
                    dx, dy = x1 - x2, y1 - y2
                    if abs(dx) <= 40 and abs(dy) <= 40:
                        votes[(dx, dy)] += 1
        for (dx, dy), _ in votes.most_common(6):
            score = sum((x + dx, y + dy, color) in composite_set
                        for (x, y), color in candidate.pixels.items())
            if best is None or score > best[0]:
                best = (score, candidate, dx, dy)
    if best is None:
        raise ValueError('cannot align the mount with the ride frame')
    score, candidate, dx, dy = best
    translated = Layer({(x + dx, y + dy): color for (x, y), color in candidate.pixels.items()})
    confidence = score / max(1, min(len(composite.pixels), len(candidate.pixels)))
    return translated, score, confidence


def rider_region(composite: Layer, mount: Layer) -> tuple[int, int, int, int]:
    """Estimate the rider's box from colors uncommon in the mount."""
    mount_colors = Counter(mount.pixels.values())
    composite_colors = Counter(composite.pixels.values())
    skin = [position for position, color in composite.pixels.items()
            if color in {45, 46, 47, 48, 70, 71, 72}
            and mount_colors[color] <= 2 and mount.pixels.get(position) != color]
    if len(skin) >= 8:
        return (min(x for x, _ in skin) - 4, min(y for _, y in skin) - 4,
                max(x for x, _ in skin) + 5, max(y for _, y in skin) + 29)
    seeds = [position for position, color in composite.pixels.items()
             if mount_colors[color] <= 2 and composite_colors[color] >= 2]
    if len(seeds) < 5:
        seeds = [position for position, color in composite.pixels.items()
                 if mount.pixels.get(position) != color and mount_colors[color] <= 10]
    if not seeds:
        raise ValueError('cannot identify rider colors')
    # Keep the main cluster; isolated saddle/pet details would widen the box.
    seed_set = set(seeds)
    components = []
    while seed_set:
        first = seed_set.pop()
        component = [first]
        stack = [first]
        while stack:
            x, y = stack.pop()
            for nx in (x - 1, x, x + 1):
                for ny in (y - 1, y, y + 1):
                    neighbor = (nx, ny)
                    if neighbor in seed_set:
                        seed_set.remove(neighbor)
                        component.append(neighbor)
                        stack.append(neighbor)
        if len(component) >= 4:
            components.append(component)
    components.sort(key=len, reverse=True)
    selected = components[:3] if components else [seeds]
    points = [point for group in selected for point in group]
    x0 = min(x for x, _ in points) - 3
    y0 = min(y for _, y in points) - 3
    x1 = max(x for x, _ in points) + 4
    y1 = max(y for _, y in points) + 4
    return x0, y0, x1, y1


def extract_rider(composite: Layer, mount: Layer, box: tuple[int, int, int, int],
                  base_colors: Counter | None = None) -> Layer:
    x0, y0, x1, y1 = box
    mount_colors = Counter(mount.pixels.values())
    return Layer({position: color for position, color in composite.pixels.items()
                  if x0 <= position[0] < x1 and y0 <= position[1] < y1
                  and mount.pixels.get(position) != color
                  and (base_colors is None or
                       (base_colors[color] >= 2 and mount_colors[color] <= 2))})


def rider_palette(base: Layer, mount: Layer) -> Counter:
    """Prefer skin and clothing in the lower part of the unmounted person."""
    _, y0, _, y1 = base.bounds
    lower = y0 + round((y1 - y0) * 0.62)
    lower_colors = Counter(color for (_, y), color in base.pixels.items() if y >= lower)
    all_colors = Counter(base.pixels.values())
    mount_colors = Counter(mount.pixels.values())
    skin = {45, 46, 47, 48, 70, 71, 72}
    chosen = {color for color, count in lower_colors.items()
              if count >= 3 and all_colors[color] >= 4 and mount_colors[color] <= 2}
    chosen |= skin & all_colors.keys()
    return Counter({color: all_colors[color] for color in chosen})


def transfer_rider(reference: Layer, mount: Layer, reference_box: tuple[int, int, int, int],
                   replacement: Layer, replacement_box: tuple[int, int, int, int],
                   reference_colors: Counter) -> Layer:
    rx0, ry0, rx1, ry1 = reference_box
    ax0, ay0, ax1, ay1 = replacement_box
    output = dict(reference.pixels)
    mount_colors = Counter(mount.pixels.values())
    for position, color in list(output.items()):
        if (rx0 <= position[0] < rx1 and ry0 <= position[1] < ry1
                and reference_colors[color] >= 2 and mount_colors[color] <= 2):
            if position in mount.pixels:
                output[position] = mount.pixels[position]
            else:
                del output[position]
    if not replacement.pixels:
        raise ValueError('replacement rider is empty')
    source_width, source_height = max(1, ax1 - ax0), max(1, ay1 - ay0)
    target_width, target_height = max(1, rx1 - rx0), max(1, ry1 - ry0)
    scale = min(target_width / source_width, target_height / source_height, 1.5)
    for (x, y), color in replacement.pixels.items():
        tx = rx0 + round((target_width - source_width * scale) / 2 + (x - ax0) * scale)
        ty = ry1 - round((ay1 - y) * scale)
        output[(tx, ty)] = color
    return Layer(output)


def to_indexed(layer: Layer, number: int) -> IndexedImage:
    x0, y0, x1, y1 = layer.bounds
    width, height = x1 - x0, y1 - y0
    pixels = bytearray(width * height)
    for (x, y), color in layer.pixels.items():
        pixels[(y - y0) * width + x - x0] = color
    return IndexedImage(number, x0, y0, width, height, bytes(pixels))


def build_one(library: AssetLibrary, a_person: tuple[Animation, ...], c_person: tuple[Animation, ...],
              a_ride: tuple[Animation, ...], a_mount: tuple[Animation, ...],
              b_mount: tuple[Animation, ...], c_ride: Animation, frame_index: int,
              a_box=None, c_box=None) -> tuple[IndexedImage, dict]:
    frame = c_ride.frames[frame_index]
    image, x, y = library.render_frame(frame)
    reference = layer_from_image(image, x, y)
    b_candidates = [layer_from_image(*library.render_frame(f))
                    for f in candidate_frames(b_mount, c_ride.direction, c_ride.action)]
    mount, _, b_confidence = match_mount(reference, b_candidates)
    reference_box = c_box or rider_region(reference, mount)

    a_animation = animation_for(a_ride, c_ride.direction, c_ride.action)
    a_frame = proportional_frame(a_animation, frame_index, len(c_ride.frames))
    a_image, ax, ay = library.render_frame(a_frame)
    replacement = layer_from_image(a_image, ax, ay)
    d_candidates = [layer_from_image(*library.render_frame(f))
                    for f in candidate_frames(a_mount, c_ride.direction, c_ride.action)]
    other_mount, _, a_confidence = match_mount(replacement, d_candidates)
    replacement_box = a_box or rider_region(replacement, other_mount)
    a_base = animation_for(a_person, c_ride.direction, c_ride.action)
    c_base = animation_for(c_person, c_ride.direction, c_ride.action)
    a_image, ax0, ay0 = library.render_frame(proportional_frame(a_base, frame_index, len(c_ride.frames)))
    c_image, cx0, cy0 = library.render_frame(proportional_frame(c_base, frame_index, len(c_ride.frames)))
    a_colors = rider_palette(layer_from_image(a_image, ax0, ay0), other_mount)
    c_colors = rider_palette(layer_from_image(c_image, cx0, cy0), mount)
    rider = extract_rider(replacement, other_mount, replacement_box, a_colors)
    if not rider.pixels:
        raise ValueError('A rider could not be extracted')
    result = transfer_rider(reference, mount, reference_box, rider, replacement_box, c_colors)
    return to_indexed(result, 0), {
        'b_match': round(b_confidence, 3),
        'a_match': round(a_confidence, 3),
        'reference_box': reference_box,
        'replacement_box': replacement_box,
    }


def main():
    parser = argparse.ArgumentParser(description='Generate A riding B using C riding B as the pose template.')
    parser.add_argument('--client', type=Path, required=True, help='client root containing data/')
    parser.add_argument('--a', type=int, required=True, help='A character GraphNo')
    parser.add_argument('--b', type=int, required=True, help='B pet GraphNo')
    parser.add_argument('--c', type=int, required=True, help='C riding B GraphNo')
    parser.add_argument('--output', type=Path, required=True, help='editable project directory')
    parser.add_argument('--preview-only', action='store_true', help='generate one standing frame for inspection')
    parser.add_argument('--max-animations', type=int, help='prepare only the first N animations for a trial run')
    parser.add_argument('--a-box', type=int, nargs=4, metavar=('X0','Y0','X1','Y1'))
    parser.add_argument('--c-box', type=int, nargs=4, metavar=('X0','Y0','X1','Y1'))
    args = parser.parse_args()
    library = AssetLibrary(args.client)
    for number in (args.a, args.b, args.c):
        if number not in library.sprite_index:
            parser.error(f'GraphNo {number} does not exist in this client')
    ride_table = Path(__file__).resolve().parents[2] / 'server/gmsv/char/char_base.c'
    rides = find_rides_for_character(ride_table,
                                     args.a, library)
    if not rides:
        parser.error(f'no existing riding reference found for A={args.a}')
    a_ride_number, a_mount_number = rides[0]
    c_person_number = find_character_for_ride(ride_table, args.c, args.b, library)
    a_person = library.sprite(args.a)
    c_person = library.sprite(c_person_number)
    a_ride = library.sprite(a_ride_number)
    a_mount = library.sprite(a_mount_number)
    b_mount = library.sprite(args.b)
    c_ride = library.sprite(args.c)
    args.output.mkdir(parents=True, exist_ok=True)
    print(f'A riding reference: {a_ride_number} (mount {a_mount_number}); C character: {c_person_number}')
    if args.preview_only:
        best = None
        for animation_index, animation in enumerate(c_ride):
            if not animation.frames:
                continue
            try:
                image, quality = build_one(library, a_person, c_person, a_ride, a_mount,
                                           b_mount, animation, 0, args.a_box, args.c_box)
            except (ValueError, KeyError) as error:
                print(f'  animation {animation_index}: skipped ({error})')
                continue
            score = min(quality['a_match'], quality['b_match'])
            if best is None or score > best[0]:
                best = (score, image, quality, animation)
            if score >= 0.9:
                break
        if best is None:
            parser.error('no animation could be composited; try --a-box/--c-box')
        score, image, quality, animation = best
        target = args.output / f'preview_{args.a}_{args.b}_{args.c}.png'
        write_indexed(target, image.width, image.height, image.pixels, library.colors)
        print(f'preview: {target}\n'
              f'best animation: direction={animation.direction} action={animation.action} '
              f'score={score:.3f}\nmatching: {quality}')
        library.close()
        return
    frames_dir = args.output / 'frames'
    frames_dir.mkdir(exist_ok=True)
    selected = c_ride[:args.max_animations] if args.max_animations else c_ride
    manifest = {
        'format': 1,
        'a': args.a,
        'b': args.b,
        'c': args.c,
        'a_ride': a_ride_number,
        'a_mount': a_mount_number,
        'c_person': c_person_number,
        'complete': len(selected) == len(c_ride),
        'animations': [],
    }
    for animation_index, animation in enumerate(selected):
        output_animation = {
            'direction': animation.direction,
            'action': animation.action,
            'duration': animation.duration,
            'frames': [],
        }
        for frame_index, source_frame in enumerate(animation.frames):
            file_name = f'anim_{animation_index:03d}_frame_{frame_index:03d}.png'
            warning = None
            try:
                image, quality = build_one(library, a_person, c_person, a_ride, a_mount,
                                           b_mount, animation, frame_index, args.a_box, args.c_box)
                if min(quality['a_match'], quality['b_match']) < 0.2:
                    warning = 'mount alignment is weak; inspect this frame'
            except (ValueError, KeyError) as error:
                source_image, x, y = library.render_frame(source_frame)
                image = IndexedImage(0, x, y, source_image.width, source_image.height,
                                     source_image.pixels)
                quality = None
                warning = str(error) + '; original C frame exported for manual editing'
            write_indexed(frames_dir / file_name, image.width, image.height,
                          image.pixels, library.colors)
            output_animation['frames'].append({
                'file': f'frames/{file_name}',
                'x': image.x,
                'y': image.y,
                'sound': source_frame.sound,
                'quality': quality,
                'warning': warning,
            })
        manifest['animations'].append(output_animation)
        print(f'prepared {animation_index + 1}/{len(selected)}: '
              f'direction={animation.direction} action={animation.action} '
              f'frames={len(animation.frames)}', flush=True)
    (args.output / 'manifest.json').write_text(
        json.dumps(manifest, ensure_ascii=False, indent=2), encoding='utf-8'
    )
    make_preview_html(args.output, manifest)
    library.close()
    print(f'project ready: {args.output}; edit PNG frames, then run pack.py')


def make_preview_html(output: Path, manifest: dict):
    data = json.dumps(manifest, ensure_ascii=False).replace('<', '\\u003c')
    html = r'''<!doctype html><meta charset="utf-8"><title>Ride sprite frame review</title>
<style>body{margin:0;background:#17191d;color:#eee;font:15px sans-serif;padding:20px}
select,button{font:inherit;margin-right:10px;padding:5px}.stage{position:relative;width:320px;height:320px;
background:#111;margin-top:18px;overflow:hidden}.stage img{position:absolute;image-rendering:pixelated;
transform:scale(2);transform-origin:top left}.note{color:#ffc86a;max-width:700px}</style>
<h2>骑乘造型逐帧预览</h2><p>修改 frames/ 中的 PNG 后刷新此页。透明区域保留宠物背景；打包前检查每个动作。</p>
<select id="animation"></select><button id="play">播放</button><button id="prev">上一帧</button>
<button id="next">下一帧</button><span id="status"></span><div class="stage"><img id="frame"></div>
<p class="note" id="warning"></p><script>
const manifest = __DATA__;
const list=document.getElementById('animation'),image=document.getElementById('frame');
let animation=0,frame=0,playing=false,timer=null;
manifest.animations.forEach((item,i)=>{let option=document.createElement('option');option.value=i;
option.textContent=`${i+1}/${manifest.animations.length} 方向 ${item.direction} 动作 ${item.action} (${item.frames.length} 帧)`;
list.append(option)});
function show(){const a=manifest.animations[animation],f=a.frames[frame];
image.src=f.file+'?v='+Date.now();image.style.left=(160+2*f.x)+'px';image.style.top=(220+2*f.y)+'px';
document.getElementById('status').textContent=`帧 ${frame+1}/${a.frames.length}  偏移 (${f.x}, ${f.y})`;
document.getElementById('warning').textContent=f.warning||''}
function tick(){if(!playing)return;const a=manifest.animations[animation];frame++;
if(frame>=a.frames.length){frame=0;animation=(animation+1)%manifest.animations.length;list.value=animation}
show();const next=manifest.animations[animation];timer=setTimeout(tick,Math.max(30,next.duration/Math.max(1,next.frames.length)))}
document.getElementById('play').onclick=()=>{playing=!playing;document.getElementById('play').textContent=playing?'暂停':'播放';
clearTimeout(timer);if(playing)tick()};
document.getElementById('prev').onclick=()=>{frame=(frame-1+manifest.animations[animation].frames.length)%manifest.animations[animation].frames.length;show()};
document.getElementById('next').onclick=()=>{frame=(frame+1)%manifest.animations[animation].frames.length;show()};
list.onchange=()=>{animation=Number(list.value);frame=0;show()};show();</script>'''
    (output / 'preview.html').write_text(html.replace('__DATA__', data), encoding='utf-8')


if __name__ == '__main__':
    main()
