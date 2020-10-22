/**
 * @file lv_gpu_nxp_pxp.c
 *
 */

/**
 * MIT License
 *
 * Copyright (c) 2020 NXP
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next paragraph)
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */


/*********************
 *      INCLUDES
 *********************/

#include "../lv_conf_internal.h"

#if LV_USE_GPU_NXP_PXP

#include "lvgl.h"
#include "lv_gpu_nxp_pxp.h"
#include "../lv_misc/lv_mem.h"
#include "../lv_misc/lv_log.h"

#include "fsl_pxp.h"
#include "fsl_cache.h"

/*********************
 *      DEFINES
 *********************/

#if LV_COLOR_16_SWAP
    #error Color swap not implemented. Disable LV_COLOR_16_SWAP feature.
#endif

#if LV_COLOR_DEPTH==16
    #define PXP_OUT_PIXEL_FORMAT kPXP_OutputPixelFormatRGB565
    #define PXP_AS_PIXEL_FORMAT kPXP_AsPixelFormatRGB565
    #define PXP_PS_PIXEL_FORMAT kPXP_PsPixelFormatRGB565
#elif
    #error Only 16bit color depth is supported. Set LV_COLOR_DEPTH to 16.
#endif


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_gpu_nxp_pxp_run(void);
static void lv_gpu_nxp_pxp_blit_recolor(lv_color_t * dest, lv_coord_t dest_width, const lv_color_t * src,
                                        lv_coord_t src_width,
                                        lv_coord_t copy_width, lv_coord_t copy_height, lv_opa_t opa, lv_color_t recolor, lv_opa_t recolorOpa);
static void lv_gpu_nxp_invalidate_cache(uint32_t address, uint32_t width, uint32_t height, uint32_t stride,
                                        uint32_t pxSize);

/**********************
 *  STATIC VARIABLES
 **********************/

static bool colorKeyEnabled = false;
static uint32_t colorKey = 0x0;

static bool recolorEnabled = false;
static lv_color_t recolor = {.full = 0x0};
static lv_opa_t recolorOpa = 0x0;

static lv_nxp_pxp_cfg_t pxp_cfg;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Reset and initialize PXP device. This function should be called as a part
 * of display init sequence.
 *
 * @return LV_RES_OK: PXP init ok; LV_RES_INV: init error. See error log for more information.
 */
lv_res_t lv_gpu_nxp_pxp_init(lv_nxp_pxp_cfg_t * cfg)
{
    if(!cfg || !cfg->pxp_interrupt_deinit || !cfg->pxp_interrupt_init || !cfg->pxp_run) {
        LV_LOG_ERROR("PXP configuration error. Check callback pointers.");
        return LV_RES_INV;
    }

    PXP_Init(PXP);
    PXP_EnableCsc1(PXP, false); /* Disable CSC1, it is enabled by default. */
    PXP_EnableInterrupts(PXP, kPXP_CompleteInterruptEnable);

    pxp_cfg = *cfg;
    if(pxp_cfg.pxp_interrupt_init() != LV_RES_OK) {
        PXP_Deinit(PXP);
        LV_LOG_ERROR("PXP interrupt init error. Check pxp_interrupt_init callback.");
        return LV_RES_INV;
    }

    colorKey = lv_color_to32(LV_COLOR_TRANSP);

    return LV_RES_OK;
}

/**
 * Disable PXP device. Should be called during display deinit sequence.
 */
void lv_gpu_nxp_pxp_deinit(void)
{
    pxp_cfg.pxp_interrupt_deinit();
    PXP_DisableInterrupts(PXP, kPXP_CompleteInterruptEnable);
    PXP_Deinit(LV_GPU_NXP_PXP_ID);
}


/**
 * Fill area, with optional opacity.
 *
 * @param[in/out] dest_buf destination buffer
 * @param[in] dest_width width (stride) of destination buffer in pixels
 * @param[in] fill_area area to fill
 * @param[in] color color
 * @param[in] opa transparency of the color
 */
