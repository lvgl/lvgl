/**
 * @file lv_draw_pxp.c
 *
 */

/**
 * Copyright 2022-2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_pxp.h"
#include "../../sw/lv_draw_sw_private.h"

#if LV_USE_DRAW_PXP
#include "lv_pxp_cfg.h"
#include "lv_pxp_utils.h"
#include "../../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/*
 * Tell whether PXP can draw a descriptor on the layer.
 */
static bool _pxp_can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc);
static bool _pxp_can_draw_img(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
static bool _pxp_can_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);

static void _pxp_draw_begin(lv_layer_t * layer, const lv_area_t * clip_area);
#if LV_USE_PARALLEL_DRAW_DEBUG
static void _pxp_draw_debug(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_area_t * coords);
#endif
static void _pxp_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);
static void _pxp_draw_img(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);
static void _pxp_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_draw_unit_draw_cbs_t draw_cbs = {
    .draw_fill_cb = _pxp_draw_fill,
    .can_draw_fill_cb = _pxp_can_draw_fill,
    .draw_image_cb = _pxp_draw_img,
    .can_draw_image_cb = _pxp_can_draw_img,
    .draw_layer_cb = _pxp_draw_layer,
    .can_draw_layer_cb = _pxp_can_draw_layer,
};

#if LV_USE_PARALLEL_DRAW_DEBUG
    #define _draw_info LV_GLOBAL_DEFAULT()->draw_info
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_pxp_init(void)
{
    lv_pxp_init();

#if LV_USE_DRAW_PXP
    lv_draw_buf_pxp_init_handlers();

    lv_draw_pxp_unit_t * draw_pxp_unit = lv_draw_create_unit(sizeof(lv_draw_pxp_unit_t));
    draw_pxp_unit->base_unit.name = "NXP_PXP";
    draw_pxp_unit->base_unit.draw_cbs = &draw_cbs;
    draw_pxp_unit->base_unit.fallback = lv_draw_sw_get_draw_unit();
#endif /*LV_USE_DRAW_PXP*/
}

void lv_draw_pxp_deinit(void)
{
    lv_pxp_deinit();
}

void lv_draw_pxp_rotate(const void * src_buf, void * dest_buf, int32_t src_width, int32_t src_height,
                        int32_t src_stride, int32_t dest_stride, lv_rotation_t rotation,
                        lv_color_format_t cf)
{
    lv_pxp_reset();

    /* Convert rotation angle
     * To be in sync with CPU, the received angle is counterclockwise
     * and the PXP constants are for clockwise rotation
     *
     *    counterclockwise          clockwise
     * LV_ROTATION_90  -> kPXP_Rotate270
     * LV_ROTATION_270 -> kPXP_Rotate90
     */
    pxp_rotate_degree_t pxp_rotation;
    switch(rotation) {
        case LV_ROTATION_0:
            pxp_rotation = kPXP_Rotate0;
            break;
        case LV_ROTATION_270:
            pxp_rotation = kPXP_Rotate90;
            break;
        case LV_ROTATION_180:
            pxp_rotation = kPXP_Rotate180;
            break;
        case LV_ROTATION_90:
            pxp_rotation = kPXP_Rotate270;
            break;
        default:
            pxp_rotation = kPXP_Rotate0;
            break;
    }
    PXP_SetRotateConfig(PXP_ID, kPXP_RotateOutputBuffer, pxp_rotation, kPXP_FlipDisable);

    /*Simple blit, no effect - Disable PS buffer*/
    PXP_SetProcessSurfacePosition(PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);

    /*AS buffer - source image*/
    pxp_as_buffer_config_t asBufferConfig = {
        .pixelFormat = pxp_get_as_px_format(cf),
        .bufferAddr = (uint32_t)src_buf,
        .pitchBytes = src_stride
    };
    PXP_SetAlphaSurfaceBufferConfig(PXP_ID, &asBufferConfig);
    PXP_SetAlphaSurfacePosition(PXP_ID, 0U, 0U, src_width - 1U, src_height - 1U);
    PXP_EnableAlphaSurfaceOverlayColorKey(PXP_ID, false);

    /*Output buffer.*/
    pxp_output_buffer_config_t outputBufferConfig = {
        .pixelFormat = pxp_get_out_px_format(cf),
        .interlacedMode = kPXP_OutputProgressive,
        .buffer0Addr = (uint32_t)dest_buf,
        .buffer1Addr = (uint32_t)0U,
        .pitchBytes = dest_stride,
        .width = src_width,
        .height = src_height
    };
    PXP_SetOutputBufferConfig(PXP_ID, &outputBufferConfig);

    lv_pxp_run();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static inline bool _pxp_src_cf_supported(lv_color_format_t cf)
{
    bool is_cf_supported = false;

    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            is_cf_supported = true;
            break;
        default:
            break;
    }

    return is_cf_supported;
}

static inline bool _pxp_dest_cf_supported(lv_color_format_t cf)
{
    bool is_cf_supported = false;

    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            is_cf_supported = true;
            break;
        default:
            break;
    }

    return is_cf_supported;
}

