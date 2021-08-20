#!/usr/bin/python3

from PIL import Image
import argparse, textwrap

PALETTE = [
    (0, 0, 0),
    (29, 43, 83),
    (126, 37, 83),
    (0, 135, 81),
    (171, 82, 54),
    (95, 87, 79),
    (194, 195, 199),
    (255, 241, 232),
    (255, 0, 77),
    (255, 163, 0),
    (255, 236, 39),
    (0, 228, 54),
    (41, 173, 255),
    (131, 118, 156),
    (255, 119, 168),
    (255, 204, 170),
]

def color_dif(c1, c2):
    return sum((a - b) ** 2 for a, b in zip(c1, c2))

def find_color_index(c):
    return min(range(len(PALETTE)), key=lambda i:color_dif(c, PALETTE[i]))

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("png_file")
    args = parser.parse_args()

    img = Image.open(args.png_file).convert("RGBA")
    W, H = img.size

    code = []
    for y in range(W):
        for x in range(W):
            color = img.getpixel((x, y))
            i = find_color_index(color)
            code.append(i)

    for l in textwrap.wrap(", ".join("0x%X" % i for i in code), width = 90):
        print(l)

main()
