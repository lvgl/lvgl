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

void test_barcode_set_data_rejects_invalid_data(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);

    lv_barcode_set_data(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL_STRING("https://lvgl.io", lv_barcode_get_data(barcode));
    TEST_ASSERT_NOT_NULL(lv_canvas_get_draw_buf(barcode));

    /*Empty data clears the barcode*/
    lv_barcode_set_data(barcode, "");
    TEST_ASSERT_NULL(lv_barcode_get_data(barcode));
    TEST_ASSERT_NULL(lv_canvas_get_draw_buf(barcode));

    lv_barcode_set_data(barcode, "LVGL");
    TEST_ASSERT_EQUAL_STRING("LVGL", lv_barcode_get_data(barcode));
    TEST_ASSERT_NOT_NULL(lv_canvas_get_draw_buf(barcode));
}

void test_barcode_set_scale_rejects_zero(void)
{
#if LV_USE_CHECK_ARG
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);

    lv_barcode_set_scale(barcode, 3);
    lv_barcode_set_scale(barcode, 0);
    TEST_ASSERT_EQUAL(3, lv_barcode_get_scale(barcode));
#endif
}

#else /*LV_USE_BARCODE*/

void setUp(void) { }
void tearDown(void) { }
void test_barcode_set_data_rejects_invalid_data(void) { }
void test_barcode_set_scale_rejects_zero(void) { }

#endif /*LV_USE_BARCODE*/
#endif /*LV_BUILD_TEST*/
