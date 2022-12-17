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

#include "stdio.h"
#include "main.h"
#include LV_GPU_DMA2D_CMSIS_INCLUDE

/*********************
 *      DEFINES
 *********************/

#if LV_COLOR_16_SWAP
    // Note: DMA2D red/blue swap (RBS) works for (A)RGB 24/32 color as well
    #define RBS_BIT 1U
#else
    #define RBS_BIT 0U
#endif

#if LV_COLOR_DEPTH == 16
    #define DMA2D_INPUT_COLOR DMA2D_INPUT_RGB565
    #define DMA2D_OUTPUT_COLOR DMA2D_OUTPUT_RGB565
#elif LV_COLOR_DEPTH == 32
    #define DMA2D_INPUT_COLOR DMA2D_INPUT_ARGB8888
    #define DMA2D_OUTPUT_COLOR DMA2D_OUTPUT_ARGB8888
#else
    #error "Cannot use DMA2D with LV_COLOR_DEPTH other than 16 or 32"
#endif

#define CACHE_ROW_SIZE 32U // cache row size in Bytes

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static bool isDma2dInProgess = false; // indicates whether DMA2D transfer *initiated here* is in progress

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


// astyle --options=lvgl/scripts/code-format.cfg --ignore-exclude-errors lvgl/src/draw/stm32_dma2d/*.c lvgl/src/draw/stm32_dma2d/*.h

/**
 * Turn on the peripheral and set output color mode, this only needs to be done once
 */
void lv_draw_stm32_dma2d_init(void)
{
    /*Enable DMA2D clock*/
#if defined(STM32F4) || defined(STM32F7)
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN; // enable DMA2D
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

    // AHB master timer configuration
    DMA2D->AMTCR = 0; // AHB bus guaranteed dead time disabled
}

void lv_draw_stm32_dma2d_ctx_init(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    lv_draw_sw_init_ctx(drv, draw_ctx);

    lv_draw_stm32_dma2d_ctx_t * dma2d_draw_ctx = (lv_draw_sw_ctx_t *)draw_ctx;

    dma2d_draw_ctx->blend = lv_draw_stm32_dma2d_blend;
    dma2d_draw_ctx->base_draw.draw_img = lv_draw_stm32_dma2d_img;
    dma2d_draw_ctx->base_draw.wait_for_finish = lv_gpu_stm32_dma2d_wait_cb;
    dma2d_draw_ctx->base_draw.buffer_copy = lv_draw_stm32_dma2d_buffer_copy;
}

void lv_draw_stm32_dma2d_ctx_deinit(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    LV_UNUSED(drv);
    LV_UNUSED(draw_ctx);
}

