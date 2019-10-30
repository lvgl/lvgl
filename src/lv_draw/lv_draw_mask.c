/**
 * @file lv_mask.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_mask.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_log.h"

/*********************
 *      DEFINES
 *********************/
#define LV_MASK_MAX_NUM     8

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    lv_draw_mask_param_t param;
    lv_draw_mask_cb_t cb;
    void * custom_id;
}lv_mask_saved_t;


/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_draw_mask_res_t lv_draw_mask_line(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_draw_mask_param_t * param);
static lv_draw_mask_res_t lv_draw_mask_radius(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_draw_mask_param_t * param);
static lv_draw_mask_res_t lv_draw_mask_angle(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_draw_mask_param_t * param);
static lv_draw_mask_res_t lv_draw_mask_fade(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_draw_mask_param_t * param);
static lv_draw_mask_res_t lv_draw_mask_map(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_draw_mask_param_t * param);

static lv_draw_mask_res_t line_mask_flat(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_draw_mask_line_param_t * p);
static lv_draw_mask_res_t line_mask_steep(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_draw_mask_line_param_t * p);

static inline lv_opa_t mask_mix(lv_opa_t mask_act, lv_opa_t mask_new);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_mask_saved_t mask_list[LV_MASK_MAX_NUM];

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int16_t lv_draw_mask_add(lv_draw_mask_param_t * param, void * custom_id)
{
    /*Search a free entry*/
    uint8_t i;
    for(i = 0; i < LV_MASK_MAX_NUM; i++) {
        if(mask_list[i].param.line.cb == NULL) break;
    }

    if(i >= LV_MASK_MAX_NUM) {
        LV_LOG_WARN("lv_mask_add: no place to add the mask");
        return LV_MASK_ID_INV;
    }

    memcpy(&mask_list[i].param, param, sizeof(lv_draw_mask_param_t));
    mask_list[i].custom_id = custom_id;

    return i;
}


lv_draw_mask_res_t lv_draw_mask_apply(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len)
{
    bool changed = false;
    lv_draw_mask_res_t res = LV_DRAW_MASK_RES_FULL_COVER;
    uint8_t i;
    for(i = 0; i < LV_MASK_MAX_NUM; i++) {
        if(mask_list[i].param.line.cb) {
            res = mask_list[i].param.line.cb(mask_buf, abs_x, abs_y, len, (void*)&mask_list[i].param);
            if(res == LV_DRAW_MASK_RES_FULL_TRANSP) return LV_DRAW_MASK_RES_FULL_TRANSP;
            else if(res == LV_DRAW_MASK_RES_CHANGED) changed = true;
        }
    }

    return changed ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;
}

void lv_draw_mask_remove_id(int16_t id)
{
    if(id != LV_MASK_ID_INV) {
        mask_list[id].param.line.cb = NULL;
    }
}

void lv_draw_mask_remove_custom(void * custom_id)
{
    uint8_t i;
    for(i = 0; i < LV_MASK_MAX_NUM; i++) {
        if(mask_list[i].custom_id == custom_id) {
            mask_list[i].param.line.cb = NULL;
        }
    }
}

uint8_t lv_draw_mask_get_cnt(void)
{
    uint8_t cnt = 0;
    uint8_t i;
    for(i = 0; i < LV_MASK_MAX_NUM; i++) {
        if(mask_list[i].param.line.cb) cnt++;
    }
    return cnt;
}

void lv_draw_mask_line_points_init(lv_draw_mask_param_t * param, lv_coord_t p1x, lv_coord_t p1y, lv_coord_t p2x, lv_coord_t p2y, lv_draw_mask_line_side_t side)
{
    lv_draw_mask_line_param_t * p = &param->line;
    memset(p, 0x00, sizeof(lv_draw_mask_line_param_t));

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
    p->yx_steep = 0;
    p->xy_steep = 0;
    p->cb = lv_draw_mask_line;

    lv_coord_t dx = p2x-p1x;
    lv_coord_t dy = p2y-p1y;

    if(p->flat) {
        /*Normalize the steep. Delta x should be relative to delta x = 1024*/
        int32_t m;

        if(dx) {
            m = (1 << 20) / dx;  /*m is multiplier to normalize y (upscaled by 1024)*/
            p->yx_steep = (m * dy) >> 10;
        }

        if(dy) {
            m = (1 << 20) / dy;  /*m is multiplier to normalize x (upscaled by 1024)*/
            p->xy_steep = (m * dx) >> 10;
        }
        p->steep = p->yx_steep;
    } else {
        /*Normalize the steep. Delta y should be relative to delta x = 1024*/
        int32_t m;

        if(dy) {
            m = (1 << 20) / dy;  /*m is multiplier to normalize x (upscaled by 1024)*/
            p->xy_steep = (m * dx) >> 10;
        }

        if(dx) {
            m = (1 << 20) / dx;  /*m is multiplier to normalize x (upscaled by 1024)*/
            p->yx_steep = (m * dy) >> 10;
        }
        p->steep = p->xy_steep;
    }

    if(p->side == LV_DRAW_MASK_LINE_SIDE_LEFT) p->inv = 0;
    else if(p->side == LV_DRAW_MASK_LINE_SIDE_RIGHT) p->inv = 1;
    else if(p->side == LV_DRAW_MASK_LINE_SIDE_TOP) {
        if(p->steep > 0) p->inv = 1;
        else p->inv = 0;
    }
    else if(p->side == LV_DRAW_MASK_LINE_SIDE_BOTTOM) {
        if(p->steep > 0) p->inv = 0;
        else p->inv = 1;
    }

    p->spx = p->steep >> 2;
    if(p->steep < 0) p->spx = -p->spx;
}

