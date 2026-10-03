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

void test_qrcode_set_text_rejects_null(void)
{
#if LV_USE_CHECK_ARG
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_text(qr, "https://lvgl.io");
    TEST_ASSERT_TRUE(lv_qrcode_is_render_valid(qr));

    /*The stored payload is kept*/
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_set_text(qr, NULL));
    TEST_ASSERT_TRUE(lv_qrcode_is_render_valid(qr));
    TEST_ASSERT_EQUAL_STRING("https://lvgl.io", lv_qrcode_get_text(qr));
#endif /*LV_USE_CHECK_ARG*/
}

void test_qrcode_set_data_rejects_invalid_arguments(void)
{
    /*The bound is enforced by LV_CHECK_ARG, which compiles to nothing when disabled.
     *The guard is inside the function because the Unity runner generator ignores
     *preprocessor directives and would reference a non-existent symbol otherwise.*/
#if LV_USE_CHECK_ARG
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_set_data(qr, NULL, 4));

    /*More bytes than any QR code can hold is rejected before anything is stored*/
    static char over_len[qrcodegen_BUFFER_LEN_MAX + 1];
    lv_memset(over_len, 'a', sizeof(over_len));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_set_data(qr, over_len, qrcodegen_BUFFER_LEN_MAX + 1));

    /*A previously set payload survives a rejected call*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_set_data(qr, "https://lvgl.io", 15));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_set_data(qr, over_len, qrcodegen_BUFFER_LEN_MAX + 1));
#endif /*LV_USE_CHECK_ARG*/
}

void test_qrcode_set_text_rejects_invalid_length(void)
{
#if LV_USE_CHECK_ARG
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);

    static char text[qrcodegen_BUFFER_LEN_MAX + 2];
    lv_memset(text, 'a', sizeof(text) - 1);
    text[sizeof(text) - 1] = '\0';

    /*The maximum length passes the check and is stored, the same as for set_data(). No
     *QR code can hold it, so the encode still fails.*/
    text[qrcodegen_BUFFER_LEN_MAX] = '\0';
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_set_text(qr, text));
    TEST_ASSERT_EQUAL(qrcodegen_BUFFER_LEN_MAX, lv_qrcode_get_data(qr, NULL, 0));

    /*One byte more is rejected, and the stored payload is kept*/
    text[qrcodegen_BUFFER_LEN_MAX] = 'a';
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_set_text(qr, text));
    TEST_ASSERT_EQUAL(qrcodegen_BUFFER_LEN_MAX, lv_qrcode_get_data(qr, NULL, 0));
#endif /*LV_USE_CHECK_ARG*/
}

#else /*LV_USE_QRCODE*/

void setUp(void) { }
void tearDown(void) { }
void test_qrcode_set_text_rejects_null(void) { }
void test_qrcode_set_data_rejects_invalid_arguments(void) { }
void test_qrcode_set_text_rejects_invalid_length(void) { }

#endif /*LV_USE_QRCODE*/
#endif /*LV_BUILD_TEST*/
