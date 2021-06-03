#!/usr/bin/env python3

import os

lvgldirname = os.path.abspath('..')
lvgldirname = os.path.basename(lvgldirname)
lvgldirname = '"' + lvgldirname + '"'

base_defines = '"-DLV_CONF_PATH=' + lvgldirname +'/tests/lv_test_conf.h -DLV_BUILD_TEST"'
optimization = '"-O3 -g0"'

def build(defines, test_name):
  global base_defines, optimization

  print("~~~~~~~~~~~~~~~~~~~~~~~~")
  print(test_name)
  print("~~~~~~~~~~~~~~~~~~~~~~~~")
  
  d_all = base_defines[:-1] + " ";
  
  for d in defines:
    d_all += " -D" + d + "=" + str(defines[d])
  
  d_all += '"'
  test_file_name = test_name + ".c"
  test_file_runner_name = test_name + "_Runner.c"
  # -s makes it silence
  cmd = "make -s -j BIN=test.bin MAINSRC=" + test_file_name + " TEST_SRC=" + test_file_runner_name + "  LVGL_DIR_NAME=" + lvgldirname + " DEFINES=" + d_all + " OPTIMIZATION=" + optimization

  print("")
  print("Build")
  print("-----------------------")
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
  
  print("")
  print("Finished")
  print("-----------------------")

def clean():
  print("")
  print("Clean")
  print("-----------------------")
  os.system("make clean LVGL_DIR_NAME=" + lvgldirname)
  os.system("rm -f ./test.bin")
