#!/usr/bin/python3

import os, sys


hebrew_syms = "קורא נלהב שצובר אוסף עצום של ספרים יקרים"
devanagari_syms = "हरित कार्यकर्ता, एक स्थायी कल के लिए प्रयासरत।"
#arabic_syms = "عاشق تاریخ و عاشق همه چیز عتیقه فيلم برتقالي وناقد سينمائي عرضي"
#arabic_range = "0xfe70-0xfeff,0xfb50-0xfdcf"
arabic_range = "0xFEEA, 0xFEE4, 0xFEEB, 0xFED6, 0xFEB7, 0xFE8E, 0xFECB, 0xFEED, 0xFEA6, 0xFBFE, 0xFEAD, 0xFE8E, 0xFE97, 0xFED6, 0xFEB7, 0xFE8E, 0xFECB, 0xFEA9, 0xFEAD, 0xFEEE, 0xFEE3, 0xFE96, 0xFEE4, 0xFEB4, 0xFED7, 0xFEEA, 0xFED8, 0xFBFF, 0xFE98, 0xFECB, 0xFEB0, 0xFBFF, 0xFB7C, 0xFE96, 0xFEB3, 0xFE8D, 0xFEE5, 0xFEAE, 0xFED7, 0xFEE6, 0xFEE3, 0xFEEA, 0xFED7, 0xFEFC, 0xFECB, 0xFEF1, 0xFE8B, 0xFEE8, 0xFEF4, 0xFEAA, 0xFEE7, 0xFEE1, 0xFED3, 0xFEF8, 0xFEDF, 0xFECD, 0xFE92, 0xFEE0, 0xFEF4, 0xFE92, 0xFED4, 0xFEF4, 0xFE91, 0xFE90, 0xFEA0, 0xFECC, 0xFEF2, 0xFEBF"


russian_syms = "Любитель приключений на свежем воздухе, всегда ищущий новых острых ощущений"
chinese_syms = "对编程和技术充满热情。 开源倡导者。"
latin_range = "0x20-0x24f"

cmd = f"lv_font_conv --no-compress --no-prefilter --bpp 4 --size 15 --format lvgl --output font_multilang_small.c"
cmd = cmd +  f" --font NotoSerifHebrew-SemiBold.ttf --symbols \"{hebrew_syms}\""
cmd = cmd +  f" --font NotoSerifDevanagari-SemiBold.ttf --symbols \"{devanagari_syms}\""
cmd = cmd +  f" --font NotoNaskhArabic-SemiBold.ttf   -r \"{arabic_range}\""
cmd = cmd +  f" --font NotoSansSC-Medium.otf --symbols \"{chinese_syms}\""
cmd = cmd +  f" --font Montserrat-SemiBold.ttf -r {latin_range} --symbols \"{russian_syms}\""


print(cmd )
os.system(cmd)

cmd = f"lv_font_conv --no-compress --no-prefilter --bpp 4 --size 22 --format lvgl --output font_multilang_large.c --font Montserrat-Bold.ttf -r 0x20-0x7F"

print("\n" + cmd)
os.system(cmd)






