#!../run_testcase.py
#LVGL MicroPython binding tester script: Events - trigger and capture events of a widget

LV_TESTCASE_SUBTESTS = 1


#test-objects:

lv_testbutton = lv.button( lv.scr_act() )


#tests:

def lv_test_eventhandler (event):
    global lv_testcase_result
    code = event.get_code()
    if code == lv.EVENT.CLICKED: lv_subtest_success("Click Test",1)

lv_testbutton.add_event( lv_test_eventhandler, lv.EVENT.ALL, None )

lv_testbutton.send_event( lv.EVENT.CLICKED, None )

