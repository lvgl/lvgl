/**
 * @file lv_demo_smartwatch_home.c
 * Home screen layout & functions. Basically the watchface for the smartwatch.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_home.h"

/*********************
 *      DEFINES
 *********************/

#define MAX_FACES 15

/**********************
 *      TYPEDEFS
 **********************/
/**
 * watchface object
 */
typedef struct {
    const char * name;             /**< name of the watchface, shown in the watchface selector */
    const lv_image_dsc_t * preview; /**< preview of the watchface, shown in the watchface selector */
    lv_obj_t ** watchface;         /**< pointer to watchface root object */
    lv_obj_t ** seconds;           /**< pointer to analog seconds object in the watchface, used for smooth animation */
} watchface_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void create_screen_home(void);

static void create_home_hints(void);

static void lv_demo_smartwatch_add_watchface(const char * name, const lv_image_dsc_t * src, int index);
static void clock_screen_event_cb(lv_event_t * e);
static void animate_analog_seconds(lv_obj_t * target);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * face_park;
static lv_obj_t * home_panel;
static lv_obj_t * clock_screen;
static lv_obj_t * hour_label;
static lv_obj_t * minute_label;
static lv_obj_t * date_label;
static lv_obj_t * weather_icon;
static lv_obj_t * weather_temperature;
static lv_obj_t * weekday_label;
static lv_obj_t * am_pm_label;

static lv_obj_t * alert_panel;
static lv_obj_t * alert_icon;
static lv_obj_t * alert_text;

static lv_obj_t * face_select;

static watchface_t faces[MAX_FACES];
static uint32_t num_faces;
static uint32_t current_face_index;

static lv_obj_t * hint_panel;
static lv_obj_t * hint_up;
static lv_obj_t * hint_down;
static lv_obj_t * hint_left;
static lv_obj_t * hint_right;

static lv_anim_t seconds_animation;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_register_watchface_cb(const char * name, const lv_image_dsc_t * preview, lv_obj_t ** watchface,
                                              lv_obj_t ** seconds)
{
    if(num_faces >= MAX_FACES) {
        LV_LOG_WARN("Maximum watchfaces reached. Cannot add more watchfaces");
        return;
    }
    faces[num_faces].name = name;
    faces[num_faces].preview = preview;
    faces[num_faces].watchface = watchface;
    faces[num_faces].seconds = seconds;
    lv_demo_smartwatch_add_watchface(name, preview, num_faces);
    num_faces++;
}

void lv_demo_smartwatch_home_create(lv_obj_t * parent)
{

    home_panel = lv_tileview_add_tile(parent, 0, 1, LV_DIR_TOP | LV_DIR_RIGHT);
    lv_obj_remove_flag(home_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(home_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(home_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    face_park = lv_obj_create(NULL); /* parent of inactive watchfaces */

    create_screen_home();

    create_home_hints();

    lv_demo_smartwatch_register_watchface_cb("Default", &img_digital_preview, &clock_screen, NULL);
}

void lv_demo_smartwatch_face_events_cb(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
        lv_demo_smartwatch_set_load_app_list(false); /* flag was not open from app list */
        lv_demo_smartwatch_notifications_load(LV_SCR_LOAD_ANIM_OVER_RIGHT, 500, 0);
    }

    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_TOP) {
        lv_demo_smartwatch_set_load_app_list(false); /* flag was not open from app list */
        lv_demo_smartwatch_weather_load(LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0);
    }

    if(event_code == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_disp_t * display = lv_display_get_default();
        lv_obj_t * active_screen = lv_display_get_screen_active(display);
        if(active_screen != lv_demo_smartwatch_get_tileview()) {
            /* event was triggered but the current screen is no longer active */
            return;
        }
        lv_screen_load_anim(face_select, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, false);
    }


}

void lv_demo_smartwatch_face_selected_cb(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);

    if(target == lv_demo_smartwatch_get_tileview()) {
        /* the event might be triggered after watchface has been selected, return immediately */
        return;
    }

    if(event_code == LV_EVENT_CLICKED) {
        uint32_t index = (uint32_t)(intptr_t)lv_event_get_user_data(e);

        if(index >= num_faces) {
            LV_LOG_WARN("Selected watchface index exceeds available faces.");
            lv_demo_smartwatch_load_home_watchface();
            return;
        }
        lv_obj_scroll_to_view(lv_obj_get_child(face_select, index), LV_ANIM_OFF);

        lv_demo_smartwatch_face_load(index);
    }
}

