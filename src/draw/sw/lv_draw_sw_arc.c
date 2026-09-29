/**
 * @file lv_draw_sw_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_area_private.h"
#include "lv_draw_sw_mask_private.h"
#include "blend/lv_draw_sw_blend_private.h"
#include "../lv_image_decoder_private.h"
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW
#if LV_DRAW_SW_COMPLEX

#include "../../misc/lv_math.h"
#include "../../misc/lv_log.h"
#include "../../stdlib/lv_mem.h"
#include "../../stdlib/lv_string.h"
#include "../lv_draw_private.h"

static void add_circle(const lv_opa_t * circle_mask, const lv_area_t * blend_area, const lv_area_t * circle_area,
                       lv_opa_t * mask_buf);
static void get_rounded_area(int16_t angle, int32_t radius, int32_t thickness, lv_area_t * res_area,
                             lv_point_t * circle_center);
static void fill_circle_mask(lv_opa_t * circle_mask, const lv_area_t * circle_area, const lv_point_t * circle_center,
                             int32_t thickness);

/*********************
 *      DEFINES
 *********************/
#define SPLIT_RADIUS_LIMIT 10  /*With radius greater than this the arc will drawn in quarters. A quarter is drawn only if there is arc in it*/
#define SPLIT_ANGLE_GAP_LIMIT 60  /*With small gaps in the arc don't bother with splitting because there is nothing to skip.*/
#define CIRCLE_SHIFT 4  /*The round ends are placed in 1/CIRCLE_UNIT px*/
#define CIRCLE_UNIT (1 << CIRCLE_SHIFT)

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