/**
 *
 * @param p
 * @param p1x
 * @param p1y
 * @param deg right 0 deg, bottom: 90
 * @param side
 */
void lv_draw_mask_line_angle_init(lv_draw_mask_param_t * param, lv_coord_t p1x, lv_coord_t p1y, int16_t deg, lv_draw_mask_line_side_t side)
{
    /* Find an optimal degree.
     * lv_mask_line_points_init will swap the points to keep the smaller y in p1
     * Theoretically a line with `deg` or `deg+180` is the same only the points are swapped
     * Find the degree which keeps the origo in place */
    if(deg > 180) deg -= 180; /*> 180 will swap the origo*/


    lv_coord_t p2x;
    lv_coord_t p2y;

    p2x = (lv_trigo_sin(deg + 90) >> 5) + p1x;
    p2y = (lv_trigo_sin(deg) >> 5) + p1y;

    lv_draw_mask_line_points_init(param, p1x, p1y, p2x, p2y, side);
}


void lv_draw_mask_angle_init(lv_draw_mask_param_t * param, lv_coord_t origo_x, lv_coord_t origo_y, lv_coord_t start_angle, lv_coord_t end_angle)
{
    lv_draw_mask_angle_param_t * p = &param->angle;

    lv_draw_mask_line_side_t start_side;
    lv_draw_mask_line_side_t end_side;

    if(end_angle < start_angle) {
        p->delta_deg = 360 - start_angle + end_angle;
    } else {
        p->delta_deg = LV_MATH_ABS(end_angle - start_angle);
    }

    p->start_angle = start_angle;
    p->end_angle = end_angle;
    p->origo.x = origo_x;
    p->origo.y = origo_y;
    p->cb = lv_draw_mask_angle;

    if(start_angle > 0 && start_angle < 180) {
        start_side = LV_DRAW_MASK_LINE_SIDE_LEFT;
    }
    else if(start_angle > 180 && start_angle < 360) {
        start_side = LV_DRAW_MASK_LINE_SIDE_RIGHT;
    }

    if(end_angle > 0 && end_angle < 180) {
        end_side = LV_DRAW_MASK_LINE_SIDE_RIGHT;
    }
    else if(end_angle > 180 && end_angle < 360) {
        end_side = LV_DRAW_MASK_LINE_SIDE_LEFT;
    }

    lv_draw_mask_line_angle_init((lv_draw_mask_param_t*)&p->start_line, origo_x, origo_y, start_angle, start_side);
    lv_draw_mask_line_angle_init((lv_draw_mask_param_t*)&p->end_line, origo_x, origo_y, end_angle, end_side);
}



void lv_draw_mask_radius_init(lv_draw_mask_param_t * param, const lv_area_t * rect, lv_coord_t radius, bool inv)
{
    lv_draw_mask_radius_param_t * p = &param->radius;

    lv_coord_t short_side = LV_MATH_MIN(lv_area_get_width(rect), lv_area_get_height(rect));
    if(radius > short_side >> 1) radius = short_side >> 1;

    lv_area_copy(&p->rect, rect);
    p->radius = radius;
    p->inv = inv ? 1 : 0;
    p->cb = lv_draw_mask_radius;
}



void lv_draw_mask_fade_init(lv_draw_mask_param_t * param, lv_area_t * rect, lv_opa_t opa_top, lv_coord_t y_top, lv_opa_t opa_bottom, lv_coord_t y_bottom)
{
    lv_draw_mask_fade_param_t * p = &param->fade;

    lv_area_copy(&p->rect, rect);
    p->opa_top= opa_top;
    p->opa_bottom = opa_bottom;
    p->y_top= y_top;
    p->y_bottom = y_bottom;
    p->cb = lv_draw_mask_fade;
}


