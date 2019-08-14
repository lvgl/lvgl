/**
 * @file lv_mask.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_mask.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void line_mask_flat(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_mask_line_param_t * p);
static void line_mask_steep(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_mask_line_param_t * p);

static lv_opa_t mask_mix(lv_opa_t mask_act, lv_opa_t mask_new);

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


void lv_mask_line_points_init(lv_mask_line_param_t * p, lv_coord_t p1x, lv_coord_t p1y, lv_coord_t p2x, lv_coord_t p2y, lv_line_mask_side_t side)
{
    memset(p, 0x00, sizeof(lv_mask_line_param_t));
    if(p1y > p2y) {
        lv_coord_t t;
        t = p2x;
        p2x = p1x;
        p1x = t;

        t = p2y;
        p2y = p1y;
        p1y = t;
    }


    p->origo.x = p1x;
    p->origo.y = p1y;
    p->side = side;
    p->flat = (LV_MATH_ABS(p2x-p1x) > LV_MATH_ABS(p2y-p1y)) ? 1 : 0;
    if(p->flat) {
        /*Normalize the steep. Delta x should be relative to delta x = 1024*/
        int32_t m;
        m = (1 << 20) / (p2x-p1x);  /*m is multiplier to normalize y (upscaled by 1024)*/
        p->steep = (m * (p2y-p1y)) >> 10;
    } else {
        /*Normalize the steep. Delta y should be relative to delta x = 1024*/
        int32_t m;
        m = (1 << 20) / (p2y-p1y);  /*m is multiplier to normalize x (upscaled by 1024)*/
        p->steep = (m * (p2x-p1x)) >> 10;
    }

    if(p->side == LV_LINE_MASK_SIDE_LEFT) p->inv = 0;
    else if(p->side == LV_LINE_MASK_SIDE_RIGHT) p->inv = 1;
    else if(p->side == LV_LINE_MASK_SIDE_TOP) {
        if(p->steep > 0) p->inv = 0;
        else p->inv = 1;
    }
    else if(p->side == LV_LINE_MASK_SIDE_BOTTOM) {
        if(p->steep > 0) p->inv = 1;
        else p->inv = 0;
    }
}

void lv_mask_line_angle_init(lv_mask_line_param_t * p, lv_coord_t p1x, lv_coord_t p1y, int16_t deg, lv_line_mask_side_t side)
{
    lv_coord_t p2x;
    lv_coord_t p2y;

    p2x = (lv_trigo_sin(deg - 90) >> 5) + p1x;
    p2y = (lv_trigo_sin(deg) >> 5) + p1y;

    lv_mask_line_points_init(p, p1x, p1y, p2x, p2y, side);
}

void lv_mask_line(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, void * param)
{
    lv_mask_line_param_t * p = (lv_mask_line_param_t *)param;

    /*Make to points relative to the origo*/
    abs_y -= p->origo.y;
    abs_x -= p->origo.x;

    /*Handle special cases*/
    if(p->steep == 0) {
        /*Horizontal*/
        if(p->flat) {
            /*Non sense: Can't be on the right/left of a horizontal line*/
            if(p->side == LV_LINE_MASK_SIDE_LEFT || p->side == LV_LINE_MASK_SIDE_RIGHT) return;
            else if(p->side == LV_LINE_MASK_SIDE_TOP && abs_y+1 < p->origo.y) return;
            else if(p->side == LV_LINE_MASK_SIDE_BOTTOM && abs_y > p->origo.y) return;
            else {
                memset(mask_buf, 0x00, len);
                return;
            }
        }
        /*Vertical*/
        else {
            /*Non sense: Can't be on the top/bottom of a vertical line*/
            if(p->side == LV_LINE_MASK_SIDE_TOP || p->side == LV_LINE_MASK_SIDE_BOTTOM) return;
            else if(p->side == LV_LINE_MASK_SIDE_RIGHT && abs_x > 0) return;
            else if(p->side == LV_LINE_MASK_SIDE_LEFT) {
                if(abs_x + len < 0) return;
                else {
                    int32_t k = - abs_x;
                    if(k < 0) k = 0;
                    if(k >= 0 && k < len) memset(&mask_buf[k], 0x00, len - k);
                    return;
                }
            }
            else {
                memset(mask_buf, 0x00, len);
                return;
            }
        }
    }


    if(p->flat) {
        line_mask_flat(mask_buf, abs_x, abs_y, len, p);
    } else {
        line_mask_steep(mask_buf, abs_x, abs_y, len, p);
    }
}


