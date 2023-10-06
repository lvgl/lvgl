#! ../../../../../../../lv_micropython/ports/unix/build-standard/micropython ../run_testcase.py
#LVGL MicroPython binding tester script: Colour - creation by lv_color_*() and lv_palette_*()


#test-objects:

LV_ORIGINAL_COLOR = 0x000000
lv_testcolor = lv.color_hex(0x000000)


#tests:

LV_ORIGINAL_B = lv_testcolor.blue
LV_TESTCOLOR_B = 0x5A
lv_testcolor.blue = LV_TESTCOLOR_B

lv_test.assert_equal( LV_TESTCOLOR_B, lv_testcolor.blue, "Color 'blue' compound modification" )
lv_test.assert_differ( LV_ORIGINAL_B, lv_testcolor.blue, "Color 'blue' compound modification" )
lv_test.assert_colordiff( lv.color_hex(LV_ORIGINAL_COLOR), lv_testcolor, "Color 'blue' compound modification" )


LV_TESTCOLOR_R = 0x45
LV_TESTCOLOR_G = 0x67
LV_TESTCOLOR_B = 0x89
lv_testcolor = lv.color_make(LV_TESTCOLOR_R,LV_TESTCOLOR_G,LV_TESTCOLOR_B)

lv_test.assert_colormatch( lv.color_make(LV_TESTCOLOR_R,LV_TESTCOLOR_G,LV_TESTCOLOR_B), lv_testcolor
                           , "Color-creation by lv.color_make()" )

LV_TESTCOLOR = 0xAABBCC
lv_testcolor = lv.color_hex(LV_TESTCOLOR)

lv_test.assert_colormatch( lv.color_hex(LV_TESTCOLOR), lv_testcolor, "Color-creation by lv.color_hex()" )


LV_TESTCOLOR = 0xDEF
lv_testcolor = lv.color_hex3(LV_TESTCOLOR)

lv_test.assert_colormatch( lv.color_hex3(LV_TESTCOLOR), lv_testcolor, "Color-creation by lv.color_hex3()" )


lv_testcolor = lv.palette_main(lv.PALETTE.GREEN)

lv_test.assert_colormatch( lv.palette_main(lv.PALETTE.GREEN), lv_testcolor, "Color-creation from palette" )

