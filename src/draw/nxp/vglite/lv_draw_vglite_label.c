/**
 * @file lv_draw_vglite_label.c
 *
 */

/**
 * Copyright 2023 NXP
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

#include "../../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _draw_vglite_letter(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area);

/**
 * Draw letter (character bitmap blend) with optional color and opacity
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] mask_buf Mask buffer
 * @param[in] mask_area Mask area with relative coordinates of source buffer
 * @param[in] mask_stride Stride of mask buffer in bytes
 * @param[in] color Color
 * @param[in] opa Opacity
 *
 */
static void _vglite_draw_letter(const lv_area_t * dest_area,
                                const void * mask_buf, const lv_area_t * mask_area, uint32_t mask_stride,
                                lv_color_t color, lv_opa_t opa);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vglite_label(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc,
                          const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;

    lv_draw_label_iterate_characters(draw_unit, dsc, coords, _draw_vglite_letter);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _draw_vglite_letter(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area)
{
    if(glyph_draw_dsc) {
        if(glyph_draw_dsc->bitmap == NULL) {
#if LV_USE_FONT_PLACEHOLDER
            /* Draw a placeholder rectangle*/
            lv_draw_border_dsc_t border_draw_dsc;
            lv_draw_border_dsc_init(&border_draw_dsc);
            border_draw_dsc.opa = glyph_draw_dsc->opa;
            border_draw_dsc.color = glyph_draw_dsc->color;
            border_draw_dsc.width = 1;
            lv_draw_vglite_border(draw_unit, &border_draw_dsc, glyph_draw_dsc->bg_coords);
#endif
        }
        else if(glyph_draw_dsc->format == LV_DRAW_LETTER_BITMAP_FORMAT_A8) {
            /*Do not draw transparent things*/
            if(glyph_draw_dsc->opa <= LV_OPA_MIN)
                return;

            lv_layer_t * layer = draw_unit->target_layer;

            lv_area_t blend_area;
            if(!_lv_area_intersect(&blend_area, glyph_draw_dsc->letter_coords, draw_unit->clip_area))
                return;
            lv_area_move(&blend_area, -layer->draw_buf_ofs.x, -layer->draw_buf_ofs.y);

            const uint8_t * mask_buf = glyph_draw_dsc->bitmap;
            lv_area_t mask_area;
            lv_area_copy(&mask_area, glyph_draw_dsc->letter_coords);
            lv_area_move(&mask_area, -layer->draw_buf_ofs.x, -layer->draw_buf_ofs.y);

            uint32_t mask_stride = lv_draw_buf_width_to_stride(
                                       lv_area_get_width(glyph_draw_dsc->letter_coords),
                                       LV_COLOR_FORMAT_A8);
            if(mask_buf) {
                mask_buf += mask_stride * (blend_area.y1 - glyph_draw_dsc->letter_coords->y1) +
                            (blend_area.x1 - glyph_draw_dsc->letter_coords->x1);
            }

            if(!vglite_buf_aligned(mask_buf, mask_stride, LV_COLOR_FORMAT_A8)) {
                /* Draw a placeholder rectangle*/
                lv_draw_border_dsc_t border_draw_dsc;
                lv_draw_border_dsc_init(&border_draw_dsc);
                border_draw_dsc.opa = glyph_draw_dsc->opa;
                border_draw_dsc.color = glyph_draw_dsc->color;
                border_draw_dsc.width = 1;
                lv_draw_vglite_border(draw_unit, &border_draw_dsc, glyph_draw_dsc->bg_coords);
            }
            else {

                _vglite_draw_letter(&blend_area, mask_buf, &mask_area, mask_stride,
                                    glyph_draw_dsc->color, glyph_draw_dsc->opa);
            }
        }
        else if(glyph_draw_dsc->format == LV_DRAW_LETTER_BITMAP_FORMAT_IMAGE) {
#if LV_USE_IMGFONT
            lv_draw_img_dsc_t img_dsc;
            lv_draw_img_dsc_init(&img_dsc);
            img_dsc.angle = 0;
            img_dsc.zoom = LV_ZOOM_NONE;
            img_dsc.opa = glyph_draw_dsc->opa;
            img_dsc.src = glyph_draw_dsc->bitmap;
            lv_draw_vglite_img(draw_unit, &img_dsc, glyph_draw_dsc->letter_coords);
#endif
        }
    }

    if(fill_draw_dsc && fill_area) {
        lv_draw_vglite_fill(draw_unit, fill_draw_dsc, fill_area);
    }
}

static void _vglite_draw_letter(const lv_area_t * dest_area,
                                const void * mask_buf, const lv_area_t * mask_area, uint32_t mask_stride,
                                lv_color_t color, lv_opa_t opa)
{
    vg_lite_error_t err = VG_LITE_SUCCESS;
    vg_lite_buffer_t * dst_vgbuf = vglite_get_dest_buf();

    vg_lite_buffer_t mask_vgbuf;
    mask_vgbuf.format = VG_LITE_A8;
    mask_vgbuf.tiled = VG_LITE_LINEAR;
    mask_vgbuf.image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
    mask_vgbuf.transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
    mask_vgbuf.width = (int32_t)lv_area_get_width(mask_area);
    mask_vgbuf.height = (int32_t)lv_area_get_height(mask_area);
    mask_vgbuf.stride = (int32_t)mask_stride;

    lv_memzero(&mask_vgbuf.yuv, sizeof(mask_vgbuf.yuv));

    mask_vgbuf.memory = (void *)mask_buf;
    mask_vgbuf.address = (uint32_t)mask_vgbuf.memory;
    mask_vgbuf.handle = NULL;

    uint32_t rect[] = {
        (uint32_t)0, /* start x */
        (uint32_t)0, /* start y */
        (uint32_t)lv_area_get_width(mask_area), /* width */
        (uint32_t)lv_area_get_height(mask_area) /* height */
    };

    lv_color32_t col32 = lv_color_to_32(color, opa);
    vg_lite_color_t vgcol = vglite_get_color(col32, false);

    /* Set vgmatrix. */
    vglite_set_translation_matrix(dest_area);
    vg_lite_matrix_t * vgmatrix = vglite_get_matrix();

    /*Blit with font color as paint color*/
    err = vg_lite_blit_rect(dst_vgbuf, &mask_vgbuf, rect, vgmatrix, VG_LITE_BLEND_SRC_OVER, vgcol,
                            VG_LITE_FILTER_POINT);
    LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Draw letter failed.");

    vglite_run();
}

#endif /*LV_USE_DRAW_VGLITE*/
