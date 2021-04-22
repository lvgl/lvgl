# Create a transparent canvas with Chroma keying and indexed color format (palette).

CANVAS_WIDTH  = 50
CANVAS_HEIGHT = 50

def bufsize(w, h, bits, indexed=False):
    """this function determines required buffer size
       depending on the color depth"""
    size = (w * bits // 8 + 1) * h
    if indexed:
        # + 4 bytes per palette color
        size += 4 * (2**bits)
    return size

# Create a button to better see the transparency
lv.btn(lv.scr_act())

# Create a buffer for the canvas
cbuf = bytearray(bufsize(CANVAS_WIDTH, CANVAS_HEIGHT, 1, indexed=True))

# Create a canvas and initialize its the palette
canvas = lv.canvas(lv.scr_act())
canvas.set_buffer(cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, lv.img.CF.INDEXED_1BIT)
# transparent color can be defined in lv_conf.h and set to pure green by default
canvas.set_palette(0, lv.color_make(0x00, 0xFF, 0x00))
canvas.set_palette(1, lv.color_make(0xFF, 0x00, 0x00))

# Create colors with the indices of the palette
c0 = lv.color_t()
c1 = lv.color_t()

c0.full = 0
c1.full = 1

# Transparent background
canvas.fill_bg(c1)

# Create hole on the canvas
for y in range(10,30):
    for x in range(5, 20):
        canvas.set_px(x, y, c0)
