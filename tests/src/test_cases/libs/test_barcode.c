#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#if LV_USE_BARCODE

#include <string.h>

#define BARCODE_DATA  "https://lvgl.io"
#define BARCODE_SIZE  460
#define BARCODE_SCALE 2
#define BARCODE_THICK 50

static lv_obj_t * active_screen = NULL;
static uint32_t redraw_warning_cnt;
static uint32_t encode_cnt;

static void count_barcode_logs_cb(lv_log_level_t level, const char * buf)
{
    LV_UNUSED(level);
    /*Emitted by the draw hook when a deferred change was not applied explicitly*/
    if(strstr(buf, "was not called after the property changes") != NULL) redraw_warning_cnt++;
    /*Emitted by barcode_encode_data(), i.e. once per pass over the code128 encoder*/
    if(strstr(buf, "barcode width = ") != NULL) encode_cnt++;
}

/*A horizontal barcode with the defaults the screenshots were taken with*/
static lv_obj_t * barcode_create_hor(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);

    lv_barcode_set_size(barcode, BARCODE_SIZE);
    lv_barcode_set_scale(barcode, BARCODE_SCALE);
    lv_obj_set_height(barcode, BARCODE_THICK);
    return barcode;
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
    lv_obj_t * barcode = barcode_create_hor();

    lv_color_t dark_color = lv_color_black();
    lv_color_t light_color = lv_color_white();

    lv_barcode_set_dark_color(barcode, dark_color);
    lv_barcode_set_light_color(barcode, light_color);

    TEST_ASSERT_EQUAL_COLOR(dark_color, lv_barcode_get_dark_color(barcode));
    TEST_ASSERT_EQUAL_COLOR(light_color, lv_barcode_get_light_color(barcode));
    TEST_ASSERT_EQUAL(BARCODE_SIZE, lv_barcode_get_size(barcode));
    TEST_ASSERT_EQUAL(BARCODE_SCALE, lv_barcode_get_scale(barcode));
    TEST_ASSERT_EQUAL(LV_DIR_HOR, lv_barcode_get_direction(barcode));

    /*Horizontal*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, BARCODE_DATA));
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");

    /*Vertical. The length along the bars is the same; the thickness moves to the width.*/
    lv_barcode_set_direction(barcode, LV_DIR_VER);
    lv_obj_set_size(barcode, BARCODE_THICK, LV_SIZE_CONTENT);
    TEST_ASSERT_EQUAL(LV_DIR_VER, lv_barcode_get_direction(barcode));
    TEST_ASSERT_TRUE(lv_barcode_is_render_valid(barcode));
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_2.png");
}

void test_barcode_properties_after_data(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);

    /*The data comes first here, the properties after. The result has to be the same
     *bitmap test_barcode_normal() gets by setting them the other way round.*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, BARCODE_DATA));

    lv_barcode_set_dark_color(barcode, lv_color_black());
    lv_barcode_set_light_color(barcode, lv_color_white());
    lv_barcode_set_size(barcode, BARCODE_SIZE);
    lv_barcode_set_scale(barcode, BARCODE_SCALE);
    lv_obj_set_height(barcode, BARCODE_THICK);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");

    lv_barcode_set_direction(barcode, LV_DIR_VER);
    lv_obj_set_size(barcode, BARCODE_THICK, LV_SIZE_CONTENT);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_2.png");
}

void test_barcode_default_geometry(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, BARCODE_DATA));
    lv_obj_update_layout(barcode);

    /*As long as the bars need, and thick enough to see without setting anything*/
    TEST_ASSERT_EQUAL(lv_barcode_get_size(barcode), lv_obj_get_width(barcode));
    TEST_ASSERT_EQUAL(LV_DPI_DEF, lv_obj_get_height(barcode));

    /*The thickness is a plain size, so the Widget's own setter wins*/
    lv_obj_set_height(barcode, BARCODE_THICK);
    lv_obj_update_layout(barcode);
    TEST_ASSERT_EQUAL(BARCODE_THICK, lv_obj_get_height(barcode));
}

