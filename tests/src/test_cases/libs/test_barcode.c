#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#if LV_USE_BARCODE

#include <string.h>

static lv_obj_t * active_screen = NULL;
static uint32_t redraw_warning_cnt;
static uint32_t encode_cnt;

static void count_barcode_logs_cb(lv_log_level_t level, const char * buf)
{
    LV_UNUSED(level);
    /*Emitted by the draw hook when a deferred change was not applied explicitly*/
    if(strstr(buf, "was not called after the property changes") != NULL) redraw_warning_cnt++;
    /*Emitted by barcode_encode(), i.e. once per pass over the code128 encoder*/
    if(strstr(buf, "barcode width = ") != NULL) encode_cnt++;
}

void setUp(void)
{
    active_screen = lv_screen_active();
    redraw_warning_cnt = 0;
    encode_cnt = 0;
}

void tearDown(void)
{
    lv_log_register_print_cb(NULL);
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
    res = lv_barcode_set_text(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");

    /* Test vertical mode */
    lv_barcode_set_direction(barcode, LV_DIR_VER);
    lv_obj_set_size(barcode, 50, LV_SIZE_CONTENT);
    res = lv_barcode_set_text(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_2.png");

    /* Test tiled + horizontal mode */
    lv_barcode_set_tiled(barcode, true);
    lv_barcode_set_direction(barcode, LV_DIR_HOR);
    lv_obj_set_size(barcode, LV_SIZE_CONTENT, 50);

    res = lv_barcode_set_text(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_tiled_1.png");

    /* Test tiled + vertical mode */
    lv_barcode_set_direction(barcode, LV_DIR_VER);
    lv_obj_set_size(barcode, 50, LV_SIZE_CONTENT);
    res = lv_barcode_set_text(barcode, "https://lvgl.io");
    TEST_ASSERT_EQUAL(res, LV_RESULT_OK);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_tiled_2.png");
}

void test_barcode_properties_after_text(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);

    /*Needed before anything can be generated - see test_barcode_text_before_size_recovers()*/
    lv_obj_set_height(barcode, 50);

    /*Properties set after the data used to be dropped. The result has to be the same
     *bitmap test_barcode_normal() gets by setting them first.*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "https://lvgl.io"));

    lv_barcode_set_dark_color(barcode, lv_color_black());
    lv_barcode_set_light_color(barcode, lv_color_white());
    lv_barcode_set_scale(barcode, 2);
    lv_barcode_set_direction(barcode, LV_DIR_HOR);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");

    /*Direction and object size*/
    lv_barcode_set_direction(barcode, LV_DIR_VER);
    lv_obj_set_size(barcode, 50, LV_SIZE_CONTENT);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_2.png");

    /*Tiled mode*/
    lv_barcode_set_tiled(barcode, true);
    lv_barcode_set_direction(barcode, LV_DIR_HOR);
    lv_obj_set_size(barcode, LV_SIZE_CONTENT, 50);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_tiled_1.png");

    lv_barcode_set_direction(barcode, LV_DIR_VER);
    lv_obj_set_size(barcode, 50, LV_SIZE_CONTENT);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_tiled_2.png");
}

void test_barcode_text_before_size_recovers(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_barcode_set_scale(barcode, 2);

    /*A barcode sizes itself to its content, so with no height there is no canvas to
     *allocate. This could not succeed before the change either.*/
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_barcode_set_text(barcode, "https://lvgl.io"));
    TEST_ASSERT_TRUE(lv_barcode_get_render_failed(barcode));

    /*New: the data is remembered, so it appears once there is a height to fit it to*/
    lv_barcode_set_dark_color(barcode, lv_color_black());
    lv_barcode_set_light_color(barcode, lv_color_white());
    lv_obj_set_height(barcode, 50);
    lv_obj_update_layout(barcode);
    TEST_ASSERT_FALSE(lv_barcode_get_render_failed(barcode));
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");
}

void test_barcode_encoding_after_text(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, 50);
    lv_barcode_set_scale(barcode, 2);

    /*GS1 strips spaces, raw encodes them, so the bar count differs*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "LVGL 10"));
    TEST_ASSERT_EQUAL(LV_BARCODE_ENCODING_CODE128_GS1, lv_barcode_get_encoding(barcode));
    lv_obj_update_layout(barcode);
    const int32_t gs1_w = lv_obj_get_width(barcode);

    lv_barcode_set_encoding(barcode, LV_BARCODE_ENCODING_CODE128_RAW);
    TEST_ASSERT_EQUAL(LV_BARCODE_ENCODING_CODE128_RAW, lv_barcode_get_encoding(barcode));
    lv_obj_update_layout(barcode);
    TEST_ASSERT_NOT_EQUAL(gs1_w, lv_obj_get_width(barcode));
    TEST_ASSERT_FALSE(lv_barcode_get_render_failed(barcode));

    /*Switching back returns to the original geometry*/
    lv_barcode_set_encoding(barcode, LV_BARCODE_ENCODING_CODE128_GS1);
    lv_obj_update_layout(barcode);
    TEST_ASSERT_EQUAL(gs1_w, lv_obj_get_width(barcode));
}

