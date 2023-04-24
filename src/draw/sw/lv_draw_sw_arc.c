/**
 * @file lv_draw_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW
#if LV_USE_DRAW_MASKS

#include "../../misc/lv_math.h"
#include "../../misc/lv_log.h"
#include "../../misc/lv_mem.h"
#include "../lv_draw.h"

static void get_rounded_area(int16_t angle, lv_coord_t radius, uint8_t thickness, lv_area_t * res_area);
void lv_draw_sw_rect(lv_draw_unit_t * draw_unit, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

/*********************
 *      DEFINES
 *********************/
#define SPLIT_RADIUS_LIMIT 10  /*With radius greater than this the arc will drawn in quarters. A quarter is drawn only if there is arc in it*/
#define SPLIT_ANGLE_GAP_LIMIT 60  /*With small gaps in the arc don't bother with splitting because there is nothing to skip.*/

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

void lv_draw_sw_arc(lv_draw_unit_t * draw_unit, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{
#if LV_USE_DRAW_MASKS
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(dsc->start_angle == dsc->end_angle) return;

    lv_coord_t width = dsc->width;
    if(width > dsc->radius) width = dsc->radius;

    lv_draw_rect_dsc_t cir_dsc;
    lv_draw_rect_dsc_init(&cir_dsc);
    cir_dsc.blend_mode = dsc->blend_mode;
    if(dsc->img_src) {
        cir_dsc.bg_opa = LV_OPA_TRANSP;
        cir_dsc.bg_img_src = dsc->img_src;
        cir_dsc.bg_img_opa = dsc->opa;
    }
    else {
        cir_dsc.bg_opa = LV_OPA_TRANSP;
        cir_dsc.border_opa = dsc->opa;
        cir_dsc.border_color = dsc->color;
        cir_dsc.border_width = dsc->width;
    }

    lv_area_t area_out = *coords;

    /*Draw a full ring*/
    if(dsc->start_angle + 360 == dsc->end_angle || dsc->start_angle == dsc->end_angle + 360) {
        cir_dsc.radius = LV_RADIUS_CIRCLE;
        lv_draw_sw_rect(draw_unit, &cir_dsc, &area_out);
        return;
    }

    lv_area_t area_in;
    lv_area_copy(&area_in, &area_out);
    area_in.x1 += dsc->width;
    area_in.y1 += dsc->width;
    area_in.x2 -= dsc->width;
    area_in.y2 -= dsc->width;

    int32_t start_angle = dsc->start_angle;
    int32_t end_angle = dsc->end_angle;
    while(start_angle >= 360) start_angle -= 360;
    while(end_angle >= 360) end_angle -= 360;

    void * mask_list[4] = {0};

    /*Create an angle mask*/
    lv_draw_sw_mask_angle_param_t mask_angle_param;
    lv_draw_sw_mask_angle_init(&mask_angle_param, dsc->center.x, dsc->center.y, start_angle, end_angle);
    mask_list[0] = &mask_angle_param;

    /*Create an outer mask*/
    lv_draw_sw_mask_radius_param_t mask_out_param;
    lv_draw_sw_mask_radius_init(&mask_out_param, &area_out, LV_RADIUS_CIRCLE, false);
    mask_list[1] = &mask_out_param;

    /*Create inner the mask*/
    lv_draw_sw_mask_radius_param_t mask_in_param;
    bool mask_in_param_valid = false;
    if(lv_area_get_width(&area_in) > 0 && lv_area_get_height(&area_in) > 0) {
        lv_draw_sw_mask_radius_init(&mask_in_param, &area_in, LV_RADIUS_CIRCLE, true);
        mask_list[2] = &mask_in_param;
        mask_in_param_valid = true;
    }

    lv_area_t clipped_area;
    if(!_lv_area_intersect(&clipped_area, &area_out, draw_unit->clip_area)) return;

    lv_coord_t blend_h = lv_area_get_height(&clipped_area);
    lv_coord_t blend_w = lv_area_get_width(&clipped_area);
    lv_coord_t h;
    lv_opa_t * mask_buf = lv_malloc(blend_w);

    lv_area_t blend_area = clipped_area;
    lv_draw_sw_blend_dsc_t blend_dsc = {0};
    blend_dsc.color = dsc->color;
    blend_dsc.mask_buf = mask_buf;
    blend_dsc.opa = dsc->opa;
    blend_dsc.blend_area = &blend_area;
    blend_dsc.mask_area = &blend_area;

    blend_area.y2 = blend_area.y1;
    for(h = 0; h < blend_h; h++) {
        lv_memset(mask_buf, 0xff, blend_w);
        blend_dsc.mask_res = lv_draw_sw_mask_apply(mask_list, mask_buf, blend_area.x1, blend_area.y1, blend_w);

        lv_draw_sw_blend(draw_unit, &blend_dsc);

        blend_area.y1 ++;
        blend_area.y2 ++;
    }

    lv_draw_sw_mask_free_param(&mask_angle_param);
    lv_draw_sw_mask_free_param(&mask_out_param);
    if(mask_in_param_valid) {
        lv_draw_sw_mask_free_param(&mask_in_param);
    }
    lv_free(mask_buf);

    if(dsc->rounded) {
        lv_draw_rect_dsc_t end_circle_dsc;
        lv_draw_rect_dsc_init(&end_circle_dsc);
        end_circle_dsc.bg_color = dsc->color;
        end_circle_dsc.radius = LV_RADIUS_CIRCLE;


        lv_area_t round_area;
        get_rounded_area(start_angle, dsc->radius, width, &round_area);
        lv_area_move(&round_area, dsc->center.x, dsc->center.y);
        lv_draw_sw_rect(draw_unit, &end_circle_dsc, &round_area);

        get_rounded_area(end_angle, dsc->radius, width, &round_area);
        lv_area_move(&round_area, dsc->center.x, dsc->center.y);
        lv_draw_sw_rect(draw_unit, &end_circle_dsc, &round_area);
    }
#else
    LV_LOG_WARN("Can't draw arc with LV_USE_DRAW_MASKS == 0");
    LV_UNUSED(center);
    LV_UNUSED(radius);
    LV_UNUSED(start_angle);
    LV_UNUSED(end_angle);
    LV_UNUSED(layer);
    LV_UNUSED(dsc);
#endif /*LV_USE_DRAW_MASKS*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void get_rounded_area(int16_t angle, lv_coord_t radius, uint8_t thickness, lv_area_t * res_area)
{
    const uint8_t ps = 8;
    const uint8_t pa = 127;

    int32_t thick_half = thickness / 2;
    uint8_t thick_corr = (thickness & 0x01) ? 0 : 1;

    int32_t cir_x;
    int32_t cir_y;

    cir_x = ((radius - thick_half) * lv_trigo_sin(90 - angle)) >> (LV_TRIGO_SHIFT - ps);
    cir_y = ((radius - thick_half) * lv_trigo_sin(angle)) >> (LV_TRIGO_SHIFT - ps);

    /*Actually the center of the pixel need to be calculated so apply 1/2 px offset*/
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

#endif /*LV_USE_DRAW_MASKS*/
#endif /*LV_USE_DRAW_SW*/
