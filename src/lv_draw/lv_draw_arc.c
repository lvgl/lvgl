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
#define SPLIT_RADIUS_LIMIT 10  /*With radius greater then this the arc will drawn in quarters. A quarter is drawn only if there is arc in it */
#define SPLIT_ANGLE_GAP_LIMIT 60  /*With small gaps in the arc don't bother with splitting because there is nothing to skip. */


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
    lv_opa_t opa_scale;
    lv_style_t * style;
    const lv_area_t * draw_area;
    const lv_area_t * clip_area;
}quarter_draw_dsc_t;


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void draw_quarter_0(quarter_draw_dsc_t*  q);
static void draw_quarter_1(quarter_draw_dsc_t*  q);
static void draw_quarter_2(quarter_draw_dsc_t*  q);
static void draw_quarter_3(quarter_draw_dsc_t*  q);
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
void lv_draw_arc(lv_coord_t center_x, lv_coord_t center_y, uint16_t radius, const lv_area_t * clip_area,
        uint16_t start_angle, uint16_t end_angle, const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_style_t circle_style;
    lv_style_copy(&circle_style, style);
    circle_style.body.radius = LV_RADIUS_CIRCLE;
    circle_style.body.opa = LV_OPA_TRANSP;
    circle_style.body.border.width = style->line.width;
    circle_style.body.border.color = style->line.color;
    circle_style.body.border.opa = style->line.opa;

    lv_draw_mask_angle_param_t mask_angle_param;
    lv_draw_mask_angle_init(&mask_angle_param, center_x, center_y, start_angle, end_angle);

    int16_t mask_angle_id = lv_draw_mask_add(&mask_angle_param, NULL);

    lv_area_t area;
    area.x1 = center_x - radius;
    area.y1 = center_y - radius;
    area.x2 = center_x + radius - 1;  /*-1 because the center already belongs to the left/bottom part*/
    area.y2 = center_y + radius - 1;

    int32_t angle_gap;
    if(end_angle > start_angle) {
        angle_gap = 360 - (end_angle - start_angle);
    } else {
        angle_gap = end_angle - start_angle;
    }
    if(angle_gap > SPLIT_ANGLE_GAP_LIMIT && radius > SPLIT_RADIUS_LIMIT) {
        /*Handle each quarter individually and skip which is empty*/
        quarter_draw_dsc_t q_dsc;
        q_dsc.center_x = center_x;
        q_dsc.center_y = center_y;
        q_dsc.radius = radius;
        q_dsc.start_angle = start_angle;
        q_dsc.end_angle = end_angle;
        q_dsc.start_quarter= start_angle / 90;
        q_dsc.end_quarter = end_angle / 90;
        q_dsc.width = circle_style.body.border.width;
        q_dsc.opa_scale = opa_scale;
        q_dsc.style =  &circle_style;
        q_dsc.draw_area = &area;
        q_dsc.clip_area = clip_area;

        draw_quarter_0(&q_dsc);
        draw_quarter_1(&q_dsc);
        draw_quarter_2(&q_dsc);
        draw_quarter_3(&q_dsc);
    } else {
        lv_draw_rect(&area, clip_area, &circle_style, opa_scale);
    }
    lv_draw_mask_remove_id(mask_angle_id);

    if(style->line.rounded) {
        circle_style.body.main_color = style->line.color;
        circle_style.body.grad_color = style->line.color;
        circle_style.body.opa        = LV_OPA_COVER;
        circle_style.body.border.width = 0;

        lv_area_t round_area;
        get_rounded_area(start_angle, radius, style->line.width, &round_area);
        round_area.x1 += center_x;
        round_area.x2 += center_x;
        round_area.y1 += center_y;
        round_area.y2 += center_y;

        lv_draw_rect(&round_area, clip_area, &circle_style, opa_scale);

        get_rounded_area(end_angle, radius, style->line.width, &round_area);
        round_area.x1 += center_x;
        round_area.x2 += center_x;
        round_area.y1 += center_y;
        round_area.y2 += center_y;

        lv_draw_rect(&round_area, clip_area, &circle_style, opa_scale);
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
        quarter_area.y1 = q->center_y + ((lv_trigo_sin(q->start_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
        quarter_area.x2 = q->center_x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

        quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->end_angle) * q->radius) >> LV_TRIGO_SHIFT);
        quarter_area.x1 = q->center_x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
    }
    else if(q->start_quarter == 0 || q->end_quarter == 0) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 0) {
            quarter_area.x1 = q->center_x;
            quarter_area.y2 = q->center_y + q->radius;

            quarter_area.y1 = q->center_y + ((lv_trigo_sin(q->start_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.x2 = q->center_x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

            bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
        }
         if(q->end_quarter == 0) {
            quarter_area.x2 = q->center_x + q->radius;
            quarter_area.y1 = q->center_y;

            quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->end_angle) * q->radius) >> LV_TRIGO_SHIFT);
            quarter_area.x1 = q->center_x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

            bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
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

        bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
    }
}

