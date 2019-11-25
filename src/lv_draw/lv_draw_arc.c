/**
 * @file lv_draw_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_arc.h"
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
static void ver_line(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_coord_t len, lv_color_t color,
                     lv_opa_t opa);
static void hor_line(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_coord_t len, lv_color_t color,
                     lv_opa_t opa);
static bool deg_test_norm(uint16_t deg, uint16_t start, uint16_t end);
static bool deg_test_inv(uint16_t deg, uint16_t start, uint16_t end);

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
 * Draw an arc. (Can draw pie too with great thickness.)
 * @param center_x the x coordinate of the center of the arc
 * @param center_y the y coordinate of the center of the arc
 * @param radius the radius of the arc
 * @param mask the arc will be drawn only in this mask
 * @param start_angle the start angle of the arc (0 deg on the bottom, 90 deg on the right)
 * @param end_angle the end angle of the arc
 * @param style style of the arc (`body.thickness`, `body.main_color`, `body.opa` is used)
 * @param opa_scale scale down all opacities by the factor
 */
void lv_draw_arc(lv_coord_t center_x, lv_coord_t center_y, uint16_t radius, const lv_area_t * mask,
                 uint16_t start_angle, uint16_t end_angle, const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_coord_t thickness = style->line.width;
    if(thickness > radius) thickness = radius;

#if LV_ANTIALIAS
    thickness--;
    radius--;
#endif

    lv_coord_t r_out = radius;
    lv_coord_t r_in  = r_out - thickness;
    int16_t deg_base;
    int16_t deg;
    lv_coord_t x_start[4];
    lv_coord_t x_end[4];

    lv_color_t color = style->line.color;
    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->body.opa : (uint16_t)((uint16_t)style->body.opa * opa_scale) >> 8;

    bool (*deg_test)(uint16_t, uint16_t, uint16_t);
    if(start_angle <= end_angle)
        deg_test = deg_test_norm;
    else
        deg_test = deg_test_inv;

    int middle_r_out = r_out;
#if !LV_ANTIALIAS
    thickness--;
    middle_r_out = r_out - 1;
#endif
    if(deg_test(270, start_angle, end_angle))
        hor_line(center_x - middle_r_out, center_y, mask, thickness, color, opa); /*Left Middle*/
    if(deg_test(90, start_angle, end_angle))
        hor_line(center_x + r_in, center_y, mask, thickness, color, opa); /*Right Middle*/
    if(deg_test(180, start_angle, end_angle))
        ver_line(center_x, center_y - middle_r_out, mask, thickness, color, opa); /*Top Middle*/
    if(deg_test(0, start_angle, end_angle))
        ver_line(center_x, center_y + r_in, mask, thickness, color, opa); /*Bottom middle*/

    uint32_t r_out_sqr = r_out * r_out;
    uint32_t r_in_sqr  = r_in * r_in;
#if LV_ANTIALIAS
    uint32_t r_out_aa_sqr = (r_out + 1) * (r_out + 1);
    uint32_t r_in_aa_sqr  = (r_in - 1) * (r_in - 1);
#endif
    int16_t xi;
    int16_t yi;
    for(yi = -r_out; yi < 0; yi++) {
        x_start[0] = LV_COORD_MIN;
        x_start[1] = LV_COORD_MIN;
        x_start[2] = LV_COORD_MIN;
        x_start[3] = LV_COORD_MIN;
        x_end[0]   = LV_COORD_MIN;
        x_end[1]   = LV_COORD_MIN;
        x_end[2]   = LV_COORD_MIN;
        x_end[3]   = LV_COORD_MIN;
        int xe     = 0;
        for(xi = -r_out; xi < 0; xi++) {

            uint32_t r_act_sqr = xi * xi + yi * yi;
#if LV_ANTIALIAS
            if(r_act_sqr > r_out_aa_sqr) {
                continue;
            }
#else
            if(r_act_sqr > r_out_sqr) continue;
#endif

            deg_base = lv_atan2(xi, yi) - 180;

#if LV_ANTIALIAS
            int opa2 = -1;
            if(r_act_sqr > r_out_sqr) {
                opa2 = LV_OPA_100 * (r_out + 1) - lv_sqrt(LV_OPA_100 * LV_OPA_100 * r_act_sqr);
                if(opa2 < LV_OPA_0)
                    opa2 = LV_OPA_0;
                else if(opa2 > LV_OPA_100)
                    opa2 = LV_OPA_100;
            } else if(r_act_sqr < r_in_sqr) {
                if(xe == 0) xe = xi;
                opa2 = lv_sqrt(LV_OPA_100 * LV_OPA_100 * r_act_sqr) - LV_OPA_100 * (r_in - 1);
                if(opa2 < LV_OPA_0)
                    opa2 = LV_OPA_0;
                else if(opa2 > LV_OPA_100)
                    opa2 = LV_OPA_100;
                if(r_act_sqr < r_in_aa_sqr)
                    break; /*No need to continue the iteration in x once we found the inner edge of the
                              arc*/
            }
            if(opa2 != -1) {
                if(deg_test(180 + deg_base, start_angle, end_angle)) {
                    lv_draw_px(center_x + xi, center_y + yi, mask, color, opa2);
                }
                if(deg_test(360 - deg_base, start_angle, end_angle)) {
                    lv_draw_px(center_x + xi, center_y - yi, mask, color, opa2);
                }
                if(deg_test(180 - deg_base, start_angle, end_angle)) {
                    lv_draw_px(center_x - xi, center_y + yi, mask, color, opa2);
                }
                if(deg_test(deg_base, start_angle, end_angle)) {
                    lv_draw_px(center_x - xi, center_y - yi, mask, color, opa2);
                }
                continue;
            }
#endif

            deg = 180 + deg_base;
            if(deg_test(deg, start_angle, end_angle)) {
                if(x_start[0] == LV_COORD_MIN) x_start[0] = xi;
            } else if(x_start[0] != LV_COORD_MIN && x_end[0] == LV_COORD_MIN) {
                x_end[0] = xi - 1;
            }

            deg = 360 - deg_base;
            if(deg_test(deg, start_angle, end_angle)) {
                if(x_start[1] == LV_COORD_MIN) x_start[1] = xi;
            } else if(x_start[1] != LV_COORD_MIN && x_end[1] == LV_COORD_MIN) {
                x_end[1] = xi - 1;
            }

            deg = 180 - deg_base;
            if(deg_test(deg, start_angle, end_angle)) {
                if(x_start[2] == LV_COORD_MIN) x_start[2] = xi;
            } else if(x_start[2] != LV_COORD_MIN && x_end[2] == LV_COORD_MIN) {
                x_end[2] = xi - 1;
            }

            deg = deg_base;
            if(deg_test(deg, start_angle, end_angle)) {
                if(x_start[3] == LV_COORD_MIN) x_start[3] = xi;
            } else if(x_start[3] != LV_COORD_MIN && x_end[3] == LV_COORD_MIN) {
                x_end[3] = xi - 1;
            }

            if(r_act_sqr < r_in_sqr) {
                xe = xi;
                break; /*No need to continue the iteration in x once we found the inner edge of the
                          arc*/
            }
        }

        if(x_start[0] != LV_COORD_MIN) {
            if(x_end[0] == LV_COORD_MIN) x_end[0] = xe - 1;
            hor_line(center_x + x_start[0], center_y + yi, mask, x_end[0] - x_start[0], color, opa);
        }

        if(x_start[1] != LV_COORD_MIN) {
            if(x_end[1] == LV_COORD_MIN) x_end[1] = xe - 1;
            hor_line(center_x + x_start[1], center_y - yi, mask, x_end[1] - x_start[1], color, opa);
        }

        if(x_start[2] != LV_COORD_MIN) {
            if(x_end[2] == LV_COORD_MIN) x_end[2] = xe - 1;
            hor_line(center_x - x_end[2], center_y + yi, mask, LV_MATH_ABS(x_end[2] - x_start[2]), color, opa);
        }

        if(x_start[3] != LV_COORD_MIN) {
            if(x_end[3] == LV_COORD_MIN) x_end[3] = xe - 1;
            hor_line(center_x - x_end[3], center_y - yi, mask, LV_MATH_ABS(x_end[3] - x_start[3]), color, opa);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void ver_line(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_coord_t len, lv_color_t color, lv_opa_t opa)
{
    lv_area_t area;
    lv_area_set(&area, x, y, x, y + len);

    lv_draw_fill(&area, mask, color, opa);
}

static void hor_line(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_coord_t len, lv_color_t color, lv_opa_t opa)
{
    lv_area_t area;
    lv_area_set(&area, x, y, x + len, y);

    lv_draw_fill(&area, mask, color, opa);
}

static bool deg_test_norm(uint16_t deg, uint16_t start, uint16_t end)
{
    if(deg >= start && deg <= end)
        return true;
    else
        return false;
}

static bool deg_test_inv(uint16_t deg, uint16_t start, uint16_t end)
{
    if(deg >= start || deg <= end) {
        return true;
    } else
        return false;
}
