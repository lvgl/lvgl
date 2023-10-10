#! ../../../../../../../lv_micropython/ports/unix/build-standard/micropython ../run_testcase.py
#LVGL MicroPython binding tester script: Font - change on a label and check the label's size-change


#test-objects:

lv_testlabel = lv.label( lv.scr_act() )
lv_test.wait( lv_const.TRANSITION_WAIT_TIME )
lv_testlabel_original_size = lv_testlabel.get_width()


#tests:

lv_testlabel.set_style_text_font( lv.font_dejavu_16_persian_hebrew,0) #lv.font_montserrat_18, 0 )

lv_test.assert_differ( lv_testlabel.get_style_text_font(0).line_height, lv.font_default().line_height, "Label font-change" )


lv_test.wait( lv_const.TRANSITION_WAIT_TIME )

lv_test.assert_differ( lv_testlabel_original_size, lv_testlabel.get_width(), "Label size-change caused by font-change" )
