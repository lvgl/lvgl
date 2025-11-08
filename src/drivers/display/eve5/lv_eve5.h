/**
 * @file lv_eve5.h
 *
 */
#ifndef LV_EVE5_H
#define LV_EVE5_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_EVE5

#include "../../../display/lv_display.h"
#include "EVE_Hal.h"
#include "Esd_GpuAlloc5.h"

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
 * Create an EVE5 display
 * @return pointer to the created display
 */
lv_display_t *lv_eve5_create(EVE_HalContext *hal, Esd_GpuAlloc *allocator);

/**
 * Get the EVE HAL context from the display
 * @param disp pointer to a display
 * @return pointer to EVE HAL context
 */
EVE_HalContext *lv_eve5_get_hal(lv_display_t *disp);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVE5_H */
