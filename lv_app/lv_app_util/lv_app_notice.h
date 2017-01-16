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

/**
 * Initialize the Notifications
 */
void lv_app_notice_init(void);

/**
 * Add a notification with a given text
 * @param format pritntf-like format string
 * @return pointer the notice which is a message box (lv_mbox) object
 */
lv_obj_t *  lv_app_notice_add(const char * format, ...);

/**********************
 *      MACROS
 **********************/

#endif /*LV_APP_NOTICE_H*/

#endif
