///**
// * @file lv_draw_arc.c
// *
// */
//
///*********************
// *      INCLUDES
// *********************/
//#include "lv_draw_arc.h"
//#include "lv_draw_mask.h"
//#include "../lv_misc/lv_math.h"
//
///*********************
// *      DEFINES
// *********************/
//
///**********************
// *      TYPEDEFS
// **********************/
//
///**********************
// *  STATIC PROTOTYPES
// **********************/
//static void get_rounded_area(int16_t angle, lv_coord_t radius, uint8_t tickness, lv_area_t * res_area);
//
///**********************
// *  STATIC VARIABLES
// **********************/
//
///**********************
// *      MACROS
// **********************/
//
///**********************
// *   GLOBAL FUNCTIONS
// **********************/
//
///**
// * Draw an arc. (Can draw pie too with great thickness.)
// * @param center_x the x coordinate of the center of the arc
// * @param center_y the y coordinate of the center of the arc
// * @param radius the radius of the arc
// * @param mask the arc will be drawn only in this mask
// * @param start_angle the start angle of the arc (0 deg on the bottom, 90 deg on the right)
// * @param end_angle the end angle of the arc
// * @param style style of the arc (`body.thickness`, `body.main_color`, `body.opa` is used)
// * @param opa_scale scale down all opacities by the factor
// */
//void lv_draw_arc(lv_coord_t center_x, lv_coord_t center_y, uint16_t radius, const lv_area_t * clip_area,
//        uint16_t start_angle, uint16_t end_angle, const lv_style_t * style, lv_opa_t opa_scale)
//{
//    lv_style_t circle_style;
//    lv_style_copy(&circle_style, style);
//    circle_style.body.radius = LV_RADIUS_CIRCLE;
//    circle_style.body.opa = LV_OPA_TRANSP;
//    circle_style.body.border.width = style->line.width;
//    circle_style.body.border.color = style->line.color;
//    circle_style.body.border.opa = style->line.opa;
//
//    lv_draw_mask_angle_param_t mask_angle_param;
//    lv_draw_mask_angle_init(&mask_angle_param, center_x, center_y, start_angle, end_angle);
//
//    int16_t mask_angle_id = lv_draw_mask_add(&mask_angle_param, NULL);
//
//    lv_area_t area;
//    area.x1 = center_x - radius;
//    area.y1 = center_y - radius;
//    area.x2 = center_x + radius - 1;  /*-1 because the center already belongs to the left/bottom part*/
//    area.y2 = center_y + radius - 1;
//
//    lv_draw_rect(&area, clip_area, &circle_style, LV_OPA_COVER);
//
//    lv_draw_mask_remove_id(mask_angle_id);
//
//    if(style->line.rounded) {
//        circle_style.body.main_color = style->line.color;
//        circle_style.body.grad_color = style->line.color;
//        circle_style.body.opa        = LV_OPA_COVER;
//        circle_style.body.border.width = 0;
//
//        lv_area_t round_area;
//        get_rounded_area(start_angle, radius, style->line.width, &round_area);
//        round_area.x1 += center_x;
//        round_area.x2 += center_x;
//        round_area.y1 += center_y;
//        round_area.y2 += center_y;
//
//        lv_draw_rect(&round_area, clip_area, &circle_style, opa_scale);
//
//        get_rounded_area(end_angle, radius, style->line.width, &round_area);
//        round_area.x1 += center_x;
//        round_area.x2 += center_x;
//        round_area.y1 += center_y;
//        round_area.y2 += center_y;
//
//        lv_draw_rect(&round_area, clip_area, &circle_style, opa_scale);
//    }
//}
//
//
///**********************
// *   STATIC FUNCTIONS
// **********************/
//
//static void get_rounded_area(int16_t angle, lv_coord_t radius, uint8_t tickness, lv_area_t * res_area)
//{
//    const uint8_t ps = 8;
//    const uint8_t pa = 127;
//
//    lv_coord_t thick_half = tickness / 2;
//    lv_coord_t thick_corr = tickness & 0x01 ? 0 : 1;
//
//    lv_coord_t rx_corr;
//    lv_coord_t ry_corr;
//
//    if(angle > 90 && angle < 270) rx_corr = 0;
//    else  rx_corr = 0;
//
//    if(angle > 0 && angle < 180) ry_corr = 0;
//    else  ry_corr = 0;
//
//    lv_coord_t cir_x;
//    lv_coord_t cir_y;
//
//    cir_x = ((radius - rx_corr - thick_half) * lv_trigo_sin(90 - angle)) >> (LV_TRIGO_SHIFT - ps);
//    cir_y = ((radius - ry_corr - thick_half) * lv_trigo_sin(angle)) >> (LV_TRIGO_SHIFT - ps);
//
//    /* Actually the center of the pixel need to be calculated so apply 1/2 px offset*/
//    if(cir_x > 0) {
//        cir_x = (cir_x - pa) >> ps;
//        res_area->x1 = cir_x - thick_half + thick_corr;
//        res_area->x2 = cir_x + thick_half;
//    }
//    else {
//        cir_x = (cir_x + pa) >> ps;
//        res_area->x1 = cir_x - thick_half;
//        res_area->x2 = cir_x + thick_half - thick_corr;
//    }
//
//    if(cir_y > 0) {
//        cir_y = (cir_y - pa) >> ps;
//        res_area->y1 = cir_y - thick_half + thick_corr;
//        res_area->y2 = cir_y + thick_half;
//    }
//    else {
//        cir_y = (cir_y + pa) >> ps;
//        res_area->y1 = cir_y - thick_half;
//        res_area->y2 = cir_y + thick_half - thick_corr;
//    }
//}
