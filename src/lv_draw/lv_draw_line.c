/**
 * @file lv_draw_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <stdbool.h>
#include "lv_draw.h"
#include "../lv_core/lv_refr.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
    lv_point_t p1;
    lv_point_t p2;
    lv_point_t p_act;
    lv_coord_t dx;
    lv_coord_t sx; /*-1: x1 < x2; 1: x2 >= x1*/
    lv_coord_t dy;
    lv_coord_t sy; /*-1: y1 < y2; 1: y2 >= y1*/
    lv_coord_t err;
    lv_coord_t e2;
    bool hor; /*Rather horizontal or vertical*/
} line_draw_t;

typedef struct
{
    lv_coord_t width;
    lv_coord_t width_1;
    lv_coord_t width_half;
} line_width_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void line_draw_hor(line_draw_t * main_line, const lv_area_t * mask, const lv_style_t * style,
                          lv_opa_t opa_scale);
static void line_draw_ver(line_draw_t * main_line, const lv_area_t * mask, const lv_style_t * style,
                          lv_opa_t opa_scale);
static void line_draw_skew(line_draw_t * main_line, bool dir_ori, const lv_area_t * mask, const lv_style_t * style,
                           lv_opa_t opa_scale);
static void line_init(line_draw_t * line, const lv_point_t * p1, const lv_point_t * p2);
static bool line_next(line_draw_t * line);
static bool line_next_y(line_draw_t * line);
static bool line_next_x(line_draw_t * line);

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
 * Draw a line
 * @param point1 first point of the line
 * @param point2 second point of the line
 * @param mask the line will be drawn only on this area
 * @param style pointer to a line's style
 * @param opa_scale scale down all opacities by the factor
 */
