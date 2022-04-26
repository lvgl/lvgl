#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver

lottie = lv.img_create(lv.scr_act())
lv.rlottie_from_file(lottie, 100, 100,"lv_example_rlottie_approve.json", 0)
lottie.center()
