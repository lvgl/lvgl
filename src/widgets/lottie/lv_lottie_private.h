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

#if LV_USE_LOTTIE

#include "lv_lottie.h"
#include "../canvas/lv_canvas_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#ifndef __THORVG_CAPI_H__ /*To avoid redefinition of typedef warning*/
typedef struct _Tvg_Paint Tvg_Paint;
typedef struct _Tvg_Canvas Tvg_Canvas;
typedef struct _Tvg_Animation Tvg_Animation;
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
