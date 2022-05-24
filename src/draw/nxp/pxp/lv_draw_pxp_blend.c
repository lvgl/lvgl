/**
 * @file lv_draw_pxp_blend.c
 *
 */

/**
 * MIT License
 *
 * Copyright 2020-2022 NXP
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

#include "lv_draw_pxp_blend.h"

#if LV_USE_GPU_NXP_PXP

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
#elif LV_COLOR_DEPTH==32
    #define PXP_OUT_PIXEL_FORMAT kPXP_OutputPixelFormatARGB8888
    #define PXP_AS_PIXEL_FORMAT kPXP_AsPixelFormatARGB8888
    #define PXP_PS_PIXEL_FORMAT kPXP_PsPixelFormatRGB888
#elif
    #error Only 16bit and 32bit color depth are supported. Set LV_COLOR_DEPTH to 16 or 32.
#endif

#if defined (__alpha__) || defined (__ia64__) || defined (__x86_64__) \
    || defined (_WIN64) || defined (__LP64__) || defined (__LLP64__)
    #define ALIGN_SIZE 8
#else
    #define ALIGN_SIZE 4
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * BLock Image Transfer - copy rectangular image from src buffer to dst buffer
 * with combination of transformation (rotation, scale, recolor) and opacity, alpha channel
 * or color keying. This requires two steps. First step is used for transformation into
 * a temporary buffer and the second one will handle the color format or opacity.
 *
 * @param[in/out] dest_buf destination buffer
 * @param[in] dest_area area to be copied from src_buf to dst_buf
 * @param[in] dest_stride width (stride) of destination buffer in pixels
 * @param[in] src_buf source buffer
 * @param[in] src_area source area with absolute coordinates to draw on destination buffer
 * @param[in] dsc image descriptor
 * @param[in] cf color format
 * @retval LV_RES_OK Fill completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_PXP_LOG_ERRORS)
 */
static lv_res_t lv_gpu_nxp_pxp_blit_opa(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                        const lv_color_t * src_buf, const lv_area_t * src_area,
                                        const lv_draw_img_dsc_t * dsc, lv_img_cf_t cf);

/**
 * BLock Image Transfer - copy rectangular image from src buffer to dst buffer
 * with transformation and full opacity.
 *
 * @param[in/out] dest_buf destination buffer
 * @param[in] dest_area area to be copied from src_buf to dst_buf
 * @param[in] dest_stride width (stride) of destination buffer in pixels
 * @param[in] src_buf source buffer
 * @param[in] src_area source area with absolute coordinates to draw on destination buffer
 * @param[in] dsc image descriptor
 * @param[in] cf color format
 * @retval LV_RES_OK Fill completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_PXP_LOG_ERRORS)
 */
static lv_res_t lv_gpu_nxp_pxp_blit_cover(lv_color_t * dest_buf, const lv_area_t * dest_area,
                                          lv_coord_t dest_stride,
                                          const lv_color_t * src_buf, const lv_area_t * src_area,
                                          const lv_draw_img_dsc_t * dsc, lv_img_cf_t cf);

/**
 * BLock Image Transfer - copy rectangular image from src buffer to dst buffer
 * without transformation but handling color format or opacity.
 *
 * @param[in/out] dest_buf destination buffer
 * @param[in] dest_area area to be copied from src_buf to dst_buf
 * @param[in] dest_stride width (stride) of destination buffer in pixels
 * @param[in] src_buf source buffer
 * @param[in] src_area source area with absolute coordinates to draw on destination buffer
 * @param[in] dsc image descriptor
 * @param[in] cf color format
 * @retval LV_RES_OK Fill completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_PXP_LOG_ERRORS)
 */
