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

/*********************
 *      DEFINES
 *********************/
#if LV_COLOR_16_SWAP
    // Note: DMA2D red/blue swap (RBS) works for all color modes
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

// For code/implementation discussion refer to https://github.com/lvgl/lvgl/issues/3714#issuecomment-1365187036
// astyle --options=lvgl/scripts/code-format.cfg --ignore-exclude-errors lvgl/src/draw/stm32_dma2d/*.c lvgl/src/draw/stm32_dma2d/*.h

static bool isDma2dInProgess = false; // indicates whether DMA2D transfer *initiated here* is in progress

/**
 * Turn on the peripheral and set output color mode, this only needs to be done once
 */
void lv_draw_stm32_dma2d_init(void)
{
    // Enable DMA2D clock
#if defined(STM32F4) || defined(STM32F7)
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN; // enable DMA2D
#elif defined(STM32H7)
    RCC->AHB3ENR |= RCC_AHB3ENR_DMA2DEN;
#else
# warning "LVGL can't enable the clock of DMA2D"
#endif

    // Wait for hardware access to complete
    __asm volatile("DSB\n");

    // Delay after setting peripheral clock
    volatile uint32_t temp = RCC->AHB1ENR;
    LV_UNUSED(temp);

    // AHB master timer configuration
    DMA2D->AMTCR = 0; // AHB bus guaranteed dead time disabled
#if defined(LV_STM32_DMA2D_TEST)
    _lv_gpu_stm32_dwt_init(); // init µs timer
#endif
}

