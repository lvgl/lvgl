/**
 * @file lv_demo_high_res_api_example.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_high_res.h"
#if LV_USE_DEMO_HIGH_RES

/* see `wifi_ssid_observer_cb` */
#if 0
    #include <stdlib.h>
#endif

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
static void door_timer_cb(lv_timer_t * t);
static void wifi_ssid_observer_cb(lv_observer_t * observer, lv_subject_t * subject);

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
    lv_subject_set_int(&api->subjects.door, 0); /* tell the UI the door is closed */
    lv_subject_set_int(&api->subjects.lightbulb_matter, 0); /* 0 or 1 */
    lv_subject_set_int(&api->subjects.lightbulb_zigbee, 1); /* 0 or 1 */
    lv_subject_set_int(&api->subjects.fan_matter, 0); /* 0-3 */
    lv_subject_set_int(&api->subjects.fan_zigbee, 0); /* 0 or 1*/
    lv_subject_set_int(&api->subjects.air_purifier, 3); /* 0-3 */

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
    lv_subject_add_observer(&api->subjects.door, output_subject_observer_cb, (void *)"door");
    lv_subject_add_observer(&api->subjects.lightbulb_matter, output_subject_observer_cb, (void *)"Matter lightbulb");
    lv_subject_add_observer(&api->subjects.lightbulb_zigbee, output_subject_observer_cb, (void *)"Zigbee lightbulb");
    lv_subject_add_observer(&api->subjects.fan_matter, output_subject_observer_cb, (void *)"Matter fan");
    lv_subject_add_observer(&api->subjects.fan_zigbee, output_subject_observer_cb, (void *)"Zigbee fan");
    lv_subject_add_observer(&api->subjects.air_purifier, output_subject_observer_cb, (void *)"air purifier");

    /* unlock after being locked for 3 seconds */
    lv_timer_t * locked_timer = lv_timer_create_basic();
    lv_obj_add_event_cb(api->base_obj, delete_timer_cb, LV_EVENT_DELETE, locked_timer);
    lv_subject_add_observer(&api->subjects.locked, locked_observer_cb, locked_timer);

    /* slowly increment the time */
    lv_timer_t * clock_timer = lv_timer_create(clock_timer_cb, 10000, api);
    lv_obj_add_event_cb(api->base_obj, delete_timer_cb, LV_EVENT_DELETE, clock_timer);

    /* simulate the door opening and closing */
    lv_timer_t * door_timer = lv_timer_create(door_timer_cb, 3000, api);
    lv_obj_add_event_cb(api->base_obj, delete_timer_cb, LV_EVENT_DELETE, door_timer);

    lv_subject_add_observer(&api->subjects.wifi_ssid, wifi_ssid_observer_cb, api);
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

static void door_timer_cb(lv_timer_t * t)
{
    lv_demo_high_res_api_t * api = lv_timer_get_user_data(t);
    lv_subject_set_int(&api->subjects.door, !lv_subject_get_int(&api->subjects.door));
}

static void wifi_ssid_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_demo_high_res_api_t * api = lv_observer_get_user_data(observer);
    const char * ssid = lv_subject_get_pointer(subject);
    const char * password = lv_subject_get_pointer(&api->subjects.wifi_password);

    if(ssid) {
        LV_LOG_USER("The WiFi SSID is \"%s\"", ssid);
        if(ssid[0] == '\0') ssid = NULL;
    }
    else LV_LOG_USER("The WiFi SSID is `NULL`");

    if(password) {
        LV_LOG_USER("The WiFi password is %d characters long", (int) lv_strlen(password));
        if(password[0] == '\0') password = NULL;
    }
    else LV_LOG_USER("The WiFi password is `NULL`");

    /* This block contains non-portable code which is a working example implementation
     * of how you can programmatically connect the host device to WiFi using the
     * SSID and password that was entered in the demo.
     * It is for illustrative purposes. It will not be maintained. It was tested on Ubuntu 20.04.
     */
#if 0
    if(ssid) {
        char command[256];
        if(password) {
            lv_snprintf(command, sizeof(command), "nmcli device wifi connect '%s' password '%s'", ssid, password);
        }
        else {
            lv_snprintf(command, sizeof(command), "nmcli device wifi connect '%s'", ssid);
        }
        int exit_status = system(command); /* requires #include <stdlib.h> */

        if(exit_status == 0) {
            LV_LOG_USER("Successfully connected to the WiFi network.");

            /* TODO: get the IP address using popen("hostname -I") or similar */
            const char * ip = "(IP unknown)";
            lv_subject_set_pointer(&api->subjects.wifi_ip, (void *) ip);

            /* If this is a public showroom demo, you may want to enable this block so that
             * a network can only be connected to only once by you before real connection
             * functionality is disabled for the remainder of the demo's operation.
             */
#if 0
            LV_LOG_USER("This functionality will now be disabled for the remainder of the demo.");
            lv_observer_remove(observer);
#endif
        }
        else {
            LV_LOG_USER("Could not connect to the WiFi network. Exit status: %d", exit_status);

            lv_subject_set_pointer(subject, NULL);
        }
    }
    else {
        /* optional TODO: perform disconnect if connected */
    }
#else
    /* Set an imaginary IP address. */
    if(ssid) {
        lv_subject_set_pointer(&api->subjects.wifi_ip, "10.0.2.15");
    }
#endif
}

#endif /*LV_USE_DEMO_HIGH_RES*/
