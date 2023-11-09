/**
 * @file lv_draw_vglite_blend.c
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

#include "lv_draw_vglite.h"

#if LV_USE_DRAW_VGLITE
#include "lv_vglite_buf.h"
#include "lv_vglite_matrix.h"
#include "lv_vglite_utils.h"

#include "../../../misc/lv_log.h"

/*********************
 *      DEFINES
 *********************/

#if VGLITE_BLIT_SPLIT_ENABLED
/**
* BLIT split threshold - BLITs with width or height higher than this value will
* be done in multiple steps. Value must be multiple of stride alignment in px.
* For most color formats the alignment is 16px (except the index formats).
*/
#define VGLITE_BLIT_SPLIT_THR 352

/* Enable for logging debug traces. */
#define VGLITE_LOG_TRACE 0

#if VGLITE_LOG_TRACE
#define VGLITE_TRACE(fmt, ...)                \
    do {                                      \
        LV_LOG(fmt, ##__VA_ARGS__);           \
    } while (0)
#else
#define VGLITE_TRACE(fmt, ...)                \
    do {                                      \
    } while (0)
#endif
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * BLock Image Transfer - copy rectangular image from src_buf to dst_buf with effects.
 * By default, image is copied directly, with optional opacity.
 *
 * @param[in] dest_area Destination area with relative coordinates to dest buffer
 * @param[in] src_area Source area with relative coordinates to src buffer
 * @param[in] opa Opacity
 *
 */
static void _vglite_blit_single(const lv_area_t * dest_area, const lv_area_t * src_area, lv_opa_t opa);

#if VGLITE_BLIT_SPLIT_ENABLED
/**
 * Move buffer pointer as close as possible to area, but with respect to alignment requirements.
 *
 * @param[in] buf Buffer address pointer
 * @param[in] area Area with relative coordinates to the buffer
 * @param[in] stride Stride of buffer in bytes
 * @param[in] cf Color format of buffer
 */
static void _move_buf_close_to_area(void ** buf, lv_area_t * area, uint32_t stride, lv_color_format_t cf);

/**
 * BLock Image Transfer - copy rectangular image from src_buf to dst_buf with effects.
 * By default, image is copied directly, with optional opacity.
 *
 * @param dest_buf Destination buffer
 * @param[in] dest_area Destination area with relative coordinates to dest buffer
 * @param[in] dest_stride Stride of destination buffer in bytes
 * @param[in] dest_cf Color format of destination buffer
 * @param[in] src_buf Source buffer
 * @param[in] src_area Source area with relative coordinates to src buffer
 * @param[in] src_stride Stride of source buffer in bytes
 * @param[in] src_cf Color format of source buffer
 * @param[in] opa Opacity
 *
 */
static void _vglite_blit_split(void * dest_buf, lv_area_t * dest_area, uint32_t dest_stride, lv_color_format_t dest_cf,
                               const void * src_buf, lv_area_t * src_area, uint32_t src_stride, lv_color_format_t src_cf,
                               lv_opa_t opa);
#else
/**
 * BLock Image Transfer - copy rectangular image from src_buf to dst_buf with transformation.
 * By default, image is copied directly, with optional opacity.
 *
 * @param[in] dest_area Area with relative coordinates to dest buffer
 * @param[in] clip_area Clip area with relative coordinates to dest buffer
 * @param[in] src_area Source area with relative coordinates to src buffer
 * @param[in] dsc Image descriptor
 *
 */
static void _vglite_blit_transform(const lv_area_t * dest_area, const lv_area_t * clip_area,
                                   const lv_area_t * src_area, const lv_draw_image_dsc_t * dsc);
#endif /*VGLITE_BLIT_SPLIT_ENABLED*/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vglite_img(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * dsc,
                        const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_layer_t * layer = draw_unit->target_layer;
    const lv_image_dsc_t * img_dsc = dsc->src;

    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -layer->draw_buf_ofs.x, -layer->draw_buf_ofs.y);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_unit->clip_area);
    lv_area_move(&rel_clip_area, -layer->draw_buf_ofs.x, -layer->draw_buf_ofs.y);

    lv_area_t blend_area;
    bool has_transform = dsc->rotation != 0 || dsc->zoom != LV_SCALE_NONE;
    if(has_transform)
        lv_area_copy(&blend_area, &rel_coords);
    else if(!_lv_area_intersect(&blend_area, &rel_coords, &rel_clip_area))
        return; /*Fully clipped, nothing to do*/

    const void * src_buf = img_dsc->data;

    lv_area_t src_area;
    src_area.x1 = blend_area.x1 - (coords->x1 - layer->draw_buf_ofs.x);
    src_area.y1 = blend_area.y1 - (coords->y1 - layer->draw_buf_ofs.y);
    src_area.x2 = src_area.x1 + lv_area_get_width(coords) - 1;
    src_area.y2 = src_area.y1 + lv_area_get_height(coords) - 1;

    lv_color_format_t src_cf = img_dsc->header.cf;
    uint32_t src_stride = img_dsc->header.stride;

    /* Set src_vgbuf structure. */
    vglite_set_src_buf(src_buf, lv_area_get_width(&src_area), lv_area_get_height(&src_area), src_stride, src_cf);

