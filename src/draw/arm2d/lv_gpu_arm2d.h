/**
 * @file lv_gpu_arm2d.h
 *
 */

#ifndef LV_GPU_ARM2D_H
#define LV_GPU_ARM2D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_ARM2D
#include "../../misc/lv_color.h"
#include "../../core/lv_disp.h"
#include "../sw/lv_draw_sw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef lv_draw_sw_ctx_t lv_draw_arm2d_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_arm2d_ctx_init(struct _lv_disp_t * disp, lv_draw_ctx_t * draw_ctx);

void lv_draw_arm2d_ctx_deinit(struct _lv_disp_t * disp, lv_draw_ctx_t * draw_ctx);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_GPU_ARM2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GPU_ARM2D_H*/