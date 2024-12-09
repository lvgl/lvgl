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
#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN && LV_MEM_SIZE < 144 * 1024
    #warning "It's recommended to have at least 144kB RAM for the smartwatch demo"
#endif

/*********************
 *      DEFINES
 *********************/

LV_IMG_DECLARE(img_weather_day_0);      // assets\dy-0.png
LV_IMG_DECLARE(img_weather_day_1);      // assets\dy-1.png
LV_IMG_DECLARE(img_weather_day_2);      // assets\dy-2.png
LV_IMG_DECLARE(img_weather_day_3);      // assets\dy-3.png
LV_IMG_DECLARE(img_weather_day_4);      // assets\dy-4.png
LV_IMG_DECLARE(img_weather_day_5);      // assets\dy-5.png
LV_IMG_DECLARE(img_weather_day_6);      // assets\dy-6.png
LV_IMG_DECLARE(img_weather_day_7);      // assets\dy-7.png

LV_IMG_DECLARE(img_weather_night_0);         // assets\nt-0.png
LV_IMG_DECLARE(img_weather_night_1);         // assets\nt-1.png
LV_IMG_DECLARE(img_weather_night_2);         // assets\nt-2.png
LV_IMG_DECLARE(img_weather_night_3);         // assets\nt-3.png
LV_IMG_DECLARE(img_weather_night_4);         // assets\nt-4.png
LV_IMG_DECLARE(img_weather_night_5);         // assets\nt-5.png
LV_IMG_DECLARE(img_weather_night_6);         // assets\nt-6.png
LV_IMG_DECLARE(img_weather_night_7);         // assets\nt-7.png

LV_IMG_DECLARE(img_facebook_icon);   // assets\facebook.png
LV_IMG_DECLARE(img_instagram_icon);  // assets\instagram.png
LV_IMG_DECLARE(img_kakao_icon);      // assets\kakao.png
LV_IMG_DECLARE(img_line_icon);      // assets\line.png
LV_IMG_DECLARE(img_mail_icon);      // assets\mail.png
LV_IMG_DECLARE(img_messenger_icon); // assets\messenger.png
LV_IMG_DECLARE(img_penguin_icon);       // assets\penguin.png
LV_IMG_DECLARE(img_skype_icon);         // assets\skype.png
LV_IMG_DECLARE(img_sms_icon);           // assets\sms.png
LV_IMG_DECLARE(img_telegram_icon);      // assets\telegram.png
LV_IMG_DECLARE(img_twitter_icon);       // assets\twitter.png
LV_IMG_DECLARE(img_viber_icon);         // assets\viber.png
LV_IMG_DECLARE(img_vkontakte_icon);     // assets\vkontakte.png
LV_IMG_DECLARE(img_weibo_icon);         // assets\weibo.png
LV_IMG_DECLARE(img_whatsapp_icon);      // assets\whatsapp.png
LV_IMG_DECLARE(img_wechat_icon);    // assets\wechat.png
LV_IMG_DECLARE(img_chrns_icon);      // assets\chrns.png

