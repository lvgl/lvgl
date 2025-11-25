/**
 * @file lv_draw_eve5_primitives.c
 *
 * EVE5 (BT820) Primitive Drawing Implementation
 *
 * Hardware-accelerated rendering for geometric primitives:
 * - Filled rectangles and circles
 * - Borders (hollow rectangles with rounded corners)
 * - Lines (horizontal, vertical, diagonal)
 * - Triangles
 * - Arcs (with rounded ends, angle masking)
 *
 * Uses EVE's stencil buffer for complex shapes and masking operations.
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw.h"
#include "../lv_draw_rect.h"
#include "../lv_draw_line.h"
#include "../lv_draw_triangle.h"
#include "../lv_draw_arc.h"

/**********************
 * STATIC PROTOTYPES
 **********************/

/* Primitive helpers */
static void draw_circle(lv_draw_eve5_unit_t *u, int32_t cx, int32_t cy, int32_t radius);
static void draw_rect(lv_draw_eve5_unit_t *u, int32_t x1, int32_t y1, int32_t x2, int32_t y2, 
                      int32_t radius, const lv_area_t *clip_area, const lv_area_t *layer_area);

/* Stencil reset (call after any stencil-based rendering) */
static void reset_stencil(lv_draw_eve5_unit_t *u);

/* Arc helpers */
static void draw_arc_mask_angle(lv_draw_eve5_unit_t *u, const lv_draw_arc_dsc_t *dsc,
                                int32_t cx, int32_t cy, int32_t start_angle, int32_t end_angle,
                                const lv_area_t *clip_area, const lv_area_t *layer_area);
static uint8_t get_edge_strip_direction(int16_t angle);
static bool is_same_quadrant(int16_t start_angle, int16_t end_angle);
static int32_t chord_length(int16_t radius, int16_t angle_degrees);

/**********************
 * PRIMITIVE HELPERS
 **********************/

static void draw_circle(lv_draw_eve5_unit_t *u, int32_t cx, int32_t cy, int32_t radius)
{
    if(radius <= 0) return;

    EVE_CoDl_pointSize(u->hal, radius * 16);
    EVE_CoDl_begin(u->hal, POINTS);
    EVE_CoDl_vertex2f_0(u->hal, cx, cy);
    EVE_CoDl_end(u->hal);
}

static void draw_rect(lv_draw_eve5_unit_t *u, int32_t x1, int32_t y1, int32_t x2, int32_t y2, 
                      int32_t radius, const lv_area_t *clip_area, const lv_area_t *layer_area)
{
    int32_t w = x2 - x1;
    int32_t h = y2 - y1;

    if(w <= 0 || h <= 0) return;

    int32_t max_r = LV_MIN(w, h) / 2;
    if(radius > max_r) radius = max_r;
    if(radius < 0) radius = 0;

    bool needs_scissor_fix = (radius < 1);

    /* EVE hardware quirk: lineWidth < 16 (1 pixel in 1/16 units) causes
     * alpha blending artifacts that fade corners to invisible.
     * Fix: use minimum radius of 1 and scissor to get hard edges. */
    if(needs_scissor_fix) {
        if(clip_area && layer_area) {
            /* Convert rect to screen coords */
            lv_area_t rect_screen;
            rect_screen.x1 = x1 + layer_area->x1;
            rect_screen.y1 = y1 + layer_area->y1;
            rect_screen.x2 = x2 + layer_area->x1;
            rect_screen.y2 = y2 + layer_area->y1;

            /* Intersect with existing clip - don't expand beyond what's allowed */
            lv_area_t scissor_area;
            if(!lv_area_intersect(&scissor_area, &rect_screen, clip_area)) {
                return;  /* Completely clipped, nothing to draw */
            }

            lv_draw_eve5_set_scissor(u, &scissor_area, layer_area);
        } else {
            /* No clip info - just use rect bounds directly (less safe but functional) */
            EVE_CoDl_scissorXY(u->hal, x1, y1);
            EVE_CoDl_scissorSize(u->hal, w, h);
        }
        radius = 1;
    }

    EVE_CoDl_lineWidth(u->hal, radius * 16);
    EVE_CoDl_begin(u->hal, RECTS);
    EVE_CoDl_vertex2f_0(u->hal, x1 + radius, y1 + radius);
    EVE_CoDl_vertex2f_0(u->hal, x2 - radius, y2 - radius);
    EVE_CoDl_end(u->hal);

    /* Restore original scissor */
    if(needs_scissor_fix && clip_area && layer_area) {
        lv_draw_eve5_set_scissor(u, clip_area, layer_area);
    }
}

