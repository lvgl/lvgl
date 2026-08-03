#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#if LV_USE_QRCODE
#include "../../../src/libs/qrcode/qrcodegen.h"

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

void test_qrcode_update_and_auto_toggle(void)
{
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);

    /*No data set yet: update reports failure, and keeps reporting it when repeated*/
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_update(qr));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_update(qr));

    lv_qrcode_set_auto_update(qr, false);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_set_data(qr, "https://lvgl.io"));

    /*First update regenerates; a second plain update is a no-op but still reports success*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr));

    /*Forcing a regeneration without a property change: mark the bitmap dirty through
     *the private field, as documented on lv_qrcode_update()*/
    ((lv_qrcode_t *)qr)->needs_update = true;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr));

    /*A change in manual mode, then re-enabling auto update, regenerates immediately
     *and lv_qrcode_set_auto_update() returns that update's result*/
    lv_qrcode_set_quiet_zone(qr, true);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_set_auto_update(qr, true));
}

void test_qrcode_update_reports_render_failure(void)
{
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_auto_update(qr, false);

    /*A payload that is short enough to be stored but too long to be encoded even by
     *the largest QR code version (byte mode, ECC MEDIUM tops out at ~2331 bytes)*/
    static char too_long[3000];
    lv_memset(too_long, 'a', sizeof(too_long) - 1);
    too_long[sizeof(too_long) - 1] = '\0';
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_set_data(qr, too_long));

    /*The bitmap cannot be generated. The failure must be reported, and it must keep
     *being reported: a later call skips the (deterministic) work but must not turn
     *that into a claim of success.*/
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_update(qr));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_update(qr));

    /*Recovering with an encodable payload works and is reported as success*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_set_data(qr, "https://lvgl.io"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr));
}

void test_qrcode_canvas_too_small_is_reported(void)
{
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_auto_update(qr, false);

    /*"https://lvgl.io" needs a version 2 code, i.e. 25 modules. A 10 px canvas cannot
     *hold even one pixel per module, so the bitmap would stay blank. That must be
     *reported as a failure rather than as a successfully rendered QR code.*/
    lv_qrcode_set_size(qr, 10);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_set_data(qr, "https://lvgl.io"));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_update(qr));

    /*The same holds with a quiet zone, which needs even more room*/
    lv_qrcode_set_quiet_zone(qr, true);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_update(qr));

    /*A canvas large enough for both renders and reports success*/
    lv_qrcode_set_size(qr, 150);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr));
    lv_qrcode_set_quiet_zone(qr, false);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr));
}

void test_qrcode_set_data_rejects_oversized_payloads(void)
{
    /*The bound is enforced by LV_CHECK_ARG, which compiles to nothing when disabled.
     *The guard is inside the function because the Unity runner generator ignores
     *preprocessor directives and would reference a non-existent symbol otherwise.*/
#if LV_USE_CHECK_ARG
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);

    /*Only the argument checks are under test here; these payloads are far too large to
     *encode, so keep the automatic bitmap generation (and its failures) out of the way*/
    lv_qrcode_set_auto_update(qr, false);

    /*A string of exactly the maximum length is stored with its NUL terminator, which
     *must still fit the qrcodegen_BUFFER_LEN_MAX bound the binary setter enforces*/
    static char max_len[qrcodegen_BUFFER_LEN_MAX];
    lv_memset(max_len, 'a', sizeof(max_len) - 1);
    max_len[sizeof(max_len) - 1] = '\0';
    TEST_ASSERT_EQUAL(qrcodegen_BUFFER_LEN_MAX - 1, lv_strlen(max_len));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_set_data(qr, max_len));
    TEST_ASSERT_EQUAL(qrcodegen_BUFFER_LEN_MAX - 1, lv_qrcode_get_data_binary(qr, NULL, 0));

    /*One byte more is rejected: the stored copy would exceed the bound*/
    static char over_len[qrcodegen_BUFFER_LEN_MAX + 1];
    lv_memset(over_len, 'a', sizeof(over_len) - 1);
    over_len[sizeof(over_len) - 1] = '\0';
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_set_data(qr, over_len));

    /*The rejected call must not have replaced the previously stored data*/
    TEST_ASSERT_EQUAL(qrcodegen_BUFFER_LEN_MAX - 1, lv_qrcode_get_data_binary(qr, NULL, 0));

    /*The binary setter enforces the same bound, with no byte reserved for a NUL*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_set_data_binary(qr, over_len, qrcodegen_BUFFER_LEN_MAX));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_set_data_binary(qr, over_len, qrcodegen_BUFFER_LEN_MAX + 1));
#endif /*LV_USE_CHECK_ARG*/
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
