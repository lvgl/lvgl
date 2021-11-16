/**
 * @file lv_draw_blend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_blend.h"
#include "lv_img_decoder.h"
#include "../misc/lv_math.h"
#include "../hal/lv_hal_disp.h"
#include "../core/lv_refr.h"

#if LV_USE_GPU_NXP_PXP
    #include "../gpu/lv_gpu_nxp_pxp.h"
#elif LV_USE_GPU_NXP_VG_LITE
    #include "../gpu/lv_gpu_nxp_vglite.h"
#elif LV_USE_GPU_STM32_DMA2D
    #include "../gpu/lv_gpu_stm32_dma2d.h"
#endif

/*********************
 *      DEFINES
 *********************/
#define GPU_SIZE_LIMIT      240

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_draw_blend_fill(const lv_area_t * clip_area, const lv_area_t * fill_area,
                                          lv_color_t color, lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_opa_t opa,
                                          lv_blend_mode_t mode)
{
    const lv_draw_backend_t * backend = lv_draw_backend_get();
    backend->blend_fill(clip_area, fill_area, color, mask, mask_res, opa, mode);
}

void lv_draw_blend_map(const lv_area_t * clip_area, const lv_area_t * map_area,
                                         const lv_color_t * map_buf,
                                         lv_opa_t * mask, lv_draw_mask_res_t mask_res,
                                         lv_opa_t opa, lv_blend_mode_t mode)
{
    const lv_draw_backend_t * backend = lv_draw_backend_get();
    backend->blend_map(clip_area, map_area, map_buf,
                       mask, mask_res, opa, mode);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


