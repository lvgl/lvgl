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

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * sports_screen;


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
        lv_style_set_bg_color(&main_style, lv_color_hex(0xff4a4a4a));
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


    lv_obj_t * label = lv_label_create(sports_screen);
    lv_label_set_text(label, "Sports");
    lv_obj_set_align(label, LV_ALIGN_CENTER);
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

static void sports_screen_events(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_GESTURE) {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

        if(dir == LV_DIR_LEFT) {
            ui_animate_x(lv_demo_smartwatch_get_sports_screen(), -384 - TRANSITION_GAP, 1000, 0);
            ui_animate_x_from(lv_demo_smartwatch_get_music_screen(), 384 + TRANSITION_GAP, 0, 1000, 0);
        }
        if(dir == LV_DIR_RIGHT) {
            ui_animate_x_from(lv_demo_smartwatch_get_health_screen(), -384 - TRANSITION_GAP, 0, 1000, 0);
            ui_animate_x(lv_demo_smartwatch_get_sports_screen(), 384 + TRANSITION_GAP, 1000, 0);
        }

    }
}

#endif /*LV_USE_DEMO_SMARTWATCH*/