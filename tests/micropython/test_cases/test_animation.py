#! ../../../../../../../lv_micropython/ports/unix/build-standard/micropython ../run_testcase.py
#LVGL MicroPython binding tester script: Animation to change widget-position, start & ready callbacks

LV_TESTANIM_FRAMERATE = 10 #ms
LV_TESTANIM_LOOPCOUNT = 30


#test-objects:

lv_testbutton = lv.button( lv.scr_act() )

lv_testanim = lv.anim_t()
lv_testanim.init()
lv_testanim.set_var (lv_testbutton)
lv_testanim.set_values (lv_testbutton.get_x(), 100)
lv_testanim.set_time(500)
lv_testanim.set_path_cb(lv.anim_t.path_overshoot)
lv_testanim.set_custom_exec_cb( lambda a,v: lv_testbutton.set_x(v) )

lv_testsignal_start = False
lv_testsignal_ready = False


#tests:

def lv_testanim_start_cb (o):
    global lv_testsignal_start
    lv_testsignal_start = True

lv_testanim.set_start_cb( lv_testanim_start_cb )


def lv_testanim_ready_cb (o):
    global lv_testsignal_ready
    lv_testsignal_ready = True

lv_testanim.set_ready_cb( lv_testanim_ready_cb )


lv.anim_t.start (lv_testanim)
for i in range(LV_TESTANIM_LOOPCOUNT):
    lv.refr_now(None)
    time.sleep_ms(LV_TESTANIM_FRAMERATE)


lv_test.wait( LV_TESTANIM_LOOPCOUNT * LV_TESTANIM_FRAMERATE * 2 ) #* 2 to be sure


lv_test.assert_true( lv_testsignal_start, "Button animation start-callback" )

lv_test.assert_true( lv_testsignal_ready, "Button animation ready-callback" )

lv_test.assert_greater( 10, lv_testbutton.get_x(),  "Button movement animation" )

