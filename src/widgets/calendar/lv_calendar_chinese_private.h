/**
 * @file lv_calendar_chinese_private.h
 *
 */

#ifndef LV_CALENDAR_CHINESE_PRIVATE_H
#define LV_CALENDAR_CHINESE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_calendar_chinese.h"

#if LV_USE_CALENDAR && LV_USE_CALENDAR_CHINESE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_calendar_chinese_t {
    lv_calendar_date_t today;
    bool leep_month;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_CALENDAR && LV_USE_CALENDAR_CHINESE */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CALENDAR_CHINESE_PRIVATE_H*/
