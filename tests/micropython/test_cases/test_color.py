#! ../../../../../../../lv_micropython/ports/unix/build-standard/micropython ../run_testcase.py
#LVGL MicroPython binding tester script: Colour - creation by lv_color_*() and lv_palette_*()


#test-objects:

lv_testcolor = lv.color_hex(0x000000)


#tests:

LV_TESTCOLOR1 = 0xCC
lv_testcolor.blue = LV_TESTCOLOR1

lv_test.assert_equal( LV_TESTCOLOR1, lv_testcolor.blue, "Color modification" )


lv_testcolor = lv.palette_main(lv.PALETTE.GREEN)

lv_test.assert_colormatch( lv.palette_main(lv.PALETTE.GREEN), lv_testcolor, "Color setting from palette" )

