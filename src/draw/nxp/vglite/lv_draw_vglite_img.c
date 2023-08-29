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
* BLIT split threshold - BLITs with width or height higher than this value will be done
* in multiple steps. Value must be 16-aligned. Don't change.
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

#if VGLITE_BLIT_SPLIT_ENABLED
/**
 * BLock Image Transfer - copy rectangular image from src_buf to dst_buf with effects.
 * By default, image is copied directly, with optional opacity.
 *
 * @param dest_buf Destination buffer
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] dest_stride Stride of destination buffer in bytes
 * @param[in] src_buf Source buffer
 * @param[in] src_area Source area with relative coordinates of source buffer
 * @param[in] src_stride Stride of source buffer in bytes
 * @param[in] opa Opacity
 *
 */
static void _vglite_blit_split(void * dest_buf, lv_area_t * dest_area, uint32_t dest_stride,
                               const void * src_buf, lv_area_t * src_area, uint32_t src_stride,
                               lv_opa_t opa);
#else
/**
 * BLock Image Transfer - copy rectangular image from src_buf to dst_buf with effects.
 * By default, image is copied directly, with optional opacity.
 *
 * @param[in] dest_area Destination area
 * @param[in] src_area Source area with relative coordinates of source buffer
 * @param[in] opa Opacity
 *
 */
static void _vglite_blit(const lv_area_t * dest_area, const lv_area_t * src_area, lv_opa_t opa);

/**
 * BLock Image Transfer - copy rectangular image from src_buf to dst_buf with transformation.
 * By default, image is copied directly, with optional opacity.
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] clip_area Clip area with relative coordinates of destination buffer
 * @param[in] src_area Source area with relative coordinates of source buffer
 * @param[in] dsc Image descriptor
 *
 */
static void _vglite_blit_transform(const lv_area_t * dest_area, const lv_area_t * clip_area,
                                   const lv_area_t * src_area, const lv_draw_img_dsc_t * dsc);
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

void lv_draw_vglite_img(lv_draw_unit_t * draw_unit, const lv_draw_img_dsc_t * dsc,
                        const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_layer_t * layer = draw_unit->target_layer;
    const lv_img_dsc_t * img_dsc = dsc->src;

    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -layer->draw_buf_ofs.x, -layer->draw_buf_ofs.y);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_unit->clip_area);
    lv_area_move(&rel_clip_area, -layer->draw_buf_ofs.x, -layer->draw_buf_ofs.y);

    lv_area_t blend_area;
    bool has_transform = dsc->angle != 0 || dsc->zoom != LV_ZOOM_NONE;
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

    lv_color_format_t cf = img_dsc->header.cf;
    uint32_t src_stride = lv_draw_buf_width_to_stride(lv_area_get_width(coords), cf);

    /* Set src_vgbuf structure. */
    vglite_set_src_buf(src_buf, lv_area_get_width(&src_area), lv_area_get_height(&src_area), src_stride, cf);

#if VGLITE_BLIT_SPLIT_ENABLED
    void * dest_buf = lv_draw_buf_get_buf(&layer->draw_buf);
    uint32_t dest_stride = lv_draw_buf_get_stride(&layer->draw_buf);

    if(!has_transform)
        _vglite_blit_split(dest_buf, &blend_area, dest_stride,
                           src_buf, &src_area, src_stride, dsc->opa);
#else
    if(has_transform)
        _vglite_blit_transform(&blend_area, &rel_clip_area, &src_area, dsc);
    else
        _vglite_blit(&blend_area, &src_area, dsc->opa);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Blit single image, with optional opacity.
 *
 */
static void _vglite_blit_single(const lv_area_t * src_area, lv_opa_t opa)
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

#if VGLITE_BLIT_SPLIT_ENABLED
/**
 * Move buffer pointer as close as possible to area, but with respect to alignment requirements. X-axis only.
 *
 */
static void _align_x(lv_area_t * area, uint8_t ** buf)
{
    int alignedAreaStartPx = area->x1 - (area->x1 % (LV_DRAW_BUF_ALIGN / sizeof(lv_color_t))); // FIXME
    LV_ASSERT_MSG(alignedAreaStartPx < 0, "Negative X alignment.");

    area->x1 -= alignedAreaStartPx;
    area->x2 -= alignedAreaStartPx;
    *buf += alignedAreaStartPx;
}

