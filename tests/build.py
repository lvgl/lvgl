#!/usr/bin/env python3

import os
import re

lvgldirname = os.path.abspath('..')
lvgldirname = os.path.basename(lvgldirname)
lvgldirname = '"' + lvgldirname + '"'

base_defines = '"-DLV_CONF_PATH=' + lvgldirname +'/tests/lv_test_conf.h -DLV_BUILD_TEST"'

def build(defines):
  global base_defines
  optimization = '"-O3 -g0"'
  d_all = base_defines[:-1] + " ";
  
  for d in defines:
    d_all += " -D" + d + "=" + str(defines[d])
  
  d_all += '"'
  # -s makes it silence
  cmd = "make -s -j BIN=test.bin " + "MAINSRC=lv_test_main.c LVGL_DIR_NAME=" + lvgldirname + " DEFINES=" + d_all + " OPTIMIZATION=" + optimization

  print("")
  print("Build")
  print("-----------------------")
#  print(cmd)
  ret = os.system(cmd)
  if(ret != 0): 
    print("BUILD ERROR! (error code  " + str(ret) + ")")
    exit(1)

  print("")
  print("Run")
  print("-----------------------")
  ret = os.system("./test.bin")
  if(ret != 0): 
    print("RUN ERROR! (error code  " + str(ret) + ")")
    exit(1)


def build_test(defines, test_name):
  global base_defines
  optimization = '"-g0"'

  print("")
  print("")
  print("~~~~~~~~~~~~~~~~~~~~~~~~")
  print(re.search("/[a-z_]*$", test_name).group(0)[1:])
  print("~~~~~~~~~~~~~~~~~~~~~~~~")
  
  d_all = base_defines[:-1] + " ";
  
  for d in defines:
    d_all += " -D" + d + "=" + str(defines[d])
  
  d_all += '"'
  test_file_name = test_name + ".c"
  test_file_runner_name = test_name + "_Runner.c"
  test_file_runner_name = test_file_runner_name.replace("/test_cases/", "/test_runners/")
  csrcs = " EXTRA_CSRCS=\"unity/unity.c unity/unity_support.c src/test_fonts/font_1.c src/test_fonts/font_2.c src/test_fonts/font_3.c \" "
  # -s makes it silence
  cmd = "make -s -j BIN=test.bin MAINSRC=" + test_file_name + " TEST_SRC=" + test_file_runner_name + csrcs + "  LVGL_DIR_NAME=" + lvgldirname + " DEFINES=" + d_all + " OPTIMIZATION=" + optimization

  print("")
  print("Build")
  print("-----------------------")
#  print(cmd)
  ret = os.system(cmd)
  if(ret != 0): 
    print("BUILD ERROR! (error code  " + str(ret) + ")")
    exit(1)
  
  print("")
  print("Run")
  print("-----------------------")
  ret = os.system("./test.bin")
  if(ret != 0): 
    print("RUN ERROR! (error code  " + str(ret) + ")")
    exit(1)

def clean():
  print("")
  print("Clean")
  print("-----------------------")
  os.system("make clean LVGL_DIR_NAME=" + lvgldirname)
  os.system("rm -f ./test.bin")