void lv_demo_smartwatch_home_set_weather(int temp, const lv_img_dsc_t * icon)
{
    lv_label_set_text_fmt(weather_temperature, "%d°C", temp);
    lv_image_set_src(weather_icon, icon);
}

void lv_demo_smartwatch_home_set_time(int minute, int hour, const char * am_pm, int date, const char * month,
                                      const char * weekday)
{
    lv_label_set_text_fmt(hour_label, "%02d", hour);
    lv_label_set_text_fmt(weekday_label, "%s", weekday);
    lv_label_set_text_fmt(minute_label, "%02d", minute);
    lv_label_set_text_fmt(date_label, "%02d\n%s", date, month);
    lv_label_set_text(am_pm_label, am_pm);
}

lv_obj_t * lv_demo_smartwatch_get_tile_home(void)
{
    return home_panel;
}

lv_obj_t * lv_demo_smartwatch_face_get_current(void)
{
    return *faces[current_face_index].watchface;
}

void lv_demo_smartwatch_face_update_seconds(int second)
{
    lv_anim_custom_delete(&seconds_animation, NULL);

    for(int i = 0; (uint32_t)i < num_faces; i++) {
        if(faces[i].seconds != NULL) {
            lv_image_set_rotation(*faces[i].seconds, second * 60);
            animate_analog_seconds(*faces[i].seconds);
        }
    }
}

bool lv_demo_smartwatch_face_load(uint16_t index)
{
    LV_LOG_WARN("Loading watchface at index %d", index);

    if(index >= num_faces) {
        LV_LOG_WARN("Cannot load watchface. Selected watchface index exceeds available faces.");
        return false;
    }

    if(*faces[index].watchface == NULL) {
        LV_LOG_WARN("Cannot load watchface, the object is null");
        return false;
    }
    lv_obj_scroll_to_view(lv_obj_get_child(face_select, index), LV_ANIM_OFF);
    if(current_face_index != index) {
        current_face_index = index;

        /* remove all objects in the home panel by assigning them a new parent */
        for(uint32_t i = 0; i < lv_obj_get_child_count(home_panel); i++) {
            lv_obj_t * current = lv_obj_get_child(home_panel, i);
            if(current != NULL) {
                lv_obj_set_parent(current, face_park);
            }
        }
        /* set the selected watchface parent */
        lv_obj_set_parent((lv_obj_t *)*faces[index].watchface, home_panel);

        lv_obj_set_parent(hint_panel, face_park);
    }

    lv_demo_smartwatch_load_home_watchface();

    return true;
}