static void reset_stencil(lv_draw_eve5_unit_t *u)
{
    EVE_CoDl_stencilFunc(u->hal, ALWAYS, 0, 0);
    EVE_CoDl_stencilOp(u->hal, KEEP, KEEP);
    EVE_CoDl_colorMask(u->hal, 1, 1, 1, 1);
    EVE_CoDl_blendFunc(u->hal, SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
}

/**********************
 * ARC HELPERS
 **********************/

static int32_t chord_length(int16_t radius, int16_t angle_degrees)
{
    angle_degrees %= 360;
    if(angle_degrees < 0) angle_degrees += 360;
    int32_t sin_value = lv_trigo_sin(angle_degrees / 2);
    return (int32_t)(2 * radius * sin_value / 32768);
}

static uint8_t get_edge_strip_direction(int16_t angle)
{
    if(angle >= 315 || angle < 45)  return EDGE_STRIP_R;
    if(angle >= 45  && angle < 135) return EDGE_STRIP_B;
    if(angle >= 135 && angle < 225) return EDGE_STRIP_L;
    if(angle >= 225 && angle < 315) return EDGE_STRIP_A;
    return EDGE_STRIP_R;
}

static bool is_same_quadrant(int16_t start_angle, int16_t end_angle)
{
    if(start_angle <= end_angle) return false;

    int q_start = start_angle / 90;
    int q_end = end_angle / 90;
    return (q_start == q_end);
}

static void draw_arc_mask_angle(lv_draw_eve5_unit_t *u, const lv_draw_arc_dsc_t *dsc,
                                int32_t cx, int32_t cy, int32_t start_angle, int32_t end_angle,
                                const lv_area_t *clip_area, const lv_area_t *layer_area)
{
    /* Constrain angles */
    start_angle = LV_CLAMP(0, start_angle, 359);
    end_angle = LV_CLAMP(0, end_angle, 359);

    int32_t angle_range;
    if(end_angle > start_angle) {
        angle_range = end_angle - start_angle;
    }
    else {
        angle_range = 360 - start_angle + end_angle;
    }

    int32_t mid_angle_op = ((angle_range / 2) + start_angle + 180) % 360;
    int32_t mask_dir_end = (((360 - angle_range) / 4) + end_angle) % 360;
    int32_t mask_dir_start = (((360 - angle_range) / 4) + mid_angle_op) % 360;

    /* Calculate edge points (using >> 5 for ~1024 pixel radius) */
    lv_point_t start_pt, end_pt, mid_pt;
    start_pt.x = (lv_trigo_cos(start_angle) >> 5) + cx;
    start_pt.y = (lv_trigo_sin(start_angle) >> 5) + cy;
    end_pt.x = (lv_trigo_cos(end_angle) >> 5) + cx;
    end_pt.y = (lv_trigo_sin(end_angle) >> 5) + cy;
    mid_pt.x = (lv_trigo_cos(mid_angle_op) >> 5) + cx;
    mid_pt.y = (lv_trigo_sin(mid_angle_op) >> 5) + cy;

    if(angle_range <= 180) {
        /* Two-sided mask with 6 vertices */

        /* Mask end angle */
        uint8_t edge = get_edge_strip_direction(mask_dir_end);
        EVE_CoDl_begin(u->hal, edge);
        EVE_CoDl_vertex2f_0(u->hal, mid_pt.x, mid_pt.y);
        EVE_CoDl_vertex2f_0(u->hal, cx, cy);
        EVE_CoDl_vertex2f_0(u->hal, end_pt.x, end_pt.y);
        EVE_CoDl_end(u->hal);

        /* Mask start angle */
        edge = get_edge_strip_direction(mask_dir_start);
        EVE_CoDl_begin(u->hal, edge);
        EVE_CoDl_vertex2f_0(u->hal, mid_pt.x, mid_pt.y);
        EVE_CoDl_vertex2f_0(u->hal, cx, cy);
        EVE_CoDl_vertex2f_0(u->hal, start_pt.x, start_pt.y);
        EVE_CoDl_end(u->hal);
    }
    else if(is_same_quadrant(start_angle, end_angle)) {
        /* Both angles in same quadrant - special case */
        int16_t chord = chord_length(dsc->radius, 360 - angle_range);
        int16_t r_width = LV_MAX(chord / 4, 1);

        lv_point_t end_brd, start_brd;
        end_brd.x = cx + ((lv_trigo_cos(end_angle) * dsc->radius) >> LV_TRIGO_SHIFT);
        end_brd.y = cy + ((lv_trigo_sin(end_angle) * dsc->radius) >> LV_TRIGO_SHIFT);
        start_brd.x = cx + ((lv_trigo_cos(start_angle) * dsc->radius) >> LV_TRIGO_SHIFT);
        start_brd.y = cy + ((lv_trigo_sin(start_angle) * dsc->radius) >> LV_TRIGO_SHIFT);

        /* Draw blocking rectangle */
        draw_rect(u, start_brd.x, start_brd.y, end_brd.x, end_brd.y, 0, clip_area, layer_area);

        lv_point_t start_brd2, end_brd2, start_ctr, end_ctr;
        start_brd2.x = start_brd.x + ((lv_trigo_cos(start_angle - 90) * r_width) >> LV_TRIGO_SHIFT);
        start_brd2.y = start_brd.y + ((lv_trigo_sin(start_angle - 90) * r_width) >> LV_TRIGO_SHIFT);
        end_brd2.x = end_brd.x + ((lv_trigo_cos(end_angle + 90) * r_width) >> LV_TRIGO_SHIFT);
        end_brd2.y = end_brd.y + ((lv_trigo_sin(end_angle + 90) * r_width) >> LV_TRIGO_SHIFT);
        end_ctr.x = cx + ((lv_trigo_cos(end_angle + 90) * r_width) >> LV_TRIGO_SHIFT);
        end_ctr.y = cy + ((lv_trigo_sin(end_angle + 90) * r_width) >> LV_TRIGO_SHIFT);
        start_ctr.x = cx + ((lv_trigo_cos(start_angle + 270) * r_width) >> LV_TRIGO_SHIFT);
        start_ctr.y = cy + ((lv_trigo_sin(start_angle + 270) * r_width) >> LV_TRIGO_SHIFT);

        EVE_CoDl_lineWidth(u->hal, r_width * 16);
        EVE_CoDl_begin(u->hal, LINE_STRIP);
        EVE_CoDl_vertex2f_0(u->hal, start_ctr.x, start_ctr.y);
        EVE_CoDl_vertex2f_0(u->hal, start_brd2.x, start_brd2.y);
        EVE_CoDl_vertex2f_0(u->hal, end_brd2.x, end_brd2.y);
        EVE_CoDl_vertex2f_0(u->hal, end_ctr.x, end_ctr.y);
        EVE_CoDl_end(u->hal);
    }
    else {
        /* Single-sided mask with 3 vertices */
        uint8_t edge = get_edge_strip_direction(mid_angle_op);
        EVE_CoDl_begin(u->hal, edge);
        EVE_CoDl_vertex2f_0(u->hal, end_pt.x, end_pt.y);
        EVE_CoDl_vertex2f_0(u->hal, cx, cy);
        EVE_CoDl_vertex2f_0(u->hal, start_pt.x, start_pt.y);
        EVE_CoDl_end(u->hal);
    }
}

/**********************
 * FILL DRAWING
 **********************/

void lv_draw_eve5_hal_draw_fill(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    lv_layer_t *layer = t->target_layer;
    lv_draw_fill_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;

    int32_t x1 = t->area.x1 - layer->buf_area.x1;
    int32_t y1 = t->area.y1 - layer->buf_area.y1;
    int32_t x2 = t->area.x2 - layer->buf_area.x1;
    int32_t y2 = t->area.y2 - layer->buf_area.y1;
    int32_t w = x2 - x1 + 1;
    int32_t h = y2 - y1 + 1;

    const lv_area_t *clip = &t->clip_area;
    const lv_area_t *layer_area = &layer->buf_area;

    lv_draw_eve5_set_scissor(u, clip, layer_area);

    EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(u->hal, dsc->opa);

    int32_t radius = dsc->radius;
    int32_t real_radius = LV_MIN3(w / 2, h / 2, radius);

    /* Check for perfect circle */
    if(w == h && radius == LV_RADIUS_CIRCLE) {
        draw_circle(u, x1 + w / 2, y1 + h / 2, real_radius);
    }
    else {
        draw_rect(u, x1, y1, x2, y2, real_radius, clip, layer_area);
    }
}

/**********************
 * BORDER DRAWING
 **********************/

void lv_draw_eve5_hal_draw_border(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    lv_layer_t *layer = t->target_layer;
    lv_draw_border_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(dsc->side == LV_BORDER_SIDE_NONE) return;

    int32_t x1 = t->area.x1 - layer->buf_area.x1;
    int32_t y1 = t->area.y1 - layer->buf_area.y1;
    int32_t x2 = t->area.x2 - layer->buf_area.x1;
    int32_t y2 = t->area.y2 - layer->buf_area.y1;
    int32_t w = x2 - x1 + 1;
    int32_t h = y2 - y1 + 1;

    int32_t rout = dsc->radius;
    int32_t short_side = LV_MIN(w, h);
    if(rout > short_side >> 1) rout = short_side >> 1;

    /* Calculate inner area based on border sides */
    int32_t inner_x1 = x1 + ((dsc->side & LV_BORDER_SIDE_LEFT)   ? dsc->width : -dsc->width);
    int32_t inner_x2 = x2 - ((dsc->side & LV_BORDER_SIDE_RIGHT)  ? dsc->width : -dsc->width);
    int32_t inner_y1 = y1 + ((dsc->side & LV_BORDER_SIDE_TOP)    ? dsc->width : -dsc->width);
    int32_t inner_y2 = y2 - ((dsc->side & LV_BORDER_SIDE_BOTTOM) ? dsc->width : -dsc->width);

    int32_t rin = rout - dsc->width;
    if(rin < 0) rin = 0;

    const lv_area_t *clip = &t->clip_area;
    const lv_area_t *layer_area = &layer->buf_area;

    lv_draw_eve5_set_scissor(u, clip, layer_area);

    EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(u->hal, dsc->opa);

    /* Use stencil to create hollow border */

    /* Step 1: Draw outer rect to alpha only, init stencil */
    EVE_CoDl_colorMask(u->hal, 0, 0, 0, 1);
    EVE_CoDl_stencilFunc(u->hal, ALWAYS, 0, 1);
    EVE_CoDl_stencilOp(u->hal, REPLACE, REPLACE);
    draw_rect(u, x1, y1, x2, y2, 0, clip, layer_area);

    /* Step 2: Punch hole for inner region */
    EVE_CoDl_blendFunc(u->hal, ONE, ZERO);
    draw_rect(u, inner_x1 - 2, inner_y1 - 1, inner_x2 + 1, inner_y2 + 2, rin, clip, layer_area);

    /* Step 3: Set stencil for inner region */
    EVE_CoDl_stencilFunc(u->hal, ALWAYS, 1, 1);
    EVE_CoDl_stencilOp(u->hal, REPLACE, REPLACE);
    EVE_CoDl_blendFunc(u->hal, ZERO, ONE_MINUS_SRC_ALPHA);
    EVE_CoDl_colorA(u->hal, 255);
    draw_rect(u, inner_x1, inner_y1, inner_x2, inner_y2, rin, clip, layer_area);

    /* Step 4: Enable color output */
    EVE_CoDl_colorMask(u->hal, 1, 1, 1, 1);

    if(dsc->side == LV_BORDER_SIDE_FULL) {
        EVE_CoDl_blendFunc(u->hal, DST_ALPHA, ONE_MINUS_DST_ALPHA);
        draw_rect(u, inner_x1, inner_y1, inner_x2, inner_y2, rin, clip, layer_area);
    }

    /* Step 5: Draw final border where stencil != 1 */
    EVE_CoDl_stencilFunc(u->hal, NOTEQUAL, 1, 255);
    EVE_CoDl_blendFunc(u->hal, SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
    EVE_CoDl_colorA(u->hal, dsc->opa);
    draw_rect(u, x1, y1, x2, y2, rout, clip, layer_area);

    reset_stencil(u);
}

/**********************
 * LINE DRAWING
 **********************/

void lv_draw_eve5_hal_draw_line(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    lv_layer_t *layer = t->target_layer;
    lv_draw_line_dsc_t *dsc = t->draw_dsc;

    if(dsc->width == 0) return;
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->p1.x == dsc->p2.x && dsc->p1.y == dsc->p2.y) return;

    int32_t x1 = dsc->p1.x - layer->buf_area.x1;
    int32_t y1 = dsc->p1.y - layer->buf_area.y1;
    int32_t x2 = dsc->p2.x - layer->buf_area.x1;
    int32_t y2 = dsc->p2.y - layer->buf_area.y1;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(u->hal, dsc->opa);

    uint32_t line_w = dsc->width * 16; /* EVE uses 1/16 pixel units */

    /* Horizontal or vertical without rounding - use RECTS */
    bool is_vertical = (x1 == x2);
    bool is_horizontal = (y1 == y2);
    bool no_round = (!dsc->round_end && !dsc->round_start);

    if((is_vertical || is_horizontal) && no_round) {
        /* Adjust for line width */
        int32_t half_w = dsc->width / 2;
        int32_t rx1, ry1, rx2, ry2;

        if(is_vertical) {
            rx1 = x1 - half_w;
            rx2 = x1 + half_w;
            ry1 = LV_MIN(y1, y2);
            ry2 = LV_MAX(y1, y2);
        }
        else {
            ry1 = y1 - half_w;
            ry2 = y1 + half_w;
            rx1 = LV_MIN(x1, x2);
            rx2 = LV_MAX(x1, x2);
        }

        EVE_CoDl_begin(u->hal, RECTS);
        EVE_CoDl_vertex2f_0(u->hal, rx1, ry1);
        EVE_CoDl_vertex2f_0(u->hal, rx2, ry2);
        EVE_CoDl_end(u->hal);
    }
    else {
        /* Diagonal or rounded - use LINE_STRIP */
        EVE_CoDl_lineWidth(u->hal, line_w);
        EVE_CoDl_begin(u->hal, LINE_STRIP);
        EVE_CoDl_vertex2f_0(u->hal, x1, y1);
        EVE_CoDl_vertex2f_0(u->hal, x2, y2);
        EVE_CoDl_end(u->hal);
    }
}

/**********************
 * TRIANGLE DRAWING
 **********************/

void lv_draw_eve5_hal_draw_triangle(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    lv_layer_t *layer = t->target_layer;
    lv_draw_triangle_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;

    /* Check intersection with clip area */
    lv_area_t tri_area;
    tri_area.x1 = (int32_t)LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y1 = (int32_t)LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    tri_area.x2 = (int32_t)LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y2 = (int32_t)LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    lv_area_t draw_area;
    if(!lv_area_intersect(&draw_area, &tri_area, &t->clip_area)) return;

    /* Convert to layer coordinates and find optimal vertex order */
    lv_point_t p[3];

    /* If there's a vertical edge, use it as p[0]-p[1] */
    if(dsc->p[0].x == dsc->p[1].x) {
        p[0] = lv_point_from_precise(&dsc->p[0]);
        p[1] = lv_point_from_precise(&dsc->p[1]);
        p[2] = lv_point_from_precise(&dsc->p[2]);
    }
    else if(dsc->p[0].x == dsc->p[2].x) {
        p[0] = lv_point_from_precise(&dsc->p[0]);
        p[1] = lv_point_from_precise(&dsc->p[2]);
        p[2] = lv_point_from_precise(&dsc->p[1]);
    }
    else if(dsc->p[1].x == dsc->p[2].x) {
        p[0] = lv_point_from_precise(&dsc->p[1]);
        p[1] = lv_point_from_precise(&dsc->p[2]);
        p[2] = lv_point_from_precise(&dsc->p[0]);
    }
    else {
        p[0] = lv_point_from_precise(&dsc->p[0]);
        p[1] = lv_point_from_precise(&dsc->p[1]);
        p[2] = lv_point_from_precise(&dsc->p[2]);

        /* Sort by Y: smallest at p[0] */
        if(p[0].y > p[1].y) lv_point_swap(&p[0], &p[1]);
        if(p[0].y > p[2].y) lv_point_swap(&p[0], &p[2]);
        if(p[1].y < p[2].y) lv_point_swap(&p[1], &p[2]);
    }

    /* Ensure p[0] is on top */
    if(p[0].y > p[1].y) lv_point_swap(&p[0], &p[1]);

    /* Convert to layer coordinates */
    for(int i = 0; i < 3; i++) {
        p[i].x -= layer->buf_area.x1;
        p[i].y -= layer->buf_area.y1;
    }

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(u->hal, dsc->opa);

    /* Use stencil with edge strip to fill triangle */
    EVE_CoDl_colorMask(u->hal, 0, 0, 0, 0);
    EVE_CoDl_stencilOp(u->hal, KEEP, INVERT);
    EVE_CoDl_stencilFunc(u->hal, ALWAYS, 255, 255);

    EVE_CoDl_begin(u->hal, EDGE_STRIP_A);
    EVE_CoDl_vertex2f_0(u->hal, p[0].x, p[0].y);
    EVE_CoDl_vertex2f_0(u->hal, p[1].x, p[1].y);
    EVE_CoDl_vertex2f_0(u->hal, p[2].x, p[2].y);

    /* Draw where stencil was inverted (inside triangle) */
    EVE_CoDl_colorMask(u->hal, 1, 1, 1, 1);
    EVE_CoDl_stencilFunc(u->hal, EQUAL, 255, 255);

    EVE_CoDl_vertex2f_0(u->hal, 0, 0);
    EVE_CoDl_vertex2f_0(u->hal, 2046, 0); /* Max EVE coordinate */
    EVE_CoDl_end(u->hal);

    reset_stencil(u);
}

/**********************
 * ARC DRAWING
 **********************/

void lv_draw_eve5_hal_draw_arc(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    lv_layer_t *layer = t->target_layer;
    lv_draw_arc_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(dsc->start_angle == dsc->end_angle) return;

    int32_t cx = dsc->center.x - layer->buf_area.x1;
    int32_t cy = dsc->center.y - layer->buf_area.y1;
    int32_t width = dsc->width;
    int32_t radius_out = dsc->radius;
    int32_t radius_in = dsc->radius - dsc->width;

    if(width > radius_out) width = radius_out;
    if(radius_in < 0) radius_in = 0;

    int32_t start_angle = ((int32_t)dsc->start_angle) % 360;
    int32_t end_angle = ((int32_t)dsc->end_angle) % 360;

    const lv_area_t *clip = &t->clip_area;
    const lv_area_t *layer_area = &layer->buf_area;

    lv_draw_eve5_set_scissor(u, clip, layer_area);

    EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(u->hal, dsc->opa);

    /* Step 1: Draw outer circle to alpha only, init stencil */
    EVE_CoDl_colorMask(u->hal, 0, 0, 0, 1);
    EVE_CoDl_stencilFunc(u->hal, ALWAYS, 0, 1);
    EVE_CoDl_stencilOp(u->hal, REPLACE, REPLACE);
    draw_circle(u, cx, cy, radius_out);

    /* Step 2: Punch hole for inner radius */
    EVE_CoDl_blendFunc(u->hal, ONE, ZERO);
    draw_circle(u, cx, cy, radius_in + 2);

    /* Step 3: Apply angle mask */
    EVE_CoDl_stencilFunc(u->hal, ALWAYS, 1, 1);
    EVE_CoDl_stencilOp(u->hal, REPLACE, REPLACE);
    EVE_CoDl_blendFunc(u->hal, ZERO, ONE_MINUS_SRC_ALPHA);
    EVE_CoDl_colorA(u->hal, 255);

    draw_arc_mask_angle(u, dsc, cx, cy, start_angle, end_angle, clip, layer_area);

    /* Step 4: Draw inner circle */
    draw_circle(u, cx, cy, radius_in);

    /* Step 5: Composite with destination alpha */
    EVE_CoDl_colorMask(u->hal, 1, 1, 1, 1);
    EVE_CoDl_blendFunc(u->hal, DST_ALPHA, ONE_MINUS_DST_ALPHA);
    draw_circle(u, cx, cy, radius_in);

    /* Step 6: Draw final arc where stencil != 1 */
    EVE_CoDl_stencilFunc(u->hal, NOTEQUAL, 1, 255);
    EVE_CoDl_stencilOp(u->hal, KEEP, KEEP);
    EVE_CoDl_blendFunc(u->hal, SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
    EVE_CoDl_colorA(u->hal, dsc->opa);
    draw_circle(u, cx, cy, radius_out);

    /* Step 7: Rounded ends */
    if(dsc->rounded) {
        EVE_CoDl_stencilFunc(u->hal, EQUAL, 1, 255);
        if(dsc->opa < 255) {
            EVE_CoDl_stencilOp(u->hal, ZERO, ZERO);
        }

        int32_t half_width = width / 2;
        int32_t adj_radius = radius_out - half_width;

        /* End cap */
        int32_t ex = cx + ((lv_trigo_cos(end_angle) * adj_radius) >> LV_TRIGO_SHIFT);
        int32_t ey = cy + ((lv_trigo_sin(end_angle) * adj_radius) >> LV_TRIGO_SHIFT);
        draw_circle(u, ex, ey, half_width);

        /* Start cap */
        int32_t sx = cx + ((lv_trigo_cos(start_angle) * adj_radius) >> LV_TRIGO_SHIFT);
        int32_t sy = cy + ((lv_trigo_sin(start_angle) * adj_radius) >> LV_TRIGO_SHIFT);
        draw_circle(u, sx, sy, half_width);
    }

    reset_stencil(u);
}

#endif /* LV_USE_DRAW_EVE5 */
