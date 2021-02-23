#include "../../../lvgl.h"
#if LV_USE_METER && LV_BUILD_EXAMPLES

static lv_obj_t * meter;

static void set_value(void * indic, int32_t v)
{
    lv_meter_set_indicator_value(meter, indic, v);
}

/**
 * A simple meter
 */
void lv_example_meter_1(void)
{
     meter = lv_meter_create(lv_scr_act(), NULL);
    lv_obj_align(meter, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Add a scale first*/
    lv_meter_scale_t * scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale, 51, 2, 10, lv_color_grey());
    lv_meter_set_scale_major_ticks(meter, scale, 10, 4, 15, lv_color_black(), 10);

    lv_meter_indicator_t * indic;

    /*Add a red arc to the end */
    indic = lv_meter_add_arc(meter, scale, 3, lv_color_red(), 1);
    lv_meter_set_indicator_start_value(meter, indic, 80);
    lv_meter_set_indicator_end_value(meter, indic, 100);

    /*Make the tick lines red at the end of the scale*/
    indic = lv_meter_add_scale_lines(meter, scale, lv_color_red(), lv_color_red(), false, 0);
    lv_meter_set_indicator_start_value(meter, indic, 80);
    lv_meter_set_indicator_end_value(meter, indic, 100);

    /*Add a needle line indicator*/
    indic = lv_meter_add_needle_line(meter, scale, 4, lv_color_grey(), -10);

    /*Create an animation to set the value*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_var(&a, indic);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

#endif