void lv_demo_smartwatch_show_home_hint(bool state)
{

    lv_obj_set_parent(hint_panel, home_panel);

    if(state && lv_demo_smartwatch_get_scroll_hint()) {
        lv_obj_remove_flag(hint_panel, LV_OBJ_FLAG_HIDDEN);
    }
    else {
        lv_obj_add_flag(hint_panel, LV_OBJ_FLAG_HIDDEN);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void animate_analog_seconds(lv_obj_t * target)
{
    lv_anim_init(&seconds_animation);
    lv_anim_set_duration(&seconds_animation, 60000);
    lv_anim_set_values(&seconds_animation, lv_image_get_rotation(target), lv_image_get_rotation(target) + 3600);
    lv_anim_set_var(&seconds_animation, target);
    lv_anim_set_exec_cb(&seconds_animation, (lv_anim_exec_xcb_t)lv_image_set_rotation);
    lv_anim_set_repeat_count(&seconds_animation, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&seconds_animation);
}

static void clock_screen_event_cb(lv_event_t * e)
{
    lv_demo_smartwatch_face_events_cb(e);
}

static void lv_demo_smartwatch_add_watchface(const char * name, const lv_image_dsc_t * src, int index)
{
    lv_obj_t * face_item = lv_obj_create(face_select);
    lv_obj_set_width(face_item, 160);
    lv_obj_set_height(face_item, 180);
    lv_obj_set_align(face_item, LV_ALIGN_CENTER);
    lv_obj_remove_flag(face_item, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(face_item, LV_OBJ_FLAG_SCROLL_ONE);
    lv_obj_set_style_radius(face_item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(face_item, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(face_item, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(face_item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(face_item, lv_color_hex(0x142ABC), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(face_item, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(face_item, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(face_item, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(face_item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(face_item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(face_item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(face_item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * face_preview = lv_image_create(face_item);
    lv_image_set_src(face_preview, src);
    lv_obj_set_width(face_preview, LV_SIZE_CONTENT);
    lv_obj_set_height(face_preview, LV_SIZE_CONTENT);
    lv_obj_set_align(face_preview, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(face_preview, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(face_preview, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * face_label = lv_label_create(face_item);
    lv_obj_set_width(face_label, 160);
    lv_obj_set_height(face_label, LV_SIZE_CONTENT);
    lv_obj_set_align(face_label, LV_ALIGN_BOTTOM_MID);
    lv_label_set_long_mode(face_label, LV_LABEL_LONG_DOT);
    lv_label_set_text(face_label, name);
    lv_obj_set_style_text_align(face_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(face_label, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(face_item, lv_demo_smartwatch_face_selected_cb, LV_EVENT_ALL, (void *)(intptr_t)index);
}

static void create_screen_home(void)
{
    clock_screen = lv_obj_create(home_panel);
    lv_obj_set_width(clock_screen, lv_pct(100));
    lv_obj_set_height(clock_screen, lv_pct(100));
    lv_obj_remove_flag(clock_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(clock_screen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(clock_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(clock_screen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(clock_screen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    hour_label = lv_label_create(clock_screen);
    lv_obj_set_width(hour_label, 151);
    lv_obj_set_height(hour_label, LV_SIZE_CONTENT);
    lv_obj_set_x(hour_label, -89);
    lv_obj_set_y(hour_label, -25);
    lv_obj_set_align(hour_label, LV_ALIGN_CENTER);
    lv_label_set_text(hour_label, "20");
    lv_obj_set_style_text_align(hour_label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(hour_label, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);

    minute_label = lv_label_create(clock_screen);
    lv_obj_set_width(minute_label, LV_SIZE_CONTENT);
    lv_obj_set_height(minute_label, LV_SIZE_CONTENT);
    lv_obj_set_x(minute_label, 25);
    lv_obj_set_y(minute_label, 40);
    lv_obj_set_align(minute_label, LV_ALIGN_CENTER);
    lv_label_set_text(minute_label, "28");
    lv_obj_set_style_text_font(minute_label, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);

    date_label = lv_label_create(clock_screen);
    lv_obj_set_width(date_label, 113);
    lv_obj_set_height(date_label, LV_SIZE_CONTENT);
    lv_obj_set_x(date_label, 59);
    lv_obj_set_y(date_label, -22);
    lv_obj_set_align(date_label, LV_ALIGN_CENTER);
    lv_label_set_long_mode(date_label, LV_LABEL_LONG_CLIP);
    lv_label_set_text(date_label, "08\nJuly");
    lv_obj_set_style_text_font(date_label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    weather_icon = lv_image_create(clock_screen);
    lv_image_set_src(weather_icon, &img_weather_day_6);
    lv_obj_set_width(weather_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(weather_icon, LV_SIZE_CONTENT);
    lv_obj_set_x(weather_icon, -43);
    lv_obj_set_y(weather_icon, 20);
    lv_obj_set_align(weather_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(weather_icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(weather_icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(weather_icon, 150);

    weather_temperature = lv_label_create(clock_screen);
    lv_obj_set_width(weather_temperature, LV_SIZE_CONTENT);
    lv_obj_set_height(weather_temperature, LV_SIZE_CONTENT);
    lv_obj_set_x(weather_temperature, -41);
    lv_obj_set_y(weather_temperature, 50);
    lv_obj_set_align(weather_temperature, LV_ALIGN_CENTER);
    lv_label_set_text(weather_temperature, "--°C");
    lv_obj_set_style_text_font(weather_temperature, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    weekday_label = lv_label_create(clock_screen);
    lv_obj_set_width(weekday_label, LV_SIZE_CONTENT);
    lv_obj_set_height(weekday_label, LV_SIZE_CONTENT);
    lv_obj_set_x(weekday_label, 0);
    lv_obj_set_y(weekday_label, -70);
    lv_obj_set_align(weekday_label, LV_ALIGN_CENTER);
    lv_label_set_text(weekday_label, "Sunday");
    lv_obj_set_style_text_font(weekday_label, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    am_pm_label = lv_label_create(clock_screen);
    lv_obj_set_width(am_pm_label, LV_SIZE_CONTENT);
    lv_obj_set_height(am_pm_label, LV_SIZE_CONTENT);
    lv_obj_set_x(am_pm_label, 12);
    lv_obj_set_y(am_pm_label, 80);
    lv_obj_set_align(am_pm_label, LV_ALIGN_CENTER);
    lv_label_set_text(am_pm_label, "PM");
    lv_obj_set_style_text_font(am_pm_label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    alert_panel = lv_obj_create(clock_screen);
    lv_obj_set_width(alert_panel, 200);
    lv_obj_set_height(alert_panel, 55);
    lv_obj_set_align(alert_panel, LV_ALIGN_CENTER);
    lv_obj_add_flag(alert_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(alert_panel, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(alert_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(alert_panel, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(alert_panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(alert_panel, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(alert_panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(alert_panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(alert_panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(alert_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(alert_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    alert_icon = lv_image_create(alert_panel);
    lv_image_set_src(alert_icon, &img_wechat_icon);
    lv_obj_set_width(alert_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(alert_icon, LV_SIZE_CONTENT);
    lv_obj_set_align(alert_icon, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(alert_icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(alert_icon, LV_OBJ_FLAG_SCROLLABLE);

    alert_text = lv_label_create(alert_panel);
    lv_obj_set_width(alert_text, 142);
    lv_obj_set_height(alert_text, 40);
    lv_obj_set_x(alert_text, 39);
    lv_obj_set_y(alert_text, 0);
    lv_obj_set_align(alert_text, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(alert_text, LV_LABEL_LONG_DOT);
    lv_label_set_text(alert_text, "this is a notification example");
    lv_obj_set_style_text_font(alert_text, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(clock_screen, clock_screen_event_cb, LV_EVENT_ALL, NULL);

    face_select = lv_obj_create(NULL);
    lv_obj_set_width(face_select, 240);
    lv_obj_set_height(face_select, 240);
    lv_obj_set_align(face_select, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(face_select, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(face_select, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(face_select, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(face_select, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(face_select, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(face_select, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(face_select, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(face_select, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(face_select, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(face_select, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(face_select, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(face_select, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(face_select, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void create_home_hints(void)
{
    hint_panel = lv_obj_create(home_panel);

    lv_obj_set_width(hint_panel, lv_pct(100));
    lv_obj_set_height(hint_panel, lv_pct(100));
    lv_obj_set_align(hint_panel, LV_ALIGN_CENTER);
    lv_obj_remove_flag(hint_panel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(hint_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(hint_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(hint_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(hint_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(hint_panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(hint_panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(hint_panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(hint_panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    hint_up = lv_image_create(hint_panel);
    lv_image_set_src(hint_up, &img_note_icon);
    lv_image_set_scale(hint_up, 200);
    lv_obj_set_width(hint_up, LV_SIZE_CONTENT);
    lv_obj_set_height(hint_up, LV_SIZE_CONTENT);
    lv_obj_set_align(hint_up, LV_ALIGN_TOP_MID);
    lv_obj_remove_flag(hint_up, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(hint_up, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(hint_up, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(hint_up, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(hint_up, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    hint_down = lv_image_create(hint_panel);
    lv_image_set_src(hint_down, &img_cloud_icon);
    lv_image_set_scale(hint_down, 200);
    lv_obj_set_width(hint_down, LV_SIZE_CONTENT);
    lv_obj_set_height(hint_down, LV_SIZE_CONTENT);
    lv_obj_set_align(hint_down, LV_ALIGN_BOTTOM_MID);
    lv_obj_remove_flag(hint_down, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(hint_down, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(hint_down, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(hint_down, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(hint_down, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    hint_left = lv_image_create(hint_panel);
    lv_image_set_src(hint_left, &img_chat_icon);
    lv_image_set_scale(hint_left, 200);
    lv_obj_set_width(hint_left, LV_SIZE_CONTENT);
    lv_obj_set_height(hint_left, LV_SIZE_CONTENT);
    lv_obj_set_align(hint_left, LV_ALIGN_LEFT_MID);
    lv_obj_remove_flag(hint_left, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(hint_left, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(hint_left, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(hint_left, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(hint_left, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    hint_right = lv_image_create(hint_panel);
    lv_image_set_src(hint_right, &img_application_icon);
    lv_image_set_scale(hint_right, 200);
    lv_obj_set_width(hint_right, LV_SIZE_CONTENT);
    lv_obj_set_height(hint_right, LV_SIZE_CONTENT);
    lv_obj_set_align(hint_right, LV_ALIGN_RIGHT_MID);
    lv_obj_remove_flag(hint_right, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(hint_right, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(hint_right, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(hint_right, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(hint_right, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

#endif /*LV_USE_DEMO_SMARTWATCH*/