void lv_draw_mask_map_init(lv_draw_mask_param_t * param, lv_area_t * coords, const lv_opa_t * map)
{
    lv_draw_mask_map_param_t * p = &param->fade;

    lv_area_copy(&p->coords, coords);
    p->map = map;
    p->cb = lv_draw_mask_map;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_draw_mask_res_t lv_draw_mask_line(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_draw_mask_param_t * param)
{
    lv_draw_mask_line_param_t * p = &param->line;

    /*Make to points relative to the origo*/
    abs_y -= p->origo.y;
    abs_x -= p->origo.x;

    /*Handle special cases*/
    if(p->steep == 0) {
        /*Horizontal*/
        if(p->flat) {
            /*Non sense: Can't be on the right/left of a horizontal line*/
            if(p->side == LV_DRAW_MASK_LINE_SIDE_LEFT || p->side == LV_DRAW_MASK_LINE_SIDE_RIGHT) return LV_DRAW_MASK_RES_FULL_COVER;
            else if(p->side == LV_DRAW_MASK_LINE_SIDE_TOP && abs_y+1 < 0) return LV_DRAW_MASK_RES_FULL_COVER;
            else if(p->side == LV_DRAW_MASK_LINE_SIDE_BOTTOM && abs_y > 0) return LV_DRAW_MASK_RES_FULL_COVER;
            else {
                return LV_DRAW_MASK_RES_FULL_TRANSP;
            }
        }
        /*Vertical*/
        else {
            /*Non sense: Can't be on the top/bottom of a vertical line*/
            if(p->side == LV_DRAW_MASK_LINE_SIDE_TOP || p->side == LV_DRAW_MASK_LINE_SIDE_BOTTOM) return LV_DRAW_MASK_RES_FULL_COVER;
            else if(p->side == LV_DRAW_MASK_LINE_SIDE_RIGHT && abs_x > 0) return LV_DRAW_MASK_RES_FULL_COVER;
            else if(p->side == LV_DRAW_MASK_LINE_SIDE_LEFT) {
                if(abs_x + len < 0) return LV_DRAW_MASK_RES_FULL_COVER;
                else {
                    int32_t k = - abs_x;
                    if(k < 0) return LV_DRAW_MASK_RES_FULL_TRANSP;
                    if(k >= 0 && k < len) memset(&mask_buf[k], 0x00, len - k);
                    return  LV_DRAW_MASK_RES_CHANGED;
                }
            }
            else {
                if(abs_x + len < 0) return LV_DRAW_MASK_RES_FULL_TRANSP;
                else {
                    int32_t k = - abs_x;
                    if(k < 0) k = 0;
                    if(k >= len) return LV_DRAW_MASK_RES_FULL_TRANSP;
                    else if(k >= 0 && k < len) memset(&mask_buf[0], 0x00,k);
                    return  LV_DRAW_MASK_RES_CHANGED;
                }
            }
        }
    }

    lv_draw_mask_res_t res;
    if(p->flat) {
        res = line_mask_flat(mask_buf, abs_x, abs_y, len, p);
    } else {
        res = line_mask_steep(mask_buf, abs_x, abs_y, len, p);
    }

    return res;
}

static lv_draw_mask_res_t line_mask_flat(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_draw_mask_line_param_t * p)
{
    lv_coord_t y_at_x;
    y_at_x = (int32_t)((int32_t)p->yx_steep * abs_x) >> 10;

    if(p->yx_steep > 0) {
        if(y_at_x > abs_y) {
            if(p->inv) {
                return LV_DRAW_MASK_RES_FULL_COVER;
            } else {
                return LV_DRAW_MASK_RES_FULL_TRANSP;
            }
        }
    } else {
        if(y_at_x < abs_y) {
            if(p->inv) {
                return LV_DRAW_MASK_RES_FULL_COVER;
            } else {
                return LV_DRAW_MASK_RES_FULL_TRANSP;
            }
        }
    }

    /* At the end of the mask if the limit line is smaller then the mask's y.
     * Then the mask is in the "good" area*/
    y_at_x = (int32_t)((int32_t)p->yx_steep * (abs_x + len)) >> 10;
    if(p->yx_steep > 0) {
        if(y_at_x < abs_y) {
            if(p->inv) {
                return LV_DRAW_MASK_RES_FULL_TRANSP;
            } else {
                return LV_DRAW_MASK_RES_FULL_COVER;
            }
        }
    } else {
        if(y_at_x > abs_y) {
            if(p->inv) {
                return LV_DRAW_MASK_RES_FULL_TRANSP;
            } else {
                return LV_DRAW_MASK_RES_FULL_COVER;
            }
        }
    }

    int32_t xe;
    if(p->yx_steep > 0) xe = ((abs_y << 8) * p->xy_steep) >> 10;
    else xe = (((abs_y + 1) << 8) * p->xy_steep) >> 10;

    int32_t xei = xe >> 8;
    int32_t xef = xe & 0xFF;

    int32_t px_h;
    if(xef == 0) px_h = 255;
    else px_h = 255 - (((255 - xef) * p->spx) >> 8);
    int32_t k = xei - abs_x;
    lv_opa_t m;

    if(xef) {
        if(k >= 0 && k < len) {
            m = 255 - (((255-xef) * (255 - px_h)) >> 9);
            if(p->inv) m = 255 - m;
            mask_buf[k] = mask_mix(mask_buf[k], m);
        }
        k++;
    }

    while(px_h > p->spx) {
        if(k >= 0 && k < len) {
            m = px_h - (p->spx >> 1);
            if(p->inv) m = 255 - m;
            mask_buf[k] = mask_mix(mask_buf[k], m);
        }
        px_h -= p->spx;
        k++;
        if(k >= len) break;
    }


    if(k < len && k >= 0) {
        int32_t x_inters = (px_h * p->xy_steep) >> 10;
        m = (x_inters * px_h) >> 9;
        if(p->yx_steep < 0) m = 255 - m;
        if(p->inv) m = 255 - m;
        mask_buf[k] = mask_mix(mask_buf[k], m);
    }


    if(p->inv) {
        k = xei - abs_x;
        if(k > len) {
            return LV_DRAW_MASK_RES_FULL_TRANSP;
        }
        if(k >= 0)
        {
            memset(&mask_buf[0], 0x00,  k);
        }
    } else {
        k++;
        if(k < 0) {
            return LV_DRAW_MASK_RES_FULL_TRANSP;
        }
        if(k <= len) {
            memset(&mask_buf[k], 0x00,  len - k);
        }
    }

    return LV_DRAW_MASK_RES_CHANGED;
}

static lv_draw_mask_res_t line_mask_steep(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_draw_mask_line_param_t * p)
{
    int32_t k;
    lv_coord_t x_at_y;
    /* At the beginning of the mask if the limit line is greater then the mask's y.
     * Then the mask is in the "wrong" area*/
    x_at_y = (int32_t)((int32_t)p->xy_steep * abs_y) >> 10;
    if(p->xy_steep > 0) x_at_y++;
    if(x_at_y < abs_x) {
        if(p->inv) {
            return LV_DRAW_MASK_RES_FULL_COVER;
        } else {
            return LV_DRAW_MASK_RES_FULL_TRANSP;
        }
    }

    /* At the end of the mask if the limit line is smaller then the mask's y.
     * Then the mask is in the "good" area*/
    x_at_y = (int32_t)((int32_t)p->xy_steep * (abs_y)) >> 10;
    if(x_at_y > abs_x + len) {
        if(p->inv) {
            return LV_DRAW_MASK_RES_FULL_TRANSP;
        } else {
            return LV_DRAW_MASK_RES_FULL_COVER;
        }
    }

    int32_t xe = ((abs_y << 8) * p->xy_steep) >> 10;
    int32_t xei = xe >> 8;
    int32_t xef = xe & 0xFF;

    int32_t xq = (((abs_y + 1) << 8) * p->xy_steep) >> 10;
    int32_t xqi = xq >> 8;
    int32_t xqf = xq & 0xFF;

    lv_opa_t m;

    k = xei - abs_x;
    if(xei != xqi && (p->xy_steep < 0 && xef == 0)) {
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
            if(k >= len) {
                return LV_DRAW_MASK_RES_FULL_TRANSP;
            }
            if(k >= 0) memset(&mask_buf[0], 0x00,  k);

        } else {
            if(k > len) k = len;
            if(k == 0) return LV_DRAW_MASK_RES_FULL_TRANSP;
            else if(k > 0) memset(&mask_buf[k] ,0x00,  len - k);
        }

    } else {
        int32_t y_inters;
        if(p->xy_steep < 0) {
            y_inters = (xef * (-p->yx_steep)) >> 10;
            if(k >= 0 && k < len ) {
                m = (y_inters * xef) >> 9;
                if(p->inv) m = 255 - m;
                mask_buf[k] = mask_mix(mask_buf[k], m);
            }
            k--;

            int32_t x_inters = ((255-y_inters) * (-p->xy_steep)) >> 10;

            if(k >= 0 && k < len ) {
                m = 255-(((255-y_inters) * x_inters) >> 9);
                if(p->inv) m = 255 - m;
                mask_buf[k] = mask_mix(mask_buf[k], m);
            }

            k+=2;

            if(p->inv) {
                k = xei - abs_x - 1;

                if(k > len) k = len;
                else if(k > 0) memset(&mask_buf[0], 0x00,  k);

            } else {
                if(k > len) return LV_DRAW_MASK_RES_FULL_COVER;
                if(k >= 0) memset(&mask_buf[k] ,0x00,  len - k);
            }

        } else {
            y_inters = ((255-xef) * p->yx_steep) >> 10;
            if(k >= 0 && k < len ) {
                m = 255 - ((y_inters * (255-xef)) >> 9);
                if(p->inv) m = 255 - m;
                mask_buf[k] = mask_mix(mask_buf[k], m);
            }

            k++;

            int32_t x_inters = ((255-y_inters) * p->xy_steep) >> 10;
            if(k >= 0 && k < len ) {
                m = ((255-y_inters) * x_inters) >> 9;
                if(p->inv) m = 255 - m;
                mask_buf[k] = mask_mix(mask_buf[k], m);
            }
            k++;

            if(p->inv) {
                k = xei - abs_x;
                if(k > len)  return LV_DRAW_MASK_RES_FULL_TRANSP;
                if(k >= 0) memset(&mask_buf[0], 0x00,  k);

            } else {
                if(k > len) k = len;
                if(k == 0) return LV_DRAW_MASK_RES_FULL_TRANSP;
                else if(k > 0) memset(&mask_buf[k] ,0x00,  len - k);
            }

        }
    }

    return LV_DRAW_MASK_RES_CHANGED;
}


