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

void test_barcode_update_rejects_invalid_data(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_set_height(barcode, 50);

#if LV_USE_CHECK_ARG
    /*Without the argument check the data is dereferenced, so only assert this when it is on*/
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_barcode_update(barcode, NULL));
    TEST_ASSERT_FALSE(lv_barcode_is_render_valid(barcode));
#endif

    /*Nothing to encode*/
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_barcode_update(barcode, ""));
    TEST_ASSERT_FALSE(lv_barcode_is_render_valid(barcode));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_update(barcode, "https://lvgl.io"));
    TEST_ASSERT_TRUE(lv_barcode_is_render_valid(barcode));

    /*Emptying forgets the data, so a property change cannot bring the old barcode back*/
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_barcode_update(barcode, ""));
    TEST_ASSERT_FALSE(lv_barcode_is_render_valid(barcode));
    lv_barcode_set_scale(barcode, 2);
    TEST_ASSERT_FALSE(lv_barcode_is_render_valid(barcode));
}

#else /*LV_USE_BARCODE*/

void setUp(void) { }
void tearDown(void) { }
void test_barcode_update_rejects_invalid_data(void) { }

#endif /*LV_USE_BARCODE*/
#endif /*LV_BUILD_TEST*/
