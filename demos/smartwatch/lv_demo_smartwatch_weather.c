/**
 * @file lv_demo_smartwatch_weather.c
 * Weather screen layout & functions. Shows the current weather details as well as
 * hourly and daily forecasts.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_weather.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void create_screen_weather(void);
static void weather_screen_event_cb(lv_event_t * e);

static void set_weather_icon(lv_obj_t * obj, uint8_t id, bool day);
static const lv_img_dsc_t * get_weather_icon(uint8_t id, bool day);
/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * weather_screen;
static lv_obj_t * weather_panel;
static lv_obj_t * weather_city;
static lv_obj_t * weather_current_icon;
static lv_obj_t * weather_current_temp;
static lv_obj_t * weather_update_time;
static lv_obj_t * weather_daily_panel;
static lv_obj_t * weather_hourly_panel;
static lv_obj_t * weather_forecast_daily;
static lv_obj_t * weather_forecast_hourly;
static lv_obj_t * info_daily;
static lv_obj_t * info_hourly;

static const lv_image_dsc_t * weather_day_icons[] = {
    &img_weather_day_0,
    &img_weather_day_1,
    &img_weather_day_2,
    &img_weather_day_3,
    &img_weather_day_4,
    &img_weather_day_5,
    &img_weather_day_6,
    &img_weather_day_7
};

static const lv_image_dsc_t * weather_night_icons[] = {
    &img_weather_night_0,
    &img_weather_night_1,
    &img_weather_night_2,
    &img_weather_night_3,
    &img_weather_night_4,
    &img_weather_night_5,
    &img_weather_night_6,
    &img_weather_night_7,
};

static const char * week_days[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_weather_create(void)
{
    create_screen_weather();

    /* add demo weather parameters */
    lv_demo_smartwatch_set_weather(22, 0, true, 14, 24, "Nairobi");

    lv_demo_smartwatch_weather_daily_clear();
    for(int i = 0; i < 7; i++) {
        lv_demo_smartwatch_weather_add_daily(i, 20 - 1, i);
    }

    lv_demo_smartwatch_weather_hourly_clear();
    lv_demo_smartwatch_weather_add_hourly(0, 1, 0, 0, 0, 0, true);
    for(int h = 0; h < 4; h++) {
        lv_demo_smartwatch_weather_add_hourly(h * 6, h, 20 + h, 50 - h, 30 + h, h, false);
    }
}

void lv_demo_smartwatch_weather_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay)
{
    lv_screen_load_anim(weather_screen, anim_type, time, delay, false);
}

void lv_demo_smartwatch_set_weather(int temp, uint8_t icon, bool day, int hour, int minute, const char * city)
{
    lv_label_set_text(weather_city, city);
    lv_label_set_text_fmt(weather_update_time, "Updated at\n%02d:%02d", hour, minute);
    lv_label_set_text_fmt(weather_current_temp, "%d°C", temp);

    set_weather_icon(weather_current_icon, icon, day);

    /* also update weather in the default watchface */
    lv_demo_smartwatch_home_set_weather(temp, get_weather_icon(icon, day));
}

void lv_demo_smartwatch_weather_daily_clear(void)
{
    lv_obj_clean(weather_forecast_daily);
    lv_obj_remove_flag(info_daily, LV_OBJ_FLAG_HIDDEN);
}

void lv_demo_smartwatch_weather_hourly_clear(void)
{
    lv_obj_clean(weather_forecast_hourly);
    lv_obj_remove_flag(info_hourly, LV_OBJ_FLAG_HIDDEN);
}

