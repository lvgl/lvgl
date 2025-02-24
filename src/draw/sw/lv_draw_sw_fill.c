/**
 * @file lv_draw_sw_fill.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_area_private.h"
#include "lv_draw_sw_mask_private.h"
#include "../lv_draw_private.h"
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW

#include "blend/lv_draw_sw_blend_private.h"
#include "lv_draw_sw_grad.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_text_ap.h"
#include "../../core/lv_refr.h"
#include "../../misc/lv_assert.h"
#include "../../stdlib/lv_string.h"
#include "../lv_draw_mask.h"

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

void lv_draw_sw_fill(lv_draw_task_t * t, lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;

    lv_area_t bg_coords;
    lv_area_copy(&bg_coords, coords);

    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, &bg_coords, &t->clip_area)) return;

    lv_grad_dir_t grad_dir = dsc->grad.dir;
    lv_color_t bg_color    = grad_dir == LV_GRAD_DIR_NONE ? dsc->color : dsc->grad.stops[0].color;

    lv_draw_sw_blend_dsc_t blend_dsc = {0};
    blend_dsc.color = bg_color;

    /*Most simple case: just a plain rectangle*/
    if(dsc->radius == 0 && (grad_dir == LV_GRAD_DIR_NONE)) {
        blend_dsc.blend_area = &bg_coords;
        blend_dsc.opa = dsc->opa;
        lv_draw_sw_blend(t, &blend_dsc);
        return;
    }

    /*Complex case: there is gradient, mask, or radius*/
#if LV_DRAW_SW_COMPLEX == 0
    LV_LOG_WARN("Can't draw complex rectangle because LV_DRAW_SW_COMPLEX = 0");
#else
    lv_opa_t opa = dsc->opa >= LV_OPA_MAX ? LV_OPA_COVER : dsc->opa;

    /*Get the real radius. Can't be larger than the half of the shortest side */
    int32_t coords_bg_w = lv_area_get_width(&bg_coords);
    int32_t coords_bg_h = lv_area_get_height(&bg_coords);
    int32_t short_side = LV_MIN(coords_bg_w, coords_bg_h);
    int32_t rout = LV_MIN(dsc->radius, short_side >> 1);

    /*Add a radius mask if there is a radius*/
    int32_t clipped_w = lv_area_get_width(&clipped_coords);
    lv_opa_t * mask_buf = NULL;
    lv_draw_sw_mask_radius_param_t mask_rout_param;
    void * mask_list[2] = {NULL, NULL};
    if(rout > 0) {
        mask_buf = lv_malloc(clipped_w);
        lv_draw_sw_mask_radius_init(&mask_rout_param, &bg_coords, rout, false);
        mask_list[0] = &mask_rout_param;
    }

    int32_t h;

    lv_area_t blend_area;
    blend_area.x1 = clipped_coords.x1;
    blend_area.x2 = clipped_coords.x2;

    blend_dsc.mask_buf = mask_buf;
    blend_dsc.blend_area = &blend_area;
    blend_dsc.mask_area = &blend_area;
    blend_dsc.opa = LV_OPA_COVER;

    /*Get gradient if appropriate*/
    lv_draw_sw_grad_calc_t * grad = lv_draw_sw_grad_get(&dsc->grad, coords_bg_w, coords_bg_h);
    lv_opa_t * grad_opa_map = NULL;
    bool transp = false;
    if(grad && grad_dir >= LV_GRAD_DIR_HOR) {
        blend_dsc.src_area = &blend_area;
        blend_dsc.src_buf = grad->color_map + clipped_coords.x1 - bg_coords.x1;
        uint32_t s;
        for(s = 0; s < dsc->grad.stops_count; s++) {
            if(dsc->grad.stops[s].opa != LV_OPA_COVER) {
                transp = true;
                break;
            }
        }
        if(grad_dir == LV_GRAD_DIR_HOR) {
            if(transp) grad_opa_map = grad->opa_map + clipped_coords.x1 - bg_coords.x1;
        }
        blend_dsc.src_color_format = LV_COLOR_FORMAT_RGB888;
    }

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS

    /*Prepare complex gradient*/
    if(grad_dir >= LV_GRAD_DIR_LINEAR) {
        LV_ASSERT_NULL(grad);
        switch(grad_dir) {
            case LV_GRAD_DIR_LINEAR:
                lv_draw_sw_grad_linear_setup(&dsc->grad, coords);
                break;
            case LV_GRAD_DIR_RADIAL:
                lv_draw_sw_grad_radial_setup(&dsc->grad, coords);
                break;
            case LV_GRAD_DIR_CONICAL:
                lv_draw_sw_grad_conical_setup(&dsc->grad, coords);
                break;
            default:
                LV_LOG_WARN("Gradient type is not supported");
                return;
        }
        blend_dsc.src_area = &blend_area;
        /* For complex gradients we reuse the color map buffer for the pixel data */
        blend_dsc.src_buf = grad->color_map;
        grad_opa_map = grad->opa_map;
    }
