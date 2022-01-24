CANVAS_WIDTH   = 50
CANVAS_HEIGHT  = 50
LV_COLOR_CHROMA_KEY = lv.color_hex(0x00ff00) 

def LV_IMG_BUF_SIZE_ALPHA_1BIT(w, h):
    return int(((w / 8) + 1) * h)

def LV_IMG_BUF_SIZE_INDEXED_1BIT(w, h):
    return LV_IMG_BUF_SIZE_ALPHA_1BIT(w, h) + 4 * 2

def LV_CANVAS_BUF_SIZE_INDEXED_1BIT(w, h):
    return LV_IMG_BUF_SIZE_INDEXED_1BIT(w, h)

#
# Create a transparent canvas with Chroma keying and indexed color format (palette).
# 

# Create a button to better see the transparency
btn=lv.btn(lv.scr_act())

# Create a buffer for the canvas
cbuf= bytearray(LV_CANVAS_BUF_SIZE_INDEXED_1BIT(CANVAS_WIDTH, CANVAS_HEIGHT))

# Create a canvas and initialize its palette
canvas = lv.canvas(lv.scr_act())
canvas.set_buffer(cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, lv.img.CF.INDEXED_1BIT)
canvas.set_palette(0, LV_COLOR_CHROMA_KEY)
canvas.set_palette(1, lv.palette_main(lv.PALETTE.RED))

# Create colors with the indices of the palette
c0 = lv.color_t()
c1 = lv.color_t()

c0.full = 0
c1.full = 1

# Red background (There is no dedicated alpha channel in indexed images so LV_OPA_COVER is ignored)
canvas.fill_bg(c1, lv.OPA.COVER)

# Create hole on the canvas
for y in range(10,30):
    for x in range(5,20):
        canvas.set_px(x, y, c0)
