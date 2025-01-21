/**
 * @file lv_demo_smartwatch.h
 *
 */

#ifndef LV_DEMO_SMARTWATCH_H
#define LV_DEMO_SMARTWATCH_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"


#if LV_USE_DEMO_SMARTWATCH

/*Testing of dependencies*/
#if LV_FONT_MONTSERRAT_16 == 0
#error "LV_FONT_MONTSERRAT_16 text support is required. Enable it in lv_conf.h (LV_FONT_MONTSERRAT_16 1)"
#endif
#if LV_FONT_MONTSERRAT_20 == 0
#error "LV_FONT_MONTSERRAT_20 text support is required. Enable it in lv_conf.h (LV_FONT_MONTSERRAT_20 1)"
#endif
#if LV_FONT_MONTSERRAT_46 == 0
#error "LV_FONT_MONTSERRAT_46 text support is required. Enable it in lv_conf.h (LV_FONT_MONTSERRAT_46 1)"
#endif
#if LV_FONT_MONTSERRAT_48 == 0
#error "LV_FONT_MONTSERRAT_48 text support is required. Enable it in lv_conf.h (LV_FONT_MONTSERRAT_48 1)"
#endif
#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN && LV_MEM_SIZE < 150 * 1024
#error "It's recommended to have at least 150kB RAM for the smartwatch demo, increase LV_MEM_SIZE"
#endif

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

typedef void (*lv_smartwatch_music_control_cb_t)(uint16_t);

typedef void (*lv_smartwatch_notification_click_cb_t)(uint16_t);

typedef void (*lv_smartwatch_settings_change_cb_t)(uint16_t, uint64_t);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a smartwatch demo.
 */
void lv_demo_smartwatch(void);

/**
 * Function to show dialogs
 * @param title     dialog title
 * @param message   dialog message
 */
void lv_demo_smartwatch_show_dialog(const char * title, const char * message);

/**
 * Load the home screen
 * @param anim_type   screen load animation to use
 * @param time        animation time
 * @param delay       delay time before loading the screen
 */
void lv_demo_smartwatch_home_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * Get the tileview object
 * @return pointer to the tileview object
 */
lv_obj_t * lv_demo_smartwatch_get_tileview(void);

/**
 * Get the home tile object that holds the watchfaces
 * @return pointer to the home tile object
 */
lv_obj_t * lv_demo_smartwatch_get_tile_home(void);

/**
 * Update the position of animated analog seconds
 * @param second    the current seconds position
 */
void lv_demo_smartwatch_face_update_seconds(int second);

/**
 * External apps call this function to return to the app list
 */
void lv_demo_smartwatch_app_close(void);

/**
 * Function to load watchface
 * @param index     position of the watchface
 * @return whether success or not (unsuccessful means watchface at index was not found)
 */
bool lv_demo_smartwatch_face_load(uint16_t index);

/**
 * Load the home screen watchface view
 */
void lv_demo_smartwatch_load_home_watchface(void);

/**
 * Update time on the default watchface
 * @param minute    minute
 * @param hour      hour
 * @param am_pm     am/pm label
 * @param date      day of month
 * @param month     month label
 * @param weekday   day of the week
 */
void lv_demo_smartwatch_home_set_time(int minute, int hour, const char * am_pm, int date, const char * month,
                                      const char * weekday);

/**
 * Function to send watchface events
 * @param e event
 */
void lv_demo_smartwatch_face_events_cb(lv_event_t * e);

/**
 * Register external watchfaces
 * @param name          name of the app
 * @param preview       preview image of the watchface (180x180)
 * @param watchface     pointer to main object of the watchface
 * @param seconds       pointer to analog seconds object. Used for smooth analog seconds animation
 */
void lv_demo_smartwatch_register_watchface_cb(const char * name, const lv_image_dsc_t * preview, lv_obj_t ** watchface,
                                              lv_obj_t ** seconds);

/**
 * Get pointer to the current watchface root object
 * @return pointer to root object
 */
lv_obj_t * lv_demo_smartwatch_face_get_current(void);

/**
 * Register external apps
 * @param name    name of the app
 * @param icon    launcher icon of the app (64x64)
 * @param entry   pointer to main object of the app
 */
void lv_demo_smartwatch_register_app_cb(const char * name, const lv_image_dsc_t * icon, lv_obj_t ** entry);

/**
 * Clear the notification list
 */
void lv_demo_smartwatch_clear_notifications(void);

/**
 * Add a notification to the list
 * @param app_id    id of the app sending the notification (sets icon from the inbuilt icon list)
 * @param message   the notification message preview
 * @param index     position of the message that will be returned when clicked
 */
void lv_demo_smartwatch_add_notification(int app_id, const char * message, int index);

/**
 * Show a specific notification when in the notification screen
 * @param app_id    id of the app sending the notification (sets icon from the inbuilt icon list)
 * @param message   the notification message content
 * @param time      the notification time
 */
void lv_demo_smartwatch_show_notification(int app_id, const char * message, const char * time);

/**
 * Set the callback function when a notification is clicked
 * @param cb callback function
 */
void lv_demo_smartwatch_set_notification_click_cb(lv_smartwatch_notification_click_cb_t cb);

/**
 * Set the callback function for settings actions
 * @param cb    callback function
 */
void lv_demo_smartwatch_set_settings_actions_cb(lv_smartwatch_settings_change_cb_t cb);

/**
 * Set the callback function for music control
 * @param cb callback function
 */
void lv_demo_smartwatch_set_music_control_cb(lv_smartwatch_music_control_cb_t cb);

