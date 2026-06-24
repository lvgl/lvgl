#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"

void setUp(void)
{
    lv_obj_clean(lv_screen_active());
}

void tearDown(void) { }

void test_image_recolor_xrgb8888_preserves_opaque_x_byte(void)
{
    lv_display_t * disp = lv_display_get_default();
    if(lv_display_get_color_format(disp) != LV_COLOR_FORMAT_XRGB8888) {
        TEST_IGNORE_MESSAGE("test display is not XRGB8888");
    }

    static const uint8_t pixels[2 * 2 * 4] = {
        0x40, 0x80, 0xC0, 0xFF,  0x40, 0x80, 0xC0, 0xFF,
        0x40, 0x80, 0xC0, 0xFF,  0x40, 0x80, 0xC0, 0xFF,
    };
    static lv_image_dsc_t src = {
        .header = {
            .magic = LV_IMAGE_HEADER_MAGIC,
            .cf = LV_COLOR_FORMAT_XRGB8888,
            .w = 2,
            .h = 2,
            .stride = 2 * 4,
        },
        .data_size = sizeof(pixels),
        .data = pixels,
    };

    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &src);
    lv_obj_set_style_image_recolor(img, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_image_recolor_opa(img, LV_OPA_70, 0);

    lv_refr_now(NULL);

    lv_draw_buf_t * buf = lv_display_get_buf_active(disp);
    int32_t w = buf->header.w;
    int32_t h = buf->header.h;
    uint32_t stride = buf->header.stride;
    const uint8_t * data = buf->data;

    for(int32_t y = 0; y < h; y++) {
        const uint8_t * row = data + y * stride;
        for(int32_t x = 0; x < w; x++) {
            TEST_ASSERT_EQUAL_HEX8(0xff, row[x * 4 + 3]);
        }
    }
}

#endif
