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

static void exit_cb(lv_demo_high_res_api_t * api);
static void output_subject_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void locked_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void locked_timer_cb(lv_timer_t * t);
static void delete_timer_cb(lv_event_t * e);
static void clock_timer_cb(lv_timer_t * t);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_high_res_api_example(const char * assets_path, const char * logo_path, const char * slides_path)
{
    lv_demo_high_res_api_t * api = lv_demo_high_res(assets_path, logo_path, slides_path, exit_cb);

    /* see lv_demo_high_res.h for documentation of the available subjects */
    lv_subject_set_int(&api->subjects.volume, 50);
    lv_subject_set_pointer(&api->subjects.month_name, "August");
    lv_subject_set_int(&api->subjects.month_day, 1);
    lv_subject_set_pointer(&api->subjects.week_day_name, "Wednesday");
    lv_subject_set_int(&api->subjects.temperature_outdoor, 16 * 10); /* 16 degrees C */
    lv_subject_set_int(&api->subjects.main_light_temperature, 4500); /* 4500 degrees K */
    lv_subject_set_int(&api->subjects.thermostat_target_temperature, 25 * 10); /* 25 degrees C */
    lv_subject_set_pointer(&api->subjects.wifi_ssid, "my home Wi-Fi network");
    lv_subject_set_pointer(&api->subjects.wifi_ip, "192.168.1.1");

    lv_subject_add_observer(&api->subjects.music_play, output_subject_observer_cb, (void *)"music_play");
    lv_subject_add_observer(&api->subjects.locked, output_subject_observer_cb, (void *)"locked");
    lv_subject_add_observer(&api->subjects.volume, output_subject_observer_cb, (void *)"volume");
    lv_subject_add_observer(&api->subjects.main_light_temperature, output_subject_observer_cb,
                            (void *)"main_light_temperature");
    lv_subject_add_observer(&api->subjects.main_light_intensity, output_subject_observer_cb,
                            (void *)"main_light_intensity");
    lv_subject_add_observer(&api->subjects.thermostat_fan_speed, output_subject_observer_cb,
                            (void *)"thermostat_fan_speed");
    lv_subject_add_observer(&api->subjects.thermostat_target_temperature, output_subject_observer_cb,
                            (void *)"thermostat_target_temperature");

    /* unlock after being locked for 3 seconds */
    lv_timer_t * locked_timer = lv_timer_create_basic();
    lv_obj_add_event_cb(api->base_obj, delete_timer_cb, LV_EVENT_DELETE, locked_timer);
    lv_subject_add_observer(&api->subjects.locked, locked_observer_cb, locked_timer);

    /* slowly increment the time */
    lv_timer_t * clock_timer = lv_timer_create(clock_timer_cb, 10000, api);
    lv_obj_add_event_cb(api->base_obj, delete_timer_cb, LV_EVENT_DELETE, clock_timer);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void exit_cb(lv_demo_high_res_api_t * api)
{
    lv_obj_delete(api->base_obj);
}

static void output_subject_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    const char * subject_name = lv_observer_get_user_data(observer);
    LV_LOG_USER("%s output subject value: %"PRId32, subject_name, lv_subject_get_int(subject));
}

static void locked_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    if(lv_subject_get_int(subject)) {
        /* unlock after being locked for 3 seconds */
        lv_timer_t * timer = lv_observer_get_user_data(observer);
        lv_timer_set_cb(timer, locked_timer_cb);
        lv_timer_set_period(timer, 3000);
        lv_timer_set_user_data(timer, subject);
        lv_timer_set_repeat_count(timer, 1);
        lv_timer_set_auto_delete(timer, false);
        lv_timer_resume(timer);
    }
}

static void locked_timer_cb(lv_timer_t * t)
{
    lv_subject_t * locked_subject = lv_timer_get_user_data(t);
    lv_subject_set_int(locked_subject, 0);
}

static void delete_timer_cb(lv_event_t * e)
{
    lv_timer_t * timer = lv_event_get_user_data(e);
    lv_timer_delete(timer);
}

static void clock_timer_cb(lv_timer_t * t)
{
    /* slowly increment the time */
    lv_demo_high_res_api_t * api = lv_timer_get_user_data(t);
    int32_t minutes = lv_subject_get_int(&api->subjects.minute);
    minutes += 1;
    if(minutes > 59) {
        minutes = 0;
        int32_t hour = lv_subject_get_int(&api->subjects.hour);
        hour += 1;
        if(hour > 12) {
            hour = 1;
        }
        lv_subject_set_int(&api->subjects.hour, hour);
    }
    lv_subject_set_int(&api->subjects.minute, minutes);
}

#endif /*LV_USE_DEMO_HIGH_RES*/
