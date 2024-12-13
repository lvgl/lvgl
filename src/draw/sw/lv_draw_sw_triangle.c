/**
 * @file lv_draw_sw_triangle.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_mask_private.h"
#include "blend/lv_draw_sw_blend_private.h"
#include "../lv_draw_private.h"
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW

#include "../../misc/lv_math.h"
#include "../../stdlib/lv_mem.h"
#include "../../misc/lv_area_private.h"
#include "../../misc/lv_color.h"
#include "../../stdlib/lv_string.h"
#include "../lv_draw_triangle_private.h"
#include "lv_draw_sw_gradient_private.h"

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

void lv_draw_sw_triangle(lv_draw_unit_t * draw_unit, const lv_draw_triangle_dsc_t * dsc)
{
#if LV_DRAW_SW_COMPLEX
    lv_area_t tri_area;
    tri_area.x1 = (int32_t)LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y1 = (int32_t)LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    tri_area.x2 = (int32_t)LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y2 = (int32_t)LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    bool is_common;
    lv_area_t draw_area;
    is_common = lv_area_intersect(&draw_area, &tri_area, draw_unit->clip_area);
    if(!is_common) return;

    lv_point_t p[3];
    /*If there is a vertical side use it as p[0] and p[1]*/
    if(dsc->p[0].x == dsc->p[1].x) {
        p[0] = lv_point_from_precise(&dsc->p[0]);
        p[1] = lv_point_from_precise(&dsc->p[1]);
        p[2] = lv_point_from_precise(&dsc->p[2]);
    }
    else if(dsc->p[0].x == dsc->p[2].x) {
        p[0] = lv_point_from_precise(&dsc->p[0]);
        p[1] = lv_point_from_precise(&dsc->p[2]);
        p[2] = lv_point_from_precise(&dsc->p[1]);
    }
    else if(dsc->p[1].x == dsc->p[2].x) {
        p[0] = lv_point_from_precise(&dsc->p[1]);
        p[1] = lv_point_from_precise(&dsc->p[2]);
        p[2] = lv_point_from_precise(&dsc->p[0]);
    }
    else {
        p[0] = lv_point_from_precise(&dsc->p[0]);
        p[1] = lv_point_from_precise(&dsc->p[1]);
        p[2] = lv_point_from_precise(&dsc->p[2]);

        /*Set the smallest y as p[0]*/
        if(p[0].y > p[1].y) lv_point_swap(&p[0], &p[1]);
        if(p[0].y > p[2].y) lv_point_swap(&p[0], &p[2]);

        /*Set the greatest y as p[1]*/
        if(p[1].y < p[2].y) lv_point_swap(&p[1], &p[2]);
    }

    /*Be sure p[0] is on the top*/
    if(p[0].y > p[1].y) lv_point_swap(&p[0], &p[1]);

    /*If right == true p[2] is on the right side of the p[0] p[1] line*/
    bool right = ((p[1].x - p[0].x) * (p[2].y - p[0].y) - (p[1].y - p[0].y) * (p[2].x - p[0].x)) < 0;

    void * masks[4] = {0};
    lv_draw_sw_mask_line_param_t mask_left;
    lv_draw_sw_mask_line_param_t mask_right;
    lv_draw_sw_mask_line_param_t mask_bottom;

    lv_draw_sw_mask_line_points_init(&mask_left, p[0].x, p[0].y,
                                     p[1].x, p[1].y,
                                     right ? LV_DRAW_SW_MASK_LINE_SIDE_RIGHT : LV_DRAW_SW_MASK_LINE_SIDE_LEFT);

    lv_draw_sw_mask_line_points_init(&mask_right, p[0].x, p[0].y,
                                     p[2].x, p[2].y,
                                     right ? LV_DRAW_SW_MASK_LINE_SIDE_LEFT : LV_DRAW_SW_MASK_LINE_SIDE_RIGHT);

    if(p[1].y == p[2].y) {
        lv_draw_sw_mask_line_points_init(&mask_bottom, p[1].x, p[1].y,
                                         p[2].x, p[2].y, LV_DRAW_SW_MASK_LINE_SIDE_TOP);
    }
    else {
        lv_draw_sw_mask_line_points_init(&mask_bottom, p[1].x, p[1].y,
                                         p[2].x, p[2].y,
                                         right ? LV_DRAW_SW_MASK_LINE_SIDE_LEFT  : LV_DRAW_SW_MASK_LINE_SIDE_RIGHT);
    }

    masks[0] = &mask_left;
    masks[1] = &mask_right;
    masks[2] = &mask_bottom;
    int32_t area_w = lv_area_get_width(&draw_area);
    lv_opa_t * mask_buf = lv_malloc(area_w);

    lv_area_t blend_area = draw_area;
    blend_area.y2 = blend_area.y1;
    lv_draw_sw_blend_dsc_t blend_dsc;
    blend_dsc.color = dsc->bg_color;
    blend_dsc.opa = dsc->bg_opa;
    blend_dsc.mask_buf = mask_buf;
    blend_dsc.blend_area = &blend_area;
    blend_dsc.mask_area = &blend_area;
    blend_dsc.blend_mode = LV_BLEND_MODE_NORMAL;
    blend_dsc.src_buf = NULL;

    lv_grad_dir_t grad_dir = dsc->bg_grad.dir;

    lv_grad_t * grad = lv_gradient_get(&dsc->bg_grad, lv_area_get_width(&tri_area), lv_area_get_height(&tri_area));
    lv_opa_t * grad_opa_map = NULL;
    if(grad && grad_dir == LV_GRAD_DIR_HOR) {
        blend_dsc.src_area = &blend_area;
        blend_dsc.src_buf = grad->color_map + draw_area.x1 - tri_area.x1;
        grad_opa_map = grad->opa_map + draw_area.x1 - tri_area.x1;
        blend_dsc.src_color_format = LV_COLOR_FORMAT_RGB888;
    }

    int32_t y;
    for(y = draw_area.y1; y <= draw_area.y2; y++) {
        blend_area.y1 = y;
        blend_area.y2 = y;
        lv_memset(mask_buf, 0xff, area_w);
        blend_dsc.mask_res = lv_draw_sw_mask_apply(masks, mask_buf, draw_area.x1, y, area_w);
        if(grad_dir == LV_GRAD_DIR_VER) {
            LV_ASSERT_NULL(grad);
            blend_dsc.color = grad->color_map[y - tri_area.y1];
            blend_dsc.opa = grad->opa_map[y - tri_area.y1];
            if(dsc->bg_opa < LV_OPA_MAX) blend_dsc.opa = LV_OPA_MIX2(blend_dsc.opa, dsc->bg_opa);
        }
        else if(grad_dir == LV_GRAD_DIR_HOR) {
            if(grad_opa_map) {
                int32_t i;
                if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_CHANGED) {
                    blend_dsc.mask_buf = mask_buf;
                    for(i = 0; i < area_w; i++) {
                        if(grad_opa_map[i] < LV_OPA_MAX) mask_buf[i] = LV_OPA_MIX2(mask_buf[i], grad_opa_map[i]);
                    }
                }
                else if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) {
                    blend_dsc.mask_buf = grad_opa_map;
                    blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                }
                else if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_TRANSP) {
                    continue;
                }
            }
        }
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }

    lv_free(mask_buf);
    lv_draw_sw_mask_free_param(&mask_bottom);
    lv_draw_sw_mask_free_param(&mask_left);
    lv_draw_sw_mask_free_param(&mask_right);

    if(grad) {
        lv_gradient_cleanup(grad);
    }

#else
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    LV_LOG_WARN("Can't draw triangles with LV_DRAW_SW_COMPLEX == 0");
#endif /*LV_DRAW_SW_COMPLEX*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_SW*/
