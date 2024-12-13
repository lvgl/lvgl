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


static lv_obj_t * about_label;

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
    lv_obj_set_width(panel, 200);
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
    lv_obj_set_width(panel, 200);
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
    lv_obj_set_x(icon, -74);
    lv_obj_set_y(icon, 2);
    lv_obj_set_align(icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(icon, 150);

    lv_obj_t * toggle = lv_switch_create(panel);
    lv_obj_set_width(toggle, 50);
    lv_obj_set_height(toggle, 25);
    lv_obj_set_x(toggle, 57);
    lv_obj_set_y(toggle, 29);

    if(state) {
        lv_obj_add_state(toggle, LV_STATE_CHECKED);
    }
    else {
        lv_obj_remove_state(toggle, LV_STATE_CHECKED);
    }

    lv_obj_t * label = lv_label_create(panel);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_x(label, 54);
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
    lv_obj_set_width(panel, 200);
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
    lv_obj_set_x(dropdown, 20);
    lv_obj_set_y(dropdown, 10);
    lv_obj_set_align(dropdown, LV_ALIGN_CENTER);
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
    lv_obj_set_x(icon, -74);
    lv_obj_set_y(icon, 2);
    lv_obj_set_align(icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(icon, 150);

    lv_obj_t * label = lv_label_create(panel);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_x(label, 61);
    lv_obj_set_y(label, 0);
    lv_label_set_text(label, name);

    lv_obj_add_event_cb(dropdown, settings_action_event_cb, LV_EVENT_VALUE_CHANGED,
                        (void *)(intptr_t)((ST_DROPDOWN << 16) | id));

    return dropdown;
}

lv_obj_t * lv_demo_smartwatch_settings_add_label(uint16_t id, const char * name, const lv_img_dsc_t * img)
{
    lv_obj_t * panel = lv_obj_create(settings_list);
    lv_obj_set_width(panel, 200);
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

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void settings_action_event_cb(lv_event_t * e)
{
    lv_disp_t * display = lv_display_get_default();
    lv_obj_t * active_screen = lv_display_get_screen_active(display);
    if(active_screen != settings_screen) {
        /* event was triggered but the current screen is no longer active */
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
        case 0x0002:
            lv_demo_smartwatch_set_circular_scroll(lv_obj_has_state(target, LV_STATE_CHECKED));
            break;
        case 0x0007:
            lv_demo_smartwatch_easter_egg_load(LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
            break;

        default:
            break;
    }
}

static void settings_screen_event_cb(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
        lv_demo_smartwatch_list_load(LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0);
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

    lv_demo_smartwatch_settings_add_slider(0x0001, "Screen Brightness", &img_brightness_icon, 50, 1, 255);
    lv_demo_smartwatch_settings_add_toggle(0x0002, "Circular Scroll", &img_scrolling_icon, true);
    lv_demo_smartwatch_settings_add_dropdown(0x0003, "Screen Timeout", &img_timeout_icon,
                                             "5 Seconds\n10 Seconds\n20 Seconds\n30 Seconds\nAlways On");
    lv_demo_smartwatch_settings_add_dropdown(0x0004, "Screen Rotation", &img_screen_rotate_icon, "Default\n90\n180\n270");
    lv_demo_smartwatch_settings_add_toggle(0x0005, "Show Alerts", &img_alert_icon, false);
    about_label = lv_demo_smartwatch_settings_add_label(0x0006, "LVGL Demo\nSmart Watch UI\n11:22:33:44:55:66",
                                                        &img_info_icon);
    lv_demo_smartwatch_settings_add_label(0x0007, "Made with love\nin Kenya\nusing LVGL", &img_kenya_icon);

    lv_obj_add_event_cb(settings_list, lv_demo_smartwatch_scroll_event, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(settings_screen, settings_screen_event_cb, LV_EVENT_ALL, NULL);
}

#endif /*LV_USE_DEMO_SMARTWATCH*/
