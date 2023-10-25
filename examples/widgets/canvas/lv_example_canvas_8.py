
import lvgl as lv
import display_driver

CANVAS_WIDTH  = 150
CANVAS_HEIGHT = 150

LV_COLOR_SIZE = 32

#
# Draw a path to the canvas
#

# Create a buffer for the canvas
cbuf = bytearray((LV_COLOR_SIZE // 8) * CANVAS_WIDTH * CANVAS_HEIGHT)

# Create a canvas and initialize its palette
canvas = lv.canvas(lv.screen_active())
canvas.set_buffer(cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, lv.COLOR_FORMAT.NATIVE)
canvas.fill_bg(lv.color_hex3(0xccc), lv.OPA.COVER)
canvas.center()

layer = lv.layer_t()
canvas.init_layer(layer)

context = lv.vector_dsc_create(layer)
path = lv.vector_path_create(lv.VECTOR_PATH_QUALITY.MEDIUM)

pt1 = lv.fpoint_t()
pt1.x = 10
pt1.y = 10
path.move_to(pt1);

pt2 = lv.fpoint_t()
pt2.x = 130
pt2.y = 130
path.line_to(pt2);

pt3 = lv.fpoint_t()
pt3.x = 10
pt3.y = 130
path.line_to(pt3);

path.close();

context.set_fill_color(lv.color_make(0x00, 0x80, 0xff))
context.add(path)
context.draw()

path.destroy()
context.destroy()

canvas.finish_layer(layer)