static void draw_quarter_1(quarter_draw_dsc_t * q)
{
    lv_area_t quarter_area;

    if(q->start_quarter == 1 && q->end_quarter == 1 && q->start_angle < q->end_angle) {
        /*Small arc here*/
        quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);
        quarter_area.x2 = q->center_x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        quarter_area.y1 = q->center_y + ((lv_trigo_sin(q->end_angle) * q->radius - q->width) >> LV_TRIGO_SHIFT);
        quarter_area.x1 = q->center_x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

        bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
    }
    else if(q->start_quarter == 1 || q->end_quarter == 1) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 1) {
            quarter_area.x1 = q->center_x - q->radius;
            quarter_area.y1 = q->center_y;

            quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);
            quarter_area.x2 = q->center_x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

            bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
        }
         if(q->end_quarter == 1) {
            quarter_area.x2 = q->center_x - 1;
            quarter_area.y2 = q->center_y + q->radius;

            quarter_area.y1 = q->center_y + ((lv_trigo_sin(q->end_angle) * (q->radius- q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.x1 = q->center_x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius )) >> LV_TRIGO_SHIFT);

            bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
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

        bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
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

        bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
    }
    else if(q->start_quarter == 2 || q->end_quarter == 2) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 2) {
            quarter_area.x2 = q->center_x - 1;
            quarter_area.y1 = q->center_y - q->radius;

            quarter_area.x1 = q->center_x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius )) >> LV_TRIGO_SHIFT);
            quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->start_angle) * (q->radius- q->width)) >> LV_TRIGO_SHIFT);

            bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
        }
         if(q->end_quarter == 2) {
            quarter_area.x1 = q->center_x - q->radius;
            quarter_area.y2 = q->center_y - 1;

            quarter_area.x2 = q->center_x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.y1 = q->center_y + ((lv_trigo_sin(q->end_angle) * (q->radius)) >> LV_TRIGO_SHIFT);

            bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
        }
    }
    else if((q->start_quarter == q->end_quarter && q->start_quarter != 2 && q->end_angle < q->start_angle) ||
            (q->start_quarter == 0 && q->end_quarter == 3) ||
            (q->start_quarter == 1 && q->end_quarter == 3) ||
            (q->start_quarter == 3 && q->end_quarter == 1)) {
        /*Arc crosses here*/
        quarter_area.x1 = q->center_x - q->radius;
        quarter_area.y1 = q->center_y - q->radius;
        quarter_area.x2 = q->center_x - 1;
        quarter_area.y2 = q->center_y - 1;

        bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
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
        quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->end_angle) * q->radius - q->width) >> LV_TRIGO_SHIFT);

        bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
    }
    else if(q->start_quarter == 3 || q->end_quarter == 3) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 3) {
            quarter_area.x2 = q->center_x + q->radius;
            quarter_area.y2 = q->center_y - 1;

            quarter_area.x1 = q->center_x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.y1 = q->center_y + ((lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);

            bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
        }
         if(q->end_quarter == 3) {
            quarter_area.x1 = q->center_x;
            quarter_area.y1 = q->center_y - q->radius;

            quarter_area.x2 = q->center_x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);
            quarter_area.y2 = q->center_y + ((lv_trigo_sin(q->end_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

            bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
            if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
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

        bool ok = lv_area_intersect(&quarter_area, &quarter_area, q->clip_area);
        if(ok) lv_draw_rect(q->draw_area, &quarter_area, q->style, q->opa_scale);
    }
}




static void get_rounded_area(int16_t angle, lv_coord_t radius, uint8_t tickness, lv_area_t * res_area)
{
    const uint8_t ps = 8;
    const uint8_t pa = 127;

    int32_t thick_half = tickness / 2;
    uint8_t thick_corr = tickness & 0x01 ? 0 : 1;

    lv_coord_t rx_corr;
    lv_coord_t ry_corr;

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

