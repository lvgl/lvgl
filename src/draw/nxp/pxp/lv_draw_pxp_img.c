/**
 * @file lv_draw_pxp_img.c
 *
 */

/**
 * Copyright 2020-2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_pxp.h"

#if LV_USE_PXP
#if LV_USE_DRAW_PXP
#include "lv_pxp_cfg.h"
#include "lv_pxp_utils.h"

#include <math.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/* Blit w/ recolor for images w/o opa and alpha channel */
static void _pxp_blit_recolor(uint8_t * dest_buf, const lv_area_t * dest_area, int32_t dest_stride,
                              lv_color_format_t dest_cf, const uint8_t * src_buf, const lv_area_t * src_area,
                              int32_t src_stride, lv_color_format_t src_cf, const lv_draw_image_dsc_t * dsc);

/* Blit w/ transformation for images w/o opa and alpha channel */
static void _pxp_blit_transform(uint8_t * dest_buf, const lv_area_t * dest_area, int32_t dest_stride,
                                lv_color_format_t dest_cf, const uint8_t * src_buf, const lv_area_t * src_area,
                                int32_t src_stride, lv_color_format_t src_cf, const lv_draw_image_dsc_t * dsc);

/* Blit simple w/ opa and alpha channel */
static void _pxp_blit(uint8_t * dest_buf, const lv_area_t * dest_area, int32_t dest_stride,
                      lv_color_format_t dest_cf, const uint8_t * src_buf, const lv_area_t * src_area,
                      int32_t src_stride, lv_color_format_t src_cf, lv_opa_t opa);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_pxp_img(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc,
                     const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_layer_t * layer = t->target_layer;
    lv_draw_buf_t * draw_buf = layer->draw_buf;
    const lv_image_dsc_t * img_dsc = dsc->src;

    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, &t->clip_area);
    lv_area_move(&rel_clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t blend_area;
    bool has_transform = (dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE);
    if(has_transform)
        lv_area_copy(&blend_area, &rel_coords);
    else if(!lv_area_intersect(&blend_area, &rel_coords, &rel_clip_area))
        return; /*Fully clipped, nothing to do*/

    const uint8_t * src_buf = img_dsc->data;

    lv_area_t src_area;
    src_area.x1 = blend_area.x1 - (coords->x1 - layer->buf_area.x1);
    src_area.y1 = blend_area.y1 - (coords->y1 - layer->buf_area.y1);
    src_area.x2 = src_area.x1 + lv_area_get_width(&blend_area) - 1;
    src_area.y2 = src_area.y1 + lv_area_get_height(&blend_area) - 1;
    int32_t src_stride = img_dsc->header.stride;
    lv_color_format_t src_cf = img_dsc->header.cf;

    uint8_t * dest_buf = draw_buf->data;
    int32_t dest_stride = draw_buf->header.stride;
    lv_color_format_t dest_cf = draw_buf->header.cf;
    bool has_recolor = (dsc->recolor_opa > LV_OPA_MIN);

    if(has_recolor && !has_transform)
        _pxp_blit_recolor(dest_buf, &blend_area, dest_stride, dest_cf,
                          src_buf, &src_area, src_stride, src_cf, dsc);
    else if(has_transform)
        _pxp_blit_transform(dest_buf, &blend_area, dest_stride, dest_cf,
                            src_buf, &src_area, src_stride, src_cf, dsc);
    else
        _pxp_blit(dest_buf, &blend_area, dest_stride, dest_cf,
                  src_buf, &src_area, src_stride, src_cf, dsc->opa);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _pxp_blit_recolor(uint8_t * dest_buf, const lv_area_t * dest_area, int32_t dest_stride,
                              lv_color_format_t dest_cf, const uint8_t * src_buf, const lv_area_t * src_area,
                              int32_t src_stride, lv_color_format_t src_cf, const lv_draw_image_dsc_t * dsc)
{

    int32_t dest_w = lv_area_get_width(dest_area);
    int32_t dest_h = lv_area_get_height(dest_area);
    int32_t src_w = lv_area_get_width(src_area);
    int32_t src_h = lv_area_get_height(src_area);

    bool src_has_alpha = (src_cf == LV_COLOR_FORMAT_ARGB8888);
    uint8_t src_px_size = lv_color_format_get_size(src_cf);
    uint8_t dest_px_size = lv_color_format_get_size(dest_cf);

    lv_pxp_reset();

    /*AS buffer - source image*/
    pxp_as_buffer_config_t asBufferConfig = {
        .pixelFormat = pxp_get_as_px_format(src_cf),
        .bufferAddr = (uint32_t)(src_buf + src_stride * src_area->y1 + src_px_size * src_area->x1),
        .pitchBytes = src_stride
    };
    PXP_SetAlphaSurfaceBufferConfig(PXP_ID, &asBufferConfig);
    PXP_SetAlphaSurfacePosition(PXP_ID, 0U, 0U, src_w - 1U, src_h - 1U);

    /*Disable PS, use as color generator*/
    PXP_SetProcessSurfacePosition(PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);
    PXP_SetProcessSurfaceBackGroundColor(PXP_ID, lv_color_to_u32(dsc->recolor));

    /*Output buffer*/
    pxp_output_buffer_config_t outputBufferConfig = {
        .pixelFormat = pxp_get_out_px_format(dest_cf),
        .interlacedMode = kPXP_OutputProgressive,
        .buffer0Addr = (uint32_t)(dest_buf + dest_stride * dest_area->y1 + dest_px_size * dest_area->x1),
        .buffer1Addr = (uint32_t)0U,
        .pitchBytes = dest_stride,
        .width = dest_w,
        .height = dest_h
    };
    PXP_SetOutputBufferConfig(PXP_ID, &outputBufferConfig);

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
        .srcColorMode = kPXP_PorterDuffColorWithAlpha,
        .dstGlobalAlphaMode = kPXP_PorterDuffGlobalAlpha,
        .srcGlobalAlphaMode = src_has_alpha ? kPXP_PorterDuffLocalAlpha : kPXP_PorterDuffGlobalAlpha,
        .dstFactorMode = kPXP_PorterDuffFactorStraight,
        .srcFactorMode = kPXP_PorterDuffFactorInversed,
        .dstGlobalAlpha = dsc->recolor_opa,
        .srcGlobalAlpha = 0xff,
        .dstAlphaMode = kPXP_PorterDuffAlphaStraight, /*don't care*/
        .srcAlphaMode = kPXP_PorterDuffAlphaStraight
    };
    PXP_SetPorterDuffConfig(PXP_ID, &pdConfig);

    lv_pxp_run();
}

