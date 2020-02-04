/**
 * @file lv_draw_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_arc.h"
#include "lv_draw_mask.h"
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
static void get_rounded_area(int16_t angle, lv_coord_t radius, uint8_t tickness, lv_area_t * res_area);

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
void lv_draw_arc(lv_coord_t center_x, lv_coord_t center_y, uint16_t radius,  uint16_t start_angle, uint16_t end_angle, const lv_area_t * clip_area, lv_draw_line_dsc_t * dsc)
{
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;

    lv_draw_rect_dsc_t cir_dsc;
    lv_draw_rect_dsc_init(&cir_dsc);
    cir_dsc.radius = LV_RADIUS_CIRCLE;
    cir_dsc.bg_opa = LV_OPA_TRANSP;
    cir_dsc.border_opa = dsc->opa;
    cir_dsc.border_color = dsc->color;
    cir_dsc.border_width = dsc->width;
    cir_dsc.border_blend_mode = dsc->blend_mode;

    lv_draw_mask_angle_param_t mask_angle_param;
    lv_draw_mask_angle_init(&mask_angle_param, center_x, center_y, start_angle, end_angle);

    int16_t mask_angle_id = lv_draw_mask_add(&mask_angle_param, NULL);

    lv_area_t area;
    area.x1 = center_x - radius;
    area.y1 = center_y - radius;
    area.x2 = center_x + radius - 1;  /*-1 because the center already belongs to the left/bottom part*/
    area.y2 = center_y + radius - 1;

    lv_draw_rect(&area, clip_area, &cir_dsc);

    lv_draw_mask_remove_id(mask_angle_id);

    if(dsc->round_start || dsc->round_end) {
        cir_dsc.bg_color        = dsc->color;
        cir_dsc.bg_opa        = dsc->opa;
        cir_dsc.bg_blend_mode = dsc->blend_mode;
        cir_dsc.border_width = 0;

        lv_area_t round_area;
        if(dsc->round_start) {
            get_rounded_area(start_angle, radius, dsc->width, &round_area);
            round_area.x1 += center_x;
            round_area.x2 += center_x;
            round_area.y1 += center_y;
            round_area.y2 += center_y;

            lv_draw_rect(&round_area, clip_area, &cir_dsc);
        }

        if(dsc->round_end) {
            get_rounded_area(end_angle, radius, dsc->width, &round_area);
            round_area.x1 += center_x;
            round_area.x2 += center_x;
            round_area.y1 += center_y;
            round_area.y2 += center_y;

            lv_draw_rect(&round_area, clip_area, &cir_dsc);
        }
    }
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void get_rounded_area(int16_t angle, lv_coord_t radius, uint8_t tickness, lv_area_t * res_area)
{
    const uint8_t ps = 8;
    const uint8_t pa = 127;

    int32_t thick_half = tickness / 2;
    uint8_t thick_corr = tickness & 0x01 ? 0 : 1;

    int32_t rx_corr;
    int32_t ry_corr;

    if(angle > 90 && angle < 270) rx_corr = 0;
    else  rx_corr = 0;

    if(angle > 0 && angle < 180) ry_corr = 0;
    else  ry_corr = 0;

    int32_t cir_x;
    int32_t cir_y;

    cir_x = ((radius - rx_corr - thick_half) * lv_trigo_sin(90 - angle)) >> (LV_TRIGO_SHIFT - ps);
    cir_y = ((radius - ry_corr - thick_half) * lv_trigo_sin(angle)) >> (LV_TRIGO_SHIFT - ps);

    /* Actually the center of the pixel need to be calculated so apply 1/2 px offset*/
    if(cir_x > 0) {
        cir_x = (cir_x - pa) >> ps;
        res_area->x1 = cir_x - thick_half + thick_corr;
        res_area->x2 = cir_x + thick_half;
    }
    else {
        cir_x = (cir_x + pa) >> ps;
        res_area->x1 = cir_x - thick_half;
        res_area->x2 = cir_x + thick_half - thick_corr;
    }

    if(cir_y > 0) {
        cir_y = (cir_y - pa) >> ps;
        res_area->y1 = cir_y - thick_half + thick_corr;
        res_area->y2 = cir_y + thick_half;
    }
    else {
        cir_y = (cir_y + pa) >> ps;
        res_area->y1 = cir_y - thick_half;
        res_area->y2 = cir_y + thick_half - thick_corr;
    }
}
