/**
 * @file lv_draw_nanovg_image.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG

#include "lv_nanovg_utils.h"
#include "lv_nanovg_math.h"
#include "../lv_image_decoder_private.h"
#include "../lv_draw_image_private.h"

/*********************
*      DEFINES
*********************/

/**********************
*      TYPEDEFS
**********************/

/**********************
*  STATIC PROTOTYPES
**********************/

static void image_dsc_to_matrix(lv_matrix_t * matrix, int32_t x, int32_t y, const lv_draw_image_dsc_t * dsc);

/**********************
*  STATIC VARIABLES
**********************/

/**********************
*      MACROS
**********************/

/**********************
*   GLOBAL FUNCTIONS
**********************/

void lv_draw_nanovg_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords, bool no_cache,
                          int image_handle)
{
    LV_PROFILER_DRAW_BEGIN;

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, &t->_real_area, &t->clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    if(image_handle < 0) {
        const int imageFlags = dsc->tile ? (NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY) : 0;

        const lv_draw_buf_t * src_buf = lv_nanovg_open_image_buffer(u, dsc->src, no_cache, false);

        if(!src_buf) {
            LV_PROFILER_DRAW_END;
            return;
        }
        image_handle = lv_nanovg_push_image(u, src_buf, lv_color_to_32(dsc->recolor, dsc->opa), imageFlags);
    }

    if(image_handle < 0) {
        LV_PROFILER_DRAW_END;
        return;
    }

    /* original image matrix */
    lv_matrix_t image_matrix;
    lv_matrix_identity(&image_matrix);
    image_dsc_to_matrix(&image_matrix, coords->x1, coords->y1, dsc);
    lv_nanovg_transform(u->vg, &image_matrix);

    /* Use coords as the fallback image width and height */
    const uint32_t img_w = dsc->header.w ? dsc->header.w : lv_area_get_width(coords);
    const uint32_t img_h = dsc->header.h ? dsc->header.h : lv_area_get_height(coords);

    int32_t img_ofs_x = 0;
    int32_t img_ofs_y = 0;
    int32_t rect_w = img_w;
    int32_t rect_h = img_h;

    if(dsc->tile) {
        lv_area_t tile_area;
        if(lv_area_get_width(&dsc->image_area) >= 0) {
            tile_area = dsc->image_area;
        }
        else {
            tile_area = *coords;
        }

        img_ofs_x = tile_area.x1 - coords->x1;
        img_ofs_y = tile_area.y1 - coords->y1;
        rect_w = lv_area_get_width(coords);
        rect_h = lv_area_get_height(coords);
    }

    NVGpaint paint = nvgImagePattern(u->vg, img_ofs_x, img_ofs_y, img_w, img_h, 0, image_handle,
                                     dsc->opa / (float)LV_OPA_COVER);

    nvgBeginPath(u->vg);
    lv_nanovg_path_append_rect(u->vg, 0, 0, rect_w, rect_h, dsc->clip_radius);
    nvgFillPaint(u->vg, paint);
    nvgFill(u->vg);

    LV_PROFILER_DRAW_END;
}

/**********************
*   STATIC FUNCTIONS
**********************/

static void image_dsc_to_matrix(lv_matrix_t * matrix, int32_t x, int32_t y, const lv_draw_image_dsc_t * dsc)
{
    LV_ASSERT_NULL(matrix);
    LV_ASSERT_NULL(dsc);

    int32_t rotation = dsc->rotation;
    int32_t scale_x = dsc->scale_x;
    int32_t scale_y = dsc->scale_y;

    lv_matrix_translate(matrix, x, y);

    if(rotation != 0 || scale_x != LV_SCALE_NONE || scale_y != LV_SCALE_NONE) {
        lv_point_t pivot = dsc->pivot;
        lv_matrix_translate(matrix, pivot.x, pivot.y);

        if(rotation != 0) {
            lv_matrix_rotate(matrix, rotation * 0.1f);
        }

        if(scale_x != LV_SCALE_NONE || scale_y != LV_SCALE_NONE) {
            lv_matrix_scale(
                matrix,
                (float)scale_x / LV_SCALE_NONE,
                (float)scale_y / LV_SCALE_NONE);
        }

        lv_matrix_translate(matrix, -pivot.x, -pivot.y);
    }
}

#endif /* LV_USE_DRAW_NANOVG */
