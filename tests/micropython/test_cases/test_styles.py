#!../run_testcase.py
#LVGL MicroPython binding tester script: Styles on widget-parts/states and local style-properties

LV_TESTCASE_SUBTESTS = 2


#test-objects:

lv_testslider = lv.slider( lv.scr_act() )


#tests:

lv_subtest_start( "Slider main-part default color change" )
lv_testslider.set_style_bg_color( lv.color_white(), lv.PART.MAIN )
if lv_testslider.get_style_bg_color( lv.PART.MAIN ).blue == lv.color_white().blue:
    lv_subtest_success("Slider whitening tested")

lv_subtest_start( "Slider-knob pressed color change", 2 )
lv_testslider.set_style_bg_color( lv.color_white(), lv.PART.KNOB | lv.STATE.PRESSED )
if lv_testslider.get_style_bg_color( lv.PART.KNOB | lv.STATE.PRESSED ).blue == lv.color_white().blue:
    lv_subtest_success()


