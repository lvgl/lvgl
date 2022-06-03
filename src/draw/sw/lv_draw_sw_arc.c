/**
 * @file lv_draw_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_log.h"
#include "../../misc/lv_mem.h"
#include "../lv_draw.h"

/*********************
 *      DEFINES
 *********************/
#define SPLIT_RADIUS_LIMIT 10  /*With radius greater than this the arc will drawn in quarters. A quarter is drawn only if there is arc in it*/
#define SPLIT_ANGLE_GAP_LIMIT 60  /*With small gaps in the arc don't bother with splitting because there is nothing to skip.*/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    const lv_point_t * center;
    lv_coord_t radius;
    uint16_t start_angle;
    uint16_t end_angle;
    uint16_t start_quarter;
    uint16_t end_quarter;
    lv_coord_t width;
    lv_draw_rect_dsc_t * draw_dsc;
    const lv_area_t * draw_area;
    lv_draw_ctx_t * draw_ctx;
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

void lv_draw_sw_arc(lv_draw_ctx_t * draw_ctx, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center, uint16_t radius,
                    uint16_t start_angle, uint16_t end_angle)
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
    }
    else {
        cir_dsc.bg_opa = dsc->opa;
        cir_dsc.bg_color = dsc->color;
    }

    lv_area_t area_out;
    area_out.x1 = center->x - radius;
    area_out.y1 = center->y - radius;
    area_out.x2 = center->x + radius - 1;  /*-1 because the center already belongs to the left/bottom part*/
    area_out.y2 = center->y + radius - 1;

    lv_area_t area_in;
    lv_area_copy(&area_in, &area_out);
    area_in.x1 += dsc->width;
    area_in.y1 += dsc->width;
    area_in.x2 -= dsc->width;
    area_in.y2 -= dsc->width;

    /*Create inner the mask*/
    int16_t mask_in_id = LV_MASK_ID_INV;
    lv_draw_mask_radius_param_t mask_in_param;
    bool mask_in_param_valid = false;
    if(lv_area_get_width(&area_in) > 0 && lv_area_get_height(&area_in) > 0) {
        lv_draw_mask_radius_init(&mask_in_param, &area_in, LV_RADIUS_CIRCLE, true);
        mask_in_param_valid = true;
        mask_in_id = lv_draw_mask_add(&mask_in_param, NULL);
    }

    lv_draw_mask_radius_param_t mask_out_param;
    lv_draw_mask_radius_init(&mask_out_param, &area_out, LV_RADIUS_CIRCLE, false);
    int16_t mask_out_id = lv_draw_mask_add(&mask_out_param, NULL);

    /*Draw a full ring*/
    if(start_angle + 360 == end_angle || start_angle == end_angle + 360) {
        cir_dsc.radius = LV_RADIUS_CIRCLE;
        lv_draw_rect(draw_ctx, &cir_dsc, &area_out);

        lv_draw_mask_remove_id(mask_out_id);
        if(mask_in_id != LV_MASK_ID_INV) lv_draw_mask_remove_id(mask_in_id);

        lv_draw_mask_free_param(&mask_out_param);
        if(mask_in_param_valid) {
            lv_draw_mask_free_param(&mask_in_param);
        }

        return;
    }

    while(start_angle >= 360) start_angle -= 360;
    while(end_angle >= 360) end_angle -= 360;

    lv_draw_mask_angle_param_t mask_angle_param;
    lv_draw_mask_angle_init(&mask_angle_param, center->x, center->y, start_angle, end_angle);
    int16_t mask_angle_id = lv_draw_mask_add(&mask_angle_param, NULL);

    int32_t angle_gap;
    if(end_angle > start_angle) {
        angle_gap = 360 - (end_angle - start_angle);
    }
    else {
        angle_gap = start_angle - end_angle;
    }

    const lv_area_t * clip_area_ori = draw_ctx->clip_area;

    if(angle_gap > SPLIT_ANGLE_GAP_LIMIT && radius > SPLIT_RADIUS_LIMIT) {
        /*Handle each quarter individually and skip which is empty*/
        quarter_draw_dsc_t q_dsc;
        q_dsc.center = center;
        q_dsc.radius = radius;
        q_dsc.start_angle = start_angle;
        q_dsc.end_angle = end_angle;
        q_dsc.start_quarter = (start_angle / 90) & 0x3;
        q_dsc.end_quarter = (end_angle / 90) & 0x3;
        q_dsc.width = width;
        q_dsc.draw_dsc = &cir_dsc;
        q_dsc.draw_area = &area_out;
        q_dsc.draw_ctx = draw_ctx;

        draw_quarter_0(&q_dsc);
        draw_quarter_1(&q_dsc);
        draw_quarter_2(&q_dsc);
        draw_quarter_3(&q_dsc);
    }
    else {
        lv_draw_rect(draw_ctx, &cir_dsc, &area_out);
    }

    lv_draw_mask_free_param(&mask_angle_param);
    lv_draw_mask_free_param(&mask_out_param);
    if(mask_in_param_valid) {
        lv_draw_mask_free_param(&mask_in_param);
    }

    lv_draw_mask_remove_id(mask_angle_id);
    lv_draw_mask_remove_id(mask_out_id);
    if(mask_in_id != LV_MASK_ID_INV) lv_draw_mask_remove_id(mask_in_id);

    if(dsc->rounded) {

        lv_draw_mask_radius_param_t mask_end_param;

        lv_area_t round_area;
        get_rounded_area(start_angle, radius, width, &round_area);
        round_area.x1 += center->x;
        round_area.x2 += center->x;
        round_area.y1 += center->y;
        round_area.y2 += center->y;
        lv_area_t clip_area2;
        if(_lv_area_intersect(&clip_area2, clip_area_ori, &round_area)) {
            lv_draw_mask_radius_init(&mask_end_param, &round_area, LV_RADIUS_CIRCLE, false);
            int16_t mask_end_id = lv_draw_mask_add(&mask_end_param, NULL);

            draw_ctx->clip_area = &clip_area2;
            lv_draw_rect(draw_ctx, &cir_dsc, &area_out);
            lv_draw_mask_remove_id(mask_end_id);
            lv_draw_mask_free_param(&mask_end_param);
        }

        get_rounded_area(end_angle, radius, width, &round_area);
        round_area.x1 += center->x;
        round_area.x2 += center->x;
        round_area.y1 += center->y;
        round_area.y2 += center->y;
        if(_lv_area_intersect(&clip_area2, clip_area_ori, &round_area)) {
            lv_draw_mask_radius_init(&mask_end_param, &round_area, LV_RADIUS_CIRCLE, false);
            int16_t mask_end_id = lv_draw_mask_add(&mask_end_param, NULL);

            draw_ctx->clip_area = &clip_area2;
            lv_draw_rect(draw_ctx, &cir_dsc, &area_out);
            lv_draw_mask_remove_id(mask_end_id);
            lv_draw_mask_free_param(&mask_end_param);
        }
        draw_ctx->clip_area = clip_area_ori;
    }
