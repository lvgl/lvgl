/**
 * @file lv_demo_smartwatch_private.h
 *
 */

#ifndef LV_DEMO_SMARTWATCH_PRIVATE_H
#define LV_DEMO_SMARTWATCH_PRIVATE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"


#if LV_USE_DEMO_SMARTWATCH


/*********************
 *      DEFINES
 *********************/

LV_IMAGE_DECLARE(img_weather_day_0);          /* assets/dy-0.png */
LV_IMAGE_DECLARE(img_weather_day_1);          /* assets/dy-1.png */
LV_IMAGE_DECLARE(img_weather_day_2);          /* assets/dy-2.png */
LV_IMAGE_DECLARE(img_weather_day_3);          /* assets/dy-3.png */
LV_IMAGE_DECLARE(img_weather_day_4);          /* assets/dy-4.png */
LV_IMAGE_DECLARE(img_weather_day_5);          /* assets/dy-5.png */
LV_IMAGE_DECLARE(img_weather_day_6);          /* assets/dy-6.png */
LV_IMAGE_DECLARE(img_weather_day_7);          /* assets/dy-7.png */

LV_IMAGE_DECLARE(img_weather_night_0);        /* assets/nt-0.png */
LV_IMAGE_DECLARE(img_weather_night_1);        /* assets/nt-1.png */
LV_IMAGE_DECLARE(img_weather_night_2);        /* assets/nt-2.png */
LV_IMAGE_DECLARE(img_weather_night_3);        /* assets/nt-3.png */
LV_IMAGE_DECLARE(img_weather_night_4);        /* assets/nt-4.png */
LV_IMAGE_DECLARE(img_weather_night_5);        /* assets/nt-5.png */
LV_IMAGE_DECLARE(img_weather_night_6);        /* assets/nt-6.png */
LV_IMAGE_DECLARE(img_weather_night_7);        /* assets/nt-7.png */

LV_IMAGE_DECLARE(img_facebook_icon);          /* assets/facebook.png */
LV_IMAGE_DECLARE(img_instagram_icon);         /* assets/instagram.png */
LV_IMAGE_DECLARE(img_kakao_icon);             /* assets/kakao.png */
LV_IMAGE_DECLARE(img_line_icon);              /* assets/line.png */
LV_IMAGE_DECLARE(img_mail_icon);              /* assets/mail.png */
LV_IMAGE_DECLARE(img_messenger_icon);         /* assets/messenger.png */
LV_IMAGE_DECLARE(img_penguin_icon);           /* assets/penguin.png */
LV_IMAGE_DECLARE(img_skype_icon);             /* assets/skype.png */
LV_IMAGE_DECLARE(img_sms_icon);               /* assets/sms.png */
LV_IMAGE_DECLARE(img_telegram_icon);          /* assets/telegram.png */
LV_IMAGE_DECLARE(img_twitter_icon);           /* assets/twitter.png */
LV_IMAGE_DECLARE(img_viber_icon);             /* assets/viber.png */
LV_IMAGE_DECLARE(img_vkontakte_icon);         /* assets/vkontakte.png */
LV_IMAGE_DECLARE(img_weibo_icon);             /* assets/weibo.png */
LV_IMAGE_DECLARE(img_whatsapp_icon);          /* assets/whatsapp.png */
LV_IMAGE_DECLARE(img_wechat_icon);            /* assets/wechat.png */
LV_IMAGE_DECLARE(img_chrns_icon);             /* assets/chrns.png */

