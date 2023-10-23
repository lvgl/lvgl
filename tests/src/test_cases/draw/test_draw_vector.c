#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
    lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_scr_act(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);
}

void tearDown(void)
{
    /* Function run after every test */
}


static void draw_vector(lv_layer_t * layer)
{
    lv_vector_dsc_t * ctx = lv_vector_dsc_create(layer);

    lv_area_t rect = {0, 100, 300, 300};
    lv_vector_clear(ctx, &rect, lv_color_white());
    lv_vector_draw(ctx);

    lv_vector_dsc_destroy(ctx);
}

void test_draw_vector(void)
{
    static uint8_t canvas_buf[760 * 440 * 4];

    lv_obj_t * canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, canvas_buf, 760, 440, LV_COLOR_FORMAT_ARGB8888);

    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_RED, 2), LV_OPA_50);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    draw_vector(&layer);

    lv_canvas_finish_layer(canvas, &layer);

    char fn_buf[64];
    lv_snprintf(fn_buf, sizeof(fn_buf), "draw/vector_%s.png", fn);
    TEST_ASSERT_EQUAL_SCREENSHOT(fn_buf);

    lv_obj_del(canvas);
}

#endif
