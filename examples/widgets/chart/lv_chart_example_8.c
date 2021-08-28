#include "../../lv_examples.h"
#include <stdio.h>
#include <math.h>
#if LV_USE_CHART && LV_DRAW_COMPLEX && LV_BUILD_EXAMPLES

/*  A struct is used to keep track of the series list because later we need to draw to the series in the reverse order to which they were initialised. */
static struct {
    lv_obj_t * obj;
    lv_chart_series_t * series_list[3];
} stacked_area_chart;

static void draw_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);

    /*Add the faded area before the lines are drawn*/
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(dsc->part == LV_PART_ITEMS) {
        if(!dsc->p1 || !dsc->p2) return;

        /*Add a line mask that keeps the area below the line*/
        lv_draw_mask_line_param_t line_mask_param;
        lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
        int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);

        /*Draw a rectangle that will be affected by the mask*/
        lv_draw_rect_dsc_t draw_rect_dsc;
        lv_draw_rect_dsc_init(&draw_rect_dsc);
        draw_rect_dsc.bg_opa = LV_OPA_COVER;
        draw_rect_dsc.bg_color = dsc->line_dsc->color;

        lv_area_t a;
        a.x1 = dsc->p1->x;
        a.x2 = dsc->p2->x - 1;
        a.y1 = LV_MIN(dsc->p1->y, dsc->p2->y);
        a.y2 = obj->coords.y2 - 13; //-13 cuts off where the rectangle draws over the chart margin. Without this an area of 0 doesnt look like 0
        lv_draw_rect(&a, dsc->clip_area, &draw_rect_dsc);

        /*Remove the mask*/
        lv_draw_mask_free_param(&line_mask_param);
        lv_draw_mask_remove_id(line_mask_id);
    }
}

/**
 * Stacked area chart
 */
void lv_example_chart_8(void)
{
    /*Create a stacked_area_chart.obj*/
    stacked_area_chart.obj = lv_chart_create(lv_scr_act());
    lv_obj_set_size(stacked_area_chart.obj, 200, 150);
    lv_obj_center(stacked_area_chart.obj);
    lv_chart_set_type(stacked_area_chart.obj, LV_CHART_TYPE_LINE);
    lv_chart_set_div_line_count(stacked_area_chart.obj, 5, 7);
    lv_obj_add_event_cb(stacked_area_chart.obj, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

    /*Set point size to 0 so the lines are smooth */
    lv_obj_set_style_size(stacked_area_chart.obj, 0, LV_PART_INDICATOR);

    /*Add some data series*/
    stacked_area_chart.series_list[0] = lv_chart_add_series(stacked_area_chart.obj, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    stacked_area_chart.series_list[1] = lv_chart_add_series(stacked_area_chart.obj, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
    stacked_area_chart.series_list[2] = lv_chart_add_series(stacked_area_chart.obj, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);

    for(int i = 0; i < 10; i++) {
        /* Make some random data */
        int vals[3] = {lv_rand(10,20), lv_rand(20,30), lv_rand(20,30) - i};

        /*The percentages to display on the chart*/
        int total = vals[0] + vals[1] + vals[2];
        float heights[3];
        int draw_heights[3];
        int int_sum = 0;
        float float_sum = 0;

        /* Cascade rounding ensures percentages add to 100 */
        for (int i = 0; i < 3; i++)
        {
            heights[i] = ((double)vals[i] / total) * 100;
            float_sum += heights[i];
            int_sum += (int)heights[i];
            int modifier = (int)float_sum - int_sum;

            /*  The draw heights are equal to the percentage of the total each value is + the cumulative sum of the previous percentages.
                The accumulation is how the values get "stacked" */
            draw_heights[i] = int_sum + modifier;
        }

        /* Now finally draw the points*/
        for (int i = 0; i < 3; i++)
        {
            int val = draw_heights[i];
            /*  Draw to the series in the reverse order to which they were initialised.
                Without this the higher values will draw on top of the lower ones.
                This is because the Z-height of a series matches the order it was initialsied */
            lv_chart_set_next_value(stacked_area_chart.obj, stacked_area_chart.series_list[3 - i - 1], val);
        }
    }

    lv_chart_refresh(stacked_area_chart.obj);
}

#endif