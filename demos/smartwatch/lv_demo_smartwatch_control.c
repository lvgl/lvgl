/**
 * @file lv_demo_smartwatch_control.c
 * Control screen layout & functions. Contains quick actions such as music controls.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_control.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void control_screen_create(void);
static void control_screen_event_cb(lv_event_t * e);

static void control_music_events_cb(lv_event_t * e);
static void qr_button_event_cb(lv_event_t * e);
static void close_button_event_cb(lv_event_t * e);
static void settings_button_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * control_screen;
static lv_obj_t * control_music_play;
static lv_obj_t * control_music_previous;
static lv_obj_t * control_music_next;
static lv_obj_t * control_bluetooth_icon;
static lv_obj_t * control_settings_button;
static lv_obj_t * control_volume_up;
static lv_obj_t * control_volume_down;
static lv_obj_t * control_qr_button;
static lv_obj_t * control_close_button;

static lv_smartwatch_music_control_cb_t music_control_cb;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_control_create(void)
{
    control_screen_create();
}

void lv_demo_smartwatch_control_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay)
{
    lv_screen_load_anim(control_screen, anim_type, time, delay, false);
}

void lv_demo_smartwatch_set_music_control_cb(lv_smartwatch_music_control_cb_t cb)
{
    music_control_cb = cb;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void control_screen_create(void)
{
    control_screen = lv_obj_create(NULL);
    lv_obj_remove_flag(control_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(control_screen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(control_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    control_music_play = lv_image_create(control_screen);
    lv_image_set_src(control_music_play, &img_play_icon);
    lv_obj_set_width(control_music_play, LV_SIZE_CONTENT);
    lv_obj_set_height(control_music_play, LV_SIZE_CONTENT);
    lv_obj_set_align(control_music_play, LV_ALIGN_CENTER);
    lv_obj_add_flag(control_music_play, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(control_music_play, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(control_music_play, 200);
    lv_obj_set_style_radius(control_music_play, 5, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(control_music_play, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(control_music_play, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    control_music_previous = lv_image_create(control_screen);
    lv_image_set_src(control_music_previous, &img_left_arrow_icon);
    lv_obj_set_width(control_music_previous, LV_SIZE_CONTENT);
    lv_obj_set_height(control_music_previous, LV_SIZE_CONTENT);
    lv_obj_set_x(control_music_previous, -75);
    lv_obj_set_y(control_music_previous, 0);
    lv_obj_set_align(control_music_previous, LV_ALIGN_CENTER);
    lv_obj_add_flag(control_music_previous, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(control_music_previous, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(control_music_previous, 200);
    lv_obj_set_style_radius(control_music_previous, 5, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(control_music_previous, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(control_music_previous, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    control_music_next = lv_image_create(control_screen);
    lv_image_set_src(control_music_next, &img_right_arrow_icon);
    lv_obj_set_width(control_music_next, LV_SIZE_CONTENT);
    lv_obj_set_height(control_music_next, LV_SIZE_CONTENT);
    lv_obj_set_x(control_music_next, 75);
    lv_obj_set_y(control_music_next, 0);
    lv_obj_set_align(control_music_next, LV_ALIGN_CENTER);
    lv_obj_add_flag(control_music_next, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(control_music_next, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(control_music_next, 200);
    lv_obj_set_style_radius(control_music_next, 5, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(control_music_next, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(control_music_next, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    control_bluetooth_icon = lv_image_create(control_screen);
    lv_image_set_src(control_bluetooth_icon, &img_bluetooth_g_icon);
    lv_obj_set_width(control_bluetooth_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(control_bluetooth_icon, LV_SIZE_CONTENT);
    lv_obj_set_x(control_bluetooth_icon, 0);
    lv_obj_set_y(control_bluetooth_icon, -100);
    lv_obj_set_align(control_bluetooth_icon, LV_ALIGN_CENTER);
    lv_obj_add_state(control_bluetooth_icon, LV_STATE_CHECKED);
    lv_obj_add_flag(control_bluetooth_icon, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_remove_flag(control_bluetooth_icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(control_bluetooth_icon, 200);
    lv_obj_set_style_image_recolor(control_bluetooth_icon, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_image_recolor_opa(control_bluetooth_icon, 230, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_radius(control_bluetooth_icon, 5, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(control_bluetooth_icon, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(control_bluetooth_icon, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    control_settings_button = lv_image_create(control_screen);
    lv_image_set_src(control_settings_button, &img_general_settings_icon);
    lv_obj_set_width(control_settings_button, LV_SIZE_CONTENT);
    lv_obj_set_height(control_settings_button, LV_SIZE_CONTENT);
    lv_obj_set_x(control_settings_button, 40);
    lv_obj_set_y(control_settings_button, -60);
    lv_obj_set_align(control_settings_button, LV_ALIGN_CENTER);
    lv_obj_add_flag(control_settings_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(control_settings_button, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(control_settings_button, 200);
    lv_obj_set_style_radius(control_settings_button, 5, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(control_settings_button, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(control_settings_button, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    control_volume_up = lv_image_create(control_screen);
    lv_image_set_src(control_volume_up, &img_vol_down_icon);
    lv_obj_set_width(control_volume_up, LV_SIZE_CONTENT);
    lv_obj_set_height(control_volume_up, LV_SIZE_CONTENT);
    lv_obj_set_x(control_volume_up, 40);
    lv_obj_set_y(control_volume_up, 60);
    lv_obj_set_align(control_volume_up, LV_ALIGN_CENTER);
    lv_obj_add_flag(control_volume_up, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(control_volume_up, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(control_volume_up, 200);
    lv_obj_set_style_radius(control_volume_up, 5, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(control_volume_up, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(control_volume_up, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    control_volume_down = lv_image_create(control_screen);
    lv_image_set_src(control_volume_down, &img_vol_up_icon);
    lv_obj_set_width(control_volume_down, LV_SIZE_CONTENT);
    lv_obj_set_height(control_volume_down, LV_SIZE_CONTENT);
    lv_obj_set_x(control_volume_down, -40);
    lv_obj_set_y(control_volume_down, 60);
    lv_obj_set_align(control_volume_down, LV_ALIGN_CENTER);
    lv_obj_add_flag(control_volume_down, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(control_volume_down, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(control_volume_down, 200);
    lv_obj_set_style_radius(control_volume_down, 5, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(control_volume_down, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(control_volume_down, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    control_qr_button = lv_image_create(control_screen);
    lv_image_set_src(control_qr_button, &img_pay_icon);
    lv_obj_set_width(control_qr_button, LV_SIZE_CONTENT);
    lv_obj_set_height(control_qr_button, LV_SIZE_CONTENT);
    lv_obj_set_x(control_qr_button, -40);
    lv_obj_set_y(control_qr_button, -60);
    lv_obj_set_align(control_qr_button, LV_ALIGN_CENTER);
    lv_obj_add_flag(control_qr_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(control_qr_button, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(control_qr_button, 200);
    lv_obj_set_style_radius(control_qr_button, 5, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(control_qr_button, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(control_qr_button, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    control_close_button = lv_image_create(control_screen);
    lv_image_set_src(control_close_button, &img_up_arrow_icon);
    lv_obj_set_width(control_close_button, 100);
    lv_obj_set_height(control_close_button, LV_SIZE_CONTENT);
    lv_obj_set_y(control_close_button, -5);
    lv_obj_set_align(control_close_button, LV_ALIGN_BOTTOM_MID);
    lv_obj_add_flag(control_close_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(control_close_button, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(control_close_button, 5, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(control_close_button, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(control_close_button, 150, LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_add_event_cb(control_music_play, control_music_events_cb, LV_EVENT_ALL, (void *)(intptr_t)0x9900);
    lv_obj_add_event_cb(control_music_previous, control_music_events_cb, LV_EVENT_ALL, (void *)(intptr_t)0x9D02);
    lv_obj_add_event_cb(control_music_next, control_music_events_cb, LV_EVENT_ALL, (void *)(intptr_t)0x9D03);
    lv_obj_add_event_cb(control_volume_up, control_music_events_cb, LV_EVENT_ALL, (void *)(intptr_t)0x99A1);
    lv_obj_add_event_cb(control_volume_down, control_music_events_cb, LV_EVENT_ALL, (void *)(intptr_t)0x99A2);

    lv_obj_add_event_cb(control_settings_button, settings_button_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(control_qr_button, qr_button_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(control_close_button, close_button_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(control_screen, control_screen_event_cb, LV_EVENT_ALL, NULL);
}

static void control_screen_event_cb(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_TOP) {
        lv_demo_smartwatch_home_load(LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0);
    }
}

static void qr_button_event_cb(lv_event_t * e)
{
    lv_disp_t * display = lv_display_get_default();
    lv_obj_t * active_screen = lv_display_get_screen_active(display);
    if(active_screen != control_screen) {
        /* event was triggered but the current screen is no longer active */
        return;
    }
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_CLICKED) {
        lv_demo_smartwatch_set_load_app_list(false);
        lv_demo_smartwatch_qr_load(LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 500, 0);
    }
}

