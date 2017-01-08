/**
 * @file lv_app_kb.h
 *
 */

#ifndef LV_APP_KB_H
#define LV_APP_KB_H

/*********************
 *      INCLUDES
 *********************/
#include "../lv_app.h"
#if LV_APP_ENABLE != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
	LV_APP_KB_MODE_TXT = 0x01,
	LV_APP_KB_MODE_NUM = 0x02,
}lv_app_kb_mode_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_app_kb_init(void);
void lv_app_kb_open(lv_obj_t * ta, lv_app_kb_mode_t mode, void (*close)(lv_obj_t *), void (*ok)(lv_obj_t *));
void lv_app_kb_close(bool ok);

/**********************
 *      MACROS
 **********************/
#endif /*LV_APP_ENABLE != 0*/

#endif /*LV_APP_KB_H*/