void lv_draw_sw_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{
#if LV_DRAW_SW_COMPLEX
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(dsc->start_angle == dsc->end_angle) return;

    int32_t width = dsc->width;
    if(width > dsc->radius) width = dsc->radius;

    lv_area_t area_out = *coords;
    lv_area_t clipped_area;
    if(!lv_area_intersect(&clipped_area, &area_out, &t->clip_area)) return;

    /*Draw a full ring*/
    if(dsc->img_src == NULL &&
       (dsc->start_angle + 360 == dsc->end_angle || dsc->start_angle == dsc->end_angle + 360)) {
        lv_draw_border_dsc_t cir_dsc;
        lv_draw_border_dsc_init(&cir_dsc);
        cir_dsc.opa = dsc->opa;
        cir_dsc.color = dsc->color;
        cir_dsc.width = width;
        cir_dsc.radius = LV_RADIUS_CIRCLE;
        cir_dsc.side = LV_BORDER_SIDE_FULL;
        lv_draw_sw_border(t, &cir_dsc, &area_out);
        return;
    }

    lv_area_t area_in;
    lv_area_copy(&area_in, &area_out);
    area_in.x1 += dsc->width;
    area_in.y1 += dsc->width;
    area_in.x2 -= dsc->width;
    area_in.y2 -= dsc->width;

    int32_t start_angle = (int32_t)dsc->start_angle;
    int32_t end_angle = (int32_t)dsc->end_angle;
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

    int32_t blend_h = lv_area_get_height(&clipped_area);
    int32_t blend_w = lv_area_get_width(&clipped_area);
    int32_t h;
    lv_opa_t * mask_buf = lv_malloc(blend_w);

    lv_area_t blend_area = clipped_area;
    lv_area_t img_area;
    lv_draw_sw_blend_dsc_t blend_dsc = {0};
    blend_dsc.mask_buf = mask_buf;
    blend_dsc.opa = dsc->opa;
    blend_dsc.blend_area = &blend_area;
    blend_dsc.mask_area = &blend_area;

    const uint8_t * img_mask = NULL;
    lv_image_decoder_dsc_t decoder_dsc;
    if(dsc->img_src == NULL) {
        blend_dsc.color = dsc->color;
    }
    else {
        lv_result_t res = lv_image_decoder_open(&decoder_dsc, dsc->img_src, NULL);
        if(res == LV_RESULT_INVALID || decoder_dsc.decoded == NULL) {
            LV_LOG_WARN("Can't decode the background image");
            blend_dsc.color = dsc->color;
        }
        else {
            img_area.x1 = 0;
            img_area.y1 = 0;
            img_area.x2 = decoder_dsc.decoded->header.w - 1;
            img_area.y2 = decoder_dsc.decoded->header.h - 1;
            int32_t ofs = decoder_dsc.decoded->header.w / 2;
            lv_area_move(&img_area, dsc->center.x - ofs, dsc->center.y - ofs);
            blend_dsc.src_area = &img_area;
            blend_dsc.src_buf = decoder_dsc.decoded->data;
            blend_dsc.src_stride = decoder_dsc.decoded->header.stride;
            blend_dsc.src_color_format = decoder_dsc.decoded->header.cf;
            if(blend_dsc.src_color_format == LV_COLOR_FORMAT_RGB565A8) {
                blend_dsc.src_color_format = LV_COLOR_FORMAT_RGB565;
                img_mask = (uint8_t *)blend_dsc.src_buf + blend_dsc.src_stride * lv_area_get_height(blend_dsc.src_area);
            }
        }
    }

    lv_opa_t * circle_mask_1 = NULL;
    lv_opa_t * circle_mask_2 = NULL;
    lv_area_t round_area_1;
    lv_area_t round_area_2;
    if(dsc->rounded) {
        /*The two ends are placed with sub-pixel precision, each with its own offset,
         *so their anti-aliased edges differ and each end needs its own mask.
         *A circle not aligned to the pixel grid spans one more column and row: (width + 1) x (width + 1).*/
        uint64_t circle_mask_size = (uint64_t)(width + 1) * (width + 1);
        /*On 32-bit targets a huge (unrealistic but valid) size would be truncated by the cast to `size_t`*/
        if(circle_mask_size * 2 <= SIZE_MAX) circle_mask_1 = lv_malloc((size_t)(circle_mask_size * 2));

        if(circle_mask_1 == NULL) {
            LV_LOG_WARN("Couldn't allocate the masks of the round ends, the arc is drawn without them");
        }
        else {
            circle_mask_2 = circle_mask_1 + circle_mask_size;
            lv_point_t circle_center;

            /*Only the part of a mask inside the clip area is read by `add_circle`,
             *so there is no need to fill the mask of an end that lies completely outside it.*/
            get_rounded_area(start_angle, dsc->radius, width, &round_area_1, &circle_center);
            lv_area_move(&round_area_1, dsc->center.x, dsc->center.y);
            if(lv_area_is_on(&round_area_1, &clipped_area)) {
                fill_circle_mask(circle_mask_1, &round_area_1, &circle_center, width);
            }

            get_rounded_area(end_angle, dsc->radius, width, &round_area_2, &circle_center);
            lv_area_move(&round_area_2, dsc->center.x, dsc->center.y);
            if(lv_area_is_on(&round_area_2, &clipped_area)) {
                fill_circle_mask(circle_mask_2, &round_area_2, &circle_center, width);
            }
        }
    }

    blend_area.y2 = blend_area.y1;
    for(h = 0; h < blend_h; h++) {
        lv_memset(mask_buf, 0xff, blend_w);
        blend_dsc.mask_res = lv_draw_sw_mask_apply(mask_list, mask_buf, blend_area.x1, blend_area.y1, blend_w);

        if(circle_mask_1) {
            if(blend_area.y1 >= round_area_1.y1 && blend_area.y1 <= round_area_1.y2) {
                if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_TRANSP) {
                    lv_memzero(mask_buf, blend_w);
                    blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                }
                add_circle(circle_mask_1, &blend_area, &round_area_1, mask_buf);
            }
            if(blend_area.y1 >= round_area_2.y1 && blend_area.y1 <= round_area_2.y2) {
                if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_TRANSP) {
                    lv_memzero(mask_buf, blend_w);
                    blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                }
                add_circle(circle_mask_2, &blend_area, &round_area_2, mask_buf);
            }
        }

        /*If it was an RGB565A8 image use consider its A8 part on the mask*/
        if(img_mask && blend_dsc.mask_res != LV_DRAW_SW_MASK_RES_TRANSP) {
            const uint8_t * img_mask_tmp = img_mask;
            img_mask_tmp += blend_dsc.src_stride / 2 * (blend_area.y1 - blend_dsc.src_area->y1);
            img_mask_tmp += blend_area.x1 - blend_dsc.src_area->x1;

            int32_t i;
            for(i = 0; i < blend_w; i++) {
                mask_buf[i] = LV_OPA_MIX2(mask_buf[i], img_mask_tmp[i]);
            }
            if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) {
                blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
            }
        }

        lv_draw_sw_blend(t, &blend_dsc);

        blend_area.y1 ++;
        blend_area.y2 ++;
    }

    lv_draw_sw_mask_free_param(&mask_angle_param);
    lv_draw_sw_mask_free_param(&mask_out_param);
    if(mask_in_param_valid) {
        lv_draw_sw_mask_free_param(&mask_in_param);
    }

    lv_free(mask_buf);
    if(dsc->img_src) lv_image_decoder_close(&decoder_dsc);
    if(circle_mask_1) lv_free(circle_mask_1); /*Frees both masks: they share one allocation*/
