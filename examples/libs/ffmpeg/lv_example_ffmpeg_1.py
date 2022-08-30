#!/opt/bin/lv_micropython-ffmpeg -i
import sys
import lvgl as lv
import display_driver

try:
    #
    # Open an image from a file
    #
    img = lv.img(lv.scr_act())
    img.set_src("ffmpeg.png")
    img.center()
except Exception as e:
    print(e)
    # TODO
    # fallback for online examples

    label = lv.label(lv.scr_act())
    label.set_text("FFmpeg is not installed")
    label.center()

