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
#include "lv_nanovg_image_cache.h"
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
static bool is_power_of_2(uint32_t num);
static void fill_repeat_tile_image(
    lv_draw_nanovg_unit_t * u,
    const lv_draw_image_dsc_t * dsc,
    const lv_area_t * coords,
    lv_area_t tile_area,
    const uint32_t img_w,
    const uint32_t img_h,
    const int image_handle);

/**********************
*  STATIC VARIABLES
**********************/

/**********************
*      MACROS
**********************/

/**********************
*   GLOBAL FUNCTIONS
**********************/

void lv_draw_nanovg_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords,
                          int image_handle)
{
    LV_PROFILER_DRAW_BEGIN;

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, &t->_real_area, &t->clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    /* Use coords as the fallback image width and height */
    const uint32_t img_w = dsc->header.w ? dsc->header.w : lv_area_get_width(coords);
    const uint32_t img_h = dsc->header.h ? dsc->header.h : lv_area_get_height(coords);
    bool use_repeat_tile = false;

    if(image_handle < 0) {
        int image_flags = 0;

        if(dsc->tile) {
#ifdef NANOVG_GLES2_IMPLEMENTATION
            /* GLES2 does not support sampling non-power-of-2 textures in repeating mode. */
            if(!is_power_of_2(img_w) || !is_power_of_2(img_h)) {
                LV_LOG_TRACE("Unsupported image size %" LV_PRIu32 " x %" LV_PRIu32 ". Skipping for repeat mode.", img_w, img_h);
                use_repeat_tile = true;
            }
            else
#endif
            {
                LV_UNUSED(is_power_of_2);
                image_flags |= NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY;
            }
        }

        image_handle = lv_nanovg_image_cache_get_handle(u, dsc->src, lv_color_to_32(dsc->recolor, dsc->opa), image_flags, NULL);
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

        if(use_repeat_tile) {
            /* When alignment requirements are not met, simulate tiles by repeating the texture. */
            fill_repeat_tile_image(u, dsc, coords, tile_area, img_w, img_h, image_handle);
            LV_PROFILER_DRAW_END;
            return;
        }

        img_ofs_x = tile_area.x1 - coords->x1;
        img_ofs_y = tile_area.y1 - coords->y1;
        rect_w = lv_area_get_width(coords);
        rect_h = lv_area_get_height(coords);
    }

    nvgBeginPath(u->vg);
    lv_nanovg_path_append_rect(u->vg, 0, 0, rect_w, rect_h, dsc->clip_radius);

    NVGpaint paint = nvgImagePattern(u->vg, img_ofs_x, img_ofs_y, img_w, img_h, 0, image_handle,
                                     dsc->opa / (float)LV_OPA_COVER);
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

static bool is_power_of_2(uint32_t num)
{
    uint32_t n = num > 0 ? num - 1 : 0;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n == num;
}

static void fill_repeat_tile_image(
    lv_draw_nanovg_unit_t * u,
    const lv_draw_image_dsc_t * dsc,
    const lv_area_t * coords,
    lv_area_t tile_area,
    const uint32_t img_w,
    const uint32_t img_h,
    const int image_handle)
{
    if(dsc->clip_radius) {
        LV_LOG_WARN("Unsupported clip radius for repeat mode.");
    }

    const int32_t tile_x_start = tile_area.x1;
    while(tile_area.y1 <= coords->y2) {
        while(tile_area.x1 <= coords->x2) {
            const int32_t img_ofs_x = tile_area.x1 - coords->x1;
            const int32_t img_ofs_y = tile_area.y1 - coords->y1;

            lv_area_t clipped_img_area;
            if(lv_area_intersect(&clipped_img_area, &tile_area, coords)) {
                nvgBeginPath(u->vg);
                lv_nanovg_path_append_rect(u->vg, img_ofs_x, img_ofs_y, img_w, img_h, 0);
                NVGpaint paint = nvgImagePattern(u->vg, img_ofs_x, img_ofs_y, img_w, img_h, 0, image_handle,
                                                 dsc->opa / (float)LV_OPA_COVER);
                nvgFillPaint(u->vg, paint);
                nvgFill(u->vg);
            }

            tile_area.x1 += img_w;
            tile_area.x2 += img_w;
        }

        tile_area.y1 += img_h;
        tile_area.y2 += img_h;
        tile_area.x1 = tile_x_start;
        tile_area.x2 = tile_x_start + img_w - 1;
    }
}

#endif /* LV_USE_DRAW_NANOVG */
