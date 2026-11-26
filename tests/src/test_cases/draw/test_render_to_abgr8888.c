#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "../demos/lv_demos.h"

#include "unity/unity.h"

/*Only the software renderer can render to ABGR8888*/
#if LV_DRAW_SW_SUPPORT_ABGR8888 && !LV_USE_DRAW_VG_LITE && !LV_USE_DRAW_NANOVG

void setUp(void)
{
    lv_display_set_color_format(lv_display_get_default(), LV_COLOR_FORMAT_ABGR8888);
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
    lv_obj_remove_local_style_prop(lv_screen_active(), LV_STYLE_BG_COLOR, 0);
    lv_display_set_color_format(lv_display_get_default(), LV_COLOR_FORMAT_XRGB8888);
}

static const uint8_t * get_px(int32_t x, int32_t y)
{
    lv_draw_buf_t * buf = lv_display_get_buf_active(lv_display_get_default());
    return buf->data + y * buf->header.stride + x * 4;
}

static void assert_px_rgb(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    const uint8_t * px = get_px(x, y);
    TEST_ASSERT_EQUAL_HEX8(r, px[0]);
    TEST_ASSERT_EQUAL_HEX8(g, px[1]);
    TEST_ASSERT_EQUAL_HEX8(b, px[2]);
}

/*Same algorithm as ARGB8888 with an other byte order, so it must match the ARGB8888 references*/
void test_render_to_abgr8888(void)
{
    lv_opa_t opa_values[2] = {0xff, 0x80};
    uint32_t opa;
    for(opa = 0; opa < 2; opa++) {
        uint32_t i;
        for(i = 0; i < LV_DEMO_RENDER_SCENE_NUM; i++) {

            /*Skip test with transformed indexed images if they are not loaded to RAM*/
            if(LV_BIN_DECODER_RAM_LOAD == 0 &&
               (i == LV_DEMO_RENDER_SCENE_IMAGE_NORMAL_3 ||
                i == LV_DEMO_RENDER_SCENE_IMAGE_RECOLOR_3)) continue;

            lv_demo_render(i, opa_values[opa]);

            char buf[128];
            lv_snprintf(buf, sizeof(buf), "draw/render/argb8888/demo_render_%s_opa_%d.png",
                        lv_demo_render_get_scene_name(i), opa_values[opa]);
            TEST_ASSERT_EQUAL_SCREENSHOT(buf);
        }
    }
}

void test_render_to_abgr8888_fill_byte_order(void)
{
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x204080), 0);
    lv_refr_now(NULL);

    assert_px_rgb(10, 10, 0x20, 0x40, 0x80);
    TEST_ASSERT_EQUAL_HEX8(0xff, get_px(10, 10)[3]);
}

void test_render_to_abgr8888_argb8888_image(void)
{
    /*B, G, R, A bytes*/
    static const uint8_t pixels[2 * 2 * 4] = {
        0x80, 0x40, 0x20, 0xff,  0x80, 0x40, 0x20, 0xff,
        0x80, 0x40, 0x20, 0xff,  0x80, 0x40, 0x20, 0xff,
    };

    static const lv_image_dsc_t src = {
        .header = {
            .magic = LV_IMAGE_HEADER_MAGIC,
            .cf = LV_COLOR_FORMAT_ARGB8888,
            .w = 2,
            .h = 2,
            .stride = 2 * 4
        },
        .data_size = sizeof(pixels),
        .data = pixels,
    };

    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &src);
    lv_obj_set_pos(img, 0, 0);
    lv_refr_now(NULL);

    assert_px_rgb(1, 1, 0x20, 0x40, 0x80);
}

void test_render_to_abgr8888_transformed_layer(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 100, 100);
    lv_obj_set_pos(obj, 50, 50);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x204080), 0);
    lv_obj_set_style_transform_rotation(obj, 50, 0);
    lv_obj_set_style_opa_layered(obj, LV_OPA_COVER, 0);
    lv_refr_now(NULL);

    assert_px_rgb(100, 100, 0x20, 0x40, 0x80);
}

#else

void setUp(void)
{
}

void tearDown(void)
{
}

void test_render_to_abgr8888(void)
{
    TEST_IGNORE_MESSAGE("ABGR8888 is disabled or not supported by the renderer");
}

void test_render_to_abgr8888_fill_byte_order(void)
{
    TEST_IGNORE_MESSAGE("ABGR8888 is disabled or not supported by the renderer");
}

void test_render_to_abgr8888_argb8888_image(void)
{
    TEST_IGNORE_MESSAGE("ABGR8888 is disabled or not supported by the renderer");
}

void test_render_to_abgr8888_transformed_layer(void)
{
    TEST_IGNORE_MESSAGE("ABGR8888 is disabled or not supported by the renderer");
}

#endif

#endif