void test_barcode_size_drives_the_canvas(void)
{
    lv_obj_t * barcode = barcode_create_hor();
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, BARCODE_DATA));

    /*The canvas is one pixel thick and as long as the size; the image tiling supplies the
     *rest of the thickness*/
    const lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(barcode);
    TEST_ASSERT_NOT_NULL(draw_buf);
    TEST_ASSERT_EQUAL(BARCODE_SIZE, draw_buf->header.w);
    TEST_ASSERT_EQUAL(1, draw_buf->header.h);

    /*The size is the content width, and the thickness is the Widget's own height*/
    lv_obj_update_layout(barcode);
    TEST_ASSERT_EQUAL(BARCODE_SIZE, lv_obj_get_width(barcode));
    TEST_ASSERT_EQUAL(BARCODE_THICK, lv_obj_get_height(barcode));

    /*A longer size is a longer canvas*/
    lv_barcode_set_size(barcode, BARCODE_SIZE + 100);
    draw_buf = lv_canvas_get_draw_buf(barcode);
    TEST_ASSERT_EQUAL(BARCODE_SIZE + 100, draw_buf->header.w);
    TEST_ASSERT_EQUAL(1, draw_buf->header.h);
    TEST_ASSERT_TRUE(lv_barcode_is_render_valid(barcode));

    /*A vertical barcode turns the canvas on its side*/
    lv_barcode_set_direction(barcode, LV_DIR_VER);
    draw_buf = lv_canvas_get_draw_buf(barcode);
    TEST_ASSERT_EQUAL(1, draw_buf->header.w);
    TEST_ASSERT_EQUAL(BARCODE_SIZE + 100, draw_buf->header.h);
    TEST_ASSERT_TRUE(lv_barcode_is_render_valid(barcode));
}

void test_barcode_object_size_does_not_touch_the_canvas(void)
{
    lv_obj_t * barcode = barcode_create_hor();
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, BARCODE_DATA));

    const lv_draw_buf_t * before = lv_canvas_get_draw_buf(barcode);
    TEST_ASSERT_NOT_NULL(before);

    /*The bitmap used to be regenerated on every resize, which is what made the Widget
     *depend on its own size. The thickness is the tiling's job now.*/
    lv_obj_set_height(barcode, BARCODE_THICK * 2);
    lv_obj_update_layout(barcode);

    TEST_ASSERT_EQUAL_PTR(before, lv_canvas_get_draw_buf(barcode));
    TEST_ASSERT_EQUAL(BARCODE_THICK * 2, lv_obj_get_height(barcode));
    TEST_ASSERT_TRUE(lv_barcode_is_render_valid(barcode));

    /*Even a zero height, which used to leave nothing to allocate, is harmless*/
    lv_obj_set_height(barcode, 0);
    lv_obj_update_layout(barcode);
    TEST_ASSERT_EQUAL_PTR(before, lv_canvas_get_draw_buf(barcode));
    TEST_ASSERT_TRUE(lv_barcode_is_render_valid(barcode));
}

void test_barcode_scale_zero_fits_the_bars_to_the_size(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, BARCODE_THICK);

    /*Zero is the default: the widest bar that fits*/
    TEST_ASSERT_EQUAL(0, lv_barcode_get_scale(barcode));

    /*Room for a bar twice as wide as the size the screenshots use one pixel bars at*/
    lv_barcode_set_size(barcode, BARCODE_SIZE * 2);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, BARCODE_DATA));
    TEST_ASSERT_TRUE(lv_barcode_is_render_valid(barcode));

    /*Too short for even one pixel per bar, so there is nothing to draw*/
    lv_barcode_set_size(barcode, 8);
    TEST_ASSERT_FALSE(lv_barcode_is_render_valid(barcode));

    /*And it recovers once there is room again*/
    lv_barcode_set_size(barcode, BARCODE_SIZE);
    TEST_ASSERT_TRUE(lv_barcode_is_render_valid(barcode));
}