void lv_demo_smartwatch_weather_add_daily(int day, int temp, int id)
{
    lv_obj_t * panel = lv_obj_create(weather_forecast_daily);
    lv_obj_set_width(panel, lv_pct(85));
    lv_obj_set_height(panel, 40);
    lv_obj_set_align(panel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(panel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(panel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * label_day = lv_label_create(panel);
    lv_obj_set_width(label_day, 68);
    lv_obj_set_height(label_day, LV_SIZE_CONTENT);
    lv_obj_set_x(label_day, -56);
    lv_obj_set_y(label_day, 3);
    lv_obj_set_align(label_day, LV_ALIGN_CENTER);
    lv_label_set_long_mode(label_day, LV_LABEL_LONG_CLIP);
    lv_label_set_text(label_day, week_days[day % 7]);
    lv_obj_set_style_text_font(label_day, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * icon = lv_image_create(panel);
    lv_obj_set_width(icon, LV_SIZE_CONTENT);
    lv_obj_set_height(icon, LV_SIZE_CONTENT);
    lv_obj_set_align(icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(icon, 150);
    set_weather_icon(icon, id, true);

    lv_obj_t * label = lv_label_create(panel);
    lv_obj_set_width(label, 58);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_x(label, 68);
    lv_obj_set_y(label, 3);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_label_set_text_fmt(label, "%d°C", temp);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_flag(info_daily, LV_OBJ_FLAG_HIDDEN);
}

void lv_demo_smartwatch_weather_add_hourly(int hour, int id, int temp, int humidity, int wind, int uv, bool info)
{
    lv_obj_t * panel = lv_obj_create(weather_forecast_hourly);
    lv_obj_set_width(panel, 100);
    lv_obj_set_height(panel, 184);
    lv_obj_set_align(panel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(panel, info ? 0 : 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * hour_label = lv_label_create(panel);
    lv_obj_set_width(hour_label, 80);
    lv_obj_set_height(hour_label, LV_SIZE_CONTENT);
    lv_obj_set_align(hour_label, LV_ALIGN_CENTER);
    lv_label_set_text_fmt(hour_label, info ? "Hour" : "%02d:00", hour);
    lv_obj_set_style_text_align(hour_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(hour_label, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(hour_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(hour_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(hour_label, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(hour_label, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * hour_icon = lv_image_create(panel);
    lv_obj_set_width(hour_icon, 40);
    lv_obj_set_height(hour_icon, 40);
    lv_obj_set_align(hour_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(hour_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(hour_icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(hour_icon, 140);
    set_weather_icon(hour_icon, id, true);

    lv_obj_t * temp_label = lv_label_create(panel);
    lv_obj_set_width(temp_label, LV_SIZE_CONTENT);
    lv_obj_set_height(temp_label, LV_SIZE_CONTENT);
    lv_obj_set_align(temp_label, LV_ALIGN_CENTER);
    lv_label_set_text_fmt(temp_label, info ? "Temperature" : "%d°C", temp);
    lv_obj_set_style_text_font(temp_label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * humidity_label = lv_label_create(panel);
    lv_obj_set_width(humidity_label, LV_SIZE_CONTENT);
    lv_obj_set_height(humidity_label, LV_SIZE_CONTENT);
    lv_obj_set_align(humidity_label, LV_ALIGN_CENTER);
    lv_label_set_text_fmt(humidity_label, info ? "Humidity" : "%d%%", humidity);
    lv_obj_set_style_text_font(humidity_label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * wind_label = lv_label_create(panel);
    lv_obj_set_width(wind_label, LV_SIZE_CONTENT);
    lv_obj_set_height(wind_label, LV_SIZE_CONTENT);
    lv_obj_set_align(wind_label, LV_ALIGN_CENTER);
    lv_label_set_text_fmt(wind_label, info ? "Wind Speed" : "%d km/h", wind);
    lv_obj_set_style_text_font(wind_label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * uv_label = lv_label_create(panel);
    lv_obj_set_width(uv_label, LV_SIZE_CONTENT);
    lv_obj_set_height(uv_label, LV_SIZE_CONTENT);
    lv_obj_set_align(uv_label, LV_ALIGN_CENTER);
    lv_label_set_text_fmt(uv_label, info ? "UV" : "%d", uv);
    lv_obj_set_style_text_font(uv_label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_flag(info_hourly, LV_OBJ_FLAG_HIDDEN);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void create_screen_weather(void)
{

    weather_screen = lv_tileview_create(NULL);
    lv_obj_set_scrollbar_mode(weather_screen, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_style_bg_color(weather_screen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(weather_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);


    weather_panel = lv_tileview_add_tile(weather_screen, 0, 0, LV_DIR_BOTTOM | LV_DIR_RIGHT);
    lv_obj_set_width(weather_panel, lv_pct(100));
    lv_obj_set_height(weather_panel, lv_pct(100));
    lv_obj_set_align(weather_panel, LV_ALIGN_CENTER);
    lv_obj_remove_flag(weather_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(weather_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(weather_panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(weather_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(weather_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    weather_city = lv_label_create(weather_panel);
    lv_obj_set_width(weather_city, LV_SIZE_CONTENT);
    lv_obj_set_height(weather_city, LV_SIZE_CONTENT);
    lv_obj_set_x(weather_city, -3);
    lv_obj_set_y(weather_city, -85);
    lv_obj_set_align(weather_city, LV_ALIGN_CENTER);
    lv_label_set_text(weather_city, "");
    lv_obj_set_style_text_font(weather_city, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    weather_current_icon = lv_image_create(weather_panel);
    lv_image_set_src(weather_current_icon, &img_weather_day_0);
    lv_obj_set_width(weather_current_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(weather_current_icon, LV_SIZE_CONTENT);
    lv_obj_set_x(weather_current_icon, 0);
    lv_obj_set_y(weather_current_icon, -29);
    lv_obj_set_align(weather_current_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(weather_current_icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(weather_current_icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(weather_current_icon, 400);

    weather_current_temp = lv_label_create(weather_panel);
    lv_obj_set_width(weather_current_temp, LV_SIZE_CONTENT);
    lv_obj_set_height(weather_current_temp, LV_SIZE_CONTENT);
    lv_obj_set_x(weather_current_temp, 0);
    lv_obj_set_y(weather_current_temp, 44);
    lv_obj_set_align(weather_current_temp, LV_ALIGN_CENTER);
    lv_label_set_text(weather_current_temp, "--°C");
    lv_obj_set_style_text_font(weather_current_temp, &lv_font_montserrat_46, LV_PART_MAIN | LV_STATE_DEFAULT);

    weather_update_time = lv_label_create(weather_panel);
    lv_obj_set_width(weather_update_time, LV_SIZE_CONTENT);
    lv_obj_set_height(weather_update_time, LV_SIZE_CONTENT);
    lv_obj_set_x(weather_update_time, 0);
    lv_obj_set_y(weather_update_time, 97);
    lv_obj_set_align(weather_update_time, LV_ALIGN_CENTER);
    lv_label_set_text(weather_update_time, "unavailable\n--:--");
    lv_obj_set_style_text_align(weather_update_time, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    weather_daily_panel = lv_tileview_add_tile(weather_screen, 1, 0, LV_DIR_LEFT);
    lv_obj_set_width(weather_daily_panel, lv_pct(100));
    lv_obj_set_height(weather_daily_panel, lv_pct(100));

    weather_forecast_daily = lv_obj_create(weather_daily_panel);
    lv_obj_set_width(weather_forecast_daily, lv_pct(100));
    lv_obj_set_height(weather_forecast_daily, lv_pct(100));
    lv_obj_set_align(weather_forecast_daily, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(weather_forecast_daily, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(weather_forecast_daily, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(weather_forecast_daily, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(weather_forecast_daily, LV_DIR_VER);
    lv_obj_set_style_radius(weather_forecast_daily, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(weather_forecast_daily, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(weather_forecast_daily, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(weather_forecast_daily, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(weather_forecast_daily, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(weather_forecast_daily, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(weather_forecast_daily, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(weather_forecast_daily, 100, LV_PART_MAIN | LV_STATE_DEFAULT);

    info_daily = lv_label_create(weather_daily_panel);
    lv_obj_set_width(info_daily, 180);
    lv_obj_set_height(info_daily, LV_SIZE_CONTENT);
    lv_obj_set_align(info_daily, LV_ALIGN_CENTER);
    lv_label_set_text(info_daily,
                      "Weather information has not yet been synced. Connect the device to Chronos app to get weather info. Make sure to enable it in the app settings.");

    weather_hourly_panel = lv_tileview_add_tile(weather_screen, 0, 1, LV_DIR_TOP);
    lv_obj_set_width(weather_hourly_panel, lv_pct(100));
    lv_obj_set_height(weather_hourly_panel, lv_pct(100));

    weather_forecast_hourly = lv_obj_create(weather_hourly_panel);
    lv_obj_set_width(weather_forecast_hourly, lv_pct(100));
    lv_obj_set_height(weather_forecast_hourly, lv_pct(100));
    lv_obj_set_align(weather_forecast_hourly, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(weather_forecast_hourly, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(weather_forecast_hourly, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(weather_forecast_hourly, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(weather_forecast_hourly, LV_DIR_HOR);
    lv_obj_set_style_radius(weather_forecast_hourly, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(weather_forecast_hourly, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(weather_forecast_hourly, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(weather_forecast_hourly, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(weather_forecast_hourly, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(weather_forecast_hourly, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(weather_forecast_hourly, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(weather_forecast_hourly, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(weather_forecast_hourly, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    info_hourly = lv_label_create(weather_hourly_panel);
    lv_obj_set_width(info_hourly, 180);
    lv_obj_set_height(info_hourly, LV_SIZE_CONTENT);
    lv_obj_set_align(info_hourly, LV_ALIGN_CENTER);
    lv_label_set_text(info_hourly,
                      "Weather information has not yet been synced. Connect the device to Chronos app to get weather info. Make sure to enable it in the app settings.");

    lv_obj_add_event_cb(weather_forecast_daily, lv_demo_smartwatch_scroll_event, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(weather_screen, weather_screen_event_cb, LV_EVENT_ALL, NULL);
}

static void weather_screen_event_cb(lv_event_t * e)
{

    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_SCREEN_LOAD_START) {

        lv_obj_scroll_by(weather_forecast_daily, 0, 1, LV_ANIM_OFF);
        lv_obj_scroll_by(weather_forecast_daily, 0, -1, LV_ANIM_OFF);


        lv_obj_set_scrollbar_mode(weather_screen, lv_demo_smartwatch_get_scrollbar_mode());
        lv_obj_set_scrollbar_mode(weather_forecast_daily, lv_demo_smartwatch_get_scrollbar_mode());
        lv_obj_set_scrollbar_mode(weather_forecast_hourly, lv_demo_smartwatch_get_scrollbar_mode());

        lv_tileview_set_tile_by_index(weather_screen, 0, 0, LV_ANIM_OFF);

        if(lv_demo_smartwatch_get_load_app_list()) {
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT | LV_DIR_BOTTOM | LV_DIR_RIGHT);
        }
        else {
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_TOP | LV_DIR_BOTTOM | LV_DIR_RIGHT);
        }

    }

    if(event_code == LV_EVENT_VALUE_CHANGED) {
        if(lv_tileview_get_tile_active(weather_screen) == weather_panel) {
            if(lv_demo_smartwatch_get_load_app_list()) {
                lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT | LV_DIR_BOTTOM | LV_DIR_RIGHT);
            }
            else {
                lv_demo_smartwatch_show_scroll_hint(LV_DIR_TOP | LV_DIR_BOTTOM | LV_DIR_RIGHT);
            }
        }
        else if(lv_tileview_get_tile_active(weather_screen) == weather_daily_panel) {
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT);
        }
        else if(lv_tileview_get_tile_active(weather_screen) == weather_hourly_panel) {
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_TOP);
        }

    }


    if(lv_tileview_get_tile_active(weather_screen) == weather_panel) {
        if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_BOTTOM) {

            if(!lv_demo_smartwatch_get_load_app_list()) {
                lv_demo_smartwatch_home_load(LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 500, 0);
            }
            else {
                LV_LOG_WARN("Swipe right to exit");
                lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT | LV_DIR_BOTTOM | LV_DIR_RIGHT);
            }
        }
        if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
            if(lv_demo_smartwatch_get_load_app_list()) {
                lv_demo_smartwatch_list_load(LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0);
            }
            else {
                LV_LOG_WARN("Swipe down to exit");
                lv_demo_smartwatch_show_scroll_hint(LV_DIR_TOP | LV_DIR_BOTTOM | LV_DIR_RIGHT);
            }
        }
    }

    if(lv_tileview_get_tile_active(weather_screen) == weather_daily_panel) {
        if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) != LV_DIR_RIGHT) {
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT);
        }
    }

    if(lv_tileview_get_tile_active(weather_screen) == weather_hourly_panel) {
        if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) != LV_DIR_BOTTOM) {
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_TOP);
        }
    }


}

static void set_weather_icon(lv_obj_t * obj, uint8_t id, bool day)
{
    lv_image_set_src(obj, day ? weather_day_icons[id % 8] : weather_night_icons[id % 8]);
}

static const lv_img_dsc_t * get_weather_icon(uint8_t id, bool day)
{
    return day ? weather_day_icons[id % 8] : weather_night_icons[id % 8];
}

#endif /*LV_USE_DEMO_SMARTWATCH*/
