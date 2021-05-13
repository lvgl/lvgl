#!/usr/bin/env python3

import os

lvgldirname = os.path.abspath('..')
lvgldirname = os.path.basename(lvgldirname)
lvgldirname = '"' + lvgldirname + '"'

base_defines = '"-DLV_CONF_PATH=' + lvgldirname +'/tests/lv_test_conf.h -DLV_BUILD_TEST"'
optimization = '"-O3 -g0"'

def build(name, defines):
  global base_defines, optimization

  print("=============================")
  print(name)
  print("=============================")
  
  d_all = base_defines[:-1] + " ";
  
  for d in defines:
    d_all += " -D" + d + "=" + str(defines[d])
  
  d_all += '"'
  cmd = "make -j8 BIN=test.bin LVGL_DIR_NAME=" + lvgldirname + " DEFINES=" + d_all + " OPTIMIZATION=" + optimization
  
  print("---------------------------")
  print("Clean")
  print("---------------------------")
  os.system("make clean LVGL_DIR_NAME=" + lvgldirname)
  os.system("rm -f ./test.bin")
  print("---------------------------")
  print("Build")
  print("---------------------------")
  ret = os.system(cmd)
  if(ret != 0): 
    print("BUILD ERROR! (error code  " + str(ret) + ")")
    exit(1)
  
  print("---------------------------")
  print("Run")
  print("---------------------------")
  ret = os.system("./test.bin")
  if(ret != 0): 
    print("RUN ERROR! (error code  " + str(ret) + ")")
    exit(1)
  
  print("---------------------------")
  print("Finished")
  print("---------------------------")

minimal_monochrome = {
  "LV_COLOR_DEPTH":1,
  "LV_MEM_SIZE":64 * 1024,
  
  "LV_DPI_DEF":40,
  "LV_DRAW_COMPLEX":0,
  
  "LV_USE_LOG":1,

  "LV_USE_ASSERT_NULL":0,
  "LV_USE_ASSERT_MALLOC":0,
  "LV_USE_ASSERT_MEM_INTEGRITY":0,
  "LV_USE_ASSERT_OBJ":0,
  "LV_USE_ASSERT_STYLE":0,
  
  "LV_USE_USER_DATA": 0,
  
  "LV_FONT_UNSCII_8":1,
  
  "LV_USE_BIDI": 0,
  "LV_USE_ARABIC_PERSIAN_CHARS":0,
  
  "LV_BUILD_EXAMPLES":1,
  
  "LV_FONT_DEFAULT":"\\\"&lv_font_montserrat_14\\\"",

}


minimal_16bit = {
  "LV_COLOR_DEPTH":16,
  "LV_MEM_CUSTOM":1,
  
  "LV_DPI_DEF":40,
  "LV_DRAW_COMPLEX":0,
  
  "LV_USE_LOG":1,

  "LV_USE_ASSERT_NULL":0,
  "LV_USE_ASSERT_MALLOC":0,
  "LV_USE_ASSERT_MEM_INTEGRITY":0,
  "LV_USE_ASSERT_OBJ":0,
  "LV_USE_ASSERT_STYLE":0,
  
  "LV_USE_USER_DATA": 0,
  
  "LV_FONT_UNSCII_8":1,
  
  "LV_USE_BIDI": 0,
  "LV_USE_ARABIC_PERSIAN_CHARS":0,
  
  "LV_BUILD_EXAMPLES":1,
  
  "LV_FONT_DEFAULT":"\\\"&lv_font_montserrat_14\\\"",

}

