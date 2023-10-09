#! ../../../../../../../lv_micropython/ports/unix/build-standard/micropython ../run_testcase.py
#LVGL MicroPython binding tester script: Styles on widget-parts/states and local style-properties


#test-objects:

lv_testslider = lv.slider( lv.scr_act() )
lv_testcolor = lv.color_hex(0xAABBCC)


#tests:

lv_testslider.set_style_bg_color( lv.color_black(), lv.PART.MAIN )

lv_test.assert_colormatch( lv.color_black(), lv_testslider.get_style_bg_color(lv.PART.MAIN)
                           , "Slider main-part default color change" )


lv_testslider.set_style_bg_color( lv_testcolor, lv.PART.KNOB | lv.STATE.PRESSED )
lv_testslider.add_state( lv.STATE.PRESSED )
lv_test.wait( lv_const.TRANSITION_WAIT_TIME )  # (a polling with timeout might be better)

lv_test.assert_colormatch ( lv_testcolor, lv_testslider.get_style_bg_color(lv.PART.KNOB)
                            , "Slider-knob pressed color change" )


#test other parts/states too

