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

#include "../../lv_draw_label_private.h"
#include "../../../stdlib/lv_string.h"
#include "../../../font/lv_font_fmt_txt_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _draw_vglite_letter(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
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

static bool _use_static_bitmap = false;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vglite_label(vglite_draw_task_t * vglite_task)
{
    const lv_draw_label_dsc_t * dsc = vglite_task->t->draw_dsc;
    const lv_area_t * coords = &vglite_task->t->area;

    if(dsc->opa <= LV_OPA_MIN) return;

    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)dsc->font->dsc;
    bool is_src_buf_aligned = vglite_src_buf_aligned(fdsc->glyph_bitmap, fdsc->stride, LV_COLOR_FORMAT_A8);
    bool has_static_bitmap = lv_font_has_static_bitmap(dsc->font);

    _use_static_bitmap = is_src_buf_aligned & has_static_bitmap;

    lv_draw_label_iterate_characters(vglite_task->t, dsc, coords, _draw_vglite_letter);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _draw_vglite_letter(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area)
{
    if(glyph_draw_dsc) {

        switch(glyph_draw_dsc->format) {

            case LV_FONT_GLYPH_FORMAT_NONE: {
#if LV_USE_FONT_PLACEHOLDER
                    /* Draw a placeholder rectangle*/
                    vglite_draw_task_t * vglite_task = lv_malloc_zeroed(sizeof(vglite_draw_task_t));
                    LV_ASSERT_MALLOC(vglite_task);

                    lv_draw_border_dsc_t border_draw_dsc;

                    lv_draw_border_dsc_init(&border_draw_dsc);
                    border_draw_dsc.opa = glyph_draw_dsc->opa;
                    border_draw_dsc.color = glyph_draw_dsc->color;
                    border_draw_dsc.width = 1;

                    vglite_task->t = t;
                    vglite_task->t->draw_dsc = &border_draw_dsc;
                    vglite_task->t->area = *glyph_draw_dsc->bg_coords;

                    lv_draw_vglite_border(vglite_task);

                    /** Cleanup for vglite_task */
                    VGLITE_CHECK_ERROR(vg_lite_finish());
                    if(vglite_task->path) {
                        VGLITE_CHECK_ERROR(vg_lite_clear_path(vglite_task->path));
                        lv_free(vglite_task->path_data);
                        lv_free(vglite_task->path);
                    }
                    lv_free(vglite_task);
#endif
                }
                break;
            case LV_FONT_GLYPH_FORMAT_A1 ... LV_FONT_GLYPH_FORMAT_A8: {
                    /*Do not draw transparent things*/
                    if(glyph_draw_dsc->opa <= LV_OPA_MIN)
                        return;

                    lv_layer_t * layer = t->target_layer;

                    lv_area_t blend_area;
                    if(!lv_area_intersect(&blend_area, glyph_draw_dsc->letter_coords, &t->clip_area))
                        return;
                    lv_area_move(&blend_area, -layer->buf_area.x1, -layer->buf_area.y1);

                    const void * mask_buf = NULL;
                    const lv_draw_buf_t * draw_buf = NULL;
                    uint32_t mask_stride;
                    if(!_use_static_bitmap) {
                        draw_buf = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
                        if(draw_buf != NULL) {
                            mask_buf = draw_buf->data;
                        }
                        else {
                            return;
                        }
                        mask_stride = draw_buf->header.stride;
                    }
                    else {
                        glyph_draw_dsc->g->req_raw_bitmap = 1;
                        mask_buf = lv_font_get_glyph_static_bitmap(glyph_draw_dsc->g);
                        mask_stride = glyph_draw_dsc->g->stride;
                    }

                    uint32_t mask_width = lv_area_get_width(glyph_draw_dsc->letter_coords);
                    uint32_t mask_height = lv_area_get_height(glyph_draw_dsc->letter_coords);

                    lv_area_t mask_area;
                    mask_area.x1 = blend_area.x1 - (glyph_draw_dsc->letter_coords->x1 - layer->buf_area.x1);
                    mask_area.y1 = blend_area.y1 - (glyph_draw_dsc->letter_coords->y1 - layer->buf_area.y1);
                    mask_area.x2 = mask_width - 1;
                    mask_area.y2 = mask_height - 1;

                    /* Set src_buf structure. */
                    vglite_set_src_buf(mask_buf, mask_width, mask_height, mask_stride, LV_COLOR_FORMAT_A8);

                    /* Set matrix. */
                    vglite_set_translation_matrix(&blend_area);

                    if(!_use_static_bitmap)
                        lv_draw_buf_invalidate_cache(draw_buf, &mask_area);

                    _vglite_draw_letter(&mask_area, glyph_draw_dsc->color, glyph_draw_dsc->opa);
                }
                break;
            case LV_FONT_GLYPH_FORMAT_IMAGE: {
#if LV_USE_IMGFONT
                    glyph_draw_dsc->glyph_data = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
                    vglite_draw_task_t * vglite_task = lv_malloc_zeroed(sizeof(vglite_draw_task_t));
                    LV_ASSERT_MALLOC(vglite_task);

                    lv_draw_image_dsc_t img_dsc;
                    lv_draw_image_dsc_init(&img_dsc);
                    img_dsc.opa = glyph_draw_dsc->opa;
                    img_dsc.src = glyph_draw_dsc->glyph_data;

                    void * old_dsc = t->draw_dsc;
                    lv_area_t old_area = t->area;

                    t->draw_dsc = &img_dsc;
                    t->area = *glyph_draw_dsc->letter_coords;
                    vglite_task->t = t;

                    lv_draw_vglite_img(vglite_task);

                    /** Cleanup for vglite_task */
                    vg_lite_finish();
                    if(vglite_task->path) {
                        VGLITE_CHECK_ERROR(vg_lite_clear_path(vglite_task->path));
                        lv_free(vglite_task->path_data);
                        lv_free(vglite_task->path);
                    }
                    lv_free(vglite_task);

                    t->draw_dsc = old_dsc;
                    t->area = old_area;
#endif
                }
                break;
            default:
                break;
        }
    }

    if(fill_draw_dsc && fill_area) {
        vglite_draw_task_t * vglite_task = lv_malloc_zeroed(sizeof(vglite_draw_task_t));
        LV_ASSERT_MALLOC(vglite_task);

        t->draw_dsc = fill_draw_dsc;
        t->area = *fill_area;
        vglite_task->t = t;

        lv_draw_vglite_fill(vglite_task);

        /** Cleanup for vglite_task */
        vg_lite_finish();
        if(vglite_task->path) {
            VGLITE_CHECK_ERROR(vg_lite_clear_path(vglite_task->path));
            lv_free(vglite_task->path_data);
            lv_free(vglite_task->path);
        }
        if(vglite_task->gradient) {
            VGLITE_CHECK_ERROR(vg_lite_clear_grad(vglite_task->gradient));
            lv_free(vglite_task->gradient);
        }
        lv_free(vglite_task);
    }
}

