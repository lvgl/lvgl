#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static lv_obj_t * g_screen_active;

void setUp(void)
{
    g_screen_active = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(g_screen_active);
}

static void draw_event_cb(lv_event_t * e)
{
    int * draw_counter = lv_event_get_user_data(e);
    (*draw_counter)++;
}
void test_canvas_functions_invalidate(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);
    int draw_counter = 0;
    lv_obj_add_event_cb(canvas, draw_event_cb, LV_EVENT_DRAW_MAIN, &draw_counter);
    lv_refr_now(NULL);
    TEST_ASSERT(draw_counter == 0);

    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 100, 100, LV_COLOR_FORMAT_NATIVE);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);

    lv_canvas_set_draw_buf(canvas, &draw_buf);
    lv_refr_now(NULL);
    TEST_ASSERT(draw_counter == 1);

    lv_canvas_set_px(canvas, 0, 0, lv_color_black(), LV_OPA_COVER);
    lv_refr_now(NULL);
    TEST_ASSERT(draw_counter == 2);

    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);
    lv_refr_now(NULL);
    TEST_ASSERT(draw_counter == 3);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.p1.x = 10;
    line_dsc.p1.y = 10;
    line_dsc.p2.x = 20;
    line_dsc.p2.y = 20;
    line_dsc.width = 5;
    lv_draw_line(&layer, &line_dsc);
    lv_canvas_finish_layer(canvas, &layer);
    lv_refr_now(NULL);
    TEST_ASSERT(draw_counter == 4);

    lv_refr_now(NULL);
    TEST_ASSERT(draw_counter == 4);
}

void test_canvas_fill_and_set_px(void)
{
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_obj_center(canvas);

    LV_DRAW_BUF_DEFINE_STATIC(buf_i1, 10, 10, LV_COLOR_FORMAT_I1);
    LV_DRAW_BUF_INIT_STATIC(buf_i1);
    lv_canvas_set_draw_buf(canvas, &buf_i1);
    lv_canvas_set_palette(canvas, 0, lv_color32_make(0x00, 0xff, 0x00, 0xff));
    lv_canvas_set_palette(canvas, 1, lv_color32_make(0x00, 0x00, 0xff, 0xff));
    lv_canvas_fill_bg(canvas, lv_color_hex(0), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(1), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");

    LV_DRAW_BUF_DEFINE_STATIC(buf_i2, 10, 10, LV_COLOR_FORMAT_I2);
    LV_DRAW_BUF_INIT_STATIC(buf_i2);
    lv_canvas_set_draw_buf(canvas, &buf_i2);
    lv_canvas_set_palette(canvas, 0, lv_color32_make(0x00, 0xff, 0x00, 0xff));
    lv_canvas_set_palette(canvas, 3, lv_color32_make(0x00, 0x00, 0xff, 0xff));
    lv_canvas_fill_bg(canvas, lv_color_hex(0), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(3), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");

    LV_DRAW_BUF_DEFINE_STATIC(buf_i4, 10, 10, LV_COLOR_FORMAT_I4);
    LV_DRAW_BUF_INIT_STATIC(buf_i4);
    lv_canvas_set_draw_buf(canvas, &buf_i4);
    lv_canvas_set_palette(canvas, 0, lv_color32_make(0x00, 0xff, 0x00, 0xff));
    lv_canvas_set_palette(canvas, 15, lv_color32_make(0x00, 0x00, 0xff, 0xff));
    lv_canvas_fill_bg(canvas, lv_color_hex(0), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(15), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");

    LV_DRAW_BUF_DEFINE_STATIC(buf_i8, 10, 10, LV_COLOR_FORMAT_I8);
    LV_DRAW_BUF_INIT_STATIC(buf_i8);
    lv_canvas_set_draw_buf(canvas, &buf_i8);
    lv_canvas_set_palette(canvas, 0, lv_color32_make(0x00, 0xff, 0x00, 0xff));
    lv_canvas_set_palette(canvas, 255, lv_color32_make(0x00, 0x00, 0xff, 0xff));
    lv_canvas_fill_bg(canvas, lv_color_hex(0), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(255), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");

    LV_DRAW_BUF_DEFINE_STATIC(buf_rgb888, 10, 10, LV_COLOR_FORMAT_RGB888);
    LV_DRAW_BUF_INIT_STATIC(buf_rgb888);
    lv_canvas_set_draw_buf(canvas, &buf_rgb888);
    lv_canvas_fill_bg(canvas, lv_color_hex(0x00ff00), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(0x0000ff), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");

    LV_DRAW_BUF_DEFINE_STATIC(buf_rgb565, 10, 10, LV_COLOR_FORMAT_RGB565);
    LV_DRAW_BUF_INIT_STATIC(buf_rgb565);
    lv_canvas_set_draw_buf(canvas, &buf_rgb565);
    lv_canvas_fill_bg(canvas, lv_color_hex(0x00ff00), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(0x0000ff), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");

    LV_DRAW_BUF_DEFINE_STATIC(buf_xrgb8888, 10, 10, LV_COLOR_FORMAT_XRGB8888);
    LV_DRAW_BUF_INIT_STATIC(buf_xrgb8888);
    lv_canvas_set_draw_buf(canvas, &buf_xrgb8888);
    lv_canvas_fill_bg(canvas, lv_color_hex(0x00ff00), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(0x0000ff), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");

    LV_DRAW_BUF_DEFINE_STATIC(buf_argb8888, 10, 10, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(buf_argb8888);
    lv_canvas_set_draw_buf(canvas, &buf_argb8888);
    lv_canvas_fill_bg(canvas, lv_color_hex(0x00ff00), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(0x0000ff), LV_OPA_COVER);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");
}

#endif
