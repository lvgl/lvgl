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
canvas.set_buffer(cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, lv.COLOR_FORMAT.NATIVE)
canvas.fill_bg(lv.color_hex3(0xccc), lv.OPA.COVER)
canvas.center()

dsc = lv.draw_arc_dsc_t()
dsc.init()
dsc.color = lv.palette_main(lv.PALETTE.RED)
dsc.width = 5
dsc.center.x = 25
dsc.center.y = 25
dsc.width = 15
dsc.start_angle = 0
dsc.end_angle = 220

layer = lv.layer_t()
canvas.init_layer(layer);

lv.draw_arc(layer, dsc)

canvas.finish_layer(layer)

