#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#if LV_USE_QRCODE
#include "../../../src/libs/qrcode/qrcodegen.h"

#include <string.h>

static lv_obj_t * active_screen = NULL;
static uint32_t encode_error_cnt;

static uint32_t redraw_warning_cnt;

static void count_qrcode_logs_cb(lv_log_level_t level, const char * buf)
{
    LV_UNUSED(level);
    /*Emitted by the draw hook when the re-encode it had to do failed*/
    if(strstr(buf, "could not be re-encoded during the redraw") != NULL) encode_error_cnt++;
    /*Emitted by the draw hook when a deferred change was not applied explicitly*/
    if(strstr(buf, "was not called after the property changes") != NULL) redraw_warning_cnt++;
}

void setUp(void)
{
    active_screen = lv_screen_active();
    encode_error_cnt = 0;
    redraw_warning_cnt = 0;
}

void tearDown(void)
{
    lv_log_register_print_cb(NULL);
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
    lv_qrcode_set_data(qr, data);
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
    lv_qrcode_set_data(qr, "https://lvgl.io");

    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);
    lv_obj_center(qr);

    /*Add a border with bg_color*/
    lv_obj_set_style_border_color(qr, bg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qrcode_1.png");
}

void test_qrcode_size_after_data(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);

    /*Set the data before the size, so the bitmap is first encoded into the default
     *sized buffer. Changing the size afterwards must re-encode it, not leave the
     *fresh (empty) buffer as it is.*/
    lv_qrcode_set_data(qr, "https://lvgl.io");
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);
    lv_obj_center(qr);

    lv_obj_set_style_border_color(qr, bg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qrcode_1.png");
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
    lv_qrcode_set_data(qr, "https://lvgl.io");
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
    lv_qrcode_set_data(qr, "https://lvgl.io");

    lv_qrcode_set_quiet_zone(qr, true);
    lv_obj_center(qr);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qrcode_2.png");
}

void test_qrcode_update_mode_default_is_immediate(void)
{
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);

    /*Immediate is the default so that existing code keeps its behaviour*/
    TEST_ASSERT_EQUAL(LV_QRCODE_UPDATE_MODE_IMMEDIATE, lv_qrcode_get_update_mode(qr));

    lv_qrcode_set_update_mode(qr, LV_QRCODE_UPDATE_MODE_DEFERRED);
    TEST_ASSERT_EQUAL(LV_QRCODE_UPDATE_MODE_DEFERRED, lv_qrcode_get_update_mode(qr));

    lv_qrcode_set_update_mode(qr, LV_QRCODE_UPDATE_MODE_IMMEDIATE);
    TEST_ASSERT_EQUAL(LV_QRCODE_UPDATE_MODE_IMMEDIATE, lv_qrcode_get_update_mode(qr));
}

void test_qrcode_update_mode_deferred_renders_on_redraw(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);

    /*Deferred mode expects an explicit lv_qrcode_update(). This test deliberately omits
     *it to cover the fallback: the redraw notices the bitmap is out of date, warns, and
     *encodes it anyway, so the result must still be correct.*/
    lv_log_register_print_cb(count_qrcode_logs_cb);
    lv_qrcode_set_update_mode(qr, LV_QRCODE_UPDATE_MODE_DEFERRED);
    lv_qrcode_set_data(qr, "https://lvgl.io");
    lv_qrcode_set_quiet_zone(qr, true);
    lv_obj_center(qr);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qrcode_2.png");
    lv_log_register_print_cb(NULL);

    /*Falling through to the redraw is the warned fallback, and warns once per change*/
#if LV_USE_LOG
    TEST_ASSERT_EQUAL(1, redraw_warning_cnt);
#endif
}

