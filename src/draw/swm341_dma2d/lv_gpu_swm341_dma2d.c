/**
 * @file lv_gpu_swm341_dma2d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gpu_swm341_dma2d.h"
#include "../../core/lv_refr.h"

#if LV_USE_GPU_SWM341_DMA2D

#include LV_GPU_SWM341_DMA2D_INCLUDE

/*********************
 *      DEFINES
 *********************/

#if LV_COLOR_16_SWAP
    #error "Can't use DMA2D with LV_COLOR_16_SWAP 1"
#endif

#if LV_COLOR_DEPTH == 8
    #error "Can't use DMA2D with LV_COLOR_DEPTH == 8"
#endif

#if LV_COLOR_DEPTH == 16
    #define LV_DMA2D_COLOR_FORMAT LV_SWM341_DMA2D_RGB565
#elif LV_COLOR_DEPTH == 32
    #define LV_DMA2D_COLOR_FORMAT LV_SWM341_DMA2D_ARGB8888
#else
    /*Can't use GPU with other formats*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_draw_swm341_dma2d_blend_fill(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area,
                                            lv_color_t color);

static void lv_draw_swm341_dma2d_blend_map(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                           const lv_color_t * src_buf, lv_coord_t src_stride, lv_opa_t opa);

static void lv_draw_swm341_dma2d_img_decoded(lv_draw_ctx_t * draw, const lv_draw_img_dsc_t * dsc,
                                             const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t color_format);

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
void lv_draw_swm341_dma2d_init(void)
{
    /*Enable DMA2D clock*/
    SYS->CLKEN0 |= (1 << SYS_CLKEN0_DMA2D_Pos);

    DMA2D->CR &= ~DMA2D_CR_WAIT_Msk;
    DMA2D->CR |= (CyclesPerUs << DMA2D_CR_WAIT_Pos);

    DMA2D->IF = 0xFF;
    DMA2D->IE = (0 << DMA2D_IE_DONE_Pos);

    /*set output colour mode*/
    DMA2D->L[DMA2D_LAYER_OUT].PFCCR = (LV_DMA2D_COLOR_FORMAT << DMA2D_PFCCR_CFMT_Pos);
}

void lv_draw_swm341_dma2d_ctx_init(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{

    lv_draw_sw_init_ctx(drv, draw_ctx);

    lv_draw_swm341_dma2d_ctx_t * dma2d_draw_ctx = (lv_draw_sw_ctx_t *)draw_ctx;

    dma2d_draw_ctx->blend = lv_draw_swm341_dma2d_blend;
    //    dma2d_draw_ctx->base_draw.draw_img_decoded = lv_draw_swm341_dma2d_img_decoded;
    dma2d_draw_ctx->base_draw.wait_for_finish = lv_gpu_swm341_dma2d_wait_cb;
}

void lv_draw_swm341_dma2d_ctx_deinit(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    LV_UNUSED(drv);
    LV_UNUSED(draw_ctx);
}

void lv_draw_swm341_dma2d_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area))
        return;

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
            src_buf += src_stride * (blend_area.y1 - dsc->blend_area->y1) + (blend_area.x1 - dsc->blend_area->x1);
            lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
            lv_draw_swm341_dma2d_blend_map(dest_buf, &blend_area, dest_stride, src_buf, src_stride, dsc->opa);
            done = true;
        }
        else if(dsc->opa >= LV_OPA_MAX) {
            lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
            lv_draw_swm341_dma2d_blend_fill(dest_buf, dest_stride, &blend_area, dsc->color);
            done = true;
        }
    }

    if(!done) lv_draw_sw_blend_basic(draw_ctx, dsc);
}

static void lv_draw_swm341_dma2d_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                             const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t color_format)
{
    /*TODO basic ARGB8888 image can be handles here*/

    lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, color_format);
}

