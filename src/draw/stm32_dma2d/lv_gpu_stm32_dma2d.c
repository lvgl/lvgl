/**
 * @file lv_gpu_stm32_dma2d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gpu_stm32_dma2d.h"
#include "../core/lv_disp.h"
#include "../core/lv_refr.h"

#if LV_USE_GPU_STM32_DMA2D == 0

#include LV_GPU_DMA2D_CMSIS_INCLUDE

/*********************
 *      DEFINES
 *********************/

#if LV_COLOR_16_SWAP
    // TODO: F7 has red blue swap bit in control register for all layers and output
    #error "Can't use DMA2D with LV_COLOR_16_SWAP 1"
#endif

#if LV_COLOR_DEPTH == 8
    #error "Can't use DMA2D with LV_COLOR_DEPTH == 8"
#endif

#if LV_COLOR_DEPTH == 16
    #define LV_DMA2D_COLOR_FORMAT LV_DMA2D_RGB565
#elif LV_COLOR_DEPTH == 32
    #define LV_DMA2D_COLOR_FORMAT LV_DMA2D_ARGB8888
#else
    /*Can't use GPU with other formats*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/


void lv_draw_stm32_dma2d_blend_fill(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area,
                     lv_color_t color, lv_opa_t * mask, lv_opa_t opa, lv_blend_mode_t blend_mode);


void lv_draw_stm32_dma2d_blend_map(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * clip_area,
                    const lv_color_t * src_buf, const lv_area_t * src_area,
                    lv_opa_t * mask, lv_opa_t opa, lv_blend_mode_t blend_mode);

/**
 * Can be used as `gpu_wait_cb` in display driver to
 * let the MCU run while the GPU is working
 */
void lv_gpu_stm32_dma2d_wait_cb(lv_disp_drv_t * drv);