#if VGLITE_BLIT_SPLIT_ENABLED
    void * dest_buf = lv_draw_buf_get_buf(&layer->draw_buf);
    uint32_t dest_stride = lv_draw_buf_get_stride(&layer->draw_buf);
    lv_color_format_t dest_cf = layer->draw_buf.color_format;

    if(!has_transform)
        _vglite_blit_split(dest_buf, &blend_area, dest_stride, dest_cf,
                           src_buf, &src_area, src_stride, src_cf, dsc->opa);
#else
    if(has_transform)
        _vglite_blit_transform(&blend_area, &rel_clip_area, &src_area, dsc);
    else
        _vglite_blit_single(&blend_area, &src_area, dsc->opa);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void _vglite_blit(const lv_area_t * src_area, lv_opa_t opa)
{
    vg_lite_error_t err = VG_LITE_SUCCESS;
    vg_lite_buffer_t * dst_vgbuf = vglite_get_dest_buf();
    vg_lite_buffer_t * src_vgbuf = vglite_get_src_buf();

    uint32_t rect[] = {
        (uint32_t)src_area->x1, /* start x */
        (uint32_t)src_area->y1, /* start y */
        (uint32_t)lv_area_get_width(src_area), /* width */
        (uint32_t)lv_area_get_height(src_area) /* height */
    };

    uint32_t color;
    vg_lite_blend_t blend;
    if(opa >= (lv_opa_t)LV_OPA_MAX) {
        color = 0xFFFFFFFFU;
        blend = VG_LITE_BLEND_SRC_OVER;
        src_vgbuf->transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
    }
    else {
        if(vg_lite_query_feature(gcFEATURE_BIT_VG_PE_PREMULTIPLY)) {
            color = (opa << 24) | 0x00FFFFFFU;
        }
        else {
            color = (opa << 24) | (opa << 16) | (opa << 8) | opa;
        }
        blend = VG_LITE_BLEND_SRC_OVER;
        src_vgbuf->image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
        src_vgbuf->transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
    }

    vg_lite_matrix_t * vgmatrix = vglite_get_matrix();

    err = vg_lite_blit_rect(dst_vgbuf, src_vgbuf, rect, vgmatrix, blend, color, VG_LITE_FILTER_POINT);
    LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Blit rectangle failed.");

    vglite_run();
}

static void _vglite_blit_single(const lv_area_t * dest_area, const lv_area_t * src_area, lv_opa_t opa)
{
    /* Set scissor. */
    vglite_set_scissor(dest_area);

    /* Set vgmatrix. */
    vglite_set_translation_matrix(dest_area);

    /* Start blit. */
    _vglite_blit(src_area, opa);

    /* Disable scissor. */
    vglite_disable_scissor();
}

