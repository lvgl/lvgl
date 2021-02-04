#include "../../../lvgl.h"
#include <stdio.h>
#if LV_USE_METER

static lv_obj_t * meter;

static void set_value(lv_meter_indicator_t * indic, int32_t v)
{
    lv_meter_set_indicator_end_value(meter, indic, v);
}


/**
 * A meter with multiple arcs
 */
void lv_ex_meter_2(void)
{
    meter = lv_meter_create(lv_scr_act(), NULL);
    lv_obj_align(meter, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Remove the circle from the middle*/
    lv_obj_remove_style(meter, LV_PART_INDICATOR, LV_STATE_ANY, NULL);

    /*Add a scale first*/
    lv_meter_scale_t * scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale, 11, 2, 10, LV_COLOR_GRAY);
    lv_meter_set_scale_major_ticks(meter, scale, 1, 2, 30, lv_color_hex3(0xeee), 10);
    lv_meter_set_scale_range(meter, scale, 0, 100, 270, 90);

    /*Add a three arc indicator */
    lv_meter_indicator_t * indic1 = lv_meter_add_arc(meter, scale, 10, LV_COLOR_RED, 0);
    lv_meter_indicator_t * indic2 = lv_meter_add_arc(meter, scale, 10, LV_COLOR_GREEN, -10);
    lv_meter_indicator_t * indic3 = lv_meter_add_arc(meter, scale, 10, LV_COLOR_BLUE, -20);

    /*Create an animation to set the value*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) set_value);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    lv_anim_set_time(&a, 2000);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_var(&a, indic1);
    lv_anim_start(&a);

    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_set_var(&a, indic2);
    lv_anim_start(&a);

    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_var(&a, indic3);
    lv_anim_start(&a);
}

#endif
