/**
 * @file lv_gpu_stm32_dma2d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gpu_stm32_dma2d.h"
#include "../lv_core/lv_disp.h"
#include "../lv_core/lv_refr.h"

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
static void invalidate_cache(void);
static void wait_finish(void);

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
void lv_gpu_stm32_dma2d_init(void)
{
    /* Enable DMA2D clock */
#if defined(STM32F4) || defined(STM32F7)
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN;
#elif defined(STM32H7)
    RCC->AHB3ENR |= RCC_AHB3ENR_DMA2DEN;
#else
# warning "LVGL can't enable the clock of DMA2D"
#endif

    /* Wait for hardware access to complete */
    __asm volatile("DSB\n");

    /* Delay after setting peripheral clock */
    volatile uint32_t temp = RCC->AHB1ENR;

    /* set output colour mode */
    DMA2D->OPFCCR = LV_DMA2D_COLOR_FORMAT;
}

/**
 * Fill an area in the buffer with a color
 * @param buf a buffer which should be filled
 * @param buf_w width of the buffer in pixels
 * @param color fill color
 * @param fill_w width to fill in pixels (<= buf_w)
 * @param fill_h height to fill in pixels
 * @note `buf_w - fill_w` is offset to the next line after fill
 */
void lv_gpu_stm32_dma2d_fill(lv_color_t * buf, lv_coord_t buf_w, lv_color_t color, lv_coord_t fill_w, lv_coord_t fill_h)
{
    invalidate_cache();

    DMA2D->CR = 0x30000;
    DMA2D->OMAR = (uint32_t)buf;
    /* as input color mode is same as output we don't need to convert here do we? */
    DMA2D->OCOLR = color.full;
    DMA2D->OOR = buf_w - fill_w;
    DMA2D->NLR = (fill_w << DMA2D_NLR_PL_Pos) | (fill_h << DMA2D_NLR_NL_Pos);

    /* start transfer */
    DMA2D->CR |= DMA2D_CR_START_Msk;

    wait_finish();
}

/**
 * Fill an area in the buffer with a color but take into account a mask which describes the opacity of each pixel
 * @param buf a buffer which should be filled using a mask
 * @param buf_w width of the buffer in pixels
 * @param color fill color
 * @param mask 0..255 values describing the opacity of the corresponding pixel. It's width is `fill_w`
 * @param opa overall opacity. 255 in `mask` should mean this opacity.
 * @param fill_w width to fill in pixels (<= buf_w)
 * @param fill_h height to fill in pixels
 * @note `buf_w - fill_w` is offset to the next line after fill
 */
void lv_gpu_stm32_dma2d_fill_mask(lv_color_t * buf, lv_coord_t buf_w, lv_color_t color, const lv_opa_t * mask,
                                  lv_opa_t opa, lv_coord_t fill_w, lv_coord_t fill_h)
{
#if 0
    invalidate_cache();

    /* Configure the DMA2D Mode, Color Mode and line output offset */
    hdma2d.Init.Mode         = DMA2D_M2M_BLEND;
    hdma2d.Init.ColorMode    = DMA2D_OUTPUT_FORMAT;
    hdma2d.Init.OutputOffset = buf_w - fill_w;

    /* Configure the foreground -> The character */
    lv_color32_t c32;
    c32.full = lv_color_to32(color);
    c32.ch.alpha = opa;
    hdma2d.LayerCfg[1].AlphaMode       = DMA2D_COMBINE_ALPHA;
    hdma2d.LayerCfg[1].InputAlpha      = c32.full;
    hdma2d.LayerCfg[1].InputColorMode  = DMA2D_INPUT_A8;
    hdma2d.LayerCfg[1].InputOffset     = 0;

    /* Configure the background -> Display buffer */
    hdma2d.LayerCfg[0].AlphaMode       = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[0].InputAlpha      = 0x00;
    hdma2d.LayerCfg[0].InputColorMode  = DMA2D_INPUT_FORMAT;
    hdma2d.LayerCfg[0].InputOffset     = buf_w - fill_w;

    /* DMA2D Initialization */
    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_ConfigLayer(&hdma2d, 0);
    HAL_DMA2D_ConfigLayer(&hdma2d, 1);
    HAL_DMA2D_BlendingStart(&hdma2d, (uint32_t) mask, (uint32_t) buf, (uint32_t)buf, fill_w, fill_h);
    wait_finish();
#endif
}