#if VGLITE_BLIT_SPLIT_ENABLED
static void _move_buf_close_to_area(void ** buf, lv_area_t * area, uint32_t stride, lv_color_format_t cf)
{
    uint8_t ** buf_u8 = (uint8_t **)buf;
    uint8_t align_bytes = vglite_get_alignment(cf);
    uint8_t bits_per_pixel = vglite_get_px_size(cf);

    uint16_t align_pixels = align_bytes * 8 / bits_per_pixel;

    if(area->x1 >= (int32_t)(area->x1 % align_pixels)) {
        uint16_t shift_x = area->x1 - (area->x1 % align_pixels);

        area->x1 -= shift_x;
        area->x2 -= shift_x;
        *buf_u8 += (shift_x * bits_per_pixel) / 8;
    }

    if(area->y1) {
        uint16_t shift_y = area->y1;

        area->y1 -= shift_y;
        area->y2 -= shift_y;
        *buf_u8 += shift_y * stride;
    }
}

static void _vglite_blit_split(void * dest_buf, lv_area_t * dest_area, uint32_t dest_stride, lv_color_format_t dest_cf,
                               const void * src_buf, lv_area_t * src_area, uint32_t src_stride, lv_color_format_t src_cf,
                               lv_opa_t opa)
{
    VGLITE_TRACE("Blit "
                 "Area: ([%d,%d], [%d,%d]) -> ([%d,%d], [%d,%d]) | "
                 "Size: ([%dx%d] -> [%dx%d]) | "
                 "Addr: (0x%x -> 0x%x)",
                 src_area->x1, src_area->y1, src_area->x2, src_area->y2,
                 dest_area->x1, dest_area->y1, dest_area->x2, dest_area->y2,
                 lv_area_get_width(src_area), lv_area_get_height(src_area),
                 lv_area_get_width(dest_area), lv_area_get_height(dest_area),
                 (uintptr_t)src_buf, (uintptr_t)dest_buf);

    /* Move starting pointers as close as possible to [x1, y1], so coordinates are as small as possible */
    _move_buf_close_to_area((void **)&src_buf, src_area, src_stride, src_cf);
    _move_buf_close_to_area(&dest_buf, dest_area, dest_stride, dest_cf);

    /* If we're in limit, do a single BLIT */
    if((src_area->x2 < VGLITE_BLIT_SPLIT_THR) &&
       (src_area->y2 < VGLITE_BLIT_SPLIT_THR)) {

        /* Set new dest_vgbuf and src_vgbuf memory addresses */
        vglite_set_dest_buf_ptr(dest_buf);
        vglite_set_src_buf_ptr(src_buf);

        _vglite_blit_single(dest_area, src_area, opa);

        VGLITE_TRACE("Single "
                     "Area: ([%d,%d], [%d,%d]) -> ([%d,%d], [%d,%d]) | "
                     "Size: ([%dx%d] -> [%dx%d]) | "
                     "Addr: (0x%x -> 0x%x)",
                     src_area->x1, src_area->y1, src_area->x2, src_area->y2,
                     dest_area->x1, dest_area->y1, dest_area->x2, dest_area->y2,
                     lv_area_get_width(src_area), lv_area_get_height(src_area),
                     lv_area_get_width(dest_area), lv_area_get_height(dest_area),
                     (uintptr_t)src_buf, (uintptr_t)dest_buf);
    };

    /* Split the BLIT into multiple tiles */
    VGLITE_TRACE("Split "
                 "Area: ([%d,%d], [%d,%d]) -> ([%d,%d], [%d,%d]) | "
                 "Size: ([%dx%d] -> [%dx%d]) | "
                 "Addr: (0x%x -> 0x%x)",
                 src_area->x1, src_area->y1, src_area->x2, src_area->y2,
                 dest_area->x1, dest_area->y1, dest_area->x2, dest_area->y2,
                 lv_area_get_width(src_area), lv_area_get_height(src_area),
                 lv_area_get_width(dest_area), lv_area_get_height(dest_area),
                 (uintptr_t)src_buf, (uintptr_t)dest_buf);

    int32_t width = LV_MIN(lv_area_get_width(src_area), lv_area_get_width(dest_area));
    int32_t height = LV_MIN(lv_area_get_height(src_area), lv_area_get_height(dest_area));

    /* Number of tiles needed */
    uint8_t total_tiles_x = (src_area->x1 + width + VGLITE_BLIT_SPLIT_THR - 1) /
                            VGLITE_BLIT_SPLIT_THR;
    uint8_t total_tiles_y = (src_area->y1 + height + VGLITE_BLIT_SPLIT_THR - 1) /
                            VGLITE_BLIT_SPLIT_THR;

    uint16_t shift_src_x = src_area->x1;
    uint16_t shift_dest_x = dest_area->x1;

    VGLITE_TRACE("X shift: src: %d, dst: %d", shift_src_x, shift_dest_x);

    uint8_t * tile_dest_buf;
    lv_area_t tile_dest_area;
    const uint8_t * tile_src_buf;
    lv_area_t tile_src_area;

    for(uint8_t y = 0; y < total_tiles_y; y++) {
        /* y1 always start from 0 */
        tile_src_area.y1 = 0;

        /* Calculate y2 coordinates */
        if(y < total_tiles_y - 1)
            tile_src_area.y2 = VGLITE_BLIT_SPLIT_THR - 1;
        else
            tile_src_area.y2 = height - y * VGLITE_BLIT_SPLIT_THR - 1;

        /* No vertical shift, dest y is always in sync with src y */
        tile_dest_area.y1 = tile_src_area.y1;
        tile_dest_area.y2 = tile_src_area.y2;

        /* Advance start pointer for every tile, except the first column (y = 0) */
        tile_src_buf = (uint8_t *)src_buf + y * VGLITE_BLIT_SPLIT_THR * src_stride;
        tile_dest_buf = (uint8_t *)dest_buf + y * VGLITE_BLIT_SPLIT_THR * dest_stride;

        for(uint8_t x = 0; x < total_tiles_x; x++) {
            /* x1 always start from the same shift */
            tile_src_area.x1 = shift_src_x;
            tile_dest_area.x1 = shift_dest_x;
            if(x > 0) {
                /* Advance start pointer for every tile, except the first raw (x = 0) */
                tile_src_buf += VGLITE_BLIT_SPLIT_THR * vglite_get_px_size(src_cf) / 8;
                tile_dest_buf += VGLITE_BLIT_SPLIT_THR * vglite_get_px_size(dest_cf) / 8;
            }

            /* Calculate x2 coordinates */
            if(x < total_tiles_x - 1)
                tile_src_area.x2 = VGLITE_BLIT_SPLIT_THR - 1;
            else
                tile_src_area.x2 = width - x * VGLITE_BLIT_SPLIT_THR - 1;

            tile_dest_area.x2 = tile_src_area.x2;

            /* Shift x2 coordinates */
            tile_src_area.x2 += shift_src_x;
            tile_dest_area.x2 += shift_dest_x;

            /* Set new dest_vgbuf and src_vgbuf memory addresses */
            vglite_set_dest_buf_ptr(tile_dest_buf);
            vglite_set_src_buf_ptr(tile_src_buf);

            _vglite_blit_single(&tile_dest_area, &tile_src_area, opa);

            VGLITE_TRACE("Tile [%d, %d] "
                         "Area: ([%d,%d], [%d,%d]) -> ([%d,%d], [%d,%d]) | "
                         "Size: ([%dx%d] -> [%dx%d]) | "
                         "Addr: (0x%x -> 0x%x)",
                         x, y,
                         tile_src_area.x1, tile_src_area.y1, tile_src_area.x2, tile_src_area.y2,
                         tile_dest_area.x1, tile_dest_area.y1, tile_dest_area.x2, tile_dest_area.y2,
                         lv_area_get_width(&tile_src_area), lv_area_get_height(&tile_src_area),
                         lv_area_get_width(&tile_dest_area), lv_area_get_height(&tile_dest_area),
                         (uintptr_t)tile_src_buf, (uintptr_t)tile_dest_buf);
        }
    }
}
#else
static void _vglite_blit_transform(const lv_area_t * dest_area, const lv_area_t * clip_area,
                                   const lv_area_t * src_area, const lv_draw_image_dsc_t * dsc)
{
    /* Set scissor. */
    vglite_set_scissor(clip_area);

    /* Set vgmatrix. */
    vglite_set_transformation_matrix(dest_area, dsc);

    /* Start blit. */
    _vglite_blit(src_area, dsc->opa);

    /* Disable scissor. */
    vglite_disable_scissor();
}
#endif /*VGLITE_BLIT_SPLIT_ENABLED*/

#endif /*LV_USE_DRAW_VGLITE*/
