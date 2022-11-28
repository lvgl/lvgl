CANVAS_WIDTH  = 50
CANVAS_HEIGHT = 50

LV_COLOR_SIZE = 32

#
# Draw a line to the canvas
#

# Create a buffer for the canvas
cbuf = bytearray((LV_COLOR_SIZE // 8) * CANVAS_WIDTH * CANVAS_HEIGHT)

# Create a canvas and initialize its palette
canvas = lv.canvas(lv.scr_act())
canvas.set_buffer(cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, lv.img.CF.TRUE_COLOR)
canvas.fill_bg(lv.color_hex3(0xccc), lv.OPA.COVER)
canvas.center()

dsc = lv.draw_line_dsc_t()
dsc.init()

dsc.color = lv.palette_main(lv.PALETTE.RED)
dsc.width = 4
dsc.round_end = 1
dsc.round_start = 1

p = [ {"x":15,"y":15},
      {"x":35,"y":10},
      {"x":10,"y":40} ]
      
canvas.draw_line(p, 3, dsc)