static lv_draw_mask_res_t lv_draw_mask_angle(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_draw_mask_param_t * param)
{
    lv_draw_mask_angle_param_t * p = &param->angle;

    lv_coord_t rel_y = abs_y - p->origo.y;
    lv_coord_t rel_x = abs_x - p->origo.x;


    if(p->start_angle < 180 && p->end_angle < 180 && p->start_angle != 0  && p->end_angle != 0 && p->start_angle > p->end_angle) {

        if(abs_y < p->origo.y) {
            return LV_DRAW_MASK_RES_FULL_COVER;
        }

        /*Start angle mask can work only from the end of end angle mask */
        lv_coord_t end_angle_first = (rel_y * p->end_line.xy_steep) >> 10;
        lv_coord_t start_angle_last= ((rel_y+1) * p->start_line.xy_steep) >> 10;


        /*Do not let the line end cross the origo else it will affect the opposite part*/
        if(p->start_angle > 270 && p->start_angle <= 359 && start_angle_last < 0) start_angle_last = 0;
        else if(p->start_angle > 0 && p->start_angle <= 90 && start_angle_last < 0) start_angle_last = 0;
        else if(p->start_angle > 90 && p->start_angle < 270 && start_angle_last > 0) start_angle_last = 0;

        if(p->end_angle > 270 && p->end_angle <= 359 && start_angle_last < 0) start_angle_last = 0;
        else if(p->end_angle > 0 &&   p->end_angle <= 90 && start_angle_last < 0) start_angle_last = 0;
        else if(p->end_angle > 90 &&  p->end_angle < 270 && start_angle_last > 0) start_angle_last = 0;


        int32_t dist = (end_angle_first - start_angle_last) >> 1;

        lv_draw_mask_res_t res1 = LV_DRAW_MASK_RES_FULL_COVER;
        lv_draw_mask_res_t res2 = LV_DRAW_MASK_RES_FULL_COVER;

        int32_t tmp = start_angle_last + dist - rel_x;
        if(tmp > len) tmp = len;
        if(tmp > 0) {
            res1 = lv_draw_mask_line(&mask_buf[0], abs_x, abs_y, tmp, (lv_draw_mask_param_t*)&p->start_line);
            if(res1 == LV_DRAW_MASK_RES_FULL_TRANSP) {
                memset(&mask_buf[0], 0x00, tmp);
            }
        }

        if(tmp > len) tmp = len;
        if(tmp < 0) tmp = 0;
        res2 = lv_draw_mask_line(&mask_buf[tmp], abs_x+tmp, abs_y, len-tmp, (lv_draw_mask_param_t*)&p->end_line);
        if(res2 == LV_DRAW_MASK_RES_FULL_TRANSP) {
            memset(&mask_buf[tmp], 0x00, len-tmp);
        }
        if(res1 == res2) return res1;
        else return LV_DRAW_MASK_RES_CHANGED;
    }
    else if(p->start_angle > 180 && p->end_angle > 180 && p->start_angle > p->end_angle) {

        if(abs_y > p->origo.y) {
            return LV_DRAW_MASK_RES_FULL_COVER;
        }

        /*Start angle mask can work only from the end of end angle mask */
        lv_coord_t end_angle_first = (rel_y * p->end_line.xy_steep) >> 10;
        lv_coord_t start_angle_last= ((rel_y+1) * p->start_line.xy_steep) >> 10;

        /*Do not let the line end cross the origo else it will affect the opposite part*/
        if(p->start_angle > 270 && p->start_angle <= 359 && start_angle_last < 0) start_angle_last = 0;
        else if(p->start_angle > 0 && p->start_angle <= 90 && start_angle_last < 0) start_angle_last = 0;
        else if(p->start_angle > 90 && p->start_angle < 270 && start_angle_last > 0) start_angle_last = 0;

        if(p->end_angle > 270 && p->end_angle <= 359 && start_angle_last < 0) start_angle_last = 0;
        else if(p->end_angle > 0 &&   p->end_angle <= 90 && start_angle_last < 0) start_angle_last = 0;
        else if(p->end_angle > 90 &&  p->end_angle < 270 && start_angle_last > 0) start_angle_last = 0;

        int32_t dist = (end_angle_first - start_angle_last) >> 1;

        lv_draw_mask_res_t res1 = LV_DRAW_MASK_RES_FULL_COVER;
        lv_draw_mask_res_t res2 = LV_DRAW_MASK_RES_FULL_COVER;

        int32_t tmp = start_angle_last + dist - rel_x;
        if(tmp > len) tmp = len;
        if(tmp > 0) {
            res1 = lv_draw_mask_line(&mask_buf[0], abs_x, abs_y, tmp, (lv_draw_mask_param_t*)&p->end_line);
            if(res1 == LV_DRAW_MASK_RES_FULL_TRANSP) {
                memset(&mask_buf[0], 0x00, tmp);
            }
        }

        if(tmp > len) tmp = len;
        if(tmp < 0) tmp = 0;
        res2 = lv_draw_mask_line(&mask_buf[tmp], abs_x+tmp, abs_y, len-tmp, (lv_draw_mask_param_t*)&p->start_line);
        if(res2 == LV_DRAW_MASK_RES_FULL_TRANSP) {
            memset(&mask_buf[tmp], 0x00, len-tmp);
        }
        if(res1 == res2) return res1;
        else return LV_DRAW_MASK_RES_CHANGED;
    }
    else  {

        lv_draw_mask_res_t res1 = LV_DRAW_MASK_RES_FULL_COVER;
        lv_draw_mask_res_t res2 = LV_DRAW_MASK_RES_FULL_COVER;

        if(p->start_angle == 180) {
            if(abs_y < p->origo.y) res1 = LV_DRAW_MASK_RES_FULL_COVER;
            else res1 = LV_DRAW_MASK_RES_UNKNOWN;
        }
        else if(p->start_angle == 0) {
            if(abs_y < p->origo.y) res1 = LV_DRAW_MASK_RES_UNKNOWN;
            else res1 = LV_DRAW_MASK_RES_FULL_COVER;
        }
        else if((p->start_angle < 180 && abs_y < p->origo.y) ||
                (p->start_angle > 180 && abs_y >= p->origo.y)) {
            res1 = LV_DRAW_MASK_RES_UNKNOWN;
        }
        else  {
            res1 = lv_draw_mask_line(mask_buf, abs_x, abs_y, len, (lv_draw_mask_param_t*)&p->start_line);
        }

        if(p->end_angle == 180) {
            if(abs_y < p->origo.y) res2 = LV_DRAW_MASK_RES_UNKNOWN;
            else res2 = LV_DRAW_MASK_RES_FULL_COVER;
        }
        else if(p->end_angle == 0) {
            if(abs_y < p->origo.y) res2 = LV_DRAW_MASK_RES_FULL_COVER;
            else res2 = LV_DRAW_MASK_RES_UNKNOWN;
        }
        else if((p->end_angle < 180 && abs_y < p->origo.y) ||
                (p->end_angle > 180 && abs_y >= p->origo.y)) {
            res2 = LV_DRAW_MASK_RES_UNKNOWN;
        }
        else {
            res2 = lv_draw_mask_line(mask_buf, abs_x, abs_y, len, (lv_draw_mask_param_t*)&p->end_line);
        }

        if(res1 == LV_DRAW_MASK_RES_FULL_TRANSP || res2 == LV_DRAW_MASK_RES_FULL_TRANSP) return LV_DRAW_MASK_RES_FULL_TRANSP;
        else if(res1 == LV_DRAW_MASK_RES_UNKNOWN && res2 == LV_DRAW_MASK_RES_UNKNOWN) return LV_DRAW_MASK_RES_FULL_TRANSP;
        else if(res1 == LV_DRAW_MASK_RES_FULL_COVER &&  res2 == LV_DRAW_MASK_RES_FULL_COVER) return LV_DRAW_MASK_RES_FULL_COVER;
        else return LV_DRAW_MASK_RES_CHANGED;
    }
}

