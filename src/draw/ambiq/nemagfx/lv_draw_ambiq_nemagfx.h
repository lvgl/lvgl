
#ifndef LV_DRAW_AMBIQ_NEMAGFX_H
#define LV_DRAW_AMBIQ_NEMAGFX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"
#if LV_USE_GPU_AMBIQ_NEMAGFX

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_ambiq_nemagfx_init(void);

void lv_draw_ambiq_nemagfx_deinit(void);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_GPU_AMBIQ_NEMAGFX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_AMBIQ_NEMAGFX_H*/
