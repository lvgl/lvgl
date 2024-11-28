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
    /* input subjects */
    lv_subject_t logo;                                  /* image pointer */
    lv_subject_t logo_dark;                             /* image pointer */
    lv_subject_t hour;                                  /* int32_t */
    lv_subject_t minute;                                /* int32_t */
    lv_subject_t week_day_name;                         /* string pointer */
    lv_subject_t month_day;                             /* int32_t */
    lv_subject_t month_name;                            /* string pointer */
    lv_subject_t temperature_outdoor;                   /* int32_t (tenths of a degree C) */
    lv_subject_t temperature_indoor;                    /* int32_t (tenths of a degree C) */
    lv_subject_t wifi_ssid;                             /* string pointer */
    lv_subject_t wifi_ip;                               /* string pointer */

    /* output subjects */
    lv_subject_t volume;                                /* int32_t */
    lv_subject_t main_light_temperature;                /* int32_t */
    lv_subject_t main_light_intensity;                  /* int32_t */
    lv_subject_t music_play;                            /* int32_t */
    lv_subject_t thermostat_fan_speed;                  /* int32_t */
    lv_subject_t thermostat_target_temperature;         /* int32_t (degrees C)*/

    /* input+output subjects */
    lv_subject_t locked;                                /* int32_t */

    void * user_data;                                   /* optional extra data field for the user */
} lv_demo_high_res_subjects_t;

typedef void (*lv_demo_high_res_exit_cb_t)(lv_demo_high_res_subjects_t * subjects);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Start the High Resolution Demo on the default display.
 * This demo requires `LV_USE_DEMO_HIGH_RES` and `LV_FONT_FMT_TXT_LARGE`
 * to be enabled as well as a filesystem driver to be configured and the
 * `LV_FS_DEFAULT_DRIVE_LETTER` set. The display size should be
 * 800x480, 1280x720, or 1920x1080.
 * @param assets_base_path  folder where the image assets are.
 *                          If `NULL` "lvgl/demos/high_res/assets" will be used.
 * @param exit_cb           A callback function which will be called when the
 *                          "logout" button is clicked, or `NULL` to do nothing.
 * @return                  a struct with subjects to control the UI
 *                          and react to input
 */
lv_demo_high_res_subjects_t * lv_demo_high_res(const char * assets_base_path,
                                               lv_demo_high_res_exit_cb_t exit_cb);

/**
 * Start the High Resolution Demo on the default display.
 * This function demonstrates how the subjects can be used.
 */
void lv_demo_high_res_api_example(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_HIGH_RES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_HIGH_RES_H*/