void lv_mask_radius(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_mask_radius_param_t * param)
{

}



/**********************
 *   STATIC FUNCTIONS
 **********************/

static void line_mask_flat(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_mask_line_param_t * p)
{
    lv_coord_t y_at_x;

    /* At the beginning of the mask if the limit line is greater then the mask's y.
     * Then the mask is in the "wrong" area*/
    y_at_x = (int32_t)((int32_t)p->steep * abs_x) >> 10;

    if(p->steep > 0) {
        if(y_at_x > abs_y) {
            if(p->inv) {
                return;
            } else {
                memset(mask_buf, 0x00, len);
                return;
            }
        }
    } else {
        if(y_at_x < abs_y) {
            if(p->inv) {
                return;
            } else {
                memset(mask_buf, 0x00, len);
                return;
            }
        }
    }

    /* At the end of the mask if the limit line is smaller then the mask's y.
     * Then the mask is in the "good" area*/
    y_at_x = (int32_t)((int32_t)p->steep * (abs_x + len)) >> 10;
    if(p->steep > 0) {
        if(y_at_x < abs_y) {
            if(p->inv) {
                memset(mask_buf, 0x00, len);
                return;
            } else {
                return;
            }
        }
    } else {
        if(y_at_x > abs_y) {
            if(p->inv) {
                memset(mask_buf, 0x00, len);
                return;
            } else {
                return;
            }
        }
    }

    int32_t xe;
    if(p->steep > 0) xe = ((abs_y << 18) / p->steep);
    else xe = (((abs_y + 1) << 18) / p->steep);

    int32_t xei = xe >> 8;
    int32_t xef = xe & 0xFF;

    int32_t sps = p->steep >> 2;
    if(p->steep < 0) sps = -sps;

    int32_t px_h;
    if(xef == 0) px_h = 255;
    else px_h = 255 - (((255 - xef) * sps) >> 8);
    int32_t k = xei - abs_x;
    lv_opa_t m;

    if(xef) {
        if(k >= 0) {
            m = 255 - (((255-xef) * (255 - px_h)) >> 9);
            if(p->inv) m = 255 - m;
            mask_buf[k] = mask_mix(mask_buf[k], m);
        }
        k++;
    }

    while(px_h > sps) {
        if(k >= 0) {
            m = px_h - (sps >> 1);
            if(p->inv) m = 255 - m;
            mask_buf[k] = mask_mix(mask_buf[k], m);
        }
        px_h -= sps;
        k++;
        if(k >= len) break;
    }

    if(k < len && k >= 0) {
        int32_t x_inters = (px_h << 10) / p->steep;
        m =  (x_inters * px_h) >> 9;
        if(p->inv) m = 255 - m;
        mask_buf[k] = mask_mix(mask_buf[k], m);
    }

    if(p->inv) {
        k = xei - abs_x;
        if(k > len) k= len;
        if(k >= 0)
        {
            memset(&mask_buf[0], 0x00,  k);
        }
    } else {
        k++;
        if(k < len && k >= 0) {
            memset(&mask_buf[k], 0x00,  len - k);
        }
    }
}

