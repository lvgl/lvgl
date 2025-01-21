/**
 * @file lv_demo_smartwatch_home.h
 *
 */

#ifndef LV_DEMO_SMARTWATCH_HOME_H
#define LV_DEMO_SMARTWATCH_HOME_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"

#if LV_USE_DEMO_SMARTWATCH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create the home page. Called only once.
 * @param       parent  tileview object
 */
void lv_demo_smartwatch_home_create(lv_obj_t * parent);

/**
 * Get the home tile object that holds the watchfaces
 * @return pointer to the home tile object
 */
lv_obj_t * lv_demo_smartwatch_get_tile_home(void);

/**
 * Register external watchfaces
 * @param name      name of the app
 * @param preview   preview image of the watchface (180x180)
 * @param watchface pointer to main object of the watchface
 * @param seconds   pointer to analog seconds object. Used for smooth analog seconds animation
 */
void lv_demo_smartwatch_register_watchface_cb(const char * name, const lv_image_dsc_t * preview, lv_obj_t ** watchface,
                                              lv_obj_t ** seconds);

/**
 * Watchface selected callback
 * @param e event
 */
void lv_demo_smartwatch_face_selected_cb(lv_event_t * e);

/**
 * Update the position of animated analog seconds
 * @param second    the current seconds position
 */
void lv_demo_smartwatch_face_update_seconds(int second);

/**
 * Function to send watchface events
 * @param e     event
 */
void lv_demo_smartwatch_face_events_cb(lv_event_t * e);

/**
 * function to load watchface
 * @param index     position of the watchface
 * @return whether success or not (unsuccessful means watchface at index was not found)
 */
bool lv_demo_smartwatch_face_load(uint16_t index);

/**
 * Update weather in the default watchface
 * @param temp  current temp in °C
 * @param icon  weather icon to use (0-7)
 */
void lv_demo_smartwatch_home_set_weather(int temp, const lv_img_dsc_t * icon);

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

/**********************
 *      MACROS
 **********************/


#endif /*LV_USE_DEMO_SMARTWATCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_SMARTWATCH_HOME_H*/