void test_barcode_resize_regenerates(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, 50);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "https://lvgl.io"));

    /*Used to keep the height it had when the data was set*/
    const lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(barcode);
    TEST_ASSERT_NOT_NULL(draw_buf);
    TEST_ASSERT_EQUAL(50, draw_buf->header.h);

    lv_obj_set_height(barcode, 80);
    lv_obj_update_layout(barcode);

    draw_buf = lv_canvas_get_draw_buf(barcode);
    TEST_ASSERT_NOT_NULL(draw_buf);
    TEST_ASSERT_EQUAL(80, draw_buf->header.h);
    TEST_ASSERT_FALSE(lv_barcode_get_render_failed(barcode));
}

void test_barcode_scale_resizes_the_canvas(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, 50);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "https://lvgl.io"));

    const int32_t w1 = (int32_t)lv_canvas_get_draw_buf(barcode)->header.w;

    /*Scale is the pixel width of one bar*/
    lv_barcode_set_scale(barcode, 2);
    TEST_ASSERT_EQUAL(2, lv_barcode_get_scale(barcode));
    TEST_ASSERT_EQUAL(w1 * 2, (int32_t)lv_canvas_get_draw_buf(barcode)->header.w);
    TEST_ASSERT_FALSE(lv_barcode_get_render_failed(barcode));
}

void test_barcode_update_mode_default_is_immediate(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);

    /*Immediate keeps existing code's behaviour*/
    TEST_ASSERT_EQUAL(LV_BARCODE_UPDATE_MODE_IMMEDIATE, lv_barcode_get_update_mode(barcode));

    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_DEFERRED);
    TEST_ASSERT_EQUAL(LV_BARCODE_UPDATE_MODE_DEFERRED, lv_barcode_get_update_mode(barcode));

    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_IMMEDIATE);
    TEST_ASSERT_EQUAL(LV_BARCODE_UPDATE_MODE_IMMEDIATE, lv_barcode_get_update_mode(barcode));
}

void test_barcode_update_mode_deferred_fills_on_redraw(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, 50);
    lv_barcode_set_dark_color(barcode, lv_color_black());
    lv_barcode_set_light_color(barcode, lv_color_white());

    /*Deliberately omit the explicit lv_barcode_render() to cover the fallback: the redraw
     *warns and fills the bars in anyway, so the bitmap must still be correct.*/
    lv_log_register_print_cb(count_barcode_logs_cb);
    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_DEFERRED);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "https://lvgl.io"));
    lv_barcode_set_scale(barcode, 2);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");
    lv_log_register_print_cb(NULL);

#if LV_USE_LOG
    TEST_ASSERT_EQUAL(1, redraw_warning_cnt);
#endif
}

