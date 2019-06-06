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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#if LV_USE_CALENDAR != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
    uint16_t year;
    int8_t month;
    int8_t day;
} lv_calendar_date_t;

/*Data of calendar*/
typedef struct
{
    /*None*/ /*Ext. of ancestor*/
    /*New data for this type */
    lv_calendar_date_t today;               /*Date of today*/
    lv_calendar_date_t showed_date;         /*Currently visible month (day is ignored)*/
    lv_calendar_date_t * highlighted_dates; /*Apply different style on these days (pointer to an
                                               array defined by the user)*/
    uint8_t highlighted_dates_num;          /*Number of elements in `highlighted_days`*/
    int8_t btn_pressing;                    /*-1: prev month pressing, +1 next month pressing on the header*/
    lv_calendar_date_t pressed_date;
    const char ** day_names;   /*Pointer to an array with the name of the days (NULL: use default names)*/
    const char ** month_names; /*Pointer to an array with the name of the month (NULL. use default names)*/

    /*Styles*/
    const lv_style_t * style_header;
    const lv_style_t * style_header_pr;
    const lv_style_t * style_day_names;
    const lv_style_t * style_highlighted_days;
    const lv_style_t * style_inactive_days;
    const lv_style_t * style_week_box;
    const lv_style_t * style_today_box;
} lv_calendar_ext_t;

/*Styles*/
enum {
    LV_CALENDAR_STYLE_BG, /*Also the style of the "normal" date numbers*/
    LV_CALENDAR_STYLE_HEADER,
    LV_CALENDAR_STYLE_HEADER_PR,
    LV_CALENDAR_STYLE_DAY_NAMES,
    LV_CALENDAR_STYLE_HIGHLIGHTED_DAYS,
    LV_CALENDAR_STYLE_INACTIVE_DAYS,
    LV_CALENDAR_STYLE_WEEK_BOX,
    LV_CALENDAR_STYLE_TODAY_BOX,
};
typedef uint8_t lv_calendar_style_t;

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
void lv_calendar_set_highlighted_dates(lv_obj_t * calendar, lv_calendar_date_t * highlighted, uint16_t date_num);

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
 * @param day_names pointer to an array with the names. E.g. `const char * days[12] = {"Jan", "Feb",
 * ...}` Only the pointer will be saved so this variable can't be local which will be destroyed
 * later.
 */
void lv_calendar_set_month_names(lv_obj_t * calendar, const char ** day_names);

/**
 * Set a style of a calendar.
 * @param calendar pointer to calendar object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_calendar_set_style(lv_obj_t * calendar, lv_calendar_style_t type, const lv_style_t * style);

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
 * Get style of a calendar.
 * @param calendar pointer to calendar object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
const lv_style_t * lv_calendar_get_style(const lv_obj_t * calendar, lv_calendar_style_t type);

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
