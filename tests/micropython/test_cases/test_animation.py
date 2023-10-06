#! ../../../../../../../lv_micropython/ports/unix/build-standard/micropython ../run_testcase.py
#LVGL MicroPython binding tester script: Animation to change widget-position, start & ready callbacks


#test-objects:

lv_testbutton = lv.button( lv.scr_act() )

lv_testanim = lv.anim_t()
lv_testanim.init()
lv_testanim.set_var (lv_testbutton)
lv_testanim.set_values (lv_testbutton.get_x(), 100)
lv_testanim.set_time(500)
lv_testanim.set_path_cb(lv.anim_t.path_overshoot)
lv_testanim.set_custom_exec_cb( lambda a,v: lv_testbutton.set_x(v) )


#tests:

def lv_testanim_start_cb (o):
    lv_test.assert_always( "Button animation started (callback)" )

lv_testanim.set_start_cb( lv_testanim_start_cb )


def lv_testanim_ready_cb (o):
    lv_test.assert_always( "Button animation finished (callback)" )

lv_testanim.set_ready_cb( lv_testanim_ready_cb )


lv.anim_t.start (lv_testanim)
for i in range(100):
    lv.refr_now(None)
    time.sleep_ms(10)

lv_test.assert_less( 10, lv_testbutton.get_x(),  "Button movement animation" )

