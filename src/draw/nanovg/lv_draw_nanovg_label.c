/**
 * @file lv_draw_nanovg_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG

#include "lv_nanovg_pending.h"
#include "lv_nanovg_utils.h"
#include "../lv_draw_label_private.h"
#include "../lv_draw_image_private.h"
#include "../../misc/cache/lv_cache_entry_private.h"

/*********************
*      DEFINES
*********************/

/**********************
*      TYPEDEFS
**********************/

/**********************
*  STATIC PROTOTYPES
**********************/

static void draw_letter_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                           lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area);

/**********************
*  STATIC VARIABLES
**********************/

/**********************
*      MACROS
**********************/

/**********************
*   GLOBAL FUNCTIONS
**********************/

void lv_draw_nanovg_label_init(lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
}

void lv_draw_nanovg_label_deinit(lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
}

void lv_draw_nanovg_letter(lv_draw_task_t * t, const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords)
{
    LV_ASSERT_NULL(t);
    LV_ASSERT_NULL(dsc);
    LV_ASSERT_NULL(coords);

    if(dsc->opa <= LV_OPA_MIN)
        return;

    LV_PROFILER_DRAW_BEGIN;

    lv_draw_glyph_dsc_t glyph_dsc;
    lv_draw_glyph_dsc_init(&glyph_dsc);
    glyph_dsc.opa = dsc->opa;
    glyph_dsc.bg_coords = NULL;
    glyph_dsc.color = dsc->color;
    glyph_dsc.rotation = dsc->rotation;
    glyph_dsc.pivot = dsc->pivot;

    lv_draw_unit_draw_letter(t, &glyph_dsc, &(lv_point_t) {
        .x = coords->x1, .y = coords->y1
    },
    dsc->font, dsc->unicode, draw_letter_cb);

    if(glyph_dsc._draw_buf) {
        lv_draw_buf_destroy(glyph_dsc._draw_buf);
        glyph_dsc._draw_buf = NULL;
    }

    LV_PROFILER_DRAW_END;
}

void lv_draw_nanovg_label(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_label_iterate_characters(t, dsc, coords, draw_letter_cb);
    LV_PROFILER_DRAW_END;
}

/**********************
*   STATIC FUNCTIONS
**********************/

static inline void convert_letter_matrix(lv_matrix_t * matrix, const lv_draw_glyph_dsc_t * dsc)
{
    lv_matrix_translate(matrix, dsc->letter_coords->x1, dsc->letter_coords->y1);

    if(!dsc->rotation) {
        return;
    }

    const lv_point_t pivot = {
        .x = dsc->pivot.x,
        .y = dsc->g->box_h + dsc->g->ofs_y
    };
    lv_matrix_translate(matrix, pivot.x, pivot.y);
    lv_matrix_rotate(matrix, dsc->rotation / 10.0f);
    lv_matrix_translate(matrix, -pivot.x, -pivot.y);
}

static bool draw_letter_clip_areas(lv_draw_task_t * t, const lv_draw_glyph_dsc_t * dsc, lv_area_t * letter_area,
                                   lv_area_t * cliped_area)
{
    *letter_area = *dsc->letter_coords;

    if(dsc->rotation) {
        const lv_point_t pivot = {
            .x = dsc->pivot.x,
            .y = dsc->g->box_h + dsc->g->ofs_y
        };

        lv_image_buf_get_transformed_area(
            letter_area,
            lv_area_get_width(dsc->letter_coords),
            lv_area_get_height(dsc->letter_coords),
            dsc->rotation,
            LV_SCALE_NONE,
            LV_SCALE_NONE,
            &pivot);
        lv_area_move(letter_area, dsc->letter_coords->x1, dsc->letter_coords->y1);
    }

    if(!lv_area_intersect(cliped_area, &t->clip_area, letter_area)) {
        return false;
    }

    return true;
}

