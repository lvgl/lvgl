/**
 * @file lv_demo_smartwatch_settings.c
 * Settings screen layout & functions. Contains basic settings for smartwatch.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_settings.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    ST_SLIDER,
    ST_SWITCH,
    ST_DROPDOWN,
    ST_LABEL,
    ST_MAX
} settings_type_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void create_screen_settings(void);
static void settings_action_event_cb(lv_event_t * e);
static void settings_screen_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * settings_screen;
static lv_obj_t * settings_list;

static lv_obj_t * brightness_slider;
static lv_obj_t * circular_switch;
static lv_obj_t * timeout_dropdown;
static lv_obj_t * rotation_dropdown;
static lv_obj_t * alerts_switch;
static lv_obj_t * about_label;
static lv_obj_t * scroll_mode;
static lv_obj_t * hint_switch;
static bool manual_trigger;

static lv_smartwatch_settings_change_cb_t settings_change_cb;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_settings_create(void)
{
    create_screen_settings();
}

void lv_demo_smartwatch_settings_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay)
{
    lv_screen_load_anim(settings_screen, anim_type, time, delay, false);
}

lv_obj_t * lv_demo_smartwatch_settings_add_slider(uint16_t id, const char * name, const lv_img_dsc_t * img,
                                                  int32_t value,
                                                  int32_t min, int32_t max)
{
    lv_obj_t * panel = lv_obj_create(settings_list);
    lv_obj_set_width(panel, lv_pct(85));
    lv_obj_set_height(panel, 64);
    lv_obj_set_align(panel, LV_ALIGN_CENTER);
    lv_obj_remove_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(panel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * slider = lv_slider_create(panel);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, value, LV_ANIM_OFF);
    if(lv_slider_get_mode(slider) == LV_SLIDER_MODE_RANGE)
        lv_slider_set_left_value(slider, 0, LV_ANIM_OFF);
    lv_obj_set_width(slider, 123);
    lv_obj_set_height(slider, 10);
    lv_obj_set_x(slider, 65);
    lv_obj_set_y(slider, 12);
    lv_obj_set_align(slider, LV_ALIGN_LEFT_MID);
    lv_obj_remove_flag(slider, LV_OBJ_FLAG_GESTURE_BUBBLE);

    lv_obj_t * icon = lv_image_create(panel);
    lv_image_set_src(icon, img);
    lv_obj_set_width(icon, LV_SIZE_CONTENT);
    lv_obj_set_height(icon, LV_SIZE_CONTENT);
    lv_obj_set_align(icon, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(icon, 150);

    lv_obj_t * label = lv_label_create(panel);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_x(label, 60);
    lv_obj_set_y(label, 3);
    lv_label_set_text(label, name);

    lv_obj_add_event_cb(slider, settings_action_event_cb, LV_EVENT_VALUE_CHANGED,
                        (void *)(intptr_t)((ST_SLIDER << 16) | id));

    return slider;
}

lv_obj_t * lv_demo_smartwatch_settings_add_toggle(uint16_t id, const char * name, const lv_img_dsc_t * img, bool state)
{
    lv_obj_t * panel = lv_obj_create(settings_list);
    lv_obj_set_width(panel, lv_pct(85));
    lv_obj_set_height(panel, 64);
    lv_obj_set_align(panel, LV_ALIGN_CENTER);
    lv_obj_remove_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(panel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * icon = lv_image_create(panel);
    lv_image_set_src(icon, img);
    lv_obj_set_width(icon, LV_SIZE_CONTENT);
    lv_obj_set_height(icon, LV_SIZE_CONTENT);
    lv_obj_set_align(icon, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(icon, 150);

    lv_obj_t * toggle = lv_switch_create(panel);
    lv_obj_set_width(toggle, 50);
    lv_obj_set_height(toggle, 25);
    lv_obj_set_x(toggle, 65);
    lv_obj_set_y(toggle, 12);
    lv_obj_set_align(toggle, LV_ALIGN_LEFT_MID);

    if(state) {
        lv_obj_add_state(toggle, LV_STATE_CHECKED);
    }
    else {
        lv_obj_remove_state(toggle, LV_STATE_CHECKED);
    }

    lv_obj_t * label = lv_label_create(panel);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_x(label, 60);
    lv_obj_set_y(label, 3);
    lv_label_set_text(label, name);

    lv_obj_add_event_cb(toggle, settings_action_event_cb, LV_EVENT_VALUE_CHANGED,
                        (void *)(intptr_t)((ST_SWITCH << 16) | id));

    return toggle;
}

lv_obj_t * lv_demo_smartwatch_settings_add_dropdown(uint16_t id, const char * name, const lv_img_dsc_t * img,
                                                    const char * options)
{
    lv_obj_t * panel = lv_obj_create(settings_list);
    lv_obj_set_width(panel, lv_pct(85));
    lv_obj_set_height(panel, 64);
    lv_obj_set_x(panel, 37);
    lv_obj_set_y(panel, 7);
    lv_obj_set_align(panel, LV_ALIGN_CENTER);
    lv_obj_remove_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(panel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * dropdown = lv_dropdown_create(panel);
    lv_dropdown_set_options(dropdown, options);
    lv_obj_set_width(dropdown, 120);
    lv_obj_set_height(dropdown, LV_SIZE_CONTENT);
    lv_obj_set_x(dropdown, 65);
    lv_obj_set_y(dropdown, 12);
    lv_obj_set_align(dropdown, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(dropdown, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_bg_color(dropdown, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(dropdown, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(dropdown, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(dropdown, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(dropdown, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(lv_dropdown_get_list(dropdown), lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(lv_dropdown_get_list(dropdown), 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * icon = lv_image_create(panel);
    lv_image_set_src(icon, img);
    lv_obj_set_width(icon, LV_SIZE_CONTENT);
    lv_obj_set_height(icon, LV_SIZE_CONTENT);
    lv_obj_set_align(icon, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(icon, 150);

    lv_obj_t * label = lv_label_create(panel);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_x(label, 60);
    lv_obj_set_y(label, 3);
    lv_label_set_text(label, name);

    lv_obj_add_event_cb(dropdown, settings_action_event_cb, LV_EVENT_VALUE_CHANGED,
                        (void *)(intptr_t)((ST_DROPDOWN << 16) | id));

    return dropdown;
}

lv_obj_t * lv_demo_smartwatch_settings_add_label(uint16_t id, const char * name, const lv_img_dsc_t * img)
{
    lv_obj_t * panel = lv_obj_create(settings_list);
    lv_obj_set_width(panel, lv_pct(85));
    lv_obj_set_height(panel, LV_SIZE_CONTENT);
    lv_obj_set_align(panel, LV_ALIGN_CENTER);
    lv_obj_remove_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(panel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * icon = lv_image_create(panel);
    lv_image_set_src(icon, img);
    lv_obj_set_width(icon, LV_SIZE_CONTENT);
    lv_obj_set_height(icon, LV_SIZE_CONTENT);
    lv_obj_set_x(icon, -5);
    lv_obj_set_y(icon, 2);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(icon, 150);

    lv_obj_t * label = lv_label_create(panel);
    lv_obj_set_width(label, 130);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_x(label, 60);
    lv_obj_set_y(label, 7);
    lv_label_set_text(label, name);

    lv_obj_add_event_cb(panel, settings_action_event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)((ST_LABEL << 16) | id));

    return label;
}

void lv_demo_smartwatch_set_settings_actions_cb(lv_smartwatch_settings_change_cb_t cb)
{
    settings_change_cb = cb;
}

void lv_demo_smartwatch_set_default_brightness(uint8_t brightness)
{
    lv_slider_set_value(brightness_slider, brightness, LV_ANIM_OFF);
    lv_demo_smartwatch_update_brightness_slider(brightness);
    manual_trigger = true;
    lv_obj_send_event(brightness_slider, LV_EVENT_VALUE_CHANGED, (void *)(intptr_t)((ST_SLIDER << 16) | 0x0001));
}

void lv_demo_smartwatch_set_default_timeout(uint8_t timeout)
{
    lv_dropdown_set_selected(timeout_dropdown, timeout, LV_ANIM_OFF);
    manual_trigger = true;
    lv_obj_send_event(timeout_dropdown, LV_EVENT_VALUE_CHANGED, (void *)(intptr_t)((ST_DROPDOWN << 16) | 0x0003));
}

void lv_demo_smartwatch_set_default_rotation(uint8_t rotation)
{
    lv_dropdown_set_selected(rotation_dropdown, rotation, LV_ANIM_OFF);
    manual_trigger = true;
    lv_obj_send_event(rotation_dropdown, LV_EVENT_VALUE_CHANGED, (void *)(intptr_t)((ST_DROPDOWN << 16) | 0x0004));
}

void lv_demo_smartwatch_set_default_circular_scroll(bool enabled)
{
    if(enabled) {
        lv_obj_add_state(circular_switch, LV_STATE_CHECKED);
    }
    else {
        lv_obj_remove_state(circular_switch, LV_STATE_CHECKED);
    }
    manual_trigger = true;
    lv_obj_send_event(circular_switch, LV_EVENT_VALUE_CHANGED, (void *)(intptr_t)((ST_SWITCH << 16) | 0x0002));
}

void lv_demo_smartwatch_set_default_alert_state(bool enabled)
{
    if(enabled) {
        lv_obj_add_state(alerts_switch, LV_STATE_CHECKED);
    }
    else {
        lv_obj_remove_state(alerts_switch, LV_STATE_CHECKED);
    }
    manual_trigger = true;
    lv_obj_send_event(alerts_switch, LV_EVENT_VALUE_CHANGED, (void *)(intptr_t)((ST_SWITCH << 16) | 0x0005));
}

void lv_demo_smartwatch_set_default_hints_state(bool enabled)
{
    if(enabled) {
        lv_obj_add_state(hint_switch, LV_STATE_CHECKED);
    }
    else {
        lv_obj_remove_state(hint_switch, LV_STATE_CHECKED);
    }
    manual_trigger = true;
    lv_obj_send_event(hint_switch, LV_EVENT_VALUE_CHANGED, (void *)(intptr_t)((ST_SWITCH << 16) | 0x0009));
}

void lv_demo_smartwatch_set_default_scrollbar_mode(lv_scrollbar_mode_t mode)
{
    lv_dropdown_set_selected(scroll_mode, mode, LV_ANIM_OFF);
    manual_trigger = true;
    lv_obj_send_event(scroll_mode, LV_EVENT_VALUE_CHANGED, (void *)(intptr_t)((ST_DROPDOWN << 16) | 0x0008));
}

void lv_demo_smartwatch_set_default_about_info(const char * info)
{
    lv_label_set_text(about_label, info);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void settings_action_event_cb(lv_event_t * e)
{

    lv_obj_t * active_screen = lv_screen_active();

    LV_LOG_WARN("Settings actions event triggered, settings screen %s, Manually triggered %s",
                active_screen == settings_screen ? "active" : "inactive", manual_trigger ? "yes" : "no");

    if(active_screen != settings_screen && !manual_trigger) {
        /* event was triggered but the current screen is no longer active */
        /* event was not manually triggerd */
        LV_LOG_WARN("Settings actions event triggered but the current screen is no longer active");
        return;
    }

    uint32_t data = (uint32_t)(intptr_t)lv_event_get_user_data(e);
    uint16_t id = (uint16_t)data;
    uint16_t type = (uint16_t)(data >> 16);
    uint64_t value;
    lv_obj_t * target = lv_event_get_target(e);

    /* extract value based on the type */
    switch(type) {
        case ST_SLIDER:
            value = lv_slider_get_value(target);
            break;
        case ST_DROPDOWN:
            value = lv_dropdown_get_selected(target);
            break;
        case ST_SWITCH:
            value = lv_obj_has_state(target, LV_STATE_CHECKED) ? 1 : 0;
            break;
        case ST_LABEL:
            break;

        default:
            break;
    }

    /* send actions to user callback function if set */
    if(settings_change_cb != NULL) {
        settings_change_cb(id, value);
    }

    /* handle actions for the inbuilt settings */
    switch(id) {
        case 0x0001:
            if(!manual_trigger) {
                /* Update the brightness slider in the control screen */
                lv_demo_smartwatch_update_brightness_slider(value);
            }
            break;
        case 0x0002:
            lv_demo_smartwatch_set_circular_scroll(value);
            lv_obj_scroll_by(settings_list, 0, value ? 1 : -1, LV_ANIM_OFF);
            break;
        case 0x0007:
            lv_demo_smartwatch_easter_egg_load(LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
            break;
        case 0x0008:
            lv_demo_smartwatch_set_scrollbar_mode(value);
            lv_obj_set_scrollbar_mode(settings_list, lv_demo_smartwatch_get_scrollbar_mode());
            break;
        case 0x0009:
            lv_demo_smartwatch_set_scroll_hint(value);
            break;

        default:
            break;
    }

    manual_trigger = false;
}