void test_qrcode_render_applies_deferred_changes(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);
    lv_qrcode_set_data(qr, "https://lvgl.io");

    lv_log_register_print_cb(count_qrcode_logs_cb);

    /*The intended deferred flow: change the properties, then encode once explicitly.
     *lv_qrcode_render() takes no payload and still reports the result.*/
    lv_qrcode_set_update_mode(qr, LV_QRCODE_UPDATE_MODE_DEFERRED);
    lv_qrcode_set_quiet_zone(qr, true);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_render(qr));
    lv_obj_center(qr);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qrcode_2.png");
    lv_log_register_print_cb(NULL);

    /*Nothing was left for the redraw to do, so it must not warn*/
#if LV_USE_LOG
    TEST_ASSERT_EQUAL(0, redraw_warning_cnt);
#endif
}

void test_qrcode_update_before_switching_mode_reports_result(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);

    lv_qrcode_set_update_mode(qr, LV_QRCODE_UPDATE_MODE_DEFERRED);
    lv_qrcode_set_data(qr, "https://lvgl.io");
    lv_qrcode_set_quiet_zone(qr, true);

    /*The order that can report a failure: render() encodes the deferred change and
     *returns the result, so switching the mode afterwards has nothing left to encode
     *and does not need to warn about a result it could not report.*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_render(qr));
    lv_qrcode_set_update_mode(qr, LV_QRCODE_UPDATE_MODE_IMMEDIATE);
    lv_obj_center(qr);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qrcode_2.png");
}

void test_qrcode_update_mode_immediate_applies_pending_change(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);

    lv_qrcode_set_update_mode(qr, LV_QRCODE_UPDATE_MODE_DEFERRED);
    lv_qrcode_set_data(qr, "https://lvgl.io");
    lv_qrcode_set_quiet_zone(qr, true);

    /*Switching back to immediate while out of date must still encode the deferred
     *change (it only warns that it cannot report the result). The draw hook asserts
     *that a bitmap is only ever out of date in deferred mode, so if this did not
     *re-encode, the redraw below would abort the test.*/
    lv_qrcode_set_update_mode(qr, LV_QRCODE_UPDATE_MODE_IMMEDIATE);
    lv_obj_center(qr);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qrcode_2.png");
}

void test_qrcode_deferred_render_failure_is_detectable(void)
{
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);

    /*Nothing encoded yet*/
    TEST_ASSERT_FALSE(lv_qrcode_is_render_valid(qr));

    lv_qrcode_set_size(qr, 150);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr, "https://lvgl.io", 15));
    TEST_ASSERT_TRUE(lv_qrcode_is_render_valid(qr));

    /*In deferred mode the re-encode happens in the draw pass, so nothing returns its
     *result to the caller. Shrinking the canvas below one pixel per QR module makes it
     *fail, and that failure is only observable through this flag.*/
    lv_qrcode_set_update_mode(qr, LV_QRCODE_UPDATE_MODE_DEFERRED);
    lv_qrcode_set_size(qr, 10);
    lv_obj_center(qr);
    lv_refr_now(NULL);
    TEST_ASSERT_FALSE(lv_qrcode_is_render_valid(qr));

    /*Growing it back re-encodes successfully*/
    lv_qrcode_set_size(qr, 150);
    lv_refr_now(NULL);
    TEST_ASSERT_TRUE(lv_qrcode_is_render_valid(qr));
}

void test_qrcode_failed_render_is_not_retried_every_frame(void)
{
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_size(qr, 150);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr, "https://lvgl.io", 15));
    lv_obj_center(qr);
    lv_refr_now(NULL);

    lv_qrcode_set_update_mode(qr, LV_QRCODE_UPDATE_MODE_DEFERRED);
    lv_log_register_print_cb(count_qrcode_logs_cb);

    /*Shrinking the canvas makes the deferred re-encode fail in the draw pass, which is the
     *one place a failure is worth logging because no caller can see it.*/
    lv_qrcode_set_size(qr, 10);

    /*The draw pass must attempt it once and then leave it alone: a state that cannot be
     *encoded only changes when a property changes, so no per-frame retry and no per-frame
     *log, however many times the object is redrawn.*/
    for(int i = 0; i < 5; i++) {
        lv_obj_invalidate(qr);
        lv_refr_now(NULL);
    }

    lv_log_register_print_cb(NULL);

    TEST_ASSERT_FALSE(lv_qrcode_is_render_valid(qr));
    TEST_ASSERT_TRUE(((lv_qrcode_t *)qr)->needs_update);
