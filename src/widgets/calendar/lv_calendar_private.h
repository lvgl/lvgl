/**
 * @file lv_calendar_private.h
 *
 */

#ifndef LV_CALENDAR_PRIVATE_H
#define LV_CALENDAR_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_calendar.h"

#if LV_USE_CALENDAR

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Data of calendar */
struct _lv_calendar_t {
    lv_obj_t obj;
    /* New data for this type */
    lv_obj_t * btnm;
    lv_calendar_date_t today;                /**< Date of today */
    lv_calendar_date_t showed_date;          /**< Currently visible month (day is ignored) */
    lv_calendar_date_t * highlighted_dates;  /**< Apply different style on these days (pointer to user-defined array) */
    size_t highlighted_dates_num;            /**< Number of elements in `highlighted_days` */
    const char * map[8 * 7];
#ifdef LV_USE_CALENDAR_CHINESE
    bool use_chinese_calendar;

    /** 7 * 6: A week has 7 days, and the calendar displays 6 weeks in total.
     * 20: Including the number of dates, line breaks, names for each day,
     * and reserving several spaces for addresses. */
    char nums [7 * 6][20];
#else
    /** 7 * 6: A week has 7 days, and the calendar displays 6 weeks in total.
     * 6: Including the number of dates, and reserving several spaces for
     * addresses.*/
    char nums [7 * 6][4];
#endif
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_CALENDAR */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CALENDAR_PRIVATE_H*/