#else
    LV_LOG_WARN("Can't draw arc with LV_DRAW_COMPLEX == 0");
    LV_UNUSED(center);
    LV_UNUSED(radius);
    LV_UNUSED(start_angle);
    LV_UNUSED(end_angle);
    LV_UNUSED(draw_ctx);
    LV_UNUSED(dsc);
#endif /*LV_DRAW_COMPLEX*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_DRAW_COMPLEX
static void draw_quarter_0(quarter_draw_dsc_t * q)
{
    const lv_area_t * clip_area_ori = q->draw_ctx->clip_area;
    lv_area_t quarter_area;

    if(q->start_quarter == 0 && q->end_quarter == 0 && q->start_angle < q->end_angle) {
        /*Small arc here*/
        quarter_area.y1 = q->center->y + ((lv_trigo_sin(q->start_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
        quarter_area.x2 = q->center->x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

        quarter_area.y2 = q->center->y + ((lv_trigo_sin(q->end_angle) * q->radius) >> LV_TRIGO_SHIFT);
        quarter_area.x1 = q->center->x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
        if(ok) {
            q->draw_ctx->clip_area = &quarter_area;
            lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
        }
    }
    else if(q->start_quarter == 0 || q->end_quarter == 0) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 0) {
            quarter_area.x1 = q->center->x;
            quarter_area.y2 = q->center->y + q->radius;

            quarter_area.y1 = q->center->y + ((lv_trigo_sin(q->start_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.x2 = q->center->x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
            if(ok) {
                q->draw_ctx->clip_area = &quarter_area;
                lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
            }
        }
        if(q->end_quarter == 0) {
            quarter_area.x2 = q->center->x + q->radius;
            quarter_area.y1 = q->center->y;

            quarter_area.y2 = q->center->y + ((lv_trigo_sin(q->end_angle) * q->radius) >> LV_TRIGO_SHIFT);
            quarter_area.x1 = q->center->x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
            if(ok) {
                q->draw_ctx->clip_area = &quarter_area;
                lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
            }
        }
    }
    else if((q->start_quarter == q->end_quarter && q->start_quarter != 0 && q->end_angle < q->start_angle) ||
            (q->start_quarter == 2 && q->end_quarter == 1) ||
            (q->start_quarter == 3 && q->end_quarter == 2) ||
            (q->start_quarter == 3 && q->end_quarter == 1)) {
        /*Arc crosses here*/
        quarter_area.x1 = q->center->x;
        quarter_area.y1 = q->center->y;
        quarter_area.x2 = q->center->x + q->radius;
        quarter_area.y2 = q->center->y + q->radius;

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
        if(ok) {
            q->draw_ctx->clip_area = &quarter_area;
            lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
        }
    }
    q->draw_ctx->clip_area = clip_area_ori;
}

static void draw_quarter_1(quarter_draw_dsc_t * q)
{
    const lv_area_t * clip_area_ori = q->draw_ctx->clip_area;
    lv_area_t quarter_area;

    if(q->start_quarter == 1 && q->end_quarter == 1 && q->start_angle < q->end_angle) {
        /*Small arc here*/
        quarter_area.y2 = q->center->y + ((lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);
        quarter_area.x2 = q->center->x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        quarter_area.y1 = q->center->y + ((lv_trigo_sin(q->end_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
        quarter_area.x1 = q->center->x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
        if(ok) {
            q->draw_ctx->clip_area = &quarter_area;
            lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
        }
    }
    else if(q->start_quarter == 1 || q->end_quarter == 1) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 1) {
            quarter_area.x1 = q->center->x - q->radius;
            quarter_area.y1 = q->center->y;

            quarter_area.y2 = q->center->y + ((lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);
            quarter_area.x2 = q->center->x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
            if(ok) {
                q->draw_ctx->clip_area = &quarter_area;
                lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
            }
        }
        if(q->end_quarter == 1) {
            quarter_area.x2 = q->center->x - 1;
            quarter_area.y2 = q->center->y + q->radius;

            quarter_area.y1 = q->center->y + ((lv_trigo_sin(q->end_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.x1 = q->center->x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
            if(ok) {
                q->draw_ctx->clip_area = &quarter_area;
                lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
            }
        }
    }
    else if((q->start_quarter == q->end_quarter && q->start_quarter != 1 && q->end_angle < q->start_angle) ||
            (q->start_quarter == 0 && q->end_quarter == 2) ||
            (q->start_quarter == 0 && q->end_quarter == 3) ||
            (q->start_quarter == 3 && q->end_quarter == 2)) {
        /*Arc crosses here*/
        quarter_area.x1 = q->center->x - q->radius;
        quarter_area.y1 = q->center->y;
        quarter_area.x2 = q->center->x - 1;
        quarter_area.y2 = q->center->y + q->radius;

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
        if(ok) {
            q->draw_ctx->clip_area = &quarter_area;
            lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
        }
    }
    q->draw_ctx->clip_area = clip_area_ori;
}

static void draw_quarter_2(quarter_draw_dsc_t * q)
{
    const lv_area_t * clip_area_ori = q->draw_ctx->clip_area;
    lv_area_t quarter_area;

    if(q->start_quarter == 2 && q->end_quarter == 2 && q->start_angle < q->end_angle) {
        /*Small arc here*/
        quarter_area.x1 = q->center->x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);
        quarter_area.y2 = q->center->y + ((lv_trigo_sin(q->start_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        quarter_area.y1 = q->center->y + ((lv_trigo_sin(q->end_angle) * q->radius) >> LV_TRIGO_SHIFT);
        quarter_area.x2 = q->center->x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
        if(ok) {
            q->draw_ctx->clip_area = &quarter_area;
            lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
        }
    }
    else if(q->start_quarter == 2 || q->end_quarter == 2) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 2) {
            quarter_area.x2 = q->center->x - 1;
            quarter_area.y1 = q->center->y - q->radius;

            quarter_area.x1 = q->center->x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);
            quarter_area.y2 = q->center->y + ((lv_trigo_sin(q->start_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
            if(ok) {
                q->draw_ctx->clip_area = &quarter_area;
                lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
            }
        }
        if(q->end_quarter == 2) {
            quarter_area.x1 = q->center->x - q->radius;
            quarter_area.y2 = q->center->y - 1;

            quarter_area.x2 = q->center->x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.y1 = q->center->y + ((lv_trigo_sin(q->end_angle) * (q->radius)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
            if(ok) {
                q->draw_ctx->clip_area = &quarter_area;
                lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
            }
        }
    }
    else if((q->start_quarter == q->end_quarter && q->start_quarter != 2 && q->end_angle < q->start_angle) ||
            (q->start_quarter == 0 && q->end_quarter == 3) ||
            (q->start_quarter == 1 && q->end_quarter == 3) ||
            (q->start_quarter == 1 && q->end_quarter == 0)) {
        /*Arc crosses here*/
        quarter_area.x1 = q->center->x - q->radius;
        quarter_area.y1 = q->center->y - q->radius;
        quarter_area.x2 = q->center->x - 1;
        quarter_area.y2 = q->center->y - 1;

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
        if(ok) {
            q->draw_ctx->clip_area = &quarter_area;
            lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
        }
    }
    q->draw_ctx->clip_area = clip_area_ori;
}

static void draw_quarter_3(quarter_draw_dsc_t * q)
{
    const lv_area_t * clip_area_ori = q->draw_ctx->clip_area;
    lv_area_t quarter_area;

    if(q->start_quarter == 3 && q->end_quarter == 3 && q->start_angle < q->end_angle) {
        /*Small arc here*/
        quarter_area.x1 = q->center->x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
        quarter_area.y1 = q->center->y + ((lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);

        quarter_area.x2 = q->center->x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);
        quarter_area.y2 = q->center->y + ((lv_trigo_sin(q->end_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
        if(ok) {
            q->draw_ctx->clip_area = &quarter_area;
            lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
        }
    }
    else if(q->start_quarter == 3 || q->end_quarter == 3) {
        /*Start and/or end arcs here*/
        if(q->start_quarter == 3) {
            quarter_area.x2 = q->center->x + q->radius;
            quarter_area.y2 = q->center->y - 1;

            quarter_area.x1 = q->center->x + ((lv_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);
            quarter_area.y1 = q->center->y + ((lv_trigo_sin(q->start_angle) * (q->radius)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
            if(ok) {
                q->draw_ctx->clip_area = &quarter_area;
                lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
            }
        }
        if(q->end_quarter == 3) {
            quarter_area.x1 = q->center->x;
            quarter_area.y1 = q->center->y - q->radius;

            quarter_area.x2 = q->center->x + ((lv_trigo_sin(q->end_angle + 90) * (q->radius)) >> LV_TRIGO_SHIFT);
            quarter_area.y2 = q->center->y + ((lv_trigo_sin(q->end_angle) * (q->radius - q->width)) >> LV_TRIGO_SHIFT);

            bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
            if(ok) {
                q->draw_ctx->clip_area = &quarter_area;
                lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
            }
        }
    }
    else if((q->start_quarter == q->end_quarter && q->start_quarter != 3 && q->end_angle < q->start_angle) ||
            (q->start_quarter == 2 && q->end_quarter == 0) ||
            (q->start_quarter == 1 && q->end_quarter == 0) ||
            (q->start_quarter == 2 && q->end_quarter == 1)) {
        /*Arc crosses here*/
        quarter_area.x1 = q->center->x;
        quarter_area.y1 = q->center->y - q->radius;
        quarter_area.x2 = q->center->x + q->radius;
        quarter_area.y2 = q->center->y - 1;

        bool ok = _lv_area_intersect(&quarter_area, &quarter_area, clip_area_ori);
        if(ok) {
            q->draw_ctx->clip_area = &quarter_area;
            lv_draw_rect(q->draw_ctx, q->draw_dsc, q->draw_area);
        }
    }

    q->draw_ctx->clip_area = clip_area_ori;
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
