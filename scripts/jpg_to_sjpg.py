##################################################################
# sjpeg converter script version 1.0
# Dependencies: (PYTHON-3)
##################################################################
SJPG_FILE_FORMAT_VERSION = "V1.00"  #
JPEG_SPLIT_HEIGHT   = 16
##################################################################
import math, os, sys, time
from PIL import Image


OUTPUT_FILE_NAME = ""
INPUT_FILE       = ""


if len(sys.argv) == 2:
    INPUT_FILE = sys.argv[1]
    OUTPUT_FILE_NAME = INPUT_FILE.split("/")[-1].split("\\")[-1].split(".")[0]
else:
    print("usage:\n\t python " + sys.argv[0] + " input_file.jpg")
    sys.exit(0)

try:
    im = Image.open(INPUT_FILE)
except:
    print("\nFile not found!")
    sys.exit(0)


print("\nConversion started...\n")
start_time = time.time()
width, height = im.size

print("Input:")
print("\t" + INPUT_FILE)
print("\tRES = " + str(width) + " x " + str(height) + '\n')


lenbuf = []
block_size = JPEG_SPLIT_HEIGHT;
spilts = math.ceil(height/block_size)

c_code = '''//LVGL SJPG C ARRAY\n#include "lvgl/lvgl.h"\n\nconst uint8_t ''' + OUTPUT_FILE_NAME + '''_map[] = {\n'''

sjpeg_data = bytearray()
sjpeg = bytearray()


row_remaining = height;
for i in range(spilts):
    if row_remaining < block_size:
        crop = im.crop((0, i*block_size, width, row_remaining + i*block_size))
    else:
        crop = im.crop((0, i*block_size, width, block_size + i*block_size))

    row_remaining = row_remaining - block_size;
    crop.save(str(i)+".jpg", quality=90)




for i in range(spilts):
    f = open(str(i)+".jpg", "rb")
    a = f.read()
    f.close()
    sjpeg_data = sjpeg_data + a
    lenbuf.append(len(a))

header = bytearray()

#4 BYTES
header = header + bytearray("_SJPG__".encode("UTF-8"));

#6 BYTES VERSION
header = header + bytearray(("\x00" + SJPG_FILE_FORMAT_VERSION + "\x00").encode("UTF-8"));

#WIDTH 2 BYTES
header = header + width.to_bytes(2, byteorder='little');

#HEIGHT 2 BYTES
header = header + height.to_bytes(2, byteorder='little');

#NUMBER OF ITEMS 2 BYTES
header = header + spilts.to_bytes(2, byteorder='little');

#NUMBER OF ITEMS 2 BYTES
header = header + int(JPEG_SPLIT_HEIGHT).to_bytes(2, byteorder='little');

for item_len in lenbuf:
    # WIDTH 2 BYTES
    header = header + item_len.to_bytes(2, byteorder='little');


data = bytearray()

sjpeg = header + sjpeg_data;

if 1:
    for i in range(len(lenbuf)):
        os.remove(str(i) + ".jpg")


f = open(OUTPUT_FILE_NAME+".sjpg","wb");
f.write(sjpeg)
f.close()

new_line_threshold = 0
for i in range(len(sjpeg)):
    c_code = c_code + "\t" + str(hex(sjpeg[i])) + ","
    new_line_threshold = new_line_threshold + 1
    if (new_line_threshold >= 16):
        c_code = c_code + "\n"
        new_line_threshold = 0


c_code = c_code + "\n};\n\nlv_img_dsc_t "
c_code = c_code + OUTPUT_FILE_NAME + " = {\n"
c_code = c_code + "\t.header.always_zero = 0,\n"
c_code = c_code + "\t.header.w = " + str(width) + ",\n"
c_code = c_code + "\t.header.h = " + str(height) + ",\n"
c_code = c_code + "\t.data_size = " + str(len(sjpeg)) + ",\n"
c_code = c_code + "\t.header.cf = LV_IMG_CF_RAW,\n"
c_code = c_code + "\t.data = " + OUTPUT_FILE_NAME+"_map" + ",\n};"


f = open(OUTPUT_FILE_NAME + '.c', 'w')
f.write(c_code)
f.close()


time_taken = (time.time() - start_time)

print("Output:")
print("\tTime taken = " + str(round(time_taken,2)) + " sec")
print("\tbin size = " + str(round(len(sjpeg)/1024, 1)) + " KB" )
print("\t" + OUTPUT_FILE_NAME + ".sjpg\t(bin file)" + "\n\t" + OUTPUT_FILE_NAME + ".c\t\t(c array)")

print("\nAll good!")
