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
#include "../../../core/lv_obj.h"
#if LV_USE_CALENDAR_HEADER_ARROW

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
 * Create a calendar objects
 * @param par pointer to an object, it will be the parent of the new calendar
 * @return pointer to the created calendar
 */
lv_obj_t * lv_calendar_header_arrow_create(lv_obj_t * parent, lv_obj_t * calendar, lv_coord_t btn_size);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CALENDAR_HEADER_ARROW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CALENDAR_H*/