static void _pxp_blit_transform(uint8_t * dest_buf, const lv_area_t * dest_area, int32_t dest_stride,
                                lv_color_format_t dest_cf, const uint8_t * src_buf, const lv_area_t * src_area,
                                int32_t src_stride, lv_color_format_t src_cf, const lv_draw_image_dsc_t * dsc)
{
    int32_t src_w = lv_area_get_width(src_area);
    int32_t src_h = lv_area_get_height(src_area);
    int32_t dest_w = lv_area_get_width(dest_area);
    int32_t dest_h = lv_area_get_height(dest_area);

    lv_point_t pivot = dsc->pivot;
    /*The offsets are now relative to the transformation result with pivot ULC*/
    int32_t piv_offset_x = 0;
    int32_t piv_offset_y = 0;

    int32_t trim_x = 0;
    int32_t trim_y = 0;

    bool has_rotation = (dsc->rotation != 0);
    bool has_scale = (dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE);
    uint8_t src_px_size = lv_color_format_get_size(src_cf);
    uint8_t dest_px_size = lv_color_format_get_size(dest_cf);

    lv_pxp_reset();

    if(has_rotation) {
        /*Convert rotation angle and calculate offsets caused by pivot*/
        pxp_rotate_degree_t pxp_angle;
        switch(dsc->rotation) {
            case 0:
                pxp_angle = kPXP_Rotate0;
                piv_offset_x = 0;
                piv_offset_y = 0;
                break;
            case 900:
                pxp_angle = kPXP_Rotate90;
                piv_offset_x = pivot.x + pivot.y - src_h;
                piv_offset_y = pivot.y - pivot.x;
                break;
            case 1800:
                pxp_angle = kPXP_Rotate180;
                piv_offset_x = 2 * pivot.x - src_w;
                piv_offset_y = 2 * pivot.y - src_h;
                break;
            case 2700:
                pxp_angle = kPXP_Rotate270;
                piv_offset_x = pivot.x - pivot.y;
                piv_offset_y = pivot.x + pivot.y - src_w;
                break;
            default:
                pxp_angle = kPXP_Rotate0;
                piv_offset_x = 0;
                piv_offset_y = 0;
        }
        /*PS buffer rotation and decimation does not function at the same time*/
        PXP_SetRotateConfig(PXP_ID, kPXP_RotateOutputBuffer, pxp_angle, kPXP_FlipDisable);
    }

    if(has_scale) {
        float fp_scale_x = (float)dsc->scale_x / LV_SCALE_NONE;
        float fp_scale_y = (float)dsc->scale_y / LV_SCALE_NONE;
        int32_t int_scale_x = (int32_t)fp_scale_x;
        int32_t int_scale_y = (int32_t)fp_scale_y;

        /*Any scale_factor in (k, k + 1] will result in a trim equal to k*/
        trim_x = (fp_scale_x == int_scale_x) ? int_scale_x - 1 : int_scale_x;
        trim_y = (fp_scale_y == int_scale_y) ? int_scale_y - 1 : int_scale_y;

        dest_w = src_w * fp_scale_x + trim_x;
        dest_h = src_h * fp_scale_y + trim_y;

        /*Final pivot offset = scale_factor * rotation_pivot_offset + scaling_pivot_offset*/
        piv_offset_x = floorf(fp_scale_x * piv_offset_x) - floorf((fp_scale_x - 1) * pivot.x);
        piv_offset_y = floorf(fp_scale_y * piv_offset_y) - floorf((fp_scale_y - 1) * pivot.y);
    }

    /*PS buffer - source image*/
    pxp_ps_buffer_config_t psBufferConfig = {
        .pixelFormat = pxp_get_ps_px_format(src_cf),
        .swapByte = false,
        .bufferAddr = (uint32_t)(src_buf + src_stride * src_area->y1 + src_px_size * src_area->x1),
        .bufferAddrU = 0,
        .bufferAddrV = 0,
        .pitchBytes = src_stride
    };
    PXP_SetProcessSurfaceBufferConfig(PXP_ID, &psBufferConfig);
    PXP_SetProcessSurfacePosition(PXP_ID, 0U, 0U, dest_w - trim_x - 1U, dest_h - trim_y - 1U);

    if(has_scale)
        PXP_SetProcessSurfaceScaler(PXP_ID, src_w, src_h, dest_w, dest_h);

    /*AS disabled */
    PXP_SetAlphaSurfacePosition(PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);

    /*Output buffer*/
    pxp_output_buffer_config_t outputBufferConfig = {
        .pixelFormat = pxp_get_out_px_format(dest_cf),
        .interlacedMode = kPXP_OutputProgressive,
        .buffer0Addr = (uint32_t)(dest_buf + dest_stride * (dest_area->y1 + piv_offset_y) + dest_px_size * (dest_area->x1 + piv_offset_x)),
        .buffer1Addr = (uint32_t)0U,
        .pitchBytes = dest_stride,
        .width = dest_w - trim_x,
        .height = dest_h - trim_y
    };
    PXP_SetOutputBufferConfig(PXP_ID, &outputBufferConfig);

    lv_pxp_run();
}

