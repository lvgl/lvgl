import os

print("Generating 12 px")
os.system("python built_in_font_gen.py --size 12 -o lv_font_roboto_12.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_roboto_12.c')

print("\nGenerating 16 px")
os.system("python built_in_font_gen.py --size 16 -o lv_font_roboto_16.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_roboto_16.c')

print("\nGenerating 22 px")
os.system("python built_in_font_gen.py --size 22 -o lv_font_roboto_22.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_roboto_22.c')

print("\nGenerating 28 px")
os.system("python built_in_font_gen.py --size 28 -o lv_font_roboto_28.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_roboto_28.c')

print("\nGenerating 12 px subpx")
os.system("python built_in_font_gen.py --size 12 -o lv_font_roboto_12_subpx.c --bpp 4 --subpx")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_roboto_12_subpx.c')

print("\nGenerating 28 px compressed")
os.system("python built_in_font_gen.py --size 28 -o lv_font_roboto_28_compressed.c --bpp 4 --compressed")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_roboto_28_compressed.c')

