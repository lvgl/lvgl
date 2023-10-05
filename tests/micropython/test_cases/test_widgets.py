#! ../run_testcase.py
#LVGL MicroPython binding tester script: Widget-creation/deletion and basic flag/property-settings


#test-objects:

lv_testbutton = lv.button( lv.scr_act() )


#tests:

if lv_test.DO: lv_testbutton.add_flag( lv.obj.FLAG.CHECKABLE )
lv_test.check( lv_testbutton.has_flag( lv.obj.FLAG.CHECKABLE ), True, "Widget Flag-setting Test: CHECKABLE" )

if lv_test.DO: lv_testbutton.add_flag( lv.obj.FLAG.FLOATING )
lv_test.check( lv_testbutton.has_flag( lv.obj.FLAG.FLOATING ), True, "Widget Flag-setting Test: FLOATING" )

