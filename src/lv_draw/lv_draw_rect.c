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

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = lv_area_intersect(&draw_area, coords, clip);
    if(is_common == false) return;

    const lv_area_t * disp_area = &vdb->area;
    lv_color_t * disp_buf = vdb->buf_act;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    lv_coord_t draw_area_w = lv_area_get_width(&draw_area);

    /*Create a mask if there is a radius*/
    lv_opa_t mask_buf[LV_HOR_RES_MAX];

    uint8_t other_mask_cnt = lv_mask_get_cnt();
    int16_t mask_rout_id = LV_MASK_ID_INV;

    /*Get the real radius*/
    lv_coord_t rout = style->body.radius;
    lv_coord_t short_side = LV_MATH_MIN(lv_area_get_width(coords), lv_area_get_height(coords));
    if(rout > short_side >> 1) rout = short_side >> 1;

    /*Most simple case: just a plain rectangle*/
    if(other_mask_cnt == 0 && rout == 0 && style->body.main_color.full == style->body.grad_color.full) {
        lv_blend_fill(disp_area, clip, coords,
                      disp_buf, LV_IMG_CF_TRUE_COLOR, style->body.main_color,
                      NULL, LV_MASK_RES_FULL_COVER, style->body.opa, LV_BLIT_MODE_NORMAL);
    }
    /*More complex case: there is a radius, gradient or mask.*/
    else {

        lv_mask_param_t mask_rout_param;
        if(rout > 0) {
            lv_mask_radius_init(&mask_rout_param, coords, rout, false);
            mask_rout_id = lv_mask_add(lv_mask_radius, &mask_rout_param, NULL);
        }

        /*Draw the background line by line*/
        lv_coord_t h;
        lv_mask_res_t mask_res = LV_MASK_RES_FULL_COVER;
        lv_color_t grad_color = style->body.main_color;

        /*Fill the first row with 'color'*/
        if(opa >= LV_OPA_MIN) {

            lv_area_t fill_area;
            fill_area.x1 = coords->x1;
            fill_area.x2 = coords->x2;
            fill_area.y1 = disp_area->y1 + draw_area.y1;
            fill_area.y2 = fill_area.y1;
            for(h = draw_area.y1; h <= draw_area.y2; h++) {
                lv_coord_t y = h + vdb->area.y1;

                /*In not corner areas apply the mask only if required*/
                if(y > coords->y1 + rout + 1 &&
                   y < coords->y2 - rout - 1) {
                    mask_res = LV_MASK_RES_FULL_COVER;
                    if(other_mask_cnt != 0) {
                        memset(mask_buf, LV_OPA_COVER, draw_area_w);
                        mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);
                    }
                }
                /*In corner areas apply the mask anyway*/
                else {
                    memset(mask_buf, LV_OPA_COVER, draw_area_w);
                    mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);
                }

                /*Get the current line color*/
                if(style->body.main_color.full != style->body.grad_color.full) {
                    lv_opa_t mix = (uint32_t)((uint32_t) (y - coords->y1) * 255) / lv_area_get_height(coords);
                    grad_color = lv_color_mix(style->body.grad_color, style->body.main_color, mix);
                }

                lv_blend_fill(disp_area, clip, &fill_area,
                        disp_buf,  LV_IMG_CF_TRUE_COLOR, grad_color,
                        mask_buf, mask_res, style->body.opa, LV_BLIT_MODE_NORMAL);

                fill_area.y1++;
                fill_area.y2++;
            }
        }
    }

    /*Draw the border if any*/
    lv_coord_t border_width = style->body.border.width;
    if(border_width) {
        /*Move the vdb_buf_tmp to the first row*/
        lv_mask_param_t mask_rsmall_param;

        /*Get the inner radius*/
        lv_coord_t rin = rout - border_width;
        if(rin < 0) rin = 0;

        /*Get the inner area*/
        lv_area_t area_small;
        lv_area_copy(&area_small, coords);
        area_small.x1 += border_width;
        area_small.x2 -= border_width;
        area_small.y1 += border_width;
        area_small.y2 -= border_width;

        /*Create the mask*/
        lv_mask_radius_init(&mask_rsmall_param, &area_small, rout - border_width, true);
        int16_t mask_rsmall_id = lv_mask_add(lv_mask_radius, &mask_rsmall_param, NULL);

        lv_coord_t corner_size = LV_MATH_MAX(rout, border_width);

        lv_coord_t h;
        lv_mask_res_t mask_res;
        lv_area_t fill_area;

        /*Apply some optimization if there is no other mask*/
        if(other_mask_cnt == 0) {
            /*Draw the upper corner area*/
            lv_coord_t upper_corner_end = coords->y1 - disp_area->y1 + corner_size;
            fill_area.x1 = coords->x1;
            fill_area.x2 = coords->x2;
            fill_area.y1 = disp_area->y1 + draw_area.y1;
            fill_area.y2 = fill_area.y1;
            for(h = draw_area.y1; h <= upper_corner_end; h++) {
                memset(mask_buf, LV_OPA_COVER, draw_area_w);
                mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

                lv_blend_fill(disp_area, clip, &fill_area,
                        disp_buf,  LV_IMG_CF_TRUE_COLOR, style->body.border.color,
                        mask_buf, mask_res, style->body.border.opa, LV_BLIT_MODE_NORMAL);

                fill_area.y1++;
                fill_area.y2++;

            }

            /*Draw the lowe corner area corner area*/
            lv_coord_t lower_corner_end = coords->y2 - disp_area->y1 - corner_size;
            fill_area.y1 = disp_area->y1 + lower_corner_end;
            fill_area.y2 = fill_area.y1;
            for(h = lower_corner_end; h <= draw_area.y2; h++) {
               memset(mask_buf, LV_OPA_COVER, draw_area_w);
               mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

               lv_blend_fill(disp_area, clip, &fill_area,
                       disp_buf,  LV_IMG_CF_TRUE_COLOR, style->body.border.color,
                       mask_buf, mask_res, style->body.border.opa, LV_BLIT_MODE_NORMAL);

               fill_area.y1++;
               fill_area.y2++;
           }

           /*Draw the left vertical border part*/
            fill_area.x1 = coords->x1;
            fill_area.x2 = coords->x1 + border_width - 1;
            fill_area.y1 = coords->y1 + corner_size + 1;
            fill_area.y2 = coords->y2 - corner_size - 1;

            lv_blend_fill(disp_area, clip, &fill_area,
                    disp_buf,  LV_IMG_CF_TRUE_COLOR, style->body.border.color,
                    NULL, LV_MASK_RES_FULL_COVER, style->body.border.opa, LV_BLIT_MODE_NORMAL);

            fill_area.x1 = coords->x2 - border_width + 1;
            fill_area.x2 = coords->x2;

            lv_blend_fill(disp_area, clip, &fill_area,
                       disp_buf,  LV_IMG_CF_TRUE_COLOR, style->body.border.color,
                       NULL, LV_MASK_RES_FULL_COVER, style->body.border.opa, LV_BLIT_MODE_NORMAL);
        }
        /*Process line by line if there is other mask too*/
        else {
            fill_area.x1 = coords->x1;
            fill_area.x2 = coords->x2;
            fill_area.y1 = disp_area->y1 + draw_area.y1;
            fill_area.y2 = fill_area.y1;
            for(h = draw_area.y1; h <= draw_area.y2; h++) {
                memset(mask_buf, LV_OPA_COVER, draw_area_w);
                mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

                lv_blend_fill(disp_area, clip, &fill_area,
                        disp_buf,  LV_IMG_CF_TRUE_COLOR, style->body.border.color,
                        mask_buf, mask_res, style->body.border.opa, LV_BLIT_MODE_NORMAL);

                fill_area.y1++;
                fill_area.y2++;

            }
        }
        lv_mask_remove_id(mask_rsmall_id);
    }

    lv_mask_remove_id(mask_rout_id);
}