static bool _pxp_draw_img_supported(const lv_draw_image_dsc_t * draw_dsc)
{
    const lv_image_dsc_t * img_dsc = draw_dsc->src;

    bool has_recolor = (draw_dsc->recolor_opa > LV_OPA_MIN);
    bool has_transform = (draw_dsc->rotation != 0 || draw_dsc->scale_x != LV_SCALE_NONE ||
                          draw_dsc->scale_y != LV_SCALE_NONE);

    /* Recolor and transformation are not supported at the same time. */
    if(has_recolor && has_transform)
        return false;

    bool has_opa = (draw_dsc->opa < (lv_opa_t)LV_OPA_MAX);
    bool src_has_alpha = (img_dsc->header.cf == LV_COLOR_FORMAT_ARGB8888);

    /*
     * Recolor or transformation for images w/ opa or alpha channel can't
     * be obtained in a single PXP configuration. Two steps are required.
     */
    if((has_recolor || has_transform) && (has_opa || src_has_alpha))
        return false;

    /* PXP can only rotate at 90x angles. */
    if(draw_dsc->rotation % 900)
        return false;

    /*
     * PXP is set to process 16x16 blocks to optimize the system for memory
     * bandwidth and image processing time.
     * The output engine essentially truncates any output pixels after the
     * desired number of pixels has been written.
     * When rotating a source image and the output is not divisible by the block
     * size, the incorrect pixels could be truncated and the final output image
     * can look shifted.
     *
     * No combination of rotate with flip, scaling or decimation is possible
     * if buffer is unaligned.
     */
    if(has_transform && (img_dsc->header.w % 16 || img_dsc->header.h % 16))
        return false;

    return true;
}

static bool _pxp_can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);

    if(!_pxp_dest_cf_supported(layer->color_format))
        return false;

    /* Most simple case: just a plain rectangle (no radius, no gradient). */
    return (dsc->radius == 0) && (dsc->grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_NONE);
}

static bool _pxp_can_draw_img(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);

    if(!_pxp_dest_cf_supported(layer->color_format))
        return false;

    const lv_image_dsc_t * img_dsc = dsc->src;

    if(img_dsc->header.cf >= LV_COLOR_FORMAT_PROPRIETARY_START)
        return false;

    if((!_pxp_src_cf_supported(img_dsc->header.cf)) ||
       (!pxp_buf_aligned(img_dsc->data, img_dsc->header.stride)))
        return false;

    return _pxp_draw_img_supported(dsc);
}

static bool _pxp_can_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);

    if(!_pxp_dest_cf_supported(layer->color_format))
        return false;

    const lv_layer_t * layer_to_draw = dsc->src;

    if(!_pxp_src_cf_supported(layer_to_draw->color_format))
        return false;

    return _pxp_draw_img_supported(dsc);
}

static void _pxp_draw_begin(lv_layer_t * layer, const lv_area_t * clip_area)
{
    /* Invalidate only the drawing area, relative to the buffer */
    lv_area_t draw_area = *clip_area;
    lv_area_move(&draw_area, -layer->buf_area.x1, -layer->buf_area.y1);
    lv_draw_buf_invalidate_cache(layer->draw_buf, &draw_area);
}

static void _pxp_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    _pxp_draw_begin(layer, clip_area);
    lv_draw_pxp_fill(draw_unit, layer, clip_area, dsc, coords);
#if LV_USE_PARALLEL_DRAW_DEBUG
    _pxp_draw_debug(draw_unit, layer, clip_area, coords);
#endif
}

static void _pxp_draw_img(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    _pxp_draw_begin(layer, clip_area);
    lv_draw_pxp_img(draw_unit, layer, clip_area, dsc, coords);
#if LV_USE_PARALLEL_DRAW_DEBUG
    _pxp_draw_debug(draw_unit, layer, clip_area, coords);
#endif
}

static void _pxp_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    _pxp_draw_begin(layer, clip_area);
    lv_draw_pxp_layer(draw_unit, layer, clip_area, dsc, coords);
}

#if LV_USE_PARALLEL_DRAW_DEBUG
/*Tint the drawn area with the unit's color and write its index. Layers manage it for themselves.*/
static void _pxp_draw_debug(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_area_t * coords)
{
    lv_area_t draw_area;
    if(!lv_area_intersect(&draw_area, coords, clip_area))
        return;

    int32_t idx = draw_unit->idx;

    lv_draw_fill_dsc_t fill_dsc;
    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.color = lv_palette_main(idx % LV_PALETTE_LAST);
    fill_dsc.opa = LV_OPA_10;
    lv_draw_sw_fill(layer, clip_area, &fill_dsc, &draw_area);

    lv_draw_border_dsc_t border_dsc;
    lv_draw_border_dsc_init(&border_dsc);
    border_dsc.color = fill_dsc.color;
    border_dsc.opa = LV_OPA_80;
    border_dsc.width = 1;
    lv_draw_sw_border(layer, clip_area, &border_dsc, &draw_area);

    lv_point_t txt_size;
    lv_text_get_size(&txt_size, "W", LV_FONT_DEFAULT, 0, 0, 100, LV_TEXT_FLAG_NONE);

    lv_area_t txt_area;
    txt_area.x1 = draw_area.x1;
    txt_area.y1 = draw_area.y1;
    txt_area.x2 = draw_area.x1 + txt_size.x - 1;
    txt_area.y2 = draw_area.y1 + txt_size.y - 1;

    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.color = lv_color_white();
    lv_draw_sw_fill(layer, clip_area, &fill_dsc, &txt_area);

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", idx);
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_black();
    label_dsc.text = buf;
    lv_draw_sw_label(layer, clip_area, &label_dsc, &txt_area);
}
#endif
#endif /*LV_USE_DRAW_PXP*/
