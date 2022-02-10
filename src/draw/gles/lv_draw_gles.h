/**
 * @file lv_draw_gles.h
 *
 */

#ifndef LV_DRAW_GLES_H
#define LV_DRAW_GLES_H


#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_GLES


#include "../lv_draw.h"
#include "../../core/lv_disp.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_ctx_t base_draw;
} lv_draw_gles_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_gles_init_ctx(lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx);

void lv_draw_gles_deinit_ctx(lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx);


/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_GLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_GLES_H*/
