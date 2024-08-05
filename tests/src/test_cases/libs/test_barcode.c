#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#if LV_USE_BARCODE

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_barcode_normal(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);

    lv_obj_center(barcode);

    lv_color_t dark_color = lv_color_black();
    lv_color_t light_color = lv_color_white();
    uint16_t scale = 2;
    lv_result_t res;

    lv_barcode_set_dark_color(barcode, dark_color);
    lv_barcode_set_light_color(barcode, light_color);
    lv_barcode_set_scale(barcode, scale);

    TEST_ASSERT_EQUAL_COLOR(lv_barcode_get_dark_color(barcode), dark_color);
    TEST_ASSERT_EQUAL_COLOR(lv_barcode_get_light_color(barcode), light_color);
    TEST_ASSERT_EQUAL(lv_barcode_get_scale(barcode), scale);

    /* Test horizontal mode */
    lv_barcode_set_direction(barcode, LV_DIR_HOR);
    lv_obj_set_height(barcode, 50);
    res = lv_barcode_update(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");

    /* Test vertical mode */
    lv_barcode_set_direction(barcode, LV_DIR_VER);
    lv_obj_set_size(barcode, 50, LV_SIZE_CONTENT);
    res = lv_barcode_update(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_2.png");

    /* Test tiled + horizontal mode */
    lv_barcode_set_tiled(barcode, true);
    lv_barcode_set_direction(barcode, LV_DIR_HOR);
    lv_obj_set_size(barcode, LV_SIZE_CONTENT, 50);

    res = lv_barcode_update(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_tiled_1.png");

    /* Test tiled + vertical mode */
    lv_barcode_set_direction(barcode, LV_DIR_VER);
    lv_obj_set_size(barcode, 50, LV_SIZE_CONTENT);
    res = lv_barcode_update(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_tiled_2.png");
}

#else

void setUp(void)
{
}

void tearDown(void)
{
}

void test_barcode_normal(void)
{
}

#endif

#endif
