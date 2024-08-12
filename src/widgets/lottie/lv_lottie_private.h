/**
 * @file lv_lottie_private.h
 *
 */

#ifndef LV_LOTTIE_PRIVATE_H
#define LV_LOTTIE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_LOTTIE

#include "lv_lottie.h"
#include "../canvas/lv_canvas_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#if LV_USE_THORVG_EXTERNAL
#include <thorvg_capi.h>
#else
#include "../../libs/thorvg/thorvg_capi.h"
#endif

typedef struct {
    lv_canvas_t canvas;
    Tvg_Paint * tvg_paint;
    Tvg_Canvas * tvg_canvas;
    Tvg_Animation * tvg_anim;
    lv_anim_t * anim;
    int32_t last_rendered_time;
} lv_lottie_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_LOTTIE_H*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LOTTIE_PRIVATE_H*/