LV_IMAGE_DECLARE(img_brightness_icon);        /* assets/brightness.png */
LV_IMAGE_DECLARE(img_scrolling_icon);         /* assets/scrolling.png */
LV_IMAGE_DECLARE(img_timeout_icon);           /* assets/timeout.png */
LV_IMAGE_DECLARE(img_bat_icon);               /* assets/bat.png */
LV_IMAGE_DECLARE(img_info_icon);              /* assets/info.png */
LV_IMAGE_DECLARE(img_play_icon);              /* assets/play.png */
LV_IMAGE_DECLARE(img_left_arrow_icon);        /* assets/left-arrow.png */
LV_IMAGE_DECLARE(img_right_arrow_icon);       /* assets/right-arrow.png */
LV_IMAGE_DECLARE(img_bt_icon);                /* assets/bt.png */
LV_IMAGE_DECLARE(img_search_icon);            /* assets/search.png */
LV_IMAGE_DECLARE(img_camera_icon);            /* assets/camera.png */
LV_IMAGE_DECLARE(img_notify_icon);            /* assets/notify.png */
LV_IMAGE_DECLARE(img_setting_icon);           /* assets/setting.png */
LV_IMAGE_DECLARE(img_play_pause_g_icon);      /* assets/play_pause_g.png */
LV_IMAGE_DECLARE(img_previous_g_icon);        /* assets/previous_g.png */
LV_IMAGE_DECLARE(img_next_g_icon);            /* assets/next_g.png */
LV_IMAGE_DECLARE(img_bluetooth_g_icon);       /* assets/bluetooth_g.png */
LV_IMAGE_DECLARE(img_search_g_icon);          /* assets/search_g.png */
LV_IMAGE_DECLARE(img_volupe_up_g_icon);       /* assets/volupe_up_g.png */
LV_IMAGE_DECLARE(img_volume_down_g_icon);     /* assets/volume_down_g.png */
LV_IMAGE_DECLARE(img_qr_icon);                /* assets/qr_icon.png */
LV_IMAGE_DECLARE(img_twitter_x_icon);         /* assets/twitter_x.png */
LV_IMAGE_DECLARE(img_battery_plugged_icon);   /* assets/battery_plugged.png */
LV_IMAGE_DECLARE(img_ble_app_icon);           /* assets/ble_app.png */
LV_IMAGE_DECLARE(img_battery_state_icon);     /* assets/battery_state.png */
LV_IMAGE_DECLARE(img_answer_icon);            /* assets/answer.png */
LV_IMAGE_DECLARE(img_app_info_icon);          /* assets/app_info.png */
LV_IMAGE_DECLARE(img_smartwatch_icon);        /* assets/smartwatch.png */
LV_IMAGE_DECLARE(img_vol_up_icon);            /* assets/vol_up.png */
LV_IMAGE_DECLARE(img_vol_down_icon);          /* assets/vol_down.png */
LV_IMAGE_DECLARE(img_pay_icon);               /* assets/pay.png */
LV_IMAGE_DECLARE(img_web_icon);               /* assets/web.png */
LV_IMAGE_DECLARE(img_kenya_icon);             /* assets/kenya.png */
LV_IMAGE_DECLARE(img_lvgl_logo_icon);         /* assets/lvgl_logo.png */
LV_IMAGE_DECLARE(img_lvgl_logo_black_icon);   /* assets/lvgl_logo_black.png */
LV_IMAGE_DECLARE(img_lvgl_logo_red_icon);     /* assets/lvgl_logo_red.png */
LV_IMAGE_DECLARE(img_lvgl_logo_green_icon);   /* assets/lvgl_logo_green.png */
LV_IMAGE_DECLARE(img_lvgl_logo_blue_icon);    /* assets/lvgl_logo_blue.png */
LV_IMAGE_DECLARE(img_game_icon);              /* assets/game_icon.png */
LV_IMAGE_DECLARE(img_language_icon);          /* assets/language.png */
LV_IMAGE_DECLARE(img_file_manager_icon);      /* assets/file_manager.png */
LV_IMAGE_DECLARE(img_drive_icon);             /* assets/drive.png */
LV_IMAGE_DECLARE(img_file_icon);              /* assets/file.png */
LV_IMAGE_DECLARE(img_directory_icon);         /* assets/directory.png */
LV_IMAGE_DECLARE(img_back_file_icon);         /* assets/back_file.png */
LV_IMAGE_DECLARE(img_clock_icon);             /* assets/clock.png */
LV_IMAGE_DECLARE(img_bin_icon);               /* assets/bin.png */
LV_IMAGE_DECLARE(img_up_arrow_icon);          /* assets/up_arrow.png */
LV_IMAGE_DECLARE(img_screen_rotate_icon);     /* assets/screen_rotate.png */
LV_IMAGE_DECLARE(img_wechat_pay_icon);        /* assets/wechat_pay.png */
LV_IMAGE_DECLARE(img_alipay_icon);            /* assets/alipay.png */
LV_IMAGE_DECLARE(img_paypal_icon);            /* assets/paypal.png */
LV_IMAGE_DECLARE(img_digital_preview);        /* assets/digital_preview.png */
LV_IMAGE_DECLARE(img_alert_icon);             /* assets/alert.png */
LV_IMAGE_DECLARE(img_general_settings_icon);  /* assets/general_settings.png */
LV_IMAGE_DECLARE(img_notifications_app_icon); /* assets/notifications_app.png */
LV_IMAGE_DECLARE(img_weather_app_icon);       /* assets/weather_app.png */
LV_IMAGE_DECLARE(img_note_icon);               /* assets/note.png */
LV_IMAGE_DECLARE(img_application_icon);       /* assets/application.png */
LV_IMAGE_DECLARE(img_cloud_icon);             /* assets/cloud.png */
LV_IMAGE_DECLARE(img_chat_icon);              /* assets/chat.png */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/**
 * Load the home screen
 * @param anim_type     screen load animation to use
 * @param time          animation time
 * @param delay         delay time before loading the screen
 */
