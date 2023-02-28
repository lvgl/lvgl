#include "../../lv_examples.h"
#if LV_USE_METER && LV_BUILD_EXAMPLES

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef enum {
    METER_SIMPLE,
    METER_LINEAR,
    METER_MULTI_ARCS,
    METER_MULTI_ARCS_LINEAR,
    METER_CLOCK,
    METER_PIE_CHART,
    METER_PIE_CHART_LINEAR,
} meter_t;

static struct meter_item {
    lv_obj_t * obj;
    lv_anim_t a;
    lv_point_t coord;
    uint16_t w;
    uint16_t h;
    uint8_t indic_cnt;
    uint8_t reverse : 1;
    uint8_t toggle_needle_side: 1;
    uint8_t hide_borders : 1;
    uint8_t meter_type: 3;
    char * name;
    lv_meter_indicator_t * indic[5];
} meter_items[] = {
    /*Horizontal meters*/
    {
        .coord.x = 0, .coord.y = 0, .w = 400, .h = 80, .reverse = 0,
        .hide_borders = 1, .meter_type = METER_LINEAR,
        .name = "Simple linear horizontal meter, reverse off, no borders"
    },
    {
        .coord.x = 0, .coord.y = 80, .w = 400, .h = 80, .reverse = 1,
        .hide_borders = 0, .meter_type = METER_LINEAR, .toggle_needle_side = 1,
        .name = "Simple linear horizontal meter, reverse on, with borders, "
        "needle side toggled"
    },
    {
        .coord.x = 0, .coord.y = 170, .w = 400, .h = 80, .reverse = 0,
        .hide_borders = 1, .meter_type = METER_MULTI_ARCS_LINEAR,
        .name = "Multiple arcs linear horizontal meter, reverse off, no borders"
    },
    {
        .coord.x = 0, .coord.y = 250, .w = 400, .h = 80, .reverse = 0,
        .hide_borders = 0, .meter_type = METER_PIE_CHART_LINEAR,
        .name = "Pie chart as linear horizontal meter, reverse off, with borders"
    },
    /*Vertical meters*/
    {
        .coord.x = 400, .coord.y = 0, .w = 100, .h = 400, .reverse = 0,
        .hide_borders = 1, .meter_type = METER_LINEAR,
        .name = "Simple vertical meter, reverse off, no_borders"
    },
    {
        .coord.x = 400 + 100, .coord.y = 0, .w = 100, .h = 400, .reverse = 1,
        .hide_borders = 1, .meter_type = METER_LINEAR,
        .name = "Simple vertical meter, reverse on, no borders"
    },
    {
        .coord.x = 400 + 200, .coord.y = 0, .w = 100, .h = 400, .reverse = 1,
        .hide_borders = 0, .meter_type = METER_MULTI_ARCS_LINEAR,
        .name = "Multiple arcs vertical meter, reverse on, with borders"
    },
    {
        .coord.x = 400 + 300, .coord.y = 0, .w = 100, .h = 400, .reverse = 1,
        .hide_borders = 1, .meter_type = METER_LINEAR, .toggle_needle_side = 1,
        .name = "Simple linear vertical meter, reverse on, no borders, "
        "needle side toggled"
    },
    /*Round meters*/
    {
        .coord.x = 0, .coord.y = 360, .w = 200, .h = 200, .reverse = 0,
        .hide_borders = 0, .meter_type = METER_SIMPLE,
        .name = "Simple meter, reverse off, with borders"
    },
    {
        .coord.x = 200, .coord.y = 360, .w = 200, .h = 200, .reverse = 0,
        .hide_borders = 0, .meter_type = METER_MULTI_ARCS,
        .name = "Multi arcs meter, reverse off, with borders"
    },
    {
        .coord.x = 400, .coord.y = 400, .w = 200, .h = 200, .reverse = 0,
        .hide_borders = 0, .meter_type = METER_CLOCK,
        .name = "Clock meter, reverse off, with borders"
    },
    {
        .coord.x = 600, .coord.y = 400, .w = 200, .h = 200, .reverse = 0,
        .hide_borders = 0, .meter_type = METER_PIE_CHART,
        .name = "Pie meter, reverse off, with borders"
    },
};

