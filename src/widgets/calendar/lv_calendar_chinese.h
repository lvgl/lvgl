/**
 * @file lv_calendar_chinese.h
 *
 */

#ifndef LV_CALENDAR_CHINESE_H
#define LV_CALENDAR_CHINESE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"
#include "lv_calendar.h"
#if LV_USE_CALENDAR && LV_USE_CALENDAR_CHINESE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_calendar_date_t today;
    bool leep_month;
} lv_calendar_chinese_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Enable the chinese calendar.
 * @param obj   pointer to a calendar object.
 * @param en    true: enable chinese calendar; false: disable
 */
void lv_calendar_set_chinese_mode(lv_obj_t * obj, bool en);

/**
 * Get the name of the day
 * @param gregorian   to obtain the gregorian time for the name
 * @return            return the name of the day
 */
const char * lv_calendar_get_day_name(lv_calendar_date_t * gregorian);

/**
 * Get the chinese time of the gregorian time (reference: https://www.cnblogs.com/liyang31tg/p/4123171.html)
 * @param gregorian_time   need to convert to chinese time in gregorian time
 * @param chinese_time     the chinese time convert from gregorian time
 */
void lv_calendar_gregorian_to_chinese(lv_calendar_date_t * gregorian_time, lv_calendar_chinese_t * chinese_time);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CALENDAR_CHINESE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CALENDAR_CHINESE_H*/