void lv_gpu_nxp_pxp_fill(lv_color_t * dest_buf, lv_coord_t dest_width, const lv_area_t * fill_area, lv_color_t color,
                         lv_opa_t opa)
{
    PXP_Init(LV_GPU_NXP_PXP_ID);
    PXP_EnableCsc1(LV_GPU_NXP_PXP_ID, false);     /* Disable CSC1, it is enabled by default. */
    PXP_SetProcessBlockSize(PXP, kPXP_BlockSize16); /* Block size 16x16 for higher performance */

    /* OUT buffer configure */
    pxp_output_buffer_config_t outputConfig = {
        .pixelFormat    = PXP_OUT_PIXEL_FORMAT,
        .interlacedMode = kPXP_OutputProgressive,
        .buffer0Addr    = (uint32_t)(dest_buf + dest_width * fill_area->y1 + fill_area->x1),
        .buffer1Addr    = (uint32_t)NULL,
        .pitchBytes     = dest_width * sizeof(lv_color_t),
        .width          = fill_area->x2 - fill_area->x1 + 1,
        .height         = fill_area->y2 - fill_area->y1 + 1,
    };
    lv_gpu_nxp_invalidate_cache(outputConfig.buffer0Addr, outputConfig.width, outputConfig.height, outputConfig.pitchBytes,
                                sizeof(lv_color_t));
    PXP_SetOutputBufferConfig(LV_GPU_NXP_PXP_ID, &outputConfig);

    if(opa > LV_OPA_MAX) {
        /* Simple color fill without opacity - AS disabled, PS as color generator */
        PXP_SetAlphaSurfacePosition(LV_GPU_NXP_PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U); /* Disable AS. */
        PXP_SetProcessSurfacePosition(LV_GPU_NXP_PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U); /* Disable PS. */
        PXP_SetProcessSurfaceBackGroundColor(LV_GPU_NXP_PXP_ID, lv_color_to32(color));
    }
    else {
        /* Fill with opacity - AS used as source (same as OUT), PS used as color generator, blended together */
        pxp_as_buffer_config_t asBufferConfig;
        pxp_porter_duff_config_t pdConfig;

        /* Set AS to OUT */
        asBufferConfig.pixelFormat = PXP_AS_PIXEL_FORMAT;
        asBufferConfig.bufferAddr  = (uint32_t)outputConfig.buffer0Addr;
        asBufferConfig.pitchBytes  = outputConfig.pitchBytes;

        PXP_SetAlphaSurfaceBufferConfig(LV_GPU_NXP_PXP_ID, &asBufferConfig);
        PXP_SetAlphaSurfacePosition(LV_GPU_NXP_PXP_ID, 0U, 0U, fill_area->x2 - fill_area->x1 + 1,
                                    fill_area->y2 - fill_area->y1 + 1);

        /* Disable PS, use as color generator */
        PXP_SetProcessSurfacePosition(LV_GPU_NXP_PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);
        PXP_SetProcessSurfaceBackGroundColor(LV_GPU_NXP_PXP_ID, lv_color_to32(color));

        /* Configure Porter-Duff blending - For RGB 565 only! */
        pdConfig.enable = 1;
        pdConfig.dstColorMode = kPXP_PorterDuffColorStraight;
        pdConfig.srcColorMode = kPXP_PorterDuffColorStraight;
        pdConfig.dstGlobalAlphaMode = kPXP_PorterDuffGlobalAlpha;
        pdConfig.srcGlobalAlphaMode = kPXP_PorterDuffGlobalAlpha;
        pdConfig.srcFactorMode = kPXP_PorterDuffFactorStraight;
        pdConfig.dstFactorMode = kPXP_PorterDuffFactorStraight;
        pdConfig.srcGlobalAlpha = opa;
        pdConfig.dstGlobalAlpha = 255 - opa;
        pdConfig.srcAlphaMode = kPXP_PorterDuffAlphaStraight; /* don't care */
        pdConfig.dstAlphaMode = kPXP_PorterDuffAlphaStraight; /* don't care */
        PXP_SetPorterDuffConfig(LV_GPU_NXP_PXP_ID, &pdConfig);
    }

    lv_gpu_nxp_pxp_run(); /* Start PXP task */
}

/**
 * @brief BLock Image Transfer - copy rectangular image from src buffer to dst buffer with effects.
 *
 * By default, image is copied directly, with optional opacity configured by \p opa.
 * Color keying can be enabled by calling lv_gpu_nxp_pxp_enable_color_key() before calling this function.
 * Recoloring can be enabled by calling  lv_gpu_nxp_pxp_enable_recolor() before calling this function.
 * Note that color keying and recoloring at the same time is not supported and black rectangle is rendered.
 *
 * @param[in/out] dest destination buffer
 * @param[in] dest_width width (stride) of destination buffer in pixels
 * @param[in] src source buffer
 * @param[in] src_with width (stride) of source buffer in pixels
 * @param[in] copy_w width of area to be copied from src to dest
 * @param[in] copy_h height of area to be copied from src to dest
 * @param[in] opa opacity of the result
 */
