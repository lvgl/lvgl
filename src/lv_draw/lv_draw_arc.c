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
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define SPLIT_RADIUS_LIMIT 10  /*With radius greater then this the arc will drawn in quarters. A quarter is drawn only if there is arc in it */
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
static void draw_quarter_0(quarter_draw_dsc_t * q);
static void draw_quarter_1(quarter_draw_dsc_t * q);
static void draw_quarter_2(quarter_draw_dsc_t * q);
static void draw_quarter_3(quarter_draw_dsc_t * q);
static void get_rounded_area(int16_t angle, lv_coord_t radius, uint8_t thickness, lv_area_t * res_area);


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
 * @param clip_area the arc will be drawn only in this area
 * @param dsc pointer to an initialized `lv_draw_line_dsc_t` variable
 */
void lv_draw_arc(lv_coord_t center_x, lv_coord_t center_y, uint16_t radius,  uint16_t start_angle, uint16_t end_angle,
                 const lv_area_t * clip_area, const lv_draw_line_dsc_t * dsc)
{
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(start_angle == end_angle) return;

    lv_style_int_t width = dsc->width;
    if(width > radius) width = radius;

    lv_draw_rect_dsc_t cir_dsc;
    lv_draw_rect_dsc_init(&cir_dsc);
    cir_dsc.radius = LV_RADIUS_CIRCLE;
    cir_dsc.bg_opa = LV_OPA_TRANSP;
    cir_dsc.border_opa = dsc->opa;
    cir_dsc.border_color = dsc->color;
    cir_dsc.border_width = width;
    cir_dsc.border_blend_mode = dsc->blend_mode;

    lv_area_t area;
    area.x1 = center_x - radius;
    area.y1 = center_y - radius;
    area.x2 = center_x + radius - 1;  /*-1 because the center already belongs to the left/bottom part*/
    area.y2 = center_y + radius - 1;

    /*Draw a full ring*/
    if(start_angle + 360 == end_angle || start_angle == end_angle + 360) {
        lv_draw_rect(&area, clip_area, &cir_dsc);
        return;
    }

    if(start_angle >= 360) start_angle -= 360;
    if(end_angle >= 360) end_angle -= 360;

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
        q_dsc.draw_area = &area;
        q_dsc.clip_area = clip_area;

        draw_quarter_0(&q_dsc);
        draw_quarter_1(&q_dsc);
        draw_quarter_2(&q_dsc);
        draw_quarter_3(&q_dsc);
    }
    else {
        lv_draw_rect(&area, clip_area, &cir_dsc);
    }
    lv_draw_mask_remove_id(mask_angle_id);

    if(dsc->round_start || dsc->round_end) {
        cir_dsc.bg_color        = dsc->color;
        cir_dsc.bg_opa        = dsc->opa;
        cir_dsc.bg_blend_mode = dsc->blend_mode;
        cir_dsc.border_width = 0;

        lv_area_t round_area;
        if(dsc->round_start) {
            get_rounded_area(start_angle, radius, width, &round_area);
            round_area.x1 += center_x;
            round_area.x2 += center_x;
            round_area.y1 += center_y;
            round_area.y2 += center_y;

            lv_draw_rect(&round_area, clip_area, &cir_dsc);
        }

        if(dsc->round_end) {
            get_rounded_area(end_angle, radius, width, &round_area);
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

static void draw_quarter_0(quarter_draw_dsc_t * q)
{
    lv_area_t quarter_area;

    if(q->start_quarter == 0 && q->end_quarter == 0 && q->start_angle < q->end_angle) {
        /*Small arc here*/
        quarter_area.y1 = q->center_y + ((_lv_trigo_sin(q->start_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
        quarter_area.x2 = q->center_x + ((_lv_trigo_sin(q->start_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

        quarter_area.y2 = q->center_y + ((_lv_trigo_sin(q->end_angle) * q->radius) >> LV_TRIGO_SHIFT);
        quarter_area.x1 = q->center_x + ((_lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
    }
    else if(q->start_quarter == 0 || q->end_quarter == 0) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 0) {
            quarter_area.x1 = q->center_x;
            quarter_area.y2 = q->center_y + q->radius;

            quarter_area.y1 = q->center_y + ((_lv_trigo_sin(q->start_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.x2 = q->center_x + ((_lv_trigo_sin(q->start_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
        }
        if(q->end_quarter == 0) {
            quarter_area.x2 = q->center_x + q->radius;
            quarter_area.y1 = q->center_y;

            quarter_area.y2 = q->center_y + ((_lv_trigo_sin(q->end_angle) * q->radius) >> LV_TRIGO_SHIFT);
            quarter_area.x1 = q->center_x + ((_lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

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
        quarter_area.y2 = q->center_y + ((_lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);
        quarter_area.x2 = q->center_x + ((_lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        quarter_area.y1 = q->center_y + ((_lv_trigo_sin(q->end_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
        quarter_area.x1 = q->center_x + ((_lv_trigo_sin(q->end_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
    }
    else if(q->start_quarter == 1 || q->end_quarter == 1) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 1) {
            quarter_area.x1 = q->center_x - q->radius;
            quarter_area.y1 = q->center_y;

            quarter_area.y2 = q->center_y + ((_lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);
            quarter_area.x2 = q->center_x + ((_lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
        }
        if(q->end_quarter == 1) {
            quarter_area.x2 = q->center_x - 1;
            quarter_area.y2 = q->center_y + q->radius;

            quarter_area.y1 = q->center_y + ((_lv_trigo_sin(q->end_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.x1 = q->center_x + ((_lv_trigo_sin(q->end_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

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
        quarter_area.x1 = q->center_x + ((_lv_trigo_sin(q->start_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);
        quarter_area.y2 = q->center_y + ((_lv_trigo_sin(q->start_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        quarter_area.y1 = q->center_y + ((_lv_trigo_sin(q->end_angle) * q->radius) >> LV_TRIGO_SHIFT);
        quarter_area.x2 = q->center_x + ((_lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
    }
    else if(q->start_quarter == 2 || q->end_quarter == 2) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 2) {
            quarter_area.x2 = q->center_x - 1;
            quarter_area.y1 = q->center_y - q->radius;

            quarter_area.x1 = q->center_x + ((_lv_trigo_sin(q->start_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);
            quarter_area.y2 = q->center_y + ((_lv_trigo_sin(q->start_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
        }
        if(q->end_quarter == 2) {
            quarter_area.x1 = q->center_x - q->radius;
            quarter_area.y2 = q->center_y - 1;

            quarter_area.x2 = q->center_x + ((_lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.y1 = q->center_y + ((_lv_trigo_sin(q->end_angle) * (q->radius)) >> LV_TRIGO_SHIFT);

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
        quarter_area.x1 = q->center_x + ((_lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
        quarter_area.y1 = q->center_y + ((_lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);

        quarter_area.x2 = q->center_x + ((_lv_trigo_sin(q->end_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);
        quarter_area.y2 = q->center_y + ((_lv_trigo_sin(q->end_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
    }
    else if(q->start_quarter == 3 || q->end_quarter == 3) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 3) {
            quarter_area.x2 = q->center_x + q->radius;
            quarter_area.y2 = q->center_y - 1;

            quarter_area.x1 = q->center_x + ((_lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.y1 = q->center_y + ((_lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->draw_dsc);
        }
        if(q->end_quarter == 3) {
            quarter_area.x1 = q->center_x;
            quarter_area.y1 = q->center_y - q->radius;

            quarter_area.x2 = q->center_x + ((_lv_trigo_sin(q->end_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);
            quarter_area.y2 = q->center_y + ((_lv_trigo_sin(q->end_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

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

    cir_x = ((radius - thick_half) * _lv_trigo_sin(90 - angle)) >> (LV_TRIGO_SHIFT - ps);
    cir_y = ((radius - thick_half) * _lv_trigo_sin(angle)) >> (LV_TRIGO_SHIFT - ps);

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