void lv_draw_stm32_dma2d_ctx_init(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    lv_draw_sw_init_ctx(drv, draw_ctx);

    lv_draw_stm32_dma2d_ctx_t * dma2d_draw_ctx = (lv_draw_sw_ctx_t *)draw_ctx;

    dma2d_draw_ctx->blend = lv_draw_stm32_dma2d_blend;
    dma2d_draw_ctx->base_draw.draw_img = lv_draw_stm32_dma2d_img;
    // Note: currently it does not make sense use lv_gpu_stm32_dma2d_wait_cb() since waiting starts right after the dma2d transfer
    //dma2d_draw_ctx->base_draw.wait_for_finish = lv_gpu_stm32_dma2d_wait_cb;
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
    // Note: x1 must be zero. Otherwise, there is no way to correctly calculate dest_stride.
    //assert_param(draw_ctx->buf_area->x1 == 0); // critical
    // Both draw buffer start address and buffer size *must* be 32-byte aligned since draw buffer cache is being invalidated.
    //uint32_t drawBufferLength = lv_area_get_size(draw_ctx->buf_area) * sizeof(lv_color_t);
    //assert_param(drawBufferLength % CACHE_ROW_SIZE == 0); // critical, but this is not the way to test it
    assert_param((uint32_t)draw_ctx->buf % CACHE_ROW_SIZE == 0); // critical

    if(dsc->src_buf) {
        // For performance reasons, both source buffer start address and buffer size *should* be 32-byte aligned since source buffer cache is being cleaned.
        //uint32_t srcBufferLength = lv_area_get_size(dsc->blend_area) * sizeof(lv_color_t);
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
        //uint32_t srcBufferLength = lv_area_get_size(dsc->mask_area) * sizeof(lv_opa_t);
        //assert_param(srcBufferLength % CACHE_ROW_SIZE == 0); // FIXME: assert fails (performance, non-critical)
        //assert_param((uint32_t)mask % CACHE_ROW_SIZE == 0); // FIXME: assert fails (performance, non-critical)

        lv_coord_t mask_stride = lv_area_get_width(dsc->mask_area);
        lv_point_t mask_offset; // mask offset in relation to draw_area
        mask_offset.x = draw_area.x1 - dsc->mask_area->x1;
        mask_offset.y = draw_area.y1 - dsc->mask_area->y1;

        if(dsc->src_buf == NULL) {  // 93.5%
            lv_area_move(&draw_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
            _lv_draw_stm32_dma2d_blend_paint(draw_ctx->buf, dest_stride, &draw_area, mask, mask_stride, &mask_offset, dsc->color,
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
            _lv_draw_stm32_dma2d_blend_map(draw_ctx->buf, dest_stride, &draw_area, dsc->src_buf, src_stride, &src_offset, dsc->opa,
                                           ARGB8888, false);
#else
            // Note: 16-bit bitmap hardware blending with mask and background is possible, but requires a temp 24 or 32-bit buffer to combine bitmap with mask first.

            lv_draw_sw_blend_basic(draw_ctx, dsc); // (e.g. Shop Items)
            lv_coord_t draw_width = lv_area_get_width(&draw_area);
            lv_coord_t draw_height = lv_area_get_height(&draw_area);
            uint32_t dest_address = (uint32_t)(draw_ctx->buf + (dest_stride * draw_area.y1) + draw_area.x1);
            // clean cache after software drawing
            _lv_gpu_stm32_dma2d_clean_cache(dest_address, dest_stride - draw_width, draw_width, draw_height, sizeof(lv_color_t));
#endif
        }
    }
    else {
        if(dsc->src_buf == NULL) {  // 6.1%
            lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);
            lv_area_move(&draw_area, -draw_ctx->buf_area->x1,
                         -draw_ctx->buf_area->y1); // translate the screen draw area to the origin of the buffer area
            _lv_draw_stm32_dma2d_blend_fill(draw_ctx->buf, dest_stride, &draw_area, dsc->color, dsc->opa);
        }
        else {   // 0.2%
            lv_coord_t src_stride = lv_area_get_width(dsc->blend_area);
            lv_point_t src_offset; // source image offset in relation to draw_area
            src_offset.x = draw_area.x1 - dsc->blend_area->x1;
            src_offset.y = draw_area.y1 - dsc->blend_area->y1;
            lv_area_move(&draw_area, -draw_ctx->buf_area->x1,
                         -draw_ctx->buf_area->y1); // translate the screen draw area to the origin of the buffer area
            _lv_draw_stm32_dma2d_blend_map(draw_ctx->buf, dest_stride, &draw_area, dsc->src_buf, src_stride, &src_offset, dsc->opa,
                                           DMA2D_INPUT_COLOR, true);
        }
    }
}

// Does dest_area = intersect(draw_ctx->clip_area, src_area) ?
// See: https://github.com/lvgl/lvgl/issues/3714#issuecomment-1331710788
void lv_draw_stm32_dma2d_buffer_copy(lv_draw_ctx_t * draw_ctx, void * dest_buf, lv_coord_t dest_stride,
                                     const lv_area_t * dest_area, void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area)
{
    // Both draw buffer start address and buffer size *must* be 32-byte aligned since draw buffer cache is being invalidated.
    //uint32_t drawBufferLength = lv_area_get_size(draw_ctx->buf_area) * sizeof(lv_color_t);
    //assert_param(drawBufferLength % CACHE_ROW_SIZE == 0); // critical, but this is not the way to test it
    assert_param((uint32_t)draw_ctx->buf % CACHE_ROW_SIZE == 0); // critical
    // FIXME:
    // 1. Both src_buf and dest_buf pixel size *must* be known to use DMA2D.
    // 2. Verify both buffers start addresses and lengths are 32-byte (cache row size) aligned.
    LV_UNUSED(draw_ctx);
    lv_point_t src_offset;
    src_offset.x = dest_area->x1 - src_area->x1;
    src_offset.y = dest_area->y1 - src_area->y1;
    // FIXME: use lv_area_move(dest_area, -dest_area->x1, -dest_area->y1) here ?
    // TODO: It is assumed that dest_buf and src_buf buffers are of lv_color_t type. Verify it, this assumption may be incorrect.
    _lv_draw_stm32_dma2d_blend_map((const lv_color_t *)dest_buf, dest_stride, dest_area, (const lv_color_t *)src_buf,
                                   src_stride, &src_offset, 0xff, DMA2D_INPUT_COLOR, true);
    // TODO: Investigate if output buffer cache needs to be invalidated. It depends on what the destination buffer is and how it is used next - by dma2d or not.
    _lv_gpu_stm32_dma2d_await_dma_transfer_finish(NULL); // TODO: is this line needed here?
}

lv_res_t lv_draw_stm32_dma2d_img(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc, const lv_area_t * src_area,
                                 const void * src)
{
    //if(lv_img_src_get_type(src) != LV_IMG_SRC_VARIABLE) return LV_RES_INV;
    const lv_img_dsc_t * img = src;
    bitmap_color_code_t bitmapColorCode;
    bool ignoreBitmapAlpha = false;

    switch(img->header.cf) {
        case LV_IMG_CF_RGBA8888:
            bitmapColorCode = ARGB8888;
            break; // note: LV_IMG_CF_RGBA8888 is actually ARGB8888
        case LV_IMG_CF_RGBX8888:
            bitmapColorCode = ARGB8888;
            ignoreBitmapAlpha = true;
            break;
        case LV_IMG_CF_RGB565:
            bitmapColorCode = RGB565;
            break;
        case LV_IMG_CF_TRUE_COLOR:
            bitmapColorCode = DMA2D_INPUT_COLOR;
            break;
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
            if(DMA2D_INPUT_COLOR == DMA2D_INPUT_ARGB8888) {
                bitmapColorCode = ARGB8888;
                break;
            }
            else {
                return LV_RES_INV;
            }
        default:
            return LV_RES_INV;
    }

    if(dsc->angle != 0 || dsc->zoom != 256) {
        return LV_RES_INV; // sorry, dma2d can handle that
    }

    // FIXME: src pixel size *must* be known to use DMA2D
    // FIXME: If image is drawn by SW, then output cache needs to be cleaned next. Currently it is not possible.
    // Both draw buffer start address and buffer size *must* be 32-byte aligned since draw buffer cache is being invalidated.
    //uint32_t drawBufferLength = lv_area_get_size(draw_ctx->buf_area) * sizeof(lv_color_t);
    //assert_param(drawBufferLength % CACHE_ROW_SIZE == 0); // critical, but this is not the way to test it
    assert_param((uint32_t)draw_ctx->buf % CACHE_ROW_SIZE == 0); // critical

    // For performance reasons, both source buffer start address and buffer size *should* be 32-byte aligned since source buffer cache is being cleaned.
    //uint32_t srcBufferLength = lv_area_get_size(src_area) * sizeof(lv_color_t); // TODO: verify src pixel size = sizeof(lv_color_t)
    //assert_param(srcBufferLength % CACHE_ROW_SIZE == 0); // FIXME: assert fails (performance, non-critical)
    //assert_param((uint32_t)src % CACHE_ROW_SIZE == 0); // FIXME: assert fails (performance, non-critical)

    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, src_area, draw_ctx->clip_area)) return LV_RES_OK;

    lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);
    lv_point_t src_offset; // source image offset in relation to draw_area
    src_offset.x = draw_area.x1 - src_area->x1;
    src_offset.y = draw_area.y1 - src_area->y1;
    lv_area_move(&draw_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
    _lv_draw_stm32_dma2d_blend_map(draw_ctx->buf, dest_stride, &draw_area, img->data, img->header.w,
                                   &src_offset, dsc->opa, bitmapColorCode, ignoreBitmapAlpha);
    return LV_RES_OK;
}

