/**
 * @file lv_gpu_stm32_dma2d.h
 *
 */

#ifndef LV_GPU_STM32_DMA2D_H
#define LV_GPU_STM32_DMA2D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_color.h"
#include "../../hal/lv_hal_disp.h"
#include "../sw/lv_draw_sw.h"

#if LV_USE_GPU_STM32_DMA2D

/*********************
 *      DEFINES
 *********************/
#if defined(LV_STM32_DMA2D_TEST)
// removes "static" modifier for some internal methods in order to test them
#define LV_STM32_DMA2D_STATIC
#else
#define LV_STM32_DMA2D_STATIC static
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef lv_draw_sw_ctx_t lv_draw_stm32_dma2d_ctx_t;
struct _lv_disp_drv_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_draw_stm32_dma2d_init(void);

void lv_draw_stm32_dma2d_ctx_init(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

void lv_draw_stm32_dma2d_ctx_deinit(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

void lv_draw_stm32_dma2d_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);

void lv_draw_stm32_dma2d_buffer_copy(lv_draw_ctx_t * draw_ctx,
                                     void * dest_buf, lv_coord_t dest_stride, const lv_area_t * dest_area,
                                     void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area);

lv_res_t lv_draw_stm32_dma2d_img(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                 const lv_area_t * src_area, const void * src);

void lv_gpu_stm32_dma2d_wait_cb(lv_draw_ctx_t * draw_ctx);

/**********************
 *  STATIC PROTOTYPES
 **********************/
LV_STM32_DMA2D_STATIC void _lv_draw_stm32_dma2d_blend_fill(const lv_color_t * dst_buf, lv_coord_t dst_stride,
                                                           const lv_area_t * draw_area, lv_color_t color, lv_opa_t opa);
LV_STM32_DMA2D_STATIC void _lv_draw_stm32_dma2d_blend_map(const lv_color_t * dest_buf, lv_coord_t dest_stride,
                                                          const lv_area_t * draw_area, const void * src_buf, lv_coord_t src_stride, const lv_point_t * src_offset,
                                                          lv_opa_t opa, bool isSrcArgb32);
LV_STM32_DMA2D_STATIC void _lv_draw_stm32_dma2d_blend_paint(const lv_color_t * dst_buf, lv_coord_t dst_stride,
                                                            const lv_area_t * draw_area, const lv_opa_t * mask_buf, lv_coord_t mask_stride, const lv_point_t * mask_offset,
                                                            lv_color_t color, lv_opa_t opa);
LV_STM32_DMA2D_STATIC lv_res_t _lv_draw_stm32_dma2d_img(lv_draw_ctx_t * draw, const lv_draw_img_dsc_t * dsc,
                                                        const lv_area_t * coords,
                                                        const void * src);
LV_STM32_DMA2D_STATIC void _lv_draw_stm32_dma2d_copy_buffer(const lv_color_t * dest_buf, lv_coord_t dest_stride,
                                                            const lv_area_t * draw_area, const void * src_buf, lv_coord_t src_stride, const lv_point_t * src_offset);
LV_STM32_DMA2D_STATIC void _lv_gpu_stm32_dma2d_wait_for_dma_transfer_to_finish(lv_disp_drv_t * disp_drv);
LV_STM32_DMA2D_STATIC void _lv_gpu_stm32_dma2d_start_dma_transfer(void);
LV_STM32_DMA2D_STATIC void _lv_gpu_stm32_dma2d_invalidate_cache(uint32_t sourceAddress, lv_coord_t offset,
                                                                lv_coord_t width, lv_coord_t height,
                                                                uint8_t pixelSize);
LV_STM32_DMA2D_STATIC void _lv_gpu_stm32_dma2d_clean_cache(uint32_t sourceAddress, lv_coord_t offset, lv_coord_t width,
                                                           lv_coord_t height,
                                                           uint8_t pixelSize);

static bool _lv_gpu_stm32_dma2d_dwt_init(void);
static void _lv_gpu_stm32_dma2d_dwt_reset(void);
static uint32_t _lv_gpu_stm32_dma2d_dwt_get_us(void);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_GPU_STM32_DMA2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GPU_STM32_DMA2D_H*/
