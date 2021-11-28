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
                                              lv_blend_mode_t blend_mode)
{
    /*Do not draw transparent things*/
    if(opa < LV_OPA_MIN) return;
    if(mask_res == LV_DRAW_MASK_RES_TRANSP) return;
    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask = NULL;

    /*Get clipped fill area which is the real draw area.
     *It is always the same or inside `fill_area`*/
    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, clip_area, fill_area)) return;

    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    lv_disp_draw_buf_t * draw_buf = lv_disp_get_draw_buf(disp);
    const lv_area_t * disp_area = &draw_buf->area;
    lv_color_t * disp_buf = draw_buf->buf_act;

    /*Now `draw_area` has absolute coordinates.
     *Make it relative to `disp_area` to simplify the drawing to `disp_buf`*/
    lv_area_move(&draw_area, -disp_area->x1, -disp_area->y1);

    lv_coord_t stride = lv_area_get_width(disp_area);

    if(disp->driver->gpu_wait_cb) disp->driver->gpu_wait_cb(disp->driver);

    const lv_draw_backend_t * backend = lv_draw_backend_get();
    backend->blend_fill(disp_buf, stride, &draw_area, color, mask, opa, blend_mode);

}

void lv_draw_blend_map(const lv_area_t * clip_area, const lv_area_t * map_area,
                       const lv_color_t * map_buf,
                       lv_opa_t * mask, lv_draw_mask_res_t mask_res,
                       lv_opa_t opa, lv_blend_mode_t blend_mode)
{
    /*Do not draw transparent things*/
    if(opa < LV_OPA_MIN) return;
    if(mask_res == LV_DRAW_MASK_RES_TRANSP) return;
    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask = NULL;

    /*Get clipped fill area which is the real draw area.
     *It is always the same or inside `map_area`*/
    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, clip_area, map_area)) return;

    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    lv_disp_draw_buf_t * draw_buf = lv_disp_get_draw_buf(disp);
    const lv_area_t * disp_area = &draw_buf->area;
    lv_color_t * buf = draw_buf->buf_act;


    /*Now `draw_area` has absolute coordinates.
     *Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    lv_area_move(&draw_area, -disp_area->x1, -disp_area->y1);

    lv_area_t map_area_relative;
    lv_area_copy(&map_area_relative, map_area);
    lv_area_move(&map_area_relative, -disp_area->x1, -disp_area->y1);

    if(disp->driver->gpu_wait_cb) disp->driver->gpu_wait_cb(disp->driver);

    lv_coord_t stride = lv_area_get_width(disp_area);

    const lv_draw_backend_t * backend = lv_draw_backend_get();
    backend->blend_map(buf, stride, &draw_area,
                       map_buf, &map_area_relative,
                       mask, opa, blend_mode);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