void lv_gpu_nxp_pxp_blit(lv_color_t * dest, lv_coord_t dest_width, const lv_color_t * src, lv_coord_t src_width,
                         lv_coord_t copy_width, lv_coord_t copy_height, lv_opa_t opa)
{

    if(recolorEnabled) {  /* switch to recolor version of blit */
        lv_gpu_nxp_pxp_blit_recolor(dest,  dest_width, src, src_width, copy_width, copy_height, opa, recolor, recolorOpa);
        return;
    };

    PXP_Init(PXP);
    PXP_EnableCsc1(PXP, false);     /* Disable CSC1, it is enabled by default. */
    PXP_SetProcessBlockSize(PXP, kPXP_BlockSize16); /* block size 16x16 for higher performance */

    pxp_output_buffer_config_t outputBufferConfig;
    pxp_as_buffer_config_t asBufferConfig;
    pxp_as_blend_config_t asBlendConfig;

    asBlendConfig.alpha = opa;
    asBlendConfig.invertAlpha = false;
    asBlendConfig.alphaMode = kPXP_AlphaRop;
    asBlendConfig.ropMode = kPXP_RopMergeAs;

    if(opa >= LV_OPA_MAX && !colorKeyEnabled) {
        /* Simple blit, no effect - Disable PS buffer */
        PXP_SetProcessSurfacePosition(LV_GPU_NXP_PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);
    }
    else {
        /* Alpha blending or color keying enabled - PS must be enabled to fetch background pixels
           PS and OUT buffers are the same, blend will be done in-place */
        pxp_ps_buffer_config_t psBufferConfig = {
            .pixelFormat = PXP_PS_PIXEL_FORMAT,
            .swapByte    = false,
            .bufferAddr  = (uint32_t)dest,
            .bufferAddrU = 0U,
            .bufferAddrV = 0U,
            .pitchBytes  = dest_width * sizeof(lv_color_t)
        };
        asBlendConfig.alphaMode = kPXP_AlphaOverride;
        PXP_SetProcessSurfaceBufferConfig(LV_GPU_NXP_PXP_ID, &psBufferConfig);
        PXP_SetProcessSurfacePosition(LV_GPU_NXP_PXP_ID, 0U, 0U, copy_width - 1, copy_height - 1);
    }

    /* AS buffer - source image */
    asBufferConfig.pixelFormat = PXP_AS_PIXEL_FORMAT;
    asBufferConfig.bufferAddr  = (uint32_t)src;
    asBufferConfig.pitchBytes  = src_width * sizeof(lv_color_t);
    PXP_SetAlphaSurfaceBufferConfig(LV_GPU_NXP_PXP_ID, &asBufferConfig);
    PXP_SetAlphaSurfacePosition(LV_GPU_NXP_PXP_ID, 0U, 0U, copy_width - 1U, copy_height - 1U);
    PXP_SetAlphaSurfaceBlendConfig(LV_GPU_NXP_PXP_ID, &asBlendConfig);

    lv_gpu_nxp_invalidate_cache(asBufferConfig.bufferAddr, copy_width, copy_height, asBufferConfig.pitchBytes,
                                sizeof(lv_color_t));

    if(colorKeyEnabled) {
        PXP_SetAlphaSurfaceOverlayColorKey(LV_GPU_NXP_PXP_ID, colorKey, colorKey);
    }
    PXP_EnableAlphaSurfaceOverlayColorKey(LV_GPU_NXP_PXP_ID, colorKeyEnabled);


    /* Output buffer. */
    outputBufferConfig.pixelFormat    = (pxp_output_pixel_format_t)PXP_OUT_PIXEL_FORMAT;
    outputBufferConfig.interlacedMode = kPXP_OutputProgressive;
    outputBufferConfig.buffer0Addr    = (uint32_t)dest;
    outputBufferConfig.buffer1Addr    = (uint32_t)0U;
    outputBufferConfig.pitchBytes     = dest_width * sizeof(lv_color_t);
    outputBufferConfig.width          = copy_width;
    outputBufferConfig.height         = copy_height;
    PXP_SetOutputBufferConfig(LV_GPU_NXP_PXP_ID, &outputBufferConfig);

    lv_gpu_nxp_invalidate_cache(outputBufferConfig.buffer0Addr, outputBufferConfig.width, outputBufferConfig.height,
                                outputBufferConfig.pitchBytes, sizeof(lv_color_t));

    lv_gpu_nxp_pxp_run(); /* Start PXP task */
}

