#!/opt/bin/lv_micropython -i
# Initialize 

import lvgl as lv
import display_driver
import fs_driver

CANVAS_WIDTH  = 50
CANVAS_HEIGHT = 50

LV_COLOR_SIZE = 32

#
# Draw an arc to the canvas
#

# Create a buffer for the canvas
cbuf = bytearray((LV_COLOR_SIZE // 8) * CANVAS_WIDTH * CANVAS_HEIGHT)

# Create a canvas and initialize its palette
canvas = lv.canvas(lv.scr_act())
canvas.set_buffer(cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, lv.img.CF.TRUE_COLOR)
canvas.fill_bg(lv.color_hex3(0xccc), lv.OPA.COVER)
canvas.center()

dsc = lv.draw_arc_dsc_t()
dsc.init()
dsc.color = lv.palette_main(lv.PALETTE.RED)
dsc.width = 5

canvas.draw_arc(25, 25, 15, 0, 220, dsc)