#if LV_USE_LOG
    TEST_ASSERT_EQUAL(1, encode_error_cnt);
    TEST_ASSERT_EQUAL(1, redraw_warning_cnt);
#endif

    /*A property change is what allows another attempt - proven by one that can succeed*/
    lv_qrcode_set_size(qr, 150);
    lv_refr_now(NULL);
    TEST_ASSERT_TRUE(lv_qrcode_is_render_valid(qr));
    TEST_ASSERT_FALSE(((lv_qrcode_t *)qr)->needs_update);
}


void test_qrcode_failures_are_silent_when_the_caller_sees_the_result(void)
{
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_size(qr, 150);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr, "https://lvgl.io", 15));

    lv_log_register_print_cb(count_qrcode_logs_cb);

    /*Immediate mode: every size change is a fresh encode attempt and all of these are too
     *small for the payload. Something like a size animation would hit this on every frame,
     *and none of it may reach the log - the state is observable through the API instead.*/
    for(int32_t size = 10; size < 20; size++) lv_qrcode_set_size(qr, size);

    /*An explicit render reports the failure through its return value, so it stays silent too*/
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_render(qr));

    lv_log_register_print_cb(NULL);

#if LV_USE_LOG
    TEST_ASSERT_EQUAL(0, encode_error_cnt);
#endif
    TEST_ASSERT_FALSE(lv_qrcode_is_render_valid(qr));

    /*A failed encode must not claim the bitmap is up to date*/
    TEST_ASSERT_TRUE(((lv_qrcode_t *)qr)->needs_update);

    /*A size that fits clears both*/
    lv_qrcode_set_size(qr, 150);
    TEST_ASSERT_TRUE(lv_qrcode_is_render_valid(qr));
    TEST_ASSERT_FALSE(((lv_qrcode_t *)qr)->needs_update);
}


void test_qrcode_update_reports_unencodable_payload(void)
{
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);
    lv_qrcode_set_size(qr, 150);

    /*Short enough to be stored, but too long to be encoded even by the largest QR
     *code version (byte mode at ECC MEDIUM tops out at ~2331 bytes)*/
    static char too_long[3000];
    lv_memset(too_long, 'a', sizeof(too_long) - 1);
    too_long[sizeof(too_long) - 1] = '\0';
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_update(qr, too_long, sizeof(too_long) - 1));

    TEST_ASSERT_FALSE(lv_qrcode_is_render_valid(qr));

    /*A payload that does fit is reported as success*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr, "https://lvgl.io", 15));
    TEST_ASSERT_TRUE(lv_qrcode_is_render_valid(qr));
}

void test_qrcode_canvas_too_small_is_reported(void)
{
    lv_obj_t * qr = lv_qrcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(qr);

    /*"https://lvgl.io" needs a version 2 code, i.e. 25 modules. A 10 px canvas cannot
     *hold even one pixel per module, so the bitmap would stay blank. That must be
     *reported as a failure rather than as a successfully rendered QR code.*/
    lv_qrcode_set_size(qr, 10);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_update(qr, "https://lvgl.io", 15));

    /*The same holds with a quiet zone, which needs even more room*/
    lv_qrcode_set_quiet_zone(qr, true);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_qrcode_update(qr, "https://lvgl.io", 15));

    /*A canvas large enough for both renders and reports success*/
    lv_qrcode_set_size(qr, 150);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr, "https://lvgl.io", 15));
    lv_qrcode_set_quiet_zone(qr, false);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_qrcode_update(qr, "https://lvgl.io", 15));
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
