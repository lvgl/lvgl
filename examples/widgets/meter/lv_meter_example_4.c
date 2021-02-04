#include "../../../lvgl.h"
#include <stdio.h>
#if LV_USE_METER

/**
 * Create a pie chart
 */
void lv_ex_meter_4(void)
{
    lv_obj_t * meter = lv_meter_create(lv_scr_act(), NULL);
    lv_obj_align(meter, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Remove the background and the circle from the middle*/
    lv_obj_remove_style(meter, LV_PART_MAIN, LV_STATE_ANY, NULL);
    lv_obj_remove_style(meter, LV_PART_INDICATOR, LV_STATE_ANY, NULL);

    /*Add a scale first with no ticks.*/
    lv_meter_scale_t * scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale, 0, 0, 0, LV_COLOR_BLACK);
    lv_meter_set_scale_range(meter, scale, 0, 100, 360, 0);

    /*Add a three arc indicator */
    lv_coord_t indic_w = lv_obj_get_width(meter) / 2;
    lv_meter_indicator_t * indic1 = lv_meter_add_arc(meter, scale, indic_w, LV_COLOR_ORANGE, 0);
    lv_meter_set_indicator_start_value(meter, indic1, 0);
    lv_meter_set_indicator_end_value(meter, indic1, 40);

    lv_meter_indicator_t * indic2 = lv_meter_add_arc(meter, scale, indic_w, LV_COLOR_GREEN, 0);
    lv_meter_set_indicator_start_value(meter, indic2, 40);  /*Start from the previous*/
    lv_meter_set_indicator_end_value(meter, indic2, 80);

    lv_meter_indicator_t * indic3 = lv_meter_add_arc(meter, scale, indic_w, LV_COLOR_BLUE, 0);
    lv_meter_set_indicator_start_value(meter, indic3, 80);  /*Start from the previous*/
    lv_meter_set_indicator_end_value(meter, indic3, 100);
}

#endif