void test_barcode_scale_that_does_not_fit_is_detectable(void)
{
    lv_obj_t * barcode = barcode_create_hor();
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, BARCODE_DATA));
    TEST_ASSERT_TRUE(lv_barcode_is_render_valid(barcode));

    /*A forced scale wins over the size, so it can ask for more room than there is. The
     *setter returns void, so this flag is the only way to notice.*/
    lv_barcode_set_scale(barcode, 100);
    TEST_ASSERT_FALSE(lv_barcode_is_render_valid(barcode));

    /*Back to a scale that fits*/
    lv_barcode_set_scale(barcode, BARCODE_SCALE);
    TEST_ASSERT_TRUE(lv_barcode_is_render_valid(barcode));
}

void test_barcode_encoding_after_data(void)
{
    lv_obj_t * barcode = barcode_create_hor();

    /*GS1 strips spaces, raw encodes them, so the two need a different scale to fit*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, "LVGL 10"));
    TEST_ASSERT_EQUAL(LV_BARCODE_ENCODING_CODE128_GS1, lv_barcode_get_encoding(barcode));

    lv_barcode_set_encoding(barcode, LV_BARCODE_ENCODING_CODE128_RAW);
    TEST_ASSERT_EQUAL(LV_BARCODE_ENCODING_CODE128_RAW, lv_barcode_get_encoding(barcode));
    TEST_ASSERT_TRUE(lv_barcode_is_render_valid(barcode));

    lv_barcode_set_encoding(barcode, LV_BARCODE_ENCODING_CODE128_GS1);
    TEST_ASSERT_EQUAL(LV_BARCODE_ENCODING_CODE128_GS1, lv_barcode_get_encoding(barcode));
    TEST_ASSERT_TRUE(lv_barcode_is_render_valid(barcode));
}

void test_barcode_get_data_returns_the_stored_copy(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);

    /*Nothing set yet*/
    TEST_ASSERT_NULL(lv_barcode_get_data(barcode));

    char data[] = BARCODE_DATA;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, data));

    /*A copy, not the caller's buffer*/
    TEST_ASSERT_NOT_EQUAL(data, lv_barcode_get_data(barcode));
    TEST_ASSERT_EQUAL_STRING(BARCODE_DATA, lv_barcode_get_data(barcode));

    /*Emptying forgets it*/
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_barcode_set_data(barcode, ""));
    TEST_ASSERT_NULL(lv_barcode_get_data(barcode));
}

void test_barcode_update_mode_default_is_immediate(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);

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
    lv_obj_set_height(barcode, BARCODE_THICK);
    lv_barcode_set_dark_color(barcode, lv_color_black());
    lv_barcode_set_light_color(barcode, lv_color_white());
    lv_barcode_set_size(barcode, BARCODE_SIZE);

    /*Deliberately omit the explicit lv_barcode_render() to cover the fallback: the redraw
     *warns and generates the bars anyway, so the bitmap must still be correct.*/
    lv_log_register_print_cb(count_barcode_logs_cb);
    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_DEFERRED);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, BARCODE_DATA));
    lv_barcode_set_scale(barcode, BARCODE_SCALE);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");
    lv_log_register_print_cb(NULL);

#if LV_USE_LOG
    TEST_ASSERT_EQUAL(1, redraw_warning_cnt);
#endif
}

void test_barcode_deferred_property_is_not_applied_until_render(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, BARCODE_THICK);
    lv_barcode_set_dark_color(barcode, lv_color_black());
    lv_barcode_set_light_color(barcode, lv_color_white());
    lv_barcode_set_size(barcode, BARCODE_SIZE);

    /*Setting the data always generates right away, in either mode, as the QR code does*/
    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_DEFERRED);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, BARCODE_DATA));
    TEST_ASSERT_FALSE(((lv_barcode_t *)barcode)->needs_update);

    /*A property change is what waits for the render*/
    lv_barcode_set_scale(barcode, BARCODE_SCALE);
    TEST_ASSERT_TRUE(((lv_barcode_t *)barcode)->needs_update);
    TEST_ASSERT_TRUE(lv_barcode_is_render_valid(barcode));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_render(barcode));
    TEST_ASSERT_FALSE(((lv_barcode_t *)barcode)->needs_update);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");
}

