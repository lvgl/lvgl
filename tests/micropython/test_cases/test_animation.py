#!../run_testcase.py
#LVGL MicroPython binding tester script: Animation to change widget-position, start & ready callbacks

LV_TESTCASE_SUBTESTS = 1


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

lv.anim_t.start (lv_testanim)

if lv_subtest_start("Move button"):
    for i in range(100):
        lv.refr_now(None)
        time.sleep_ms(10)

if lv_testbutton.get_x() > 10: lv_subtest_success("Button moved")