static lv_draw_mask_res_t lv_draw_mask_radius(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_draw_mask_param_t * param)
{
    lv_draw_mask_radius_param_t * p = &param->radius;

    if(p->inv == 0) {
        if(abs_y < p->rect.y1 || abs_y > p->rect.y2) {
            return LV_DRAW_MASK_RES_FULL_TRANSP;
        }
    } else {
        if(abs_y < p->rect.y1 || abs_y > p->rect.y2) {
            return LV_DRAW_MASK_RES_FULL_COVER;
        }
    }

    if((abs_x >= p->rect.x1 + p->radius && abs_x + len <= p->rect.x2 - p->radius) ||
            (abs_y >= p->rect.y1 + p->radius && abs_y <= p->rect.y2 - p->radius)) {
        if(p->inv == 0) {
            /*Remove the edges*/
            int32_t last =  p->rect.x1 - abs_x;
            if(last > len) return LV_DRAW_MASK_RES_FULL_TRANSP;
            if(last >= 0) {
                memset(&mask_buf[0], 0x00, last);
            }

            int32_t first = p->rect.x2 - abs_x + 1;
            if(first <= 0) return LV_DRAW_MASK_RES_FULL_TRANSP;
            else if(first < len) {
                memset(&mask_buf[first], 0x00, len-first);
            }
            if(last == 0 && first == len) return LV_DRAW_MASK_RES_FULL_COVER;
            else return LV_DRAW_MASK_RES_CHANGED;
        }
        else {
            int32_t first = p->rect.x1 - abs_x;
            if(first < 0) first = 0;
            if(first <= len) {
                int32_t last =  p->rect.x2 - abs_x - first + 1;
                if(first + last > len) last = len - first;
                if(last >= 0) {
                    memset(&mask_buf[first], 0x00, last);
                }
            }
        }
        return LV_DRAW_MASK_RES_CHANGED;
    }

    int32_t k = p->rect.x1 -abs_x; /*First relevant coordinate on the of the mask*/
    lv_coord_t w = lv_area_get_width(&p->rect);
    lv_coord_t h = lv_area_get_height(&p->rect);
    abs_x -= p->rect.x1;
    abs_y -= p->rect.y1;

    uint32_t r2 = p->radius * p->radius;

    /*Handle corner areas*/
    if(abs_y < p->radius || abs_y > h - p->radius - 1) {
        /* y = 0 should mean the top of the circle */
        lv_coord_t y;
        if(abs_y < p->radius)  y = p->radius - abs_y;
        else y = p->radius - (h - abs_y) + 1;

        /* Get the x intersection points for `abs_y` and `abs_y+1`
         * Use the circle's equation x = sqrt(r^2 - y^2) */
        lv_sqrt_res_t x0;
        lv_sqrt(r2 - (y * y), &x0);

        lv_sqrt_res_t x1;
        lv_sqrt(r2 - ((y-1) * (y-1)), &x1);

        /* If x1 is on the next round coordinate (e.g. x0: 3.5, x1:4.0)
         * then treat x1 as x1: 3.99 to handle them as they were on the same pixel*/
        if(x0.i == x1.i - 1 && x1.f == 0) {
            x1.i--;
            x1.f = 0xFF;
        }

        /*If the two x intersections are on the same x then just get average of the fractionals*/
        if(x0.i == x1.i) {
            lv_opa_t m = (x0.f + x1.f) >> 1;
            if(p->inv) m = 255 - m;
            int32_t ofs = p->radius - x0.i - 1;

            /*Left corner*/
            int32_t kl = k + ofs;

            if(kl >= 0 && kl < len) {
                mask_buf[kl] = mask_mix(mask_buf[kl], m);
            }

            /*Right corner*/
            int32_t kr = k+(w-ofs-1);
            if(kr >= 0 && kr < len) {
                mask_buf[kr] = mask_mix(mask_buf[kr], m);
            }

            /*Clear the unused parts*/
            if(p->inv == 0) {
                kr++;
                if(kl > len)  {
                    return LV_DRAW_MASK_RES_FULL_TRANSP;
                }
                if(kl >= 0) {
                    memset(&mask_buf[0], 0x00, kl);
                }
                if(kr < 0) {
                    return LV_DRAW_MASK_RES_FULL_TRANSP;
                }
                if(kr <= len) {
                    memset(&mask_buf[kr], 0x00, len-kr);
                }
            } else {
                kl++;
                int32_t first = kl;
                if(first < 0) first = 0;

                int32_t len_tmp = kr-first;
                if(len_tmp + first > len) len_tmp = len - first;
                if(first < len && len_tmp >= 0) {
                    memset(&mask_buf[first], 0x00, len_tmp);
                }
            }
        }
        /*Multiple pixels are affected. Get y intersection of the pixels*/
        else {
            int32_t ofs = p->radius - (x0.i + 1);
            int32_t kl = k + ofs;
            int32_t kr = k + (w - ofs -1);

            if(p->inv) {
                int32_t first = kl + 1;
                if(first < 0) first = 0;

                int32_t len_tmp = kr-first;
                if(len_tmp + first > len) len_tmp = len - first;
                if(first < len && len_tmp >= 0) {
                    memset(&mask_buf[first], 0x00, len_tmp);
                }
            }

            uint32_t i = x0.i + 1;
            lv_opa_t m;
            lv_sqrt_res_t y_prev;
            lv_sqrt_res_t y_next;

            lv_sqrt(r2 - (x0.i * x0.i), &y_prev);

            if(y_prev.f == 0) {
                y_prev.i--;
                y_prev.f = 0xFF;
            }

            /*The first y intersection is special as it might be in the previous line*/
            if(y_prev.i >= y) {
                lv_sqrt(r2 - (i * i), &y_next);
                m = 255 - (((255-x0.f) * (255 - y_next.f)) >> 9);

                if(p->inv) m = 255 - m;
                if(kl >= 0 && kl < len) mask_buf[kl] = mask_mix(mask_buf[kl], m);
                if(kr >= 0 && kr < len) mask_buf[kr] = mask_mix(mask_buf[kr], m);
                kl--;
                kr++;
                y_prev.f = y_next.f;
                i++;
            }

            /*Set all points which are crossed by the circle*/
            for(; i <= x1.i; i++) {
                lv_sqrt(r2 - (i * i), &y_next);

                m = (y_prev.f + y_next.f) >> 1;
                if(p->inv) m = 255 - m;
                if(kl >= 0 && kl < len) mask_buf[kl] = mask_mix(mask_buf[kl], m);
                if(kr >= 0 && kr < len) mask_buf[kr] = mask_mix(mask_buf[kr], m);
                kl--;
                kr++;
                y_prev.f = y_next.f;
            }

            /*If the last pixel was left in its middle therefore
             * the circle still has parts on the next one*/
            if(y_prev.f) {
                m = (y_prev.f * x1.f) >> 9;
                if(p->inv) m = 255 - m;
                if(kl >= 0 && kl < len) mask_buf[kl] = mask_mix(mask_buf[kl], m);
                if(kr >= 0 && kr < len) mask_buf[kr] = mask_mix(mask_buf[kr], m);
                kl--;
                kr++;
            }

            if(p->inv == 0) {
                kl++;
                if(kl > len) {
                    return LV_DRAW_MASK_RES_FULL_TRANSP;
                }
                if(kl >= 0) memset(&mask_buf[0], 0x00, kl);

                if(kr < 0) {
                    return LV_DRAW_MASK_RES_FULL_TRANSP;
                }
                if(kr < len) memset(&mask_buf[kr], 0x00, len - kr);
            }
        }
    }

    return LV_DRAW_MASK_RES_CHANGED;
}


