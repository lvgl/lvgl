#if LV_BUILD_TEST
#include "../lvgl.h"

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

    lv_barcode_set_dark_color(barcode, dark_color);
    lv_barcode_set_light_color(barcode, light_color);
    lv_barcode_set_scale(barcode, scale);

    TEST_ASSERT_EQUAL_COLOR(lv_barcode_get_dark_color(barcode), dark_color);
    TEST_ASSERT_EQUAL_COLOR(lv_barcode_get_light_color(barcode), light_color);
    TEST_ASSERT_EQUAL(lv_barcode_get_scale(barcode), scale);

    lv_barcode_set_direction(barcode, LV_DIR_HOR);
    lv_result_t res = lv_barcode_update(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);

    lv_image_dsc_t * image_dsc = lv_canvas_get_image(barcode);
    TEST_ASSERT_NOT_NULL(image_dsc);

    lv_obj_set_size(barcode, image_dsc->header.w, 50);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");

    lv_barcode_set_direction(barcode, LV_DIR_VER);
    res = lv_barcode_update(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);

    image_dsc = lv_canvas_get_image(barcode);
    TEST_ASSERT_NOT_NULL(image_dsc);

    lv_obj_set_size(barcode, 50, image_dsc->header.h);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_2.png");

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
