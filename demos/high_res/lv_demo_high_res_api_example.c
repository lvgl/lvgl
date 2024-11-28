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

static void exit_cb(lv_demo_high_res_subjects_t * api);
static void output_subject_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void locked_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void locked_timer_cb(lv_timer_t * t);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_high_res_api_example(void)
{
    lv_demo_high_res_subjects_t * api = lv_demo_high_res(NULL, exit_cb);

    // TODO set logo

    // TODO others
    lv_subject_set_int(&api->volume, 50);

    lv_subject_add_observer(&api->volume, output_subject_observer_cb, (void *)"volume");
    lv_subject_add_observer(&api->main_light_temperature, output_subject_observer_cb, (void *)"main_light_temperature");
    lv_subject_add_observer(&api->main_light_intensity, output_subject_observer_cb, (void *)"main_light_intensity");
    lv_subject_add_observer(&api->music_play, output_subject_observer_cb, (void *)"music_play");
    lv_subject_add_observer(&api->thermostat_fan_speed, output_subject_observer_cb, (void *)"thermostat_fan_speed");
    lv_subject_add_observer(&api->thermostat_target_temperature, output_subject_observer_cb,
                            (void *)"thermostat_target_temperature");

    lv_subject_add_observer(&api->locked, locked_observer_cb, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void exit_cb(lv_demo_high_res_subjects_t * api)
{
    lv_obj_delete(lv_obj_get_child(lv_screen_active(), 0));
}

static void output_subject_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    const char * subject_name = lv_observer_get_user_data(observer);
    LV_LOG_USER("%s output subject value: %"PRId32, subject_name, lv_subject_get_int(subject));
}

static void locked_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    if(lv_subject_get_int(subject)) {
        lv_timer_t * timer = lv_timer_create(locked_timer_cb, 3000, subject);
        lv_timer_set_auto_delete(timer, true);
        lv_timer_set_repeat_count(timer, 1);
    }
}

static void locked_timer_cb(lv_timer_t * t)
{
    lv_subject_t * locked_subject = lv_timer_get_user_data(t);
    lv_subject_set_int(locked_subject, 0);
}

#endif /*LV_USE_DEMO_HIGH_RES*/
