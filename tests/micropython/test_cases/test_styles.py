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
c_ref = lv.color_hex(0xAABBCC)
lv_testslider.set_style_bg_color( c_ref, lv.PART.KNOB | lv.STATE.PRESSED )
lv_testslider.add_state (lv.PART.KNOB | lv.STATE.PRESSED)
c_pressed = lv_testslider.get_style_bg_color( lv.PART.KNOB | lv.STATE.PRESSED )
if  c_pressed.blue==c_ref.blue and c_pressed.green==c_ref.green and c_pressed.blue==c_ref.blue:
    lv_subtest_success()


