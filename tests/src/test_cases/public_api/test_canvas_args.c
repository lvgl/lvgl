#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "lv_test_init.h"

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

static void canvas_draw_buf_reshape(lv_draw_buf_t * draw_buf)
{
#if LV_USE_DRAW_VG_LITE
    /* VG-Lite requires automatic stride calculation */
    lv_draw_buf_t * buf = lv_draw_buf_reshape(draw_buf,
                                              draw_buf->header.cf,
                                              draw_buf->header.w,
                                              draw_buf->header.h,
                                              LV_STRIDE_AUTO);
    TEST_ASSERT(buf == draw_buf);
#else
    LV_UNUSED(draw_buf);
#endif
}

/*A draw buffer that was never initialised has no handlers to draw with*/
void test_canvas_args_uninitialized_draw_buf(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);

    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 100, 100, LV_COLOR_FORMAT_DEFAULT);

    lv_canvas_set_draw_buf(canvas, &draw_buf);
    TEST_ASSERT_NULL(lv_canvas_get_draw_buf(canvas));
    TEST_ASSERT_NULL(lv_canvas_get_image(canvas));
    TEST_ASSERT_NULL(lv_canvas_get_buf(canvas));
}

void test_canvas_empty_draw_buf(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);

    TEST_ASSERT_NULL(lv_canvas_get_draw_buf(canvas));
    TEST_ASSERT_NULL(lv_canvas_get_image(canvas));
    TEST_ASSERT_NULL(lv_canvas_get_buf(canvas));
    lv_layer_t layer;

    LV_DRAW_BUF_DEFINE_STATIC(src_buf, 10, 10, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(src_buf);
    canvas_draw_buf_reshape(&src_buf);

    lv_canvas_copy_buf(canvas, NULL, &src_buf, NULL);
    lv_canvas_fill_bg(canvas, lv_color_hex(0xFFFFFF), LV_OPA_COVER);
    lv_canvas_init_layer(canvas, &layer);
    lv_canvas_set_px(canvas, 0, 0, lv_color_hex(0x000000), LV_OPA_COVER);

    lv_color32_t src_px = lv_color_to_32(lv_color_hex(0x000000), LV_OPA_0);
    lv_color32_t dst_px = lv_canvas_get_px(canvas, 0, 0);
    TEST_ASSERT_TRUE(lv_color32_eq(src_px, dst_px));

    lv_canvas_set_palette(canvas, 0, src_px);
    lv_canvas_finish_layer(canvas, &layer);
}

void test_canvas_out_of_area(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);

    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 10, 10, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);
    canvas_draw_buf_reshape(&draw_buf);
    lv_canvas_set_draw_buf(canvas, &draw_buf);

    lv_color_t test_color = lv_color_hex(0x1234);

    lv_canvas_set_px(canvas, -1, -1, test_color, LV_OPA_0);
    lv_color32_t px = lv_canvas_get_px(canvas, -1, -1);
    TEST_ASSERT_EQUAL_UINT8(0x00, px.red);
    TEST_ASSERT_EQUAL_UINT8(0x00, px.green);
    TEST_ASSERT_EQUAL_UINT8(0x00, px.blue);
    TEST_ASSERT_EQUAL_UINT8(0x00, px.alpha);
}

#endif /*LV_BUILD_TEST*/
