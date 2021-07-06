/**
 * @file lv_draw_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_arc.h"
#include "lv_draw_rect.h"
#include "lv_draw_mask.h"
#include "../misc/lv_math.h"
#include "../misc/lv_log.h"
#include "../misc/lv_mem.h"

/*********************
 *      DEFINES
 *********************/
#define SPLIT_RADIUS_LIMIT 10  /*With radius greater then this the arc will drawn in quarters. A quarter is drawn only if there is arc in it*/
#define SPLIT_ANGLE_GAP_LIMIT 60  /*With small gaps in the arc don't bother with splitting because there is nothing to skip.*/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_coord_t center_x;
    lv_coord_t center_y;
    lv_coord_t radius;
    uint16_t start_angle;
    uint16_t end_angle;
    uint16_t start_quarter;
    uint16_t end_quarter;
    lv_coord_t width;
    lv_draw_rect_dsc_t * draw_dsc;
    const lv_area_t * draw_area;
    const lv_area_t * clip_area;
} quarter_draw_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_DRAW_COMPLEX
static void draw_quarter_0(quarter_draw_dsc_t * q);
static void draw_quarter_1(quarter_draw_dsc_t * q);
static void draw_quarter_2(quarter_draw_dsc_t * q);
static void draw_quarter_3(quarter_draw_dsc_t * q);
static void get_rounded_area(int16_t angle, lv_coord_t radius, uint8_t thickness, lv_area_t * res_area);
#endif /*LV_DRAW_COMPLEX*/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_draw_arc_dsc_init(lv_draw_arc_dsc_t * dsc)
{
    lv_memset_00(dsc, sizeof(lv_draw_arc_dsc_t));
    dsc->width = 1;
    dsc->opa = LV_OPA_COVER;
    dsc->color = lv_color_black();
}