void lv_gpu_stm32_dma2d_wait_cb(lv_draw_ctx_t * draw_ctx)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    _lv_gpu_stm32_dma2d_await_dma_transfer_finish(disp->driver);
    lv_draw_sw_wait_for_finish(draw_ctx);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief Fills draw_area with specified color.
 * @param color color to be painted, note: alpha is ignored
 */
LV_STM32_DMA2D_STATIC void _lv_draw_stm32_dma2d_blend_fill(const lv_color_t * dest_buf, lv_coord_t dest_stride,
                                                           const lv_area_t * draw_area, lv_color_t color, lv_opa_t opa)
{
    assert_param(!isDma2dInProgess); // critical
    lv_coord_t draw_width = lv_area_get_width(draw_area);
    lv_coord_t draw_height = lv_area_get_height(draw_area);

    _lv_gpu_stm32_dma2d_await_dma_transfer_finish(NULL);

    if(opa >= LV_OPA_MAX) {
        DMA2D->CR = 0x3UL << DMA2D_CR_MODE_Pos; // Register-to-memory (no FG nor BG, only output stage active)

        DMA2D->OPFCCR = DMA2D_OUTPUT_COLOR;
        DMA2D->OPFCCR |= (RBS_BIT << DMA2D_OPFCCR_RBS_Pos);
        DMA2D->OMAR = (uint32_t)(dest_buf + (dest_stride * draw_area->y1) + draw_area->x1);
        DMA2D->OOR = dest_stride - draw_width;  // out buffer offset
        // Note: unlike FGCOLR and BGCOLR, OCOLR bits must match DMA2D_OUTPUT_COLOR, alpha can be specified
#if RBS_BIT
        DMA2D->OCOLR = (color.ch.blue << 11) | (color.ch.green_l << 5 | color.ch.green_h << 8) | (color.ch.red);
#else
        DMA2D->OCOLR = color.full;
#endif
    }
    else {
        DMA2D->CR = 0x2UL << DMA2D_CR_MODE_Pos; // Memory-to-memory with blending (FG and BG fetch with PFC and blending)

        DMA2D->FGPFCCR = DMA2D_INPUT_A8;
        DMA2D->FGPFCCR |= (opa << DMA2D_FGPFCCR_ALPHA_Pos);
        // Alpha Mode 1: Replace original foreground image alpha channel value by FGPFCCR.ALPHA
        DMA2D->FGPFCCR |= (0x1UL << DMA2D_FGPFCCR_AM_Pos); 
        //DMA2D->FGPFCCR |= (RBS_BIT << DMA2D_FGPFCCR_RBS_Pos);

        // Note: in Alpha Mode 1 FGMAR and FGOR are not used to supply foreground A8 bytes,
        // those bytes are replaced by constant ALPHA defined in FGPFCCR
        DMA2D->FGMAR = (uint32_t)dest_buf;
        DMA2D->FGOR = dest_stride;
        DMA2D->FGCOLR = lv_color_to32(color) & 0x00ffffff; // swap FGCOLR R/B bits if FGPFCCR.RBS (RBS_BIT) bit is set

        DMA2D->BGPFCCR = DMA2D_INPUT_COLOR;
        DMA2D->BGPFCCR |= (RBS_BIT << DMA2D_BGPFCCR_RBS_Pos);
        DMA2D->BGMAR = (uint32_t)(dest_buf + (dest_stride * draw_area->y1) + draw_area->x1);
        DMA2D->BGOR = dest_stride - draw_width;
        DMA2D->BGCOLR = 0;  // used in A4 and A8 modes only
        _lv_gpu_stm32_dma2d_clean_cache(DMA2D->BGMAR, DMA2D->BGOR, draw_width, draw_height, sizeof(lv_color_t));

        DMA2D->OPFCCR = DMA2D_OUTPUT_COLOR;
        DMA2D->OPFCCR |= (RBS_BIT << DMA2D_OPFCCR_RBS_Pos);
        DMA2D->OMAR = DMA2D->BGMAR;
        DMA2D->OOR = DMA2D->BGOR;
        DMA2D->OCOLR = 0;
    }
    // PL - pixel per lines (14 bit), NL - number of lines (16 bit)
    DMA2D->NLR = (draw_width << DMA2D_NLR_PL_Pos) | (draw_height << DMA2D_NLR_NL_Pos);

    _lv_gpu_stm32_dma2d_start_dma_transfer();
}

