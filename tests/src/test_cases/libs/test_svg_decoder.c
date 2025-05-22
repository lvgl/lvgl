#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static void assert_screenshot(const char * path)
{
#ifndef NON_AMD64_BUILD
    char fn_buf[64];
    lv_snprintf(fn_buf, sizeof(fn_buf), "libs/%s.lp64.png", path);
    TEST_ASSERT_EQUAL_SCREENSHOT(fn_buf);
#else
    char fn_buf[64];
    lv_snprintf(fn_buf, sizeof(fn_buf), "libs/%s.lp32.png", path);
    TEST_ASSERT_EQUAL_SCREENSHOT(fn_buf);
#endif
}

void test_svg_decoder(void)
{
    LV_IMAGE_DECLARE(test_image_svg);
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_obj_set_size(img, lv_pct(100), lv_pct(50));
    lv_obj_set_style_outline_width(img, 4, 0);
    lv_image_set_src(img, &test_image_svg);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    assert_screenshot("svg_decoder_1");
}

void test_svg_decoder_file(void)
{
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_obj_set_size(img, lv_pct(100), lv_pct(100));
    lv_obj_set_style_outline_width(img, 4, 0);
    lv_image_set_src(img, "A:src/test_assets/test_img_svg_tiger.svg");
    lv_image_set_scale(img, 96);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    assert_screenshot("svg_decoder_2");
}

void test_svg_snapshot(void)
{
    LV_IMAGE_DECLARE(test_image_svg);
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &test_image_svg);
    lv_obj_center(img);

    lv_draw_buf_t * draw_buf = lv_snapshot_take(img, LV_COLOR_FORMAT_ARGB8888);
    lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_t * img2 = lv_image_create(lv_screen_active());
    lv_image_set_src(img2, draw_buf);
    lv_obj_set_style_outline_width(img2, 2, 0);
    lv_obj_set_style_outline_color(img2, lv_color_hex3(0xf00), 0);
    lv_obj_center(img2);
    assert_screenshot("svg_decoder_3");
    lv_draw_buf_destroy(draw_buf);
}

#endif
