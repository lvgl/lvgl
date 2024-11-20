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
    lv_subject_t temperature_outdoor;                   /* int32_t (tenths of a degree) */
    lv_subject_t temperature_outdoor_low;               /* int32_t (tenths of a degree) */
    lv_subject_t temperature_outdoor_high;              /* int32_t (tenths of a degree) */
    lv_subject_t temperature_outdoor_description;       /* string pointer */
    lv_subject_t temperature_outdoor_image;             /* image pointer */
    lv_subject_t temperature_indoor;                    /* int32_t (tenths of a degree) */
    lv_subject_t gas_savings_total_spent;               /* int32_t */
    lv_subject_t gas_savings_gas_equivalent;            /* int32_t */
    lv_subject_t security_slides;                       /* image array pointer */
    lv_subject_t ev_charge_percent;                     /* int32_t */
    /* output subjects */
    lv_subject_t temperature_units_are_celsius;         /* int32_t (bool) */
    lv_subject_t volume;                                /* int32_t */
    lv_subject_t main_light_temperature;                /* int32_t */
    lv_subject_t main_light_intensity;                  /* int32_t */
    lv_subject_t ev_is_charging;                        /* int32_t (bool) */
} lv_demo_high_res_subjects_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Start the High Resolution Demo on the default display.
 * @param assets_base_path  folder where the image assets are.
 *                          If NULL "lvgl/demos/high_res/assets" will be used.
 * @return                  a struct with subjects to control the UI
 *                          and react to input
 */
lv_demo_high_res_subjects_t * lv_demo_high_res(const char * assets_base_path);

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
