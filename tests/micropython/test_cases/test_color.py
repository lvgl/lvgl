#! ../run_testcase.py
#LVGL MicroPython binding tester script: Colour - creation by lv_color_*() and lv_palette_*()


#test-objects:

lv_testcolor = lv.color_hex(0x000000)


#tests:

LV_TESTCOLOR1 = 0xCC
if lv_test.DO: lv_testcolor.blue = LV_TESTCOLOR1

lv_test.check( lv_testcolor.blue, LV_TESTCOLOR1, "Color modification" )


if lv_test.DO: lv_testcolor = lv.palette_main(lv.PALETTE.GREEN)

lv_test.check( lv_testcolor.red, lv.palette_main(lv.PALETTE.GREEN).red, "Color setting from palette" )

