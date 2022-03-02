/**
 * @file lv_gpu_stm32_dma2d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gpu_stm32_dma2d.h"
#include "../../core/lv_refr.h"

#if LV_USE_GPU_STM32_DMA2D

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

static void lv_draw_stm32_dma2d_blend_fill(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area,
                                           lv_color_t color);


static void lv_draw_stm32_dma2d_blend_map(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                          const lv_color_t * src_buf, lv_coord_t src_stride, lv_opa_t opa);

static void lv_draw_stm32_dma2d_img_decoded(lv_draw_ctx_t * draw, const lv_draw_img_dsc_t * dsc,
                                            const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t color_format);


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


void lv_draw_stm32_dma2d_ctx_init(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{

    lv_draw_sw_init_ctx(drv, draw_ctx);

    lv_draw_stm32_dma2d_ctx_t * dma2d_draw_ctx = (lv_draw_sw_ctx_t *)draw_ctx;

    dma2d_draw_ctx->blend = lv_draw_stm32_dma2d_blend;
    //    dma2d_draw_ctx->base_draw.draw_img_decoded = lv_draw_stm32_dma2d_img_decoded;
    dma2d_draw_ctx->base_draw.wait_for_finish = lv_gpu_stm32_dma2d_wait_cb;

}

void lv_draw_stm32_dma2d_ctx_deinit(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    LV_UNUSED(drv);
    LV_UNUSED(draw_ctx);
}


void lv_draw_stm32_dma2d_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area)) return;

    bool done = false;

    if(dsc->mask_buf == NULL && dsc->blend_mode == LV_BLEND_MODE_NORMAL && lv_area_get_size(&blend_area) > 100) {
        lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);

        lv_color_t * dest_buf = draw_ctx->buf;
        dest_buf += dest_stride * (blend_area.y1 - draw_ctx->buf_area->y1) + (blend_area.x1 - draw_ctx->buf_area->x1);

        const lv_color_t * src_buf = dsc->src_buf;
        if(src_buf) {
            lv_draw_sw_blend_basic(draw_ctx, dsc);
            lv_coord_t src_stride;
            src_stride = lv_area_get_width(dsc->blend_area);
            src_buf += src_stride * (blend_area.y1 - dsc->blend_area->y1) + (blend_area.x1 -  dsc->blend_area->x1);
            lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
            lv_draw_stm32_dma2d_blend_map(dest_buf, &blend_area, dest_stride, src_buf, src_stride, dsc->opa);
            done = true;
        }
        else if(dsc->opa >= LV_OPA_MAX) {
            lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
            lv_draw_stm32_dma2d_blend_fill(dest_buf, dest_stride, &blend_area, dsc->color);
            done = true;
        }
    }

    if(!done) lv_draw_sw_blend_basic(draw_ctx, dsc);
}


static void lv_draw_stm32_dma2d_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                            const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t color_format)
{
    /*TODO basic ARGB8888 image can be handles here*/

    lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, color_format);
}

static void lv_draw_stm32_dma2d_blend_fill(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area,
                                           lv_color_t color)
{
    /*Simply fill an area*/
    int32_t area_w = lv_area_get_width(fill_area);
    int32_t area_h = lv_area_get_height(fill_area);
    invalidate_cache();

    DMA2D->CR = 0x30000;
    DMA2D->OMAR = (uint32_t)dest_buf;
    /*as input color mode is same as output we don't need to convert here do we?*/
    DMA2D->OCOLR = color.full;
    DMA2D->OOR = dest_stride - area_w;
    DMA2D->NLR = (area_w << DMA2D_NLR_PL_Pos) | (area_h << DMA2D_NLR_NL_Pos);

    /*start transfer*/
    DMA2D->CR |= DMA2D_CR_START_Msk;

}


static void lv_draw_stm32_dma2d_blend_map(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                          const lv_color_t * src_buf, lv_coord_t src_stride, lv_opa_t opa)
{

    /*Simple copy*/
    int32_t dest_w = lv_area_get_width(dest_area);
    int32_t dest_h = lv_area_get_height(dest_area);

    invalidate_cache();
    if(opa >= LV_OPA_MAX) {
        DMA2D->CR = 0;
        /*copy output colour mode, this register controls both input and output colour format*/
        DMA2D->FGPFCCR = LV_DMA2D_COLOR_FORMAT;
        DMA2D->FGMAR = (uint32_t)src_buf;
        DMA2D->FGOR = src_stride - dest_w;
        DMA2D->OMAR = (uint32_t)dest_buf;
        DMA2D->OOR = dest_stride - dest_w;
        DMA2D->NLR = (dest_w << DMA2D_NLR_PL_Pos) | (dest_h << DMA2D_NLR_NL_Pos);

        /*start transfer*/
        DMA2D->CR |= DMA2D_CR_START_Msk;
    }
    else {
        DMA2D->CR = 0x20000;

        DMA2D->BGPFCCR = LV_DMA2D_COLOR_FORMAT;
        DMA2D->BGMAR = (uint32_t)dest_buf;
        DMA2D->BGOR = dest_stride - dest_w;

        DMA2D->FGPFCCR = (uint32_t)LV_DMA2D_COLOR_FORMAT
                         /*alpha mode 2, replace with foreground * alpha value*/
                         | (2 << DMA2D_FGPFCCR_AM_Pos)
                         /*alpha value*/
                         | (opa << DMA2D_FGPFCCR_ALPHA_Pos);
        DMA2D->FGMAR = (uint32_t)src_buf;
        DMA2D->FGOR = src_stride - dest_w;

        DMA2D->OMAR = (uint32_t)src_buf;
        DMA2D->OOR = src_stride - dest_w;
        DMA2D->NLR = (dest_w << DMA2D_NLR_PL_Pos) | (dest_h << DMA2D_NLR_NL_Pos);

        /*start transfer*/
        DMA2D->CR |= DMA2D_CR_START_Msk;
    }
}

void lv_gpu_stm32_dma2d_wait_cb(lv_draw_ctx_t * draw_ctx)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    if(disp->driver && disp->driver->wait_cb) {
        while(DMA2D->CR & DMA2D_CR_START_Msk) {
            disp->driver->wait_cb(disp->driver);
        }
    }
    else {
        while(DMA2D->CR & DMA2D_CR_START_Msk);
    }
    lv_draw_sw_wait_for_finish(draw_ctx);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void invalidate_cache(void)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    if(disp->driver->clean_dcache_cb) disp->driver->clean_dcache_cb(disp->driver);
    else {
#if __CORTEX_M >= 0x07
        if((SCB->CCR) & (uint32_t)SCB_CCR_DC_Msk)
            SCB_CleanInvalidateDCache();
#endif
    }
}

#endif