static lv_draw_mask_res_t lv_draw_mask_fade(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_draw_mask_param_t * param)
{
    lv_draw_mask_fade_param_t * p = &param->fade;

    if(abs_y < p->rect.y1) return LV_DRAW_MASK_RES_FULL_COVER;
    if(abs_y > p->rect.y2) return LV_DRAW_MASK_RES_FULL_COVER;
    if(abs_x + len < p->rect.x1) return LV_DRAW_MASK_RES_FULL_COVER;
    if(abs_x > p->rect.x2) return LV_DRAW_MASK_RES_FULL_COVER;

    if(abs_x + len > p->rect.x2) len -= abs_x + len - p->rect.x2 - 1;

    if(abs_x < p->rect.x1) {
        lv_coord_t x_ofs = 0;
        x_ofs = p->rect.x1 - abs_x;
        len -= x_ofs;
        mask_buf += x_ofs;
    }

    lv_coord_t i;

    if(abs_y <= p->y_top) {
        for(i = 0; i < len; i++) {
            mask_buf[i] = mask_mix(mask_buf[i], p->opa_top);
        }
        return LV_DRAW_MASK_RES_CHANGED;
    } else if(abs_y >= p->y_bottom) {
        for(i = 0; i < len; i++) {
            mask_buf[i] = mask_mix(mask_buf[i], p->opa_bottom);
        }
        return LV_DRAW_MASK_RES_CHANGED;
    } else {
        /*Calculate the opa proportionally*/
        int16_t opa_diff = p->opa_bottom - p->opa_top;
        lv_coord_t y_diff = p->y_bottom - p->y_top + 1;
        lv_opa_t opa_act = (int32_t)((int32_t)(abs_y - p->y_top) * opa_diff) / y_diff;
        opa_act += p->opa_top;

        for(i = 0; i < len; i++) {
            mask_buf[i] = mask_mix(mask_buf[i], opa_act);
        }
        return LV_DRAW_MASK_RES_CHANGED;


    }
    return LV_DRAW_MASK_RES_FULL_COVER;
}

