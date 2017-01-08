/**
 * @file lv_app_fsel.h
 *
 */

#ifndef LV_APP_FSEL_H
#define LV_APP_FSEL_H

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

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_app_fsel_init(void);
void lv_app_fsel_open(const char * path, const char * filter, void * param, void (*ok_action)(void *, const char *));
void lv_app_fsel_close();

/**********************
 *      MACROS
 **********************/

#endif /*LV_APP_ENABLE != 0*/

#endif /*LV_APP_FSEL_H*/
