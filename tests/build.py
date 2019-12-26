import os

warnings = '"-Wall"' 
base_defines = '"-DLV_CONF_PATH=tests/lv_test_conf.h -DLV_BUILD_TEST"'
optimization = '"-O3 -g0"'


def build(name, defines):
  global warnings, base_defines, optimization

  print("=============================")
  print(name)
  print("=============================")
  
  d_all = base_defines[:-1] + " ";
  
  for d in defines:
    d_all += " -D" + d + "=" + str(defines[d])
  
  d_all += '"'
  cmd = "make -j8 BIN=test.bin DEFINES=" + d_all + " WARNINGS=" + warnings + " OPTIMIZATION=" + optimization
  
  print("---------------------------")
  print("Clean")
  print("---------------------------")
  os.system("make clean")
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
  "LV_DPI":40,
  "LV_MEM_SIZE":4*1024,
  "LV_HOR_RES_MAX":128,
  "LV_VER_RES_MAX":64,
  "LV_COLOR_DEPTH":1,
  "LV_USE_GROUP":0,
  "LV_USE_ANIMATION":0,
  "LV_ANTIALAIS":0,
  "LV_GPU":0,
  "LV_USE_FILESYSTEM":0,
  "LV_USE_USER_DATA":0,
  "LV_USE_LOG":0,
  "LV_USE_DEBUG":0,
  "LV_THEME_LIVE_UPDATE":0,
  "LV_FONT_ROBOTO_12":0,
  "LV_FONT_ROBOTO_16":0,
  "LV_FONT_ROBOTO_22":0,
  "LV_FONT_ROBOTO_28":0,
  "LV_FONT_ROBOTO_12_SUBPX":0,
  "LV_FONT_ROBOTO_28_COMPRESSED":0,
  "LV_FONT_UNSCII_8":1,
  "LV_FONT_DEFAULT":"\\\"&lv_font_unscii_8\\\"",
  "LV_USE_BIDI": 0,
  "LV_USE_OBJ_REALIGN": 0,
  "LV_USE_ARC":0,
  "LV_USE_BAR":1,
  "LV_USE_BTN":1,
  "LV_USE_BTNM":0,
  "LV_USE_CALENDAR":0,
  "LV_USE_CANVAS":0,
  "LV_USE_CB":0,
  "LV_USE_CHART":0,
  "LV_USE_CONT":1,
  "LV_USE_CPICKER":0,
  "LV_USE_DDLIST":0,
  "LV_USE_GAUGE":0,
  "LV_USE_IMG":1,
  "LV_USE_IMGBTN":0,
  "LV_USE_KB":0,
  "LV_USE_LABEL":1,
  "LV_USE_LED":0,
  "LV_USE_LINE":0,
  "LV_USE_LIST":0,
  "LV_USE_LMETER":0,
  "LV_USE_OBJMASK":0,
  "LV_USE_MBOX":0,
  "LV_USE_PAGE":0,
  "LV_USE_PRELOAD":0,
  "LV_USE_ROLLER":0,
  "LV_USE_SLIDER":0,
  "LV_USE_SPINBOX":0,
  "LV_USE_SW":0,
  "LV_USE_TA":0,
  "LV_USE_TABLE":0,
  "LV_USE_TABVIEW":0,
  "LV_USE_TILEVIEW":0,
  "LV_USE_WIN":0 
}

