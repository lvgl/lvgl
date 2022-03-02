#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver

lottie = lv.rlottie_create_from_file(lv.scr_act(), 100, 100,"lv_example_rlottie_approve.json")
lottie.center()