/**
 * @brief Draws src (foreground) map on dst (background) map.
 * @param src_offset src offset in relation to dst, useful when src is larger than draw_area
 * @param opa constant opacity to be applied
 * @param bitmapColorCode bitmap color type
 * @param ignoreAlpha if TRUE, bitmap src alpha channel is ignored
 */
LV_STM32_DMA2D_STATIC void _lv_draw_stm32_dma2d_blend_map(const lv_color_t * dest_buf, lv_coord_t dest_stride,
                                                          const lv_area_t * draw_area, const void * src_buf, lv_coord_t src_stride, const lv_point_t * src_offset, lv_opa_t opa,
                                                          bitmap_color_code_t src_color_code, bool ignore_src_alpha)
{
    assert_param(!isDma2dInProgess); // critical
    if(opa <= LV_OPA_MIN) return;
    lv_coord_t draw_width = lv_area_get_width(draw_area);
    lv_coord_t draw_height = lv_area_get_height(draw_area);
    bool bitmapHasOpacity = !ignore_src_alpha && (src_color_code == ARGB8888 || src_color_code == ARGB1555 ||
                                                  src_color_code == ARGB4444);

    if(opa >= LV_OPA_MAX) opa = 0xff;

    uint8_t srcBpp; // source bytes per pixel
    switch(src_color_code) {
        case ARGB8888:
            srcBpp = 4;
            break;
        case RGB888:
            srcBpp = 3;
            break;
        case RGB565:
        case ARGB1555:
        case ARGB4444:
            srcBpp = 2;
            break;
        default:
            LV_LOG_ERROR("unsupported color code");
            return;
    }

    _lv_gpu_stm32_dma2d_await_dma_transfer_finish(NULL);

    DMA2D->FGPFCCR = src_color_code;

    if(opa == 0xff && !bitmapHasOpacity) {
        // no need to blend
        if(src_color_code == DMA2D_OUTPUT_COLOR) {
            // no need to convert pixel format (PFC) either
            DMA2D->CR = 0x0UL;
        }
        else {
            DMA2D->CR = 0x1UL << DMA2D_CR_MODE_Pos; // Memory-to-memory with PFC (FG fetch only with FG PFC active)
        }
        // Alpha Mode 0: No modification of the foreground image alpha channel value
    }
    else {
        // blend
        DMA2D->CR = 0x2UL << DMA2D_CR_MODE_Pos; // Memory-to-memory with blending (FG and BG fetch with PFC and blending)
        DMA2D->FGPFCCR |= (opa << DMA2D_FGPFCCR_ALPHA_Pos);
        if(bitmapHasOpacity) {
            // Alpha Mode 2: Replace original foreground image alpha channel value by FGPFCCR.ALPHA multiplied with original alpha channel value
            DMA2D->FGPFCCR |= (0x2UL << DMA2D_FGPFCCR_AM_Pos);
        }
        else {
            // Alpha Mode 1: Replace original foreground image alpha channel value by FGPFCCR.ALPHA
            DMA2D->FGPFCCR |= (0x1UL << DMA2D_FGPFCCR_AM_Pos);
        }
    }

    DMA2D->FGPFCCR |= (RBS_BIT << DMA2D_FGPFCCR_RBS_Pos);
    DMA2D->FGMAR = ((uint32_t)src_buf) + srcBpp * ((src_stride * src_offset->y) + src_offset->x);
    DMA2D->FGOR = src_stride - draw_width;
    DMA2D->FGCOLR = 0;  // used in A4 and A8 modes only
    _lv_gpu_stm32_dma2d_clean_cache(DMA2D->FGMAR, DMA2D->FGOR, draw_width, draw_height, srcBpp);

    DMA2D->OPFCCR = DMA2D_OUTPUT_COLOR;
    DMA2D->OPFCCR |= (RBS_BIT << DMA2D_OPFCCR_RBS_Pos);
    DMA2D->OMAR = (uint32_t)(dest_buf + (dest_stride * draw_area->y1) + draw_area->x1);
    DMA2D->OOR = dest_stride - draw_width;
    DMA2D->OCOLR = 0;

    if(opa != 0xff || bitmapHasOpacity) {
        // use background (BG*) registers
        DMA2D->BGPFCCR = DMA2D_INPUT_COLOR;
        DMA2D->BGPFCCR |= (RBS_BIT << DMA2D_BGPFCCR_RBS_Pos);
        DMA2D->BGMAR = DMA2D->OMAR;
        DMA2D->BGOR = DMA2D->OOR;
        DMA2D->BGCOLR = 0;  // used in A4 and A8 modes only
        _lv_gpu_stm32_dma2d_clean_cache(DMA2D->BGMAR, DMA2D->BGOR, draw_width, draw_height, sizeof(lv_color_t));
    }

    // PL - pixel per lines (14 bit), NL - number of lines (16 bit)
    DMA2D->NLR = (draw_width << DMA2D_NLR_PL_Pos) | (draw_height << DMA2D_NLR_NL_Pos);

    _lv_gpu_stm32_dma2d_start_dma_transfer();
}

