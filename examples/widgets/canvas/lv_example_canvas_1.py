_CANVAS_WIDTH = 200
_CANVAS_HEIGHT = 150
LV_IMAGE_ZOOM_NONE = 256

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

image = lv.image_dsc_t()

image.data = cbuf[:]
image.header.cf = lv.COLOR_FORMAT.NATIVE
image.header.w = _CANVAS_WIDTH
image.header.h = _CANVAS_HEIGHT

canvas.fill_bg(lv.palette_lighten(lv.PALETTE.GREY, 3), lv.OPA.COVER)


image_dsc = lv.draw_image_dsc_t()
image_dsc.init();
image_dsc.angle = 120;
image_dsc.src = image;
image_dsc.pivot.x = _CANVAS_WIDTH // 2;
image_dsc.pivot.y = _CANVAS_HEIGHT // 2;

coords_image = lv.area_t()
coords_image.x1 = 0
coords_image.y1 = 0
coords_image.x2 = _CANVAS_WIDTH - 1
coords_image.y2 = _CANVAS_HEIGHT - 1

lv.draw_image(layer, image_dsc, coords_image)

