import argparse
import os
import sys

parser = argparse.ArgumentParser(description='Create fonts for LittelvGL including the built-in symbols. lv_font_conv needs to be installed. See https://github.com/littlevgl/lv_font_conv')
parser.add_argument('-s', '--size', 
					type=int, 
					metavar = 'px', 
					nargs='?', 
					help='Size of the font in px')
parser.add_argument('-bpp', 
					type=int, 
					metavar = '1,2,4', 
					nargs='?', 
					help='Bit per pixel')
parser.add_argument('-r', '--range', 
					nargs='+', 
					metavar = 'start-end', 
					default=['0x20-0x7F'], 
					help='Ranges and/or characters to include. Default is 0x20-7F (ASCII). E.g. -r 0x20-0x7F 0x200 324')
parser.add_argument('-o', '--output', 
					nargs='?', 
					metavar='file', 
					help='Output file name. E.g. my_font_20.c')
parser.add_argument('--compressed', action='store_true',
                    help='Compress the bitmaps')
                    
args = parser.parse_args()

if args.compressed == False:
	compr = "--no-compress --no-prefilter"
else:
	compr = ""
 
r_str = ""
for r in args.range:
	 r_str = r_str + "-r " + r + " ";

#Built in symbols
syms = "-r 61441  -r 61448 -r 61451 -r 61452 -r 61453 -r 61457 -r 61459 -r 61460 -r 61461 -r 61465 -r 61468 -r 61473 -r 61478 -r 61479 -r 61480 -r 61502 -r 61504 -r 61512 -r 61515 -r 61516 -r 61517 -r 61521 -r 61522 -r 61523 -r 61524 -r 61543 -r 61544 -r 61553 -r 61556 -r 61559 -r 61560 -r 61561 -r 61563 -r 61587 -r 61589 -r 61636 -r 61637 -r 61639 -r 61671 -r 61683 -r 61724 -r 61732 -r 61787 -r 61931 -r 62016 -r 62017 -r 62018 -r 62019 -r 62020 -r 62099"

#Run the command
cmd = "lv_font_conv {} --bpp {} --size {} --font ./Roboto-Regular.woff {} --font FontAwesome.ttf {} --format lvgl -o {}".format(compr, args.bpp, args.size, r_str, syms, args.output)
os.system(cmd)