static void invalidate_cache(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Turn on the peripheral and set output color mode, this only needs to be done once
 */
void lv_draw_stm32_dma2d_init(void)
{
    /*Enable DMA2D clock*/
#if defined(STM32F4) || defined(STM32F7)
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN;
#elif defined(STM32H7)
    RCC->AHB3ENR |= RCC_AHB3ENR_DMA2DEN;
#else
# warning "LVGL can't enable the clock of DMA2D"
#endif

    /*Wait for hardware access to complete*/
    __asm volatile("DSB\n");

    /*Delay after setting peripheral clock*/
    volatile uint32_t temp = RCC->AHB1ENR;
    LV_UNUSED(temp);

    /*set output colour mode*/
    DMA2D->OPFCCR = LV_DMA2D_COLOR_FORMAT;
}


lv_draw_t * lv_draw_stm32_dma2d_create(void)
{
    lv_draw_sw_t * draw = (lv_draw_sw_t *)lv_draw_sw_create();

    draw->blend = lv_draw_stm32_dma2d_blend;
    draw->base_draw.draw_img_core = lv_draw_stm32_dma2d_img_core;

    return (lv_draw_t *)draw;
}



void lv_draw_stm32_dma2d_blend(lv_draw_t * draw, const lv_draw_sw_blend_dsc_t * dsc)
{
    lv_draw_sw_blend(draw, dsc);
}


void lv_draw_stm32_dma2d_img_core(lv_draw_t * draw, const lv_draw_img_dsc_t * dsc)
{
    lv_draw_sw_img(draw, dsc);
}

//void lv_draw_stm32_dma2d_blend_fill(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area,
//                     lv_color_t color, lv_opa_t * mask, lv_opa_t opa, lv_blend_mode_t blend_mode)
//{
//    /*Simply fill an area*/
//    int32_t area_w = lv_area_get_width(fill_area);
//    int32_t area_h = lv_area_get_height(fill_area);
//    if(mask == NULL && opa >= LV_OPA_MAX && blend_mode == LV_BLEND_MODE_NORMAL && area_w*area_h > 240) {
//        invalidate_cache();
//
//        dest_buf += dest_stride * fill_area->y1 + fill_area->x1;
//
//        DMA2D->CR = 0x30000;
//        DMA2D->OMAR = (uint32_t)dest_buf;
//        /*as input color mode is same as output we don't need to convert here do we?*/
//        DMA2D->OCOLR = color.full;
//        DMA2D->OOR = dest_stride - area_w;
//        DMA2D->NLR = (area_w << DMA2D_NLR_PL_Pos) | (area_h << DMA2D_NLR_NL_Pos);
//
//        /*start transfer*/
//        DMA2D->CR |= DMA2D_CR_START_Msk;
//
//        return;
//    }
//}
//
//
//void lv_draw_stm32_dma2d_blend_map(const lv_draw_class_t * class_p, lv_draw_t * draw, const lv_draw_sw_blend_map_dsc_t * dsc)
//{
//
//    /*Simple copy*/
//    int32_t clip_w = lv_area_get_width(clip_area);
//    int32_t clip_h = lv_area_get_height(clip_area);
//
//    int32_t src_stride = lv_area_get_width(src_area);
//
//    dest_buf += dest_stride * clip_area->y1 + clip_area->x1;
//
//    src_buf += src_stride * (clip_area->y1 - src_area->y1);
//    src_buf += (clip_area->x1 - src_area->x1);
//
//    if(mask == NULL && blend_mode == LV_BLEND_MODE_NORMAL && clip_w * clip_h > 240) {
//        invalidate_cache();
//        if(opa >= LV_OPA_MAX) {
//            DMA2D->CR = 0;
//            /*copy output colour mode, this register controls both input and output colour format*/
//            DMA2D->FGPFCCR = LV_DMA2D_COLOR_FORMAT;
//            DMA2D->FGMAR = (uint32_t)src_buf;
//            DMA2D->FGOR = src_stride - clip_w;
//            DMA2D->OMAR = (uint32_t)dest_buf;
//            DMA2D->OOR = dest_stride - clip_w;
//            DMA2D->NLR = (clip_w << DMA2D_NLR_PL_Pos) | (clip_h << DMA2D_NLR_NL_Pos);
//
//            /*start transfer*/
//            DMA2D->CR |= DMA2D_CR_START_Msk;
//        } else {
//            DMA2D->CR = 0x20000;
//
//            DMA2D->BGPFCCR = LV_DMA2D_COLOR_FORMAT;
//            DMA2D->BGMAR = (uint32_t)dest_buf;
//            DMA2D->BGOR = dest_stride - clip_w;
//
//            DMA2D->FGPFCCR = (uint32_t)LV_DMA2D_COLOR_FORMAT
//                             /*alpha mode 2, replace with foreground * alpha value*/
//                             | (2 << DMA2D_FGPFCCR_AM_Pos)
//                             /*alpha value*/
//                             | (opa << DMA2D_FGPFCCR_ALPHA_Pos);
//            DMA2D->FGMAR = (uint32_t)src_buf;
//            DMA2D->FGOR = src_stride - clip_w;
//
//            DMA2D->OMAR = (uint32_t)src_buf;
//            DMA2D->OOR = src_stride - clip_w;
//            DMA2D->NLR = (clip_w << DMA2D_NLR_PL_Pos) | (clip_h << DMA2D_NLR_NL_Pos);
//
//            /*start transfer*/
//            DMA2D->CR |= DMA2D_CR_START_Msk;
//        }
//        return;
//    }
//
//    /*If not returned earlier call the base's blend function*/
//    const lv_draw_sw_class_t * base = (const lv_draw_sw_class_t *)class_p->base_class;
//    while(base->blend_map == NULL) {    /*Find the first base with set blend_map callback*/
//        base = (const lv_draw_sw_class_t *)base->base_class;
//    }
//
//    if(base) base->blend_map(base, draw, dsc);
//}
//
//
//void lv_gpu_stm32_dma2d_wait_cb(lv_disp_drv_t * drv)
//{
//    if(drv && drv->wait_cb) {
//        while(DMA2D->CR & DMA2D_CR_START_Msk) {
//            drv->wait_cb(drv);
//        }
//    }
//    else {
//        while(DMA2D->CR & DMA2D_CR_START_Msk);
//    }
//}

/**********************
 *   STATIC FUNCTIONS
 **********************/

//static void invalidate_cache(void)
//{
//    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
//    if(disp->driver->clean_dcache_cb) disp->driver->clean_dcache_cb(disp->driver);
//    else {
//#if __CORTEX_M >= 0x07
//        if((SCB->CCR) & (uint32_t)SCB_CCR_DC_Msk)
//            SCB_CleanInvalidateDCache();
//#endif
//    }
//}

#endif
