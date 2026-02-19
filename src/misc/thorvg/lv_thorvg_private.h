/**
 * @file lv_thorvg_private.h
 *
 */

#ifndef LV_THORVG_PRIVATE_H
#define LV_THORVG_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_THORVG_EXTERNAL || LV_USE_THORVG_INTERNAL

#include "../lv_types.h"

#if LV_USE_THORVG_EXTERNAL
#include <thorvg_capi.h>
#else
#include "../../libs/thorvg/thorvg_capi.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

Tvg_Canvas * lv_thorvg_canvas_create(lv_display_t * disp);

Tvg_Result lv_thorvg_canvas_set_target(Tvg_Canvas * canvas, uint32_t * buffer, uint32_t stride, uint32_t w, uint32_t h);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_THORVG_EXTERNAL || LV_USE_THORVG_INTERNAL */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_THORVG_PRIVATE_H*/