#else
    LV_LOG_WARN("Can't draw arc with LV_DRAW_SW_COMPLEX == 0");
    LV_UNUSED(center);
    LV_UNUSED(radius);
    LV_UNUSED(start_angle);
    LV_UNUSED(end_angle);
    LV_UNUSED(layer);
    LV_UNUSED(dsc);
#endif /*LV_DRAW_SW_COMPLEX*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void add_circle(const lv_opa_t * circle_mask, const lv_area_t * blend_area, const lv_area_t * circle_area,
                       lv_opa_t * mask_buf)
{
    lv_area_t circle_common_area;
    if(lv_area_intersect(&circle_common_area, circle_area, blend_area)) {
        int32_t width = lv_area_get_width(circle_area);
        const lv_opa_t * circle_mask_tmp = circle_mask + width * (circle_common_area.y1 - circle_area->y1);
        circle_mask_tmp += circle_common_area.x1 - circle_area->x1;

        lv_opa_t * mask_buf_tmp = mask_buf + circle_common_area.x1 - blend_area->x1;

        uint32_t x;
        uint32_t w = lv_area_get_width(&circle_common_area);
        for(x = 0; x < w; x++) {
            uint32_t res = mask_buf_tmp[x] + circle_mask_tmp[x];
            mask_buf_tmp[x] = res > 255 ? 255 : res;
        }
    }

}

/*The center of the round end is kept with 1/CIRCLE_UNIT px precision:
 *snapping it to the pixel grid moves the end in and out of the anti-aliased edges of the arc as the angle changes.
 *As for the angle and radius masks, the center of the arc is the top-left corner of its center pixel.
 *`res_area` is in pixels, relative to the center of the arc;
 *`circle_center` is in 1/CIRCLE_UNIT px, relative to the top-left corner of `res_area`.*/
static void get_rounded_area(int16_t angle, int32_t radius, int32_t thickness, lv_area_t * res_area,
                             lv_point_t * circle_center)
{
    /*The ends are centered on the middle line of the arc, at `radius - thickness / 2`.
     *`middle_radius` is in half pixels so that an odd thickness does not lose the 0.5 px.
     *`shift` removes the trigonometric scale (LV_TRIGO_SHIFT) and the half pixels (+1),
     *keeping CIRCLE_SHIFT fractional bits, so the result is in 1/CIRCLE_UNIT px.*/
    int32_t shift = LV_TRIGO_SHIFT + 1 - CIRCLE_SHIFT;
    int32_t round_half = 1 << (shift - 1); /*Round to nearest, the same way for positive and negative coordinates*/
    int32_t middle_radius = 2 * radius - thickness;

    /*circle_x, circle_y and circle_radius are in 1/CIRCLE_UNIT px, relative to the center of the arc*/
    /*int64_t: with a huge radius the products overflow int32_t*/
    int32_t circle_x = (int32_t)(((int64_t)middle_radius * lv_trigo_cos(angle) + round_half) >> shift);
    int32_t circle_y = (int32_t)(((int64_t)middle_radius * lv_trigo_sin(angle) + round_half) >> shift);
    int32_t circle_radius = thickness * CIRCLE_UNIT / 2;

    /*The pixels the circle can cover are those overlapped by its extent
     *[center - circle_radius, center + circle_radius): any other pixel has its center
     *at least `circle_radius + 0.5 px` away, where the coverage is zero.
     *The extent is `thickness` px long, so it overlaps `thickness` pixels when aligned to the grid
     *and `thickness + 1` pixels when not.*/
    res_area->x1 = (circle_x - circle_radius) >> CIRCLE_SHIFT;
    res_area->y1 = (circle_y - circle_radius) >> CIRCLE_SHIFT;
    res_area->x2 = (circle_x + circle_radius - 1) >> CIRCLE_SHIFT;
    res_area->y2 = (circle_y + circle_radius - 1) >> CIRCLE_SHIFT;

    /*Relative to `res_area`, so that the area can be moved with `lv_area_move` without touching the center*/
    circle_center->x = circle_x - res_area->x1 * CIRCLE_UNIT;
    circle_center->y = circle_y - res_area->y1 * CIRCLE_UNIT;
}