all_obj_minimal_features = {
  "LV_DPI":60,
  "LV_MEM_SIZE":12*1024,
  "LV_HOR_RES_MAX":320,
  "LV_VER_RES_MAX":240,
  "LV_COLOR_DEPTH":8,
  "LV_USE_GROUP":0,
  "LV_USE_ANIMATION":0,
  "LV_ANTIALAIS":0,
  "LV_GPU":0,
  "LV_USE_FILESYSTEM":0,
  "LV_USE_USER_DATA":0,
  "LV_USE_LOG":0,
  "LV_USE_DEBUG":0,
  "LV_THEME_LIVE_UPDATE":0,
  "LV_FONT_ROBOTO_12":0,
  "LV_FONT_ROBOTO_16":1,
  "LV_FONT_ROBOTO_22":0,
  "LV_FONT_ROBOTO_28":0,
  "LV_FONT_ROBOTO_12_SUBPX":0,
  "LV_FONT_ROBOTO_28_COMPRESSED":0,
  "LV_FONT_UNSCII_8":0,
  "LV_FONT_DEFAULT":"\\\"&lv_font_roboto_16\\\"",
  "LV_USE_BIDI": 0,
  "LV_USE_OBJ_REALIGN": 0,
  "LV_USE_EXT_CLICK_AREA":"LV_EXT_CLICK_AREA_TINY",
  "LV_USE_ARC":1,
  "LV_USE_BAR":1,
  "LV_USE_BTN":1,
  "LV_USE_BTNM":1,
  "LV_USE_CALENDAR":1,
  "LV_USE_CANVAS":1,
  "LV_USE_CB":1,
  "LV_USE_CHART":1,
  "LV_USE_CONT":1,
  "LV_USE_CPICKER":1,
  "LV_USE_DDLIST":1,
  "LV_USE_GAUGE":1,
  "LV_USE_IMG":1,
  "LV_USE_IMGBTN":1,
  "LV_USE_KB":1,
  "LV_USE_LABEL":1,
  "LV_USE_LED":1,
  "LV_USE_LINE":1,
  "LV_USE_LIST":1,
  "LV_USE_LMETER":1,
  "LV_USE_OBJMASK":1,
  "LV_USE_MBOX":1,
  "LV_USE_PAGE":1,
  "LV_USE_PRELOAD":0, #Disabled beacsue needs anim
  "LV_USE_ROLLER":1,
  "LV_USE_SLIDER":1,
  "LV_USE_SPINBOX":1,
  "LV_USE_SW":1,
  "LV_USE_TA":1,
  "LV_USE_TABLE":1,
  "LV_USE_TABVIEW":1,
  "LV_USE_TILEVIEW":1,
  "LV_USE_WIN":1 
}

all_obj_all_features = {
  "LV_DPI":100,
  "LV_MEM_SIZE":32*1024,
  "LV_HOR_RES_MAX":480,
  "LV_VER_RES_MAX":320,
  "LV_COLOR_DEPTH":16,
  "LV_USE_GROUP":1,
  "LV_USE_ANIMATION":1,
  "LV_ANTIALAIS":1,
  "LV_GPU":1,
  "LV_USE_FILESYSTEM":1,
  "LV_USE_USER_DATA":1,
  "LV_USE_LOG":1,
  "LV_THEME_LIVE_UPDATE":1,
  "LV_USE_THEME_TEMPL":1,  
  "LV_USE_THEME_DEFAULT":1, 
  "LV_USE_THEME_ALIEN":1,   
  "LV_USE_THEME_NIGHT":1,   
  "LV_USE_THEME_MONO":1,   
  "LV_USE_THEME_MATERIAL":1,   
  "LV_USE_THEME_ZEN":1, 
  "LV_USE_THEME_NEMO": 1,
  "LV_FONT_ROBOTO_12":1,
  "LV_FONT_ROBOTO_16":1,
  "LV_FONT_ROBOTO_22":1,
  "LV_FONT_ROBOTO_28":1,
  "LV_FONT_ROBOTO_12_SUBPX":1,
  "LV_FONT_ROBOTO_28_COMPRESSED":1,
  "LV_FONT_UNSCII_8":1,
  "LV_FONT_DEFAULT":"\\\"&lv_font_roboto_16\\\"",
  "LV_USE_ARC":1,
  "LV_USE_BAR":1,
  "LV_USE_BTN":1,
  "LV_USE_BTNM":1,
  "LV_USE_CALENDAR":1,
  "LV_USE_CANVAS":1,
  "LV_USE_CB":1,
  "LV_USE_CHART":1,
  "LV_USE_CONT":1,
  "LV_USE_CPICKER":1,
  "LV_USE_DDLIST":1,
  "LV_USE_GAUGE":1,
  "LV_USE_IMG":1,
  "LV_USE_IMGBTN":1,
  "LV_USE_KB":1,
  "LV_USE_LABEL":1,
  "LV_USE_LED":1,
  "LV_USE_LINE":1,
  "LV_USE_LIST":1,
  "LV_USE_LMETER":1,
  "LV_USE_OBJMASK":1,
  "LV_USE_MBOX":1,
  "LV_USE_PAGE":1,
  "LV_USE_PRELOAD":1,
  "LV_USE_ROLLER":1,
  "LV_USE_SLIDER":1,
  "LV_USE_SPINBOX":1,
  "LV_USE_SW":1,
  "LV_USE_TA":1,
  "LV_USE_TABLE":1,
  "LV_USE_TABVIEW":1,
  "LV_USE_TILEVIEW":1,
  "LV_USE_WIN":1 
}


