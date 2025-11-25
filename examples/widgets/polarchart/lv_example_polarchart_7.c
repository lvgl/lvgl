#include "../../lv_examples.h"
#if LV_USE_POLARCHART && LV_BUILD_EXAMPLES

static void draw_event_cb(lv_event_t * e)
{
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
    if(base_dsc->part == LV_PART_INDICATOR) {
        lv_obj_t * obj = lv_event_get_target_obj(e);
        lv_polarchart_series_t * ser = lv_polarchart_get_series_next(obj, NULL);
        lv_draw_rect_dsc_t * rect_draw_dsc = (lv_draw_rect_dsc_t *)lv_draw_task_get_draw_dsc(draw_task);
        uint32_t cnt = lv_polarchart_get_point_count(obj);

        /*Make older value more transparent*/
        rect_draw_dsc->bg_opa = (lv_opa_t)((LV_OPA_COVER * base_dsc->id2) / (cnt - 1));

        /*Make smaller values blue, higher values red*/
        int32_t * angle_array = lv_polarchart_get_series_angle_array(obj, ser);
        int32_t * radial_array = lv_polarchart_get_series_radial_array(obj, ser);
        /*dsc->id is the tells drawing order, but we need the ID of the point being drawn.*/
        uint32_t start_point = lv_polarchart_get_x_start_point(obj, ser);
        uint32_t p_act = (start_point + base_dsc->id2) % cnt; /*Consider start point to get the index of the array*/
        lv_opa_t x_opa = (lv_opa_t)((angle_array[p_act] * LV_OPA_50) / 200);
        lv_opa_t y_opa = (lv_opa_t)((radial_array[p_act] * LV_OPA_50) / 1000);

        rect_draw_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_RED),
                                               lv_palette_main(LV_PALETTE_BLUE),
                                               x_opa + y_opa);
    }
}

static void add_data(lv_timer_t * timer)
{
    lv_obj_t * polarchart = (lv_obj_t *)lv_timer_get_user_data(timer);
    lv_polarchart_set_next_value2(polarchart, lv_polarchart_get_series_next(polarchart, NULL), (int32_t)lv_rand(0, 200),
                                  (int32_t)lv_rand(0, 1000));
}

/**
 * A scatter polarchart
 */
void lv_example_polarchart_7(void)
{
    lv_obj_t * polarchart = lv_polarchart_create(lv_screen_active());
    lv_obj_set_size(polarchart, 200, 150);
    lv_obj_align(polarchart, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(polarchart, draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(polarchart, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_obj_set_style_line_width(polarchart, 0, LV_PART_ITEMS);   /*Remove the lines*/

    lv_polarchart_set_type(polarchart, LV_CHART_TYPE_SCATTER);

    lv_polarchart_set_axis_range(polarchart, LV_CHART_AXIS_ANGLE, 0, 360);
    lv_polarchart_set_axis_range(polarchart, LV_CHART_AXIS_RADIAL, 0, 1000);

    lv_polarchart_set_point_count(polarchart, 50);

    lv_polarchart_series_t * ser = lv_polarchart_add_series(polarchart, lv_palette_main(LV_PALETTE_RED),
                                                            LV_CHART_AXIS_RADIAL);
    uint32_t i;
    for(i = 0; i < 50; i++) {
        lv_polarchart_set_next_value2(polarchart, ser, (int32_t)lv_rand(0, 200), (int32_t)lv_rand(0, 1000));
    }

    lv_timer_create(add_data, 100, polarchart);
}

#endif
