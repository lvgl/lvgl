#if LV_BUILD_TEST
#include "../lvgl.h"

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

    LV_DRAW_BUF_DEFINE(draw_buf, 100, 100, LV_COLOR_FORMAT_NATIVE);
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

#endif
