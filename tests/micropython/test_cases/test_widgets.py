#! ../../../../../../../lv_micropython/ports/unix/build-standard/micropython ../run_testcase.py
#LVGL MicroPython binding tester script: Widget-creation/deletion and basic flag/property-settings


#test-objects:

lv_testbutton = lv.button( lv.scr_act() )
lv_testlabel = lv.label( lv.scr_act() )
lv_testslider = lv.slider( lv.scr_act() )
lv_testchart = lv.chart( lv.scr_act() )
lv_testdropdown = lv.dropdown( lv.scr_act() )
lv_testroller = lv.roller( lv.scr_act() )


#tests:

lv_testbutton.add_flag( lv.obj.FLAG.CHECKABLE )
lv_test.assert_true( lv_testbutton.has_flag( lv.obj.FLAG.CHECKABLE ), "Button Widget Flag-setting: CHECKABLE" )

lv_testbutton.add_flag( lv.obj.FLAG.FLOATING )
lv_test.assert_true( lv_testbutton.has_flag( lv.obj.FLAG.FLOATING ), "Button Widget Flag-setting: FLOATING" )


lv_testlabel.add_flag( lv.obj.FLAG.CHECKABLE )
lv_test.assert_true( lv_testlabel.has_flag( lv.obj.FLAG.CHECKABLE ), "Label Widget Flag-setting: CHECKABLE" )


lv_testslider.add_flag( lv.obj.FLAG.CHECKABLE )
lv_test.assert_true( lv_testslider.has_flag( lv.obj.FLAG.CHECKABLE ), "Slider Widget Flag-setting: CHECKABLE" )


lv_testchart.add_flag( lv.obj.FLAG.CHECKABLE )
lv_test.assert_true( lv_testchart.has_flag( lv.obj.FLAG.CHECKABLE ), "Slider Widget Flag-setting: CHECKABLE" )


lv_testdropdown.add_flag( lv.obj.FLAG.CHECKABLE )
lv_test.assert_true( lv_testdropdown.has_flag( lv.obj.FLAG.CHECKABLE ), "Slider Widget Flag-setting: CHECKABLE" )


lv_testroller.add_flag( lv.obj.FLAG.CHECKABLE )
lv_test.assert_true( lv_testroller.has_flag( lv.obj.FLAG.CHECKABLE ), "Slider Widget Flag-setting: CHECKABLE" )


#lv.obj.delete( lv_testroller )
#lv_test.assert_false( lv_testroller.has_flag( lv.obj.FLAG.CHECKABLE ), "Slider Widget Deletion" )
