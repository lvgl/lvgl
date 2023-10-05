#! ../run_testcase.py
#LVGL MicroPython binding tester script: Styles on widget-parts/states and local style-properties

def lv_compare_colors (color1, color2):
    return ( color1.blue==color2.blue and color1.green==color2.green and color1.blue==color2.blue )


#test-objects:

lv_testslider = lv.slider( lv.scr_act() )
lv_testcolor = lv.color_hex(0xAABBCC)


#tests:

if lv_test.DO:
    lv_testslider.set_style_bg_color( lv.color_white(), lv.PART.MAIN )

lv_test.check( lv_compare_colors( lv_testslider.get_style_bg_color(lv.PART.MAIN), lv.color_white() )
               , True, "Slider main-part default color change" )


if lv_test.DO:
    lv_testslider.set_style_bg_color( lv_testcolor, lv.PART.KNOB | lv.STATE.PRESSED )
    lv_testslider.add_state( lv.STATE.PRESSED )
    lv_test.wait(200)  #seems required, a polling with timeout would be better

lv_test.check( lv_compare_colors( lv_testslider.get_style_bg_color(lv.PART.KNOB), lv_testcolor )
               ,True , "Slider-knob pressed color change" )


#lv_test.assert_equal( expected, actual, message ) - seems the preferred form for asserts
