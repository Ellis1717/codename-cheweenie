"""Original seamless brick PBR maps, using only Python's standard library.

192 cm repeat: eight 24 cm bricks across, twenty-four 8 cm courses.
Height derivatives use centimetres, so changing resolution preserves relief.
"""
import hashlib
import json
import math
import pathlib
import struct
import zlib

ROOT = pathlib.Path(__file__).resolve().parents[1]
OUT = ROOT / 'SourceArt/WorldRealism/ReadableBrick'
SIZE = 1024
TILE_CM = 192.0


def write_png(path, pixels):
    def chunk(kind, data):
        return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data))
    rows = b''.join(b'\0' + pixels[y * SIZE * 3:(y + 1) * SIZE * 3] for y in range(SIZE))
    path.write_bytes(b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', SIZE, SIZE, 8, 2, 0, 0, 0))
                     + chunk(b'IDAT', zlib.compress(rows, 9)) + chunk(b'IEND', b''))


def build():
    OUT.mkdir(parents=True, exist_ok=True)
    heights = []
    base = bytearray()
    rough = bytearray()
    for y in range(SIZE):
        v = y / SIZE * 24
        row = math.floor(v)
        for x in range(SIZE):
            u = x / SIZE * 8 + (row % 2) * .5
            col = math.floor(u) % 8
            # A 1 cm mortar joint and rounded 0.35 cm clay shoulders.
            distance = min(min(u % 1, 1 - u % 1) * 24, min(v % 1, 1 - v % 1) * 8)
            a, b = x / SIZE * math.tau, y / SIZE * math.tau
            grain = (math.sin(a * 71 + math.sin(b * 29)) + math.sin(b * 83 + math.sin(a * 17))
                     + math.sin(a * 117 + b * 131)) / 3
            grit = (((x * 374761393 + y * 668265263) ^ (x * y * 1274126177)) & 65535) / 65535 - .5
            edge = max(0., min(1., (distance - .5 + grain * .10) / .35))
            edge = edge * edge * (3 - 2 * edge)
            broad = math.sin(a * 3 + math.sin(b * 2)) * math.sin(b * 5)
            variation = math.sin(col * 7.3 + row * 13.4) * .075
            heights.append(.38 * edge + .018 * grain * edge + .012 * broad * edge + .008 * grit)
            clay = (132, 73, 52)
            mortar = (132, 125, 111)
            for c, m in zip(clay, mortar):
                base.append(round(m * (1 + grit * .025) * (1 - edge) + c * (1 + variation + grain * .04 + broad * .035 + grit * .045) * edge))
            r = round(255 * (.94 * (1 - edge) + (.80 + variation * .25 + grain * .025) * edge))
            rough.extend((r, r, r))
    normals = bytearray()
    step = TILE_CM / SIZE * 2
    for y in range(SIZE):
        for x in range(SIZE):
            dx = (heights[y * SIZE + (x + 1) % SIZE] - heights[y * SIZE + (x - 1) % SIZE]) / step
            dy = (heights[((y + 1) % SIZE) * SIZE + x] - heights[((y - 1) % SIZE) * SIZE + x]) / step
            length = math.sqrt(dx * dx + dy * dy + 1)
            normals.extend(round((c / length * .5 + .5) * 255) for c in (-dx, dy, 1))
    for role, data in [('Base', base), ('Normal', normals), ('Rough', rough)]:
        write_png(OUT / ('Brick_' + role + '.png'), data)
    (OUT / 'manifest.json').write_text(json.dumps({
        'original_procedural_art': True, 'resolution': SIZE, 'tile_cm': TILE_CM,
        'brick_pitch_cm': [24, 8], 'mortar_width_cm': 1, 'relief_cm': .38,
        'normal_convention': 'DirectX; world projection converts green to negative V',
        'generator_sha256': hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
        'textures': {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(OUT.glob('*.png'))}
    }, indent=2) + '\n')
    print('CHEWEENIE_READABLE_BRICK_GENERATED', OUT)


if __name__ == '__main__':
    build()
