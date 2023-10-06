#! ../../../../../../../lv_micropython/ports/unix/build-standard/micropython ../run_testcase.py
#LVGL MicroPython binding tester script: FileSystem - file-creation/writing/reading by lv.fs_*()

import fs_driver

#test-objects:

LV_TESTFILE_NAME = lv_test.FOLDER + "assets/testfile"

fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'S')

#fs_driver.fs_open( "S:"+LV_TESTFILE_NAME, lv.fs.MODE_WR )

#lv.fs_init()

#tests:


