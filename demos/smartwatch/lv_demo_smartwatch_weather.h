/**
 * @file lv_demo_smartwatch_weather.h
 *
 */

#ifndef LV_DEMO_SMARTWATCH_WEATHER_H
#define LV_DEMO_SMARTWATCH_WEATHER_H

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
 * Create the weather page. Called only once.
 */
void lv_demo_smartwatch_weather_create(void);

/**
 * Load the weather screen
 * @param anim_type     screen load animation to use
 * @param time          animation time
 * @param delay         delay time before loading the screen
 */
void lv_demo_smartwatch_weather_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * Update weather in the default watchface
 * @param temp      current temp in °C
 * @param icon      weather icon to use (0-7)
 */
void lv_demo_smartwatch_home_set_weather(int temp, const lv_img_dsc_t * icon);

/**
 * Clear daily forecast list
 */
void lv_demo_smartwatch_weather_daily_clear(void);

/**
 * Clear hourly forecast list
 */
void lv_demo_smartwatch_weather_hourly_clear(void);

/**
 * Add daily forecast weather to the list
 * @param day    day of the week (0-7)
 * @param temp   temp in °C
 * @param id     weather icon to use (0-7)
 */
void lv_demo_smartwatch_weather_add_daily(int day, int temp, int id);

/**
 * Add hourly forecast weather to the list
 * @param hour      hour oof the day (0-23)
 * @param id        weather icon to use (0-7)
 * @param temp      temp in °C
 * @param humidity  relative humidity %
 * @param wind      wind speed in km/hr
 * @param uv        uv index
 * @param info      item type (use true when adding forecast details, false adds it as a title row)
 */
void lv_demo_smartwatch_weather_add_hourly(int hour, int id, int temp, int humidity, int wind, int uv, bool info);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_SMARTWATCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_SMARTWATCH_HOME_H*/