#endif

    /* Draw the top of the rectangle line by line and mirror it to the bottom. */
    for(h = 0; h < rout; h++) {
        int32_t top_y = bg_coords.y1 + h;
        int32_t bottom_y = bg_coords.y2 - h;
        if(top_y < clipped_coords.y1 && bottom_y > clipped_coords.y2) continue;   /*This line is clipped now*/

        bool preblend = false;

        /* Initialize the mask to opa instead of 0xFF and blend with LV_OPA_COVER.
         * It saves calculating the final opa in lv_draw_sw_blend*/
        lv_memset(mask_buf, opa, clipped_w);
        blend_dsc.mask_res = lv_draw_sw_mask_apply(mask_list, mask_buf, blend_area.x1, top_y, clipped_w);
        if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;

        bool hor_grad_processed = false;
        if(top_y >= clipped_coords.y1) {
            blend_area.y1 = top_y;
            blend_area.y2 = top_y;

            switch(grad_dir) {
                case LV_GRAD_DIR_VER:
                    LV_ASSERT_NULL(grad);
                    blend_dsc.color = grad->color_map[top_y - bg_coords.y1];
                    blend_dsc.opa = grad->opa_map[top_y - bg_coords.y1];
                    break;
                case LV_GRAD_DIR_HOR:
                    hor_grad_processed = true;
                    preblend = grad_opa_map != NULL;
                    break;
#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS
                case LV_GRAD_DIR_LINEAR:
                    lv_draw_sw_grad_linear_get_line(&dsc->grad, clipped_coords.x1 - bg_coords.x1, top_y - bg_coords.y1, coords_bg_w, grad);
                    preblend = true;
                    break;
                case LV_GRAD_DIR_RADIAL:
                    lv_draw_sw_grad_radial_get_line(&dsc->grad, clipped_coords.x1 - bg_coords.x1, top_y - bg_coords.y1, coords_bg_w, grad);
                    preblend = true;
                    break;
                case LV_GRAD_DIR_CONICAL:
                    lv_draw_sw_grad_conical_get_line(&dsc->grad, clipped_coords.x1 - bg_coords.x1, top_y - bg_coords.y1, coords_bg_w, grad);
                    preblend = true;
                    break;
#endif
                default:
                    break;
            }
            /* pre-blend the mask */
            if(preblend) {
                int32_t i;
                for(i = 0; i < clipped_w; i++) {
                    if(grad_opa_map[i] < LV_OPA_MAX) mask_buf[i] = (mask_buf[i] * grad_opa_map[i]) >> 8;
                }
                blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
            }
            lv_draw_sw_blend(t, &blend_dsc);
        }

        if(bottom_y <= clipped_coords.y2) {
            blend_area.y1 = bottom_y;
            blend_area.y2 = bottom_y;

            switch(grad_dir) {
                case LV_GRAD_DIR_VER:
                    LV_ASSERT_NULL(grad);
                    blend_dsc.color = grad->color_map[bottom_y - bg_coords.y1];
                    blend_dsc.opa = grad->opa_map[bottom_y - bg_coords.y1];
                    break;
                case LV_GRAD_DIR_HOR:
                    preblend = !hor_grad_processed && (grad_opa_map != NULL);
                    break;
#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS
                case LV_GRAD_DIR_LINEAR:
                    lv_draw_sw_grad_linear_get_line(&dsc->grad, clipped_coords.x1 - bg_coords.x1, bottom_y - bg_coords.y1, coords_bg_w,
                                                    grad);
                    preblend = true;
                    break;
                case LV_GRAD_DIR_RADIAL:
                    lv_draw_sw_grad_radial_get_line(&dsc->grad, clipped_coords.x1 - bg_coords.x1, bottom_y - bg_coords.y1, coords_bg_w,
                                                    grad);
                    preblend = true;
                    break;
                case LV_GRAD_DIR_CONICAL:
                    lv_draw_sw_grad_conical_get_line(&dsc->grad, clipped_coords.x1 - bg_coords.x1, bottom_y - bg_coords.y1, coords_bg_w,
                                                     grad);
                    preblend = true;
                    break;
#endif
                default:
                    break;
            }
            /* pre-blend the mask */
            if(preblend) {
                int32_t i;
                if(grad_dir >= LV_GRAD_DIR_LINEAR) {
                    /*Need to generate the mask again, because we have mixed in the upper part of the gradient*/
                    lv_memset(mask_buf, opa, clipped_w);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(mask_list, mask_buf, blend_area.x1, top_y, clipped_w);
                    if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                }
                for(i = 0; i < clipped_w; i++) {
                    if(grad_opa_map[i] < LV_OPA_MAX) mask_buf[i] = (mask_buf[i] * grad_opa_map[i]) >> 8;
                }
                blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
            }
            lv_draw_sw_blend(t, &blend_dsc);
        }
    }

    /* Draw the center of the rectangle.*/

    /*If no gradient, the center is a simple rectangle*/
    if(grad_dir == LV_GRAD_DIR_NONE) {
        blend_area.y1 = bg_coords.y1 + rout;
        blend_area.y2 = bg_coords.y2 - rout;
        blend_dsc.opa = opa;
        blend_dsc.mask_buf = NULL;
        lv_draw_sw_blend(t, &blend_dsc);
    }
    /*With gradient draw line by line*/
    else {
        blend_dsc.opa = opa;
        switch(grad_dir) {
            case LV_GRAD_DIR_VER:
                blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_FULL_COVER;
                break;
            case LV_GRAD_DIR_HOR:
                blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                blend_dsc.mask_buf = grad_opa_map;
                break;
            case LV_GRAD_DIR_LINEAR:
            case LV_GRAD_DIR_RADIAL:
            case LV_GRAD_DIR_CONICAL:
                blend_dsc.mask_res = transp ? LV_DRAW_SW_MASK_RES_CHANGED : LV_DRAW_SW_MASK_RES_FULL_COVER;
                blend_dsc.mask_buf = grad_opa_map;
                break;
            default:
                break;
        }

        int32_t h_start = LV_MAX(bg_coords.y1 + rout, clipped_coords.y1);
        int32_t h_end = LV_MIN(bg_coords.y2 - rout, clipped_coords.y2);
        for(h = h_start; h <= h_end; h++) {
            blend_area.y1 = h;
            blend_area.y2 = h;

            switch(grad_dir) {
                case LV_GRAD_DIR_VER:
                    LV_ASSERT_NULL(grad);
                    blend_dsc.color = grad->color_map[h - bg_coords.y1];
                    if(opa >= LV_OPA_MAX) blend_dsc.opa = grad->opa_map[h - bg_coords.y1];
                    else blend_dsc.opa = LV_OPA_MIX2(grad->opa_map[h - bg_coords.y1], opa);
                    break;
#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS
                case LV_GRAD_DIR_LINEAR:
                    lv_draw_sw_grad_linear_get_line(&dsc->grad, clipped_coords.x1 - bg_coords.x1, h - bg_coords.y1, coords_bg_w, grad);
                    break;
                case LV_GRAD_DIR_RADIAL:
                    lv_draw_sw_grad_radial_get_line(&dsc->grad, clipped_coords.x1 - bg_coords.x1, h - bg_coords.y1, coords_bg_w, grad);
                    break;
                case LV_GRAD_DIR_CONICAL:
                    lv_draw_sw_grad_conical_get_line(&dsc->grad, clipped_coords.x1 - bg_coords.x1, h - bg_coords.y1, coords_bg_w, grad);
                    break;
#endif
                default:
                    break;
            }
            lv_draw_sw_blend(t, &blend_dsc);
        }
    }

    if(mask_buf) {
        lv_free(mask_buf);
        lv_draw_sw_mask_free_param(&mask_rout_param);
    }
    if(grad) {
        lv_draw_sw_grad_cleanup(grad);
    }
#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS
    if(grad_dir >= LV_GRAD_DIR_LINEAR) {
        switch(grad_dir) {
            case LV_GRAD_DIR_LINEAR:
                lv_draw_sw_grad_linear_cleanup(&dsc->grad);
                break;
            case LV_GRAD_DIR_RADIAL:
                lv_draw_sw_grad_radial_cleanup(&dsc->grad);
                break;
            case LV_GRAD_DIR_CONICAL:
                lv_draw_sw_grad_conical_cleanup(&dsc->grad);
                break;
            default:
                break;
        }
    }
#endif

#endif
}

#endif /*LV_USE_DRAW_SW*/