static void line_mask_steep(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_mask_line_param_t * p)
{
    int32_t k;
    lv_coord_t x_at_y;
    /* At the beginning of the mask if the limit line is greater then the mask's y.
     * Then the mask is in the "wrong" area*/
    x_at_y = (int32_t)((int32_t)p->steep * abs_y) >> 10;
    if(x_at_y < abs_x) {
        if(p->inv) {
            return;
        } else {
            memset(mask_buf, 0x00, len);
            return;
        }
    }

    /* At the end of the mask if the limit line is smaller then the mask's y.
     * Then the mask is in the "good" area*/
    x_at_y = (int32_t)((int32_t)p->steep * (abs_y)) >> 10;
    if(x_at_y > abs_x + len) {
        if(p->inv) {
            memset(mask_buf, 0x00, len);
            return;
        } else {
            return;
        }
    }

    int32_t xe = ((abs_y << 8) * p->steep) >> 10;
    int32_t xei = xe >> 8;
    int32_t xef = xe & 0xFF;

    int32_t xq = (((abs_y + 1) << 8) * p->steep) >> 10;
    int32_t xqi = xq >> 8;
    int32_t xqf = xq & 0xFF;

    lv_opa_t m;

    k = xei - abs_x;
    if(xei != xqi && (p->steep < 0 && xef == 0)) {
        xef = 0xFF;
        xei = xqi;
        k--;
    }

    if(xei == xqi) {
        if(k >= 0 && k < len) {
            m = (xef + xqf) >> 1;
            if(p->inv) m = 255 - m;
            mask_buf[k] = mask_mix(mask_buf[k], m);
        }
        k++;

        if(p->inv) {
            k = xei - abs_x;
            if(k > len) k= len;
            if(k >= 0) memset(&mask_buf[0], 0x00,  k);

        } else {
            if(k >= 0 && k < len ) memset(&mask_buf[k] ,0x00,  len - (k));
        }

    } else {
        int32_t y_inters;
        if(p->steep < 0) {
            y_inters = (xef << 10) / (-p->steep);
            if(k >= 0 && k < len ) {
                m = (y_inters * xef) >> 9;
                if(p->inv) m = 255 - m;
                mask_buf[k] = mask_mix(mask_buf[k], m);
            }
            k--;

            int32_t x_inters = ((255-y_inters) * (-p->steep)) >> 10;

            if(k >= 0 && k < len ) {
                m = 255-(((255-y_inters) * x_inters) >> 9);
                if(p->inv) mask_buf[k] = 255 - mask_buf[k];
            }

            k+=2;

            if(p->inv) {
                k = xei - abs_x - 1;

                if(k > len) k= len;
                if(k >= 0) memset(&mask_buf[0], 0x00,  k);

            } else {
                if(k >= 0 && k < len ) memset(&mask_buf[k] ,0x00,  len - (k));
            }

        } else {
            y_inters = ((255-xef) << 10) / p->steep;
            if(k >= 0 && k < len ) {
                m = 255 - ((y_inters * (255-xef)) >> 9);
                if(p->inv) m = 255 - m;
                mask_buf[k] = mask_mix(mask_buf[k], m);
            }

            k++;

            int32_t x_inters = ((255-y_inters) * p->steep) >> 10;
            if(k >= 0 && k < len ) {
                m = ((255-y_inters) * x_inters) >> 9;
                if(p->inv) m = 255 - m;
                mask_buf[k] = mask_mix(mask_buf[k], m);
            }
            k++;

            if(p->inv) {
                k = xei - abs_x;
                if(k > len) k= len;
                if(k >= 0) memset(&mask_buf[0], 0x00,  k);

            } else {
                if(k >= 0 && k < len ) memset(&mask_buf[k] ,0x00,  len - (k));
            }

        }
    }
}

static lv_opa_t mask_mix(lv_opa_t mask_act, lv_opa_t mask_new)
{
    if(mask_new > LV_OPA_MAX) return mask_act;
    if(mask_new < LV_OPA_MIN) return 0;

    return (uint16_t)((uint16_t) (mask_act * mask_new) >> 8);

}
