#!../run_testcase.py
#LVGL MicroPython binding tester script: Colour - creation by lv_color_*() and lv_palette_*()

LV_TESTCASE_SUBTESTS = 2


#test-objects:

lv_testcolor = lv.color_hex(0xAABBCC)


#tests:

if lv_subtest_start("Color modification"): lv_testcolor.blue=0xCC
if lv_testcolor.blue==0xCC: lv_subtest_success()

if lv_subtest_start("Color setting from palette"): lv_testcolor = lv.palette_main(lv.PALETTE.GREEN)
if lv_testcolor.red==lv.palette_main(lv.PALETTE.GREEN).red: lv_subtest_success()

#lv_test_assert( lv_testcolor.red, lv.palette_main(lv.PALETTE.GREEN).red, "Color set from palette" )