static void set_value(void * indic, int32_t v)
{
    for(uint8_t i = 0; i < ARRAY_SIZE(meter_items); i++) {
        for(uint8_t j = 0; j < meter_items[i].indic_cnt; j++) {
            if(meter_items[i].indic[j] == indic) {
                lv_meter_set_indicator_value(meter_items[i].obj, indic, v);
                break;
            }
        }
    }
}

static void set_value_mult_sc(void * indic, int32_t v)
{
    for(uint8_t i = 0; i < ARRAY_SIZE(meter_items); i++) {
        for(uint8_t j = 0; j < meter_items[i].indic_cnt; j++) {
            if(meter_items[i].indic[j] == indic) {
                lv_meter_set_indicator_end_value(meter_items[i].obj, indic, v);
                break;
            }
        }
    }
}

static void pie_meter_builder(struct meter_item * meter_in)
{
    lv_obj_t * meter =  meter_in->obj;

    /*Remove the background and the circle from the middle*/
    lv_obj_remove_style(meter, NULL, LV_PART_MAIN);
    lv_obj_remove_style(meter, NULL, LV_PART_INDICATOR);

    /*After deleting Main style we need to re-apply size and position*/
    lv_obj_set_size(meter, meter_in->w, meter_in->h);
    lv_obj_set_pos(meter, meter_in->coord.x, meter_in->coord.y);

    /*Add a scale first with no ticks.*/
    lv_meter_scale_t * scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale, 0, 0, 0, lv_color_black());
    lv_meter_set_scale_range(meter, scale, 0, 100, 360, 0);

    /*Add a three arc indicator*/
    lv_coord_t indic_w =  meter_in->w / 2;
    lv_meter_indicator_t * indic1 = lv_meter_add_arc(meter, scale, indic_w,
                                                     lv_palette_main(LV_PALETTE_ORANGE), 0);
    lv_meter_set_indicator_start_value(meter, indic1, 0);
    lv_meter_set_indicator_end_value(meter, indic1, 40);

    lv_meter_indicator_t * indic2 = lv_meter_add_arc(meter, scale, indic_w,
                                                     lv_palette_main(LV_PALETTE_YELLOW), 0);
    lv_meter_set_indicator_start_value(meter, indic2, 40);  /*Start from the previous*/
    lv_meter_set_indicator_end_value(meter, indic2, 80);

    lv_meter_indicator_t * indic3 = lv_meter_add_arc(meter, scale, indic_w,
                                                     lv_palette_main(LV_PALETTE_DEEP_ORANGE), 0);
    lv_meter_set_indicator_start_value(meter, indic3, 80);  /*Start from the previous*/
    lv_meter_set_indicator_end_value(meter, indic3, 100);
}