minimal_16bit_swap = {
  "LV_COLOR_DEPTH":16,
  "LV_COLOR_16_SWAP":1,
  
  "LV_MEM_SIZE":64 * 1024,
  
  "LV_DPI_DEF":40,
  "LV_DRAW_COMPLEX":0,
  
  "LV_USE_LOG":1,

  "LV_USE_ASSERT_NULL":0,
  "LV_USE_ASSERT_MALLOC":0,
  "LV_USE_ASSERT_MEM_INTEGRITY":0,
  "LV_USE_ASSERT_OBJ":0,
  "LV_USE_ASSERT_STYLE":0,
  
  "LV_USE_USER_DATA": 0,
  
  "LV_FONT_UNSCII_8":1,
  
  "LV_USE_FONT_SUBPX": 1,
  "LV_USE_BIDI": 0,
  "LV_USE_ARABIC_PERSIAN_CHARS":0,
  
  "LV_BUILD_EXAMPLES":1,

  "LV_FONT_DEFAULT":"\\\"&lv_font_montserrat_14\\\"",
}


full_32bit = {
  "LV_COLOR_DEPTH":32,
  "LV_MEM_SIZE":8 * 1024 * 1024,
  
  "LV_DPI_DEF":160,
  "LV_DRAW_COMPLEX":1,
  "LV_SHADOW_CACHE_SIZE":1,
  "LV_IMG_CACHE_DEF_SIZE":32,
  
  "LV_USE_LOG":1,
  "LV_LOG_PRINTF":1,
  "LV_USE_FONT_SUBPX": 1,
  "LV_FONT_SUBPX_BGR":1,
  "LV_USE_PERF_MONITOR":1,
  
  "LV_USE_ASSERT_NULL":1,
  "LV_USE_ASSERT_MALLOC":1,
  "LV_USE_ASSERT_MEM_INTEGRITY":1,
  "LV_USE_ASSERT_OBJ":1,
  "LV_USE_ASSERT_STYLE":1,
  
  "LV_USE_USER_DATA": 1,
  
  "LV_USE_LARGE_COORD": 1,
  
  "LV_FONT_MONTSERRAT_8":1,
  "LV_FONT_MONTSERRAT_10":1,
  "LV_FONT_MONTSERRAT_12":1,
  "LV_FONT_MONTSERRAT_14":1,
  "LV_FONT_MONTSERRAT_16":1,
  "LV_FONT_MONTSERRAT_18":1,
  "LV_FONT_MONTSERRAT_20":1,
  "LV_FONT_MONTSERRAT_22":1,
  "LV_FONT_MONTSERRAT_24":1,
  "LV_FONT_MONTSERRAT_26":1,
  "LV_FONT_MONTSERRAT_28":1,
  "LV_FONT_MONTSERRAT_30":1,
  "LV_FONT_MONTSERRAT_32":1,
  "LV_FONT_MONTSERRAT_34":1,
  "LV_FONT_MONTSERRAT_36":1,
  "LV_FONT_MONTSERRAT_38":1,
  "LV_FONT_MONTSERRAT_40":1,
  "LV_FONT_MONTSERRAT_42":1,
  "LV_FONT_MONTSERRAT_44":1,
  "LV_FONT_MONTSERRAT_46":1,
  "LV_FONT_MONTSERRAT_48":1,  
  "LV_FONT_MONTSERRAT_12_SUBPX":1,  
  "LV_FONT_MONTSERRAT_28_COMPRESSED":1,  
  "LV_FONT_DEJAVU_16_PERSIAN_HEBREW":1,  
  "LV_FONT_SIMSUN_16_CJK":1,  
  "LV_FONT_UNSCII_8":1,
  "LV_FONT_UNSCII_16":1,
  "LV_FONT_FMT_TXT_LARGE":1,
  "LV_USE_FONT_COMPRESSED":1,

  "LV_USE_BIDI": 1,
  "LV_USE_ARABIC_PERSIAN_CHARS":1,
  "LV_USE_PERF_MONITOR":1,
  "LV_USE_MEM_MONITOR":1,

  "LV_LABEL_TEXT_SELECTION":1,

  "LV_BUILD_EXAMPLES":1,
  
  "LV_FONT_DEFAULT":"\\\"&lv_font_montserrat_32\\\"",
}

build("Minimal config monochrome", minimal_monochrome)
build("Minimal config, 16 bit color depth", minimal_16bit)
build("Minimal config, 16 bit color depth swapped", minimal_16bit_swap)
build("Full config, 32 bit color depth", full_32bit)
