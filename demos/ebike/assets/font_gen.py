#!/usr/bin/python3

import os,sys


def run(c):

    print("------------------------------------------------")
    print(c)
    os.system(c)
    print("------------------------------------------------\n")

arabic_font = "../../../scripts/built_in_font/DejaVuSans.ttf"
arabic_chars_small = "0xFE99,0xFE97,0xFE8D,0xFEBF,0xFE83,0xFE95,0xFE8E,0xFEF3,0xFE8B,0xFEBC,0xFE87,0xFEB3,0xFE98,0xFECB,0xFEDF,0xFED3,0xFEEF,0xFED8,0xFEB1,0xFEAD,0x627,0xFEA9,0xFEF9,0xFED0,0x0644,0xFECF,0xFE94,0x0639,0xFECC,0x0646,0x0648,0x0645,0xFEE3,0xFEE4,0xFEE2,0x0631,0x0633,0x0628,0xFE91,0x0637,0x064A,0x062D,0x0641,0x0636,0x0642,0x0649,0x0638,0x0625,0x0623,0x0621,0xFEE0,0xFEDE,0xFECA,0xFEE8,0xFEE6,0xFEAE,0xFEB4,0xFEB2,0xFE92,0xFE90,0xFEC4,0xFEC2,0xFEF4,0xFEF2,0xFEAA,0xFEDB,0xFEDC,0xFEA0,0xFEA3,0xFEA4,0xFEA2,0xFEF7,0xFEC0,0xFEF0,0xFEE1,0xFEEE,0xFEC9"

arabic_chars_large = "0xFEA9,0xFE8D,0xFEAA,0xFECB,0xFEF9,0xFE95,0xFE8E,0xFEF3,0xFE8B,0xFEBC,0xFEA3,0xFE87,0xFEF4"

cjk_font = "../../../scripts/built_in_font/SimSun.woff"
cjk_chars = "语語言標題月日电池今日距离天的速度时间设置蓝牙灯亮度音量最大限度光照强统计三平均高时速简体中文。"



run(f"lv_font_conv --no-compress --no-prefilter --bpp 4 --size 14  --font Inter-SemiBold.ttf -r 0x20-0x7F --font {arabic_font} --range {arabic_chars_small} --font {cjk_font} --symbols {cjk_chars} --format lvgl -o font_ebike_inter_14.c --force-fast-kern-format")
run(f"lv_font_conv --no-compress --no-prefilter --bpp 4 --size 20  --font Inter-SemiBold.ttf -r 0x20-0x7F --font {arabic_font} --range {arabic_chars_small} --font {cjk_font} --symbols {cjk_chars} --format lvgl -o font_ebike_inter_20.c --force-fast-kern-format")

run(f"lv_font_conv --no-compress --no-prefilter --bpp 4 --size 24  --font TrumpGothicPro.ttf -r 0x20-0x7F --font {arabic_font} --range {arabic_chars_large} --font {cjk_font} --symbols {cjk_chars}  --format lvgl -o font_ebike_trump_24.c --force-fast-kern-format")
run(f"lv_font_conv --no-compress --no-prefilter --bpp 4 --size 40  --font TrumpGothicPro.ttf -r 0x20-0x7F --font {arabic_font} --range {arabic_chars_large} --font {cjk_font} --symbols {cjk_chars} --format lvgl -o font_ebike_trump_40.c --force-fast-kern-format")
run(f"lv_font_conv --no-compress --no-prefilter --bpp 4 --size 48  --font TrumpGothicPro.ttf -r 0x20-0x7F --format lvgl -o font_ebike_trump_48.c --force-fast-kern-format")
run(f"lv_font_conv --no-compress --no-prefilter --bpp 4 --size 72  --font TrumpGothicPro.ttf -r 0x20-0x7F --format lvgl -o font_ebike_trump_72.c --force-fast-kern-format")