static void clock_meter_builder(struct meter_item * meter)
{
    /*Create a scale for the minutes*/
    /*61 ticks in a 360 degrees range (the last and the first line overlaps)*/
    lv_meter_scale_t * scale_min = lv_meter_add_scale(meter->obj);
    lv_meter_set_scale_ticks(meter->obj, scale_min, 61, 1, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_range(meter->obj, scale_min, 0, 60, 360, 270);

    /*Create another scale for the hours. It's only visual and contains only major ticks*/
    lv_meter_scale_t * scale_hour = lv_meter_add_scale(meter->obj);
    lv_meter_set_scale_ticks(meter->obj, scale_hour, 12, 0, 0, lv_palette_main(LV_PALETTE_GREY));    /*12 ticks*/
    lv_meter_set_scale_major_ticks(meter->obj, scale_hour, 1, 2, 20, lv_color_black(), 10);    /*Every tick is major*/
    lv_meter_set_scale_range(meter->obj, scale_hour, 1, 12, 330, 300);       /*[1..12] values in an almost full circle*/

    LV_IMG_DECLARE(img_hand)

    /*Add a the hands from images*/
    lv_meter_indicator_t * indic_min = lv_meter_add_needle_img(meter->obj, scale_min,
                                                               &img_hand, 5, 5);
    lv_meter_indicator_t * indic_hour = lv_meter_add_needle_img(meter->obj, scale_min,
                                                                &img_hand, 5, 5);

    meter->indic[0] = indic_min;
    meter->indic[1] = indic_hour;
    meter->indic_cnt = 2;

    /*Create an animation to set the value*/
    lv_anim_init(&meter->a);
    lv_anim_set_exec_cb(&meter->a, set_value);
    lv_anim_set_values(&meter->a, 0, 60);
    lv_anim_set_repeat_count(&meter->a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_time(&meter->a, 2000);     /*2 sec for 1 turn of the minute hand (1 hour)*/
    lv_anim_set_var(&meter->a, indic_min);
    lv_anim_start(&meter->a);

    lv_anim_set_var(&meter->a, indic_hour);
    lv_anim_set_time(&meter->a, 24000);    /*24 sec for 1 turn of the hour hand*/
    lv_anim_set_values(&meter->a, 0, 60);
    lv_anim_start(&meter->a);
}

static void multiarcs_meter_builder(struct meter_item * meter)
{
    /*Remove the circle from the middle*/
    lv_obj_remove_style(meter->obj, NULL, LV_PART_INDICATOR);

    /*Add a scale first*/
    lv_meter_scale_t * scale = lv_meter_add_scale(meter->obj);
    lv_meter_set_scale_ticks(meter->obj, scale, 11, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter->obj, scale, 1, 2, 30, lv_color_hex3(0xeee), 15);
    lv_meter_set_scale_range(meter->obj, scale, 0, 100, 270, 90);

    if(meter->reverse) {
        lv_obj_set_style_base_dir(meter->obj, LV_BASE_DIR_RTL, 0);
    }
    else {
        lv_obj_set_style_base_dir(meter->obj, LV_BASE_DIR_LTR, 0);
    }

    /*Add a three arc indicator*/
    lv_meter_indicator_t * indic1 = lv_meter_add_arc(meter->obj, scale, 10,
                                                     lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_indicator_t * indic2 = lv_meter_add_arc(meter->obj, scale, 10,
                                                     lv_palette_main(LV_PALETTE_GREEN), -10);
    lv_meter_indicator_t * indic3 = lv_meter_add_arc(meter->obj, scale, 10,
                                                     lv_palette_main(LV_PALETTE_BLUE), -20);

    meter->indic[0] = indic1;
    meter->indic[1] = indic2;
    meter->indic[2] = indic3;
    meter->indic_cnt = 3;

    /*Create an animation to set the value*/
    lv_anim_init(&meter->a);
    lv_anim_set_exec_cb(&meter->a, set_value_mult_sc);
    lv_anim_set_values(&meter->a, 0, 100);
    lv_anim_set_repeat_delay(&meter->a, 100);
    lv_anim_set_playback_delay(&meter->a, 100);
    lv_anim_set_repeat_count(&meter->a, LV_ANIM_REPEAT_INFINITE);

    lv_anim_set_time(&meter->a, 2000);
    lv_anim_set_playback_time(&meter->a, 500);
    lv_anim_set_var(&meter->a, indic1);
    lv_anim_start(&meter->a);

    lv_anim_set_time(&meter->a, 1000);
    lv_anim_set_playback_time(&meter->a, 1000);
    lv_anim_set_var(&meter->a, indic2);
    lv_anim_start(&meter->a);

    lv_anim_set_time(&meter->a, 1000);
    lv_anim_set_playback_time(&meter->a, 2000);
    lv_anim_set_var(&meter->a, indic3);
    lv_anim_start(&meter->a);
}

static void simple_meter_builder(struct meter_item * meter)
{
    /*Add a scale first*/
    lv_meter_scale_t * scale = lv_meter_add_scale(meter->obj);
    lv_meter_set_scale_ticks(meter->obj, scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter->obj, scale, 8, 4, 15, lv_color_black(), 12);

    lv_obj_set_style_text_align(meter->obj, LV_TEXT_ALIGN_CENTER, 0);

    /*Update scale direction: up to down or left to right*/
    if(meter->reverse) {
        lv_obj_set_style_base_dir(meter->obj, LV_BASE_DIR_RTL, 0);
    }
    else {
        lv_obj_set_style_base_dir(meter->obj, LV_BASE_DIR_LTR, 0);
    }

    /*Set needle side: it can be swapped with labels*/
    lv_meter_set_needle_side(scale, meter->toggle_needle_side);

    lv_meter_indicator_t * indic;

    /*Add a blue arc to the start*/
    indic = lv_meter_add_arc(meter->obj, scale, 5, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(meter->obj, indic, 0);
    lv_meter_set_indicator_end_value(meter->obj, indic, 20);

    /*Make the tick lines blue at the start of the scale*/
    indic = lv_meter_add_scale_lines(meter->obj, scale, lv_palette_main(LV_PALETTE_BLUE),
                                     lv_palette_main(LV_PALETTE_BLUE),
                                     false, 0);
    lv_meter_set_indicator_start_value(meter->obj, indic, 0);
    lv_meter_set_indicator_end_value(meter->obj, indic, 20);

    /*Add a red arc to the end*/
    indic = lv_meter_add_arc(meter->obj, scale, 5, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(meter->obj, indic, 80);
    lv_meter_set_indicator_end_value(meter->obj, indic, 100);

    /*Make the tick lines red at the end of the scale*/
    indic = lv_meter_add_scale_lines(meter->obj, scale, lv_palette_main(LV_PALETTE_RED),
                                     lv_palette_main(LV_PALETTE_RED),
                                     false,
                                     0);
    lv_meter_set_indicator_start_value(meter->obj, indic, 80);
    lv_meter_set_indicator_end_value(meter->obj, indic, 100);

    /*Add a needle line indicator*/
    indic = lv_meter_add_needle_line(meter->obj, scale, 4, lv_palette_main(LV_PALETTE_GREY), 0);

    meter->indic[0] = indic;
    meter->indic_cnt = 1;

    /*Create an animation to set the value*/
    lv_anim_init(&meter->a);
    lv_anim_set_exec_cb(&meter->a, set_value);
    lv_anim_set_var(&meter->a, indic);
    lv_anim_set_values(&meter->a, 0, 100);
    lv_anim_set_time(&meter->a, 3000);
    lv_anim_set_repeat_delay(&meter->a, 100);
    lv_anim_set_playback_time(&meter->a, 500);
    lv_anim_set_playback_delay(&meter->a, 100);
    lv_anim_set_repeat_count(&meter->a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&meter->a);
}

static void create_meter(uint8_t index)
{
    meter_items[index].obj = lv_meter_create(lv_scr_act());
    lv_obj_set_pos(meter_items[index].obj, meter_items[index].coord.x,
                   meter_items[index].coord.y);
    lv_obj_set_size(meter_items[index].obj, meter_items[index].w, meter_items[index].h);

    /*Remove rectangle around*/
    if(meter_items[index].hide_borders) {
        lv_obj_set_style_bg_opa(meter_items[index].obj, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(meter_items[index].obj, 0, 0);
    }

    /* Switch to linear if need */
    if(meter_items[index].meter_type == METER_LINEAR ||
       meter_items[index].meter_type == METER_MULTI_ARCS_LINEAR ||
       meter_items[index].meter_type == METER_PIE_CHART_LINEAR
      ) {
        lv_meter_set_indicator_mode(meter_items[index].obj, LV_METER_INDICATOR_MODE_LINEAR);

        if(meter_items[index].hide_borders == 0) {
            /* Linear meter(with borders) centered by center of tick. Label height bigger than
             * needle, so the whole widget looks shifted to one side.
             * For fix this situation adjust a pad.*/
            if(meter_items[index].w > meter_items[index].h) {
                if(meter_items[index].toggle_needle_side == 0) {
                    lv_obj_set_style_pad_bottom(meter_items[index].obj, 0, 0);
                }
            }
            else {
                lv_obj_set_style_pad_right(meter_items[index].obj, 0, 0);
            }
        }
    }

    switch(meter_items[index].meter_type) {

        case METER_SIMPLE:
        case METER_LINEAR:
            simple_meter_builder(&meter_items[index]);
            break;

        case METER_MULTI_ARCS:
        case METER_MULTI_ARCS_LINEAR:
            multiarcs_meter_builder(&meter_items[index]);
            break;

        case METER_CLOCK:
            clock_meter_builder(&meter_items[index]);
            break;

        case METER_PIE_CHART:
        case METER_PIE_CHART_LINEAR:
            pie_meter_builder(&meter_items[index]);
            break;

        default:
            LV_LOG_ERROR("Invalid meter type for ' %s '", meter_items[index].name);
            break;
    }
}

/**
 * Create all type of meters
 */
void lv_example_meter_5(void)
{
    for(uint8_t i = 0; i < ARRAY_SIZE(meter_items); i++) {
        create_meter(i);
        LV_LOG_WARN("%s - Created", meter_items[i].name);
    }
}

#endif