/*Pixels whose squared distance from the center is above `outer_sqr` are empty, below `inner_sqr` are full.
 *In between, about 0.5 px on each side of the edge, the coverage fades linearly in the squared distance,
 *which avoids a square root.
 *The limits are centered on the edge so that a pixel on it is half covered:
 *using (circle_radius +- 0.5 px)^2 instead would make the circle look bigger.*/
static void fill_circle_mask(lv_opa_t * circle_mask, const lv_area_t * circle_area, const lv_point_t * circle_center,
                             int32_t thickness)
{
    /*circle_radius, dx and dy are in 1/CIRCLE_UNIT px.
     *The squares are int64_t as huge thicknesses overflow int32_t. The division stays 32-bit (faster):
     *it holds because `lv_draw_sw_arc` limits the thickness to the radius, a `uint16_t`.*/
    LV_ASSERT(thickness <= UINT16_MAX);

    int32_t circle_radius = thickness * CIRCLE_UNIT / 2;
    int64_t circle_sqr = (int64_t)circle_radius * circle_radius;
    int64_t outer_sqr = circle_sqr + (int64_t)circle_radius * CIRCLE_UNIT;
    int64_t inner_sqr = LV_MAX(circle_sqr - (int64_t)circle_radius * CIRCLE_UNIT, (int64_t)0);
    uint32_t edge_sqr = (uint32_t)(outer_sqr - inner_sqr);
    int32_t mask_width = lv_area_get_width(circle_area);
    int32_t mask_height = lv_area_get_height(circle_area);

    /*Pixel (x, y) of the mask covers [x, x + 1) x [y, y + 1) px: the distance to the circle's center
     *is measured from the pixel's center, hence the `+ CIRCLE_UNIT / 2` (0.5 px)*/
    for(int32_t y = 0; y < mask_height; y++) {
        int32_t dy = y * CIRCLE_UNIT + CIRCLE_UNIT / 2 - circle_center->y;
        int64_t dy_sqr = (int64_t)dy * dy;

        for(int32_t x = 0; x < mask_width; x++) {
            int32_t dx = x * CIRCLE_UNIT + CIRCLE_UNIT / 2 - circle_center->x;
            int64_t dist_sqr = (int64_t)dx * dx + dy_sqr;

            if(dist_sqr >= outer_sqr) *circle_mask = LV_OPA_TRANSP;
            else if(dist_sqr <= inner_sqr) *circle_mask = LV_OPA_COVER;
            else *circle_mask = (lv_opa_t)(((uint32_t)(outer_sqr - dist_sqr) * LV_OPA_COVER) / edge_sqr);

            circle_mask++;
        }
    }
}

#else /*LV_DRAW_SW_COMPLEX*/

void lv_draw_sw_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(t);
    LV_UNUSED(dsc);
    LV_UNUSED(coords);

    LV_LOG_WARN("LV_DRAW_SW_COMPLEX needs to be enabled");
}

#endif /*LV_DRAW_SW_COMPLEX*/
#endif /*LV_USE_DRAW_SW*/
