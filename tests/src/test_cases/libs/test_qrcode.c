#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#if LV_USE_QRCODE
#include <string.h>

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_qrcode_normal(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);

    /*Set data*/
    const char * data = "https://lvgl.io";
    lv_result_t res = lv_qrcode_set_data(qr, data);
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);
    lv_obj_center(qr);

    /*Add a border with bg_color*/
    lv_obj_set_style_border_color(qr, bg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qrcode_1.png");
}

void test_qrcode_color_after_data(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_size(qr, 150);

    /*Set the data first, then the colors. The colors must still be applied
     *(regression test for the setters being ignored after rendering)*/
    const char * data = "https://lvgl.io";
    lv_result_t res = lv_qrcode_set_data(qr, data);
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);

    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);
    lv_obj_center(qr);

    /*Add a border with bg_color*/
    lv_obj_set_style_border_color(qr, bg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qrcode_1.png");
}

void test_qrcode_manual_update(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);

    /*Disable auto update, set every property, then regenerate once explicitly*/
    lv_qrcode_set_auto_update(qr, false);
    TEST_ASSERT_FALSE(lv_qrcode_get_auto_update(qr));

    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);
    lv_qrcode_set_data(qr, "https://lvgl.io");

    lv_result_t res = lv_qrcode_update(qr);
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);

    /*A second consecutive update is a no-op but still reports success*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr));
    lv_obj_center(qr);

    lv_obj_set_style_border_color(qr, bg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qrcode_1.png");
}

void test_qrcode_auto_update_on_draw_when_forgotten(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);

    /*Disable auto update and "forget" to call lv_qrcode_update().
     *The QR code must still be regenerated on the next redraw.*/
    lv_qrcode_set_auto_update(qr, false);
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);
    lv_qrcode_set_data(qr, "https://lvgl.io");
    lv_obj_center(qr);

    lv_obj_set_style_border_color(qr, bg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qrcode_1.png");
}

void test_qrcode_get_data(void)
{
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);

    /*No data yet: the copy-out getter reports size 0 and copies nothing*/
    uint8_t out[8];
    TEST_ASSERT_EQUAL(0, lv_qrcode_get_data_binary(qr, out, sizeof(out)));
    TEST_ASSERT_EQUAL(0, lv_qrcode_get_data_binary(qr, NULL, 0));

    /*Binary data with an embedded NUL byte to prove the getter is length-based, not string-based*/
    const uint8_t data[] = {'a', 'b', 0x00, 'c', 'd'};
    lv_result_t res = lv_qrcode_set_data_binary(qr, data, sizeof(data));
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);

    TEST_ASSERT_EQUAL(sizeof(data), lv_qrcode_get_data_binary(qr, NULL, 0));

    /*Copy into a large-enough buffer: full size returned and all bytes copied*/
    lv_memset(out, 0xEE, sizeof(out));
    TEST_ASSERT_EQUAL(sizeof(data), lv_qrcode_get_data_binary(qr, out, sizeof(out)));
    TEST_ASSERT_EQUAL_MEMORY(data, out, sizeof(data));

    /*Copy into a too-small buffer: full size still returned, only buf_size copied (truncation)*/
    uint8_t small[2];
    TEST_ASSERT_EQUAL(sizeof(data), lv_qrcode_get_data_binary(qr, small, sizeof(small)));
    TEST_ASSERT_EQUAL_MEMORY(data, small, sizeof(small));

    /*NULL buffer only queries the size*/
    TEST_ASSERT_EQUAL(sizeof(data), lv_qrcode_get_data_binary(qr, NULL, 0));

    /*The string getter refuses binary data (embedded NUL -> not a valid string)*/
    TEST_ASSERT_NULL(lv_qrcode_get_data(qr));

    /*The string getter (get_data) mirrors set_data and round-trips text payloads*/
    const char * text = "https://lvgl.io";
    lv_qrcode_set_data(qr, text);
    TEST_ASSERT_EQUAL(strlen(text), lv_qrcode_get_data_binary(qr, NULL, 0));
    TEST_ASSERT_EQUAL_STRING(text, lv_qrcode_get_data(qr));

    /*Binary data that does not end with a NUL is not a string: get_data returns NULL*/
    const uint8_t ascii[] = {'h', 'i'};
    lv_qrcode_set_data_binary(qr, ascii, sizeof(ascii));
    TEST_ASSERT_NULL(lv_qrcode_get_data(qr));
    TEST_ASSERT_EQUAL(sizeof(ascii), lv_qrcode_get_data_binary(qr, NULL, 0));
}

void test_qrcode_quiet_zone(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);
    lv_qrcode_set_quiet_zone(qr, true);

    /*Set data*/
    const char * data = "https://lvgl.io";
    lv_result_t res = lv_qrcode_set_data(qr, data);
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);
    lv_obj_center(qr);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qrcode_2.png");
}

void test_qrcode_quiet_zone_after_data(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);

    /*Set the data first (renders without quiet zone), then enable the quiet zone.
     *This is the case the old code silently dropped: a geometry change after the
     *data must re-encode the bitmap. The result must match the quiet-zone reference.*/
    const char * data = "https://lvgl.io";
    lv_result_t res = lv_qrcode_set_data(qr, data);
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);

    lv_qrcode_set_quiet_zone(qr, true);
    lv_obj_center(qr);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qrcode_2.png");
}

void test_qrcode_update_force_and_auto_toggle(void)
{
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);

    /*No data set yet: update reports failure whether forced or not*/
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_update(qr));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_force_update(qr));

    lv_qrcode_set_auto_update(qr, false);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_set_data(qr, "https://lvgl.io"));

    /*First update regenerates; a second plain update is a no-op; force regenerates anyway*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_force_update(qr));

    /*A change in manual mode, then re-enabling auto update, regenerates immediately
     *and lv_qrcode_set_auto_update() returns that update's result*/
    lv_qrcode_set_quiet_zone(qr, true);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_set_auto_update(qr, true));
}

#else

void setUp(void)
{
}

void tearDown(void)
{
}

void test_qrcode_normal(void)
{
}

#endif

#endif
