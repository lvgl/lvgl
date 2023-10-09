#! ../../../../../../../lv_micropython/ports/unix/build-standard/micropython ../run_testcase.py
#LVGL MicroPython binding tester script: Layout - Flexbox and Grid with a few widgets


#test-objects:

BUTTON_COUNT = 20
lv_testbuttons = []

for i in range(BUTTON_COUNT):
    lv_testbuttons.append( lv.button( lv.scr_act() ) )


#tests:

lv.scr_act().set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)
lv_test.wait( lv_const.TRANSITION_WAIT_TIME ) #required for proper result

lv_test.assert_greater( 0, lv_testbuttons[BUTTON_COUNT-1].get_x(), "FlexBox layout widget X-coordinate" )
lv_test.assert_greater( 0, lv_testbuttons[BUTTON_COUNT-1].get_y(), "FlexBOx layout widget Y-coordinate" )


#test grid layout too
