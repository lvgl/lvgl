/**
 * @file lv_mask.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
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

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


void lv_mask_apply(lv_color_t * dest_buf, lv_color_t * src_buf, lv_opa_t * mask_buf, lv_coord_t len)
{
    lv_coord_t i;
    for(i = 0; i < len; i++) {
        dest_buf[i] = lv_color_mix(src_buf[i], dest_buf[i], mask_buf[i]);
    }
}

void lv_mask_line_left(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, bool inner, void * param)
{
    lv_mask_line_param_t * p = (lv_mask_line_param_t *)param;

    /*make to points absolute to the origo*/
    abs_x -= p->origo.x;
    abs_y -= p->origo.y;

    if(p->flat) {
        lv_coord_t y_at_x;
        /* At the beginning of the mask if the limit line is greater then the mask's y.
         * Then the mask is in the "wrong" area*/
        y_at_x = (int32_t)((int32_t)p->steep * abs_x) >> 10;
        if(y_at_x > abs_y) {
            memset(mask_buf, 0x00, len);
            return;
        }

        /* At the end of the mask if the limit line is smaller then the mask's y.
         * Then the mask is in the "good" area*/
        y_at_x = (int32_t)((int32_t)p->steep * (abs_x + len)) >> 10;
        if(y_at_x < abs_y) {
            return;
        }

        int32_t xe = ((abs_y << 18) / p->steep);
        int32_t xei = xe >> 8;
        int32_t xef = xe & 0xFF;

        int32_t sps = p->steep >> 2;
        int32_t px_h;

        if(xef == 0) px_h = 255;
        else px_h = 255 - (((255 - xef) * sps) >> 8);

        int32_t k = xei - abs_x;
        if(xef) {
            k++;
            if(k >= 0) mask_buf[k] = 255 - (((255-xef) * (255 - px_h)) >> 8);
        }

        while(px_h > sps) {
            if(k >= 0) mask_buf[k] = px_h - (sps >> 1);
            px_h -= sps;
            k++;
            if(k >= len) return;
        }

        if(k < len && k >= 0) {
            mask_buf[k] = px_h  >> 1; /*Approximation*/
        }

        k++;
        if(k < len && k >= 0)
        {
            memset(&mask_buf[k] ,0x00,  len - k);
        }
    } else {
        lv_coord_t x_at_y;
        /* At the beginning of the mask if the limit line is greater then the mask's y.
         * Then the mask is in the "wrong" area*/
        x_at_y = (int32_t)((int32_t)p->steep * abs_y) >> 10;
        if(x_at_y < abs_x) {
            memset(mask_buf, 0x00, len);
            return;
        }

        /* At the end of the mask if the limit line is smaller then the mask's y.
         * Then the mask is in the "good" area*/
        x_at_y = (int32_t)((int32_t)p->steep * (abs_y)) >> 10;
        if(x_at_y > abs_x + len) {
            return;
        }
        int32_t xe = ((abs_y << 8) * p->steep) >> 10;
        int32_t xei = xe >> 8;
        int32_t xef = xe & 0xFF;

        int32_t xq = (((abs_y + 1) << 8) * p->steep) >> 10;
        int32_t xqi = xq >> 8;
        int32_t xqf = xq & 0xFF;

        printf("%d: %d.%d\n", abs_y, xei, xef * 100 / 255);

        int32_t k = xei - abs_x;

        if(xei == xqi) {
            if(k >= 0 && k < len) {
                mask_buf[k] = (xef + xqf) >> 1;
                k++;
            }
        } else {;
            int32_t y_inters = ((255-xef) << 10) / p->steep;
            printf("y: %d, xef: %d, y_inters: %d, %d\n", abs_y, xef, y_inters * 100 / 255);
            mask_buf[k] = 255 - ((y_inters * (255-xef)) >> 8);
            k++;

            int32_t x_inters = ((255-y_inters) * p->steep) >> 10;
            printf("x_inters: %d\n", x_inters * 100 / 255);

            mask_buf[k] = ((255-y_inters) * x_inters) >> 8;
            k++;

        }

        if(k >= 0 && k < len )
        {
            memset(&mask_buf[k] ,0x00,  len - (k));
        }

    }

}




/**********************
 *   STATIC FUNCTIONS
 **********************/
