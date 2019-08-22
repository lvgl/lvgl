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