static void _pxp_blit(uint8_t * dest_buf, const lv_area_t * dest_area, int32_t dest_stride,
                      lv_color_format_t dest_cf, const uint8_t * src_buf, const lv_area_t * src_area,
                      int32_t src_stride, lv_color_format_t src_cf, lv_opa_t opa)
{
    int32_t dest_w = lv_area_get_width(dest_area);
    int32_t dest_h = lv_area_get_height(dest_area);
    int32_t src_w = lv_area_get_width(src_area);
    int32_t src_h = lv_area_get_height(src_area);

    bool src_has_alpha = (src_cf == LV_COLOR_FORMAT_ARGB8888);
    uint8_t src_px_size = lv_color_format_get_size(src_cf);
    uint8_t dest_px_size = lv_color_format_get_size(dest_cf);

    lv_pxp_reset();

    pxp_as_blend_config_t asBlendConfig = {
        .alpha = opa,
        .invertAlpha = false,
        .alphaMode = kPXP_AlphaRop,
        .ropMode = kPXP_RopMergeAs
    };

    if(opa >= (lv_opa_t)LV_OPA_MAX && !src_has_alpha) {
        /*Simple blit, no effect - Disable PS buffer*/
        PXP_SetProcessSurfacePosition(PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);
    }
    else {
        /*PS must be enabled to fetch background pixels.
          PS and OUT buffers are the same, blend will be done in-place*/
        pxp_ps_buffer_config_t psBufferConfig = {
            .pixelFormat = pxp_get_ps_px_format(dest_cf),
            .swapByte = false,
            .bufferAddr = (uint32_t)(dest_buf + dest_stride * dest_area->y1 + dest_px_size * dest_area->x1),
            .bufferAddrU = 0U,
            .bufferAddrV = 0U,
            .pitchBytes = dest_stride
        };

        if(opa >= (lv_opa_t)LV_OPA_MAX)
            asBlendConfig.alphaMode = src_has_alpha ? kPXP_AlphaEmbedded : kPXP_AlphaOverride;
        else
            asBlendConfig.alphaMode = src_has_alpha ? kPXP_AlphaMultiply : kPXP_AlphaOverride;

        PXP_SetProcessSurfaceBufferConfig(PXP_ID, &psBufferConfig);
        PXP_SetProcessSurfacePosition(PXP_ID, 0U, 0U, dest_w - 1U, dest_h - 1U);
    }

    /*AS buffer - source image*/
    pxp_as_buffer_config_t asBufferConfig = {
        .pixelFormat = pxp_get_as_px_format(src_cf),
        .bufferAddr = (uint32_t)(src_buf + src_stride * src_area->y1 + src_px_size * src_area->x1),
        .pitchBytes = src_stride
    };
    PXP_SetAlphaSurfaceBufferConfig(PXP_ID, &asBufferConfig);
    PXP_SetAlphaSurfacePosition(PXP_ID, 0U, 0U, src_w - 1U, src_h - 1U);
    PXP_SetAlphaSurfaceBlendConfig(PXP_ID, &asBlendConfig);
    PXP_EnableAlphaSurfaceOverlayColorKey(PXP_ID, false);

    /*Output buffer.*/
    pxp_output_buffer_config_t outputBufferConfig = {
        .pixelFormat = pxp_get_out_px_format(dest_cf),
        .interlacedMode = kPXP_OutputProgressive,
        .buffer0Addr = (uint32_t)(dest_buf + dest_stride * dest_area->y1 + dest_px_size * dest_area->x1),
        .buffer1Addr = (uint32_t)0U,
        .pitchBytes = dest_stride,
        .width = dest_w,
        .height = dest_h
    };
    PXP_SetOutputBufferConfig(PXP_ID, &outputBufferConfig);

    lv_pxp_run();
}

#endif /*LV_USE_DRAW_PXP*/
#endif /*LV_USE_PXP*/