advanced_features = {
  "LV_DPI":100,
  "LV_MEM_SIZE":4*1024*1024,
  "LV_MEM_CUSTOM":1,
  "LV_HOR_RES_MAX":800,
  "LV_VER_RES_MAX":480,
  "LV_COLOR_DEPTH":16,
  "LV_COLOR_SCREEN_TRANSP":1,
  "LV_USE_GROUP":1,
  "LV_USE_ANIMATION":1,
  "LV_ANTIALAIS":1,
  "LV_GPU":1,
  "LV_USE_FILESYSTEM":1,
  "LV_USE_USER_DATA":1,
  "LV_IMG_CACHE_DEF_SIZE":32,
  "LV_USE_LOG":1,
  "LV_LOG_PRINTF":1,
  "LV_USE_DEBUG":1,
  "LV_USE_ASSERT_NULL":1,
  "LV_USE_ASSERT_MEM":1,
  "LV_USE_ASSERT_STR":1,
  "LV_USE_ASSERT_OBJ":1,
  "LV_USE_ASSERT_STYLE":1,
  "LV_THEME_LIVE_UPDATE":1,
  "LV_USE_THEME_TEMPL":1,  
  "LV_USE_THEME_DEFAULT":1, 
  "LV_USE_THEME_ALIEN":1,   
  "LV_USE_THEME_NIGHT":1,   
  "LV_USE_THEME_MONO":1,   
  "LV_USE_THEME_MATERIAL":1,   
  "LV_USE_THEME_ZEN":1, 
  "LV_USE_THEME_NEMO": 1,
  "LV_FONT_ROBOTO_12":1,
  "LV_FONT_ROBOTO_16":1,
  "LV_FONT_ROBOTO_22":1,
  "LV_FONT_ROBOTO_28":1,
  "LV_FONT_ROBOTO_12_SUBPX":1,
  "LV_FONT_ROBOTO_28_COMPRESSED":1,
  "LV_FONT_UNSCII_8":1,
  "LV_FONT_DEFAULT":"\\\"&lv_font_roboto_16\\\"",
  "LV_USE_BIDI": 1,
  "LV_USE_OBJ_REALIGN": 1,
  "LV_FONT_FMT_TXT_LARGE":1,
  "LV_FONT_SUBPX_BGR":1,
  "LV_USE_BIDI": 1,
  "LV_USE_OBJ_REALIGN": 1,
  "LV_USE_EXT_CLICK_AREA":"LV_EXT_CLICK_AREA_FULL",
  "LV_USE_ARC":1,
  "LV_USE_BAR":1,
  "LV_USE_BTN":1,
  "LV_USE_BTNM":1,
  "LV_USE_CALENDAR":1,
  "LV_USE_CANVAS":1,
  "LV_USE_CB":1,
  "LV_USE_CHART":1,
  "LV_USE_CONT":1,
  "LV_USE_CPICKER":1,
  "LV_USE_DDLIST":1,
  "LV_USE_GAUGE":1,
  "LV_USE_IMG":1,
  "LV_USE_IMGBTN":1,
  "LV_USE_KB":1,
  "LV_USE_LABEL":1,
  "LV_USE_LED":1,
  "LV_USE_LINE":1,
  "LV_USE_LIST":1,
  "LV_USE_LMETER":1,
  "LV_USE_OBJMASK":1,
  "LV_USE_MBOX":1,
  "LV_USE_PAGE":1,
  "LV_USE_PRELOAD":1,
  "LV_USE_ROLLER":1,
  "LV_USE_SLIDER":1,
  "LV_USE_SPINBOX":1,
  "LV_USE_SW":1,
  "LV_USE_TA":1,
  "LV_USE_TABLE":1,
  "LV_USE_TABVIEW":1,
  "LV_USE_TILEVIEW":1,
  "LV_USE_WIN":1 
}


build("Minimal monochrome", minimal_monochrome)
build("All objects, minimal features", all_obj_minimal_features)
build("All objects, all features", all_obj_all_features)
  



