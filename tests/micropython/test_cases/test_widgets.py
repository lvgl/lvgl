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


lv_testdropdown.clear_flag( lv.obj.FLAG.CLICKABLE )

lv_test.assert_false( lv_testdropdown.has_flag( lv.obj.FLAG.CLICKABLE ), "DropDown Widget Flag-clearing: CLICKABLE" )


LABEL_WIDTH = 100
LABEL_HEIGHT = 50
lv_testlabel.set_size( LABEL_WIDTH, LABEL_HEIGHT )
lv_test.wait( lv_const.TRANSITION_WAIT_TIME )

lv_test.assert_equal( LABEL_WIDTH, lv_testlabel.get_width(), "Label Widget size-setting (width-check)" )
lv_test.assert_equal( LABEL_HEIGHT, lv_testlabel.get_height(), "Label Widget size-setting (height-check)" )


SLIDER_X = 200
lv_testslider.set_x( SLIDER_X )
lv_test.wait( lv_const.TRANSITION_WAIT_TIME )

lv_test.assert_equal( SLIDER_X, lv_testslider.get_x(), "Slider Widget X-position setting" )


CHART_X = 10
CHART_Y = 20
lv_testchart.set_pos( CHART_X, CHART_Y )
lv_test.wait( lv_const.TRANSITION_WAIT_TIME )

lv_test.assert_equal( CHART_X, lv_testchart.get_x(), "Chart Widget position-setting (X-check)" )
lv_test.assert_equal( CHART_Y, lv_testchart.get_y(), "Chart Widget position-setting (Y-check)" )


lv_testroller.set_align( lv.ALIGN.CENTER )
lv_test.wait( lv_const.TRANSITION_WAIT_TIME )

lv_test.assert_greater( 0, lv_testroller.get_x(), "Roller Widget align-setting (center)" )


lv_testroller_deleted = False
lv.roller.delete( lv_testroller )
try: lv_testroller.is_valid()
except: lv_testroller_deleted = True

lv_test.assert_true( lv_testroller_deleted, "Roller Widget Deletion" )

