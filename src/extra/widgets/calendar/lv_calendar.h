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
#include "../../../widgets/lv_btnmatrix.h"

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
    int8_t month;  /** 1..12*/
    int8_t day;    /** 1..31*/
} lv_calendar_date_t;

/*Data of calendar*/
typedef struct {
    lv_obj_t obj;
    lv_obj_t * btnm;
    /*New data for this type*/
    lv_calendar_date_t today;               /*Date of today*/
    lv_calendar_date_t showed_date;         /*Currently visible month (day is ignored)*/
    lv_calendar_date_t *
    highlighted_dates; /*Apply different style on these days (pointer to an array defined by the user)*/
    uint16_t highlighted_dates_num;          /*Number of elements in `highlighted_days`*/
    const char * map[8 * 7];
    char nums [7 * 6][4];
} lv_calendar_t;

extern const lv_obj_class_t lv_calendar_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_calendar_create(lv_obj_t * parent);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the today's date
 * @param obj  pointer to a calendar object
 * @param year      today's year
 * @param month     today's month [1..12]
 * @param day       today's day [1..31]
 */
void lv_calendar_set_today_date(lv_obj_t * obj, uint32_t year, uint32_t month, uint32_t day);

/**
 * Set the currently showed
 * @param obj           pointer to a calendar object
 * @param year          today's year
 * @param month         today's month [1..12]
 */
void lv_calendar_set_showed_date(lv_obj_t * obj, uint32_t year, uint32_t month);

/**
 * Set the highlighted dates
 * @param obj           pointer to a calendar object
 * @param highlighted   pointer to an `lv_calendar_date_t` array containing the dates.
 *                      Only the pointer will be saved so this variable can't be local which will be destroyed later.
 * @param date_num number of dates in the array
 */
void lv_calendar_set_highlighted_dates(lv_obj_t * obj, lv_calendar_date_t highlighted[], uint16_t date_num);

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
 * @param obj   pointer to a calendar object
 * @return      pointer to a the button matrix
 */
lv_obj_t * lv_calendar_get_btnmatrix(const lv_obj_t * obj);

/**
 * Get the today's date
 * @param calendar pointer to a calendar object
 * @return return pointer to an `lv_calendar_date_t` variable containing the date of today.
 */
const lv_calendar_date_t * lv_calendar_get_today_date(const lv_obj_t * calendar);

/**
 * Get the currently showed
 * @param calendar pointer to a calendar object
 * @return pointer to an `lv_calendar_date_t` variable containing the date is being shown.
 */
const lv_calendar_date_t * lv_calendar_get_showed_date(const lv_obj_t * calendar);

/**
 * Get the highlighted dates
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
 * Get the currently pressed day
 * @param calendar pointer to a calendar object
 * @param date store the pressed date here
 * @return LV_RES_OK: there is a valid pressed date; LV_RES_INV: there is no pressed data
 */
lv_res_t lv_calendar_get_pressed_date(const lv_obj_t * calendar, lv_calendar_date_t * date);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_CALENDAR*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CALENDAR_H*/