void test_barcode_render_applies_deferred_changes(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, BARCODE_THICK);
    lv_barcode_set_dark_color(barcode, lv_color_black());
    lv_barcode_set_light_color(barcode, lv_color_white());
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, BARCODE_DATA));

    lv_log_register_print_cb(count_barcode_logs_cb);

    /*The intended flow: change the properties, then generate once explicitly*/
    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_DEFERRED);
    lv_barcode_set_size(barcode, BARCODE_SIZE);
    lv_barcode_set_scale(barcode, BARCODE_SCALE);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_render(barcode));

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");
    lv_log_register_print_cb(NULL);

    /*Nothing left for the redraw to do*/
#if LV_USE_LOG
    TEST_ASSERT_EQUAL(0, redraw_warning_cnt);
#endif
}

void test_barcode_update_mode_immediate_applies_pending_change(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, BARCODE_THICK);
    lv_barcode_set_dark_color(barcode, lv_color_black());
    lv_barcode_set_light_color(barcode, lv_color_white());

    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_DEFERRED);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, BARCODE_DATA));
    lv_barcode_set_size(barcode, BARCODE_SIZE);
    lv_barcode_set_scale(barcode, BARCODE_SCALE);

    /*Must still apply the deferred change. The draw hook asserts a bitmap is only out of
     *date in deferred mode, so if it did not, the redraw below would abort the test.*/
    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_IMMEDIATE);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");
}

void test_barcode_failed_render_is_not_retried_every_frame(void)
{
    lv_obj_t * barcode = barcode_create_hor();
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, BARCODE_DATA));
    lv_refr_now(NULL);

    /*Fail while still in immediate mode, so the failure is recorded in the setter*/
    lv_barcode_set_scale(barcode, 100);
    TEST_ASSERT_FALSE(lv_barcode_is_render_valid(barcode));
    TEST_ASSERT_TRUE(((lv_barcode_t *)barcode)->needs_update);

    /*A known-bad state must not be retried on every redraw, so the draw hook stays quiet*/
    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_DEFERRED);

    lv_log_register_print_cb(count_barcode_logs_cb);
    for(int i = 0; i < 5; i++) {
        lv_obj_invalidate(barcode);
        lv_refr_now(NULL);
    }
    lv_log_register_print_cb(NULL);

    TEST_ASSERT_FALSE(lv_barcode_is_render_valid(barcode));
#if LV_USE_LOG
    TEST_ASSERT_EQUAL(0, redraw_warning_cnt);
#endif

    /*A property change is what allows another attempt - proven by one that can succeed*/
    lv_barcode_set_scale(barcode, BARCODE_SCALE);
    lv_refr_now(NULL);
    TEST_ASSERT_TRUE(lv_barcode_is_render_valid(barcode));
    TEST_ASSERT_FALSE(((lv_barcode_t *)barcode)->needs_update);
}

#if LV_USE_LOG && LV_LOG_LEVEL <= LV_LOG_LEVEL_INFO

void test_barcode_regeneration_encodes_the_data_once(void)
{
    lv_obj_t * barcode = barcode_create_hor();
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_set_data(barcode, BARCODE_DATA));
    lv_refr_now(NULL);

    /*A regeneration is a single pass over the code128 encoder*/
    lv_log_register_print_cb(count_barcode_logs_cb);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_render(barcode));
    lv_log_register_print_cb(NULL);
    TEST_ASSERT_EQUAL(1, encode_cnt);

    /*Deferred mode collapses several property changes into a single encode*/
    lv_barcode_set_update_mode(barcode, LV_BARCODE_UPDATE_MODE_DEFERRED);
    encode_cnt = 0;
    lv_log_register_print_cb(count_barcode_logs_cb);
    lv_barcode_set_scale(barcode, 3);
    lv_barcode_set_scale(barcode, 1);
    lv_barcode_set_direction(barcode, LV_DIR_VER);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_barcode_render(barcode));
    lv_log_register_print_cb(NULL);
    TEST_ASSERT_EQUAL(1, encode_cnt);
}

#endif /*LV_USE_LOG && LV_LOG_LEVEL <= LV_LOG_LEVEL_INFO*/

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
