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

    lv_coord_t vdb_width       = lv_area_get_width(&vdb->area);
    lv_coord_t draw_a_width    = lv_area_get_width(&draw_rel_a);

    /*Move the vdb_buf_tmp to the first row*/
    lv_color_t * vdb_buf_tmp = vdb->buf_act;
    vdb_buf_tmp += vdb_width * draw_rel_a.y1;

    lv_opa_t mask_buf[LV_HOR_RES_MAX];
    lv_coord_t short_side = LV_MATH_MIN(lv_area_get_width(coords), lv_area_get_height(coords));
    lv_coord_t rout = style->body.radius;
    if(rout > short_side >> 1) rout = short_side >> 1;

    /*Create the outer mask*/
    lv_mask_param_t mask_rout_param;
    int16_t mask_rout_id = LV_MASK_ID_INV;

    uint8_t other_mask_cnt = lv_mask_get_cnt();

    if(rout != 0)  {
        lv_mask_radius_init(&mask_rout_param, coords, rout, false);
        mask_rout_id = lv_mask_add(lv_mask_radius, &mask_rout_param, NULL);
    }

    /*Draw the background line by line*/
    lv_coord_t h;
    lv_mask_res_t mask_res = LV_MASK_RES_FULL_COVER;
    lv_color_t grad_color = style->body.main_color;

    /*Fill the first row with 'color'*/
    if(opa >= LV_OPA_MIN) {
        for(h = draw_rel_a.y1; h <= draw_rel_a.y2; h++) {
            lv_coord_t y = h + vdb->area.y1;
            if(y > coords->y1 + rout + 1 &&
               y < coords->y2 - rout - 1) {
                mask_res = LV_MASK_RES_FULL_COVER;
                if(other_mask_cnt != 0) {
                    memset(mask_buf, LV_OPA_COVER, draw_a_width);
                    mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_rel_a.x1, vdb->area.y1 + h, draw_a_width);
                }
            } else {
                memset(mask_buf, LV_OPA_COVER, draw_a_width);
                mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_rel_a.x1, vdb->area.y1 + h, draw_a_width);
            }

            if(style->body.main_color.full != style->body.grad_color.full) {
                lv_opa_t mix = (uint32_t)((uint32_t) (y - coords->y1) * 255) / lv_area_get_height(coords);
                grad_color = lv_color_mix(style->body.grad_color, style->body.main_color, mix);
            }

            lv_blend_color(&vdb_buf_tmp[draw_rel_a.x1], LV_IMG_CF_TRUE_COLOR, draw_a_width,
                    grad_color, LV_IMG_CF_TRUE_COLOR,
                    mask_buf, mask_res, style->body.opa, LV_BLIT_MODE_NORMAL);

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

        lv_coord_t len_left = (coords->x1 + border_width) - (vdb->area.x1 + draw_rel_a.x1);
        if(draw_rel_a.x1 + len_left > draw_rel_a.x2) {
            len_left = draw_rel_a.x2 - draw_rel_a.x1 + 1;
        }

        lv_coord_t first_right = coords->x2 - (vdb->area.x1 + draw_rel_a.x1 + border_width - 1);
        if(first_right < 0) first_right = 0;
        lv_coord_t len_right = draw_a_width - first_right;

        lv_coord_t corner_size = LV_MATH_MAX(rout, border_width) + 1;

        /*Fill the first row with 'color'*/
        for(h = draw_rel_a.y1; h <= draw_rel_a.y2; h++) {
            /* Do not blend the large empty area in the middle.
             * Truncate the mask to use only the very edges*/
            lv_coord_t y = h + vdb->area.y1;
            if(y > coords->y1 + corner_size &&
               y < coords->y2 - corner_size) {
                mask_res = LV_MASK_RES_FULL_COVER;
                if(other_mask_cnt != 0) {
                    memset(mask_buf, LV_OPA_COVER, draw_a_width);
                    mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_rel_a.x1, vdb->area.y1 + h, draw_a_width);
                }

                /* It's sure that we don't need more then border_width pixels on the left.*/
                if(len_left > 0) {
                    lv_blend_color(&vdb_buf_tmp[draw_rel_a.x1], LV_IMG_CF_TRUE_COLOR, len_left,
                                            style->body.border.color, LV_IMG_CF_TRUE_COLOR,
                                            mask_buf, mask_res, style->body.border.opa, LV_BLIT_MODE_NORMAL);
                }
                /* Similarly we don't need more then border_width pixels on the right.*/
                if(len_right > 0) {
                    lv_blend_color(&vdb_buf_tmp[draw_rel_a.x1 + first_right], LV_IMG_CF_TRUE_COLOR, len_right,
                                            style->body.border.color, LV_IMG_CF_TRUE_COLOR,
                                            mask_buf + first_right, mask_res, style->body.border.opa, LV_BLIT_MODE_NORMAL);
                }
            }
            else {
                memset(mask_buf, LV_OPA_COVER, draw_a_width);
                mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_rel_a.x1, vdb->area.y1 + h, draw_a_width);

                lv_blend_color(&vdb_buf_tmp[draw_rel_a.x1], LV_IMG_CF_TRUE_COLOR, draw_a_width,
                        style->body.border.color, LV_IMG_CF_TRUE_COLOR,
                        mask_buf, mask_res, style->body.border.opa, LV_BLIT_MODE_NORMAL);
            }
            vdb_buf_tmp += vdb_width;
        }

        lv_mask_remove_id(mask_rsmall_id);
    }

    lv_mask_remove_id(mask_rout_id);
}