LV_IMG_DECLARE(img_brightness_icon); // assets\brightness.png
LV_IMG_DECLARE(img_scrolling_icon);  // assets\scrolling.png
LV_IMG_DECLARE(img_timeout_icon);    // assets\timeout.png
LV_IMG_DECLARE(img_bat_icon);        // assets\bat.png
LV_IMG_DECLARE(img_info_icon);       // assets\info.png
LV_IMG_DECLARE(img_play_icon);       // assets\play.png
LV_IMG_DECLARE(img_left_arrow_icon);     // assets\left-arrow.png
LV_IMG_DECLARE(img_right_arrow_icon);     // assets\right-arrow.png
LV_IMG_DECLARE(img_bt_icon);         // assets\bt.png
LV_IMG_DECLARE(img_search_icon);     // assets\search.png
LV_IMG_DECLARE(img_camera_icon);     // assets\camera.png
LV_IMG_DECLARE(img_notify_icon);        // assets\notify.png
LV_IMG_DECLARE(img_setting_icon);       // assets\setting.png
LV_IMG_DECLARE(img_play_pause_g_icon);  // assets\play_pause_g.png
LV_IMG_DECLARE(img_previous_g_icon);    // assets\previous_g.png
LV_IMG_DECLARE(img_next_g_icon);        // assets\next_g.png
LV_IMG_DECLARE(img_bluetooth_g_icon);   // assets\bluetooth_g.png
LV_IMG_DECLARE(img_search_g_icon);      // assets\search_g.png
LV_IMG_DECLARE(img_volupe_up_g_icon);   // assets\volupe_up_g.png
LV_IMG_DECLARE(img_volume_down_g_icon); // assets\volume_down_g.png
LV_IMG_DECLARE(img_qr_icon);       // assets\qr_icon.png
LV_IMG_DECLARE(img_twitter_x_icon);     // assets\twitter_x.png
LV_IMG_DECLARE(img_battery_plugged_icon); // assets\battery_plugged.png
LV_IMG_DECLARE(img_ble_app_icon);         // assets\ble_app.png
LV_IMG_DECLARE(img_battery_state_icon);   // assets\battery_state.png
LV_IMG_DECLARE(img_answer_icon);          // assets\answer.png
LV_IMG_DECLARE(img_app_info_icon);        // assets\app_info.png
LV_IMG_DECLARE(img_smartwatch_icon);      // assets\smartwatch.png
LV_IMG_DECLARE(img_vol_up_icon);          // assets\vol_up.png
LV_IMG_DECLARE(img_vol_down_icon);        // assets\vol_down.png
LV_IMG_DECLARE(img_pay_icon);             // assets\pay.png
LV_IMG_DECLARE(img_web_icon);             // assets\web.png
LV_IMG_DECLARE(img_kenya_icon);           // assets\kenya.png
LV_IMG_DECLARE(img_lvgl_logo_icon);       // assets\lvgl_logo.png
LV_IMG_DECLARE(img_lvgl_logo_black_icon); // assets/lvgl_logo_black.png
LV_IMG_DECLARE(img_lvgl_logo_red_icon);   // assets/lvgl_logo_red.png
LV_IMG_DECLARE(img_lvgl_logo_green_icon); // assets/lvgl_logo_green.png
LV_IMG_DECLARE(img_lvgl_logo_blue_icon);  // assets/lvgl_logo_blue.png
LV_IMG_DECLARE(img_game_icon);       // assets\game_icon.png
LV_IMG_DECLARE(img_language_icon);        // assets\language.png
LV_IMG_DECLARE(img_file_manager_icon);
LV_IMG_DECLARE(img_drive_icon);         // assets/drive.png
LV_IMG_DECLARE(img_file_icon);          // assets/file.png
LV_IMG_DECLARE(img_directory_icon);     // assets/directory.png
LV_IMG_DECLARE(img_back_file_icon);     // assets/back_file.png
LV_IMG_DECLARE(img_clock_icon);         // assets/clock.png
LV_IMG_DECLARE(img_bin_icon);           // assets/bin.png
LV_IMG_DECLARE(img_up_arrow_icon);      // assets/up_arrow.png
LV_IMG_DECLARE(img_screen_rotate_icon); // assets/screen_rotate.png
LV_IMG_DECLARE(img_wechat_pay_icon);
LV_IMG_DECLARE(img_alipay_icon);
LV_IMG_DECLARE(img_paypal_icon);
LV_IMG_DECLARE(img_digital_preview);
LV_IMG_DECLARE(img_alert_icon);
LV_IMG_DECLARE(img_general_settings_icon);
LV_IMG_DECLARE(img_notifications_app_icon);
LV_IMG_DECLARE(img_weather_app_icon);

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
 * function to show dialogs
 * @param title    dialog title
 * @param message    dialog message
 */
void lv_demo_smartwatch_show_dialog(const char *title, const char *message);

/**
 * get pointer to the watchface root object
 * @return pointer to root object
 */
lv_obj_t *lv_smartwatch_face_get_root();