static lv_draw_mask_res_t lv_draw_mask_map(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, lv_draw_mask_param_t * param)
{
    lv_draw_mask_map_param_t * p = &param->map;

    /*Handle out of the mask cases*/
    if(abs_y < p->coords.y1) return LV_DRAW_MASK_RES_FULL_COVER;
    if(abs_y > p->coords.y2) return LV_DRAW_MASK_RES_FULL_COVER;
    if(abs_x + len < p->coords.x1) return LV_DRAW_MASK_RES_FULL_COVER;
    if(abs_x > p->coords.x2) return LV_DRAW_MASK_RES_FULL_COVER;

    /*Got to the current row in the map*/
    const lv_opa_t * map_tmp = p->map;
    map_tmp += (abs_y - p->coords.y1) * lv_area_get_width(&p-> coords);


    if(abs_x + len > p->coords.x2) len -= abs_x + len - p->coords.x2 - 1;

    if(abs_x < p->coords.x1) {
        lv_coord_t x_ofs = 0;
        x_ofs = p->coords.x1 - abs_x;
        len -= x_ofs;
        mask_buf += x_ofs;
    } else {
        map_tmp += (abs_x - p->coords.x1);
    }

    lv_coord_t i;
    for(i = 0; i < len; i++) {
        mask_buf[i] = mask_mix(mask_buf[i], map_tmp[i]);
    }

    return LV_DRAW_MASK_RES_CHANGED;
}


static inline lv_opa_t mask_mix(lv_opa_t mask_act, lv_opa_t mask_new)
{
    if(mask_new > LV_OPA_MAX) return mask_act;
    if(mask_new < LV_OPA_MIN) return 0;

    return (uint16_t)((uint16_t) (mask_act * mask_new) >> 8);

}