/**
 * @brief Paints solid color with alpha mask with additional constant opacity. Useful e.g. for painting anti-aliased fonts.
 * @param src_offset src offset in relation to dst, useful when src (alpha mask) is larger than draw_area
 * @param color color to paint, note: alpha is ignored
 * @param opa constant opacity to be applied
 */
LV_STM32_DMA2D_STATIC void _lv_draw_stm32_dma2d_blend_paint(const lv_color_t * dest_buf, lv_coord_t dest_stride,
                                                            const lv_area_t * draw_area, const lv_opa_t * mask_buf, lv_coord_t mask_stride, const lv_point_t * mask_offset,
                                                            lv_color_t color, lv_opa_t opa)
{
    assert_param(!isDma2dInProgess); // critical
    lv_coord_t draw_width = lv_area_get_width(draw_area);
    lv_coord_t draw_height = lv_area_get_height(draw_area);

    _lv_gpu_stm32_dma2d_await_dma_transfer_finish(NULL);

    DMA2D->CR = 0x2UL << DMA2D_CR_MODE_Pos;  // Memory-to-memory with blending (FG and BG fetch with PFC and blending)

    DMA2D->FGPFCCR = DMA2D_INPUT_A8;
    if(opa < LV_OPA_MAX) {
        DMA2D->FGPFCCR |= (opa << DMA2D_FGPFCCR_ALPHA_Pos);
        DMA2D->FGPFCCR |= (0x2UL <<
                           DMA2D_FGPFCCR_AM_Pos); // Alpha Mode: Replace original foreground image alpha channel value by FGPFCCR.ALPHA multiplied with original alpha channel value
    }
    //DMA2D->FGPFCCR |= (RBS_BIT << DMA2D_FGPFCCR_RBS_Pos);
    DMA2D->FGMAR = (uint32_t)(mask_buf + (mask_stride * mask_offset->y) + mask_offset->x);
    DMA2D->FGOR = mask_stride - draw_width;
    DMA2D->FGCOLR = lv_color_to32(color) & 0x00ffffff;  // swap FGCOLR R/B bits if FGPFCCR.RBS (RBS_BIT) bit is set
    _lv_gpu_stm32_dma2d_clean_cache(DMA2D->FGMAR, DMA2D->FGOR, draw_width, draw_height, sizeof(lv_opa_t));

    DMA2D->BGPFCCR = DMA2D_INPUT_COLOR;
    DMA2D->BGPFCCR |= (RBS_BIT << DMA2D_BGPFCCR_RBS_Pos);
    DMA2D->BGMAR = (uint32_t)(dest_buf + (dest_stride * draw_area->y1) + draw_area->x1);
    DMA2D->BGOR = dest_stride - draw_width;
    DMA2D->BGCOLR = 0;  // used in A4 and A8 modes only
    _lv_gpu_stm32_dma2d_clean_cache(DMA2D->BGMAR, DMA2D->BGOR, draw_width, draw_height, sizeof(lv_color_t));

    DMA2D->OPFCCR = DMA2D_OUTPUT_COLOR;
    DMA2D->OPFCCR |= (RBS_BIT << DMA2D_OPFCCR_RBS_Pos);
    DMA2D->OMAR = DMA2D->BGMAR;
    DMA2D->OOR = DMA2D->BGOR;
    DMA2D->OCOLR = 0;
    // PL - pixel per lines (14 bit), NL - number of lines (16 bit)
    DMA2D->NLR = (draw_width << DMA2D_NLR_PL_Pos) | (draw_height << DMA2D_NLR_NL_Pos);

    _lv_gpu_stm32_dma2d_start_dma_transfer();
}