void lv_draw_stm32_dma2d_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    if(dsc->blend_mode != LV_BLEND_MODE_NORMAL) {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
        return;
    }

    // Both draw buffer start address and buffer size *must* be 32-byte aligned since draw buffer cache is being invalidated.
    uint32_t drawBufferLength = lv_area_get_size(draw_ctx->buf_area) * sizeof(lv_color_t);
    assert_param(drawBufferLength % CACHE_ROW_SIZE == 0); // critical
    assert_param((uint32_t)draw_ctx->buf % CACHE_ROW_SIZE == 0); // critical

    if(dsc->src_buf) {
        // For performance reasons, both source buffer start address and buffer size *should* be 32-byte aligned since source buffer cache is being cleaned.
        uint32_t srcBufferLength = lv_area_get_size(dsc->blend_area) * sizeof(lv_color_t);
        //assert_param(srcBufferLength % CACHE_ROW_SIZE == 0); // FIXME: assert fails (performance, non-critical)
        //assert_param((uint32_t)dsc->src_buf % CACHE_ROW_SIZE == 0); // FIXME: assert fails (performance, non-critical)
    }

    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, dsc->blend_area, draw_ctx->clip_area)) return;
    // + draw_ctx->buf_area has the entire draw buffer location
    // + draw_ctx->clip_area has the current draw buffer location
    // + dsc->blend_area has the location of the area intended to be painted - image etc.
    // + draw_area has the area actually being painted
    // All coordinates are relative to the screen.

    const lv_opa_t * mask = dsc->mask_buf;

    if(dsc->mask_buf && dsc->mask_res == LV_DRAW_MASK_RES_TRANSP) return;
    else if(dsc->mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask = NULL;

    lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);

    if(mask != NULL) {
        // For performance reasons, both mask buffer start address and buffer size *should* be 32-byte aligned since mask buffer cache is being cleaned.
        uint32_t srcBufferLength = lv_area_get_size(dsc->mask_area) * sizeof(lv_opa_t);
        //assert_param(srcBufferLength % CACHE_ROW_SIZE == 0); // FIXME: assert fails (performance, non-critical)
        //assert_param((uint32_t)mask % CACHE_ROW_SIZE == 0); // FIXME: assert fails (performance, non-critical)

        lv_coord_t mask_stride = lv_area_get_width(dsc->mask_area);
        lv_point_t mask_offset; // mask offset in relation to draw_area
        mask_offset.x = draw_area.x1 - dsc->mask_area->x1;
        mask_offset.y = draw_area.y1 - dsc->mask_area->y1;

        if(dsc->src_buf == NULL) {  // 93.5%
            lv_area_move(&draw_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
            lv_draw_stm32_dma2d_blend_paint(draw_ctx->buf, dest_stride, &draw_area, mask, mask_stride, &mask_offset, dsc->color,
                                            dsc->opa);
        }
        else {   // 0.2%
            // note: (x)RGB dsc->src_buf does not carry alpha channel bytes,
            // alpha channel bytes are carried in dsc->mask_buf
#if LV_COLOR_DEPTH == 32
            lv_coord_t src_stride = lv_area_get_width(dsc->blend_area);
            lv_point_t src_offset; // source image offset in relation to draw_area
            src_offset.x = draw_area.x1 - dsc->blend_area->x1;
            src_offset.y = draw_area.y1 - dsc->blend_area->y1;
            lv_coord_t draw_width = lv_area_get_width(&draw_area);
            lv_coord_t draw_height = lv_area_get_height(&draw_area);

            // merge mask alpha bytes with src RGB bytes
            // TODO: optimize by reading 4 or 8 mask bytes at a time
            mask += (mask_stride * mask_offset.y) + mask_offset.x;
            lv_color_t * src_buf = (lv_color_t *)dsc->src_buf;
            src_buf += (src_stride * src_offset.y) + src_offset.x;
            uint16_t mask_buffer_offset = mask_stride - draw_width;
            uint16_t src_buffer_offset = src_stride - draw_width;
            while(draw_height > 0) {
                draw_height--;
                for(uint16_t x = 0; x < draw_width; x++) {
                    (*src_buf).ch.alpha = *mask;
                    src_buf++;
                    mask++;
                }
                mask += mask_buffer_offset;
                src_buf += src_buffer_offset;
            }

            lv_area_move(&draw_area, -draw_ctx->buf_area->x1,
                         -draw_ctx->buf_area->y1); // translate the screen draw area to the origin of the buffer area
            lv_draw_stm32_dma2d_blend_map(draw_ctx->buf, dest_stride, &draw_area, dsc->src_buf, src_stride, &src_offset, dsc->opa,
                                          true);
#else
            // Note: 16-bit bitmap hardware blending with mask and background is possible, but requires a temp 24 or 32-bit buffer to combine bitmap with mask first.
            // In case of 32-bit bitmap existing buffer is used
            lv_draw_sw_blend_basic(draw_ctx, dsc);
#endif
        }
    }
    else {
        if(dsc->src_buf == NULL) {  // 6.1%
            lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);
            lv_area_move(&draw_area, -draw_ctx->buf_area->x1,
                         -draw_ctx->buf_area->y1); // translate the screen draw area to the origin of the buffer area
            lv_draw_stm32_dma2d_blend_fill(draw_ctx->buf, dest_stride, &draw_area, dsc->color, dsc->opa);
        }
        else {   // 0.2%
            lv_coord_t src_stride = lv_area_get_width(dsc->blend_area);
            lv_point_t src_offset; // source image offset in relation to draw_area
            src_offset.x = draw_area.x1 - dsc->blend_area->x1;
            src_offset.y = draw_area.y1 - dsc->blend_area->y1;
            lv_area_move(&draw_area, -draw_ctx->buf_area->x1,
                         -draw_ctx->buf_area->y1); // translate the screen draw area to the origin of the buffer area
            lv_draw_stm32_dma2d_blend_map(draw_ctx->buf, dest_stride, &draw_area, dsc->src_buf, src_stride, &src_offset, dsc->opa,
                                          false);
        }
    }

    //waitForDmaTransferToFinish(NULL); // FIXME: this line should not be needed here, but it is
}

