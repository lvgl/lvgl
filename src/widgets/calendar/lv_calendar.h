/**
 * @file lv_calendar.h
 *
 */

#ifndef LV_CALENDAR_H
#define LV_CALENDAR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../buttonmatrix/lv_buttonmatrix.h"

#if LV_USE_CALENDAR

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Represents a date on the calendar object (platform-agnostic).
 */
typedef struct {
    uint16_t year;
    uint8_t month;  /**< 1..12 */
    uint8_t day;    /**< 1..31 */
} lv_calendar_date_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_calendar_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a calendar widget
 * @param parent    pointer to an object, it will be the parent of the new calendar
 * @return          pointer the created calendar
 */
lv_obj_t * lv_calendar_create(lv_obj_t * parent);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the today's year, month and day at once
 * @param obj  pointer to a calendar object
 * @param year      today's year
 * @param month     today's month [1..12]
 * @param day       today's day [1..31]
 */
void lv_calendar_set_today_date(lv_obj_t * obj, uint32_t year, uint32_t month, uint32_t day);

/**
 * Set the today's year
 * @param obj  pointer to a calendar object
 * @param year      today's year
 */
void lv_calendar_set_today_year(lv_obj_t * obj, uint32_t year);

/**
 * Set the today's year
 * @param obj  pointer to a calendar object
 * @param month     today's month [1..12]
 */
void lv_calendar_set_today_month(lv_obj_t * obj, uint32_t month);

/**
 * Set the today's year
 * @param obj  pointer to a calendar object
 * @param day       today's day [1..31]
 */
void lv_calendar_set_today_day(lv_obj_t * obj, uint32_t day);

/**
 * Set the currently shown year and month at once
 * @param obj           pointer to a calendar object
 * @param year          shown year
 * @param month         shown month [1..12]
 */
void lv_calendar_set_month_shown(lv_obj_t * obj, uint32_t year, uint32_t month);

/**
 * Set the currently shown year
 * @param obj           pointer to a calendar object
 * @param year          shown year
 */
void lv_calendar_set_shown_year(lv_obj_t * obj, uint32_t year);

/**
 * Set the currently shown month
 * @param obj           pointer to a calendar object
 * @param month         shown month [1..12]
 */
void lv_calendar_set_shown_month(lv_obj_t * obj, uint32_t month);

/**
 * Set the highlighted dates
 * @param obj           pointer to a calendar object
 * @param highlighted   pointer to an `lv_calendar_date_t` array containing the dates.
 *                      Only the pointer will be saved so this variable can't be local which will be destroyed later.
 * @param date_num number of dates in the array
 */
void lv_calendar_set_highlighted_dates(lv_obj_t * obj, lv_calendar_date_t highlighted[], size_t date_num);

/**
 * Set the name of the days
 * @param obj           pointer to a calendar object
 * @param day_names     pointer to an array with the names.
 *                      E.g. `const char * days[7] = {"Sun", "Mon", ...}`
 *                      Only the pointer will be saved so this variable can't be local which will be destroyed later.
 */
void lv_calendar_set_day_names(lv_obj_t * obj, const char ** day_names);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the button matrix object of the calendar.
 * It shows the dates and day names.
 * @param obj       pointer to a calendar object
 * @return          pointer to a the button matrix
 */
lv_obj_t * lv_calendar_get_btnmatrix(const lv_obj_t * obj);

/**
 * Get the today's date
 * @param calendar  pointer to a calendar object
 * @return          return pointer to an `lv_calendar_date_t` variable containing the date of today.
 */
const lv_calendar_date_t * lv_calendar_get_today_date(const lv_obj_t * calendar);

/**
 * Get the currently showed
 * @param calendar  pointer to a calendar object
 * @return          pointer to an `lv_calendar_date_t` variable containing the date is being shown.
 */
const lv_calendar_date_t * lv_calendar_get_showed_date(const lv_obj_t * calendar);

/**
 * Get the highlighted dates
 * @param calendar  pointer to a calendar object
 * @return          pointer to an `lv_calendar_date_t` array containing the dates.
 */
lv_calendar_date_t * lv_calendar_get_highlighted_dates(const lv_obj_t * calendar);

/**
 * Get the number of the highlighted dates
 * @param calendar  pointer to a calendar object
 * @return          number of highlighted days
 */
size_t lv_calendar_get_highlighted_dates_num(const lv_obj_t * calendar);

/**
 * Get the currently pressed day
 * @param calendar  pointer to a calendar object
 * @param date      store the pressed date here
 * @return          LV_RESULT_OK: there is a valid pressed date
 *                  LV_RESULT_INVALID: there is no pressed data
 */
lv_result_t lv_calendar_get_pressed_date(const lv_obj_t * calendar, lv_calendar_date_t * date);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#include "lv_calendar_header_arrow.h"
#include "lv_calendar_header_dropdown.h"
#include "lv_calendar_chinese.h"

#endif  /*LV_USE_CALENDAR*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CALENDAR_H*/
