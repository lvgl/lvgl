#!/usr/bin/env python3
from LVGLImage import LVGLImage
import argparse
import logging
import os

logging.basicConfig(level=logging.INFO)

parser = argparse.ArgumentParser(description="LVGL Binary Image Viewer")
parser.add_argument("file", help="the .bin image file")

args = parser.parse_args()

name, ext = os.path.splitext(args.file)
if ext != ".bin":
    raise ValueError("Only support LVGL .bin image file")

output = name + ".png"
img = LVGLImage().from_bin(args.file)
img.to_png(output)
logging.info(f"convert {args.file} to {output}")

if os.name == "posix":
    os.system(f"open {output}")
else:
    try:
        from PIL import Image
    except ImportError:
        raise ImportError("Need pillow package, do `pip3 install pillow`")
    image = Image.open(output)
    image.show(title=output)
