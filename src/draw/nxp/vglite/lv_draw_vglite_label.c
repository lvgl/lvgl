/**
 * @file lv_draw_vglite_label.c
 *
 */

/**
 * Copyright 2023-2024 NXP
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
 * @param[in] mask_area Mask area with relative coordinates of source buffer
 * @param[in] color Color
 * @param[in] opa Opacity
 *
 */
static void _vglite_draw_letter(const lv_area_t * mask_area, lv_color_t color, lv_opa_t opa);

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
        switch(glyph_draw_dsc->format) {

            case LV_FONT_GLYPH_FORMAT_NONE: {
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
                break;
            case LV_FONT_GLYPH_FORMAT_A1 ... LV_FONT_GLYPH_FORMAT_A8: {
                    /*Do not draw transparent things*/
                    if(glyph_draw_dsc->opa <= LV_OPA_MIN)
                        return;

                    lv_layer_t * layer = draw_unit->target_layer;

                    lv_area_t blend_area;
                    if(!_lv_area_intersect(&blend_area, glyph_draw_dsc->letter_coords, draw_unit->clip_area))
                        return;
                    lv_area_move(&blend_area, -layer->buf_area.x1, -layer->buf_area.y1);

                    const lv_draw_buf_t * draw_buf = glyph_draw_dsc->glyph_data;
                    const void * mask_buf = draw_buf->data;

                    uint32_t mask_width = lv_area_get_width(glyph_draw_dsc->letter_coords);
                    uint32_t mask_height = lv_area_get_height(glyph_draw_dsc->letter_coords);
                    uint32_t mask_stride = draw_buf->header.stride;

                    lv_area_t mask_area;
                    mask_area.x1 = blend_area.x1 - (glyph_draw_dsc->letter_coords->x1 - layer->buf_area.x1);
                    mask_area.y1 = blend_area.y1 - (glyph_draw_dsc->letter_coords->y1 - layer->buf_area.y1);
                    mask_area.x2 = mask_width - 1;
                    mask_area.y2 = mask_height - 1;

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
                        /* Set src_vgbuf structure. */
                        vglite_set_src_buf(mask_buf, mask_width, mask_height, mask_stride, LV_COLOR_FORMAT_A8);

                        /* Set vgmatrix. */
                        vglite_set_translation_matrix(&blend_area);

                        lv_draw_buf_invalidate_cache(draw_buf, &mask_area);

                        _vglite_draw_letter(&mask_area, glyph_draw_dsc->color, glyph_draw_dsc->opa);
                    }
                }
                break;
            case LV_FONT_GLYPH_FORMAT_IMAGE: {
#if LV_USE_IMGFONT
                    lv_draw_img_dsc_t img_dsc;
                    lv_draw_img_dsc_init(&img_dsc);
                    img_dsc.angle = 0;
                    img_dsc.zoom = LV_ZOOM_NONE;
                    img_dsc.opa = glyph_draw_dsc->opa;
                    img_dsc.src = glyph_draw_dsc->glyph_data;
                    lv_draw_vglite_img(draw_unit, &img_dsc, glyph_draw_dsc->letter_coords);
#endif
                }
                break;
            default:
                break;
        }
    }

    if(fill_draw_dsc && fill_area) {
        lv_draw_vglite_fill(draw_unit, fill_draw_dsc, fill_area);
    }
}

static void _vglite_draw_letter(const lv_area_t * mask_area, lv_color_t color, lv_opa_t opa)
{
    vg_lite_buffer_t * dst_vgbuf = vglite_get_dest_buf();
    vg_lite_buffer_t * mask_vgbuf = vglite_get_src_buf();

    mask_vgbuf->image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
    mask_vgbuf->transparency_mode = VG_LITE_IMAGE_TRANSPARENT;

    vg_lite_rectangle_t rect = {
        .x = (vg_lite_int32_t)mask_area->x1,
        .y = (vg_lite_int32_t)mask_area->y1,
        .width = (vg_lite_int32_t)lv_area_get_width(mask_area),
        .height = (vg_lite_int32_t)lv_area_get_height(mask_area)
    };

    lv_color32_t col32 = lv_color_to_32(color, opa);
    vg_lite_color_t vgcol = vglite_get_color(col32, false);

    vg_lite_matrix_t * vgmatrix = vglite_get_matrix();

    /*Blit with font color as paint color*/
    VGLITE_CHECK_ERROR(vg_lite_blit_rect(dst_vgbuf, mask_vgbuf, &rect, vgmatrix, VG_LITE_BLEND_SRC_OVER, vgcol,
                                         VG_LITE_FILTER_POINT));

    vglite_run();
}

#endif /*LV_USE_DRAW_VGLITE*/