static void lv_draw_swm341_dma2d_blend_fill(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area,
                                            lv_color_t color)
{
    /*Simply fill an area*/
    int32_t area_w = lv_area_get_width(fill_area);
    int32_t area_h = lv_area_get_height(fill_area);

#if 1
    DMA2D->L[DMA2D_LAYER_OUT].COLOR = color.full;

    DMA2D->L[DMA2D_LAYER_OUT].MAR = (uint32_t)dest_buf;
    DMA2D->L[DMA2D_LAYER_OUT].OR = dest_stride - area_w;
    DMA2D->NLR = ((area_w - 1) << DMA2D_NLR_NPIXEL_Pos) | ((area_h - 1) << DMA2D_NLR_NLINE_Pos);

    /*start transfer*/
    DMA2D->CR &= ~DMA2D_CR_MODE_Msk;
    DMA2D->CR |= (3 << DMA2D_CR_MODE_Pos) |
                 (1 << DMA2D_CR_START_Pos);
#else
    for(uint32_t y = 0; y < area_h; y++) {
        for(uint32_t x = 0; x < area_w; x++) {
            dest_buf[y * dest_stride + x] = color;
        }
    }
#endif
}

static void lv_draw_swm341_dma2d_blend_map(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                           const lv_color_t * src_buf, lv_coord_t src_stride, lv_opa_t opa)
{

    /*Simple copy*/
    int32_t dest_w = lv_area_get_width(dest_area);
    int32_t dest_h = lv_area_get_height(dest_area);

    if(opa >= LV_OPA_MAX) {
#if 1
        /*copy output colour mode, this register controls both input and output colour format*/
        DMA2D->L[DMA2D_LAYER_FG].MAR = (uint32_t)src_buf;
        DMA2D->L[DMA2D_LAYER_FG].OR = src_stride - dest_w;
        DMA2D->L[DMA2D_LAYER_FG].PFCCR = (LV_DMA2D_COLOR_FORMAT << DMA2D_PFCCR_CFMT_Pos);

        DMA2D->L[DMA2D_LAYER_OUT].MAR = (uint32_t)dest_buf;
        DMA2D->L[DMA2D_LAYER_OUT].OR = dest_stride - dest_w;

        DMA2D->NLR = ((dest_w - 1) << DMA2D_NLR_NPIXEL_Pos) | ((dest_h - 1) << DMA2D_NLR_NLINE_Pos);

        /*start transfer*/
        DMA2D->CR &= ~DMA2D_CR_MODE_Msk;
        DMA2D->CR |= (0 << DMA2D_CR_MODE_Pos) |
                     (1 << DMA2D_CR_START_Pos);
#else
        lv_color_t temp_buf[1024];
        for(uint32_t y = 0; y < dest_h; y++) {
            memcpy(temp_buf, &src_buf[y * src_stride], dest_w * sizeof(lv_color_t));
            memcpy(&dest_buf[y * dest_stride], temp_buf, dest_w * sizeof(lv_color_t));
        }
#endif
    }
    else {
        DMA2D->L[DMA2D_LAYER_FG].MAR = (uint32_t)src_buf;
        DMA2D->L[DMA2D_LAYER_FG].OR = src_stride - dest_w;
        DMA2D->L[DMA2D_LAYER_FG].PFCCR = (LV_DMA2D_COLOR_FORMAT << DMA2D_PFCCR_CFMT_Pos)
                                         /*alpha mode 2, replace with foreground * alpha value*/
                                         | (2 << DAM2D_PFCCR_AMODE_Pos)
                                         /*alpha value*/
                                         | (opa << DMA2D_PFCCR_ALPHA_Pos);

        DMA2D->L[DMA2D_LAYER_BG].MAR = (uint32_t)dest_buf;
        DMA2D->L[DMA2D_LAYER_BG].OR = dest_stride - dest_w;
        DMA2D->L[DMA2D_LAYER_BG].PFCCR = (LV_DMA2D_COLOR_FORMAT << DMA2D_PFCCR_CFMT_Pos);

        DMA2D->L[DMA2D_LAYER_OUT].MAR = (uint32_t)dest_buf;
        DMA2D->L[DMA2D_LAYER_OUT].OR = dest_stride - dest_w;

        DMA2D->NLR = ((dest_w - 1) << DMA2D_NLR_NPIXEL_Pos) | ((dest_h - 1) << DMA2D_NLR_NLINE_Pos);

        /*start transfer*/
        DMA2D->CR &= ~DMA2D_CR_MODE_Msk;
        DMA2D->CR |= (2 << DMA2D_CR_MODE_Pos) |
                     (1 << DMA2D_CR_START_Pos);
    }
}

void lv_gpu_swm341_dma2d_wait_cb(lv_draw_ctx_t * draw_ctx)
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

#endif