void lv_draw_arc(lv_coord_t center_x, lv_coord_t center_y, uint16_t radius,  uint16_t start_angle, uint16_t end_angle,
                 const lv_area_t * clip_area, const lv_draw_arc_dsc_t * dsc)
{
#if LV_DRAW_COMPLEX
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(start_angle == end_angle) return;

    lv_coord_t width = dsc->width;
    if(width > radius) width = radius;

    lv_draw_rect_dsc_t cir_dsc;
    lv_draw_rect_dsc_init(&cir_dsc);
    cir_dsc.blend_mode = dsc->blend_mode;
    if(dsc->img_src) {
        cir_dsc.bg_opa = LV_OPA_TRANSP;
        cir_dsc.bg_img_src = dsc->img_src;
        cir_dsc.bg_img_opa = dsc->opa;
    } else {
        cir_dsc.bg_opa = dsc->opa;
        cir_dsc.bg_color = dsc->color;
    }

    lv_area_t area_out;
    area_out.x1 = center_x - radius;
    area_out.y1 = center_y - radius;
    area_out.x2 = center_x + radius - 1;  /*-1 because the center already belongs to the left/bottom part*/
    area_out.y2 = center_y + radius - 1;

    lv_area_t area_in;
    lv_area_copy(&area_in, &area_out);
    area_in.x1 += dsc->width;
    area_in.y1 += dsc->width;
    area_in.x2 -= dsc->width;
    area_in.y2 -= dsc->width;

    /*Create inner the mask*/
    lv_draw_mask_radius_param_t mask_in_param;
    lv_draw_mask_radius_init(&mask_in_param, &area_in, LV_RADIUS_CIRCLE, true);
    int16_t mask_in_id = lv_draw_mask_add(&mask_in_param, NULL);

    lv_draw_mask_radius_param_t mask_out_param;
    lv_draw_mask_radius_init(&mask_out_param, &area_out, LV_RADIUS_CIRCLE, false);
    int16_t mask_out_id = lv_draw_mask_add(&mask_out_param, NULL);

    /*Draw a full ring*/
    if(start_angle + 360 == end_angle || start_angle == end_angle + 360) {
        cir_dsc.radius = LV_RADIUS_CIRCLE;
        lv_draw_rect(&area_out, clip_area, &cir_dsc);

        lv_draw_mask_remove_id(mask_out_id);
        lv_draw_mask_remove_id(mask_in_id);
        return;
    }

    while(start_angle >= 360) start_angle -= 360;
    while(end_angle >= 360) end_angle -= 360;

    lv_draw_mask_angle_param_t mask_angle_param;
    lv_draw_mask_angle_init(&mask_angle_param, center_x, center_y, start_angle, end_angle);
    int16_t mask_angle_id = lv_draw_mask_add(&mask_angle_param, NULL);

    int32_t angle_gap;
    if(end_angle > start_angle) {
        angle_gap = 360 - (end_angle - start_angle);
    }
    else {
        angle_gap = start_angle - end_angle;
    }
    if(angle_gap > SPLIT_ANGLE_GAP_LIMIT && radius > SPLIT_RADIUS_LIMIT) {
        /*Handle each quarter individually and skip which is empty*/
        quarter_draw_dsc_t q_dsc;
        q_dsc.center_x = center_x;
        q_dsc.center_y = center_y;
        q_dsc.radius = radius;
        q_dsc.start_angle = start_angle;
        q_dsc.end_angle = end_angle;
        q_dsc.start_quarter = (start_angle / 90) & 0x3;
        q_dsc.end_quarter = (end_angle / 90) & 0x3;
        q_dsc.width = width;
        q_dsc.draw_dsc =  &cir_dsc;
        q_dsc.draw_area = &area_out;
        q_dsc.clip_area = clip_area;

        draw_quarter_0(&q_dsc);
        draw_quarter_1(&q_dsc);
        draw_quarter_2(&q_dsc);
        draw_quarter_3(&q_dsc);
    }
    else {
        lv_draw_rect(&area_out, clip_area, &cir_dsc);
    }
    lv_draw_mask_remove_id(mask_angle_id);
    lv_draw_mask_remove_id(mask_out_id);
    lv_draw_mask_remove_id(mask_in_id);

    if(dsc->rounded) {

        lv_draw_mask_radius_param_t mask_end_param;

        lv_area_t round_area;
        get_rounded_area(start_angle, radius, width, &round_area);
        round_area.x1 += center_x;
        round_area.x2 += center_x;
        round_area.y1 += center_y;
        round_area.y2 += center_y;
        lv_area_t clip_area2;
        if(_lv_area_intersect(&clip_area2, clip_area, &round_area)) {
            lv_draw_mask_radius_init(&mask_end_param, &round_area, LV_RADIUS_CIRCLE, false);
            int16_t mask_end_id = lv_draw_mask_add(&mask_end_param, NULL);

            lv_draw_rect(&area_out, &clip_area2, &cir_dsc);
            lv_draw_mask_remove_id(mask_end_id);
        }

        get_rounded_area(end_angle, radius, width, &round_area);
        round_area.x1 += center_x;
        round_area.x2 += center_x;
        round_area.y1 += center_y;
        round_area.y2 += center_y;
        if(_lv_area_intersect(&clip_area2, clip_area, &round_area)) {
            lv_draw_mask_radius_init(&mask_end_param, &round_area, LV_RADIUS_CIRCLE, false);
            int16_t mask_end_id = lv_draw_mask_add(&mask_end_param, NULL);

            lv_draw_rect(&area_out, &clip_area2, &cir_dsc);
            lv_draw_mask_remove_id(mask_end_id);
        }
    }
#else
    LV_LOG_WARN("Can't draw arc with LV_DRAW_COMPLEX == 0");
    LV_UNUSED(center_x);
    LV_UNUSED(center_y);
    LV_UNUSED(radius);
    LV_UNUSED(start_angle);
    LV_UNUSED(end_angle);
    LV_UNUSED(clip_area);
    LV_UNUSED(dsc);
#endif /*LV_DRAW_COMPLEX*/
}