/**
 * @brief Copies src (foreground) map to the dst (background) map.
 * @param src_offset src offset in relation to dst, useful when src is larger than draw_area
 */
LV_STM32_DMA2D_STATIC void _lv_draw_stm32_dma2d_copy_buffer(const lv_color_t * dest_buf, lv_coord_t dest_stride,
                                                            const lv_area_t * draw_area, const lv_color_t * src_buf, lv_coord_t src_stride, const lv_point_t * src_offset)
{
    assert_param(!isDma2dInProgess); // critical
    lv_coord_t draw_width = lv_area_get_width(draw_area);
    lv_coord_t draw_height = lv_area_get_height(draw_area);

    _lv_gpu_stm32_dma2d_await_dma_transfer_finish(NULL);

    DMA2D->CR = 0x0UL; // Memory-to-memory (FG fetch only)

    DMA2D->FGPFCCR = DMA2D_INPUT_COLOR;
    DMA2D->FGPFCCR |= (RBS_BIT << DMA2D_FGPFCCR_RBS_Pos);
    DMA2D->FGMAR = (uint32_t)(src_buf + (src_stride * src_offset->y) + src_offset->x);
    DMA2D->FGOR = src_stride - draw_width;
    DMA2D->FGCOLR = 0;  // used in A4 and A8 modes only
    _lv_gpu_stm32_dma2d_clean_cache(DMA2D->FGMAR, DMA2D->FGOR, draw_width, draw_height, sizeof(lv_color_t));

    // Note BG* registers do not need to be set up since BG is not used

    DMA2D->OPFCCR = DMA2D_OUTPUT_COLOR;
    DMA2D->OPFCCR |= (RBS_BIT << DMA2D_OPFCCR_RBS_Pos);
    DMA2D->OMAR = (uint32_t)(dest_buf + (dest_stride * draw_area->y1) + draw_area->x1);
    DMA2D->OOR = dest_stride - draw_width;
    DMA2D->OCOLR = 0;

    // PL - pixel per lines (14 bit), NL - number of lines (16 bit)
    DMA2D->NLR = (draw_width << DMA2D_NLR_PL_Pos) | (draw_height << DMA2D_NLR_NL_Pos);

    _lv_gpu_stm32_dma2d_start_dma_transfer();
}

