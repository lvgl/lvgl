/**
 * @file lv_gpu_stm32_dma2d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gpu_stm32_dma2d.h"
#include "../../core/lv_refr.h"

#include "main.h" // test only
#include "stdio.h"

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

static void lv_draw_stm32_dma2d_blend_fill(const lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area, lv_color_t color);
static void lv_draw_stm32_dma2d_blend_map(const lv_color_t * dst_buf, lv_coord_t dst_width, const lv_area_t * dst_area, const lv_color_t * src_buf, lv_coord_t src_width, const lv_point_t * src_pos, lv_opa_t opa);
static lv_res_t lv_draw_stm32_dma2d_img(lv_draw_ctx_t * draw, const lv_draw_img_dsc_t * dsc, const lv_area_t * coords, const void * src);

static void invalidate_cache(uint32_t sourceAddress, lv_coord_t offset, lv_coord_t width, lv_coord_t height);
static void clean_cache(uint32_t sourceAddress, lv_coord_t offset, lv_coord_t width, lv_coord_t height);
static void zeroAlpha(const lv_color_t * buf, uint32_t length);
static void trace4Areas(const lv_area_t * area1, const lv_area_t * area2, const lv_area_t * area3, const lv_area_t * area4);

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
    lv_area_t dest_area;
    if(!_lv_area_intersect(&dest_area, dsc->blend_area, draw_ctx->clip_area)) return;
    //trace4Areas(draw_ctx->buf_area, draw_ctx->clip_area, dsc->blend_area, &dest_area);
    // + draw_ctx->buf_area has the entire draw buffer location
    // + draw_ctx->clip_area has the current draw buffer location
    // + dsc->blend_area has the location of the area intended to be painted - image etc.
    // + dest_area has the area actually being painted
    // All coordinates are relative to the screen.

    bool done = false;
    invalidateCache = false;

    if(dsc->mask_buf == NULL && dsc->blend_mode == LV_BLEND_MODE_NORMAL && lv_area_get_size(&dest_area) > 100) {
        lv_coord_t dest_w = lv_area_get_width(draw_ctx->buf_area);

        if(dsc->src_buf != NULL) {
            lv_coord_t src_w = lv_area_get_width(dsc->blend_area);
            lv_point_t src_pos; // position of the clip area in relation to the source bitmap area
            src_pos.x = dest_area.x1 - dsc->blend_area->x1;
            src_pos.y = dest_area.y1 - dsc->blend_area->y1;
            //zeroAlpha(src_buf, lv_area_get_size(dsc->blend_area));
            lv_area_move(&dest_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1); // translate the screen draw area to the origin of the buffer area
            lv_draw_stm32_dma2d_blend_map(draw_ctx->buf, dest_w, &dest_area, dsc->src_buf, src_w, &src_pos, dsc->opa);
            done = true;
        }
        else if(dsc->opa >= LV_OPA_MAX) {
            lv_area_move(&dest_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1); // translate the screen draw area to the origin of the buffer area
            lv_draw_stm32_dma2d_blend_fill(draw_ctx->buf, dest_w, &dest_area, dsc->color);
            done = true;
        }
    }

    if(!done) lv_draw_sw_blend_basic(draw_ctx, dsc);
}


// Does dest_stride = width(draw_ctx->buf_area) ?
// Does dest_area = intersect(draw_ctx->clip_area, src_area) ?
void lv_draw_stm32_dma2d_buffer_copy(lv_draw_ctx_t * draw_ctx, void * dest_buf, lv_coord_t dest_stride, const lv_area_t * dest_area, void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area)
{
    LV_UNUSED(draw_ctx);
    lv_point_t src_pos;
    src_pos.x = dest_area->x1 - src_area->x1;
    src_pos.y = dest_area->y1 - src_area->y1;
    lv_draw_stm32_dma2d_blend_map(dest_buf, dest_stride, dest_area, src_buf, src_stride, &src_pos, LV_OPA_MAX);
}

static lv_res_t lv_draw_stm32_dma2d_img(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc, const lv_area_t * src_area, const void * src)
{
	//lv_draw_sw_img_decoded(draw_ctx, dsc, src_area, src, color_format);

    if(lv_img_src_get_type(src) == LV_IMG_SRC_VARIABLE) {
	    lv_area_t dest_area;
	    /*Return if fully clipped*/
	    if(!_lv_area_intersect(&dest_area, src_area, draw_ctx->clip_area)) return LV_RES_OK;

		const lv_img_dsc_t * img = src;

		//if(img->header.cf == LV_IMG_CF_RGBA8888 && dsc->angle == 0 && dsc->zoom == 256) {
        if(img->header.cf == LV_IMG_CF_RGBA8888) {
			/*TODO Blend here. Perform a fill as example*/
			lv_coord_t dest_width = lv_area_get_width(draw_ctx->buf_area);
            lv_point_t src_pos; // position of the clip area in relation to the source image area
            src_pos.x = dest_area.x1 - src_area->x1;
            src_pos.y = dest_area.y1 - src_area->y1;
            lv_area_move(&dest_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
            lv_draw_stm32_dma2d_blend_map(draw_ctx->buf, dest_width, &dest_area, (lv_color_t *)img->data, img->header.w, &src_pos, dsc->opa);
            //lv_draw_stm32_dma2d_blend_fill(draw_ctx->buf, dest_width, &dest_area, lv_color_hex3(lv_rand(0x000, 0xfff)));
			return LV_RES_OK;
		}
	}

	return LV_RES_INV;
}

