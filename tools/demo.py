import sys
from collections import defaultdict

from PIL import Image, ImageDraw

W, H, SCALE = 800, 450, 2
BACK = (11, 16, 32)
ACCENTS = [(59, 130, 246), (139, 92, 246), (16, 185, 129), (245, 158, 11), (244, 63, 94)]


def mix(a, b, k):
    return tuple(round(x + (y - x) * k) for x, y in zip(a, b))


SURFACE = (21, 31, 51)
RAISED = (31, 43, 69)
INDIGO = (99, 102, 241)
STYLE = {
    0: lambda t: ((18, 26, 43), 14, (35, 48, 74)),
    1: lambda t: (SURFACE, 10, None),
    2: lambda t: (INDIGO, 7, None),
    3: lambda t: (RAISED, 6, None),
    4: lambda t: (mix(RAISED, ACCENTS[t], 0.35), 12, None),
    5: lambda t: (SURFACE, 10, None),
    6: lambda t: (mix(RAISED, INDIGO, 0.8) if t else RAISED, 6, None),
    7: lambda t: (SURFACE, 10, None),
    8: lambda t: (mix(RAISED, INDIGO, 0.25) if t == 0 else RAISED, 8, None),
    9: lambda t: (mix(RAISED, ACCENTS[t], 0.45), 13, None),
    10: lambda t: (SURFACE, 10, None),
    11: lambda t: (mix(RAISED, INDIGO, 0.8) if t else RAISED, 7, None),
}


def draw(boxes):
    img = Image.new('RGB', (W * SCALE, H * SCALE), BACK)
    pen = ImageDraw.Draw(img)
    for kind, tint, x, y, w, h in boxes:
        fill, radius, outline = STYLE[kind](tint)
        r = [x * SCALE, y * SCALE, (x + w) * SCALE - 1, (y + h) * SCALE - 1]
        pen.rounded_rectangle(r, radius * SCALE, fill=fill,
                              outline=outline, width=SCALE if outline else 0)
    return img.resize((W, H), Image.LANCZOS)


def main(out):
    frames = defaultdict(list)
    for line in sys.stdin:
        f, kind, tint, x, y, w, h = line.split()
        frames[int(f)].append((int(kind), int(tint), float(x), float(y), float(w), float(h)))
    images = [draw(frames[f]) for f in sorted(frames)]
    sample = Image.new('RGB', (W, H * 4))
    for k in range(4):
        sample.paste(images[k * len(images) // 4], (0, H * k))
    palette = sample.quantize(128, method=Image.Quantize.MEDIANCUT)
    gif = [im.quantize(palette=palette, dither=Image.Dither.NONE) for im in images]
    gif[0].save(out, save_all=True, append_images=gif[1:], duration=50, loop=0,
                optimize=True, disposal=1)


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else 'docs/demo.gif')