// Does dest_stride = width(draw_ctx->buf_area) ?
// Does dest_area = intersect(draw_ctx->clip_area, src_area) ?
void lv_draw_stm32_dma2d_buffer_copy(lv_draw_ctx_t * draw_ctx, void * dest_buf, lv_coord_t dest_stride,
                                     const lv_area_t * dest_area, void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area)
{
    // FIXME:
    // 1. Both src_buf and dest_buf pixel size *must* be known to use DMA2D.
    // 2. Verify both buffers start addresses and lengths are 32-byte (cache row size) aligned.
    LV_UNUSED(draw_ctx);
    lv_point_t src_offset;
    src_offset.x = dest_area->x1 - src_area->x1;
    src_offset.y = dest_area->y1 - src_area->y1;
    // FIXME: use lv_area_move(dest_area, -dest_area->x1, -dest_area->y1) here ?
    // TODO: It is assumed that dest_buf and src_buf buffers are of lv_color_t type. Verify it, this assumption may be incorrect.
    lv_draw_stm32_dma2d_blend_map(dest_buf, dest_stride, dest_area, src_buf, src_stride, &src_offset, 0xff, false);
    waitForDmaTransferToFinish(NULL); // TODO: is this line needed here?
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

STATIC lv_res_t lv_draw_stm32_dma2d_img(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                        const lv_area_t * src_area, const void * src)
{
    // FIXME: src pixel size *must* be known to use DMA2D

    // Both draw buffer start address and buffer size *must* be 32-byte aligned since draw buffer cache is being invalidated.
    uint32_t drawBufferLength = lv_area_get_size(draw_ctx->buf_area) * sizeof(lv_color_t);
    assert_param(drawBufferLength % CACHE_ROW_SIZE == 0); // critical
    assert_param((uint32_t)draw_ctx->buf % CACHE_ROW_SIZE == 0); // critical

    // For performance reasons, both source buffer start address and buffer size *should* be 32-byte aligned since source buffer cache is being cleaned.
    uint32_t srcBufferLength = lv_area_get_size(src_area) * sizeof(
                                   lv_color_t); // TODO: verify src pixel size = sizeof(lv_color_t)
    //assert_param(srcBufferLength % CACHE_ROW_SIZE == 0); // FIXME: assert fails (performance, non-critical)
    //assert_param((uint32_t)src % CACHE_ROW_SIZE == 0); // FIXME: assert fails (performance, non-critical)

    if(lv_img_src_get_type(src) == LV_IMG_SRC_VARIABLE) {
        lv_area_t draw_area;
        if(!_lv_area_intersect(&draw_area, src_area, draw_ctx->clip_area)) return LV_RES_OK;
        const lv_img_dsc_t * img = src;

        if(img->header.cf == LV_IMG_CF_RGBA8888 && dsc->angle == 0 && dsc->zoom == 256) {
            // note: LV_IMG_CF_RGBA8888 is actually ARGB8888
            lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);
            lv_point_t src_offset; // source image offset in relation to draw_area
            src_offset.x = draw_area.x1 - src_area->x1;
            src_offset.y = draw_area.y1 - src_area->y1;
            lv_area_move(&draw_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
            // TODO: It is assumed that img->data buffer is of lv_color_t type. Verify it, this assumption may be incorrect.
            lv_draw_stm32_dma2d_blend_map(draw_ctx->buf, dest_stride, &draw_area, (lv_color_t *)img->data, img->header.w,
                                          &src_offset,
                                          dsc->opa, true);
            return LV_RES_OK;
        }
    }

    return LV_RES_INV;
}

/**
 * @brief Fills draw_area with specified color.
 * @param color color to be painted, note: alpha is ignored
 */
STATIC void lv_draw_stm32_dma2d_blend_fill(const lv_color_t * dest_buf, lv_coord_t dest_stride,
                                           const lv_area_t * draw_area, lv_color_t color, lv_opa_t opa)
{
    assert_param(!isDma2dInProgess); // critical
    lv_coord_t draw_width = lv_area_get_width(draw_area);
    lv_coord_t draw_height = lv_area_get_height(draw_area);

    waitForDmaTransferToFinish(NULL);

    if(opa >= LV_OPA_MAX) {
        DMA2D->CR = 0x3UL << DMA2D_CR_MODE_Pos; // Register-to-memory (no FG nor BG, only output stage active)

        DMA2D->OPFCCR = DMA2D_OUTPUT_COLOR;
        DMA2D->OPFCCR |= (RBS_BIT << DMA2D_OPFCCR_RBS_Pos);
        DMA2D->OMAR = (uint32_t)(dest_buf + (dest_stride * draw_area->y1) + draw_area->x1);
        DMA2D->OOR = dest_stride - draw_width;  // out buffer offset
        DMA2D->OCOLR = color.full | (0xff << 24);
    }
    else {
        DMA2D->CR = 0x2UL << DMA2D_CR_MODE_Pos; // Memory-to-memory with blending (FG and BG fetch with PFC and blending)

        DMA2D->FGPFCCR = DMA2D_INPUT_A8;
        DMA2D->FGPFCCR |= (opa << DMA2D_FGPFCCR_ALPHA_Pos);
        DMA2D->FGPFCCR |= (0x1UL <<
                           DMA2D_FGPFCCR_AM_Pos); // Alpha Mode 1: Replace original foreground image alpha channel value by ALPHA[7:0]
        DMA2D->FGPFCCR |= (RBS_BIT << DMA2D_FGPFCCR_RBS_Pos);

        // Note: in Alpha Mode 1 FGMAR and FGOR are not used to supply foreground A8 bytes,
        // those bytes are replaced by constant ALPHA defined in FGPFCCR
        DMA2D->FGMAR = (uint32_t)dest_buf;
        DMA2D->FGOR = dest_stride;
        DMA2D->FGCOLR = color.full;  // used in A4 and A8 modes only, alpha is ignored

        DMA2D->BGPFCCR = DMA2D_INPUT_COLOR;
        DMA2D->BGPFCCR |= (RBS_BIT << DMA2D_BGPFCCR_RBS_Pos);
        DMA2D->BGMAR = (uint32_t)(dest_buf + (dest_stride * draw_area->y1) + draw_area->x1);
        DMA2D->BGOR = dest_stride - draw_width;
        DMA2D->BGCOLR = 0;  // used in A4 and A8 modes only
        cleanCache(DMA2D->BGMAR, DMA2D->BGOR, draw_width, draw_height, sizeof(lv_color_t));

        DMA2D->OPFCCR = DMA2D_OUTPUT_COLOR;
        DMA2D->OPFCCR |= (RBS_BIT << DMA2D_OPFCCR_RBS_Pos);
        DMA2D->OMAR = DMA2D->BGMAR;
        DMA2D->OOR = DMA2D->BGOR;
        DMA2D->OCOLR = 0;
    }
    // PL - pixel per lines (14 bit), NL - number of lines (16 bit)
    DMA2D->NLR = (uint32_t)((draw_width << DMA2D_NLR_PL_Pos) & DMA2D_NLR_PL_Msk) | ((draw_height << DMA2D_NLR_NL_Pos) &
                                                                                    DMA2D_NLR_NL_Msk);

    startDmaTransfer();
}

/**
 * @brief Draws src (foreground) map on dst (background) map.
 * @param src_offset src offset in relation to dst, useful when src is larger than draw_area
 * @param opa constant opacity to be applied
 * @param isSrcArgb if TRUE, source buffer is ARGB, otherwise source buffer is xRGB
 */
STATIC void lv_draw_stm32_dma2d_blend_map(const lv_color_t * dest_buf, lv_coord_t dest_stride,
                                          const lv_area_t * draw_area, const lv_color_t * src_buf, lv_coord_t src_stride, const lv_point_t * src_offset,
                                          lv_opa_t opa, bool isSrcArgb)
{
    assert_param(!isDma2dInProgess); // critical
    lv_coord_t draw_width = lv_area_get_width(draw_area);
    lv_coord_t draw_height = lv_area_get_height(draw_area);

    waitForDmaTransferToFinish(NULL);

    if(opa >= LV_OPA_MAX) opa = 0xff;

    DMA2D->FGPFCCR = DMA2D_INPUT_COLOR;

    if(isSrcArgb) {
        // src is ARGB
        DMA2D->CR = 0x2UL << DMA2D_CR_MODE_Pos; // Memory-to-memory with blending (FG and BG fetch with PFC and blending)
        DMA2D->FGPFCCR |= (opa << DMA2D_FGPFCCR_ALPHA_Pos);
        DMA2D->FGPFCCR |= (0x2UL <<
                           DMA2D_FGPFCCR_AM_Pos); // Alpha Mode 2: Replace original foreground image alpha channel value by ALPHA[7:0] multiplied with original alpha channel value
    }
    else {
        // src is xRGB
        if(opa == 0xff) {
            // no need to blend
            DMA2D->CR = 0x1UL << DMA2D_CR_MODE_Pos; // Memory-to-memory with PFC (FG fetch only with FG PFC active)
            // Alpha Mode 0: No modification of the foreground image alpha channel value
        }
        else {
            // blend with constant ALPHA only
            DMA2D->CR = 0x2UL << DMA2D_CR_MODE_Pos; // Memory-to-memory with blending (FG and BG fetch with PFC and blending)
            DMA2D->FGPFCCR |= (opa << DMA2D_FGPFCCR_ALPHA_Pos);
            DMA2D->FGPFCCR |= (0x1UL <<
                               DMA2D_FGPFCCR_AM_Pos); // Alpha Mode 1: Replace original foreground image alpha channel value by ALPHA[7:0]
        }
    }

    DMA2D->FGPFCCR |= (RBS_BIT << DMA2D_FGPFCCR_RBS_Pos);
    DMA2D->FGMAR = (uint32_t)(src_buf + (src_stride * src_offset->y) + src_offset->x);
    DMA2D->FGOR = src_stride - draw_width;
    DMA2D->FGCOLR = 0;  // used in A4 and A8 modes only
    cleanCache(DMA2D->FGMAR, DMA2D->FGOR, draw_width, draw_height, sizeof(lv_color_t));

    DMA2D->BGPFCCR = DMA2D_INPUT_COLOR;
    DMA2D->BGPFCCR |= (RBS_BIT << DMA2D_BGPFCCR_RBS_Pos);
    DMA2D->BGMAR = (uint32_t)(dest_buf + (dest_stride * draw_area->y1) + draw_area->x1);
    DMA2D->BGOR = dest_stride - draw_width;
    DMA2D->BGCOLR = 0;  // used in A4 and A8 modes only
    cleanCache(DMA2D->BGMAR, DMA2D->BGOR, draw_width, draw_height, sizeof(lv_color_t));

    DMA2D->OPFCCR = DMA2D_OUTPUT_COLOR;
    DMA2D->OPFCCR |= (RBS_BIT << DMA2D_OPFCCR_RBS_Pos);
    DMA2D->OMAR = DMA2D->BGMAR;
    DMA2D->OOR = DMA2D->BGOR;
    DMA2D->OCOLR = 0;

    // PL - pixel per lines (14 bit), NL - number of lines (16 bit)
    DMA2D->NLR = (uint32_t)((draw_width << DMA2D_NLR_PL_Pos) & DMA2D_NLR_PL_Msk) | ((draw_height << DMA2D_NLR_NL_Pos) &
                                                                                    DMA2D_NLR_NL_Msk);

    startDmaTransfer();
}

/**
 * @brief Paints solid color with alpha mask with additional constant opacity. Useful e.g. for painting anti-aliased fonts.
 * @param src_offset src offset in relation to dst, useful when src (alpha mask) is larger than draw_area
 * @param color color to paint, note: alpha is ignored
 * @param opa constant opacity to be applied
 */
STATIC void lv_draw_stm32_dma2d_blend_paint(const lv_color_t * dest_buf, lv_coord_t dest_stride,
                                            const lv_area_t * draw_area, const lv_opa_t * mask_buf, lv_coord_t mask_stride, const lv_point_t * mask_offset,
                                            lv_color_t color, lv_opa_t opa)
{
    assert_param(!isDma2dInProgess); // critical
    lv_coord_t draw_width = lv_area_get_width(draw_area);
    lv_coord_t draw_height = lv_area_get_height(draw_area);

    waitForDmaTransferToFinish(NULL);

    DMA2D->CR = 0x2UL << DMA2D_CR_MODE_Pos;  // Memory-to-memory with blending (FG and BG fetch with PFC and blending)

    DMA2D->FGPFCCR = DMA2D_INPUT_A8;
    if(opa < LV_OPA_MAX) {
        DMA2D->FGPFCCR |= (opa << DMA2D_FGPFCCR_ALPHA_Pos);
        DMA2D->FGPFCCR |= (0x2UL <<
                           DMA2D_FGPFCCR_AM_Pos); // Alpha Mode: Replace original foreground image alpha channel value by ALPHA[7:0] multiplied with original alpha channel value
    }
    DMA2D->FGMAR = (uint32_t)(mask_buf + (mask_stride * mask_offset->y) + mask_offset->x);
    DMA2D->FGOR = mask_stride - draw_width;
    DMA2D->FGCOLR = (lv_color_to32(color) | (0xff << 24));  // used in A4 and A8 modes only
    cleanCache(DMA2D->FGMAR, DMA2D->FGOR, draw_width, draw_height, sizeof(lv_opa_t)); // adjust for 8bit mask

    DMA2D->BGPFCCR = DMA2D_INPUT_COLOR;
    DMA2D->BGPFCCR |= (RBS_BIT << DMA2D_BGPFCCR_RBS_Pos);
    DMA2D->BGMAR = (uint32_t)(dest_buf + (dest_stride * draw_area->y1) + draw_area->x1);
    DMA2D->BGOR = dest_stride - draw_width;
    DMA2D->BGCOLR = 0;  // used in A4 and A8 modes only
    cleanCache(DMA2D->BGMAR, DMA2D->BGOR, draw_width, draw_height, sizeof(lv_color_t));

    DMA2D->OPFCCR = DMA2D_OUTPUT_COLOR;
    DMA2D->OPFCCR |= (RBS_BIT << DMA2D_OPFCCR_RBS_Pos);
    DMA2D->OMAR = DMA2D->BGMAR;
    DMA2D->OOR = DMA2D->BGOR;
    DMA2D->OCOLR = 0;
    // PL - pixel per lines (14 bit), NL - number of lines (16 bit)
    DMA2D->NLR = (uint32_t)((draw_width << DMA2D_NLR_PL_Pos) & DMA2D_NLR_PL_Msk) | ((draw_height << DMA2D_NLR_NL_Pos) &
                                                                                    DMA2D_NLR_NL_Msk);

    startDmaTransfer();
}

void lv_gpu_stm32_dma2d_wait_cb(lv_draw_ctx_t * draw_ctx)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    waitForDmaTransferToFinish(disp->driver);
    lv_draw_sw_wait_for_finish(draw_ctx);
}

