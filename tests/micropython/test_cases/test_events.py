#! ../run_testcase.py
#LVGL MicroPython binding tester script: Events - trigger and capture events of a widget


#test-objects:

lv_testbutton = lv.button( lv.scr_act() )

lv_testbutton_pressed = False

def lv_test_eventhandler (event):
    global lv_testbutton_pressed
    code = event.get_code()
    if code == lv.EVENT.CLICKED: lv_testbutton_pressed = True

lv_testbutton.add_event( lv_test_eventhandler, lv.EVENT.ALL, None )


#tests:

lv_testbutton.send_event( lv.EVENT.CLICKED, None )
lv_test.wait(500)

lv_test.assert_true( lv_testbutton_pressed, "Button programmatic click test" )