void test_barcode_deferred_text_is_not_filled_until_render(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, 50);
    lv_barcode_set_dark_color(barcode, lv_color_black());
    lv_barcode_set_light_color(barcode, lv_color_white());
    lv_barcode_set_scale(barcode, 2);

    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_DEFERRED);

    /*Setting the data obeys the mode too: the canvas is sized, so this reports OK, but the
     *bars are still pending*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "https://lvgl.io"));
    TEST_ASSERT_TRUE(((lv_barcode_t *)barcode)->needs_update);
    TEST_ASSERT_FALSE(lv_barcode_get_render_failed(barcode));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_render(barcode));
    TEST_ASSERT_FALSE(((lv_barcode_t *)barcode)->needs_update);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");
}

void test_barcode_render_applies_deferred_changes(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, 50);
    lv_barcode_set_dark_color(barcode, lv_color_black());
    lv_barcode_set_light_color(barcode, lv_color_white());
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "https://lvgl.io"));

    lv_log_register_print_cb(count_barcode_logs_cb);

    /*The intended flow: change the properties, then generate once explicitly*/
    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_DEFERRED);
    lv_barcode_set_scale(barcode, 2);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_render(barcode));

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");
    lv_log_register_print_cb(NULL);

    /*Nothing left for the redraw to do*/
#if LV_USE_LOG
    TEST_ASSERT_EQUAL(0, redraw_warning_cnt);
#endif
}

void test_barcode_render_before_switching_mode_reports_result(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, 50);
    lv_barcode_set_dark_color(barcode, lv_color_black());
    lv_barcode_set_light_color(barcode, lv_color_white());

    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_DEFERRED);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "https://lvgl.io"));
    lv_barcode_set_scale(barcode, 2);

    /*render() reports the result, so the mode switch afterwards has nothing left to do*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_render(barcode));
    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_IMMEDIATE);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");
}

void test_barcode_update_mode_immediate_applies_pending_change(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, 50);
    lv_barcode_set_dark_color(barcode, lv_color_black());
    lv_barcode_set_light_color(barcode, lv_color_white());

    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_DEFERRED);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "https://lvgl.io"));
    lv_barcode_set_scale(barcode, 2);

    /*Must still apply the deferred change. The draw hook asserts a bitmap is only out of
     *date in deferred mode, so if it did not, the redraw below would abort the test.*/
    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_IMMEDIATE);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");
}

void test_barcode_failed_render_is_detectable(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);

    /*Nothing generated yet*/
    TEST_ASSERT_TRUE(lv_barcode_get_render_failed(barcode));

    lv_obj_set_height(barcode, 50);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "https://lvgl.io"));
    TEST_ASSERT_FALSE(lv_barcode_get_render_failed(barcode));

    /*Zero height leaves no canvas to allocate, and the resize handler returns void, so
     *this flag is the only way to notice*/
    lv_obj_set_height(barcode, 0);
    lv_obj_update_layout(barcode);
    TEST_ASSERT_TRUE(lv_barcode_get_render_failed(barcode));

    /*Growing it back succeeds*/
    lv_obj_set_height(barcode, 50);
    lv_obj_update_layout(barcode);
    TEST_ASSERT_FALSE(lv_barcode_get_render_failed(barcode));
}

void test_barcode_failed_render_is_not_retried_every_frame(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, 50);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "https://lvgl.io"));
    lv_refr_now(NULL);

    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_DEFERRED);

    /*A known-bad state must not be retried on every redraw, so the draw hook stays quiet*/
    lv_obj_set_height(barcode, 0);
    lv_obj_update_layout(barcode);
    TEST_ASSERT_TRUE(lv_barcode_get_render_failed(barcode));
    TEST_ASSERT_TRUE(((lv_barcode_t *)barcode)->needs_update);

    lv_log_register_print_cb(count_barcode_logs_cb);
    for(int i = 0; i < 5; i++) {
        lv_obj_invalidate(barcode);
        lv_refr_now(NULL);
    }
    lv_log_register_print_cb(NULL);

    TEST_ASSERT_TRUE(lv_barcode_get_render_failed(barcode));
#if LV_USE_LOG
    TEST_ASSERT_EQUAL(0, redraw_warning_cnt);
