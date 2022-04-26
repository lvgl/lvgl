#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver
#
# Load a lottie animation from flash
#
from lv_example_rlottie_approve import lv_example_rlottie_approve

lottie = lv.img_create(lv.scr_act());
lv.rlottie_from_raw(lottie, 100, 100, lv_example_rlottie_approve, len(lv_example_rlottie_approve), 0)
lottie.center()