static lv_res_t lv_gpu_nxp_pxp_blit_cf(lv_color_t * dest_buf, const lv_area_t * dest_area,
                                       lv_coord_t dest_stride,
                                       const lv_color_t * src_buf, const lv_area_t * src_area,
                                       const lv_draw_img_dsc_t * dsc, lv_img_cf_t cf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#define ROUND_UP(x, align) ((x + (align - 1)) & ~(align - 1))

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_gpu_nxp_pxp_fill(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area,
                             lv_color_t color, lv_opa_t opa)
{
    uint32_t area_size = lv_area_get_size(fill_area);
    lv_coord_t area_w = lv_area_get_width(fill_area);
    lv_coord_t area_h = lv_area_get_height(fill_area);

    if(opa >= (lv_opa_t)LV_OPA_MAX) {
        if(area_size < LV_GPU_NXP_PXP_FILL_SIZE_LIMIT) {
            PXP_LOG_TRACE("Area size %d smaller than limit %d.", area_size, LV_GPU_NXP_PXP_FILL_SIZE_LIMIT);
            return LV_RES_INV;
        }
    }
    else {
        if(area_size < LV_GPU_NXP_PXP_FILL_OPA_SIZE_LIMIT) {
            PXP_LOG_TRACE("Area size %d smaller than limit %d.", area_size, LV_GPU_NXP_PXP_FILL_OPA_SIZE_LIMIT);
            return LV_RES_INV;
        }
    }

    PXP_Init(LV_GPU_NXP_PXP_ID);
    PXP_EnableCsc1(LV_GPU_NXP_PXP_ID, false); /*Disable CSC1, it is enabled by default.*/
    PXP_SetProcessBlockSize(LV_GPU_NXP_PXP_ID, kPXP_BlockSize16); /*Block size 16x16 for higher performance*/

    /*OUT buffer configure*/
    pxp_output_buffer_config_t outputConfig = {
        .pixelFormat = PXP_OUT_PIXEL_FORMAT,
        .interlacedMode = kPXP_OutputProgressive,
        .buffer0Addr = (uint32_t)(dest_buf + dest_stride * fill_area->y1 + fill_area->x1),
        .buffer1Addr = (uint32_t)NULL,
        .pitchBytes = dest_stride * sizeof(lv_color_t),
        .width = area_w,
        .height = area_h
    };

    PXP_SetOutputBufferConfig(LV_GPU_NXP_PXP_ID, &outputConfig);

    if(opa >= (lv_opa_t)LV_OPA_MAX) {
        /*Simple color fill without opacity - AS disabled*/
        PXP_SetAlphaSurfacePosition(LV_GPU_NXP_PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);

    }
    else {
        /*Fill with opacity - AS used as source (same as OUT)*/
        pxp_as_buffer_config_t asBufferConfig = {
            .pixelFormat = PXP_AS_PIXEL_FORMAT,
            .bufferAddr = (uint32_t)outputConfig.buffer0Addr,
            .pitchBytes = outputConfig.pitchBytes
        };

        PXP_SetAlphaSurfaceBufferConfig(LV_GPU_NXP_PXP_ID, &asBufferConfig);
        PXP_SetAlphaSurfacePosition(LV_GPU_NXP_PXP_ID, 0U, 0U, area_w, area_h);
    }

    /*Disable PS, use as color generator*/
    PXP_SetProcessSurfacePosition(LV_GPU_NXP_PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);
    PXP_SetProcessSurfaceBackGroundColor(LV_GPU_NXP_PXP_ID, lv_color_to32(color));

    /**
     * Configure Porter-Duff blending - src settings are unused for fill without opacity (opa = 0xff).
     *
     * Note: srcFactorMode and dstFactorMode are inverted in fsl_pxp.h:
     * srcFactorMode is actually applied on PS alpha value
     * dstFactorMode is actually applied on AS alpha value
     */
    pxp_porter_duff_config_t pdConfig = {
        .enable = 1,
        .dstColorMode = kPXP_PorterDuffColorNoAlpha,
        .srcColorMode = kPXP_PorterDuffColorNoAlpha,
        .dstGlobalAlphaMode = kPXP_PorterDuffGlobalAlpha,
        .srcGlobalAlphaMode = kPXP_PorterDuffGlobalAlpha,
        .dstFactorMode = kPXP_PorterDuffFactorStraight,
        .srcFactorMode = (opa >= (lv_opa_t)LV_OPA_MAX) ? kPXP_PorterDuffFactorStraight : kPXP_PorterDuffFactorInversed,
        .dstGlobalAlpha = opa,
        .srcGlobalAlpha = opa,
        .dstAlphaMode = kPXP_PorterDuffAlphaStraight, /*don't care*/
        .srcAlphaMode = kPXP_PorterDuffAlphaStraight  /*don't care*/
    };

    PXP_SetPorterDuffConfig(LV_GPU_NXP_PXP_ID, &pdConfig);

    lv_gpu_nxp_pxp_run(); /*Start PXP task*/

    return LV_RES_OK;
}

lv_res_t lv_gpu_nxp_pxp_blit(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                             const lv_color_t * src_buf, const lv_area_t * src_area, lv_opa_t opa, lv_disp_rot_t angle)
{
    uint32_t dest_size = lv_area_get_size(dest_area);
    lv_coord_t dest_w = lv_area_get_width(dest_area);
    lv_coord_t dest_h = lv_area_get_height(dest_area);

    if(opa >= (lv_opa_t)LV_OPA_MAX) {
        if(dest_size < LV_GPU_NXP_PXP_BLIT_SIZE_LIMIT) {
            PXP_LOG_TRACE("Area size %d smaller than limit %d.", dest_size, LV_GPU_NXP_PXP_BLIT_SIZE_LIMIT);
            return LV_RES_INV;
        }
    }
    else {
        if(dest_size < LV_GPU_NXP_PXP_BLIT_OPA_SIZE_LIMIT) {
            PXP_LOG_TRACE("Area size %d smaller than limit %d.", dest_size, LV_GPU_NXP_PXP_BLIT_OPA_SIZE_LIMIT);
            return LV_RES_INV;
        }
    }

    PXP_Init(LV_GPU_NXP_PXP_ID);
    PXP_EnableCsc1(LV_GPU_NXP_PXP_ID, false); /*Disable CSC1, it is enabled by default.*/
    PXP_SetProcessBlockSize(LV_GPU_NXP_PXP_ID, kPXP_BlockSize16); /*block size 16x16 for higher performance*/

    /* convert rotation angle */
    pxp_rotate_degree_t pxp_rot;
    switch(angle) {
        case LV_DISP_ROT_NONE:
            pxp_rot = kPXP_Rotate0;
            break;
        case LV_DISP_ROT_90:
            pxp_rot = kPXP_Rotate90;
            break;
        case LV_DISP_ROT_180:
            pxp_rot = kPXP_Rotate180;
            break;
        case LV_DISP_ROT_270:
            pxp_rot = kPXP_Rotate270;
            break;
        default:
            pxp_rot = kPXP_Rotate0;
            break;
    }
    PXP_SetRotateConfig(LV_GPU_NXP_PXP_ID, kPXP_RotateOutputBuffer, pxp_rot, kPXP_FlipDisable);

    pxp_as_blend_config_t asBlendConfig = {
        .alpha = opa,
        .invertAlpha = false,
        .alphaMode = kPXP_AlphaRop,
        .ropMode = kPXP_RopMergeAs
    };

    if(opa >= (lv_opa_t)LV_OPA_MAX) {
        /*Simple blit, no effect - Disable PS buffer*/
        PXP_SetProcessSurfacePosition(LV_GPU_NXP_PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);
    }
    else {
        pxp_ps_buffer_config_t psBufferConfig = {
            .pixelFormat = PXP_PS_PIXEL_FORMAT,
            .swapByte = false,
            .bufferAddr = (uint32_t)(dest_buf + dest_stride * dest_area->y1 + dest_area->x1),
            .bufferAddrU = 0U,
            .bufferAddrV = 0U,
            .pitchBytes = dest_stride * sizeof(lv_color_t)
        };

        asBlendConfig.alphaMode = kPXP_AlphaOverride;

        PXP_SetProcessSurfaceBufferConfig(LV_GPU_NXP_PXP_ID, &psBufferConfig);
        PXP_SetProcessSurfacePosition(LV_GPU_NXP_PXP_ID, 0U, 0U, dest_w - 1, dest_h - 1);
    }

    lv_coord_t src_stride = lv_area_get_width(src_area);

    /*AS buffer - source image*/
    pxp_as_buffer_config_t asBufferConfig = {
        .pixelFormat = PXP_AS_PIXEL_FORMAT,
        .bufferAddr = (uint32_t)src_buf,
        .pitchBytes = src_stride * sizeof(lv_color_t)
    };
    PXP_SetAlphaSurfaceBufferConfig(LV_GPU_NXP_PXP_ID, &asBufferConfig);
    PXP_SetAlphaSurfacePosition(LV_GPU_NXP_PXP_ID, 0U, 0U, dest_w - 1U, dest_h - 1U);
    PXP_SetAlphaSurfaceBlendConfig(LV_GPU_NXP_PXP_ID, &asBlendConfig);
    PXP_EnableAlphaSurfaceOverlayColorKey(LV_GPU_NXP_PXP_ID, false);

    /*Output buffer.*/
    pxp_output_buffer_config_t outputBufferConfig = {
        .pixelFormat = (pxp_output_pixel_format_t)PXP_OUT_PIXEL_FORMAT,
        .interlacedMode = kPXP_OutputProgressive,
        .buffer0Addr = (uint32_t)(dest_buf + dest_stride * dest_area->y1 + dest_area->x1),
        .buffer1Addr = (uint32_t)0U,
        .pitchBytes = dest_stride * sizeof(lv_color_t),
        .width = dest_w,
        .height = dest_h
    };
    PXP_SetOutputBufferConfig(LV_GPU_NXP_PXP_ID, &outputBufferConfig);

    lv_gpu_nxp_pxp_run(); /* Start PXP task */

    return LV_RES_OK;
}

lv_res_t lv_gpu_nxp_pxp_blit_transform(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                       const lv_color_t * src_buf, const lv_area_t * src_area,
                                       const lv_draw_img_dsc_t * dsc, lv_img_cf_t cf)
{
    uint32_t dest_size = lv_area_get_size(dest_area);

    if(dsc->opa >= (lv_opa_t)LV_OPA_MAX) {
        if(dest_size < LV_GPU_NXP_PXP_BLIT_SIZE_LIMIT) {
            PXP_LOG_TRACE("Area size %d smaller than limit %d.", dest_size, LV_GPU_NXP_PXP_BLIT_SIZE_LIMIT);
            return LV_RES_INV;
        }
    }
    else {
        if(dest_size < LV_GPU_NXP_PXP_BLIT_OPA_SIZE_LIMIT) {
            PXP_LOG_TRACE("Area size %d smaller than limit %d.", dest_size, LV_GPU_NXP_PXP_BLIT_OPA_SIZE_LIMIT);
            return LV_RES_INV;
        }
    }

    bool recolor = (dsc->recolor_opa != LV_OPA_TRANSP);
    bool rotation = (dsc->angle != 0);

    if(rotation) {
        if(dsc->angle != 0 && dsc->angle != 900 && dsc->angle != 1800 && dsc->angle != 2700) {
            PXP_LOG_TRACE("Rotation angle %d is not supported. PXP can rotate only 90x angle.", dsc->angle);
            return LV_RES_INV;
        }
    }

    if(recolor || rotation) {
        if(dsc->opa >= (lv_opa_t)LV_OPA_MAX && !lv_img_cf_has_alpha(cf) && !lv_img_cf_is_chroma_keyed(cf))
            return lv_gpu_nxp_pxp_blit_cover(dest_buf, dest_area, dest_stride, src_buf, src_area, dsc, cf);
        else
            /*Recolor and/or rotation with alpha or opacity is done in two steps.*/
            return lv_gpu_nxp_pxp_blit_opa(dest_buf, dest_area, dest_stride, src_buf, src_area, dsc, cf);
    }

    return lv_gpu_nxp_pxp_blit_cf(dest_buf, dest_area, dest_stride, src_buf, src_area, dsc, cf);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_res_t lv_gpu_nxp_pxp_blit_opa(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                        const lv_color_t * src_buf, const lv_area_t * src_area,
                                        const lv_draw_img_dsc_t * dsc, lv_img_cf_t cf)
{
    lv_coord_t dest_w = lv_area_get_width(dest_area);
    lv_coord_t dest_h = lv_area_get_height(dest_area);
    lv_res_t res;
    uint32_t size = dest_w * dest_h * sizeof(lv_color_t);

    if(ROUND_UP(size, ALIGN_SIZE) >= LV_MEM_SIZE)
        PXP_RETURN_INV("Insufficient memory for temporary buffer. Please increase LV_MEM_SIZE.");

    lv_color_t * tmp_buf = (lv_color_t *)lv_mem_buf_get(size);
    if(!tmp_buf)
        PXP_RETURN_INV("Allocating temporary buffer failed.");

    const lv_area_t tmp_area = {
        .x1 = 0,
        .y1 = 0,
        .x2 = dest_w - 1,
        .y2 = dest_h - 1
    };

    /*Step 1: Transform with full opacity to temporary buffer*/
    res = lv_gpu_nxp_pxp_blit_cover(tmp_buf, &tmp_area, dest_w, src_buf, src_area, dsc, cf);
    if(res != LV_RES_OK) {
        PXP_LOG_TRACE("Blit cover with full opacity failed.");
        lv_mem_buf_release(tmp_buf);

        return res;
    }

    /*Step 2: Blit temporary results with required opacity to output*/
    res = lv_gpu_nxp_pxp_blit_cf(dest_buf, dest_area, dest_stride, tmp_buf, &tmp_area, dsc, cf);

    /*Clean-up memory*/
    lv_mem_buf_release(tmp_buf);

    return res;
}

static lv_res_t lv_gpu_nxp_pxp_blit_cover(lv_color_t * dest_buf, const lv_area_t * dest_area,
                                          lv_coord_t dest_stride,
                                          const lv_color_t * src_buf, const lv_area_t * src_area,
                                          const lv_draw_img_dsc_t * dsc, lv_img_cf_t cf)
{
    lv_coord_t dest_w = lv_area_get_width(dest_area);
    lv_coord_t dest_h = lv_area_get_height(dest_area);

    bool recolor = (dsc->recolor_opa != LV_OPA_TRANSP);
    bool rotation = (dsc->angle != 0);

    PXP_Init(LV_GPU_NXP_PXP_ID);
    PXP_EnableCsc1(LV_GPU_NXP_PXP_ID, false); /*Disable CSC1, it is enabled by default.*/
    PXP_SetProcessBlockSize(LV_GPU_NXP_PXP_ID, kPXP_BlockSize16); /*block size 16x16 for higher performance*/

    if(rotation) {
        /*
         * PXP is set to process 16x16 blocks to optimize the system for memory
         * bandwidth and image processing time.
         * The output engine essentially truncates any output pixels after the
         * desired number of pixels has been written.
         * When rotating a source image and the output is not divisible by the block
         * size, the incorrect pixels could be truncated and the final output image
         * can look shifted.
         */
        if(lv_area_get_width(src_area) % 16 || lv_area_get_height(src_area) % 16) {
            PXP_LOG_TRACE("Rotation is not supported for image w/o alignment to block size 16x16.");
            return LV_RES_INV;
        }

        /*Convert rotation angle*/
        pxp_rotate_degree_t pxp_rot;
        switch(dsc->angle) {
            case 0:
                pxp_rot = kPXP_Rotate0;
                break;
            case 900:
                pxp_rot = kPXP_Rotate90;
                break;
            case 1800:
                pxp_rot = kPXP_Rotate180;
                break;
            case 2700:
                pxp_rot = kPXP_Rotate270;
                break;
            default:
                PXP_LOG_TRACE("Rotation angle %d is not supported. PXP can rotate only 90x angle.", dsc->angle);
                return LV_RES_INV;
        }
        PXP_SetRotateConfig(LV_GPU_NXP_PXP_ID, kPXP_RotateOutputBuffer, pxp_rot, kPXP_FlipDisable);
    }

    lv_coord_t src_stride = lv_area_get_width(src_area);

    /*AS buffer - source image*/
    pxp_as_buffer_config_t asBufferConfig = {
        .pixelFormat = PXP_AS_PIXEL_FORMAT,
        .bufferAddr = (uint32_t)src_buf,
        .pitchBytes = src_stride * sizeof(lv_color_t)
    };
    PXP_SetAlphaSurfaceBufferConfig(LV_GPU_NXP_PXP_ID, &asBufferConfig);
    PXP_SetAlphaSurfacePosition(LV_GPU_NXP_PXP_ID, 0U, 0U, dest_w - 1U, dest_h - 1U);

    /*Disable PS buffer*/
    PXP_SetProcessSurfacePosition(LV_GPU_NXP_PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);
    if(recolor)
        /*Use as color generator*/
        PXP_SetProcessSurfaceBackGroundColor(LV_GPU_NXP_PXP_ID, lv_color_to32(dsc->recolor));

    /*Output buffer*/
    pxp_output_buffer_config_t outputBufferConfig = {
        .pixelFormat = (pxp_output_pixel_format_t)PXP_OUT_PIXEL_FORMAT,
        .interlacedMode = kPXP_OutputProgressive,
        .buffer0Addr = (uint32_t)(dest_buf + dest_stride * dest_area->y1 + dest_area->x1),
        .buffer1Addr = (uint32_t)0U,
        .pitchBytes = dest_stride * sizeof(lv_color_t),
        .width = dest_w,
        .height = dest_h
    };
    PXP_SetOutputBufferConfig(LV_GPU_NXP_PXP_ID, &outputBufferConfig);

    if(recolor || lv_img_cf_has_alpha(cf)) {
        /**
         * Configure Porter-Duff blending.
         *
         * Note: srcFactorMode and dstFactorMode are inverted in fsl_pxp.h:
         * srcFactorMode is actually applied on PS alpha value
         * dstFactorMode is actually applied on AS alpha value
         */
        pxp_porter_duff_config_t pdConfig = {
            .enable = 1,
            .dstColorMode = kPXP_PorterDuffColorWithAlpha,
            .srcColorMode = kPXP_PorterDuffColorNoAlpha,
            .dstGlobalAlphaMode = kPXP_PorterDuffGlobalAlpha,
            .srcGlobalAlphaMode = lv_img_cf_has_alpha(cf) ? kPXP_PorterDuffLocalAlpha : kPXP_PorterDuffGlobalAlpha,
            .dstFactorMode = kPXP_PorterDuffFactorStraight,
            .srcFactorMode = kPXP_PorterDuffFactorInversed,
            .dstGlobalAlpha = recolor ? dsc->recolor_opa : 0x00,
            .srcGlobalAlpha = 0xff,
            .dstAlphaMode = kPXP_PorterDuffAlphaStraight, /*don't care*/
            .srcAlphaMode = kPXP_PorterDuffAlphaStraight
        };
        PXP_SetPorterDuffConfig(LV_GPU_NXP_PXP_ID, &pdConfig);
    }

    lv_gpu_nxp_pxp_run(); /*Start PXP task*/

    return LV_RES_OK;
}

static lv_res_t lv_gpu_nxp_pxp_blit_cf(lv_color_t * dest_buf, const lv_area_t * dest_area,
                                       lv_coord_t dest_stride,
                                       const lv_color_t * src_buf, const lv_area_t * src_area,
                                       const lv_draw_img_dsc_t * dsc, lv_img_cf_t cf)
{
    lv_coord_t dest_w = lv_area_get_width(dest_area);
    lv_coord_t dest_h = lv_area_get_height(dest_area);

    PXP_Init(LV_GPU_NXP_PXP_ID);
    PXP_EnableCsc1(LV_GPU_NXP_PXP_ID, false); /*Disable CSC1, it is enabled by default.*/
    PXP_SetProcessBlockSize(LV_GPU_NXP_PXP_ID, kPXP_BlockSize16); /*block size 16x16 for higher performance*/

    pxp_as_blend_config_t asBlendConfig = {
        .alpha = dsc->opa,
        .invertAlpha = false,
        .alphaMode = kPXP_AlphaRop,
        .ropMode = kPXP_RopMergeAs
    };

    if(dsc->opa >= (lv_opa_t)LV_OPA_MAX && !lv_img_cf_is_chroma_keyed(cf) && !lv_img_cf_has_alpha(cf)) {
        /*Simple blit, no effect - Disable PS buffer*/
        PXP_SetProcessSurfacePosition(LV_GPU_NXP_PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);
    }
    else {
        /*PS must be enabled to fetch background pixels.
          PS and OUT buffers are the same, blend will be done in-place*/
        pxp_ps_buffer_config_t psBufferConfig = {
            .pixelFormat = PXP_PS_PIXEL_FORMAT,
            .swapByte = false,
            .bufferAddr = (uint32_t)(dest_buf + dest_stride * dest_area->y1 + dest_area->x1),
            .bufferAddrU = 0U,
            .bufferAddrV = 0U,
            .pitchBytes = dest_stride * sizeof(lv_color_t)
        };
        if(dsc->opa >= (lv_opa_t)LV_OPA_MAX) {
            asBlendConfig.alphaMode = lv_img_cf_has_alpha(cf) ? kPXP_AlphaEmbedded : kPXP_AlphaOverride;
        }
        else {
            asBlendConfig.alphaMode = lv_img_cf_has_alpha(cf) ? kPXP_AlphaMultiply : kPXP_AlphaOverride;
        }
        PXP_SetProcessSurfaceBufferConfig(LV_GPU_NXP_PXP_ID, &psBufferConfig);
        PXP_SetProcessSurfacePosition(LV_GPU_NXP_PXP_ID, 0U, 0U, dest_w - 1, dest_h - 1);
    }

    lv_coord_t src_stride = lv_area_get_width(src_area);

    /*AS buffer - source image*/
    pxp_as_buffer_config_t asBufferConfig = {
        .pixelFormat = PXP_AS_PIXEL_FORMAT,
        .bufferAddr = (uint32_t)src_buf,
        .pitchBytes = src_stride * sizeof(lv_color_t)
    };
    PXP_SetAlphaSurfaceBufferConfig(LV_GPU_NXP_PXP_ID, &asBufferConfig);
    PXP_SetAlphaSurfacePosition(LV_GPU_NXP_PXP_ID, 0U, 0U, dest_w - 1U, dest_h - 1U);
    PXP_SetAlphaSurfaceBlendConfig(LV_GPU_NXP_PXP_ID, &asBlendConfig);

    if(lv_img_cf_is_chroma_keyed(cf)) {
        lv_color_t colorKeyLow = LV_COLOR_CHROMA_KEY;
        lv_color_t colorKeyHigh = LV_COLOR_CHROMA_KEY;

        bool recolor = (dsc->recolor_opa != LV_OPA_TRANSP);

        if(recolor) {
            /* New color key after recoloring */
            lv_color_t colorKey =  lv_color_mix(dsc->recolor, LV_COLOR_CHROMA_KEY, dsc->recolor_opa);

            LV_COLOR_SET_R(colorKeyLow, colorKey.ch.red != 0 ? colorKey.ch.red - 1 : 0);
            LV_COLOR_SET_G(colorKeyLow, colorKey.ch.green != 0 ? colorKey.ch.green - 1 : 0);
            LV_COLOR_SET_B(colorKeyLow, colorKey.ch.blue != 0 ? colorKey.ch.blue - 1 : 0);

#if LV_COLOR_DEPTH==16
            LV_COLOR_SET_R(colorKeyHigh, colorKey.ch.red != 0x1f ? colorKey.ch.red + 1 : 0x1f);
            LV_COLOR_SET_G(colorKeyHigh, colorKey.ch.green != 0x3f ? colorKey.ch.green + 1 : 0x3f);
            LV_COLOR_SET_B(colorKeyHigh, colorKey.ch.blue != 0x1f ? colorKey.ch.blue + 1 : 0x1f);
#else /*LV_COLOR_DEPTH==32*/
            LV_COLOR_SET_R(colorKeyHigh, colorKey.ch.red != 0xff ? colorKey.ch.red + 1 : 0xff);
            LV_COLOR_SET_G(colorKeyHigh, colorKey.ch.green != 0xff ? colorKey.ch.green + 1 : 0xff);
            LV_COLOR_SET_B(colorKeyHigh, colorKey.ch.blue != 0xff ? colorKey.ch.blue + 1 : 0xff);
#endif
        }

        PXP_SetAlphaSurfaceOverlayColorKey(LV_GPU_NXP_PXP_ID, lv_color_to32(colorKeyLow),
                                           lv_color_to32(colorKeyHigh));
    }

    PXP_EnableAlphaSurfaceOverlayColorKey(LV_GPU_NXP_PXP_ID, lv_img_cf_is_chroma_keyed(cf));

    /*Output buffer.*/
    pxp_output_buffer_config_t outputBufferConfig = {
        .pixelFormat = (pxp_output_pixel_format_t)PXP_OUT_PIXEL_FORMAT,
        .interlacedMode = kPXP_OutputProgressive,
        .buffer0Addr = (uint32_t)(dest_buf + dest_stride * dest_area->y1 + dest_area->x1),
        .buffer1Addr = (uint32_t)0U,
        .pitchBytes = dest_stride * sizeof(lv_color_t),
        .width = dest_w,
        .height = dest_h
    };
    PXP_SetOutputBufferConfig(LV_GPU_NXP_PXP_ID, &outputBufferConfig);

    lv_gpu_nxp_pxp_run(); /* Start PXP task */

    return LV_RES_OK;
}

#endif /*LV_USE_GPU_NXP_PXP*/
