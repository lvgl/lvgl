/**
 * @file lv_draw_eve5.h
 *
 */

#ifndef LV_DRAW_EVE5_H
#define LV_DRAW_EVE5_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 * INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_EVE5

#include "EVE_Hal.h"
#include "Esd_GpuAlloc.h"

/*********************
 * DEFINES
 *********************/

/**********************
 * TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_eve5_init(EVE_HalContext *hal, Esd_GpuAlloc *allocator);
void lv_draw_eve5_deinit(void);

#endif /* LV_USE_DRAW_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_EVE5_H */
