#!/usr/bin/python3

import os, sys

files = ""

for i in sys.argv[1:]:
    files += i + " "

c = f"lv_img_conv.js {files} -f -c CF_TRUE_COLOR_ALPHA"
print(c)
os.system(c)
