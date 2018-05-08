/**
 * @file lv_calendar.h
 *
 */


/* TODO Remove these instructions
 * Search an replace: calendar -> object normal name with lower case (e.g. button, label etc.)
 *                    calendar -> object short name with lower case(e.g. btn, label etc)
 *                    CALENDAR -> object short name with upper case (e.g. BTN, LABEL etc.)
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
#include "../../lv_conf.h"
#if USE_LV_CALENDAR != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
}lv_calendar_date_t;

/*Data of calendar*/
typedef struct {
    /*None*/ /*Ext. of ancestor*/
    /*New data for this type */
    lv_calendar_date_t today;          /*Date of today*/
    lv_calendar_date_t showed_date;   /*Currently visible month (day is ignored)*/
    lv_calendar_date_t * marked_days;  /*Apply different style on these days (pointer to an array defined by the user)*/
    uint8_t marked_days_num;           /*Number of elements in `marked_days`*/

    /*Styles*/
    lv_style_t * style_main;
    lv_style_t * style_header;
    lv_style_t * style_day_names;
    lv_style_t * style_marked_days;
    lv_style_t * style_inactive_days;
    lv_style_t * style_week_box;
    lv_style_t * style_today_box;
}lv_calendar_ext_t;

/*Styles*/
typedef enum {
    LV_CALENDAR_STYLE_BG,       /*Also the style of the "normal" date numbers*/
    LV_CALENDAR_STYLE_HEADER,
    LV_CALENDAR_STYLE_DAY_NAMES,
    LV_CALENDAR_STYLE_MARKED_DAYS,
    LV_CALENDAR_STYLE_INACTIVE_DAYS,
    LV_CALENDAR_STYLE_WEEK_BOX,
    LV_CALENDAR_STYLE_TODAY_BOX,
}lv_calendar_style_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a calendar objects
 * @param par pointer to an object, it will be the parent of the new calendar
 * @param copy pointer to a calendar object, if not NULL then the new object will be copied from it
 * @return pointer to the created calendar
 */
lv_obj_t * lv_calendar_create(lv_obj_t * par, lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/


/*=====================
 * Setter functions
 *====================*/

/**
 * Set a style of a calendar.
 * @param calendar pointer to calendar object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_calendar_set_style(lv_obj_t * calendar, lv_calendar_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get style of a calendar.
 * @param calendar pointer to calendar object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
lv_style_t * lv_btn_get_style(lv_obj_t * calendar, lv_calendar_style_t type);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_CALENDAR*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_CALENDAR_H*/
