/**
 * @file lv_gpu_stm32_dma2d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gpu_stm32_dma2d.h"

#if LV_USE_GPU_STM32_DMA2D
#include "stm32f7xx_hal.h"

/*********************
 *      DEFINES
 *********************/

#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP
#define DMA2D_OUTPUT_FORMAT DMA2D_OUTPUT_RGB565
#define DMA2D_INPUT_FORMAT DMA2D_INPUT_RGB565
#elif LV_COLOR_DEPTH == 32
#define DMA2D_OUTPUT_FORMAT DMA2D_OUTPUT_ARGB8888
#define DMA2D_INPUT_FORMAT DMA2D_INPUT_ARGB8888
#else
/*Can't use GPU with other formats*/
#undef LV_USE_GPU_STM32_DMA2D
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

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
void lv_gpu_stm32_dma2d_fill(lv_color_t * buf, lv_coord_t buf_w, lv_color_t color, lv_coord_t fill_w, lv_coord_t fill_h)
{
#if __DCACHE_PRESENT
    SCB_CleanInvalidateDCache();
#endif
    hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_R2M;
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_FORMAT;
    hdma2d.Init.OutputOffset = buf_w - fill_w;
    hdma2d.LayerCfg[1].InputAlpha = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_FORMAT;
    hdma2d.LayerCfg[1].InputOffset = 0;

    /* DMA2D Initialization */
    if (HAL_DMA2D_Init(&hdma2d) == HAL_OK) {
        if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK) {
            HAL_DMA2D_Start(&hdma2d, (uint32_t)lv_color_to32(color), (uint32_t)buf, fill_w, fill_h);
            HAL_DMA2D_PollForTransfer(&hdma2d, HAL_MAX_DELAY);
        }
    }
}

void lv_gpu_stm32_dma2d_fill_mask(lv_color_t * buf, lv_coord_t buf_w, lv_color_t color, const lv_opa_t * mask, lv_opa_t opa, lv_coord_t fill_w, lv_coord_t fill_h)
{
#if __DCACHE_PRESENT
    SCB_CleanInvalidateDCache();
#endif
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
    HAL_DMA2D_BlendingStart(&hdma2d, (uint32_t) mask, (uint32_t) buf,  (uint32_t)buf, fill_w, fill_h);
    HAL_DMA2D_PollForTransfer(&hdma2d, HAL_MAX_DELAY);
}

void lv_gpu_stm32_dma2d_blend_normal_cover(lv_color_t * buf, lv_coord_t buf_w, const lv_color_t * map, lv_coord_t map_w, lv_coord_t copy_w, lv_coord_t copy_h)
{
#if __DCACHE_PRESENT
    SCB_CleanInvalidateDCache();
#endif
    hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_M2M;
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_FORMAT;
    hdma2d.Init.OutputOffset = buf_w - copy_w;

    /* Foreground layer */
    hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[1].InputAlpha = 0xFF;
    hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_FORMAT;
    hdma2d.LayerCfg[1].InputOffset = map_w - copy_w;
    hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;

    /* DMA2D Initialization */
    if (HAL_DMA2D_Init(&hdma2d) == HAL_OK) {
        HAL_DMA2D_ConfigLayer(&hdma2d, 0);
        if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK) {
            HAL_DMA2D_Start(&hdma2d, (uint32_t)map, (uint32_t)buf, copy_w, copy_h);
            HAL_DMA2D_PollForTransfer(&hdma2d, HAL_MAX_DELAY);
        }
    }
}

void lv_gpu_stm32_dma2d_blend_normal_opa(lv_color_t * buf, lv_coord_t buf_w, const lv_color_t * map, lv_opa_t opa, lv_coord_t map_w, lv_coord_t copy_w, lv_coord_t copy_h)
{
#if __DCACHE_PRESENT
    SCB_CleanInvalidateDCache();
#endif
    hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_M2M_BLEND;
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_FORMAT;
    hdma2d.Init.OutputOffset = buf_w - copy_w;

    /* Background layer */
    hdma2d.LayerCfg[0].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_FORMAT;
    hdma2d.LayerCfg[0].InputOffset = buf_w - copy_w;
    hdma2d.LayerCfg[0].AlphaInverted = DMA2D_REGULAR_ALPHA;

    /* Foreground layer */
    hdma2d.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
    hdma2d.LayerCfg[1].InputAlpha = opa;
    hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_FORMAT;
    hdma2d.LayerCfg[1].InputOffset = map_w - copy_w;
    hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;

    /* DMA2D Initialization */
    if (HAL_DMA2D_Init(&hdma2d) == HAL_OK) {
        HAL_DMA2D_ConfigLayer(&hdma2d, 0);
        if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK) {
            HAL_DMA2D_BlendingStart(&hdma2d, (uint32_t)map, (uint32_t)buf, (uint32_t)buf, copy_w, copy_h);
            HAL_DMA2D_PollForTransfer(&hdma2d, HAL_MAX_DELAY);
        }
    }
}
/**********************
 *   STATIC FUNCTIONS
 **********************/


#endif