static void _vglite_draw_letter(const lv_area_t * mask_area, lv_color_t color, lv_opa_t opa)
{
    vg_lite_buffer_t * dest_buf = vglite_get_dest_buf();
    vg_lite_buffer_t * mask_buf = vglite_get_src_buf();

    mask_buf->image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
    mask_buf->transparency_mode = VG_LITE_IMAGE_TRANSPARENT;

    vg_lite_rectangle_t rect = {
        .x = (vg_lite_int32_t)mask_area->x1,
        .y = (vg_lite_int32_t)mask_area->y1,
        .width = (vg_lite_int32_t)lv_area_get_width(mask_area),
        .height = (vg_lite_int32_t)lv_area_get_height(mask_area)
    };

    lv_color32_t col32 = lv_color_to_32(color, opa);
    vg_lite_color_t vgcol = vglite_get_color(col32, false);

    vg_lite_matrix_t * matrix = vglite_get_matrix();

    /*Blit with font color as paint color*/
    VGLITE_CHECK_ERROR(vg_lite_blit_rect(dest_buf, mask_buf, &rect, matrix, VG_LITE_BLEND_SRC_OVER, vgcol,
                                         VG_LITE_FILTER_POINT));

    vglite_run();
}

#endif /*LV_USE_DRAW_VGLITE*/