static void lv_draw_stm32_dma2d_blend_fill(const lv_color_t * dst_buf, lv_coord_t dst_width, const lv_area_t * dst_area, lv_color_t color)
{
    assert_param((uint32_t)dst_buf % 4 == 0);
    assert_param((DMA2D->CR & DMA2D_CR_START) == 0U);
    /*Simply fill an area*/
    lv_coord_t draw_width = lv_area_get_width(dst_area);
    lv_coord_t draw_height = lv_area_get_height(dst_area);
    
    DMA2D->CR = 0x3UL << DMA2D_CR_MODE_Pos; // Register-to-memory (no FG nor BG, only output stage active)
    
    DMA2D->OPFCCR = LV_DMA2D_COLOR_FORMAT;
    DMA2D->OMAR = (uint32_t)(dst_buf + (dst_width * dst_area->y1) + dst_area->x1);
    DMA2D->OCOLR = color.full;
    DMA2D->OOR = dst_width - draw_width; // out offset
    // PL - pixel per lines (14 bit), NL - number of lines (16 bit)
    DMA2D->NLR = (uint32_t)((draw_width << DMA2D_NLR_PL_Pos) & DMA2D_NLR_PL_Msk) | ((draw_height << DMA2D_NLR_NL_Pos) & DMA2D_NLR_NL_Msk);

    /* start transfer */
    invalidateCache = true;
    DMA2D->IFCR = 0x3FU; // trigger ISR flags reset
    DMA2D->CR |= DMA2D_CR_START;
}

