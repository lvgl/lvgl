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
#if USE_LV_APP_NOTICE != 0

/*********************
 *      DEFINES
 *********************/
/*Add the required configurations*/
#ifndef LV_APP_NOTICE_SHOW_TIME
#define LV_APP_NOTICE_SHOW_TIME   4000
#endif

#ifndef LV_APP_NOTICE_CLOSE_ANIM_TIME
#define LV_APP_NOTICE_CLOSE_ANIM_TIME   300
#endif

#ifndef LV_APP_NOTICE_MAX_NUM
#define LV_APP_NOTICE_MAX_NUM   6
#endif

#ifndef LV_APP_NOTICE_MAX_LEN
#define LV_APP_NOTICE_MAX_LEN   256
#endif


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