/**
 * Move buffer pointer to the area start and update variables, Y-axis only.
 *
 */
static void _align_y(lv_area_t * area, uint8_t ** buf, uint32_t stride) // FIXME
{
    int LineToAlignMem;
    int alignedAreaStartPy;
    /* find how many lines of pixels will respect memory alignment requirement */
    if((stride % (uint32_t)LV_DRAW_BUF_ALIGN) == 0x0U) {
        alignedAreaStartPy = area->y1;
    }
    else {
        LineToAlignMem = LV_DRAW_BUF_ALIGN / LV_DRAW_BUF_STRIDE_ALIGN;
        LV_ASSERT_MSG(LV_DRAW_BUF_ALIGN % LV_DRAW_BUF_STRIDE_ALIGN,
                      "Complex case: need gcd function.");
        alignedAreaStartPy = area->y1 - (area->y1 % LineToAlignMem);
        LV_ASSERT_MSG(alignedAreaStartPy < 0, "Negative Y alignment.");
    }

    area->y1 -= alignedAreaStartPy;
    area->y2 -= alignedAreaStartPy;
    *buf += (uint32_t)(alignedAreaStartPy * stride);
}

static void _vglite_blit_split(void * dest_buf, lv_area_t * dest_area, uint32_t dest_stride,
                               const void * src_buf, lv_area_t * src_area, uint32_t src_stride,
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

    /* Stage 1: Move starting pointers as close as possible to [x1, y1], so coordinates are as small as possible. */
    _align_x(src_area, (uint8_t **)&src_buf);
    _align_y(src_area, (uint8_t **)&src_buf, src_stride);
    _align_x(dest_area, (uint8_t **)&dest_buf);
    _align_y(dest_area, (uint8_t **)&dest_buf, dest_stride);

    /* Stage 2: If we're in limit, do a single BLIT */
    if((src_area->x2 < VGLITE_BLIT_SPLIT_THR) &&
       (src_area->y2 < VGLITE_BLIT_SPLIT_THR)) {
        /* Set new dest_vgbuf and src_vgbuf memory addresses. */
        vglite_set_dest_buf_ptr(dest_buf);
        vglite_set_src_buf_ptr(src_buf);

        /* Set scissor */
        vglite_set_scissor(dest_area);

        /* Set vgmatrix. */
        vglite_set_translation_matrix(dest_area);

        /* Start blit. */
        _vglite_blit_single(src_area, opa);

        /* Disable scissor. */
        vglite_disable_scissor();

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

    /* Stage 3: Split the BLIT into multiple tiles */
    VGLITE_TRACE("Split "
                 "Area: ([%d,%d], [%d,%d]) -> ([%d,%d], [%d,%d]) | "
                 "Size: ([%dx%d] -> [%dx%d]) | "
                 "Addr: (0x%x -> 0x%x)",
                 src_area->x1, src_area->y1, src_area->x2, src_area->y2,
                 dest_area->x1, dest_area->y1, dest_area->x2, dest_area->y2,
                 lv_area_get_width(src_area), lv_area_get_height(src_area),
                 lv_area_get_width(dest_area), lv_area_get_height(dest_area),
                 (uintptr_t)src_buf, (uintptr_t)dest_buf);


    lv_coord_t width = lv_area_get_width(src_area);
    lv_coord_t height = lv_area_get_height(src_area);

    /* Number of tiles needed */
    int total_tiles_x = (src_area->x1 + width + VGLITE_BLIT_SPLIT_THR - 1) /
                        VGLITE_BLIT_SPLIT_THR;
    int total_tiles_y = (src_area->y1 + height + VGLITE_BLIT_SPLIT_THR - 1) /
                        VGLITE_BLIT_SPLIT_THR;

    /* src and dst buffer shift against each other. Src buffer real data [0,0] may start actually at [3,0] in buffer, as
     * the buffer pointer has to be aligned, while dst buffer real data [0,0] may start at [1,0] in buffer. alignment may be
     * different */
    int shift_src_x = (src_area->x1 > dest_area->x1) ? (src_area->x1 - dest_area->x1) : 0;
    int shift_dest_x = (src_area->x1 < dest_area->x1) ? (dest_area->x1 - src_area->x1) : 0;

    VGLITE_TRACE("X shift: src: %d, dst: %d", shift_src_x, shift_dest_x);

    uint8_t * tile_dest_buf;
    lv_area_t tile_dest_area;
    const uint8_t * tile_src_buf;
    lv_area_t tile_src_area;

    for(int y = 0; y < total_tiles_y; y++) {

        tile_src_area.y1 = 0; /* no vertical alignment, always start from 0 */
        tile_src_area.y2 = height - y * VGLITE_BLIT_SPLIT_THR - 1;
        if(tile_src_area.y2 >= VGLITE_BLIT_SPLIT_THR) {
            tile_src_area.y2 = VGLITE_BLIT_SPLIT_THR - 1; /* Should never happen */
        }
        tile_src_buf = (uint8_t *)src_buf + y * VGLITE_BLIT_SPLIT_THR * src_stride;

        tile_dest_area.y1 = tile_src_area.y1; /* y has no alignment, always in sync with src */
        tile_dest_area.y2 = tile_src_area.y2;

        tile_dest_buf = (uint8_t *)dest_buf + y * VGLITE_BLIT_SPLIT_THR * dest_stride;

        for(int x = 0; x < total_tiles_x; x++) {

            if(x == 0) {
                /* 1st tile is special - there may be a gap between buffer start pointer
                 * and area.x1 value, as the pointer has to be aligned.
                 * tile_src_buf pointer - keep init value from Y-loop.
                 * Also, 1st tile start is not shifted! shift is applied from 2nd tile */
                tile_src_area.x1 = src_area->x1;
                tile_dest_area.x1 = dest_area->x1;
            }
            else {
                /* subsequent tiles always starts from 0, but shifted*/
                tile_src_area.x1 = 0 + shift_src_x;
                tile_dest_area.x1 = 0 + shift_dest_x;
                /* and advance start pointer + 1 tile size */
                tile_src_buf += VGLITE_BLIT_SPLIT_THR;
                tile_dest_buf += VGLITE_BLIT_SPLIT_THR;
            }

            /* Clip tile end coordinates */
            tile_src_area.x2 = width + src_area->x1 - x * VGLITE_BLIT_SPLIT_THR - 1;
            if(tile_src_area.x2 >= VGLITE_BLIT_SPLIT_THR) {
                tile_src_area.x2 = VGLITE_BLIT_SPLIT_THR - 1;
            }

            tile_dest_area.x2 = width + dest_area->x1 - x * VGLITE_BLIT_SPLIT_THR - 1;
            if(tile_dest_area.x2 >= VGLITE_BLIT_SPLIT_THR) {
                tile_dest_area.x2 = VGLITE_BLIT_SPLIT_THR - 1;
            }

            if(x < (total_tiles_x - 1)) {
                /* And adjust end coords if shifted, but not for last tile! */
                tile_src_area.x2 += shift_src_x;
                tile_dest_area.x2 += shift_dest_x;
            }

            /* Set new dest_vgbuf and src_vgbuf memory addresses. */
            vglite_set_dest_buf_ptr(tile_dest_buf);
            vglite_set_src_buf_ptr(tile_src_buf);

            /* Set scissor */
            vglite_set_scissor(&tile_dest_area);

            /* Set vgmatrix. */
            vglite_set_translation_matrix(&tile_dest_area);

            /* Start blit. */
            _vglite_blit_single(&tile_src_area, opa);

            /* Disable scissor. */
            vglite_disable_scissor();

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
static void _vglite_blit(const lv_area_t * dest_area, const lv_area_t * src_area, lv_opa_t opa)
{
    /* Set scissor. */
    vglite_set_scissor(dest_area);

    /* Set vgmatrix. */
    vglite_set_translation_matrix(dest_area);

    /* Start blit. */
    _vglite_blit_single(src_area, opa);

    /* Disable scissor. */
    vglite_disable_scissor();
}

static void _vglite_blit_transform(const lv_area_t * dest_area, const lv_area_t * clip_area,
                                   const lv_area_t * src_area, const lv_draw_img_dsc_t * dsc)
{
    /* Set scissor */
    vglite_set_scissor(clip_area);

    /* Set vgmatrix. */
    vglite_set_transformation_matrix(dest_area, dsc);

    /* Start blit. */
    _vglite_blit_single(src_area, dsc->opa);

    /* Disable scissor. */
    vglite_disable_scissor();
}
#endif /*VGLITE_BLIT_SPLIT_ENABLED*/

#endif /*LV_USE_DRAW_VGLITE*/
