#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "../demos/lv_demos.h"

#include "unity/unity.h"

#if LV_DRAW_SW_SUPPORT_ABGR8888

void setUp(void)
{
    lv_display_set_color_format(lv_display_get_default(), LV_COLOR_FORMAT_ABGR8888);
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_white(), 0);
    lv_display_set_color_format(lv_display_get_default(), LV_COLOR_FORMAT_XRGB8888);
}

static const uint8_t * get_px(int32_t x, int32_t y)
{
    lv_draw_buf_t * buf = lv_display_get_buf_active(lv_display_get_default());
    return buf->data + y * buf->header.stride + x * 4;
}

void test_render_to_abgr8888(void)
{
    lv_opa_t opa_values[2] = {0xff, 0x80};
    uint32_t opa;
    for(opa = 0; opa < 2; opa++) {
        uint32_t i;
        for(i = 0; i < LV_DEMO_RENDER_SCENE_NUM; i++) {
            if(i == LV_DEMO_RENDER_SCENE_IMAGE_NORMAL_1 || i == LV_DEMO_RENDER_SCENE_IMAGE_RECOLOR_1 ||
               i == LV_DEMO_RENDER_SCENE_IMAGE_NORMAL_2 || i == LV_DEMO_RENDER_SCENE_IMAGE_RECOLOR_2 ||
               i == LV_DEMO_RENDER_SCENE_IMAGE_NORMAL_3 || i == LV_DEMO_RENDER_SCENE_IMAGE_RECOLOR_3 ||
               i == LV_DEMO_RENDER_SCENE_ARC_IMAGE || i == LV_DEMO_RENDER_SCENE_BLEND_MODE) continue;

            lv_demo_render(i, opa_values[opa]);

            char buf[128];
            const char * ref_dir = i == LV_DEMO_RENDER_SCENE_LAYER_NORMAL ? "abgr8888" : "argb8888";
            lv_snprintf(buf, sizeof(buf), "draw/render/%s/demo_render_%s_opa_%d.png", ref_dir,
                        lv_demo_render_get_scene_name(i), opa_values[opa]);
            TEST_ASSERT_EQUAL_SCREENSHOT(buf);
        }
    }
}

void test_render_to_abgr8888_fill_byte_order(void)
{
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x204080), 0);
    lv_refr_now(NULL);

    const uint8_t * px = get_px(10, 10);
    TEST_ASSERT_EQUAL_HEX8(0x20, px[0]);
    TEST_ASSERT_EQUAL_HEX8(0x40, px[1]);
    TEST_ASSERT_EQUAL_HEX8(0x80, px[2]);
    TEST_ASSERT_EQUAL_HEX8(0xff, px[3]);
}

void test_render_to_abgr8888_image(void)
{
    static const uint8_t pixels[2 * 64] = {
        0x20, 0x40, 0x80, 0xff,  0x20, 0x40, 0x80, 0xff,
    };

    static lv_image_dsc_t src = {
        .header = {
            .magic = LV_IMAGE_HEADER_MAGIC,
            .cf = LV_COLOR_FORMAT_ABGR8888,
            .w = 2,
            .h = 2,
            .stride = 64
        },
        .data_size = sizeof(pixels),
        .data = pixels,
    };

    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &src);
    lv_obj_set_pos(img, 0, 0);
    lv_refr_now(NULL);

    const uint8_t * px = get_px(1, 0);
    TEST_ASSERT_EQUAL_HEX8(0x20, px[0]);
    TEST_ASSERT_EQUAL_HEX8(0x40, px[1]);
    TEST_ASSERT_EQUAL_HEX8(0x80, px[2]);
    TEST_ASSERT_EQUAL_HEX8(0xff, px[3]);
}

void test_render_to_abgr8888_image_recolor(void)
{
    static const uint8_t pixels[2 * 64] = {
        0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,
    };

    static lv_image_dsc_t src = {
        .header = {
            .magic = LV_IMAGE_HEADER_MAGIC,
            .cf = LV_COLOR_FORMAT_ABGR8888,
            .w = 2,
            .h = 2,
            .stride = 64
        },
        .data_size = sizeof(pixels),
        .data = pixels,
    };

    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &src);
    lv_obj_set_pos(img, 0, 0);
    lv_obj_set_style_image_recolor(img, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_image_recolor_opa(img, LV_OPA_COVER, 0);
    lv_refr_now(NULL);

    const uint8_t * px = get_px(1, 0);
    TEST_ASSERT_EQUAL_HEX8(0xff, px[0]);
    TEST_ASSERT_EQUAL_HEX8(0x00, px[1]);
    TEST_ASSERT_EQUAL_HEX8(0x00, px[2]);
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

    const uint8_t * px = get_px(100, 100);
    TEST_ASSERT_EQUAL_HEX8(0x20, px[0]);
    TEST_ASSERT_EQUAL_HEX8(0x40, px[1]);
    TEST_ASSERT_EQUAL_HEX8(0x80, px[2]);
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
    TEST_IGNORE_MESSAGE("LV_DRAW_SW_SUPPORT_ABGR8888 is not enabled");
}

#endif

#endif
