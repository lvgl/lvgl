/**
 * @file lv_blend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_blend.h"

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


/**
 *
 * @param disp_area
 * @param clip_area already truncated to disp_arae
 * @param fill_area
 * @param disp_buf
 * @param cf
 * @param color
 * @param mask
 * @param mask_res
 * @param opa
 * @param mode
 */
void lv_blend_fill(const lv_area_t * disp_area, const lv_area_t * clip_area, const lv_area_t * fill_area,
        lv_color_t * disp_buf, lv_img_cf_t cf, lv_color_t color,
        lv_opa_t * mask, lv_mask_res_t mask_res, lv_opa_t opa, lv_blend_mode_t mode)
{

    /*Do not draw transparent things*/
    if(opa < LV_OPA_MIN) return;
    if(mask_res == LV_MASK_RES_FULL_TRANSP) return;

    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = lv_area_intersect(&draw_area, clip_area, fill_area);
    if(!is_common) return;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    /*Get the width of the `disp_area` it will be used to go to the next line*/
    lv_coord_t disp_w = lv_area_get_width(disp_area);

    /*Create a temp. disp_buf which always point to current line to draw*/
    lv_color_t * disp_buf_tmp = disp_buf + disp_w * draw_area.y1;

    lv_coord_t x;
    lv_coord_t y;

    /*Simple fill (maybe with opacity), no masking*/
    if(mask_res == LV_MASK_RES_FULL_COVER) {
        if(opa > LV_OPA_MAX) {
            for(y = draw_area.y1; y <= draw_area.y2; y++) {
                for(x = draw_area.x1; x <= draw_area.x2; x++) {
                    disp_buf_tmp[x].full = color.full;
                }
                disp_buf_tmp += disp_w;
            }
        }
        else {
            for(y = draw_area.y1; y <= draw_area.y2; y++) {
               for(x = draw_area.x1; x <= draw_area.x2; x++) {
                   disp_buf_tmp[x] = lv_color_mix(color, disp_buf[x], opa);
               }
               disp_buf_tmp += disp_w;
           }
        }
    }
    /*Masked*/
    else {
        /*Get the width of the `fill_area` it will be used to go to the next line of the mask*/
        lv_coord_t fill_w = lv_area_get_width(fill_area);

        /* The mask is relative to the original `fill_area`.
         * If some lines and columns are clipped move on the mask accordingly.*/
        lv_opa_t * mask_tmp = mask + fill_w * (draw_area.y1 - fill_area->y1 + disp_area->y1);
        mask_tmp -= draw_area.x1;

        /*Buffer the result color to avoid recalculating the same color*/
        lv_color_t last_dest_color;
        lv_color_t last_res_color;
        lv_opa_t last_mask = LV_OPA_TRANSP;
        last_dest_color.full = disp_buf_tmp[0].full;
        last_res_color.full = disp_buf_tmp[0].full;

        /*Only the mask matters*/
        if(opa > LV_OPA_MAX) {
            for(y = draw_area.y1; y <= draw_area.y2; y++) {
               for(x = draw_area.x1; x <= draw_area.x2; x++) {
                    if(mask_tmp[x] != last_mask || last_dest_color.full != disp_buf_tmp[x].full) {
                        if(mask_tmp[x] > LV_OPA_MAX) last_res_color = color;
                        else if(mask_tmp[x] < LV_OPA_MIN) last_res_color = disp_buf_tmp[x];
                        else last_res_color = lv_color_mix(color, disp_buf_tmp[x], mask_tmp[x]);
                        last_mask = mask_tmp[x];
                        last_dest_color.full = disp_buf_tmp[x].full;
                    }
                    disp_buf_tmp[x] = last_res_color;
               }
               disp_buf_tmp += disp_w;
               mask_tmp += fill_w;
            }
        }
        /*Handle opa and mask values too*/
        else {
            for(y = draw_area.y1; y <= draw_area.y2; y++) {
               for(x = draw_area.x1; x <= draw_area.x2; x++) {
                    if(mask[x] != last_mask || last_dest_color.full != disp_buf_tmp[x].full) {
                        lv_opa_t opa_tmp = (uint16_t)((uint16_t)mask_tmp[x] * opa) >> 8;

                        if(opa_tmp > LV_OPA_MAX) last_res_color = color;
                        else if(opa_tmp < LV_OPA_MIN) last_res_color = disp_buf_tmp[x];
                        else last_res_color = lv_color_mix(color, disp_buf_tmp[x],opa_tmp);
                        last_mask = mask[x];
                        last_dest_color.full = disp_buf_tmp[x].full;
                    }
                    disp_buf_tmp[x] = last_res_color;
               }
               disp_buf_tmp += disp_w;
               mask_tmp += fill_w;
            }
        }
    }
}







void lv_blend_color(lv_color_t * dest_buf, lv_img_cf_t dest_cf, lv_coord_t len,
        lv_color_t color, lv_img_cf_t src_cf,
        lv_opa_t * mask, lv_mask_res_t mask_res, lv_opa_t opa, lv_blend_mode_t mode)
{
    if(opa < LV_OPA_MIN) return;
    if(mask_res == LV_MASK_RES_FULL_TRANSP) return;

    lv_coord_t i;
    if(dest_cf == LV_IMG_CF_TRUE_COLOR && src_cf == LV_IMG_CF_TRUE_COLOR) {
        /*Simple fill (maybe with opacity), no masking*/
        if(mask_res == LV_MASK_RES_FULL_COVER) {
            if(opa > LV_OPA_MAX) {
                for(i = 0; i < len; i++) {
                    dest_buf[i].full = color.full;
                }
            }
            else {
                for(i = 0; i < len; i++) {
                    dest_buf[i] = lv_color_mix(color, dest_buf[i], opa);
                }
            }
        }
        /*Masked*/
        else {
            /*Only the mask matters*/
            if(opa > LV_OPA_MAX) {
                lv_color_t last_dest_color;
                lv_color_t last_res_color;
                lv_opa_t last_mask = LV_OPA_TRANSP;
                last_dest_color.full = dest_buf[0].full;
                last_res_color.full = dest_buf[0].full;

                for(i = 0; i < len; i++) {
                    //                    if(mask[i] == 0) continue;

                    if(mask[i] != last_mask || last_dest_color.full != dest_buf[i].full) {
                        if(mask[i] > LV_OPA_MAX) last_res_color = color;
                        else if(mask[i] < LV_OPA_MIN) last_res_color = dest_buf[i];
                        else last_res_color = lv_color_mix(color, dest_buf[i], mask[i]);
                        last_mask = mask[i];
                        last_dest_color.full = dest_buf[i].full;
                    }
                    dest_buf[i] = last_res_color;
                }
            } else {
                lv_color_t last_dest_color;
                lv_color_t last_res_color;
                lv_opa_t last_mask = LV_OPA_TRANSP;
                last_dest_color.full = dest_buf[0].full;
                last_res_color.full = dest_buf[0].full;
                for(i = 0; i < len; i++) {
                    //                    if(mask[i] == 0) continue;

                    if(mask[i] != last_mask || last_dest_color.full != dest_buf[i].full) {
                        lv_opa_t tmp = (uint16_t)((uint16_t)mask[i] * opa) >> 8;
                        if(tmp > LV_OPA_MAX) last_res_color = color;
                        else if(tmp < LV_OPA_MIN) last_res_color = dest_buf[i];
                        else last_res_color = lv_color_mix(color, dest_buf[i], tmp);
                        last_mask = mask[i];
                        last_dest_color.full = dest_buf[i].full;
                    }
                    dest_buf[i] = last_res_color;
                    //
                }
            }
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
