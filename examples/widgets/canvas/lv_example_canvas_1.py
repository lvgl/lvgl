CANVAS_WIDTH  = 200
CANVAS_HEIGHT = 150

style = lv.style_t()
lv.style_copy(style, lv.style_plain)
style.body.main_color = lv.color_make(0xFF,0,0)
style.body.grad_color = lv.color_make(0x80,0,0)
style.body.radius = 4
style.body.border.width = 2
style.body.border.color = lv.color_make(0xFF,0xFF,0xFF)
style.body.shadow.color = lv.color_make(0xFF,0xFF,0xFF)
style.body.shadow.width = 4
style.line.width = 2
style.line.color = lv.color_make(0,0,0)
style.text.color = lv.color_make(0,0,0xFF)

# CF.TRUE_COLOR requires 4 bytes per pixel
cbuf = bytearray(CANVAS_WIDTH * CANVAS_HEIGHT * 4)

canvas = lv.canvas(lv.scr_act())
canvas.set_buffer(cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, lv.img.CF.TRUE_COLOR)
canvas.align(None, lv.ALIGN.CENTER, 0, 0)
canvas.fill_bg(lv.color_make(0xC0, 0xC0, 0xC0))

canvas.draw_rect(70, 60, 100, 70, style)

canvas.draw_text(40, 20, 100, style, "Some text on text canvas", lv.label.ALIGN.LEFT)

# Test the rotation. It requires an other buffer where the orignal image is stored.
# So copy the current image to buffer and rotate it to the canvas
img = lv.img_dsc_t()
img.data = cbuf[:]
img.header.cf = lv.img.CF.TRUE_COLOR
img.header.w = CANVAS_WIDTH
img.header.h = CANVAS_HEIGHT

canvas.fill_bg(lv.color_make(0xC0, 0xC0, 0xC0))
canvas.rotate(img, 30, 0, 0, CANVAS_WIDTH // 2, CANVAS_HEIGHT // 2)