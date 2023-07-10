_CANVAS_WIDTH = 200
_CANVAS_HEIGHT = 150
LV_IMG_ZOOM_NONE = 256

rect_dsc = lv.draw_rect_dsc_t()
rect_dsc.init()
rect_dsc.radius = 10
rect_dsc.bg_opa = lv.OPA.COVER
rect_dsc.bg_grad.dir = lv.GRAD_DIR.HOR

rect_dsc.bg_grad.stops = [
    lv.gradient_stop_t({'color': lv.palette_main(lv.PALETTE.RED)}),
    lv.gradient_stop_t({'color': lv.palette_main(lv.PALETTE.BLUE), 'frac':0xff})
]

rect_dsc.border_width = 2
rect_dsc.border_opa = lv.OPA._90
rect_dsc.border_color = lv.color_white()
rect_dsc.shadow_width = 5
rect_dsc.shadow_ofs_x = 5
rect_dsc.shadow_ofs_y = 5

label_dsc = lv.draw_label_dsc_t()
label_dsc.init()
label_dsc.color = lv.palette_main(lv.PALETTE.ORANGE)
label_dsc.text = "Some text on text canvas"

cbuf = bytearray(_CANVAS_WIDTH * _CANVAS_HEIGHT * 4)
# cbuf2 = bytearray(_CANVAS_WIDTH * _CANVAS_HEIGHT * 4)

canvas = lv.canvas(lv.scr_act())
canvas.set_buffer(cbuf, _CANVAS_WIDTH, _CANVAS_HEIGHT, lv.COLOR_FORMAT.NATIVE)
canvas.center()
canvas.fill_bg(lv.palette_lighten(lv.PALETTE.GREY, 3), lv.OPA.COVER)

layer = lv.layer_t()
canvas.init_layer(layer);

coords_rect = lv.area_t()
coords_rect.x1 = 70
coords_rect.y1 = 60
coords_rect.x2 = 100
coords_rect.y2 = 70

lv.draw_rect(layer, rect_dsc, coords_rect)

coords_text = lv.area_t()
coords_text.x1 = 40
coords_text.y1 = 80
coords_text.x2 = 100
coords_text.y2 = 120

lv.draw_label(layer, label_dsc, coords_text)



canvas.finish_layer(layer)


# Test the rotation. It requires another buffer where the original image is stored.
# So copy the current image to buffer and rotate it to the canvas

img = lv.img_dsc_t()

img.data = cbuf[:]
img.header.cf = lv.COLOR_FORMAT.NATIVE
img.header.w = _CANVAS_WIDTH
img.header.h = _CANVAS_HEIGHT

canvas.fill_bg(lv.palette_lighten(lv.PALETTE.GREY, 3), lv.OPA.COVER)


img_dsc = lv.draw_img_dsc_t()
img_dsc.init();
img_dsc.angle = 120;
img_dsc.src = img;
img_dsc.pivot.x = _CANVAS_WIDTH // 2;
img_dsc.pivot.y = _CANVAS_HEIGHT // 2;

coords_img = lv.area_t()
coords_img.x1 = 0
coords_img.y1 = 0
coords_img.x2 = _CANVAS_WIDTH - 1
coords_img.y2 = _CANVAS_HEIGHT - 1

lv.draw_img(layer, img_dsc, coords_img)

