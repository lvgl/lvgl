import fs_driver

CANVAS_WIDTH  = 50
CANVAS_HEIGHT = 50

LV_COLOR_SIZE = 32

#
# Draw a text to the canvas
#

# Create a buffer for the canvas
cbuf = bytearray((LV_COLOR_SIZE // 8) * CANVAS_WIDTH * CANVAS_HEIGHT)

# Create a canvas and initialize its palette
canvas = lv.canvas(lv.scr_act())
canvas.set_buffer(cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, lv.img.CF.TRUE_COLOR)
canvas.fill_bg(lv.color_hex3(0xccc), lv.OPA.COVER)
canvas.center()

dsc = lv.draw_label_dsc_t()
dsc.init()

dsc.color = lv.palette_main(lv.PALETTE.RED)

try:
    dsc.font = lv_font_montserrat_18
except:
    # needed for dynamic font loading
    fs_drv = lv.fs_drv_t()
    fs_driver.fs_register(fs_drv, 'S')

    print("Loading font montserrat_18")
    font_montserrat_18 = lv.font_load("S:../../assets/font/montserrat-18.fnt")
    if not font_montserrat_18:
        print("Font loading failed")
    else:
        dsc.font = font_montserrat_18

dsc.decor = lv.TEXT_DECOR.UNDERLINE
print('Printing "Hello"')
canvas.draw_text(10, 10, 30, dsc, "Hello")