static void draw_letter_bitmap(lv_draw_task_t * t, const lv_draw_glyph_dsc_t * dsc, const lv_draw_buf_t * src_buf)
{
    LV_PROFILER_DRAW_BEGIN;

    lv_area_t image_area;
    lv_area_t clip_area;
    if(!draw_letter_clip_areas(t, dsc, &image_area, &clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    int image_handle = lv_nanovg_push_image(u, src_buf, lv_color_to_32(dsc->color, dsc->opa));
    if(image_handle < 0) {
        LV_PROFILER_DRAW_END;
        return;
    }

    if(!dsc->rotation) {
        float x = dsc->letter_coords->x1;
        float y = dsc->letter_coords->y1;
        float w = lv_area_get_width(dsc->letter_coords);
        float h = lv_area_get_height(dsc->letter_coords);

        NVGpaint paint = nvgImagePattern(u->vg, x, y, w, h, 0, image_handle, 1.0f);

        nvgBeginPath(u->vg);
        nvgRect(u->vg, x, y, w, h);
        nvgFillColor(u->vg, nvgRGBA(0, 0, 0, 0));
        nvgFillPaint(u->vg, paint);
        nvgFill(u->vg);
    }
    else {
    }



    LV_PROFILER_DRAW_END;
}

static inline bool init_buffer_from_glyph_dsc(lv_draw_buf_t * buffer, lv_font_glyph_dsc_t * g_dsc)
{
    const void * glyph_bitmap = lv_font_get_glyph_static_bitmap(g_dsc);
    if(!glyph_bitmap) {
        return false;
    }

    lv_result_t res = lv_draw_buf_init(
                          buffer,
                          g_dsc->box_w,
                          g_dsc->box_h,
                          LV_COLOR_FORMAT_A8,
                          g_dsc->stride,
                          (void *)glyph_bitmap,
                          g_dsc->stride * g_dsc->box_h);

    return res == LV_RESULT_OK;
}

static void draw_letter_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                           lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area)
{
    if(glyph_draw_dsc) {
        switch(glyph_draw_dsc->format) {
            case LV_FONT_GLYPH_FORMAT_A1:
            case LV_FONT_GLYPH_FORMAT_A2:
            case LV_FONT_GLYPH_FORMAT_A3:
            case LV_FONT_GLYPH_FORMAT_A4:
            case LV_FONT_GLYPH_FORMAT_A8: {
                    lv_draw_buf_t src_buf;
                    if(lv_font_has_static_bitmap(glyph_draw_dsc->g->resolved_font)) {
                        if(!init_buffer_from_glyph_dsc(&src_buf, glyph_draw_dsc->g)) {
                            return;
                        }
                    }
                    else {
                        glyph_draw_dsc->glyph_data = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
                        if(!glyph_draw_dsc->glyph_data) {
                            return;
                        }

                        src_buf = *(lv_draw_buf_t *)glyph_draw_dsc->glyph_data;
                    }

                    draw_letter_bitmap(t, glyph_draw_dsc, &src_buf);
                }
                break;

#if LV_USE_FREETYPE
            case LV_FONT_GLYPH_FORMAT_VECTOR: {
                    if(lv_freetype_is_outline_font(glyph_draw_dsc->g->resolved_font)) {
                        if(!glyph_draw_dsc->glyph_data) {
                            return;
                        }

                        // draw_letter_outline(t, glyph_draw_dsc);
                    }
                }
                break;
#endif /* LV_USE_FREETYPE */

            case LV_FONT_GLYPH_FORMAT_IMAGE: {
                    glyph_draw_dsc->glyph_data = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
                    if(!glyph_draw_dsc->glyph_data) {
                        return;
                    }

                    lv_draw_image_dsc_t image_dsc;
                    lv_draw_image_dsc_init(&image_dsc);
                    image_dsc.opa = glyph_draw_dsc->opa;
                    image_dsc.src = glyph_draw_dsc->glyph_data;
                    image_dsc.rotation = glyph_draw_dsc->rotation;
                    lv_draw_nanovg_image(t, &image_dsc, glyph_draw_dsc->letter_coords, false);
                }
                break;

#if LV_USE_FONT_PLACEHOLDER
            case LV_FONT_GLYPH_FORMAT_NONE: {
                    if(glyph_draw_dsc->bg_coords == NULL) break;
                    /* Draw a placeholder rectangle*/
                    lv_draw_border_dsc_t border_draw_dsc;
                    lv_draw_border_dsc_init(&border_draw_dsc);
                    border_draw_dsc.opa = glyph_draw_dsc->opa;
                    border_draw_dsc.color = glyph_draw_dsc->color;
                    border_draw_dsc.width = 1;
                    lv_draw_nanovg_border(t, &border_draw_dsc, glyph_draw_dsc->bg_coords);
                }
                break;
#endif /* LV_USE_FONT_PLACEHOLDER */

            default:
                break;
        }
    }

    if(fill_draw_dsc && fill_area) {
        lv_draw_nanovg_fill(t, fill_draw_dsc, fill_area);
    }
}

#endif /* LV_USE_DRAW_NANOVG */