static void startDmaTransfer(void)
{
    assert_param(!isDma2dInProgess);
    isDma2dInProgess = true;
    DMA2D->IFCR = 0x3FU; // trigger ISR flags reset
    DMA2D->CR |= DMA2D_CR_START;
}

static void waitForDmaTransferToFinish(lv_disp_drv_t * disp_drv)
{
    if(disp_drv && disp_drv->wait_cb) {
        while((DMA2D->CR & DMA2D_CR_START) != 0U) {
            disp_drv->wait_cb(disp_drv);
        }
    }
    else {
        while((DMA2D->CR & DMA2D_CR_START) != 0U);
    }

    __IO uint32_t isrFlags = DMA2D->ISR;

    if(isrFlags & DMA2D_ISR_CEIF) {
        printf("DMA2D config error\n");
    }

    if(isrFlags & DMA2D_ISR_TEIF) {
        printf("DMA2D transfer error\n");
    }

    DMA2D->IFCR = 0x3FU; // trigger ISR flags reset

    if(isDma2dInProgess) {
        // invalidate output buffer cached memory ONLY after DMA2D transfer
        invalidateCache(DMA2D->OMAR, DMA2D->OOR, (DMA2D->NLR & DMA2D_NLR_PL_Msk) >> DMA2D_NLR_PL_Pos,
                        (DMA2D->NLR & DMA2D_NLR_NL_Msk) >> DMA2D_NLR_NL_Pos, LV_IMG_PX_SIZE_ALPHA_BYTE);
        isDma2dInProgess = false;
    }
}