#endif

    /*A property change is what allows another attempt - proven by one that can succeed*/
    lv_obj_set_height(barcode, 50);
    lv_obj_update_layout(barcode);
    lv_refr_now(NULL);
    TEST_ASSERT_FALSE(lv_barcode_get_render_failed(barcode));
    TEST_ASSERT_FALSE(((lv_barcode_t *)barcode)->needs_update);
}

#if LV_USE_LOG && LV_LOG_LEVEL <= LV_LOG_LEVEL_INFO

void test_barcode_regeneration_encodes_the_text_once(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, 50);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "https://lvgl.io"));
    lv_refr_now(NULL);

    /*The pattern is handed from the sizing pass to the fill, so a regeneration is a single
     *pass over the code128 encoder rather than one per stage*/
    lv_log_register_print_cb(count_barcode_logs_cb);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_render(barcode));
    lv_log_register_print_cb(NULL);
    TEST_ASSERT_EQUAL(1, encode_cnt);

    /*A scale change is fitted from the cached bar count - one encode, for the fill*/
    encode_cnt = 0;
    lv_log_register_print_cb(count_barcode_logs_cb);
    lv_barcode_set_scale(barcode, 2);
    lv_log_register_print_cb(NULL);
    TEST_ASSERT_EQUAL(1, encode_cnt);

    /*Deferred mode collapses several property changes into a single encode*/
    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_DEFERRED);
    encode_cnt = 0;
    lv_log_register_print_cb(count_barcode_logs_cb);
    lv_barcode_set_scale(barcode, 3);
    lv_barcode_set_scale(barcode, 4);
    lv_barcode_set_direction(barcode, LV_DIR_VER);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_render(barcode));
    lv_log_register_print_cb(NULL);
    TEST_ASSERT_EQUAL(1, encode_cnt);
}

#endif /*LV_USE_LOG && LV_LOG_LEVEL <= LV_LOG_LEVEL_INFO*/

void test_barcode_text_round_trip(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_set_height(barcode, 50);

    TEST_ASSERT_NULL(lv_barcode_get_text(barcode));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "https://lvgl.io"));
    TEST_ASSERT_EQUAL_STRING("https://lvgl.io", lv_barcode_get_text(barcode));

    /*The copy is reallocated in place, so a shorter text must not leave the old tail*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "LVGL"));
    TEST_ASSERT_EQUAL_STRING("LVGL", lv_barcode_get_text(barcode));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "LVGL 10 barcode widget"));
    TEST_ASSERT_EQUAL_STRING("LVGL 10 barcode widget", lv_barcode_get_text(barcode));

    lv_barcode_set_scale(barcode, 2);
    TEST_ASSERT_EQUAL_STRING("LVGL 10 barcode widget", lv_barcode_get_text(barcode));
    TEST_ASSERT_FALSE(lv_barcode_get_render_failed(barcode));
}

void test_barcode_set_text_rejects_invalid_text(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_set_height(barcode, 50);

#if LV_USE_CHECK_ARG
    /*Without the argument check the text is dereferenced, so only assert this when it is on*/
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_barcode_set_text(barcode, NULL));
    TEST_ASSERT_TRUE(lv_barcode_get_render_failed(barcode));
#endif

    /*Nothing to encode*/
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_barcode_set_text(barcode, ""));
    TEST_ASSERT_TRUE(lv_barcode_get_render_failed(barcode));
    TEST_ASSERT_NULL(lv_barcode_get_text(barcode));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_text(barcode, "https://lvgl.io"));
    TEST_ASSERT_FALSE(lv_barcode_get_render_failed(barcode));

    /*Emptying forgets the data, so a property change cannot bring the old barcode back*/
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_barcode_set_text(barcode, ""));
    TEST_ASSERT_TRUE(lv_barcode_get_render_failed(barcode));
    lv_barcode_set_scale(barcode, 2);
    TEST_ASSERT_TRUE(lv_barcode_get_render_failed(barcode));
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
