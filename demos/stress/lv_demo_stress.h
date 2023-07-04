/**
 * @file lv_demo_stress.h
 *
 */

#ifndef LV_DEMO_STRESS_H
#define LV_DEMO_STRESS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_demos.h"

/*********************
 *      DEFINES
 *********************/

#define LV_DEMO_STRESS_TIME_STEP    50

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_demo_stress(void);

void lv_demo_stress_close(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_STRESS_H*/