static void invalidateCache(uint32_t address, lv_coord_t offset, lv_coord_t width, lv_coord_t height, uint8_t pixelSize)
{
    if(((SCB->CCR) & SCB_CCR_DC_Msk) == 0) return; // L1 data cache is disabled
    uint16_t stride = pixelSize * (width + offset); // in bytes
    uint16_t ll = pixelSize * width; // line length in bytes
    uint32_t n = 0; // address of the next cache row after the last invalidated row
    lv_coord_t h = 0;

    __DSB();

    while(h < height) {
        uint32_t a = address + (h * stride);
        uint32_t e = a + ll; // end address, address of the first byte after the current line
        a &= ~(CACHE_ROW_SIZE - 1U);
        if(a < n) a = n;  // prevent the previous last cache row from being invalidated again

        while(a < e) {
            SCB->DCIMVAC = a;
            a += CACHE_ROW_SIZE;
        }

        n = a;
        h++;
    };

    __DSB();
    __ISB();
}

static void cleanCache(uint32_t address, lv_coord_t offset, lv_coord_t width, lv_coord_t height, uint8_t pixelSize)
{
    if(((SCB->CCR) & SCB_CCR_DC_Msk) == 0) return; // L1 data cache is disabled
    uint16_t stride = pixelSize * (width + offset); // in bytes
    uint16_t ll = pixelSize * width; // line length in bytes
    uint32_t n = 0; // address of the next cache row after the last cleaned row
    lv_coord_t h = 0;
    __DSB();

    while(h < height) {
        uint32_t a = address + (h * stride);
        uint32_t e = a + ll; // end address, address of the first byte after the current line
        a &= ~(CACHE_ROW_SIZE - 1U);
        if(a < n) a = n;  // prevent the previous last cache row from being cleaned again

        while(a < e) {
            SCB->DCCMVAC = a;
            a += CACHE_ROW_SIZE;
        }

        n = a;
        h++;
    };

    __DSB();
    __ISB();
}

#endif
