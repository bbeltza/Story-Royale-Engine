# Based of another icon-generator script, SOURCE: https://gist.github.com/mmozeiko/47cf52adc39441d512c28efb2efb3a20

import struct, os
from PIL import Image
from warnings import warn
from sys import argv

ICONDIR_FMT = "<HHH"
ICONDIRENTRY_FMT = "<BBBBHHII"
MODE_TO_BPP = {'1':1, 'L':8, 'P':8, 'RGB':24, 'RGBA':32, 'CMYK':32, 'YCbCr':24, 'I':32, 'F':32}


def pack(output:str, files:list[str]):
    count = len(files)

    with open(output, "wb") as out_file:
        out_file.write(struct.pack(ICONDIR_FMT, 0, 1, count))
        offset = struct.calcsize(ICONDIR_FMT) + struct.calcsize(ICONDIRENTRY_FMT)*count

        for i in files:
            size = os.path.getsize(i)
            try:
                img = Image.open(i)
            except Image.UnidentifiedImageError as err:
                warn(f"Skipping {i} because file is not an image")
                continue

            if img.width > 0x100 or img.height > 0x100:
                warn(f"Skipping {i} because the width or the height are higher than 256, .ico files support at most 256x256 image sizes")
                continue
            w = 0 if img.width == 0x100 else img.width
            h = 0 if img.height == 0x100 else img.height

            out_file.write(struct.pack(ICONDIRENTRY_FMT, w, h, 0, 0, 1, MODE_TO_BPP[img.mode], size, offset))

            old_pos = out_file.tell()

            offset += size
        
        for i in files:
            with open(i, "rb") as input_file:
                out_file.write(open(i, "rb").read())

def main():
    ARG_COUNT = len(argv) - 1
    assert ARG_COUNT >= 2, "You must pass more than 2 arguments: icon_generator.py [output] <inputs...>"

    output = argv[1]
    files = argv.copy()
    files.pop(0)
    files.pop(0)
    pack(output, files)


if __name__ == "__main__":
    main()