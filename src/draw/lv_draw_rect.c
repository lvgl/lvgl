/**
 * @file lv_draw_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_rect_private.h"
#include "lv_draw_private.h"
#include "../core/lv_obj.h"
#include "../misc/lv_assert.h"
#include "../core/lv_obj_event.h"
#include "../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void LV_ATTRIBUTE_FAST_MEM lv_draw_rect_dsc_init(lv_draw_rect_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_rect_dsc_t));
    dsc->bg_color = lv_color_white();
    dsc->bg_grad.stops[0].color = lv_color_white();
    dsc->bg_grad.stops[1].color = lv_color_black();
    dsc->bg_grad.stops[1].frac = 0xFF;
    dsc->bg_grad.stops_count = 2;
    dsc->border_color = lv_color_black();
    dsc->shadow_color = lv_color_black();
    dsc->bg_image_symbol_font = LV_FONT_DEFAULT;
    dsc->bg_opa = LV_OPA_COVER;
    dsc->bg_image_opa = LV_OPA_COVER;
    dsc->outline_opa = LV_OPA_COVER;
    dsc->border_opa = LV_OPA_COVER;
    dsc->shadow_opa = LV_OPA_COVER;
    dsc->border_side = LV_BORDER_SIDE_FULL;
}

void lv_draw_fill_dsc_init(lv_draw_fill_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(*dsc));
    dsc->opa = LV_OPA_COVER;
    dsc->base.dsc_size = sizeof(lv_draw_fill_dsc_t);
}

lv_draw_fill_dsc_t * lv_draw_task_get_fill_dsc(lv_draw_task_t * task)
{
    return task->type == LV_DRAW_TASK_TYPE_FILL ? (lv_draw_fill_dsc_t *)task->draw_dsc : NULL;
}

void lv_draw_border_dsc_init(lv_draw_border_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(*dsc));
    dsc->opa = LV_OPA_COVER;
    dsc->side = LV_BORDER_SIDE_FULL;
    dsc->base.dsc_size = sizeof(lv_draw_border_dsc_t);
}

lv_draw_border_dsc_t * lv_draw_task_get_border_dsc(lv_draw_task_t * task)
{
    return task->type == LV_DRAW_TASK_TYPE_BORDER ? (lv_draw_border_dsc_t *)task->draw_dsc : NULL;
}

void lv_draw_box_shadow_dsc_init(lv_draw_box_shadow_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(*dsc));
    dsc->opa = LV_OPA_COVER;
    dsc->base.dsc_size = sizeof(lv_draw_box_shadow_dsc_t);
}

lv_draw_box_shadow_dsc_t * lv_draw_task_get_box_shadow_dsc(lv_draw_task_t * task)
{
    return task->type == LV_DRAW_TASK_TYPE_BOX_SHADOW ? (lv_draw_box_shadow_dsc_t *)task->draw_dsc : NULL;
}

void lv_draw_rect(lv_layer_t * layer, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{

    LV_PROFILER_DRAW_BEGIN;
    bool has_shadow;
    bool has_fill;
    bool has_border;
    bool has_outline;
    bool has_bg_img;

    if(dsc->shadow_width == 0 ||
       dsc->shadow_opa <= LV_OPA_MIN ||
       (dsc->shadow_width == 1 && dsc->shadow_spread <= 0 &&
        dsc->shadow_offset_x == 0 && dsc->shadow_offset_y == 0)) {
        has_shadow = false;
    }
    else {
        has_shadow = true;
    }

    if(dsc->bg_opa <= LV_OPA_MIN) has_fill = false;
    else has_fill = true;

    if(dsc->bg_image_opa <= LV_OPA_MIN || dsc->bg_image_src == NULL) has_bg_img = false;
    else has_bg_img = true;

    if(dsc->border_opa <= LV_OPA_MIN
       || dsc->border_width == 0
       || dsc->border_post == true
       || dsc->border_side == LV_BORDER_SIDE_NONE) has_border = false;
    else has_border = true;

    if(dsc->outline_opa <= LV_OPA_MIN || dsc->outline_width == 0) has_outline = false;
    else has_outline = true;

    bool bg_cover = true;
    if(dsc->bg_opa < LV_OPA_COVER) bg_cover = false;
    else if(dsc->bg_grad.dir != LV_GRAD_DIR_NONE) {
        uint32_t s;
        for(s = 0; s < dsc->bg_grad.stops_count; s++) {
            if(dsc->bg_grad.stops[s].opa != LV_OPA_COVER) {
                bg_cover = false;
                break;
            }
        }
    }

    lv_draw_task_t * t;

    /*Shadow*/
    if(has_shadow) {
        /*Check whether the shadow is visible*/
        t = lv_draw_add_task(layer, coords);
        lv_draw_box_shadow_dsc_t * shadow_dsc = lv_malloc(sizeof(lv_draw_box_shadow_dsc_t));
        LV_ASSERT_MALLOC(shadow_dsc);
        t->draw_dsc = shadow_dsc;
        lv_area_increase(&t->_real_area, dsc->shadow_spread, dsc->shadow_spread);
        lv_area_increase(&t->_real_area, dsc->shadow_width, dsc->shadow_width);
        lv_area_move(&t->_real_area, dsc->shadow_offset_x, dsc->shadow_offset_y);
        shadow_dsc->base = dsc->base;
        shadow_dsc->base.dsc_size = sizeof(lv_draw_box_shadow_dsc_t);
        shadow_dsc->radius = dsc->radius;
        shadow_dsc->color = dsc->shadow_color;
        shadow_dsc->width = dsc->shadow_width;
        shadow_dsc->spread = dsc->shadow_spread;
        shadow_dsc->opa = dsc->shadow_opa;
        shadow_dsc->ofs_x = dsc->shadow_offset_x;
        shadow_dsc->ofs_y = dsc->shadow_offset_y;
        shadow_dsc->bg_cover = bg_cover;
        t->type = LV_DRAW_TASK_TYPE_BOX_SHADOW;
        lv_draw_finalize_task_creation(layer, t);
    }

    /*Background*/
    if(has_fill) {
        lv_area_t bg_coords = *coords;
        /*If the border fully covers make the bg area 1px smaller to avoid artifacts on the corners*/
        if(dsc->border_width > 1 && dsc->border_opa >= LV_OPA_MAX && dsc->radius != 0) {
            bg_coords.x1 += (dsc->border_side & LV_BORDER_SIDE_LEFT) ? 1 : 0;
            bg_coords.y1 += (dsc->border_side & LV_BORDER_SIDE_TOP) ? 1 : 0;
            bg_coords.x2 -= (dsc->border_side & LV_BORDER_SIDE_RIGHT) ? 1 : 0;
            bg_coords.y2 -= (dsc->border_side & LV_BORDER_SIDE_BOTTOM) ? 1 : 0;
        }

        t = lv_draw_add_task(layer, &bg_coords);
        lv_draw_fill_dsc_t * bg_dsc = lv_malloc(sizeof(lv_draw_fill_dsc_t));
        LV_ASSERT_MALLOC(bg_dsc);
        lv_draw_fill_dsc_init(bg_dsc);
        t->draw_dsc = bg_dsc;
        bg_dsc->base = dsc->base;
        bg_dsc->base.dsc_size = sizeof(lv_draw_fill_dsc_t);
        bg_dsc->radius = dsc->radius;
        bg_dsc->color = dsc->bg_color;
        bg_dsc->grad = dsc->bg_grad;
        bg_dsc->opa = dsc->bg_opa;
        t->type = LV_DRAW_TASK_TYPE_FILL;

        lv_draw_finalize_task_creation(layer, t);
    }

    /*Background image*/
    if(has_bg_img) {
        lv_image_src_t src_type = lv_image_src_get_type(dsc->bg_image_src);
        lv_result_t res = LV_RESULT_OK;
        lv_image_header_t header;
        if(src_type == LV_IMAGE_SRC_VARIABLE || src_type == LV_IMAGE_SRC_FILE) {
            res  = lv_image_decoder_get_info(dsc->bg_image_src, &header);
        }
        else {
            lv_memzero(&header, sizeof(header));

            if(src_type == LV_IMAGE_SRC_UNKNOWN) {
                res = LV_RESULT_INVALID;
            }
        }

        if(res == LV_RESULT_OK) {
            if(src_type == LV_IMAGE_SRC_VARIABLE || src_type == LV_IMAGE_SRC_FILE) {

                if(dsc->bg_image_tiled) {
                    t = lv_draw_add_task(layer, coords);
                }
                else {
                    lv_area_t a = {0, 0, header.w - 1, header.h - 1};
                    lv_area_align(coords, &a, LV_ALIGN_CENTER, 0, 0);
                    t = lv_draw_add_task(layer, &a);
                }

                lv_draw_image_dsc_t * bg_image_dsc = lv_malloc(sizeof(lv_draw_image_dsc_t));
                LV_ASSERT_MALLOC(bg_image_dsc);
                lv_draw_image_dsc_init(bg_image_dsc);
                t->draw_dsc = bg_image_dsc;
                bg_image_dsc->base = dsc->base;
                bg_image_dsc->base.dsc_size = sizeof(lv_draw_image_dsc_t);
                bg_image_dsc->src = dsc->bg_image_src;
                bg_image_dsc->opa = dsc->bg_image_opa;
                bg_image_dsc->recolor = dsc->bg_image_recolor;
                bg_image_dsc->recolor_opa = dsc->bg_image_recolor_opa;
                bg_image_dsc->tile = dsc->bg_image_tiled;
                bg_image_dsc->header = header;
                bg_image_dsc->clip_radius = dsc->radius;
                bg_image_dsc->image_area = *coords;
                t->type = LV_DRAW_TASK_TYPE_IMAGE;
                lv_draw_finalize_task_creation(layer, t);
            }
            else {
                lv_point_t s;
                lv_text_get_size(&s, dsc->bg_image_src, dsc->bg_image_symbol_font, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

                lv_area_t a = {0, 0, s.x - 1, s.y - 1};
                lv_area_align(coords, &a, LV_ALIGN_CENTER, 0, 0);
                t = lv_draw_add_task(layer, &a);

                lv_draw_label_dsc_t * bg_label_dsc = lv_malloc(sizeof(lv_draw_label_dsc_t));
                LV_ASSERT_MALLOC(bg_label_dsc);
                lv_draw_label_dsc_init(bg_label_dsc);
                t->draw_dsc = bg_label_dsc;
                bg_label_dsc->base = dsc->base;
                bg_label_dsc->base.dsc_size = sizeof(lv_draw_label_dsc_t);
                bg_label_dsc->color = dsc->bg_image_recolor;
                bg_label_dsc->font = dsc->bg_image_symbol_font;
                bg_label_dsc->text = dsc->bg_image_src;
                t->type = LV_DRAW_TASK_TYPE_LABEL;
                lv_draw_finalize_task_creation(layer, t);
            }
        }
    }

    /*Border*/
    if(has_border) {
        t = lv_draw_add_task(layer, coords);
        lv_draw_border_dsc_t * border_dsc = lv_malloc(sizeof(lv_draw_border_dsc_t));
        LV_ASSERT_MALLOC(border_dsc);
        t->draw_dsc = border_dsc;
        border_dsc->base = dsc->base;
        border_dsc->base.dsc_size = sizeof(lv_draw_border_dsc_t);
        border_dsc->radius = dsc->radius;
        border_dsc->color = dsc->border_color;
        border_dsc->opa = dsc->border_opa;
        border_dsc->width = dsc->border_width;
        border_dsc->side = dsc->border_side;
        t->type = LV_DRAW_TASK_TYPE_BORDER;
        lv_draw_finalize_task_creation(layer, t);
    }

    /*Outline*/
    if(has_outline) {
        lv_area_t outline_coords = *coords;
        lv_area_increase(&outline_coords, dsc->outline_width + dsc->outline_pad, dsc->outline_width + dsc->outline_pad);
        t = lv_draw_add_task(layer, &outline_coords);
        lv_draw_border_dsc_t * outline_dsc = lv_malloc(sizeof(lv_draw_border_dsc_t));
        LV_ASSERT_MALLOC(outline_dsc);
        t->draw_dsc = outline_dsc;
        lv_area_increase(&t->_real_area, dsc->outline_width, dsc->outline_width);
        lv_area_increase(&t->_real_area, dsc->outline_pad, dsc->outline_pad);
        outline_dsc->base = dsc->base;
        outline_dsc->base.dsc_size = sizeof(lv_draw_border_dsc_t);
        outline_dsc->radius = dsc->radius == LV_RADIUS_CIRCLE ? LV_RADIUS_CIRCLE : dsc->radius + dsc->outline_width +
                              dsc->outline_pad;
        outline_dsc->color = dsc->outline_color;
        outline_dsc->opa = dsc->outline_opa;
        outline_dsc->width = dsc->outline_width;
        outline_dsc->side = LV_BORDER_SIDE_FULL;
        t->type = LV_DRAW_TASK_TYPE_BORDER;
        lv_draw_finalize_task_creation(layer, t);
    }

    LV_ASSERT_MEM_INTEGRITY();

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
