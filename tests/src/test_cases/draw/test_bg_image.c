#if LV_BUILD_TEST || 1
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_func_1(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);

    LV_IMAGE_DECLARE(test_RGB565_RLE_align64);
    lv_obj_t * obj;

    /*Larger bg image*/
    obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 60, 40);
    lv_obj_set_style_bg_image_src(obj, &test_RGB565_RLE_align64, 0);
    lv_obj_set_style_radius(obj, 0, 0);

    obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 60, 40);
    lv_obj_set_style_bg_image_src(obj, &test_RGB565_RLE_align64, 0);
    lv_obj_set_style_radius(obj, 20, 0);

    /*Smaller bg image*/
    obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 200, 100);
    lv_obj_set_style_bg_image_src(obj, &test_RGB565_RLE_align64, 0);
    lv_obj_set_style_radius(obj, 0, 0);

    obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 200, 100);
    lv_obj_set_style_bg_image_src(obj, &test_RGB565_RLE_align64, 0);
    lv_obj_set_style_radius(obj, 20, 0);

    /*Smaller bg image tiled*/
    obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 200, 100);
    lv_obj_set_style_bg_image_src(obj, &test_RGB565_RLE_align64, 0);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_bg_image_tiled(obj, true, 0);

    obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 200, 100);
    lv_obj_set_style_bg_image_src(obj, &test_RGB565_RLE_align64, 0);
    lv_obj_set_style_radius(obj, 20, 0);
    lv_obj_set_style_bg_image_tiled(obj, true, 0);

    /*Smaller bg image tiled and opacity*/
    obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 200, 100);
    lv_obj_set_style_bg_image_src(obj, &test_RGB565_RLE_align64, 0);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_bg_image_tiled(obj, true, 0);
    lv_obj_set_style_bg_image_opa(obj, LV_OPA_50, 0);

    obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 200, 100);
    lv_obj_set_style_bg_image_src(obj, &test_RGB565_RLE_align64, 0);
    lv_obj_set_style_radius(obj, 20, 0);
    lv_obj_set_style_bg_image_tiled(obj, true, 0);
    lv_obj_set_style_bg_image_opa(obj, LV_OPA_50, 0);
#if LV_BIN_DECODER_RAM_LOAD
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/bg_image_1.png");
#endif
}

#endif
