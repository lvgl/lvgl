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

#define CACHE_ROW_SIZE 32U // cache row size in Bytes

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
/*
static void lv_draw_stm32_dma2d_blend_fill(const lv_color_t * dst_buf, lv_coord_t dst_stride,
                                           const lv_area_t * draw_area, lv_color_t color, lv_opa_t opa);
static void lv_draw_stm32_dma2d_blend_map(const lv_color_t * dst_buf, lv_coord_t dst_stride, const lv_area_t * draw_area, const lv_color_t * src_buf, lv_coord_t src_stride, const lv_point_t * src_offset, lv_opa_t opa);
static void lv_draw_stm32_dma2d_blend_paint(const lv_color_t* dst_buf, lv_coord_t dst_stride, const lv_area_t* draw_area, const lv_opa_t* mask_buf, lv_coord_t mask_stride, const lv_point_t* mask_offset, lv_color32_t color, lv_opa_t opa);
static lv_res_t lv_draw_stm32_dma2d_img(lv_draw_ctx_t * draw, const lv_draw_img_dsc_t * dsc, const lv_area_t * coords, const void * src);
static void invalidate_cache(uint32_t sourceAddress, lv_coord_t offset, lv_coord_t width, lv_coord_t height, uint8_t pixelSize);
static void clean_cache(uint32_t sourceAddress, lv_coord_t offset, lv_coord_t width, lv_coord_t height, uint8_t pixelSize);
static void waitForDmaTransferToFinish(lv_disp_drv_t * disp_drv);
static void startDmaTransfer(void);
*/
/*
static void zeroAlpha(const lv_color_t * buf, uint32_t length);
static void trace4Areas(const lv_area_t * area1, const lv_area_t * area2, const lv_area_t * area3, const lv_area_t * area4);
static void compareBuffers(const lv_color_t* b1, const lv_color_t* b2, const lv_area_t * area, lv_coord_t stride);
*/

/**********************
 *  STATIC VARIABLES
 **********************/
