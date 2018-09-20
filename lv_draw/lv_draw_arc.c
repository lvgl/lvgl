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
static uint16_t fast_atan2(int x, int y);
static void ver_line(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_coord_t len, lv_color_t color, lv_opa_t opa);
static void hor_line(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_coord_t len, lv_color_t color, lv_opa_t opa);
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

    lv_coord_t r_out = radius;
    lv_coord_t r_in = r_out - thickness;
    int16_t deg_base;
    int16_t deg;
    lv_coord_t x_start[4];
    lv_coord_t x_end[4];

    lv_color_t color = style->line.color;
    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->body.opa : (uint16_t)((uint16_t) style->body.opa * opa_scale) >> 8;


    bool (*deg_test)(uint16_t, uint16_t, uint16_t);
    if(start_angle <= end_angle) deg_test = deg_test_norm;
    else deg_test = deg_test_inv;

    if(deg_test(270, start_angle, end_angle))  hor_line(center_x - r_out + 1, center_y, mask, thickness - 1, color, opa);   // Left Middle
    if(deg_test(90, start_angle, end_angle))   hor_line(center_x + r_in, center_y,  mask, thickness - 1, color, opa);       // Right Middle
    if(deg_test(180, start_angle, end_angle))  ver_line(center_x, center_y - r_out + 1,  mask, thickness - 1, color, opa);  // Top Middle
    if(deg_test(0, start_angle, end_angle))    ver_line(center_x, center_y + r_in,  mask, thickness - 1, color, opa);       // Bottom middle

    uint32_t r_out_sqr = r_out * r_out;
    uint32_t r_in_sqr = r_in * r_in;
    int16_t xi;
    int16_t yi;
    for(yi = -r_out; yi < 0; yi++) {
        x_start[0] = LV_COORD_MIN;
        x_start[1] = LV_COORD_MIN;
        x_start[2] = LV_COORD_MIN;
        x_start[3] = LV_COORD_MIN;
        x_end[0] = LV_COORD_MIN;
        x_end[1] = LV_COORD_MIN;
        x_end[2] = LV_COORD_MIN;
        x_end[3] = LV_COORD_MIN;
        for(xi = -r_out; xi < 0; xi++) {

            uint32_t r_act_sqr = xi * xi + yi * yi;
            if(r_act_sqr > r_out_sqr) continue;

            deg_base =  fast_atan2(xi, yi) - 180;

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

            if(r_act_sqr < r_in_sqr) break; /*No need to continue the iteration in x once we found the inner edge of the arc*/
        }


        if(x_start[0] != LV_COORD_MIN) {
            if(x_end[0] == LV_COORD_MIN) x_end[0] = xi - 1;
            hor_line(center_x + x_start[0], center_y + yi, mask, x_end[0] - x_start[0], color, opa);
        }

        if(x_start[1] != LV_COORD_MIN) {
            if(x_end[1] == LV_COORD_MIN) x_end[1] = xi - 1;
            hor_line(center_x + x_start[1], center_y - yi, mask, x_end[1] - x_start[1], color, opa);
        }

        if(x_start[2] != LV_COORD_MIN) {
            if(x_end[2] == LV_COORD_MIN) x_end[2] = xi - 1;
            hor_line(center_x - x_end[2], center_y + yi, mask, LV_MATH_ABS(x_end[2] - x_start[2]), color, opa);
        }

        if(x_start[3] != LV_COORD_MIN) {
            if(x_end[3] == LV_COORD_MIN) x_end[3] = xi - 1;
            hor_line(center_x - x_end[3], center_y - yi, mask, LV_MATH_ABS(x_end[3] - x_start[3]), color, opa);
        }


#if LV_ANTIALIAS
        /*TODO*/

#endif

    }
}

static uint16_t fast_atan2(int x, int y)
{
    // Fast XY vector to integer degree algorithm - Jan 2011 www.RomanBlack.com
    // Converts any XY values including 0 to a degree value that should be
    // within +/- 1 degree of the accurate value without needing
    // large slow trig functions like ArcTan() or ArcCos().
    // NOTE! at least one of the X or Y values must be non-zero!
    // This is the full version, for all 4 quadrants and will generate
    // the angle in integer degrees from 0-360.
    // Any values of X and Y are usable including negative values provided
    // they are between -1456 and 1456 so the 16bit multiply does not overflow.

    unsigned char negflag;
    unsigned char tempdegree;
    unsigned char comp;
    unsigned int degree;     // this will hold the result
    //signed int x;            // these hold the XY vector at the start
    //signed int y;            // (and they will be destroyed)
    unsigned int ux;
    unsigned int uy;

    // Save the sign flags then remove signs and get XY as unsigned ints
    negflag = 0;
    if(x < 0) {
        negflag += 0x01;    // x flag bit
        x = (0 - x);        // is now +
    }
    ux = x;                // copy to unsigned var before multiply
    if(y < 0) {
        negflag += 0x02;    // y flag bit
        y = (0 - y);        // is now +
    }
    uy = y;                // copy to unsigned var before multiply

    // 1. Calc the scaled "degrees"
    if(ux > uy) {
        degree = (uy * 45) / ux;   // degree result will be 0-45 range
        negflag += 0x10;    // octant flag bit
    } else {
        degree = (ux * 45) / uy;   // degree result will be 0-45 range
    }

    // 2. Compensate for the 4 degree error curve
    comp = 0;
    tempdegree = degree;    // use an unsigned char for speed!
    if(tempdegree > 22) {    // if top half of range
        if(tempdegree <= 44) comp++;
        if(tempdegree <= 41) comp++;
        if(tempdegree <= 37) comp++;
        if(tempdegree <= 32) comp++;  // max is 4 degrees compensated
    } else { // else is lower half of range
        if(tempdegree >= 2) comp++;
        if(tempdegree >= 6) comp++;
        if(tempdegree >= 10) comp++;
        if(tempdegree >= 15) comp++;  // max is 4 degrees compensated
    }
    degree += comp;   // degree is now accurate to +/- 1 degree!

    // Invert degree if it was X>Y octant, makes 0-45 into 90-45
    if(negflag & 0x10) degree = (90 - degree);

    // 3. Degree is now 0-90 range for this quadrant,
    // need to invert it for whichever quadrant it was in
    if(negflag & 0x02) { // if -Y
        if(negflag & 0x01)   // if -Y -X
            degree = (180 + degree);
        else        // else is -Y +X
            degree = (180 - degree);
    } else { // else is +Y
        if(negflag & 0x01)   // if +Y -X
            degree = (360 - degree);
    }
    return degree;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void ver_line(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_coord_t len, lv_color_t color, lv_opa_t opa)
{
    lv_area_t area;
    lv_area_set(&area, x, y, x, y + len);

    fill_fp(&area, mask, color, opa);
}

static void hor_line(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_coord_t len, lv_color_t color, lv_opa_t opa)
{
    lv_area_t area;
    lv_area_set(&area, x, y, x + len, y);

    fill_fp(&area, mask, color, opa);
}

static bool deg_test_norm(uint16_t deg, uint16_t start, uint16_t end)
{
    if(deg >= start && deg <= end) return true;
    else return false;
}

static bool deg_test_inv(uint16_t deg, uint16_t start, uint16_t end)
{
    if(deg >= start || deg <= end) {
        return true;
    } else return false;
}
