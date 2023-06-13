/**
 * @file lv_calendar_header_arrow.h
 *
 */

#ifndef LV_CALENDAR_HEADER_ARROW_H
#define LV_CALENDAR_HEADER_ARROW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"
#if LV_USE_CALENDAR && LV_USE_CALENDAR_HEADER_ARROW

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t obj;
    lv_obj_t *prev_btn;
    lv_obj_t *next_btn;
} lv_calendar_header_arrow_t;

extern const lv_obj_class_t lv_calendar_header_arrow_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a calendar header with drop-drowns to select the year and month
 * @param parent    pointer to a calendar object.
 * @return          the created header
 */
lv_obj_t * lv_calendar_header_arrow_create(lv_obj_t * parent);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the previous button object of the calendar header.
 * It shows the dates and day names.
 * @param obj   pointer to a calendar arrow header object
 * @return      pointer to a button 
 */
lv_obj_t * lv_calendar_header_arrow_get_prev_btn(const lv_obj_t * obj);

/**
 * Get the next button object of the calendar header.
 * It shows the dates and day names.
 * @param obj   pointer to a calendar arrow header object
 * @return      pointer to a button 
 */
lv_obj_t * lv_calendar_header_arrow_get_next_btn(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CALENDAR_HEADER_ARROW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CALENDAR_HEADER_ARROW_H*/
