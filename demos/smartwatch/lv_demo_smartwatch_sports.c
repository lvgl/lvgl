/**
 * @file lv_demo_smartwatch_sports.c
 * Weather screen layout & functions. Shows the current sports details as well as
 * hourly and daily forecasts.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_sports.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void sports_screen_events(lv_event_t * e);
static void animate_arcs(void * var, int32_t v);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * sports_screen;
static lv_obj_t * arc_steps;
static lv_obj_t * arc_distance;
static lv_obj_t * arc_calories;
static lv_obj_t * bar_steps;
static lv_obj_t * bar_distance;
static lv_obj_t * bar_calories;
static lv_obj_t * label_steps;
static lv_obj_t * label_distance;
static lv_obj_t * label_calories;
static lv_obj_t * label_time;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_sports_create(void)
{
    static bool inited = false;
    static lv_style_t main_style;

    if(!inited) {
        lv_style_init(&main_style);
        lv_style_set_text_color(&main_style, lv_color_white());
        lv_style_set_bg_color(&main_style, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&main_style, LV_OPA_100);
        lv_style_set_clip_corner(&main_style, true);
        lv_style_set_radius(&main_style, LV_RADIUS_CIRCLE);
        lv_style_set_translate_x(&main_style, 384);

    }

    sports_screen = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(sports_screen);
    lv_obj_add_style(sports_screen, &main_style, 0);
    lv_obj_set_size(sports_screen, lv_pct(100), lv_pct(100));
    lv_obj_set_scrollbar_mode(sports_screen, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(sports_screen, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_add_event_cb(sports_screen, sports_screen_events, LV_EVENT_ALL, NULL);


    arc_steps = lv_arc_create(sports_screen);
    lv_obj_set_width(arc_steps, 246);
    lv_obj_set_height(arc_steps, 246);
    lv_obj_set_x(arc_steps, 115);
    lv_obj_set_y(arc_steps, 80);
    lv_arc_set_value(arc_steps, 100);
    lv_arc_set_bg_angles(arc_steps, 270, 199);
    lv_obj_set_style_arc_color(arc_steps, lv_color_hex(0x2F1C25), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(arc_steps, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(arc_steps, 23, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_flag(arc_steps, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_style_arc_color(arc_steps, lv_color_hex(0xF697C4), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(arc_steps, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(arc_steps, 23, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(arc_steps, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(arc_steps, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    arc_distance = lv_arc_create(sports_screen);
    lv_obj_set_width(arc_distance, 200);
    lv_obj_set_height(arc_distance, 200);
    lv_obj_set_x(arc_distance, 138);
    lv_obj_set_y(arc_distance, 103);
    lv_arc_set_value(arc_distance, 50);
    lv_arc_set_bg_angles(arc_distance, 270, 205);
    lv_obj_set_style_arc_color(arc_distance, lv_color_hex(0x0E301A), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(arc_distance, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(arc_distance, 23, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_flag(arc_distance, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_style_arc_color(arc_distance, lv_color_hex(0x4CFE8D), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(arc_distance, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(arc_distance, 23, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(arc_distance, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(arc_distance, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    arc_calories = lv_arc_create(sports_screen);
    lv_obj_set_width(arc_calories, 155);
    lv_obj_set_height(arc_calories, 156);
    lv_obj_set_x(arc_calories, 161);
    lv_obj_set_y(arc_calories, 126);
    lv_arc_set_value(arc_calories, 50);
    lv_arc_set_bg_angles(arc_calories, 270, 215);
    lv_obj_set_style_arc_color(arc_calories, lv_color_hex(0x112E2E), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(arc_calories, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(arc_calories, 23, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_flag(arc_calories, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_style_arc_color(arc_calories, lv_color_hex(0x5CF2F1), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(arc_calories, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(arc_calories, 23, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(arc_calories, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(arc_calories, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    bar_steps = lv_bar_create(sports_screen);
    lv_bar_set_value(bar_steps, 100, LV_ANIM_OFF);
    lv_bar_set_start_value(bar_steps, 0, LV_ANIM_OFF);
    lv_obj_set_width(bar_steps, 246);
    lv_obj_set_height(bar_steps, 23);
    lv_obj_set_x(bar_steps, -1);
    lv_obj_set_y(bar_steps, 80);
    lv_obj_set_style_bg_color(bar_steps, lv_color_hex(0x2F1C25), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bar_steps, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(bar_steps, lv_color_hex(0x804E66), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bar_steps, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(bar_steps, lv_color_hex(0xF697C4), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(bar_steps, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(bar_steps, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(bar_steps, LV_GRAD_DIR_HOR, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    bar_distance = lv_bar_create(sports_screen);
    lv_bar_set_value(bar_distance, 100, LV_ANIM_OFF);
    lv_bar_set_start_value(bar_distance, 0, LV_ANIM_OFF);
    lv_obj_set_width(bar_distance, 246);
    lv_obj_set_height(bar_distance, 23);
    lv_obj_set_x(bar_distance, 0);
    lv_obj_set_y(bar_distance, 103);
    lv_obj_set_style_bg_color(bar_distance, lv_color_hex(0x0E301A), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bar_distance, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(bar_distance, lv_color_hex(0x268047), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bar_distance, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(bar_distance, lv_color_hex(0x4CFE8D), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(bar_distance, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(bar_distance, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(bar_distance, LV_GRAD_DIR_HOR, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    bar_calories = lv_bar_create(sports_screen);
    lv_bar_set_value(bar_calories, 100, LV_ANIM_OFF);
    lv_bar_set_start_value(bar_calories, 0, LV_ANIM_OFF);
    lv_obj_set_width(bar_calories, 246);
    lv_obj_set_height(bar_calories, 23);
    lv_obj_set_x(bar_calories, 0);
    lv_obj_set_y(bar_calories, 126);
    lv_obj_set_style_bg_color(bar_calories, lv_color_hex(0x112E2E), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bar_calories, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(bar_calories, lv_color_hex(0x30807F), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bar_calories, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(bar_calories, lv_color_hex(0x5CF2F1), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(bar_calories, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(bar_calories, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(bar_calories, LV_GRAD_DIR_HOR, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    LV_IMAGE_DECLARE(image_mask_gradient);
    lv_obj_t * image = lv_image_create(sports_screen);
    lv_image_set_src(image, &image_mask_gradient);

    LV_FONT_DECLARE(font_inter_bold_28);
    label_time = lv_label_create(sports_screen);
    lv_obj_set_x(label_time, 3);
    lv_obj_set_y(label_time, 25);
    lv_obj_set_align(label_time, LV_ALIGN_TOP_MID);
    lv_label_set_text(label_time, "13:37");
    lv_obj_set_style_text_font(label_time, &font_inter_bold_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    LV_FONT_DECLARE(font_inter_bold_57);
    label_steps = lv_label_create(sports_screen);
    lv_obj_set_x(label_steps, -180);
    lv_obj_set_y(label_steps, 157);
    lv_obj_set_align(label_steps, LV_ALIGN_TOP_RIGHT);
    lv_label_set_text(label_steps, "10,230");
    lv_obj_set_style_text_color(label_steps, lv_color_hex(0xF697C4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label_steps, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_steps, &font_inter_bold_57, LV_PART_MAIN | LV_STATE_DEFAULT);

    label_distance = lv_label_create(sports_screen);
    lv_obj_set_x(label_distance, -181);
    lv_obj_set_y(label_distance, 212);
    lv_obj_set_align(label_distance, LV_ALIGN_TOP_RIGHT);
    lv_label_set_text(label_distance, "383");
    lv_obj_set_style_text_color(label_distance, lv_color_hex(0x4CFE8D), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label_distance, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_distance, &font_inter_bold_57, LV_PART_MAIN | LV_STATE_DEFAULT);

    label_calories = lv_label_create(sports_screen);
    lv_obj_set_x(label_calories, -180);
    lv_obj_set_y(label_calories, 272);
    lv_obj_set_align(label_calories, LV_ALIGN_TOP_RIGHT);
    lv_label_set_text(label_calories, "24");
    lv_obj_set_style_text_color(label_calories, lv_color_hex(0x5CF2F1), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label_calories, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_calories, &font_inter_bold_57, LV_PART_MAIN | LV_STATE_DEFAULT);


    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, NULL);
    lv_anim_set_values(&a, 50, 200);
    lv_anim_set_duration(&a, 2000);
    lv_anim_set_exec_cb(&a, animate_arcs);
    lv_anim_set_playback_duration(&a, 2000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);


}

lv_obj_t * lv_demo_smartwatch_get_sports_screen(void)
{
    if(!sports_screen) {
        lv_demo_smartwatch_sports_create();
    }
    return sports_screen;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void animate_arcs(void * var, int32_t v)
{
    LV_UNUSED(var);
    int bar_value = v > 100 ? 100 : v;  // Bars take up to 100
    int arc_value = v > 100 ? v - 100 : 0;  // Arcs take the remaining (if any)

    lv_bar_set_value(bar_steps, bar_value, LV_ANIM_OFF);
    lv_bar_set_value(bar_distance, bar_value, LV_ANIM_OFF);
    lv_bar_set_value(bar_calories, bar_value, LV_ANIM_OFF);

    lv_arc_set_value(arc_steps, arc_value / 5 * 4);
    lv_arc_set_value(arc_distance, arc_value / 2);
    lv_arc_set_value(arc_calories, arc_value / 3 * 2);


}

static void sports_screen_events(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_GESTURE) {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

        if(dir == LV_DIR_LEFT) {
            lv_smartwatch_animate_x(lv_demo_smartwatch_get_sports_screen(), -384 - TRANSITION_GAP, 1000, 0);
            lv_smartwatch_animate_x_from(lv_demo_smartwatch_get_music_screen(), 384 + TRANSITION_GAP, 0, 1000, 0);
        }
        if(dir == LV_DIR_RIGHT) {
            lv_smartwatch_animate_x_from(lv_demo_smartwatch_get_health_screen(), -384 - TRANSITION_GAP, 0, 1000, 0);
            lv_smartwatch_animate_x(lv_demo_smartwatch_get_sports_screen(), 384 + TRANSITION_GAP, 1000, 0);
        }

    }
}

#endif /*LV_USE_DEMO_SMARTWATCH*/