/**
 * update the position of animated analog seconds
 * @param second the current seconds position
 */
void lv_smartwatch_face_update_seconds(int second);

/**
 * function that handles animation of circular scrolling
 * @param e    event
 */
void lv_smartwatch_scroll_event(lv_event_t *e);

/**
 * set whether to enable circular scroll on lists
 * @param state    enable or disable
 */
void lv_smartwatch_set_circular_scroll(bool state);

/**
 * set whether to load app list
 * @param state    enable or disable
 */
void lv_smartwatch_set_load_app_list(bool state);

/**
 * get whether to load app list
 * @return state whether to load app list or not
 */
bool lv_smartwatch_get_load_app_list();

/**
 * External apps call this function to return to the app list
 */
void lv_smartwatch_app_close(void);

/**
 * load the home screen
 * @param anim_type    screen load animation to use
 * @param time    animation time
 * @param delay     delay time before loading the screen
 */
void lv_demo_smartwatch_home_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * function to load watchface 
 * @param index position of the watchface
 * @return whether success or not (unsuccessful means watchface at index was not found)
 */
bool lv_smartwatch_watchface_load_face(uint16_t index);

/**
 * update time on the default watchface
 * @param minute   minute
 * @param hour hour
 * @param am_pm 
 * @param date day of month
 * @param month
 * @param weekday
 */
void lv_demo_smartwatch_home_set_time(int minute, int hour, const char *am_pm, int date, const char *month, const char *weekday);

/**
 * function to send watchface events
 * @param e event
 */
void lv_smartwatch_watchface_events_cb(lv_event_t *e);

/**
 * Register external watchfaces
 * @param name    name of the app
 * @param preview preview image of the watchface (180x180)
 * @param watchface   pointer to main object of the watchface
 * @param seconds   pointer to analog seconds object. Used for smooth analog seconds animation
 */
void lv_smartwatch_register_watchface_cb(const char *name, const lv_image_dsc_t *preview, lv_obj_t **watchface, lv_obj_t **seconds);

/**
 * Register external apps
 * @param name    name of the app
 * @param icon    launcher icon of the app (64x64)
 * @param entry   pointer to main object of the app
 */
void lv_smartwatch_register_app_cb(const char *name, const lv_image_dsc_t *icon, lv_obj_t **entry);

/**
 * load the app list screen
 * @param anim_type    screen load animation to use
 * @param time    animation time
 * @param delay     delay time before loading the screen
 */
void lv_demo_smartwatch_list_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * clear the notification list
 */
void lv_demo_smartwatch_clear_notifications();

/**
 * add a notification to the list
 * @param app_id   id of the app sending the notification (sets icon from the inbuilt icon list)
 * @param message  the notification message preview
 * @param index position of the message that will be returned when clicked
 */
void lv_demo_smartwatch_add_notification(int app_id, const char *message, int index);

/**
 * add a notification to the list
 * @param app_id   id of the app sending the notification (sets icon from the inbuilt icon list)
 * @param message  the notification message content
 * @param time the notification time
 */
void lv_demo_smartwatch_show_notification(int app_id, const char *message, const char *time);

/**
 * set the callback funtion when a notification is clicked
 * @param cb callback function
 */
void lv_demo_smartwatch_set_notification_click_cb(lv_smartwatch_notification_click_cb_t cb);

/**
 * load the notifications screen
 * @param anim_type    screen load animation to use
 * @param time    animation time
 * @param delay     delay time before loading the screen
 */
void lv_demo_smartwatch_notifications_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * load the settings screen
 * @param anim_type    screen load animation to use
 * @param time    animation time
 * @param delay     delay time before loading the screen
 */
void lv_demo_smartwatch_settings_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * set the callback funtion for settings actions
 * @param cb callback function
 */
void lv_demo_smartwatch_set_settings_actions_cb(lv_smartwatch_settings_change_cb_t cb);


/**
 * load the control panel screen
 * @param anim_type    screen load animation to use
 * @param time    animation time
 * @param delay     delay time before loading the screen
 */
