/**
 * @file lv_demo_high_res_api_example.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_high_res.h"
#if LV_USE_DEMO_HIGH_RES

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void charging_anim_cb(void * api_v, int32_t val);
static void ev_is_charging_cb(lv_observer_t * observer, lv_subject_t * subject);
static int32_t celsius_to_celsius(int32_t cel);
static int32_t celsius_to_fahrenheit(int32_t cel);
static void temperature_units_cb(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 *  STATIC VARIABLES
 **********************/

static int32_t celsius_temperatures[4];

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_high_res_api_example(void)
{
    lv_demo_high_res_subjects_t * api = lv_demo_high_res();

    lv_subject_add_observer(&api->ev_is_charging, ev_is_charging_cb, api);

    celsius_temperatures[0] = lv_subject_get_int(&api->temperature_outdoor);
    celsius_temperatures[1] = lv_subject_get_int(&api->temperature_outdoor_low);
    celsius_temperatures[2] = lv_subject_get_int(&api->temperature_outdoor_high);
    celsius_temperatures[3] = lv_subject_get_int(&api->temperature_indoor);
    lv_subject_add_observer(&api->temperature_units_are_celsius, temperature_units_cb, api);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void charging_anim_cb(void * api_v, int32_t val)
{
    lv_demo_high_res_subjects_t * api = api_v;
    lv_subject_set_int(&api->ev_charge_percent, val);
}

static void ev_is_charging_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_demo_high_res_subjects_t * api = lv_observer_get_user_data(observer);
    bool ev_is_charging = lv_subject_get_int(subject);

    lv_anim_t a;
    lv_anim_init(&a);
    int32_t start = lv_subject_get_int(&api->ev_charge_percent);
    int32_t end = ev_is_charging ? 88 : 36;
    lv_anim_set_duration(&a, LV_ABS(end - start) * 35);
    lv_anim_set_exec_cb(&a, charging_anim_cb);
    lv_anim_set_values(&a, start, end);
    lv_anim_set_var(&a, api);
    lv_anim_start(&a);
}

static int32_t celsius_to_celsius(int32_t cel)
{
    return cel;
}

static int32_t celsius_to_fahrenheit(int32_t cel)
{
    return cel * 9 / 5 + 320; /* + 320 instead of 32 because temperatures are tenths */
}

static void temperature_units_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_demo_high_res_subjects_t * api = lv_observer_get_user_data(observer);
    bool units_are_celsius = lv_subject_get_int(subject);

    int32_t (*conversion)(int32_t cel) = units_are_celsius ? celsius_to_celsius : celsius_to_fahrenheit;
    lv_subject_set_int(&api->temperature_outdoor, conversion(celsius_temperatures[0]));
    lv_subject_set_int(&api->temperature_outdoor_low, conversion(celsius_temperatures[1]));
    lv_subject_set_int(&api->temperature_outdoor_high, conversion(celsius_temperatures[2]));
    lv_subject_set_int(&api->temperature_indoor, conversion(celsius_temperatures[3]));
}

#endif /*LV_USE_DEMO_HIGH_RES*/
