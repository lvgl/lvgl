/**
 * @file lv_demo_high_res.h
 *
 */

#ifndef LV_DEMO_HIGH_RES_H
#define LV_DEMO_HIGH_RES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../src/lv_conf_internal.h"
#if LV_USE_DEMO_HIGH_RES

#include "../../src/others/observer/lv_observer.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    struct {
        /* input subjects */
        /* int:     the hour shown on clocks,     e.g. 9                                               */
        lv_subject_t hour;
        /* int:     the minute shown on clocks,   e.g. 36                                              */
        lv_subject_t minute;
        /* pointer: day of the week string,       e.g. "Tuesday"                                       */
        lv_subject_t week_day_name;
        /* int:     the day of the month,         e.g. 31                                              */
        lv_subject_t month_day;
        /* pointer: the month string,             e.g. "October"                                       */
        lv_subject_t month_name;
        /* int:     tenths of a degree C,         e.g. 140 (for 14.0 degrees celsius)                  */
        lv_subject_t temperature_outdoor;
        /* int:     tenths of a degree C,         e.g. 225 (for 22.5 degrees celsius)                  */
        lv_subject_t temperature_indoor;
        /* pointer: wifi network name string,     e.g. "my home Wi-Fi network" (or `NULL` for offline) */
        lv_subject_t wifi_ssid;
        /* pointer: wifi local IP address string, e.g. "192.168.1.1" (or `NULL` for offline)           */
        lv_subject_t wifi_ip;

        /* output subjects */
        /* int:     boolean 0 for music paused, 1 for music playing                                    */
        lv_subject_t music_play;

        /* input+output subjects */
        /* int:     boolean 0 for not locked, 1 for locked. When it is 1, you can set it back to 0     */
        lv_subject_t locked;
        /* int:     a value 0-100 of the smart home volume slider                                      */
        lv_subject_t volume;
        /* int:     a value 0-20000 of the smart home light "temperature" slider in Kelvin             */
        lv_subject_t main_light_temperature;
        /* int:     a value 0-100 of the smart home light intensity slider                             */
        lv_subject_t main_light_intensity;
        /* int:     a value 0-100 of the thermostat fan speed slider                                   */
        lv_subject_t thermostat_fan_speed;
        /* int:     a value 150-300 in tenths of a degree C, e.g. 225 (for 22.5 degrees celsius)       */
        lv_subject_t thermostat_target_temperature;
    } subjects;

    /* the object that is created on the active screen which contains the demo contents.
     * Can be deleted to close the demo.
     */
    lv_obj_t * base_obj;
    void * user_data; /* optional extra data field for the user to use freely */
} lv_demo_high_res_api_t;

typedef void (*lv_demo_high_res_exit_cb_t)(lv_demo_high_res_api_t * api);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Start the High Resolution Demo on the default display, on the active screen.
 * This demo requires `LV_USE_DEMO_HIGH_RES` and `LV_FONT_FMT_TXT_LARGE`
 * to be enabled as well as a filesystem driver to be configured and the
 * `LV_FS_DEFAULT_DRIVE_LETTER` set. The display size should be
 * 800x480, 1280x720, or 1920x1080.
 * @param assets_path       Folder where the image assets are.
 *                          If `NULL`, "lvgl/demos/high_res/assets/" will be used.
 * @param logo_path         A path to a logo to display in the bottom-left
 *                          of the home screen. If `NULL`, an LVGL logo is used.
 * @param slides_path       Folder where the "About" app slideshow slides are.
 *                          If `NULL`, "about_app_slides/" will be used.
 *                          The images should be named like 1.png, 2.png, etc.
 * @param exit_cb           A callback function which will be called when the
 *                          "logout" button is clicked, or `NULL` to do nothing.
 * @return                  A struct with subjects to control the UI
 *                          and react to input.
 */
lv_demo_high_res_api_t * lv_demo_high_res(const char * assets_path,
                                          const char * logo_path,
                                          const char * slides_path,
                                          lv_demo_high_res_exit_cb_t exit_cb);

/**
 * This function demonstrates how the demo's API can be used.
 * Start the High Resolution Demo on the default display, on the active screen.
 * This demo requires `LV_USE_DEMO_HIGH_RES` and `LV_FONT_FMT_TXT_LARGE`
 * to be enabled as well as a filesystem driver to be configured and the
 * `LV_FS_DEFAULT_DRIVE_LETTER` set. The display size should be
 * 800x480, 1280x720, or 1920x1080.
 * @param assets_path       Folder where the image assets are.
 *                          If `NULL`, "lvgl/demos/high_res/assets/" will be used.
 * @param logo_path         A path to a logo to display in the bottom-left
 *                          of the home screen. If `NULL`, an LVGL logo is used.
 * @param slides_path       Folder where the "About" app slideshow slides are.
 *                          If `NULL`, "about_app_slides/" will be used.
 *                          The images should be named like 1.png, 2.png, etc.
 */
void lv_demo_high_res_api_example(const char * assets_path, const char * logo_path, const char * slides_path);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_HIGH_RES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_HIGH_RES_H*/
