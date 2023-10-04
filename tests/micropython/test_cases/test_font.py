#!../run_testcase.py
#LVGL MicroPython binding tester script: Font - change on a label and check the label's size-change

LV_TESTCASE_SUBTESTS = 1


#test-objects:

lv_testlabel = lv.label( lv.scr_act() )


#tests:

if lv_subtest_start( "Label font-change" ):
    lv_testlabel.set_style_text_font( lv.font_dejavu_16_persian_hebrew,0) #lv.font_montserrat_18, 0 )
if lv_testlabel.get_style_text_font(0).line_height != lv.font_default().line_height:
    lv_subtest_success("line height changed")

