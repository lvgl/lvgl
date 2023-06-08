/**
 * @file lv_gpu_d2_ra6m3.h
 *
 */
#ifndef LV_GPU_D2_RA6M3_H
#define LV_GPU_D2_RA6M3_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_color.h"

#if LV_USE_GPU_RA6M3_G2D
#include "../../core/lv_disp.h"
#include "../sw/lv_draw_sw.h"

/**********************
 *      DEFINE
 **********************/
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define M_PI    3.1415926

/**********************
 *      TYPEDEFS
 **********************/
typedef lv_draw_sw_ctx_t lv_draw_ra6m3_dma2d_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_draw_ra6m3_g2d_init(void);

void lv_port_gpu_init(void);

void lv_port_gpu_flush(void);

void lv_port_gpu_blit(int32_t x, int32_t y, lv_color_t * dst, const lv_area_t * fill_area);

void lv_draw_ra6m3_2d_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);

void lv_draw_ra6m3_2d_ctx_init(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

void lv_draw_ra6m3_2d_ctx_deinit(lv_disp_drv_t * disp, lv_draw_ctx_t * draw_ctx);

#endif  /*LV_USE_GPU_GD32_IPA*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