/**
 * Copy a map (typically RGB image) to a buffer
 * @param buf a buffer where map should be copied
 * @param buf_w width of the buffer in pixels
 * @param map an "image" to copy
 * @param map_w width of the map in pixels
 * @param copy_w width of the area to copy in pixels (<= buf_w)
 * @param copy_h height of the area to copy in pixels
 * @note `map_w - fill_w` is offset to the next line after copy
 */
void lv_gpu_stm32_dma2d_copy(lv_color_t * buf, lv_coord_t buf_w, const lv_color_t * map, lv_coord_t map_w,
                             lv_coord_t copy_w, lv_coord_t copy_h)
{
    invalidate_cache();

    DMA2D->CR = 0;
    /* copy output colour mode, this register controls both input and output colour format */
    DMA2D->FGPFCCR = LV_DMA2D_COLOR_FORMAT;
    DMA2D->FGMAR = (uint32_t)map;
    DMA2D->FGOR = map_w - copy_w;
    DMA2D->OMAR = (uint32_t)buf;
    DMA2D->OOR = buf_w - copy_w;
    DMA2D->NLR = (copy_w << DMA2D_NLR_PL_Pos) | (copy_h << DMA2D_NLR_NL_Pos);

    /* start transfer */
    DMA2D->CR |= DMA2D_CR_START_Msk;
    wait_finish();
}

/**
 * Blend a map (e.g. ARGB image or RGB image with opacity) to a buffer
 * @param buf a buffer where `map` should be copied
 * @param buf_w width of the buffer in pixels
 * @param map an "image" to copy
 * @param opa opacity of `map`
 * @param map_w width of the map in pixels
 * @param copy_w width of the area to copy in pixels (<= buf_w)
 * @param copy_h height of the area to copy in pixels
 * @note `map_w - fill_w` is offset to the next line after copy
 */
void lv_gpu_stm32_dma2d_blend(lv_color_t * buf, lv_coord_t buf_w, const lv_color_t * map, lv_opa_t opa,
                              lv_coord_t map_w, lv_coord_t copy_w, lv_coord_t copy_h)
{
    invalidate_cache();
    DMA2D->CR = 0x20000;

    DMA2D->BGPFCCR = LV_DMA2D_COLOR_FORMAT;
    DMA2D->BGMAR = (uint32_t)buf;
    DMA2D->BGOR = buf_w - copy_w;

    DMA2D->FGPFCCR = (uint32_t)LV_DMA2D_COLOR_FORMAT
                     /* alpha mode 2, replace with foreground * alpha value */
                     | (2 << DMA2D_FGPFCCR_AM_Pos)
                     /* alpha value */
                     | (opa << DMA2D_FGPFCCR_ALPHA_Pos);
    DMA2D->FGMAR = (uint32_t)map;
    DMA2D->FGOR = map_w - copy_w;

    DMA2D->OMAR = (uint32_t)buf;
    DMA2D->OOR = buf_w - copy_w;
    DMA2D->NLR = (copy_w << DMA2D_NLR_PL_Pos) | (copy_h << DMA2D_NLR_NL_Pos);

    /* start transfer */
    DMA2D->CR |= DMA2D_CR_START_Msk;
    wait_finish();
}

void lv_gpu_stm32_dma2d_wait_cb(lv_disp_drv_t * drv)
{
    if(drv && drv->wait_cb) {
        while(DMA2D->CR & DMA2D_CR_START_Msk) {
            drv->wait_cb(drv);
        }
    }
    else {
        while(DMA2D->CR & DMA2D_CR_START_Msk);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void invalidate_cache(void)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    if(disp->driver.clean_dcache_cb) disp->driver.clean_dcache_cb(&disp->driver);
    else {
#if __CORTEX_M >= 0x07
        if((SCB->CCR) & (uint32_t)SCB_CCR_DC_Msk)
            SCB_CleanInvalidateDCache();
#endif
    }
}

static void wait_finish(void)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    if(disp->driver.gpu_wait_cb) return;

    while(DMA2D->CR & DMA2D_CR_START_Msk) {
        if(disp->driver.wait_cb) disp->driver.wait_cb(&disp->driver);
    }
}

#endif
