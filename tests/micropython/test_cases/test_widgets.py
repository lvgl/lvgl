#LVGL MicroPython binding tester script: Widget-creation/deletion and basic flag/property-settings

LV_TESTCASE_SUBTESTS = 2


#test-objects:

lv_testbutton = lv.button( lv.scr_act() )


#tests:

lv_testbutton.add_flag( lv.obj.FLAG.CHECKABLE )
if lv_testbutton.has_flag( lv.obj.FLAG.CHECKABLE ): lv_testcase_result += 1

lv_testbutton.add_flag( lv.obj.FLAG.FLOATING )
if lv_testbutton.has_flag( lv.obj.FLAG.FLOATING ): lv_testcase_result += 1
