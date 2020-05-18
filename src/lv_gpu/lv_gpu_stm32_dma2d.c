/**
 * @file lv_gpu_stm32_dma2d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gpu_stm32_dma2d.h"
#include "../lv_core/lv_refr.h"

#if LV_USE_GPU_STM32_DMA2D

#if defined(STM32F4)
    #include "stm32f4xx_hal.h"
#elif defined(STM32F7)
    #include "stm32f7xx_hal.h"
#else
    #error "Not supported STM32 family to use DMA2D"
#endif

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
    #define DMA2D_OUTPUT_FORMAT DMA2D_OUTPUT_RGB565
    #define DMA2D_INPUT_FORMAT DMA2D_INPUT_RGB565
#elif LV_COLOR_DEPTH == 32
    #define DMA2D_OUTPUT_FORMAT DMA2D_OUTPUT_ARGB8888
    #define DMA2D_INPUT_FORMAT DMA2D_INPUT_ARGB8888
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
static void dma2d_wait(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static DMA2D_HandleTypeDef hdma2d;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

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

    hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_R2M;
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_FORMAT;
    hdma2d.Init.OutputOffset = buf_w - fill_w;
    hdma2d.LayerCfg[1].InputAlpha = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_FORMAT;
    hdma2d.LayerCfg[1].InputOffset = 0;

    /* DMA2D Initialization */
    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_ConfigLayer(&hdma2d, 1);
    HAL_DMA2D_Start(&hdma2d, (uint32_t)lv_color_to32(color), (uint32_t)buf, fill_w, fill_h);
    dma2d_wait();
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
    dma2d_wait();
}

/**
 * Copy a map (typically RGB image) to a buffer
 * @param buf a buffer where map should be copied
 * @param buf_w width of the buffer in pixels
 * @param map an "image" to copy
 * @param map_w width of teh map in pixels
 * @param copy_w width of the area to copy in pixels (<= buf_w)
 * @param copy_h height of the area to copy in pixels
 * @note `map_w - fill_w` is offset to the next line after copy
 */
void lv_gpu_stm32_dma2d_copy(lv_color_t * buf, lv_coord_t buf_w, const lv_color_t * map, lv_coord_t map_w,
                             lv_coord_t copy_w, lv_coord_t copy_h)
{
    invalidate_cache();

    hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_M2M;
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_FORMAT;
    hdma2d.Init.OutputOffset = buf_w - copy_w;

    /* Foreground layer */
    hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[1].InputAlpha = 0xFF;
    hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_FORMAT;
    hdma2d.LayerCfg[1].InputOffset = map_w - copy_w;

    /* DMA2D Initialization */
    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_ConfigLayer(&hdma2d, 0);
    HAL_DMA2D_ConfigLayer(&hdma2d, 1);
    HAL_DMA2D_Start(&hdma2d, (uint32_t)map, (uint32_t)buf, copy_w, copy_h);
    dma2d_wait();
}

/**
 * Blend a map (e.g. ARGB image or RGB image with opacity) to a buffer
 * @param buf a buffer where `map` should be copied
 * @param buf_w width of the buffer in pixels
 * @param map an "image" to copy
 * @param opa opacity of `map`
 * @param map_w width of teh map in pixels
 * @param copy_w width of the area to copy in pixels (<= buf_w)
 * @param copy_h height of the area to copy in pixels
 * @note `map_w - fill_w` is offset to the next line after copy
 */
void lv_gpu_stm32_dma2d_blend(lv_color_t * buf, lv_coord_t buf_w, const lv_color_t * map, lv_opa_t opa,
                              lv_coord_t map_w, lv_coord_t copy_w, lv_coord_t copy_h)
{
    invalidate_cache();

    hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_M2M_BLEND;
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_FORMAT;
    hdma2d.Init.OutputOffset = buf_w - copy_w;

    /* Background layer */
    hdma2d.LayerCfg[0].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_FORMAT;
    hdma2d.LayerCfg[0].InputOffset = buf_w - copy_w;

    /* Foreground layer */
    hdma2d.LayerCfg[1].AlphaMode = DMA2D_COMBINE_ALPHA;
    hdma2d.LayerCfg[1].InputAlpha = opa;
    hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_FORMAT;
    hdma2d.LayerCfg[1].InputOffset = map_w - copy_w;

    /* DMA2D Initialization */
    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_ConfigLayer(&hdma2d, 0);
    HAL_DMA2D_ConfigLayer(&hdma2d, 1);
    HAL_DMA2D_BlendingStart(&hdma2d, (uint32_t)map, (uint32_t)buf, (uint32_t)buf, copy_w, copy_h);
    dma2d_wait();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void invalidate_cache(void)
{
#if __DCACHE_PRESENT
    if(SCB->CCR & (uint32_t)SCB_CCR_DC_Msk) {
        SCB_CleanInvalidateDCache();
    }
#endif
}

static void dma2d_wait(void)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    while(HAL_DMA2D_PollForTransfer(&hdma2d, 0) == HAL_TIMEOUT) {
        if(disp->driver.wait_cb) disp->driver.wait_cb(&disp->driver);
    }
}

#endif
