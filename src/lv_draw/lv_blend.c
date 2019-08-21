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
                for(i = 0; i < len; i++) {
                    dest_buf[i] = lv_color_mix(color, dest_buf[i], mask[i]);
                }
            } else {
                for(i = 0; i < len; i++) {
                    if(mask[i] > LV_OPA_MAX) {
                        dest_buf[i] = lv_color_mix(color, dest_buf[i], opa);
                    } else {
                        dest_buf[i] = lv_color_mix(color, dest_buf[i], (mask[i] * opa) >> 8);
                    }
                }
            }
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