LV_STM32_DMA2D_STATIC void _lv_gpu_stm32_dma2d_start_dma_transfer(void)
{
    assert_param(!isDma2dInProgess);
    isDma2dInProgess = true;
    DMA2D->IFCR = 0x3FU; // trigger ISR flags reset
    // Note: cleaning output buffer cache is needed only when buffer may be misaligned or adjacent area may have been drawn in sw-fashion, e.g. using lv_draw_sw_blend_basic()
#if LV_COLOR_DEPTH == 16
    _lv_gpu_stm32_dma2d_clean_cache(DMA2D->OMAR, DMA2D->OOR, (DMA2D->NLR & DMA2D_NLR_PL_Msk) >> DMA2D_NLR_PL_Pos, (DMA2D->NLR & DMA2D_NLR_NL_Msk) >> DMA2D_NLR_NL_Pos, sizeof(lv_color_t));
#endif
    DMA2D->CR |= DMA2D_CR_START;
    // Note: for some reason mask buffer gets damaged during transfer if waiting is postponed
    _lv_gpu_stm32_dma2d_await_dma_transfer_finish(NULL); // FIXME: this line should not be needed here, but it is
}

LV_STM32_DMA2D_STATIC void _lv_gpu_stm32_dma2d_await_dma_transfer_finish(lv_disp_drv_t * disp_drv)
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
        LV_LOG_ERROR("DMA2D config error");
    }

    if(isrFlags & DMA2D_ISR_TEIF) {
        LV_LOG_ERROR("DMA2D transfer error");
    }

    DMA2D->IFCR = 0x3FU; // trigger ISR flags reset

    if(isDma2dInProgess) {
        // invalidate output buffer cached memory ONLY after DMA2D transfer
        //_lv_gpu_stm32_dma2d_invalidate_cache(DMA2D->OMAR, DMA2D->OOR, (DMA2D->NLR & DMA2D_NLR_PL_Msk) >> DMA2D_NLR_PL_Pos, (DMA2D->NLR & DMA2D_NLR_NL_Msk) >> DMA2D_NLR_NL_Pos, sizeof(lv_color_t));
        isDma2dInProgess = false;
    }
}

