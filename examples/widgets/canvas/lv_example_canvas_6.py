CANVAS_WIDTH  = 50
CANVAS_HEIGHT = 50

LV_COLOR_SIZE = 32

# Create an image from the png file
try:
    with open('../../assets/img_star.png','rb') as f:
        png_data = f.read()
except:
    print("Could not find star.png")
    sys.exit()

img_star_argb = lv.img_dsc_t({
  'data_size': len(png_data),
  'data': png_data
})

#
# Draw an image to the canvas
#

# Create a buffer for the canvas
cbuf = bytearray((LV_COLOR_SIZE // 8) * CANVAS_WIDTH * CANVAS_HEIGHT)

# Create a canvas and initialize its palette
canvas = lv.canvas(lv.scr_act())
canvas.set_buffer(cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, lv.COLOR_FORMAT.NATIVE)
canvas.fill_bg(lv.color_hex3(0xccc), lv.OPA.COVER)
canvas.center()

dsc = lv.draw_img_dsc_t()
dsc.init()

canvas.draw_img(5, 5, img_star_argb, dsc)

