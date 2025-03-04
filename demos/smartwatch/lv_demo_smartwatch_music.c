/**
 * @file lv_demo_smartwatch_music.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_music.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void music_screen_events(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * music_screen;


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_music_create(void)
{

    static bool inited = false;
    static lv_style_t main_style;

    if(!inited) {
        lv_style_init(&main_style);
        lv_style_set_text_color(&main_style, lv_color_white());
        lv_style_set_bg_color(&main_style, lv_color_hex(0x555555));
        lv_style_set_bg_opa(&main_style, LV_OPA_100);
        lv_style_set_clip_corner(&main_style, true);
        lv_style_set_radius(&main_style, LV_RADIUS_CIRCLE);
        lv_style_set_translate_x(&main_style, 384);

    }

    music_screen = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(music_screen);
    lv_obj_add_style(music_screen, &main_style, 0);
    lv_obj_set_size(music_screen, lv_pct(100), lv_pct(100));
    lv_obj_set_scrollbar_mode(music_screen, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(music_screen, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_add_event_cb(music_screen, music_screen_events, LV_EVENT_ALL, NULL);


    lv_obj_t * label = lv_label_create(music_screen);
    lv_label_set_text(label, "Music");
    lv_obj_set_align(label, LV_ALIGN_CENTER);
}

lv_obj_t * lv_demo_smartwatch_get_music_screen(void)
{
    if(!music_screen) {
        lv_demo_smartwatch_music_create();
    }
    return music_screen;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void music_screen_events(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_GESTURE) {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());


        if(dir == LV_DIR_LEFT) {
            ui_animate_x(lv_demo_smartwatch_get_music_screen(), -384, 1000, 0);
            animate_arc(arc_cont, ARC_EXPAND_RIGHT, 700, 300);
            ui_anim_opa(arc, 255, 500, 500);
            ui_anim_opa(overlay, 0, 100, 0);
        }
        if(dir == LV_DIR_RIGHT) {
            ui_animate_x_from(lv_demo_smartwatch_get_sports_screen(), -384 - TRANSITION_GAP, 0, 1000, 0);
            ui_animate_x(lv_demo_smartwatch_get_music_screen(), 384 + TRANSITION_GAP, 1000, 0);
        }

    }
}

#endif /*LV_USE_DEMO_SMARTWATCH*/