void lv_draw_line(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * mask,
                  const lv_style_t * style, lv_opa_t opa_scale)
{

    if(style->line.width == 0) return;
    if(point1->x == point2->x && point1->y == point2->y) return;

    /*Return if the points are out of the mask*/
    if(point1->x < mask->x1 - style->line.width && point2->x < mask->x1 - style->line.width) return;
    if(point1->x > mask->x2 + style->line.width && point2->x > mask->x2 + style->line.width) return;
    if(point1->y < mask->y1 - style->line.width && point2->y < mask->y1 - style->line.width) return;
    if(point1->y > mask->y2 + style->line.width && point2->y > mask->y2 + style->line.width) return;

    line_draw_t main_line;
    lv_point_t p1;
    lv_point_t p2;

    /*If the line if rather vertical then be sure y1 < y2 else x1 < x2*/

    if(LV_MATH_ABS(point1->x - point2->x) > LV_MATH_ABS(point1->y - point2->y)) {

        /*Steps less in y then x -> rather horizontal*/
        if(point1->x < point2->x) {
            p1.x = point1->x;
            p1.y = point1->y;
            p2.x = point2->x;
            p2.y = point2->y;
        } else {
            p1.x = point2->x;
            p1.y = point2->y;
            p2.x = point1->x;
            p2.y = point1->y;
        }
    } else {
        /*Steps less in x then y -> rather vertical*/
        if(point1->y < point2->y) {
            p1.x = point1->x;
            p1.y = point1->y;
            p2.x = point2->x;
            p2.y = point2->y;
        } else {
            p1.x = point2->x;
            p1.y = point2->y;
            p2.x = point1->x;
            p2.y = point1->y;
        }
    }

    line_init(&main_line, &p1, &p2);

    /*Special case draw a horizontal line*/
    if(main_line.p1.y == main_line.p2.y) {
        line_draw_hor(&main_line, mask, style, opa_scale);
    }
    /*Special case draw a vertical line*/
    else if(main_line.p1.x == main_line.p2.x) {
        line_draw_ver(&main_line, mask, style, opa_scale);
    }
    /*Arbitrary skew line*/
    else {
        bool dir_ori = false;
#if LV_ANTIALIAS
        bool aa = lv_disp_get_antialiasing(lv_refr_get_disp_refreshing());
        if(aa) {
            lv_point_t p_tmp;

            if(main_line.hor) {
                if(main_line.p1.y < main_line.p2.y) {
                    dir_ori = true;
                    p_tmp.x = main_line.p2.x;
                    p_tmp.y = main_line.p2.y - 1;
                    line_init(&main_line, &p1, &p_tmp);
                    main_line.sy = LV_MATH_ABS(main_line.sy); /*The sign can change if the line becomes horizontal*/
                } else if(main_line.p1.y > main_line.p2.y) {
                    dir_ori = false;
                    p_tmp.x = main_line.p2.x;
                    p_tmp.y = main_line.p2.y + 1;
                    line_init(&main_line, &p1, &p_tmp);
                    main_line.sy = -LV_MATH_ABS(main_line.sy); /*The sign can change if the line becomes horizontal*/
                }
            } else {
                if(main_line.p1.x < main_line.p2.x) {
                    dir_ori = true;
                    p_tmp.x = main_line.p2.x - 1;
                    p_tmp.y = main_line.p2.y;
                    line_init(&main_line, &p1, &p_tmp);
                    main_line.sx = LV_MATH_ABS(main_line.sx); /*The sign can change if the line becomes vertical*/
                } else if(main_line.p1.x > main_line.p2.x) {
                    dir_ori = false;
                    p_tmp.x = main_line.p2.x + 1;
                    p_tmp.y = main_line.p2.y;
                    line_init(&main_line, &p1, &p_tmp);
                    main_line.sx = -LV_MATH_ABS(main_line.sx); /*The sign can change if the line becomes vertical*/
                }
            }
        }
#endif
        line_draw_skew(&main_line, dir_ori, mask, style, opa_scale);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void line_draw_hor(line_draw_t * main_line, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_coord_t width      = style->line.width - 1;
    lv_coord_t width_half = width >> 1;
    lv_coord_t width_1    = width & 0x1;
    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->line.opa : (uint16_t)((uint16_t)style->line.opa * opa_scale) >> 8;

    lv_area_t act_area;
    act_area.x1 = main_line->p1.x;
    act_area.x2 = main_line->p2.x;
    act_area.y1 = main_line->p1.y - width_half - width_1;
    act_area.y2 = main_line->p2.y + width_half;

    lv_area_t draw_area;
    draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
    draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
    draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
    draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
    lv_draw_fill(&draw_area, mask, style->line.color, opa);
}

static void line_draw_ver(line_draw_t * main_line, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_coord_t width      = style->line.width - 1;
    lv_coord_t width_half = width >> 1;
    lv_coord_t width_1    = width & 0x1;
    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->line.opa : (uint16_t)((uint16_t)style->line.opa * opa_scale) >> 8;

    lv_area_t act_area;
    act_area.x1 = main_line->p1.x - width_half;
    act_area.x2 = main_line->p2.x + width_half + width_1;
    act_area.y1 = main_line->p1.y;
    act_area.y2 = main_line->p2.y;

    lv_area_t draw_area;
    draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
    draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
    draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
    draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
    lv_draw_fill(&draw_area, mask, style->line.color, opa);
}

static void line_draw_skew(line_draw_t * main_line, bool dir_ori, const lv_area_t * mask, const lv_style_t * style,
                           lv_opa_t opa_scale)
{

    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->line.opa : (uint16_t)((uint16_t)style->line.opa * opa_scale) >> 8;
#if LV_ANTIALIAS
    bool aa = lv_disp_get_antialiasing(lv_refr_get_disp_refreshing());
#endif
    lv_point_t vect_main, vect_norm;
    vect_main.x = main_line->p2.x - main_line->p1.x;
    vect_main.y = main_line->p2.y - main_line->p1.y;

    if(main_line->hor) {
        if(main_line->p1.y < main_line->p2.y + dir_ori) {
            vect_norm.x = -vect_main.y;
            vect_norm.y = vect_main.x;
        } else {
            vect_norm.x = vect_main.y;
            vect_norm.y = -vect_main.x;
        }
    } else {
        if(main_line->p1.x < main_line->p2.x + dir_ori) {
            vect_norm.x = vect_main.y;
            vect_norm.y = -vect_main.x;
        } else {
            vect_norm.x = -vect_main.y;
            vect_norm.y = vect_main.x;
        }
    }

    /* In case of a short but tick line the perpendicular ending is longer then the real line.
     * it would break the calculations so make the normal vector larger*/
    vect_norm.x = vect_norm.x << 4;
    vect_norm.y = vect_norm.y << 4;

    lv_coord_t width;
    width = style->line.width;

    /* The pattern stores the points of the line ending. It has the good direction and length.
     * The worth case is the 45° line where pattern can have 1.41 x `width` points*/

    lv_point_t * pattern = lv_draw_get_buf(width * 2 * sizeof(lv_point_t));
    lv_coord_t i = 0;

    /*Create a perpendicular pattern (a small line)*/
    if(width != 0) {
        line_draw_t pattern_line;
        lv_point_t p0 = {0, 0};
        line_init(&pattern_line, &p0, &vect_norm);

        uint32_t width_sqr = width * width;
        /* Run for a lot of times. Meanwhile the real width will be determined as well */
        for(i = 0; i < (lv_coord_t)sizeof(pattern); i++) {
            pattern[i].x = pattern_line.p_act.x;
            pattern[i].y = pattern_line.p_act.y;

            /*Finish the pattern line if it's length equal to the desired width (Use Pythagoras
             * theorem)*/
            uint32_t sqr = pattern_line.p_act.x * pattern_line.p_act.x + pattern_line.p_act.y * pattern_line.p_act.y;
            if(sqr >= width_sqr) {
                width = i;
#if LV_ANTIALIAS
                if(aa) width--;
#endif
                break;
            }

            line_next(&pattern_line);
        }
    }

#if LV_ANTIALIAS
    lv_coord_t aa_last_corner;
    lv_coord_t width_safe = width;
    if(aa) {
        if(width == 0) width_safe = 1;

        aa_last_corner = 0;
    }
#endif

    lv_coord_t x_center_ofs = 0;
    lv_coord_t y_center_ofs = 0;

    if(width != 0) {
        x_center_ofs = pattern[width - 1].x / 2;
        y_center_ofs = pattern[width - 1].y / 2;
    } else {
        if(main_line->hor && main_line->p1.y >= main_line->p2.y + dir_ori) pattern[0].y--;
        if(!main_line->hor && main_line->p1.x >= main_line->p2.x + dir_ori) pattern[0].x--;
    }

    /* Make the coordinates relative to the center */
    for(i = 0; i < width; i++) {
        pattern[i].x -= x_center_ofs;
        pattern[i].y -= y_center_ofs;
#if LV_ANTIALIAS
        if(aa) {
            if(i != 0) {
                if(main_line->hor) {
                    if(pattern[i - 1].x != pattern[i].x) {
                        lv_coord_t seg_w = pattern[i].y - pattern[aa_last_corner].y;
                        if(main_line->sy < 0) {
                            lv_draw_aa_ver_seg(main_line->p1.x + pattern[aa_last_corner].x - 1,
                                               main_line->p1.y + pattern[aa_last_corner].y + seg_w + 1, seg_w, mask,
                                               style->line.color, opa);

                            lv_draw_aa_ver_seg(main_line->p2.x + pattern[aa_last_corner].x + 1,
                                               main_line->p2.y + pattern[aa_last_corner].y + seg_w + 1, -seg_w, mask,
                                               style->line.color, opa);
                        } else {
                            lv_draw_aa_ver_seg(main_line->p1.x + pattern[aa_last_corner].x - 1,
                                               main_line->p1.y + pattern[aa_last_corner].y, seg_w, mask,
                                               style->line.color, opa);

                            lv_draw_aa_ver_seg(main_line->p2.x + pattern[aa_last_corner].x + 1,
                                               main_line->p2.y + pattern[aa_last_corner].y, -seg_w, mask,
                                               style->line.color, opa);
                        }
                        aa_last_corner = i;
                    }
                } else {
                    if(pattern[i - 1].y != pattern[i].y) {
                        lv_coord_t seg_w = pattern[i].x - pattern[aa_last_corner].x;
                        if(main_line->sx < 0) {
                            lv_draw_aa_hor_seg(main_line->p1.x + pattern[aa_last_corner].x + seg_w + 1,
                                               main_line->p1.y + pattern[aa_last_corner].y - 1, seg_w, mask,
                                               style->line.color, opa);

                            lv_draw_aa_hor_seg(main_line->p2.x + pattern[aa_last_corner].x + seg_w + 1,
                                               main_line->p2.y + pattern[aa_last_corner].y + 1, -seg_w, mask,
                                               style->line.color, opa);
                        } else {
                            lv_draw_aa_hor_seg(main_line->p1.x + pattern[aa_last_corner].x,
                                               main_line->p1.y + pattern[aa_last_corner].y - 1, seg_w, mask,
                                               style->line.color, opa);

                            lv_draw_aa_hor_seg(main_line->p2.x + pattern[aa_last_corner].x,
                                               main_line->p2.y + pattern[aa_last_corner].y + 1, -seg_w, mask,
                                               style->line.color, opa);
                        }
                        aa_last_corner = i;
                    }
                }
            }
        }
#endif
    }

#if LV_ANTIALIAS
    /*Add the last part of anti-aliasing for the perpendicular ending*/
    if(width != 0 && aa) { /*Due to rounding error with very thin lines it looks ugly*/
        if(main_line->hor) {
            lv_coord_t seg_w = pattern[width_safe - 1].y - pattern[aa_last_corner].y;
            if(main_line->sy < 0) {
                lv_draw_aa_ver_seg(main_line->p1.x + pattern[aa_last_corner].x - 1,
                                   main_line->p1.y + pattern[aa_last_corner].y + seg_w, seg_w + main_line->sy, mask,
                                   style->line.color, opa);

                lv_draw_aa_ver_seg(main_line->p2.x + pattern[aa_last_corner].x + 1,
                                   main_line->p2.y + pattern[aa_last_corner].y + seg_w, -(seg_w + main_line->sy), mask,
                                   style->line.color, opa);

            } else {
                lv_draw_aa_ver_seg(main_line->p1.x + pattern[aa_last_corner].x - 1,
                                   main_line->p1.y + pattern[aa_last_corner].y, seg_w + main_line->sy, mask,
                                   style->line.color, opa);

                lv_draw_aa_ver_seg(main_line->p2.x + pattern[aa_last_corner].x + 1,
                                   main_line->p2.y + pattern[aa_last_corner].y, -(seg_w + main_line->sy), mask,
                                   style->line.color, opa);
            }
        } else {
            lv_coord_t seg_w = pattern[width_safe - 1].x - pattern[aa_last_corner].x;
            if(main_line->sx < 0) {
                lv_draw_aa_hor_seg(main_line->p1.x + pattern[aa_last_corner].x + seg_w,
                                   main_line->p1.y + pattern[aa_last_corner].y - 1, seg_w + main_line->sx, mask,
                                   style->line.color, opa);

                lv_draw_aa_hor_seg(main_line->p2.x + pattern[aa_last_corner].x + seg_w,
                                   main_line->p2.y + pattern[aa_last_corner].y + 1, -(seg_w + main_line->sx), mask,
                                   style->line.color, opa);

            } else {
                lv_draw_aa_hor_seg(main_line->p1.x + pattern[aa_last_corner].x,
                                   main_line->p1.y + pattern[aa_last_corner].y - 1, seg_w + main_line->sx, mask,
                                   style->line.color, opa);

                lv_draw_aa_hor_seg(main_line->p2.x + pattern[aa_last_corner].x,
                                   main_line->p2.y + pattern[aa_last_corner].y + 1, -(seg_w + main_line->sx), mask,
                                   style->line.color, opa);
            }
        }
    }
#endif

#if LV_ANTIALIAS

    /*Shift the anti aliasing on the edges (-1, 1 or 0 (zero only in case width == 0))*/
    lv_coord_t aa_shift1 = 0;
    lv_coord_t aa_shift2 = 0;
    if(aa) {
        if(main_line->hor == false) {
            if(main_line->sx < 0) {
                aa_shift1 = -1;
                aa_shift2 = width == 0 ? 0 : aa_shift1;
            } else {
                aa_shift2 = 1;
                aa_shift1 = width == 0 ? 0 : aa_shift2;
            }
        } else {
            if(main_line->sy < 0) {
                aa_shift1 = -1;
                aa_shift2 = width == 0 ? 0 : aa_shift1;
            } else {
                aa_shift2 = 1;
                aa_shift1 = width == 0 ? 0 : aa_shift2;
            }
        }
    }
#endif

    volatile lv_point_t prev_p;
    prev_p.x = main_line->p1.x;
    prev_p.y = main_line->p1.y;
    lv_area_t draw_area;
    bool first_run = true;

    if(main_line->hor) {
        while(line_next_y(main_line)) {
            for(i = 0; i < width; i++) {
                draw_area.x1 = prev_p.x + pattern[i].x;
                draw_area.y1 = prev_p.y + pattern[i].y;
                draw_area.x2 = draw_area.x1 + main_line->p_act.x - prev_p.x - 1;
                draw_area.y2 = draw_area.y1;
                lv_draw_fill(&draw_area, mask, style->line.color, opa);

                /* Fill the gaps
                 * When stepping in y one pixel remains empty on every corner (don't do this on the
                 * first segment ) */
                if(i != 0 && pattern[i].x != pattern[i - 1].x && !first_run) {
                    lv_draw_px(draw_area.x1, draw_area.y1 - main_line->sy, mask, style->line.color, opa);
                }
            }

#if LV_ANTIALIAS
            if(aa) {
                lv_draw_aa_hor_seg(prev_p.x + pattern[0].x, prev_p.y + pattern[0].y - aa_shift1,
                                   -(main_line->p_act.x - prev_p.x), mask, style->line.color, opa);
                lv_draw_aa_hor_seg(prev_p.x + pattern[width_safe - 1].x,
                                   prev_p.y + pattern[width_safe - 1].y + aa_shift2, main_line->p_act.x - prev_p.x,
                                   mask, style->line.color, opa);
            }
#endif

            first_run = false;

            prev_p.x = main_line->p_act.x;
            prev_p.y = main_line->p_act.y;
        }

        for(i = 0; i < width; i++) {
            draw_area.x1 = prev_p.x + pattern[i].x;
            draw_area.y1 = prev_p.y + pattern[i].y;
            draw_area.x2 = draw_area.x1 + main_line->p_act.x - prev_p.x;
            draw_area.y2 = draw_area.y1;
            lv_draw_fill(&draw_area, mask, style->line.color, opa);

            /* Fill the gaps
             * When stepping in y one pixel remains empty on every corner */
            if(i != 0 && pattern[i].x != pattern[i - 1].x && !first_run) {
                lv_draw_px(draw_area.x1, draw_area.y1 - main_line->sy, mask, style->line.color, opa);
            }
        }

#if LV_ANTIALIAS
        if(aa) {
            lv_draw_aa_hor_seg(prev_p.x + pattern[0].x, prev_p.y + pattern[0].y - aa_shift1,
                               -(main_line->p_act.x - prev_p.x + 1), mask, style->line.color, opa);
            lv_draw_aa_hor_seg(prev_p.x + pattern[width_safe - 1].x, prev_p.y + pattern[width_safe - 1].y + aa_shift2,
                               main_line->p_act.x - prev_p.x + 1, mask, style->line.color, opa);
        }
#endif
    }
    /*Rather a vertical line*/
    else {

        while(line_next_x(main_line)) {
            for(i = 0; i < width; i++) {
                draw_area.x1 = prev_p.x + pattern[i].x;
                draw_area.y1 = prev_p.y + pattern[i].y;
                draw_area.x2 = draw_area.x1;
                draw_area.y2 = draw_area.y1 + main_line->p_act.y - prev_p.y - 1;

                lv_draw_fill(&draw_area, mask, style->line.color, opa);

                /* Fill the gaps
                 * When stepping in x one pixel remains empty on every corner (don't do this on the
                 * first segment ) */
                if(i != 0 && pattern[i].y != pattern[i - 1].y && !first_run) {
                    lv_draw_px(draw_area.x1 - main_line->sx, draw_area.y1, mask, style->line.color, opa);
                }
            }

#if LV_ANTIALIAS
            if(aa) {
                lv_draw_aa_ver_seg(prev_p.x + pattern[0].x - aa_shift1, prev_p.y + pattern[0].y,
                                   -(main_line->p_act.y - prev_p.y), mask, style->line.color, opa);
                lv_draw_aa_ver_seg(prev_p.x + pattern[width_safe - 1].x + aa_shift2,
                                   prev_p.y + pattern[width_safe - 1].y, main_line->p_act.y - prev_p.y, mask,
                                   style->line.color, opa);
            }
#endif

            first_run = false;

            prev_p.x = main_line->p_act.x;
            prev_p.y = main_line->p_act.y;
        }

        /*Draw the last part*/
        for(i = 0; i < width; i++) {
            draw_area.x1 = prev_p.x + pattern[i].x;
            draw_area.y1 = prev_p.y + pattern[i].y;
            draw_area.x2 = draw_area.x1;
            draw_area.y2 = draw_area.y1 + main_line->p_act.y - prev_p.y;

            lv_draw_fill(&draw_area, mask, style->line.color, opa);

            /* Fill the gaps
             * When stepping in x one pixel remains empty on every corner */
            if(i != 0 && pattern[i].y != pattern[i - 1].y && !first_run) {
                lv_draw_px(draw_area.x1 - main_line->sx, draw_area.y1, mask, style->line.color, opa);
            }
        }

#if LV_ANTIALIAS
        if(aa) {
            lv_draw_aa_ver_seg(prev_p.x + pattern[0].x - aa_shift1, prev_p.y + pattern[0].y,
                               -(main_line->p_act.y - prev_p.y + 1), mask, style->line.color, opa);
            lv_draw_aa_ver_seg(prev_p.x + pattern[width_safe - 1].x + aa_shift2, prev_p.y + pattern[width_safe - 1].y,
                               main_line->p_act.y - prev_p.y + 1, mask, style->line.color, opa);
        }
#endif
    }
}

static void line_init(line_draw_t * line, const lv_point_t * p1, const lv_point_t * p2)
{
    line->p1.x = p1->x;
    line->p1.y = p1->y;
    line->p2.x = p2->x;
    line->p2.y = p2->y;

    line->dx  = LV_MATH_ABS(line->p2.x - line->p1.x);
    line->sx  = line->p1.x < line->p2.x ? 1 : -1;
    line->dy  = LV_MATH_ABS(line->p2.y - line->p1.y);
    line->sy  = line->p1.y < line->p2.y ? 1 : -1;
    line->err = (line->dx > line->dy ? line->dx : -line->dy) / 2;
    line->e2  = 0;
    line->hor = line->dx > line->dy ? true : false; /*Rather horizontal or vertical*/

    line->p_act.x = line->p1.x;
    line->p_act.y = line->p1.y;
}

static bool line_next(line_draw_t * line)
{
    if(line->p_act.x == line->p2.x && line->p_act.y == line->p2.y) return false;
    line->e2 = line->err;
    if(line->e2 > -line->dx) {
        line->err -= line->dy;
        line->p_act.x += line->sx;
    }
    if(line->e2 < line->dy) {
        line->err += line->dx;
        line->p_act.y += line->sy;
    }
    return true;
}

/**
 * Iterate until step one in y direction.
 * @param line
 * @return
 */
static bool line_next_y(line_draw_t * line)
{
    lv_coord_t last_y = line->p_act.y;

    do {
        if(!line_next(line)) return false;
    } while(last_y == line->p_act.y);

    return true;
}

/**
 * Iterate until step one in x direction.
 * @param line
 * @return
 */
static bool line_next_x(line_draw_t * line)
{
    lv_coord_t last_x = line->p_act.x;

    do {
        if(!line_next(line)) return false;
    } while(last_x == line->p_act.x);

    return true;
}