static void settings_screen_event_cb(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
        lv_demo_smartwatch_list_load(LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0);
    }

    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_LEFT) {
        lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT);
    }

    if(event_code == LV_EVENT_SCREEN_LOAD_START) {
        lv_obj_scroll_by(settings_list, 0, 1, LV_ANIM_OFF);
        lv_obj_scroll_by(settings_list, 0, -1, LV_ANIM_OFF);

        lv_obj_set_scrollbar_mode(settings_list, lv_demo_smartwatch_get_scrollbar_mode());
    }

    if(event_code == LV_EVENT_SCREEN_LOADED) {
        lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT);
    }
}

static void create_screen_settings(void)
{

    settings_screen = lv_obj_create(NULL);
    lv_obj_remove_flag(settings_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(settings_screen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(settings_screen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(settings_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    settings_list = lv_obj_create(settings_screen);
    lv_obj_set_width(settings_list, lv_pct(100));
    lv_obj_set_height(settings_list, lv_pct(100));
    lv_obj_set_align(settings_list, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(settings_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(settings_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(settings_list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(settings_list, LV_DIR_VER);
    lv_obj_set_style_radius(settings_list, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(settings_list, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(settings_list, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(settings_list, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(settings_list, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(settings_list, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(settings_list, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(settings_list, 70, LV_PART_MAIN | LV_STATE_DEFAULT);

    brightness_slider = lv_demo_smartwatch_settings_add_slider(0x0001, "Screen Brightness", &img_brightness_icon, 50, 1,
                                                               255);
    circular_switch = lv_demo_smartwatch_settings_add_toggle(0x0002, "Circular Scroll", &img_scrolling_icon, true);
    scroll_mode = lv_demo_smartwatch_settings_add_dropdown(0x0008, "Scrollbar Mode", &img_scrolling_icon,
                                                           "OFF\nON\nACTIVE\nAUTO");
    timeout_dropdown = lv_demo_smartwatch_settings_add_dropdown(0x0003, "Screen Timeout", &img_timeout_icon,
                                                                "5 Seconds\n10 Seconds\n20 Seconds\n30 Seconds\nAlways On");
    rotation_dropdown = lv_demo_smartwatch_settings_add_dropdown(0x0004, "Screen Rotation", &img_screen_rotate_icon,
                                                                 "Default\n90\n180\n270");
    hint_switch = lv_demo_smartwatch_settings_add_toggle(0x0009, "Show Hints", &img_info_icon, false);
    alerts_switch = lv_demo_smartwatch_settings_add_toggle(0x0005, "Show Alerts", &img_alert_icon, false);
    about_label = lv_demo_smartwatch_settings_add_label(0x0006, "LVGL Demo\nSmart Watch UI\n11:22:33:44:55:66",
                                                        &img_info_icon);
    lv_demo_smartwatch_settings_add_label(0x0007, "Made with love\nin Kenya\nusing LVGL", &img_kenya_icon);

    lv_obj_add_event_cb(settings_list, lv_demo_smartwatch_scroll_event, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(settings_screen, settings_screen_event_cb, LV_EVENT_ALL, NULL);
}

#endif /*LV_USE_DEMO_SMARTWATCH*/
