#! ../run_testcase.py
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

if lv_test.DO:
    lv.anim_t.start (lv_testanim)
    for i in range(100):
        lv.refr_now(None)
        time.sleep_ms(10)

lv_test.check( lv_testbutton.get_x() > 10, True, "Button movement animation" )
