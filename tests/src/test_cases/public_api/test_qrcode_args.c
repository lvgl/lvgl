#if LV_BUILD_TEST
#include "../../lvgl_private.h"

#include "unity/unity.h"

#if LV_USE_QRCODE
#include "../../../src/libs/qrcode/qrcodegen.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_qrcode_set_data_ignores_null(void)
{
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_data(qr, "https://lvgl.io");
    TEST_ASSERT_TRUE(lv_qrcode_is_render_valid(qr));

    /*A NULL string is a no-op, not a crash, and leaves the stored payload alone.
     *The guard is unconditional because lv_strlen() would dereference it even when
     *argument checks are compiled out.*/
    lv_qrcode_set_data(qr, NULL);
    TEST_ASSERT_TRUE(lv_qrcode_is_render_valid(qr));
    TEST_ASSERT_EQUAL(15, ((lv_qrcode_t *)qr)->data_len);
}

void test_qrcode_update_rejects_invalid_arguments(void)
{
    /*The bound is enforced by LV_CHECK_ARG, which compiles to nothing when disabled.
     *The guard is inside the function because the Unity runner generator ignores
     *preprocessor directives and would reference a non-existent symbol otherwise.*/
#if LV_USE_CHECK_ARG
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_update(qr, NULL, 4));

    /*More bytes than any QR code can hold is rejected before anything is stored*/
    static char over_len[qrcodegen_BUFFER_LEN_MAX + 1];
    lv_memset(over_len, 'a', sizeof(over_len));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_update(qr, over_len, qrcodegen_BUFFER_LEN_MAX + 1));

    /*A previously set payload survives a rejected call*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr, "https://lvgl.io", 15));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_update(qr, over_len, qrcodegen_BUFFER_LEN_MAX + 1));
#endif /*LV_USE_CHECK_ARG*/
}

#else /*LV_USE_QRCODE*/

void setUp(void) { }
void tearDown(void) { }
void test_qrcode_set_data_ignores_null(void) { }
void test_qrcode_update_rejects_invalid_arguments(void) { }

#endif /*LV_USE_QRCODE*/
#endif /*LV_BUILD_TEST*/