void lv_demo_smartwatch_control_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * set the callback funtion for music control
 * @param cb callback function
 */
void lv_demo_smartwatch_set_music_control_cb(lv_smartwatch_music_control_cb_t cb);

/**
 * set the current weather
 * @param temp    current temp in °C
 * @param icon  weather icon to use (0-7)
 * @param day whether day or night (changes the icon)
 * @param hour  last update hour of the weather
 * @param minute    last update minute of the weather
 * @param city  the current city of the weather
 */
void lv_demo_smartwatch_set_weather(int temp, uint8_t icon, bool day, int hour, int minute, const char *city);

/**
 * clear daily forecast list
 */
void lv_demo_smartwatch_weather_daily_clear();

/**
 * clear hourly forecast list
 */
void lv_demo_smartwatch_weather_hourly_clear();

/**
 * add daily forecast weather to the list
 * @param day    day of the week (0-7)
 * @param temp  temp in °C
 * @param id  weather icon to use (0-7)
 */
void lv_demo_smartwatch_weather_add_daily(int day, int temp, int id);

/**
 * add hourly forecast weather to the list
 * @param hour    hour oof the day (0-23)
 * @param id  weather icon to use (0-7)
 * @param temp  temp in °C
 * @param humidity humidity %
 * @param wind wind speed in km/hr
 * @param uv uv index
 * @param info item type (use true when adding forecast details, false adds it as a title row)
 */
void lv_demo_smartwatch_weather_add_hourly(int hour, int id, int temp, int humidity, int wind, int uv, bool info);

/**
 * load the weather screen
 * @param anim_type    screen load animation to use
 * @param time    animation time
 * @param delay     delay time before loading the screen
 */
void lv_demo_smartwatch_weather_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * load the qr screen
 * @param anim_type    screen load animation to use
 * @param time    animation time
 * @param delay     delay time before loading the screen
 */
void lv_demo_smartwatch_qr_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * clear the qr codes in list
 */
void lv_demo_smartwatch_qr_list_clear();

/**
 * add qr codes to the list
 * @param id    determines the icon and name of the qr from inbuilt list. Value (0-9)
 * @param link   link to be added as a qr code
 */
void lv_demo_smartwatch_qr_list_add(uint8_t id, const char *link);

/**
 * load the easter egg screen
 * @param anim_type    screen load animation to use
 * @param time    animation time
 * @param delay     delay time before loading the screen
 */
void lv_demo_smartwatch_easter_egg_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * Add a slider to the main settings list
 * @param id unique id to track changes (recommended > 0x000F)
 * @param name title for the setting
 * @param img icon for the setting
 * @param value default value for the slider
 * @param min min value for the slider
 * @param max max value for the slider
 * @return pointer to slider object
 */
lv_obj_t *lv_smartwatch_settings_add_slider(uint16_t id, const char *name, const lv_img_dsc_t *img, int32_t value, int32_t min, int32_t max);

/**
 * Add a toggle switch to the main settings list
 * @param id unique id to track changes (recommended > 0x000F)
 * @param name title for the setting
 * @param img icon for the setting
 * @param state default state for the switch
 * @return pointer to switch object
 */
lv_obj_t *lv_smartwatch_settings_add_toggle(uint16_t id, const char *name, const lv_img_dsc_t *img, bool state);

/**
 * Add a dropdowwn to the main settings list
 * @param id unique id to track changes (recommended > 0x000F)
 * @param name title for the setting
 * @param img icon for the setting
 * @param options the dropdown options
 * @return pointer to dropdown object
 */
lv_obj_t *lv_smartwatch_settings_add_dropdown(uint16_t id, const char *name, const lv_img_dsc_t *img, const char *options);

/**
 * Add a label to the main settings list
 * @param id unique id to track changes (recommended > 0x000F)
 * @param name text for the label
 * @param img icon for the label
 * @return pointer to label object
 */
lv_obj_t *lv_smartwatch_settings_add_label(uint16_t id, const char *name, const lv_img_dsc_t *img);


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
