#include "../../lv_examples.h"
#if LV_USE_METER && LV_BUILD_EXAMPLES

static lv_obj_t * meter;

static void set_value(void * indic, int32_t v)
{
    lv_meter_set_indicator_end_value(meter, indic, v);
}

static void tick_label_event(lv_event_t * e)
{
    lv_obj_draw_part_dsc_t * draw_part_dsc = lv_event_get_draw_part_dsc(e);

    /*Be sure it's drawing meter related parts*/
    if(draw_part_dsc->class_p != &lv_meter_class) return;

    /*Be sure it's drawing the ticks*/
    if(draw_part_dsc->type != LV_METER_DRAW_PART_TICK) return;

    /*Be sure it's a major ticks*/
    if(draw_part_dsc->id % 5) return;

    /*The order of numbers on the clock is tricky: 12, 1, 2, 3...*/
    if(draw_part_dsc->id == 0) {
        lv_strncpy(draw_part_dsc->text, "12", 4);
    }
    else {
        lv_snprintf(draw_part_dsc->text, 4, "%d", draw_part_dsc->id / 5);
    }
}

/**
 * A clock from a meter
 */
void lv_example_meter_3(void)
{
    meter = lv_meter_create(lv_scr_act());
    lv_obj_set_size(meter, 220, 220);
    lv_obj_center(meter);

    /*Create a scale for the minutes*/
    /*61 ticks in a 360 degrees range (the last and the first line overlaps)*/
    lv_meter_set_scale_ticks(meter, 60, 1, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter, 5, 2, 20, lv_color_black(), 10);
    lv_meter_set_scale_range(meter, 0, 59, 354, 270);

    LV_IMG_DECLARE(img_hand)

    /*Add a the hands from images*/
    lv_meter_indicator_t * indic_min = lv_meter_add_needle_img(meter, &img_hand, 5, 5);
    lv_meter_indicator_t * indic_hour = lv_meter_add_needle_img(meter, &img_hand, 5, 5);

    lv_obj_add_event_cb(meter, tick_label_event, LV_EVENT_DRAW_PART_BEGIN, NULL);


    /*Create an animation to set the value*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_values(&a, 0, 59);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_time(&a, 5000);     /*2 sec for 1 turn of the minute hand (1 hour)*/
    lv_anim_set_var(&a, indic_min);
    lv_anim_start(&a);

    lv_anim_set_var(&a, indic_hour);
    lv_anim_set_time(&a, 240000);    /*24 sec for 1 turn of the hour hand*/
    lv_anim_set_values(&a, 0, 59);
    lv_anim_start(&a);
}

#endif