void lv_demo_smartwatch_home_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * Load the settings screen
 * @param anim_type     screen load animation to use
 * @param time          animation time
 * @param delay         delay time before loading the screen
 */
void lv_demo_smartwatch_settings_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * Load the control panel screen
 * @param anim_type     screen load animation to use
 * @param time          animation time
 * @param delay         delay time before loading the screen
 */
void lv_demo_smartwatch_control_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * Load the weather screen
 * @param anim_type     screen load animation to use
 * @param time          animation time
 * @param delay         delay time before loading the screen
 */
void lv_demo_smartwatch_weather_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * Load the app list screen
 * @param anim_type screen load animation to use
 * @param time      animation time
 * @param delay     delay time before loading the screen
 */
void lv_demo_smartwatch_list_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * Load the notifications screen
 * @param anim_type     screen load animation to use
 * @param time          animation time
 * @param delay         delay time before loading the screen
 */
void lv_demo_smartwatch_notifications_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * Load the qr screen
 * @param anim_type     screen load animation to use
 * @param time          animation time
 * @param delay         delay time before loading the screen
 */
void lv_demo_smartwatch_qr_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * Load the easter egg screen
 * @param anim_type     screen load animation to use
 * @param time          animation time
 * @param delay         delay time before loading the screen
 */
void lv_demo_smartwatch_easter_egg_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);


/**
 * Function that handles animation of circular scrolling
 * @param e    event
 */
void lv_demo_smartwatch_scroll_event(lv_event_t * e);

/**
 * Notify the app list that the screen is loading.
 * This is used to set the circular scroll mode and scrollbar mode of the app list.
 */
void lv_demo_smartwatch_app_list_loading(void);

/**
 * Set whether to enable circular scroll on lists
 * @param state    enable or disable
 */
void lv_demo_smartwatch_set_circular_scroll(bool state);

/**
 * Get the scrollbar mode
 * @return current scrollbar mode
 */
lv_scrollbar_mode_t lv_demo_smartwatch_get_scrollbar_mode(void);

/**
 * Set the scrollbar mode
 * @param mode    scrollbar mode
 */
void lv_demo_smartwatch_set_scrollbar_mode(lv_scrollbar_mode_t mode);

/**
 * Set whether to load app list
 * @param state    enable or disable
 */
void lv_demo_smartwatch_set_load_app_list(bool state);

/**
 * Get whether to load app list
 * @return state whether to load app list or not
 */
bool lv_demo_smartwatch_get_load_app_list(void);


/**
 * Get the app list tile object
 * @return pointer to the app list tile object
 */
lv_obj_t * lv_demo_smartwatch_get_tile_app_list(void);


/**
 * Get the controls tile object
 * @return pointer to the controls tile object
 */
lv_obj_t * lv_demo_smartwatch_get_tile_control(void);

/**
 * Update the brightness slider in the controls screen
 * @param value    new value of the slider
 */
void lv_demo_smartwatch_update_brightness_slider(uint8_t value);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_SMARTWATCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_SMARTWATCH_PRIVATE_H*/
