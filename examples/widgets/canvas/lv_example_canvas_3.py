CANVAS_WIDTH  = 50
CANVAS_HEIGHT =  50

LV_COLOR_SIZE = 32
#
# Draw a rectangle to the canvas
#
# Create a buffer for the canvas
cbuf = bytearray((LV_COLOR_SIZE // 8) * CANVAS_WIDTH * CANVAS_HEIGHT)

# Create a canvas and initialize its palette*/
canvas = lv.canvas(lv.scr_act())
canvas.set_buffer(cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, lv.COLOR_FORMAT.NATIVE)

canvas.fill_bg(lv.color_hex3(0xccc), lv.OPA.COVER)
canvas.center()

dsc = lv.draw_rect_dsc_t()
dsc.init()

dsc.bg_color = lv.palette_main(lv.PALETTE.RED)
dsc.border_color = lv.palette_main(lv.PALETTE.BLUE)
dsc.border_width = 3
dsc.outline_color = lv.palette_main(lv.PALETTE.GREEN)
dsc.outline_width = 2
dsc.outline_pad = 2
dsc.outline_opa = lv.OPA._50
dsc.radius = 5
dsc.border_width = 3


coords = lv.area_t()
coords.x1 = 10
coords.y1 = 10
coords.x2 = 30
coords.y2 = 20

layer = lv.layer_t()
canvas.init_layer(layer);

lv.draw_rect(layer, dsc, coords)

canvas.finish_layer(layer)

