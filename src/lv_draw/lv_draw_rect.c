/**
 * @file lv_draw_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl/src/lv_draw/lv_blend.h>
#include "lv_draw_rect.h"
#include "../lv_misc/lv_circ.h"
#include "../lv_misc/lv_math.h"
#include "../lv_core/lv_refr.h"
#include "lv_mask.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void draw_bg(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Draw a rectangle
 * @param coords the coordinates of the rectangle
 * @param mask the rectangle will be drawn only in this mask
 * @param style pointer to a style
 * @param opa_scale scale down all opacities by the factor
 */
void lv_draw_rect(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale)
{
    if(lv_area_get_height(coords) < 1 || lv_area_get_width(coords) < 1) return;


    draw_bg(coords, clip, style, opa_scale);


}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_bg(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_opa_t opa = style->body.opa;

    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    lv_area_t draw_a;
    bool union_ok;

    /* Get the union of `coords` and `clip`*/
    /* `clip` is already truncated to the `vdb` size
     * in 'lv_refr_area' function */
    union_ok = lv_area_intersect(&draw_a, coords, clip);

    /*If there are common part of `clip` and `vdb` then draw*/
    if(union_ok == false) return;

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    /*Store the coordinates of the `draw_a` relative to the VDB */
    lv_area_t draw_rel_a;
    draw_rel_a.x1 = draw_a.x1 - vdb->area.x1;
    draw_rel_a.y1 = draw_a.y1 - vdb->area.y1;
    draw_rel_a.x2 = draw_a.x2 - vdb->area.x1;
    draw_rel_a.y2 = draw_a.y2 - vdb->area.y1;

    uint32_t vdb_width       = lv_area_get_width(&vdb->area);
    uint32_t draw_a_width    = lv_area_get_width(&draw_rel_a);

    /*Move the vdb_buf_tmp to the first row*/
    lv_color_t * vdb_buf_tmp = vdb->buf_act;
    vdb_buf_tmp += vdb_width * draw_rel_a.y1;

    lv_opa_t mask_buf[LV_HOR_RES_MAX];
    lv_coord_t short_side = LV_MATH_MIN(lv_area_get_width(coords), lv_area_get_height(coords));
    lv_coord_t rout = style->body.radius;
    if(rout > short_side >> 1) rout = short_side >> 1;

    /*Create the outer mask*/
    lv_mask_param_t mask_rout_param;
    lv_mask_radius_init(&mask_rout_param, coords, rout, false);
    int16_t mask_rout_id = lv_mask_add(lv_mask_radius, &mask_rout_param, NULL);

    lv_area_t t;
    t.x1 = 10;
    t.y1 = 20;
    t.x2 = 30;
    t.y2 = 50;
    lv_mask_param_t tp;
    lv_mask_radius_init(&tp, &t, 5, false);
//    int16_t tpid = lv_mask_add(lv_mask_radius, &tp, NULL);

    /*Draw the background line by line*/
    lv_coord_t h;
    lv_mask_res_t mask_res;
    lv_color_t grad_color;


    /*Fill the first row with 'color'*/
    if(opa >= LV_OPA_MIN) {
        for(h = draw_rel_a.y1; h <= draw_rel_a.y2; h++) {

            lv_opa_t mix = ((uint32_t)((uint32_t)h + vdb->area.y1-coords->y1)*255) / lv_area_get_height(coords);
            grad_color = lv_color_mix(style->body.grad_color, style->body.main_color, mix);

            if(style->body.main_color.full != style->body.grad_color.full) {
                memset(mask_buf, LV_OPA_COVER, draw_a_width);
                mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_rel_a.x1, vdb->area.y1 + h, draw_a_width);

                lv_blend_color(&vdb_buf_tmp[draw_rel_a.x1], LV_IMG_CF_TRUE_COLOR, draw_a_width,
                        grad_color, LV_IMG_CF_TRUE_COLOR,
                        mask_buf, mask_res, style->body.opa, LV_BLIT_MODE_NORMAL);
            } else {
                lv_blend_color(&vdb_buf_tmp[draw_rel_a.x1], LV_IMG_CF_TRUE_COLOR, draw_a_width,
                                    style->body.main_color, LV_IMG_CF_TRUE_COLOR,
                                    NULL, LV_MASK_RES_FULL_COVER, style->body.opa, LV_BLIT_MODE_NORMAL);
            }

            vdb_buf_tmp += vdb_width;
        }
    }


    /*Draw the border if any*/
    lv_coord_t border_width = style->body.border.width;
    if(border_width) {
        /*Move the vdb_buf_tmp to the first row*/
        vdb_buf_tmp = vdb->buf_act;
        vdb_buf_tmp += vdb_width * draw_rel_a.y1;

        lv_mask_param_t mask_rsmall_param;
        lv_coord_t rin = rout - border_width;
        if(rin < 0) rin = 0;
        lv_area_t area_small;
        lv_area_copy(&area_small, coords);
        area_small.x1 += border_width;
        area_small.x2 -= border_width;
        area_small.y1 += border_width;
        area_small.y2 -= border_width;
        lv_mask_radius_init(&mask_rsmall_param, &area_small, style->body.radius - border_width, true);
        int16_t mask_rsmall_id = lv_mask_add(lv_mask_radius, &mask_rsmall_param, NULL);

        /*Fill the first row with 'color'*/
        for(h = draw_rel_a.y1; h <= draw_rel_a.y2; h++) {
            memset(mask_buf, LV_OPA_COVER, draw_a_width);
            mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_rel_a.x1, vdb->area.y1 + h, draw_a_width);

            lv_blend_color(&vdb_buf_tmp[draw_rel_a.x1], LV_IMG_CF_TRUE_COLOR, draw_a_width,
                    style->body.border.color, LV_IMG_CF_TRUE_COLOR,
                    mask_buf, mask_res, style->body.border.opa, LV_BLIT_MODE_NORMAL);

            vdb_buf_tmp += vdb_width;
        }

        lv_mask_remove_id(mask_rsmall_id);
    }

    lv_mask_remove_id(mask_rout_id);
//    lv_mask_remove_id(tpid);
}
