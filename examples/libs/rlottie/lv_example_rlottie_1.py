#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver
#
# Load a lottie animation from flash
#
from lv_example_rlottie_approve import lv_example_rlottie_approve

lottie = lv.rlottie_create_from_raw(lv.scr_act(), 100, 100, lv_example_rlottie_approve)
lottie.center()

