/**
 * @file lv_draw_sw_border.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW

#include "blend/lv_draw_sw_blend.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_text_ap.h"
#include "../../core/lv_refr.h"
#include "../../misc/lv_assert.h"
#include "../../stdlib/lv_string.h"
#include "../lv_draw_mask.h"

/*********************
 *      DEFINES
 *********************/
#define SPLIT_LIMIT             50

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void draw_border_complex(lv_draw_unit_t * draw_unit, const lv_area_t * outer_area, const lv_area_t * inner_area,
                                int32_t rout, int32_t rin, lv_color_t color, lv_opa_t opa);

static void draw_border_simple(lv_draw_unit_t * draw_unit, const lv_area_t * outer_area, const lv_area_t * inner_area,
                               lv_color_t color, lv_opa_t opa);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_border(lv_draw_unit_t * draw_unit, const lv_draw_border_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(dsc->side == LV_BORDER_SIDE_NONE) return;

    int32_t coords_w = lv_area_get_width(coords);
    int32_t coords_h = lv_area_get_height(coords);
    int32_t rout = dsc->radius;
    int32_t short_side = LV_MIN(coords_w, coords_h);
    if(rout > short_side >> 1) rout = short_side >> 1;

    /*Get the inner area*/
    lv_area_t area_inner;
    lv_area_copy(&area_inner, coords);
    area_inner.x1 += ((dsc->side & LV_BORDER_SIDE_LEFT) ? dsc->width : - (dsc->width + rout));
    area_inner.x2 -= ((dsc->side & LV_BORDER_SIDE_RIGHT) ? dsc->width : - (dsc->width + rout));
    area_inner.y1 += ((dsc->side & LV_BORDER_SIDE_TOP) ? dsc->width : - (dsc->width + rout));
    area_inner.y2 -= ((dsc->side & LV_BORDER_SIDE_BOTTOM) ? dsc->width : - (dsc->width + rout));

    int32_t rin = rout - dsc->width;
    if(rin < 0) rin = 0;

    if(rout == 0 && rin == 0) {
        draw_border_simple(draw_unit, coords, &area_inner, dsc->color, dsc->opa);
    }
    else {
        draw_border_complex(draw_unit, coords, &area_inner, rout, rin, dsc->color, dsc->opa);
    }

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

void draw_border_complex(lv_draw_unit_t * draw_unit, const lv_area_t * outer_area, const lv_area_t * inner_area,
                         int32_t rout, int32_t rin, lv_color_t color, lv_opa_t opa)
{
#if LV_DRAW_SW_COMPLEX
    /*Get clipped draw area which is the real draw area.
     *It is always the same or inside `coords`*/
    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, outer_area, draw_unit->clip_area)) return;
    int32_t draw_area_w = lv_area_get_width(&draw_area);

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memzero(&blend_dsc, sizeof(blend_dsc));
    lv_opa_t * mask_buf = lv_malloc(draw_area_w);
    blend_dsc.mask_buf = mask_buf;

    void * mask_list[3] = {0};

    /*Create mask for the inner mask*/
    lv_draw_sw_mask_radius_param_t mask_rin_param;
    lv_draw_sw_mask_radius_init(&mask_rin_param, inner_area, rin, true);
    mask_list[0] = &mask_rin_param;

    /*Create mask for the outer area*/
    lv_draw_sw_mask_radius_param_t mask_rout_param;
    if(rout > 0) {
        lv_draw_sw_mask_radius_init(&mask_rout_param, outer_area, rout, false);
        mask_list[1] = &mask_rout_param;
    }

    int32_t h;
    lv_area_t blend_area;
    blend_dsc.blend_area = &blend_area;
    blend_dsc.mask_area = &blend_area;
    blend_dsc.color = color;
    blend_dsc.opa = opa;

    /*Calculate the x and y coordinates where the straight parts area is*/
    lv_area_t core_area;
    core_area.x1 = LV_MAX(outer_area->x1 + rout, inner_area->x1);
    core_area.x2 = LV_MIN(outer_area->x2 - rout, inner_area->x2);
    core_area.y1 = LV_MAX(outer_area->y1 + rout, inner_area->y1);
    core_area.y2 = LV_MIN(outer_area->y2 - rout, inner_area->y2);
    int32_t core_w = lv_area_get_width(&core_area);

    bool top_side = outer_area->y1 <= inner_area->y1;
    bool bottom_side = outer_area->y2 >= inner_area->y2;

    /*No masks*/
    bool left_side = outer_area->x1 <= inner_area->x1;
    bool right_side = outer_area->x2 >= inner_area->x2;

    bool split_hor = true;
    if(left_side && right_side && top_side && bottom_side &&
       core_w < SPLIT_LIMIT) {
        split_hor = false;
    }

    blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_FULL_COVER;
    /*Draw the straight lines first if they are long enough*/
    if(top_side && split_hor) {
        blend_area.x1 = core_area.x1;
        blend_area.x2 = core_area.x2;
        blend_area.y1 = outer_area->y1;
        blend_area.y2 = inner_area->y1 - 1;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }

    if(bottom_side && split_hor) {
        blend_area.x1 = core_area.x1;
        blend_area.x2 = core_area.x2;
        blend_area.y1 = inner_area->y2 + 1;
        blend_area.y2 = outer_area->y2;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }

    /*If the border is very thick and the vertical sides overlap horizontally draw a single rectangle*/
    if(inner_area->x1 >= inner_area->x2 && left_side && right_side) {
        blend_area.x1 = outer_area->x1;
        blend_area.x2 = outer_area->x2;
        blend_area.y1 = core_area.y1;
        blend_area.y2 = core_area.y2;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    else {
        if(left_side) {
            blend_area.x1 = outer_area->x1;
            blend_area.x2 = inner_area->x1 - 1;
            blend_area.y1 = core_area.y1;
            blend_area.y2 = core_area.y2;
            lv_draw_sw_blend(draw_unit, &blend_dsc);
        }

        if(right_side) {
            blend_area.x1 = inner_area->x2 + 1;
            blend_area.x2 = outer_area->x2;
            blend_area.y1 = core_area.y1;
            blend_area.y2 = core_area.y2;
            lv_draw_sw_blend(draw_unit, &blend_dsc);
        }
    }

    /*Draw the corners*/
    int32_t blend_w;

    /*Left and right corner together if they are close to each other*/
    if(!split_hor) {
        /*Calculate the top corner and mirror it to the bottom*/
        blend_area.x1 = draw_area.x1;
        blend_area.x2 = draw_area.x2;
        int32_t max_h = LV_MAX(rout, inner_area->y1 - outer_area->y1);
        for(h = 0; h < max_h; h++) {
            int32_t top_y = outer_area->y1 + h;
            int32_t bottom_y = outer_area->y2 - h;
            if(top_y < draw_area.y1 && bottom_y > draw_area.y2) continue;   /*This line is clipped now*/

            lv_memset(mask_buf, 0xff, draw_area_w);
            blend_dsc.mask_res = lv_draw_sw_mask_apply(mask_list, mask_buf, blend_area.x1, top_y, draw_area_w);

            if(top_y >= draw_area.y1) {
                blend_area.y1 = top_y;
                blend_area.y2 = top_y;
                lv_draw_sw_blend(draw_unit, &blend_dsc);
            }

            if(bottom_y <= draw_area.y2) {
                blend_area.y1 = bottom_y;
                blend_area.y2 = bottom_y;
                lv_draw_sw_blend(draw_unit, &blend_dsc);
            }
        }
    }
    else {
        /*Left corners*/
        blend_area.x1 = draw_area.x1;
        blend_area.x2 = LV_MIN(draw_area.x2, core_area.x1 - 1);
        blend_w = lv_area_get_width(&blend_area);
        if(blend_w > 0) {
            if(left_side || top_side) {
                for(h = draw_area.y1; h < core_area.y1; h++) {
                    blend_area.y1 = h;
                    blend_area.y2 = h;

                    lv_memset(mask_buf, 0xff, blend_w);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(mask_list, mask_buf, blend_area.x1, h, blend_w);
                    lv_draw_sw_blend(draw_unit, &blend_dsc);
                }
            }

            if(left_side || bottom_side) {
                for(h = core_area.y2 + 1; h <= draw_area.y2; h++) {
                    blend_area.y1 = h;
                    blend_area.y2 = h;

                    lv_memset(mask_buf, 0xff, blend_w);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(mask_list, mask_buf, blend_area.x1, h, blend_w);
                    lv_draw_sw_blend(draw_unit, &blend_dsc);
                }
            }
        }

        /*Right corners*/
        blend_area.x1 = LV_MAX(draw_area.x1, blend_area.x2 + 1);    /*To not overlap with the left side*/
        blend_area.x1 = LV_MAX(draw_area.x1, core_area.x2 + 1);

        blend_area.x2 = draw_area.x2;
        blend_w = lv_area_get_width(&blend_area);

        if(blend_w > 0) {
            if(right_side || top_side) {
                for(h = draw_area.y1; h < core_area.y1; h++) {
                    blend_area.y1 = h;
                    blend_area.y2 = h;

                    lv_memset(mask_buf, 0xff, blend_w);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(mask_list, mask_buf, blend_area.x1, h, blend_w);
                    lv_draw_sw_blend(draw_unit, &blend_dsc);
                }
            }

            if(right_side || bottom_side) {
                for(h = core_area.y2 + 1; h <= draw_area.y2; h++) {
                    blend_area.y1 = h;
                    blend_area.y2 = h;

                    lv_memset(mask_buf, 0xff, blend_w);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(mask_list, mask_buf, blend_area.x1, h, blend_w);
                    lv_draw_sw_blend(draw_unit, &blend_dsc);
                }
            }
        }
    }

    lv_draw_sw_mask_free_param(&mask_rin_param);
    if(rout > 0) lv_draw_sw_mask_free_param(&mask_rout_param);
    lv_free(mask_buf);

#endif /*LV_DRAW_SW_COMPLEX*/
}
static void draw_border_simple(lv_draw_unit_t * draw_unit, const lv_area_t * outer_area, const lv_area_t * inner_area,
                               lv_color_t color, lv_opa_t opa)
{
    lv_area_t a;
    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memzero(&blend_dsc, sizeof(lv_draw_sw_blend_dsc_t));
    blend_dsc.blend_area = &a;
    blend_dsc.color = color;
    blend_dsc.opa = opa;

    bool top_side = outer_area->y1 <= inner_area->y1;
    bool bottom_side = outer_area->y2 >= inner_area->y2;
    bool left_side = outer_area->x1 <= inner_area->x1;
    bool right_side = outer_area->x2 >= inner_area->x2;

    /*Top*/
    a.x1 = outer_area->x1;
    a.x2 = outer_area->x2;
    a.y1 = outer_area->y1;
    a.y2 = inner_area->y1 - 1;
    if(top_side) {
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }

    /*Bottom*/
    a.y1 = inner_area->y2 + 1;
    a.y2 = outer_area->y2;
    if(bottom_side) {
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }

    /*Left*/
    a.x1 = outer_area->x1;
    a.x2 = inner_area->x1 - 1;
    a.y1 = (top_side) ? inner_area->y1 : outer_area->y1;
    a.y2 = (bottom_side) ? inner_area->y2 : outer_area->y2;
    if(left_side) {
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }

    /*Right*/
    a.x1 = inner_area->x2 + 1;
    a.x2 = outer_area->x2;
    if(right_side) {
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
}

#endif /*LV_USE_DRAW_SW*/
