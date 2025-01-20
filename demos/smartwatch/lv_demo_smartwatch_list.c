/**
 * @file lv_demo_smartwatch_list.c
 * App list screen layout & functions. Contains a list of apps that can be opened/launched.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_list.h"

/*********************
 *      DEFINES
 *********************/

#define MAX_APPS 15

/**********************
 *      TYPEDEFS
 **********************/

/**
 * app object details
 */
typedef struct {
    const char * name;              /**< name of the app. Shown on the app list */
    const lv_image_dsc_t * icon;    /**< icon of the app. Shown on the app list */
    lv_obj_t ** main;               /**< root object of the app */
} app_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void create_screen_list(void);
static void add_external_app(const char * app_name, int index, const void * img);
static void add_app_list(const char * app_name, int index, const void * img);

static void app_list_clicked_event_cb(lv_event_t * e);
static void app_list_clicked_external_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * app_list_screen;
static lv_obj_t * app_list;

static app_t apps[MAX_APPS];
static uint32_t num_apps;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_register_app_cb(const char * name, const lv_image_dsc_t * icon, lv_obj_t ** entry)
{
    if(num_apps >= MAX_APPS) {
        LV_LOG_WARN("Maximum apps reached. Cannot add more apps");
        return;
    }
    apps[num_apps].name = name;
    apps[num_apps].icon = icon;
    apps[num_apps].main = entry;
    add_external_app(name, num_apps, icon);
    num_apps++;
}

void lv_demo_smartwatch_list_create(lv_obj_t * parent)
{

    app_list_screen = lv_tileview_add_tile(parent, 1, 1, LV_DIR_LEFT);
    lv_obj_remove_flag(app_list_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(app_list_screen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(app_list_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    create_screen_list();

}

void lv_demo_smartwatch_list_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay)
{
    lv_demo_smartwatch_home_load(anim_type, time, delay);
}

void lv_demo_smartwatch_app_close(void)
{
    lv_demo_smartwatch_home_load(LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0);
}

void lv_demo_smartwatch_app_list_loading(void)
{
    lv_obj_scroll_by(app_list, 0, 1, LV_ANIM_OFF);
    lv_obj_scroll_by(app_list, 0, -1, LV_ANIM_OFF);

    lv_obj_set_scrollbar_mode(app_list, lv_demo_smartwatch_get_scrollbar_mode());
}

lv_obj_t * lv_demo_smartwatch_get_tile_app_list(void)
{
    return app_list_screen;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void app_list_clicked_external_event_cb(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    uint32_t index = (uint32_t)(intptr_t)lv_event_get_user_data(e);

    lv_obj_t * active_screen = lv_screen_active();
    if(active_screen != lv_demo_smartwatch_get_tileview()) {
        /* event was triggered but the current screen is no longer active */
        return;
    }
    if(event_code == LV_EVENT_CLICKED) {
        if(index >= num_apps) {
            LV_LOG_WARN("Clicked app out of bounds. Cannot launch app");
            lv_demo_smartwatch_show_dialog("App Error", "App list out of bounds");
            return;
        }
        if(*apps[index].main == NULL) {
            LV_LOG_WARN("Main object of clicked app is null. Cannot launch app");
            lv_demo_smartwatch_show_dialog("App Error", "Unable to load main object");
            return;
        }
        lv_screen_load_anim(*apps[index].main, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, false);
    }
}

static void app_list_clicked_event_cb(lv_event_t * e)
{
    lv_obj_t * active_screen = lv_screen_active();
    if(active_screen != lv_demo_smartwatch_get_tileview()) {
        /* event was triggered but the current screen is no longer active */
        return;
    }
    int index = (int)(intptr_t)lv_event_get_user_data(e);
    lv_demo_smartwatch_set_load_app_list(true); /* flag was open from app list */
    switch(index) {
        case 0:
            lv_demo_smartwatch_notifications_load(LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0);
            break;
        case 1:
            lv_demo_smartwatch_weather_load(LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0);
            break;
        case 2:
            lv_demo_smartwatch_settings_load(LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0);
            break;
        case 3:
            lv_demo_smartwatch_qr_load(LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 500, 0);
            break;
    }
}

static void add_external_app(const char * app_name, int index, const void * img)
{
    lv_obj_t * panel = lv_obj_create(app_list);
    lv_obj_set_width(panel, lv_pct(90));
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

    lv_obj_t * label = lv_label_create(panel);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_x(label, 60);
    lv_obj_set_y(label, 3);
    lv_obj_set_align(label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(label, app_name);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(panel, app_list_clicked_external_event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)index);
}

static void add_app_list(const char * app_name, int index, const void * img)
{
    lv_obj_t * panel = lv_obj_create(app_list);
    lv_obj_set_width(panel, lv_pct(90));
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

    lv_obj_t * label = lv_label_create(panel);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_x(label, 60);
    lv_obj_set_y(label, 3);
    lv_obj_set_align(label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(label, app_name);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(panel, app_list_clicked_event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)index);
}

static void create_screen_list(void)
{
    app_list = lv_obj_create(app_list_screen);
    lv_obj_set_width(app_list, lv_pct(100));
    lv_obj_set_height(app_list, lv_pct(100));
    lv_obj_set_align(app_list, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(app_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(app_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(app_list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(app_list, LV_DIR_VER);
    lv_obj_set_style_radius(app_list, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(app_list, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(app_list, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(app_list, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(app_list, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(app_list, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(app_list, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(app_list, 70, LV_PART_MAIN | LV_STATE_DEFAULT);

    add_app_list("Notifications", 0, &img_notifications_app_icon);
    add_app_list("Weather", 1, &img_weather_app_icon);
    add_app_list("QR Codes", 3, &img_qr_icon);
    add_app_list("Settings", 2, &img_general_settings_icon);

    lv_obj_add_event_cb(app_list, lv_demo_smartwatch_scroll_event, LV_EVENT_ALL, NULL);
}

#endif /*LV_USE_DEMO_SMARTWATCH*/
