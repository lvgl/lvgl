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
#include "../lv_conf_internal.h"

#if LV_USE_CALENDAR != 0

#include "../lv_core/lv_obj.h"

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
    int8_t month;
    int8_t day;
} lv_calendar_date_t;

/*Data of calendar*/
typedef struct {
    /*None*/ /*Ext. of ancestor*/
    /*New data for this type */
    lv_calendar_date_t today;               /*Date of today*/
    lv_calendar_date_t showed_date;         /*Currently visible month (day is ignored)*/
    lv_calendar_date_t * highlighted_dates; /*Apply different style on these days (pointer to an
                                               array defined by the user)*/
    int8_t btn_pressing;                    /*-1: prev month pressing, +1 next month pressing on the header*/
    uint16_t highlighted_dates_num;          /*Number of elements in `highlighted_days`*/
    lv_calendar_date_t pressed_date;
    const char ** day_names;   /*Pointer to an array with the name of the days (NULL: use default names)*/
    const char ** month_names; /*Pointer to an array with the name of the month (NULL. use default names)*/

    /*Styles*/
    lv_style_list_t style_header;
    lv_style_list_t style_day_names;
    lv_style_list_t style_date_nums;
} lv_calendar_ext_t;

/** Calendar parts*/
enum {
    LV_CALENDAR_PART_BG, /**< Background and "normal" date numbers style */
    LV_CALENDAR_PART_HEADER, /** Calendar header style */
    LV_CALENDAR_PART_DAY_NAMES, /** Day name style */
    LV_CALENDAR_PART_DATE, /** Day name style */
};
typedef uint8_t lv_calendar_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a calendar objects
 * @param par pointer to an object, it will be the parent of the new calendar
 * @param copy pointer to a calendar object, if not NULL then the new object will be copied from it
 * @return pointer to the created calendar
 */
lv_obj_t * lv_calendar_create(lv_obj_t * par, const lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the today's date
 * @param calendar pointer to a calendar object
 * @param today pointer to an `lv_calendar_date_t` variable containing the date of today. The value
 * will be saved it can be local variable too.
 */
void lv_calendar_set_today_date(lv_obj_t * calendar, lv_calendar_date_t * today);

/**
 * Set the currently showed
 * @param calendar pointer to a calendar object
 * @param showed pointer to an `lv_calendar_date_t` variable containing the date to show. The value
 * will be saved it can be local variable too.
 */
void lv_calendar_set_showed_date(lv_obj_t * calendar, lv_calendar_date_t * showed);

/**
 * Set the the highlighted dates
 * @param calendar pointer to a calendar object
 * @param highlighted pointer to an `lv_calendar_date_t` array containing the dates. ONLY A POINTER
 * WILL BE SAVED! CAN'T BE LOCAL ARRAY.
 * @param date_num number of dates in the array
 */
void lv_calendar_set_highlighted_dates(lv_obj_t * calendar, lv_calendar_date_t highlighted[], uint16_t date_num);

/**
 * Set the name of the days
 * @param calendar pointer to a calendar object
 * @param day_names pointer to an array with the names. E.g. `const char * days[7] = {"Sun", "Mon",
 * ...}` Only the pointer will be saved so this variable can't be local which will be destroyed
 * later.
 */
void lv_calendar_set_day_names(lv_obj_t * calendar, const char ** day_names);

/**
 * Set the name of the month
 * @param calendar pointer to a calendar object
 * @param month_names pointer to an array with the names. E.g. `const char * days[12] = {"Jan", "Feb",
 * ...}` Only the pointer will be saved so this variable can't be local which will be destroyed
 * later.
 */
void lv_calendar_set_month_names(lv_obj_t * calendar, const char ** month_names);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the today's date
 * @param calendar pointer to a calendar object
 * @return return pointer to an `lv_calendar_date_t` variable containing the date of today.
 */
lv_calendar_date_t * lv_calendar_get_today_date(const lv_obj_t * calendar);

/**
 * Get the currently showed
 * @param calendar pointer to a calendar object
 * @return pointer to an `lv_calendar_date_t` variable containing the date is being shown.
 */
lv_calendar_date_t * lv_calendar_get_showed_date(const lv_obj_t * calendar);

/**
 * Get the the pressed date.
 * @param calendar pointer to a calendar object
 * @return pointer to an `lv_calendar_date_t` variable containing the pressed date.
 *         `NULL` if not date pressed (e.g. the header)
 */
lv_calendar_date_t * lv_calendar_get_pressed_date(const lv_obj_t * calendar);

/**
 * Get the the highlighted dates
 * @param calendar pointer to a calendar object
 * @return pointer to an `lv_calendar_date_t` array containing the dates.
 */
lv_calendar_date_t * lv_calendar_get_highlighted_dates(const lv_obj_t * calendar);

/**
 * Get the number of the highlighted dates
 * @param calendar pointer to a calendar object
 * @return number of highlighted days
 */
uint16_t lv_calendar_get_highlighted_dates_num(const lv_obj_t * calendar);

/**
 * Get the name of the days
 * @param calendar pointer to a calendar object
 * @return pointer to the array of day names
 */
const char ** lv_calendar_get_day_names(const lv_obj_t * calendar);

/**
 * Get the name of the month
 * @param calendar pointer to a calendar object
 * @return pointer to the array of month names
 */
const char ** lv_calendar_get_month_names(const lv_obj_t * calendar);

/**
 * Get the day of the week
 * @param year a year
 * @param month a  month (1..12)
 * @param day a day (1..31)
 * @return [0..6] which means [Sun..Sat] or [Mon..Sun] depending on LV_CALENDAR_WEEK_STARTS_MONDAY
 */
uint8_t lv_calendar_get_day_of_week(uint32_t year, uint32_t month, uint32_t day);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CALENDAR*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_CALENDAR_H*/
