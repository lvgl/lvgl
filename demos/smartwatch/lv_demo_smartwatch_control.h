/**
 * @file lv_demo_smartwatch_control.h
 *
 */

#ifndef LV_DEMO_SMARTWATCH_CONTROL_H
#define LV_DEMO_SMARTWATCH_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"

#if LV_USE_DEMO_SMARTWATCH

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
 * Create the control page. Called only once.
 */
void lv_demo_smartwatch_control_create(void);

lv_obj_t * lv_demo_smartwatch_get_control_screen(void);
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_SMARTWATCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_SMARTWATCH_HOME_H*/