LV_STM32_DMA2D_STATIC void _lv_gpu_stm32_dma2d_invalidate_cache(uint32_t address, lv_coord_t offset, lv_coord_t width,
                                                                lv_coord_t height, uint8_t pixel_size)
{
    if(((SCB->CCR) & SCB_CCR_DC_Msk) == 0) return; // L1 data cache is disabled
    uint16_t stride = pixel_size * (width + offset); // in bytes
    uint16_t ll = pixel_size * width; // line length in bytes
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

LV_STM32_DMA2D_STATIC void _lv_gpu_stm32_dma2d_clean_cache(uint32_t address, lv_coord_t offset, lv_coord_t width,
                                                           lv_coord_t height, uint8_t pixel_size)
{
    if(((SCB->CCR) & SCB_CCR_DC_Msk) == 0) return; // L1 data cache is disabled
    uint16_t stride = pixel_size * (width + offset); // in bytes
    uint16_t ll = pixel_size * width; // line length in bytes
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

// initialize µs timer
LV_STM32_DMA2D_STATIC bool _lv_gpu_stm32_dwt_init(void)
{
    // disable TRC
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
    // enable TRC
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    DWT->LAR = 0xC5ACCE55;

    // disable clock cycle counter
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
    // enable  clock cycle counter
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    // reset the clock cycle counter value
    DWT->CYCCNT = 0;

    // 3 NO OPERATION instructions
    __ASM volatile("NOP");
    __ASM volatile("NOP");
    __ASM volatile("NOP");

    // check if clock cycle counter has started
    if(DWT->CYCCNT) {
        return true; // clock cycle counter started
    }
    else {
        return false; // clock cycle counter not started
    }
}

// get elapsed µs since reset
LV_STM32_DMA2D_STATIC uint32_t _lv_gpu_stm32_dwt_get_us(void)
{
    uint32_t us = (DWT->CYCCNT * 1000000) / HAL_RCC_GetHCLKFreq();
    return us;
}

// reset µs timer
LV_STM32_DMA2D_STATIC void _lv_gpu_stm32_dwt_reset(void)
{
    DWT->CYCCNT = 0;
}

#endif