/**
 * Set the current weather
 * @param temp      current temp in °C
 * @param icon      weather icon to use (0-7)
 * @param day       whether day or night (changes the icon)
 * @param hour      last update hour of the weather
 * @param minute    last update minute of the weather
 * @param city      the current city of the weather
 */
void lv_demo_smartwatch_set_weather(int temp, uint8_t icon, bool day, int hour, int minute, const char * city);

/**
 * Clear daily forecast list
 */
void lv_demo_smartwatch_weather_daily_clear(void);

/**
 * Clear hourly forecast list
 */
void lv_demo_smartwatch_weather_hourly_clear(void);

/**
 * Add daily forecast weather to the list
 * @param day    day of the week (0-7)
 * @param temp   temp in °C
 * @param id     weather icon to use (0-7)
 */
void lv_demo_smartwatch_weather_add_daily(int day, int temp, int id);

/**
 * Add hourly forecast weather to the list
 * @param hour      hour oof the day (0-23)
 * @param id        weather icon to use (0-7)
 * @param temp      temp in °C
 * @param humidity  relative humidity %
 * @param wind      wind speed in km/hr
 * @param uv        uv index
 * @param info      item type (use true when adding forecast details, false adds it as a title row)
 */
void lv_demo_smartwatch_weather_add_hourly(int hour, int id, int temp, int humidity, int wind, int uv, bool info);

/**
 * Clear the qr codes in the list
 */
void lv_demo_smartwatch_qr_list_clear(void);

/**
 * Add qr codes to the list
 * @param id        determines the icon and name of the qr from inbuilt list. Value (0-9)
 * @param link      link to be added as a qr code
 */
void lv_demo_smartwatch_qr_list_add(uint8_t id, const char * link);

/**
 * Add a slider to the main settings list
 * @param id        unique id to track changes (recommended > 0x000F)
 * @param name      title for the setting
 * @param img       icon for the setting
 * @param value     default value for the slider
 * @param min       min value for the slider
 * @param max       max value for the slider
 * @return pointer to slider object
 */
lv_obj_t * lv_demo_smartwatch_settings_add_slider(uint16_t id, const char * name, const lv_img_dsc_t * img,
                                                  int32_t value,
                                                  int32_t min, int32_t max);

/**
 * Add a toggle switch to the main settings list
 * @param id        unique id to track changes (recommended > 0x000F)
 * @param name      title for the setting
 * @param img       icon for the setting
 * @param state     default state for the switch
 * @return pointer to switch object
 */
lv_obj_t * lv_demo_smartwatch_settings_add_toggle(uint16_t id, const char * name, const lv_img_dsc_t * img, bool state);

/**
 * Add a dropdowwn to the main settings list
 * @param id        unique id to track changes (recommended > 0x000F)
 * @param name      title for the setting
 * @param img       icon for the setting
 * @param options   the dropdown options
 * @return pointer to dropdown object
 */
lv_obj_t * lv_demo_smartwatch_settings_add_dropdown(uint16_t id, const char * name, const lv_img_dsc_t * img,
                                                    const char * options);

/**
 * Add a label to the main settings list
 * @param id    unique id to track changes (recommended > 0x000F)
 * @param name  text for the label
 * @param img   icon for the label
 * @return pointer to label object
 */
lv_obj_t * lv_demo_smartwatch_settings_add_label(uint16_t id, const char * name, const lv_img_dsc_t * img);

/**
 * Set the default scrollbar mode for the smartwatch demo
 * @param mode  scrollbar mode
 */
void lv_demo_smartwatch_set_default_scrollbar_mode(lv_scrollbar_mode_t mode);

/**
 * Set the default brightness for the smartwatch demo settings
 * @param brightness    default brightness value
 */
void lv_demo_smartwatch_set_default_brightness(uint8_t brightness);

/**
 * Set the default timeout for the smartwatch demo settings
 * @param timeout   default timeout value (0-4)
 */
void lv_demo_smartwatch_set_default_timeout(uint8_t timeout);

/**
 * Set the default rotation for the smartwatch demo settings
 * @param rotation  default rotation value (0-3)
 */
void lv_demo_smartwatch_set_default_rotation(uint8_t rotation);

/**
 * Set the default circular scroll for the smartwatch demo settings
 * @param enabled   whether circular scroll is enabled
 */
void lv_demo_smartwatch_set_default_circular_scroll(bool enabled);

/**
 * Set the default alert state for the smartwatch demo settings
 * @param enabled   whether alerts are enabled
 */
void lv_demo_smartwatch_set_default_alert_state(bool enabled);

/**
 * Set the default hints state for the smartwatch demo settings
 * @param enabled   whether hints are enabled
 */
void lv_demo_smartwatch_set_default_hints_state(bool enabled);

/**
 * Get the hint state
 * @return hint state
 */
bool lv_demo_smartwatch_get_scroll_hint(void);

/**
 * Set the default about info for the smartwatch demo settings
 * @param info  about info text
 */
void lv_demo_smartwatch_set_default_about_info(const char * info);

/**
 * Show the scroll direction hint
 * @param dir   direction of available scroll
 */
void lv_demo_smartwatch_show_scroll_hint(lv_dir_t dir);

/**
 * Show the scroll direction hint
 * @param state whether to show the hint
 */
void lv_demo_smartwatch_show_home_hint(bool state);

/**
 * Set whether to show scroll hints
 * @param state whether to show scroll hints
 */
void lv_demo_smartwatch_set_scroll_hint(bool state);

/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_SMARTWATCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_SMARTWATCH_H*/