static void lv_draw_stm32_dma2d_blend_map2(const lv_color_t * dst_buf, lv_coord_t dst_width, const lv_area_t * dst_area, const lv_color_t * src_buf, lv_coord_t src_width, const lv_point_t * src_pos, lv_opa_t opa) {
    assert_param((DMA2D->CR & DMA2D_CR_START) == 0U);
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

static void lv_draw_stm32_dma2d_blend_map(const lv_color_t * dst_buf, lv_coord_t dst_width, const lv_area_t * dst_area, const lv_color_t * src_buf, lv_coord_t src_width, const lv_point_t * src_pos, lv_opa_t opa)
{
    assert_param((uint32_t)dst_buf % LV_IMG_PX_SIZE_ALPHA_BYTE == 0);
    assert_param((uint32_t)src_buf % LV_IMG_PX_SIZE_ALPHA_BYTE == 0);
    assert_param((DMA2D->CR & DMA2D_CR_START) == 0U);
    lv_coord_t draw_width = lv_area_get_width(dst_area);
    lv_coord_t draw_height = lv_area_get_height(dst_area);

    DMA2D->FGPFCCR = LV_DMA2D_COLOR_FORMAT;
    DMA2D->FGMAR   = (uint32_t)(src_buf + (src_width * src_pos->y) + src_pos->x);
    DMA2D->FGOR    = src_width - draw_width;
    
    DMA2D->OPFCCR = LV_DMA2D_COLOR_FORMAT;
    DMA2D->OMAR    = (uint32_t)(dst_buf + (dst_width * dst_area->y1) + dst_area->x1);
    DMA2D->OOR     = dst_width - draw_width;
    // PL - pixel per lines (14 bit), NL - number of lines (16 bit)
    DMA2D->NLR = (uint32_t)((draw_width << DMA2D_NLR_PL_Pos) & DMA2D_NLR_PL_Msk) | ((draw_height << DMA2D_NLR_NL_Pos) & DMA2D_NLR_NL_Msk);

    if(opa >= LV_OPA_MAX) {
        DMA2D->CR = 0;
    } else {
        DMA2D->CR = 0x2UL << DMA2D_CR_MODE_Pos;
        DMA2D->BGPFCCR = LV_DMA2D_COLOR_FORMAT;
        DMA2D->BGMAR = DMA2D->OMAR;
        DMA2D->BGOR = DMA2D->OOR;
        clean_cache(DMA2D->BGMAR, DMA2D->BGOR, draw_width, draw_height);
    }

    //SCB_CleanDCache(); // TODO: change
    clean_cache(DMA2D->FGMAR, DMA2D->FGOR, draw_width, draw_height);
    invalidateCache = true;
    DMA2D->IFCR = 0x3FU; // trigger ISR flags reset
    DMA2D->CR |= DMA2D_CR_START;
}

void lv_gpu_stm32_dma2d_wait_cb(lv_draw_ctx_t * draw_ctx)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    if(disp->driver && disp->driver->wait_cb) {
        while((DMA2D->CR & DMA2D_CR_START) != 0U) {
            disp->driver->wait_cb(disp->driver);
        }
    }
    else {
        while((DMA2D->CR & DMA2D_CR_START) != 0U);
    }
    
    __IO uint32_t isrFlags = DMA2D->ISR;

    if (isrFlags & DMA2D_FLAG_CE) {
        printf("dma2d config error\n");
    } else if (isrFlags & DMA2D_FLAG_TE) {
        printf("dma2d transfer error\n");
    }
    
    DMA2D->IFCR = 0x3FU; // trigger ISR flags reset

    if (invalidateCache) {
        //clean_cache((uint32_t)(draw_ctx->buf), dest_w * dest_h * LV_IMG_PX_SIZE_ALPHA_BYTE);
         // invalidate cache ONLY after DMA2D transfer
        invalidate_cache(DMA2D->OMAR, DMA2D->OOR, (DMA2D->NLR & DMA2D_NLR_PL_Msk) >> DMA2D_NLR_PL_Pos, (DMA2D->NLR & DMA2D_NLR_NL_Msk) >> DMA2D_NLR_NL_Pos);
        invalidateCache = false;
    }

    lv_draw_sw_wait_for_finish(draw_ctx);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#define CACHE_ROW_SIZE 32U // cache row size in Bytes
#define PIXELS_PER_CACHE_ROW (CACHE_ROW_SIZE / LV_IMG_PX_SIZE_ALPHA_BYTE)

// TODO: use if((SCB->CCR) & (uint32_t)SCB_CCR_DC_Msk)
static void invalidate_cache(uint32_t address, lv_coord_t offset, lv_coord_t width, lv_coord_t height)
{
    // int32_t dsize = 4*((height * width) + (height - 1 * offset));
    // int32_t op_size = dsize + (address & (32U - 1U));
    // DCCIMVAC((uint32_t*)address, op_size);
    // return;

//#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    uint32_t stride = LV_IMG_PX_SIZE_ALPHA_BYTE * (width + offset); // in bytes
    uint16_t lcrc = (width / PIXELS_PER_CACHE_ROW); // line cache row count
    __DSB();

    while (height > 0) {
        height--;
        uint32_t a = address + (height * stride);
        //SCB->DCCMVAC = a; // clean the first line cache row first

        for (uint16_t r = 0; r < lcrc; r++) {
            SCB->DCIMVAC = a;
            a += CACHE_ROW_SIZE;
        }
        
        //SCB->DCCMVAC = a; // clean the last line cache row first
        //SCB->DCIMVAC = a;
    };

    __DSB();
    __ISB();
//#endif
}

static void clean_cache(uint32_t address, lv_coord_t offset, lv_coord_t width, lv_coord_t height)
{
    // int32_t dsize = 4*((height * width) + (height - 1 * offset));
    // int32_t op_size = dsize + (address & (32U - 1U));
    // //SCB_CleanDCache();
    // SCB_CleanDCache_by_Addr((uint32_t*)address, op_size);
    // return;

//#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    uint32_t stride = LV_IMG_PX_SIZE_ALPHA_BYTE * (width + offset);
    uint16_t lcrc = (width / PIXELS_PER_CACHE_ROW); // line cache row count

    __DSB();
    
    while (height > 0) {
        height--;
        uint32_t a = address + (height * stride);
        
        for (uint16_t r = 0; r < lcrc; r++) {
            SCB->DCCMVAC = a;
            a += CACHE_ROW_SIZE;
        }
    };

    __DSB();
    __ISB();
//#endif
}

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

#endif
