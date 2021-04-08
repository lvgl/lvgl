#include "../../lv_examples.h"
#if LV_USE_CHART && LV_DRAW_COMPLEX && LV_BUILD_EXAMPLES

static lv_obj_t * chart1;
static lv_chart_series_t * ser1;
static lv_chart_series_t * ser2;

static void event_cb(lv_obj_t * obj, lv_event_t e)
{
    /*Add the faded area before the lines are drawn*/
    if(e == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_dsc_t * dsc = lv_event_get_param();
        if(dsc->part != LV_PART_ITEMS) return;
        if(!dsc->p1 || !dsc->p2) return;

        /*Add  a line mask that keeps the area below the line*/
        lv_draw_mask_line_param_t line_mask_param;
        lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
        int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);

        /*Add a fade effect: transparent bottom covering top*/
        lv_coord_t h = lv_obj_get_height(obj);
        lv_draw_mask_fade_param_t fade_mask_param;
        lv_draw_mask_fade_init(&fade_mask_param, &obj->coords, LV_OPA_COVER, obj->coords.y1 + h / 8, LV_OPA_TRANSP,obj->coords.y2);
        int16_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);

        /*Draw a rectangle that will be affected by the mask*/
        lv_draw_rect_dsc_t draw_rect_dsc;
        lv_draw_rect_dsc_init(&draw_rect_dsc);
        draw_rect_dsc.bg_opa = LV_OPA_20;
        draw_rect_dsc.bg_color = dsc->line_dsc->color;

        lv_area_t a;
        a.x1 = dsc->p1->x;
        a.x2 = dsc->p2->x - 1;
        a.y1 = LV_MIN(dsc->p1->y, dsc->p2->y);
        a.y2 = obj->coords.y2;
        lv_draw_rect(&a, dsc->clip_area, &draw_rect_dsc);

        /*Remove the masks*/
        lv_draw_mask_remove_id(line_mask_id);
        lv_draw_mask_remove_id(fade_mask_id);
    }
}

static void add_data(lv_timer_t * timer)
{
    LV_UNUSED(timer);
    static uint32_t cnt = 0;
    lv_chart_set_next_value(chart1, ser1, lv_rand(20, 90));

    if(cnt % 4 == 0) lv_chart_set_next_value(chart1, ser2, lv_rand(40, 60));

    cnt++;
}

/**
 * Add a faded area effect to the line chart
 */
void lv_example_chart_2(void)
{
    /*Create a chart1*/
    chart1 = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart1, 200, 150);
    lv_obj_center(chart1);
    lv_chart_set_type(chart1, LV_CHART_TYPE_LINE);   /*Show lines and points too*/

    lv_obj_add_event_cb(chart1, event_cb, NULL);
    lv_chart_set_update_mode(chart1, LV_CHART_UPDATE_MODE_CIRCULAR);

    /*Add two data series*/
    ser1 = lv_chart_add_series(chart1, lv_color_red(), LV_CHART_AXIS_PRIMARY_Y);
    ser2 = lv_chart_add_series(chart1, lv_color_blue(), LV_CHART_AXIS_SECONDARY_Y);

    uint32_t i;
    for(i = 0; i < 10; i++) {
        lv_chart_set_next_value(chart1, ser1, lv_rand(20, 90));
        lv_chart_set_next_value(chart1, ser2, lv_rand(30, 70));
    }

    lv_timer_create(add_data, 200, NULL);
}

#endif
