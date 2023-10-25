/**
 * @file lv_demo_render.h
 *
 */

#ifndef LV_DEMO_RENDER_H
#define LV_DEMO_RENDER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_demos.h"

#if LV_USE_DEMO_RENDER

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
 * Run the render verification for a scenario
 * @param idx   index of the scenario to run
 */
void lv_demo_render(uint32_t idx);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_RENDER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_RENDER_H*/
