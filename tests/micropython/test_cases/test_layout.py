#! ../../../../../../../lv_micropython/ports/unix/build-standard/micropython ../run_testcase.py
#LVGL MicroPython binding tester script: Layout - Flexbox and Grid with a few widgets


#test-objects:

BUTTON_COUNT = 20
lv_testbuttons = []

for i in range(BUTTON_COUNT):
    lv_testbuttons.append( lv.button( lv.scr_act() ) )


lv_testbuttons2 = []

GRID_CONTAINER_X = 200
GRID_CONTAINER_Y = 100
#lv_testcolumns = [ 50, 50, lv.GRID_TEMPLATE.LAST ]
#lv_testrows    = [ 50, 50, lv.GRID_TEMPLATE.LAST ]

lv_testcontainer = lv.obj( lv.scr_act() )
#lv_testcontainer.set_grid_dsc_array( lv_testcolumns, lv_testrows )
lv_testcontainer.set_pos( GRID_CONTAINER_X, GRID_CONTAINER_Y )
lv_testcontainer.set_size( 300,150 )

for i in range(BUTTON_COUNT):
    lv_testbuttons2.append( lv.button(lv_testcontainer) )


#tests:

lv.scr_act().set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)
lv_test.wait( lv_const.TRANSITION_WAIT_TIME ) #required for proper result

lv_test.assert_greater( 0, lv_testbuttons[BUTTON_COUNT-1].get_x(), "FlexBox layout last widget X-coordinate" )
lv_test.assert_greater( 0, lv_testbuttons[BUTTON_COUNT-1].get_y(), "FlexBOx layout last widget Y-coordinate" )


#lv_testcontainer.set_layout( lv.LAYOUT_GRID.value)
lv_test.wait( lv_const.TRANSITION_WAIT_TIME ) #required for proper result

#lv_test.assert_greater( 0, lv_testbuttons2[BUTTON_COUNT-1].get_x(), "FlexBox layout last widget X-coordinate" )
#lv_test.assert_greater( 0, lv_testbuttons2[BUTTON_COUNT-1].get_y(), "FlexBOx layout last widget Y-coordinate" )
