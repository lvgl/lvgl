/**
 * @file lv_draw_pxp_fill.c
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

static void _pxp_fill(uint8_t * dest_buf, const lv_area_t * dest_area, int32_t dest_stride,
                      lv_color_format_t dest_cf, const lv_draw_fill_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_pxp_fill(lv_draw_unit_t * draw_unit, const lv_draw_fill_dsc_t * dsc,
                      const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_layer_t * layer = draw_unit->target_layer;
    lv_draw_buf_t * draw_buf = layer->draw_buf;

    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_unit->clip_area);
    lv_area_move(&rel_clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t blend_area;
    if(!lv_area_intersect(&blend_area, &rel_coords, &rel_clip_area))
        return; /*Fully clipped, nothing to do*/

    _pxp_fill(draw_buf->data, &blend_area, draw_buf->header.stride, draw_buf->header.cf, dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _pxp_fill(uint8_t * dest_buf, const lv_area_t * dest_area, int32_t dest_stride,
                      lv_color_format_t dest_cf, const lv_draw_fill_dsc_t * dsc)
{
    int32_t dest_w = lv_area_get_width(dest_area);
    int32_t dest_h = lv_area_get_height(dest_area);

    lv_pxp_reset();

    uint8_t px_size = lv_color_format_get_size(dest_cf);

    /*OUT buffer configure*/
    pxp_output_buffer_config_t outputConfig = {
        .pixelFormat = pxp_get_out_px_format(dest_cf),
        .interlacedMode = kPXP_OutputProgressive,
        .buffer0Addr = (uint32_t)(dest_buf + dest_stride * dest_area->y1 + px_size * dest_area->x1),
        .buffer1Addr = (uint32_t)NULL,
        .pitchBytes = dest_stride,
        .width = dest_w,
        .height = dest_h
    };

    PXP_SetOutputBufferConfig(PXP_ID, &outputConfig);

    if(dsc->opa >= (lv_opa_t)LV_OPA_MAX) {
        /*Simple color fill without opacity - AS disabled*/
        PXP_SetAlphaSurfacePosition(PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);

    }
    else {
        /*Fill with opacity - AS used as source (same as OUT)*/
        pxp_as_buffer_config_t asBufferConfig = {
            .pixelFormat = pxp_get_as_px_format(dest_cf),
            .bufferAddr = outputConfig.buffer0Addr,
            .pitchBytes = outputConfig.pitchBytes
        };

        PXP_SetAlphaSurfaceBufferConfig(PXP_ID, &asBufferConfig);
        PXP_SetAlphaSurfacePosition(PXP_ID, 0U, 0U, dest_w - 1U, dest_h - 1U);
    }

    /*Disable PS, use as color generator*/
    PXP_SetProcessSurfacePosition(PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);
    PXP_SetProcessSurfaceBackGroundColor(PXP_ID, lv_color_to_u32(dsc->color));

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
        .srcFactorMode = (dsc->opa >= (lv_opa_t)LV_OPA_MAX) ? kPXP_PorterDuffFactorStraight : kPXP_PorterDuffFactorInversed,
        .dstGlobalAlpha = dsc->opa,
        .srcGlobalAlpha = dsc->opa,
        .dstAlphaMode = kPXP_PorterDuffAlphaStraight, /*don't care*/
        .srcAlphaMode = kPXP_PorterDuffAlphaStraight  /*don't care*/
    };

    PXP_SetPorterDuffConfig(PXP_ID, &pdConfig);

    lv_pxp_run();
}

#endif /*LV_USE_DRAW_PXP*/