/**
 * @brief Enable color keying for subsequent calls to lv_gpu_nxp_pxp_blit()
 *
 * Color key is defined by LV_COLOR_TRANSP symbol in lv_conf.h
 */
void lv_gpu_nxp_pxp_enable_color_key(void)
{
    colorKeyEnabled = true;
}

/**
 * @brief Disable color keying for subsequent calls to lv_gpu_nxp_pxp_blit()
 *
 */
void lv_gpu_nxp_pxp_disable_color_key(void)
{
    colorKeyEnabled = false;
}

/**
 * @brief Enable recolor feature for subsequent calls to lv_gpu_nxp_pxp_blit()
 *
 * @param[in] color recolor value
 * @param[in] opa effect opacity
 */
void lv_gpu_nxp_pxp_enable_recolor(lv_color_t color, lv_opa_t opa)
{
    recolorEnabled = true;
    recolor = color;
    recolorOpa = opa;

}

/**
 * @brief Disable recolor feature for subsequent calls to lv_gpu_nxp_pxp_blit()
 */
void lv_gpu_nxp_pxp_disable_recolor(void)
{
    recolorEnabled = false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief Start PXP job and wait for results
 *
 * Function used internally to start PXP task according current device
 * configuration.
 */
static void lv_gpu_nxp_pxp_run(void)
{
    pxp_cfg.pxp_run();
}

/**
 * @brief BLock Image Transfer - copy rectangular image from src buffer to dst buffer with recoloring.
 *
 * Note that color keying and recoloring at the same time is not supported and black rectangle is rendered.
 *
 * @param[in/out] dest destination buffer
 * @param[in] dest_width width (stride) of destination buffer in pixels
 * @param[in] src source buffer
 * @param[in] src_with width (stride) of source buffer in pixels
 * @param[in] copy_w width of area to be copied from src to dest
 * @param[in] copy_h height of area to be copied from src to dest
 * @param[in] opa opacity of the result
 * @param[in] recolor recolor value
 * @param[in] recolorOpa effect opacity
 */
static void lv_gpu_nxp_pxp_blit_recolor(lv_color_t * dest, lv_coord_t dest_width, const lv_color_t * src,
                                        lv_coord_t src_width,
                                        lv_coord_t copy_width, lv_coord_t copy_height, lv_opa_t opa, lv_color_t recolor, lv_opa_t recolorOpa)
{
    pxp_output_buffer_config_t outputBufferConfig;
    pxp_as_buffer_config_t asBufferConfig;

    if(colorKeyEnabled) {
        /* should never get here, recolor & color keying not supported. Draw black box instead. */
        const lv_area_t fill_area = {.x1 = 0, .y1 = 0, .x2 = copy_width - 1, .y2 = copy_height - 1};
        lv_gpu_nxp_pxp_fill(dest, dest_width, &fill_area, LV_COLOR_BLACK, LV_OPA_MAX);
        LV_LOG_WARN("Recoloring and color keying is not supported. Black rectangle rendered.");
        return ;
    }
    else {
        /* Recoloring without color keying */
        if(opa > LV_OPA_MAX) {
            /* Recolor with full opacity - AS source image, PS color generator, OUT destination */
            PXP_Init(PXP);
            PXP_EnableCsc1(PXP, false); /* Disable CSC1, it is enabled by default. */
            PXP_SetProcessBlockSize(PXP, kPXP_BlockSize16); /* block size 16x16 for higher performance */

            /* AS buffer - source image */
            asBufferConfig.pixelFormat = PXP_AS_PIXEL_FORMAT;
            asBufferConfig.bufferAddr  = (uint32_t)src;
            asBufferConfig.pitchBytes  = src_width * sizeof(lv_color_t);
            PXP_SetAlphaSurfaceBufferConfig(LV_GPU_NXP_PXP_ID, &asBufferConfig);
            PXP_SetAlphaSurfacePosition(LV_GPU_NXP_PXP_ID, 0U, 0U, copy_width - 1U, copy_height - 1U);

            lv_gpu_nxp_invalidate_cache(asBufferConfig.bufferAddr, copy_width, copy_height, asBufferConfig.pitchBytes,
                                        sizeof(lv_color_t));

            /* Disable PS buffer, use as color generator */
            PXP_SetProcessSurfacePosition(LV_GPU_NXP_PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);
            PXP_SetProcessSurfaceBackGroundColor(LV_GPU_NXP_PXP_ID, lv_color_to32(recolor));

            /* Output buffer */
            outputBufferConfig.pixelFormat    = (pxp_output_pixel_format_t)PXP_OUT_PIXEL_FORMAT;
            outputBufferConfig.interlacedMode = kPXP_OutputProgressive;
            outputBufferConfig.buffer0Addr    = (uint32_t)dest;
            outputBufferConfig.buffer1Addr    = (uint32_t)0U;
            outputBufferConfig.pitchBytes     = dest_width * sizeof(lv_color_t);
            outputBufferConfig.width          = copy_width;
            outputBufferConfig.height         = copy_height;
            PXP_SetOutputBufferConfig(LV_GPU_NXP_PXP_ID, &outputBufferConfig);

            lv_gpu_nxp_invalidate_cache(outputBufferConfig.buffer0Addr, outputBufferConfig.width, outputBufferConfig.height,
                                        outputBufferConfig.pitchBytes, sizeof(lv_color_t));

            pxp_porter_duff_config_t pdConfig;

            /* Configure Porter-Duff blending - For RGB 565 only! */
            pdConfig.enable = 1;
            pdConfig.dstColorMode = kPXP_PorterDuffColorStraight;
            pdConfig.srcColorMode = kPXP_PorterDuffColorStraight;
            pdConfig.dstGlobalAlphaMode = kPXP_PorterDuffGlobalAlpha;
            pdConfig.srcGlobalAlphaMode = kPXP_PorterDuffGlobalAlpha;
            pdConfig.srcFactorMode = kPXP_PorterDuffFactorStraight;
            pdConfig.dstFactorMode = kPXP_PorterDuffFactorStraight;
            pdConfig.srcGlobalAlpha = recolorOpa;
            pdConfig.dstGlobalAlpha = 255 - recolorOpa;
            pdConfig.srcAlphaMode = kPXP_PorterDuffAlphaStraight; /* don't care */
            pdConfig.dstAlphaMode = kPXP_PorterDuffAlphaStraight; /* don't care */
            PXP_SetPorterDuffConfig(LV_GPU_NXP_PXP_ID, &pdConfig);

            lv_gpu_nxp_pxp_run(); /* Start PXP task */

        }
        else {
            /* Recolor with transparency */

            /* Step 1: Recolor with full opacity to temporary buffer */
            lv_color_t * tmpBuf  = (lv_color_t *) _lv_mem_buf_get(copy_width * copy_height * sizeof(lv_color_t));
            lv_gpu_nxp_pxp_blit_recolor(tmpBuf, copy_width, src, src_width, copy_width, copy_height, LV_OPA_COVER, recolor,
                                        recolorOpa);

            /* Step 2: BLIT temporary results with required opacity to output */
            lv_gpu_nxp_pxp_disable_recolor(); /* make sure to take BLIT path, not the recolor */
            lv_gpu_nxp_pxp_blit(dest, dest_width, tmpBuf, copy_width, copy_width, copy_height, opa);
            lv_gpu_nxp_pxp_enable_recolor(recolor, recolorOpa); /* restore state */

            /* Step 3: Clean-up memory */
            _lv_mem_buf_release(tmpBuf);
        }
    }
}

/**
 * @brief Invalidate cache for rectangular area of memory
 *
 * @param[in] address starting address of area
 * @param[in] width width of area in pixels
 * @param[in] height height of area in pixels
 * @param[in] stride stride in bytes
 * @param[in] pxSize pixel size in bytes
 */
static void lv_gpu_nxp_invalidate_cache(uint32_t address, uint32_t width, uint32_t height, uint32_t stride,
                                        uint32_t pxSize)
{
    int y;

    for(y = 0; y < height; y++) {
        DCACHE_CleanInvalidateByRange(address, width * pxSize);
        address += stride;
    }
}
#endif /* LV_USE_GPU && LV_USE_GPU_NXP_PXP */