static bool invalidateCache = false;

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
    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, dsc->blend_area, draw_ctx->clip_area)) return;
    //trace4Areas(draw_ctx->buf_area, draw_ctx->clip_area, dsc->blend_area, &draw_area);
    // + draw_ctx->buf_area has the entire draw buffer location
    // + draw_ctx->clip_area has the current draw buffer location
    // + dsc->blend_area has the location of the area intended to be painted - image etc.
    // + draw_area has the area actually being painted
    // All coordinates are relative to the screen.

    bool done = false;
    invalidateCache = false;
    
    static uint32_t c = 0;
    static uint32_t c1 = 0;
    static uint32_t c2 = 0;
    static uint32_t c3 = 0;
    static uint32_t c4 = 0;
    static uint32_t c5 = 0;
    static uint32_t c6 = 0;
    static uint32_t c7 = 0;
    static uint32_t c8 = 0;
    static uint32_t c9 = 0;

    c++;
    
    if (c == 100) {
        c = 0;
        printf("%li %li %li %li %li %li %li %li %li\n", c1, c2, c3, c4, c5, c6, c7, c8, c9);
    }
    
    const lv_opa_t * mask;
    if(dsc->mask_buf == NULL) mask = NULL;

    if(dsc->mask_buf && dsc->mask_res == LV_DRAW_MASK_RES_TRANSP) return;
    else if(dsc->mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask = NULL;
    else mask = dsc->mask_buf;

    if (dsc->blend_mode != LV_BLEND_MODE_NORMAL) {
        c1++; // 0 - never happens
    } else if (lv_area_get_size(&draw_area) < 80) {
        c2++; // 223
    } else if (mask != NULL && dsc->src_buf != NULL) {
        c3++; // 1.6
        // note: draw_ctx->buf (xRGB) does NOT carry alpha channel bytes,
        // alpha channel bytes are carried in dsc->mask_buf
    } else if (mask != NULL && dsc->src_buf == NULL) {
        c4++; // 200
        lv_coord_t dest_w = lv_area_get_width(draw_ctx->buf_area);
        lv_coord_t mask_stride = lv_area_get_width(dsc->mask_area);
        lv_point_t mask_offset; // mask offset in relation to draw_area
        mask_offset.x = draw_area.x1 - dsc->mask_area->x1;
        mask_offset.y = draw_area.y1 - dsc->mask_area->y1;
        lv_area_move(&draw_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
        lv_color_t color = dsc->color;
        lv_opa_t opa = dsc->opa;
        // TODO: some bug here, map has wrong bytes
        lv_draw_stm32_dma2d_blend_paint(draw_ctx->buf, dest_w, &draw_area, mask, mask_stride, &mask_offset, color, opa);
        done = true;
    } else if (mask == NULL && dsc->src_buf != NULL) {
        c5++; // 1
        lv_coord_t dest_w = lv_area_get_width(draw_ctx->buf_area);
        lv_coord_t src_w = lv_area_get_width(dsc->blend_area);
        lv_point_t src_offset; // source image offset in relation to draw_area
        src_offset.x = draw_area.x1 - dsc->blend_area->x1;
        src_offset.y = draw_area.y1 - dsc->blend_area->y1;
        lv_area_move(&draw_area, -draw_ctx->buf_area->x1,
                        -draw_ctx->buf_area->y1); // translate the screen draw area to the origin of the buffer area
        lv_draw_stm32_dma2d_blend_map(draw_ctx->buf, dest_w, &draw_area, dsc->src_buf, src_w, &src_offset, dsc->opa);
        done = true;
    } else if (mask == NULL && dsc->src_buf == NULL) {
        c6++; // 8.7 + 2.5
        lv_coord_t dest_w = lv_area_get_width(draw_ctx->buf_area);
        lv_area_move(&draw_area, -draw_ctx->buf_area->x1,
                        -draw_ctx->buf_area->y1); // translate the screen draw area to the origin of the buffer area
        lv_draw_stm32_dma2d_blend_fill(draw_ctx->buf, dest_w, &draw_area, dsc->color, dsc->opa);
        done = true;
    } else {
        c7++; // 0 - never happens
    }
    
    if(!done) {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
        c8++; // 238
    }
}

// Does dest_stride = width(draw_ctx->buf_area) ?
// Does dest_area = intersect(draw_ctx->clip_area, src_area) ?
void lv_draw_stm32_dma2d_buffer_copy(lv_draw_ctx_t * draw_ctx, void * dest_buf, lv_coord_t dest_stride,
                                     const lv_area_t * dest_area, void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area)
{
    LV_UNUSED(draw_ctx);
    lv_point_t src_pos;
    src_pos.x = dest_area->x1 - src_area->x1;
    src_pos.y = dest_area->y1 - src_area->y1;
    lv_draw_stm32_dma2d_blend_map(dest_buf, dest_stride, dest_area, src_buf, src_stride, &src_pos, LV_OPA_MAX);
}

STATIC lv_res_t lv_draw_stm32_dma2d_img(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                        const lv_area_t * src_area, const void * src)
{
    //lv_draw_sw_img_decoded(draw_ctx, dsc, src_area, src, color_format);

    if(lv_img_src_get_type(src) == LV_IMG_SRC_VARIABLE) {
        lv_area_t draw_area;
        if(!_lv_area_intersect(&draw_area, src_area, draw_ctx->clip_area)) return LV_RES_OK;
        const lv_img_dsc_t * img = src;

        if(img->header.cf == LV_IMG_CF_RGBA8888 && dsc->angle == 0 && dsc->zoom == 256) {
            // note: LV_IMG_CF_RGBA8888 is actually ARGB8888
            lv_coord_t dest_width = lv_area_get_width(draw_ctx->buf_area);
            lv_point_t src_offset; // position of the clip area origin within the source image area
            src_offset.x = draw_area.x1 - src_area->x1;
            src_offset.y = draw_area.y1 - src_area->y1;
            lv_area_move(&draw_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
            lv_draw_stm32_dma2d_blend_map(draw_ctx->buf, dest_width, &draw_area, (lv_color_t *)img->data, img->header.w, &src_offset,
                                          dsc->opa);
            return LV_RES_OK;
        }
    }

    return LV_RES_INV;
}

/**
 * @brief Fills draw_area with specified color.
 * @param color color to be painted, note: alpha is ignored
 */
STATIC void lv_draw_stm32_dma2d_blend_fill(const lv_color_t * dst_buf, lv_coord_t dst_stride,
                                           const lv_area_t * draw_area, lv_color_t color, lv_opa_t opa)
{
    assert_param((uint32_t)dst_buf % CACHE_ROW_SIZE == 0);
	
    lv_coord_t draw_width = lv_area_get_width(draw_area);
	lv_coord_t draw_height = lv_area_get_height(draw_area);

	waitForDmaTransferToFinish(NULL);

    if (opa >= LV_OPA_MAX) {
        DMA2D->CR = 0x3UL << DMA2D_CR_MODE_Pos;  // Register-to-memory (no FG nor BG, only output stage active)

        DMA2D->OPFCCR = DMA2D_OUTPUT_ARGB8888;
        DMA2D->OMAR = (uint32_t)(dst_buf + (dst_stride * draw_area->y1) + draw_area->x1);
        DMA2D->OOR = dst_stride - draw_width;  // out offset
        DMA2D->OCOLR = color.full | (0xff << 24);
    } else {
        DMA2D->CR = 0x2UL << DMA2D_CR_MODE_Pos;  // Memory-to-memory with blending (FG and BG fetch with PFC and blending)
        
        DMA2D->FGPFCCR = DMA2D_INPUT_A8;
        DMA2D->FGPFCCR |= (opa << DMA2D_FGPFCCR_ALPHA_Pos);
        DMA2D->FGPFCCR |= (0x1UL << DMA2D_FGPFCCR_AM_Pos); // Alpha Mode: Replace original foreground image alpha channel value by ALPHA[7:0]
        
        // note: in Alpha Mode 1 FGMAR and FGOR are not used as to supply foreground A8 bytes,
        // those bytes are replaced by ALPHA const defined in FGPFCCR
        DMA2D->FGMAR = (uint32_t)dst_buf;
        DMA2D->FGOR = dst_stride;
        DMA2D->FGCOLR = color.full;  // used in A4 and A8 modes only, alpha is ignored

        DMA2D->BGPFCCR = DMA2D_INPUT_ARGB8888;
        DMA2D->BGMAR = (uint32_t)(dst_buf + (dst_stride * draw_area->y1) + draw_area->x1);
        DMA2D->BGOR = dst_stride - draw_width;
        DMA2D->BGCOLR = 0;  // used in A4 and A8 modes only
        clean_cache(DMA2D->BGMAR, DMA2D->BGOR, draw_width, draw_height, sizeof(lv_color_t));

        DMA2D->OPFCCR = DMA2D_OUTPUT_ARGB8888;;
        DMA2D->OMAR = DMA2D->BGMAR;
        DMA2D->OOR = DMA2D->BGOR;
        DMA2D->OCOLR = 0;
    }
	// PL - pixel per lines (14 bit), NL - number of lines (16 bit)
	DMA2D->NLR = (uint32_t)((draw_width << DMA2D_NLR_PL_Pos) & DMA2D_NLR_PL_Msk) | ((draw_height << DMA2D_NLR_NL_Pos) & DMA2D_NLR_NL_Msk);

	startDmaTransfer();
}

/**
 * @brief Draws src (foreground) map on dst (background) map.
 * @param src_offset src offset in relation to dst, useful when src is larger than draw_area
 * @param opa constant opacity to be applied
 */
STATIC void lv_draw_stm32_dma2d_blend_map(const lv_color_t* dst_buf, lv_coord_t dst_stride, const lv_area_t* draw_area, const lv_color_t* src_buf, lv_coord_t src_stride, const lv_point_t* src_offset, lv_opa_t opa) {
	
    assert_param((uint32_t)dst_buf % CACHE_ROW_SIZE == 0);
    assert_param((uint32_t)src_buf % CACHE_ROW_SIZE == 0);
    
    lv_coord_t draw_width = lv_area_get_width(draw_area);
	lv_coord_t draw_height = lv_area_get_height(draw_area);

	waitForDmaTransferToFinish(NULL);

	DMA2D->FGPFCCR = DMA2D_INPUT_ARGB8888;

	if (opa >= LV_OPA_MAX) {
		DMA2D->CR = 0x1UL << DMA2D_CR_MODE_Pos;               // Memory-to-memory with PFC (FG fetch only with FG PFC active)
	} else {
		DMA2D->CR = 0x2UL << DMA2D_CR_MODE_Pos;  // Memory-to-memory with blending (FG and BG fetch with PFC and blending)
		DMA2D->FGPFCCR |= (opa << DMA2D_FGPFCCR_ALPHA_Pos);
		// Note: select mode 1 or 2 depending on whether src_buf has xRGB or ARGB data respectively
        //DMA2D->FGPFCCR |= (0x1UL << DMA2D_FGPFCCR_AM_Pos);  // Replace original foreground image alpha channel value by ALPHA[7:0]
        DMA2D->FGPFCCR |= (0x2UL << DMA2D_FGPFCCR_AM_Pos); // Alpha Mode: Replace original foreground image alpha channel value by ALPHA[7:0] multiplied with original alpha channel value
	}

	DMA2D->FGMAR = (uint32_t)(src_buf + (src_stride * src_offset->y) + src_offset->x);
	DMA2D->FGOR = src_stride - draw_width;
	DMA2D->FGCOLR = 0;  // used in A4 and A8 modes only
	clean_cache(DMA2D->FGMAR, DMA2D->FGOR, draw_width, draw_height, sizeof(lv_color_t));

	DMA2D->BGPFCCR = DMA2D_INPUT_ARGB8888;
	DMA2D->BGMAR = (uint32_t)(dst_buf + (dst_stride * draw_area->y1) + draw_area->x1);
	DMA2D->BGOR = dst_stride - draw_width;
	DMA2D->BGCOLR = 0;  // used in A4 and A8 modes only
	clean_cache(DMA2D->BGMAR, DMA2D->BGOR, draw_width, draw_height, sizeof(lv_color_t));

	DMA2D->OPFCCR = DMA2D_OUTPUT_ARGB8888;
	DMA2D->OMAR = DMA2D->BGMAR;
	DMA2D->OOR = DMA2D->BGOR;
	DMA2D->OCOLR = 0;

	// PL - pixel per lines (14 bit), NL - number of lines (16 bit)
	DMA2D->NLR = (uint32_t)((draw_width << DMA2D_NLR_PL_Pos) & DMA2D_NLR_PL_Msk) | ((draw_height << DMA2D_NLR_NL_Pos) & DMA2D_NLR_NL_Msk);

	startDmaTransfer();
}

/**
 * @brief Paints solid color with alpha mask with additional constant opacity. Useful e.g. for painting anti-aliased fonts.
 * @param src_offset src offset in relation to dst, useful when src (alpha mask) is larger than draw_area
 * @param color color to paint, note: alpha is ignored
 * @param opa constant opacity to be applied
 */
STATIC void lv_draw_stm32_dma2d_blend_paint(const lv_color_t* dst_buf, lv_coord_t dst_stride, const lv_area_t* draw_area, const lv_opa_t* mask_buf, lv_coord_t mask_stride, const lv_point_t* mask_offset, lv_color32_t color, lv_opa_t opa) {
	assert_param((uint32_t)dst_buf % CACHE_ROW_SIZE == 0);
    assert_param((uint32_t)mask_buf % CACHE_ROW_SIZE == 0);

    lv_coord_t draw_width = lv_area_get_width(draw_area);
	lv_coord_t draw_height = lv_area_get_height(draw_area);

	waitForDmaTransferToFinish(NULL);

	DMA2D->CR = 0x2UL << DMA2D_CR_MODE_Pos;  // Memory-to-memory with blending (FG and BG fetch with PFC and blending)
	
	DMA2D->FGPFCCR = DMA2D_INPUT_A8;
    if (opa < LV_OPA_MAX) {
	    DMA2D->FGPFCCR |= (opa << DMA2D_FGPFCCR_ALPHA_Pos);
	    DMA2D->FGPFCCR |= (0x2UL << DMA2D_FGPFCCR_AM_Pos); // Alpha Mode: Replace original foreground image alpha channel value by ALPHA[7:0] multiplied with original alpha channel value
    }
	DMA2D->FGMAR = (uint32_t)(mask_buf + (mask_stride * mask_offset->y) + mask_offset->x);
	DMA2D->FGOR = mask_stride - draw_width;
	DMA2D->FGCOLR = color.full | (0xff << 24);  // used in A4 and A8 modes only
	clean_cache(DMA2D->FGMAR, DMA2D->FGOR, draw_width, draw_height, sizeof(lv_opa_t)); // adjust for 8bit mask

	DMA2D->BGPFCCR = DMA2D_INPUT_ARGB8888;
	DMA2D->BGMAR = (uint32_t)(dst_buf + (dst_stride * draw_area->y1) + draw_area->x1);
	DMA2D->BGOR = dst_stride - draw_width;
	DMA2D->BGCOLR = 0;  // used in A4 and A8 modes only
	clean_cache(DMA2D->BGMAR, DMA2D->BGOR, draw_width, draw_height, sizeof(lv_color_t));

	DMA2D->OPFCCR = DMA2D_OUTPUT_ARGB8888;;
	DMA2D->OMAR = DMA2D->BGMAR;
	DMA2D->OOR = DMA2D->BGOR;
	DMA2D->OCOLR = 0;

	// PL - pixel per lines (14 bit), NL - number of lines (16 bit)
	DMA2D->NLR = (uint32_t)((draw_width << DMA2D_NLR_PL_Pos) & DMA2D_NLR_PL_Msk) | ((draw_height << DMA2D_NLR_NL_Pos) & DMA2D_NLR_NL_Msk);

	startDmaTransfer();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void startDmaTransfer(void)
{
    invalidateCache = true;
    DMA2D->IFCR = 0x3FU; // trigger ISR flags reset
    DMA2D->CR |= DMA2D_CR_START;
}

STATIC void waitForDmaTransferToFinish(lv_disp_drv_t * disp_drv)
{
    if (disp_drv && disp_drv->wait_cb) {
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

    if(invalidateCache) {
        // invalidate cache ONLY after DMA2D transfer
        invalidate_cache(DMA2D->OMAR, DMA2D->OOR, (DMA2D->NLR & DMA2D_NLR_PL_Msk) >> DMA2D_NLR_PL_Pos,
                         (DMA2D->NLR & DMA2D_NLR_NL_Msk) >> DMA2D_NLR_NL_Pos, LV_IMG_PX_SIZE_ALPHA_BYTE);
        invalidateCache = false;
    }
}

void lv_gpu_stm32_dma2d_wait_cb(lv_draw_ctx_t * draw_ctx)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    waitForDmaTransferToFinish(disp->driver);
    lv_draw_sw_wait_for_finish(draw_ctx);
}

static void invalidate_cache(uint32_t address, lv_coord_t offset, lv_coord_t width, lv_coord_t height, uint8_t pixelSize)
{
    if(((SCB->CCR) & SCB_CCR_DC_Msk) == 0) return; // L1 data cache is disabled
    uint32_t stride = pixelSize * (width + offset); // in bytes
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

static void clean_cache(uint32_t address, lv_coord_t offset, lv_coord_t width, lv_coord_t height, uint8_t pixelSize)
{
    if(((SCB->CCR) & SCB_CCR_DC_Msk) == 0) return; // L1 data cache is disabled
    uint32_t stride = pixelSize * (width + offset); // in bytes
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

/* test code
static void zeroAlpha(const lv_color_t * buf, uint32_t length) {
    uint32_t * buf32 = (uint32_t *)buf;
    while (length > 0) {
        length--;
        //*buf32 &= 0x00ffffff; // zero MSB
        *buf32 |= 0xff000000; // set MSB
        buf32++;
    }
}

static void trace4Areas(const lv_area_t * area1, const lv_area_t * area2, const lv_area_t * area3, const lv_area_t * area4) {
    static uint32_t counter = 0;

    lv_coord_t x = area1->x1;
    lv_coord_t y = area1->y1;
    lv_coord_t w = lv_area_get_width(area1);
    lv_coord_t h = lv_area_get_height(area1);
    printf("%i %i %i %i", x, y, w, h);

    x = area2->x1;
    y = area2->y1;
    w = lv_area_get_width(area2);
    h = lv_area_get_height(area2);
    printf(" %i %i %i %i", x, y, w, h);

    x = area3->x1;
    y = area3->y1;
    w = lv_area_get_width(area3);
    h = lv_area_get_height(area3);
    printf(" %i %i %i %i", x, y, w, h);

    x = area4->x1;
    y = area4->y1;
    w = lv_area_get_width(area4);
    h = lv_area_get_height(area4);
    printf(" %i %i %i %i\n", x, y, w, h);

    counter++;
    if (counter == 900) {
        counter = 0;
        printf("\n");
    }
}

static void compareBuffers(const lv_color_t* b1, const lv_color_t* b2, const lv_area_t * area, lv_coord_t stride) {
    lv_coord_t area_w = lv_area_get_width(area);
    lv_coord_t area_h = lv_area_get_height(area);
    lv_color_t * buf1 = (lv_color_t *)b1 + (stride * area->y1) + area->x1;
    lv_color_t * buf2 = (lv_color_t *)b2 + (stride * area->y1) + area->x1;

    while (area_h > 0) {
        area_h--;
        //memcpy(dst_buf2, src_buf2, area_w * sizeof(lv_color_t));
        for (lv_coord_t x = 0; x < area_w; x++) {
            if ((*buf1).full != (*buf2).full) {
                printf("mismatch\n");
            }
            buf1++;
            buf2++;
        }
        buf1 += stride - area_w;
        buf2 += stride - area_w;
    }
}

static void lv_draw_stm32_dma2d_blend_map(const lv_color_t * dst_buf, lv_coord_t dst_width, const lv_area_t * dst_area, const lv_color_t * src_buf, lv_coord_t src_width, const lv_point_t * src_pos, lv_opa_t opa)
{
    lv_coord_t area_w = lv_area_get_width(dst_area);
    lv_coord_t area_h = lv_area_get_height(dst_area);
    lv_color_t * dst_buf2 = (lv_color_t *)dst_buf + (dst_width * dst_area->y1) + dst_area->x1;
    lv_color_t * src_buf2 = (lv_color_t *)src_buf + (src_width * src_pos->y) + src_pos->x;

    while (area_h > 0) {
        area_h--;
        memcpy(dst_buf2, src_buf2, area_w * sizeof(lv_color_t));
        src_buf2 += src_width;
        dst_buf2 += dst_width;
    }

    invalidateCache = false;
}

static void lv_draw_stm32_dma2d_blend_map(const lv_color_t * dst_buf, lv_coord_t dst_stride, const lv_area_t * dst_area, const lv_color_t * src_buf, lv_coord_t src_stride, const lv_point_t * src_pos, lv_opa_t opa)
{
    lv_coord_t area_w = lv_area_get_width(dst_area);
    lv_coord_t area_h = lv_area_get_height(dst_area);
    lv_color_t * dst_buf2 = (lv_color_t *)dst_buf + (dst_stride * dst_area->y1) + dst_area->x1;
    lv_color_t * src_buf2 = (lv_color_t *)src_buf + (src_stride * src_pos->y) + src_pos->x;

    while(area_h > 0) {
        area_h--;
        for (lv_coord_t x = 0; x < area_w; x++) {
            lv_color_t fg_color = *src_buf2;
            lv_color_t bg_color = *dst_buf2;
            lv_color_t out;
            // See: RM0410, 9.3.6 DMA2D blender (p. 286)
            uint8_t a_multi = (fg_color.ch.alpha * bg_color.ch.alpha) / 255;
            out.ch.alpha = fg_color.ch.alpha + bg_color.ch.alpha - a_multi;
            out.ch.red = (fg_color.ch.red * fg_color.ch.alpha + bg_color.ch.red * bg_color.ch.alpha - bg_color.ch.red * a_multi) / out.ch.alpha;
            out.ch.green = (fg_color.ch.green * fg_color.ch.alpha + bg_color.ch.green * bg_color.ch.alpha - bg_color.ch.green * a_multi) / out.ch.alpha;
            out.ch.blue = (fg_color.ch.blue * fg_color.ch.alpha + bg_color.ch.blue * bg_color.ch.alpha - bg_color.ch.blue * a_multi) / out.ch.alpha;
            *dst_buf2 = out;
            dst_buf2++;
            src_buf2++;
        }
        dst_buf2 += dst_stride - area_w;
        src_buf += src_stride - area_w;
    }
}

static void lv_draw_stm32_dma2d_blend_fill(const lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area, lv_color_t color)
{
    lv_coord_t dest_w = lv_area_get_width(fill_area);
    lv_coord_t dest_h = lv_area_get_height(fill_area);
    lv_color_t * buf = (lv_color_t *)dest_buf;

    while(dest_h > 0) {
        dest_h--;
        for (lv_coord_t x = 0; x < dest_w; x++) {
            *buf = color;
            buf++;
        }
        buf += dest_stride - dest_w;
    }
    invalidateCache = false;
}
*/

#endif
