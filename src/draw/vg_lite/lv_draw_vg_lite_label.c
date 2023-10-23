/**
 * @file lv_draw_vg_lite_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_vg_lite_utils.h"
#include "lv_draw_vg_lite_type.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void draw_letter_cb(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * glyph_draw_dsc,
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

void lv_draw_vg_lite_label(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc,
                           const lv_area_t * coords)
{
    return;
    if(dsc->opa <= LV_OPA_MIN) return;

    lv_draw_label_iterate_letters(draw_unit, dsc, coords, draw_letter_cb);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_letter_cb(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * glyph_draw_dsc,
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
            lv_draw_vg_lite_border(draw_unit, &border_draw_dsc, glyph_draw_dsc->bg_coords);
#endif
        }
        else if(glyph_draw_dsc->format == LV_DRAW_LETTER_BITMAP_FORMAT_A8
                || glyph_draw_dsc->format == LV_DRAW_LETTER_BITMAP_FORMAT_IMAGE) {
            const lv_area_t * letter_coords = glyph_draw_dsc->letter_coords;
            lv_image_dsc_t img_dsc;
            lv_memset(&img_dsc, 0, sizeof(img_dsc));
            img_dsc.header.w = lv_vg_lite_width_align(lv_area_get_width(letter_coords));
            img_dsc.header.h = lv_area_get_height(letter_coords);
            img_dsc.header.cf = (glyph_draw_dsc->format == LV_DRAW_LETTER_BITMAP_FORMAT_A8) ? LV_COLOR_FORMAT_A8 :
                                LV_COLOR_FORMAT_ARGB8888;
            img_dsc.data = glyph_draw_dsc->bitmap;

            lv_draw_image_dsc_t draw_img_dsc;
            lv_draw_image_dsc_init(&draw_img_dsc);
            draw_img_dsc.rotation = 0;
            draw_img_dsc.scale_x = LV_SCALE_NONE;
            draw_img_dsc.scale_y = LV_SCALE_NONE;
            draw_img_dsc.opa = glyph_draw_dsc->opa;
            draw_img_dsc.recolor_opa = glyph_draw_dsc->opa;
            draw_img_dsc.recolor = glyph_draw_dsc->color;
            draw_img_dsc.src = &img_dsc;
            lv_draw_vg_lite_img(draw_unit, &draw_img_dsc, letter_coords);
            lv_cache_lock();
            lv_cache_invalidate(lv_cache_find(&img_dsc, LV_CACHE_SRC_TYPE_PTR, 0, 0));
            lv_cache_unlock();
        }
    }

    if(fill_draw_dsc && fill_area) {
        lv_draw_vg_lite_fill(draw_unit, fill_draw_dsc, fill_area);
    }
}

#endif /*LV_USE_DRAW_VG_LITE*/