static void close_button_event_cb(lv_event_t * e)
{
    lv_disp_t * display = lv_display_get_default();
    lv_obj_t * active_screen = lv_display_get_screen_active(display);
    if(active_screen != control_screen) {
        /* event was triggered but the current screen is no longer active */
        return;
    }
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_CLICKED) {
        lv_demo_smartwatch_home_load(LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0);
    }
}

static void settings_button_event_cb(lv_event_t * e)
{
    lv_disp_t * display = lv_display_get_default();
    lv_obj_t * active_screen = lv_display_get_screen_active(display);
    if(active_screen != control_screen) {
        /* event was triggered but the current screen is no longer active */
        return;
    }
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_CLICKED) {
        lv_demo_smartwatch_settings_load(LV_SCR_LOAD_ANIM_FADE_IN, 500, 0);
    }
}

static void control_music_events_cb(lv_event_t * e)
{
    lv_disp_t * display = lv_display_get_default();
    lv_obj_t * active_screen = lv_display_get_screen_active(display);
    if(active_screen != control_screen) {
        /* event was triggered but the current screen is no longer active */
        return;
    }
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_CLICKED) {
        uint16_t code = (uint16_t)(intptr_t)lv_event_get_user_data(e);

        /* send actions to user callback if defined */
        if(music_control_cb != NULL) {
            music_control_cb(code);
        }
    }
}

#endif /*LV_USE_DEMO_SMARTWATCH*/
