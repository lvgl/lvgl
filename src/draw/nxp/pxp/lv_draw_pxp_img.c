/**
 * @file lv_draw_pxp_img.c
 *
 */

/**
 * Copyright 2020-2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */


/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_pxp.h"

#if LV_USE_DRAW_PXP
#include "lv_pxp_cfg.h"
#include "lv_pxp_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _pxp_blit(uint8_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                      lv_color_format_t dest_cf, const uint8_t * src_buf, const lv_area_t * src_area,
                      lv_coord_t src_stride, lv_color_format_t src_cf, lv_opa_t opa);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_pxp_img(lv_draw_unit_t * draw_unit, const lv_draw_img_dsc_t * dsc,
                     const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_layer_t * layer = draw_unit->target_layer;
    const lv_img_dsc_t * img_dsc = dsc->src;

    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_unit->clip_area);
    lv_area_move(&rel_clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, &rel_coords, &rel_clip_area))
        return; /*Fully clipped, nothing to do*/

    const uint8_t * src_buf = img_dsc->data;

    lv_area_t src_area;
    src_area.x1 = blend_area.x1 - (coords->x1 - layer->buf_area.x1);
    src_area.y1 = blend_area.y1 - (coords->y1 - layer->buf_area.y1);
    src_area.x2 = src_area.x1 + lv_area_get_width(coords) - 1;
    src_area.y2 = src_area.y1 + lv_area_get_height(coords) - 1;
    lv_coord_t src_stride = lv_area_get_width(coords);

    uint8_t * dest_buf = layer->buf;
    lv_coord_t dest_stride = lv_area_get_width(&layer->buf_area);
    lv_color_format_t dest_cf = layer->color_format;
    lv_color_format_t src_cf = img_dsc->header.cf;

    _pxp_blit(dest_buf, &blend_area, dest_stride, dest_cf, src_buf, &src_area, src_stride, src_cf,
              dsc->opa);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _pxp_blit(uint8_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                      lv_color_format_t dest_cf, const uint8_t * src_buf, const lv_area_t * src_area,
                      lv_coord_t src_stride, lv_color_format_t src_cf, lv_opa_t opa)
{
    lv_coord_t dest_w = lv_area_get_width(dest_area);
    lv_coord_t dest_h = lv_area_get_height(dest_area);
    lv_coord_t src_w = lv_area_get_width(src_area);
    lv_coord_t src_h = lv_area_get_height(src_area);

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
            .bufferAddr = (uint32_t)(dest_buf + dest_px_size * (dest_stride * dest_area->y1 + dest_area->x1)),
            .bufferAddrU = 0U,
            .bufferAddrV = 0U,
            .pitchBytes = dest_stride * dest_px_size
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
        .bufferAddr = (uint32_t)(src_buf + src_px_size * (src_stride * src_area->y1 + src_area->x1)),
        .pitchBytes = src_stride * src_px_size
    };
    PXP_SetAlphaSurfaceBufferConfig(PXP_ID, &asBufferConfig);
    PXP_SetAlphaSurfacePosition(PXP_ID, 0U, 0U, src_w - 1U, src_h - 1U);
    PXP_SetAlphaSurfaceBlendConfig(PXP_ID, &asBlendConfig);
    PXP_EnableAlphaSurfaceOverlayColorKey(PXP_ID, false);

    /*Output buffer.*/
    pxp_output_buffer_config_t outputBufferConfig = {
        .pixelFormat = pxp_get_out_px_format(dest_cf),
        .interlacedMode = kPXP_OutputProgressive,
        .buffer0Addr = (uint32_t)(dest_buf + dest_px_size * (dest_stride * dest_area->y1 + dest_area->x1)),
        .buffer1Addr = (uint32_t)0U,
        .pitchBytes = dest_stride * dest_px_size,
        .width = dest_w,
        .height = dest_h
    };
    PXP_SetOutputBufferConfig(PXP_ID, &outputBufferConfig);

    lv_pxp_run();
}

#endif /*LV_USE_DRAW_PXP*/
