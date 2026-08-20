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
    res = lv_barcode_set_data(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");

    /* Test vertical mode */
    lv_barcode_set_direction(barcode, LV_DIR_VER);
    lv_obj_set_size(barcode, 50, LV_SIZE_CONTENT);
    res = lv_barcode_set_data(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_2.png");

    /* Test tiled + horizontal mode */
    lv_barcode_set_tiled(barcode, true);
    lv_barcode_set_direction(barcode, LV_DIR_HOR);
    lv_obj_set_size(barcode, LV_SIZE_CONTENT, 50);

    res = lv_barcode_set_data(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_tiled_1.png");

    /* Test tiled + vertical mode */
    lv_barcode_set_direction(barcode, LV_DIR_VER);
    lv_obj_set_size(barcode, 50, LV_SIZE_CONTENT);
    res = lv_barcode_set_data(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_tiled_2.png");
}

void test_barcode_property_after_data(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, 50);

    /*Set the data first, then the properties - they must still take effect
     *(regression: previously properties set after the data were ignored).*/
    lv_result_t res = lv_barcode_set_data(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);

    lv_barcode_set_direction(barcode, LV_DIR_HOR);
    lv_barcode_set_scale(barcode, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");
}

void test_barcode_size_change_after_data(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_barcode_set_scale(barcode, 2);
    lv_barcode_set_direction(barcode, LV_DIR_HOR);
    lv_barcode_set_data(barcode, "https://lvgl.io");

    /*Change only the object height afterwards (no further set_data/update call).
     *The SIZE_CHANGED handler must regenerate the bitmap to the new size.*/
    lv_obj_set_height(barcode, 50);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");
}

void test_barcode_manual_update(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, 50);

    /*Disable auto update, set every property, then regenerate once explicitly*/
    lv_barcode_set_auto_update(barcode, false);
    TEST_ASSERT_FALSE(lv_barcode_get_auto_update(barcode));

    lv_barcode_set_direction(barcode, LV_DIR_HOR);
    lv_barcode_set_scale(barcode, 2);
    lv_barcode_set_data(barcode, "https://lvgl.io");

    lv_result_t res = lv_barcode_update(barcode);
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);

    /*A second consecutive update is a no-op but still reports success*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_update(barcode));
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");
}

void test_barcode_deferred_draw_on_redraw(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, 50);

    /*Manual mode, then "forget" to call lv_barcode_update(). The canvas is resized
     *eagerly when the data is set, but the bars are only drawn on the next redraw.*/
    lv_barcode_set_auto_update(barcode, false);
    lv_barcode_set_direction(barcode, LV_DIR_HOR);
    lv_barcode_set_scale(barcode, 2);
    lv_barcode_set_data(barcode, "https://lvgl.io");

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");
}

void test_barcode_get_data(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);

    TEST_ASSERT_NULL(lv_barcode_get_data(barcode));
    TEST_ASSERT_TRUE(lv_barcode_get_auto_update(barcode));

    /*Empty data is treated as no data: rejected, and nothing is stored*/
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_barcode_set_data(barcode, ""));
    TEST_ASSERT_NULL(lv_barcode_get_data(barcode));

    const char * data = "https://lvgl.io";
    lv_obj_set_height(barcode, 50);
    lv_barcode_set_data(barcode, data);
    TEST_ASSERT_EQUAL_STRING(data, lv_barcode_get_data(barcode));
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