void lv_draw_arc_get_area(lv_coord_t x, lv_coord_t y, uint16_t radius,  uint16_t start_angle, uint16_t end_angle, lv_coord_t w, bool rounded, lv_area_t * area)
{
    lv_coord_t rout = radius;
    lv_coord_t rin = radius - w;
    lv_coord_t extra_area = rounded ? w : 0;
    uint8_t start_quarter = start_angle / 90;
    uint8_t end_quarter = end_angle / 90;


    if(start_quarter == end_quarter && start_angle <= end_angle) {
        if(start_quarter == 0) {
            area->y1 = y + ((lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            area->x2 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;

            area->y2 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            area->x1 = x + ((lv_trigo_sin(end_angle + 90) * rin) >> LV_TRIGO_SHIFT) - extra_area;
        }
        else if(start_quarter == 1) {
            area->y2 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            area->x2 = x + ((lv_trigo_sin(start_angle + 90) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            area->y1 = y + ((lv_trigo_sin(end_angle) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            area->x1 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        }
        else if(start_quarter == 2) {
            area->x1 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
            area->y2 = y + ((lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            area->y1 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
            area->x2 = x + ((lv_trigo_sin(end_angle + 90) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        }
        else if(start_quarter == 3) {
            area->x1 = x + ((lv_trigo_sin(start_angle + 90) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            area->y1 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;

            area->x2 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            area->y2 = y + ((lv_trigo_sin(end_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        }
    }
    else if(start_quarter == 0 && end_quarter == 1) {
        area->x1 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        area->y1 = y + ((LV_MIN(lv_trigo_sin(end_angle),
                                        lv_trigo_sin(start_angle))  * rin) >> LV_TRIGO_SHIFT) - extra_area;
        area->x2 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        area->y2 = y + rout + extra_area;
    }
    else if(start_quarter == 1 && end_quarter == 2) {
        area->x1 = x - rout - extra_area;
        area->y1 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        area->x2 = x + ((LV_MAX(lv_trigo_sin(start_angle + 90),
                                        lv_trigo_sin(end_angle + 90)) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        area->y2 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
    }
    else if(start_quarter == 2 && end_quarter == 3) {
        area->x1 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        area->y1 = y - rout - extra_area;
        area->x2 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        area->y2 = y + (LV_MAX(lv_trigo_sin(end_angle) * rin,
                                       lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;
    }
    else if(start_quarter == 3 && end_quarter == 0) {
        area->x1 = x + ((LV_MIN(lv_trigo_sin(end_angle + 90),
                                        lv_trigo_sin(start_angle + 90)) * rin) >> LV_TRIGO_SHIFT) - extra_area;
        area->y1 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        area->x2 = x + rout + extra_area;
        area->y2 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;

    }
    else {
        area->x1 = x - rout;
        area->y1 = y - rout;
        area->x2 = x + rout;
        area->y2 = y + rout;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_DRAW_COMPLEX
static void draw_quarter_0(quarter_draw_dsc_t * q)
{
    lv_area_t quarter_area;

    if(q->start_quarter == 0 && q->end_quarter == 0 && q->start_angle < q->end_angle) {
        /*Small arc here*/
        quarter_area.y1 = q->center_y + ((lv_trigo_sin(q->start_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
        quarter_area.x2 = q->center_x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

        quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->end_angle) * q->radius) >> LV_TRIGO_SHIFT);
        quarter_area.x1 = q->center_x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
    }
    else if(q->start_quarter == 0 || q->end_quarter == 0) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 0) {
            quarter_area.x1 = q->center_x;
            quarter_area.y2 = q->center_y + q->radius;

            quarter_area.y1 = q->center_y + ((lv_trigo_sin(q->start_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.x2 = q->center_x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
        }
        if(q->end_quarter == 0) {
            quarter_area.x2 = q->center_x + q->radius;
            quarter_area.y1 = q->center_y;

            quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->end_angle) * q->radius) >> LV_TRIGO_SHIFT);
            quarter_area.x1 = q->center_x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
        }
    }
    else if((q->start_quarter == q->end_quarter && q->start_quarter != 0 && q->end_angle < q->start_angle) ||
            (q->start_quarter == 2 && q->end_quarter == 1) ||
            (q->start_quarter == 3 && q->end_quarter == 2) ||
            (q->start_quarter == 3 && q->end_quarter == 1)) {
        /*Arc crosses here*/
        quarter_area.x1 = q->center_x;
        quarter_area.y1 = q->center_y;
        quarter_area.x2 = q->center_x + q->radius;
        quarter_area.y2 = q->center_y + q->radius;

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
    }
}

static void draw_quarter_1(quarter_draw_dsc_t * q)
{
    lv_area_t quarter_area;

    if(q->start_quarter == 1 && q->end_quarter == 1 && q->start_angle < q->end_angle) {
        /*Small arc here*/
        quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);
        quarter_area.x2 = q->center_x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        quarter_area.y1 = q->center_y + ((lv_trigo_sin(q->end_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
        quarter_area.x1 = q->center_x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
    }
    else if(q->start_quarter == 1 || q->end_quarter == 1) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 1) {
            quarter_area.x1 = q->center_x - q->radius;
            quarter_area.y1 = q->center_y;

            quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);
            quarter_area.x2 = q->center_x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
        }
        if(q->end_quarter == 1) {
            quarter_area.x2 = q->center_x - 1;
            quarter_area.y2 = q->center_y + q->radius;

            quarter_area.y1 = q->center_y + ((lv_trigo_sin(q->end_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.x1 = q->center_x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
        }
    }
    else if((q->start_quarter == q->end_quarter && q->start_quarter != 1 && q->end_angle < q->start_angle) ||
            (q->start_quarter == 0 && q->end_quarter == 2) ||
            (q->start_quarter == 0 && q->end_quarter == 3) ||
            (q->start_quarter == 3 && q->end_quarter == 2)) {
        /*Arc crosses here*/
        quarter_area.x1 = q->center_x - q->radius;
        quarter_area.y1 = q->center_y;
        quarter_area.x2 = q->center_x - 1;
        quarter_area.y2 = q->center_y + q->radius;

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
    }
}

static void draw_quarter_2(quarter_draw_dsc_t * q)
{
    lv_area_t quarter_area;

    if(q->start_quarter == 2 && q->end_quarter == 2 && q->start_angle < q->end_angle) {
        /*Small arc here*/
        quarter_area.x1 = q->center_x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);
        quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->start_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        quarter_area.y1 = q->center_y + ((lv_trigo_sin(q->end_angle) * q->radius) >> LV_TRIGO_SHIFT);
        quarter_area.x2 = q->center_x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
    }
    else if(q->start_quarter == 2 || q->end_quarter == 2) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 2) {
            quarter_area.x2 = q->center_x - 1;
            quarter_area.y1 = q->center_y - q->radius;

            quarter_area.x1 = q->center_x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);
            quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->start_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
        }
        if(q->end_quarter == 2) {
            quarter_area.x1 = q->center_x - q->radius;
            quarter_area.y2 = q->center_y - 1;

            quarter_area.x2 = q->center_x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.y1 = q->center_y + ((lv_trigo_sin(q->end_angle) * (q->radius)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
        }
    }
    else if((q->start_quarter == q->end_quarter && q->start_quarter != 2 && q->end_angle < q->start_angle) ||
            (q->start_quarter == 0 && q->end_quarter == 3) ||
            (q->start_quarter == 1 && q->end_quarter == 3) ||
            (q->start_quarter == 1 && q->end_quarter == 0)) {
        /*Arc crosses here*/
        quarter_area.x1 = q->center_x - q->radius;
        quarter_area.y1 = q->center_y - q->radius;
        quarter_area.x2 = q->center_x - 1;
        quarter_area.y2 = q->center_y - 1;

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
    }
}

static void draw_quarter_3(quarter_draw_dsc_t * q)
{
    lv_area_t quarter_area;

    if(q->start_quarter == 3 && q->end_quarter == 3 && q->start_angle < q->end_angle) {
        /*Small arc here*/
        quarter_area.x1 = q->center_x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
        quarter_area.y1 = q->center_y + ((lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);

        quarter_area.x2 = q->center_x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);
        quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->end_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
    }
    else if(q->start_quarter == 3 || q->end_quarter == 3) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 3) {
            quarter_area.x2 = q->center_x + q->radius;
            quarter_area.y2 = q->center_y - 1;

            quarter_area.x1 = q->center_x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.y1 = q->center_y + ((lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
        }
        if(q->end_quarter == 3) {
            quarter_area.x1 = q->center_x;
            quarter_area.y1 = q->center_y - q->radius;

            quarter_area.x2 = q->center_x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);
            quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->end_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
        }
    }
    else if((q->start_quarter == q->end_quarter && q->start_quarter != 3 && q->end_angle < q->start_angle) ||
            (q->start_quarter == 2 && q->end_quarter == 0) ||
            (q->start_quarter == 1 && q->end_quarter == 0) ||
            (q->start_quarter == 2 && q->end_quarter == 1)) {
        /*Arc crosses here*/
        quarter_area.x1 = q->center_x;
        quarter_area.y1 = q->center_y - q->radius;
        quarter_area.x2 = q->center_x + q->radius;
        quarter_area.y2 = q->center_y - 1;

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
    }
}

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

#endif /*LV_DRAW_COMPLEX*/
