/**
 * @file lv_app_notice.h
 * 
 */

#ifndef LV_APP_NOTICE_H
#define LV_APP_NOTICE_H

/*********************
 *      INCLUDES
 *********************/
#include "../lv_app.h"
#include <stdarg.h>
#if LV_APP_ENABLE != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_app_notice_init(void);
void lv_app_notice_add(const char * format, ...);

/**********************
 *      MACROS
 **********************/

#endif /*LV_APP_NOTICE_H*/

#endif
