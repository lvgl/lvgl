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

    EVE_CoDl_pointSize(u->hal, radius * 16 + 8);  /* +8 for half-pixel alignment */
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
        EVE_CoDl_lineWidth(u->hal, 16);
        EVE_CoDl_begin(u->hal, RECTS);
        EVE_CoDl_vertex2f_0(u->hal, x1 - 2, y1 - 2);
        EVE_CoDl_vertex2f_0(u->hal, x2 + 2, y2 + 2);
        EVE_CoDl_end(u->hal);
    } else {
        EVE_CoDl_lineWidth(u->hal, radius * 16 + 8);  /* +8 for half-pixel alignment */
        EVE_CoDl_begin(u->hal, RECTS);
        EVE_CoDl_vertex2f_0(u->hal, x1 + radius, y1 + radius);
        EVE_CoDl_vertex2f_0(u->hal, x2 - radius, y2 - radius);
        EVE_CoDl_end(u->hal);
    }

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

    /* Determine fill color - use gradient color if available, otherwise solid color */
    lv_color_t fill_color;
    uint8_t fill_opa = dsc->opa;

    if(dsc->grad.dir != LV_GRAD_DIR_NONE && dsc->grad.stops_count > 0) {
        /* Gradient defined - pick a representative color */
        if(dsc->grad.stops_count == 1) {
            /* Single stop - just use it */
            fill_color = dsc->grad.stops[0].color;
            fill_opa = LV_OPA_MIX2(dsc->opa, dsc->grad.stops[0].opa);
        }
        else {
            /* Multiple stops - blend first and last for an average */
            lv_color_t c0 = dsc->grad.stops[0].color;
            lv_color_t c1 = dsc->grad.stops[dsc->grad.stops_count - 1].color;
            fill_color.red   = (c0.red   + c1.red)   / 2;
            fill_color.green = (c0.green + c1.green) / 2;
            fill_color.blue  = (c0.blue  + c1.blue)  / 2;
            fill_opa = LV_OPA_MIX2(dsc->opa,
                                   (dsc->grad.stops[0].opa +
                                    dsc->grad.stops[dsc->grad.stops_count - 1].opa) / 2);
        }
    }
    else {
        /* No gradient - use solid color */
        fill_color = dsc->color;
    }

    EVE_CoDl_colorRgb(u->hal, fill_color.red, fill_color.green, fill_color.blue);
    EVE_CoDl_colorA(u->hal, fill_opa);

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

    const lv_area_t *clip = &t->clip_area;
    const lv_area_t *layer_area = &layer->buf_area;

    /* Convert clip to layer coordinates */
    int32_t clip_x1 = clip->x1 - layer_area->x1;
    int32_t clip_y1 = clip->y1 - layer_area->y1;
    int32_t clip_x2 = clip->x2 - layer_area->x1;
    int32_t clip_y2 = clip->y2 - layer_area->y1;

    /*
     * Optimization: if all corners are outside the clip area, we can draw
     * simple rectangles instead of using the expensive stencil approach.
     * Check if clip area avoids all four corner regions.
     */
    bool corners_clipped = (rout > 0) &&
                           (clip_x1 > x1 + rout || clip_x2 < x1 + rout || 
                            clip_y1 > y1 + rout) &&  /* top-left */
                           (clip_x1 > x2 - rout || clip_x2 < x2 - rout || 
                            clip_y1 > y1 + rout) &&  /* top-right */
                           (clip_x1 > x1 + rout || clip_x2 < x1 + rout || 
                            clip_y2 < y2 - rout) &&  /* bottom-left */
                           (clip_x1 > x2 - rout || clip_x2 < x2 - rout || 
                            clip_y2 < y2 - rout);    /* bottom-right */

    if(corners_clipped || rout == 0) {
        /* Fast path: draw borders as simple lines */
        lv_draw_eve5_set_scissor(u, clip, layer_area);

        EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
        EVE_CoDl_colorA(u->hal, dsc->opa);
        EVE_CoDl_lineWidth(u->hal, dsc->width * 8);

        EVE_CoDl_begin(u->hal, LINES);

        if(dsc->side & LV_BORDER_SIDE_TOP) {
            int32_t y = y1 + dsc->width / 2;
            EVE_CoDl_vertex2f_1(u->hal, x1 << 1, (y << 1) - 1);
            EVE_CoDl_vertex2f_1(u->hal, x2 << 1, (y << 1) - 1);
        }
        if(dsc->side & LV_BORDER_SIDE_BOTTOM) {
            int32_t y = y2 - dsc->width / 2;
            EVE_CoDl_vertex2f_1(u->hal, x1 << 1, (y << 1) + 1);
            EVE_CoDl_vertex2f_1(u->hal, x2 << 1, (y << 1) + 1);
        }
        if(dsc->side & LV_BORDER_SIDE_LEFT) {
            int32_t x = x1 + dsc->width / 2;
            EVE_CoDl_vertex2f_1(u->hal, (x << 1) - 1, y1 << 1);
            EVE_CoDl_vertex2f_1(u->hal, (x << 1) - 1, y2 << 1);
        }
        if(dsc->side & LV_BORDER_SIDE_RIGHT) {
            int32_t x = x2 - dsc->width / 2;
            EVE_CoDl_vertex2f_1(u->hal, (x << 1) + 1, y1 << 1);
            EVE_CoDl_vertex2f_1(u->hal, (x << 1) + 1, y2 << 1);
        }

        EVE_CoDl_end(u->hal);
        return;
    }

    /* Full stencil-based path for visible rounded corners */

    /* Calculate inner area based on border sides */
    int32_t inner_x1 = x1 + ((dsc->side & LV_BORDER_SIDE_LEFT)   ? dsc->width : -dsc->width);
    int32_t inner_x2 = x2 - ((dsc->side & LV_BORDER_SIDE_RIGHT)  ? dsc->width : -dsc->width);
    int32_t inner_y1 = y1 + ((dsc->side & LV_BORDER_SIDE_TOP)    ? dsc->width : -dsc->width);
    int32_t inner_y2 = y2 - ((dsc->side & LV_BORDER_SIDE_BOTTOM) ? dsc->width : -dsc->width);

    int32_t rin = rout - dsc->width;
    if(rin < 0) rin = 0;

    lv_draw_eve5_set_scissor(u, clip, layer_area);

    EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(u->hal, dsc->opa);

    /* Use stencil to create hollow border */

    /* Step 1: Draw outer rect to alpha only, init stencil */
    EVE_CoDl_colorMask(u->hal, 0, 0, 0, 1);
    EVE_CoDl_stencilFunc(u->hal, ALWAYS, 0, 1);
    EVE_CoDl_stencilOp(u->hal, REPLACE, REPLACE);
    draw_rect(u, x1, y1, x2, y2, rout, clip, layer_area);

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
 * FILL + BORDER UNIFIED
 **********************/

void lv_draw_eve5_hal_draw_fill_with_border(lv_draw_eve5_unit_t *u,
                                             const lv_draw_task_t *fill_task,
                                             const lv_draw_task_t *border_task)
{
    const lv_draw_fill_dsc_t *fill_dsc = fill_task->draw_dsc;
    const lv_draw_border_dsc_t *border_dsc = border_task->draw_dsc;

    if (fill_dsc->opa <= LV_OPA_MIN) {
        if (border_dsc->opa <= LV_OPA_MIN) {
            return;
        }
        lv_draw_eve5_hal_draw_border(u, border_task);
        return;
    }
    if (border_dsc->opa <= LV_OPA_MIN) {
        lv_draw_eve5_hal_draw_fill(u, fill_task);
        return;
    }

    /* Check if we can use the optimized two-rectangle approach:
     * - Fill has 100% opacity (so we can blend border color onto it)
     * - Border covers all sides
     * - No gradient on fill
     */
    bool can_optimize = (fill_dsc->opa >= LV_OPA_MAX) &&
                        (border_dsc->side == LV_BORDER_SIDE_FULL) &&
                        (border_dsc->width > 0) &&
                        (fill_dsc->grad.dir == LV_GRAD_DIR_NONE);

    if(!can_optimize) {
        /* Fallback: render separately */
        lv_draw_eve5_hal_draw_fill(u, fill_task);
        lv_draw_eve5_hal_draw_border(u, border_task);
        return;
    }

    /* Optimized path: outer rect (border color) + inner rect (fill color) */
    lv_layer_t *layer = border_task->target_layer;
    const lv_area_t *layer_area = &layer->buf_area;

    int32_t x1 = border_task->area.x1 - layer_area->x1;
    int32_t y1 = border_task->area.y1 - layer_area->y1;
    int32_t x2 = border_task->area.x2 - layer_area->x1;
    int32_t y2 = border_task->area.y2 - layer_area->y1;
    int32_t w = x2 - x1 + 1;
    int32_t h = y2 - y1 + 1;

    int32_t rout = border_dsc->radius;
    int32_t short_side = LV_MIN(w, h);
    if(rout > short_side >> 1) rout = short_side >> 1;

    int32_t rin = rout - border_dsc->width;
    if(rin < 0) rin = 0;

    lv_area_t clip;
    if(!lv_area_intersect(&clip, &fill_task->clip_area, &border_task->clip_area)) {
        return;
    }

    lv_draw_eve5_set_scissor(u, &clip, layer_area);

    /* Calculate effective border color by blending with fill.
     * Since fill is opaque, border_color * border_opa + fill_color * (1 - border_opa)
     * gives us what the border region should look like. */
    uint8_t border_r, border_g, border_b;
    if(border_dsc->opa >= LV_OPA_MAX) {
        /* Full opacity - use border color directly */
        border_r = border_dsc->color.red;
        border_g = border_dsc->color.green;
        border_b = border_dsc->color.blue;
    }
    else {
        /* Partial opacity - blend border over fill */
        uint8_t opa = border_dsc->opa;
        uint8_t inv_opa = 255 - opa;
        border_r = (border_dsc->color.red * opa + fill_dsc->color.red * inv_opa) / 255;
        border_g = (border_dsc->color.green * opa + fill_dsc->color.green * inv_opa) / 255;
        border_b = (border_dsc->color.blue * opa + fill_dsc->color.blue * inv_opa) / 255;
    }

    /* Outer rectangle (blended border color) */
    EVE_CoDl_colorRgb(u->hal, border_r, border_g, border_b);
    EVE_CoDl_colorA(u->hal, 255);
    draw_rect(u, x1, y1, x2, y2, rout, &clip, layer_area);

    /* Inner rectangle (fill color) */
    int32_t bw = border_dsc->width;
    int32_t inner_x1 = x1 + bw;
    int32_t inner_y1 = y1 + bw;
    int32_t inner_x2 = x2 - bw;
    int32_t inner_y2 = y2 - bw;

    if(inner_x2 > inner_x1 && inner_y2 > inner_y1) {
        EVE_CoDl_colorRgb(u->hal, fill_dsc->color.red, fill_dsc->color.green, fill_dsc->color.blue);
        draw_rect(u, inner_x1, inner_y1, inner_x2, inner_y2, rin, &clip, layer_area);
    }
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

    if(false) { // ((is_vertical || is_horizontal) && no_round) {
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
        /* Diagonal or rounded - use LINES */
        EVE_CoDl_lineWidth(u->hal, line_w);
        EVE_CoDl_begin(u->hal, LINES);
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

    /* Convert to layer coordinates */
    lv_point_t p[3];
    p[0] = lv_point_from_precise(&dsc->p[0]);
    p[1] = lv_point_from_precise(&dsc->p[1]);
    p[2] = lv_point_from_precise(&dsc->p[2]);

    for(int i = 0; i < 3; i++) {
        p[i].x -= layer->buf_area.x1;
        p[i].y -= layer->buf_area.y1;
    }

    /* Calculate bounding box in layer coordinates */
    int32_t xmin = LV_MIN3(p[0].x, p[1].x, p[2].x);
    int32_t ymin = LV_MIN3(p[0].y, p[1].y, p[2].y);
    int32_t xmax = LV_MAX3(p[0].x, p[1].x, p[2].x);
    int32_t ymax = LV_MAX3(p[0].y, p[1].y, p[2].y);

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    /* Determine fill color - use gradient color if available, otherwise solid color */
    lv_color_t fill_color;
    uint8_t fill_opa = dsc->opa;
    
    if(dsc->grad.dir != LV_GRAD_DIR_NONE && dsc->grad.stops_count > 0) {
        /* Gradient defined - pick a representative color */
        if(dsc->grad.stops_count == 1) {
            /* Single stop - just use it */
            fill_color = dsc->grad.stops[0].color;
            fill_opa = LV_OPA_MIX2(dsc->opa, dsc->grad.stops[0].opa);
        }
        else {
            /* Multiple stops - blend first and last for an average */
            lv_color_t c0 = dsc->grad.stops[0].color;
            lv_color_t c1 = dsc->grad.stops[dsc->grad.stops_count - 1].color;
            fill_color.red   = (c0.red   + c1.red)   / 2;
            fill_color.green = (c0.green + c1.green) / 2;
            fill_color.blue  = (c0.blue  + c1.blue)  / 2;
            fill_opa = LV_OPA_MIX2(dsc->opa, 
                                   (dsc->grad.stops[0].opa + 
                                    dsc->grad.stops[dsc->grad.stops_count - 1].opa) / 2);
        }
    }
    else {
        /* No gradient - use solid color */
        fill_color = dsc->color;
    }

    EVE_CoDl_colorRgb(u->hal, fill_color.red, fill_color.green, fill_color.blue);
    EVE_CoDl_colorA(u->hal, fill_opa);

    /* Use stencil with edge strip to fill triangle */
    EVE_CoDl_colorMask(u->hal, 0, 0, 0, 0);
    EVE_CoDl_stencilOp(u->hal, KEEP, INVERT);
    EVE_CoDl_stencilFunc(u->hal, ALWAYS, 255, 255);

    EVE_CoDl_begin(u->hal, EDGE_STRIP_B);
    EVE_CoDl_vertex2f_0(u->hal, p[0].x, p[0].y);
    EVE_CoDl_vertex2f_0(u->hal, p[1].x, p[1].y);
    EVE_CoDl_vertex2f_0(u->hal, p[2].x, p[2].y);
    EVE_CoDl_vertex2f_0(u->hal, p[0].x, p[0].y);  /* Close the triangle */
    EVE_CoDl_end(u->hal);

    /* Draw where stencil was inverted (inside triangle) using RECTS */
    EVE_CoDl_colorMask(u->hal, 1, 1, 1, 1);
    EVE_CoDl_stencilFunc(u->hal, EQUAL, 255, 255);
    EVE_CoDl_lineWidth(u->hal, 16);

    EVE_CoDl_begin(u->hal, RECTS);
    EVE_CoDl_vertex2f_0(u->hal, xmin, ymin);
    EVE_CoDl_vertex2f_0(u->hal, xmax, ymax);
    EVE_CoDl_end(u->hal);

    reset_stencil(u);
}

/**********************
 * ARC DRAWING
 **********************/

#if EVE_SUPPORT_CHIPID >= EVE_BT820
/* Convert degrees (0-360) to furmans (0x0000-0xFFFF) */
static uint16_t degrees_to_furmans(int32_t degrees)
{
    /* LVGL: 0° at 3 o'clock, EVE: 0 furmans at 12 o'clock */
    /* Subtract 90° to rotate coordinate system */
    degrees = (degrees - 90) % 360;
    if(degrees < 0) degrees += 360;
    return (uint16_t)((degrees * 65536UL) / 360);
}
#endif

/* Stencil-based arc drawing for previous generation chips and edge cases */
static void draw_arc_stencil(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t,
                             int32_t cx, int32_t cy, int32_t radius_out, int32_t radius_in,
                             int32_t start_angle, int32_t end_angle)
{
    lv_draw_arc_dsc_t *dsc = t->draw_dsc;
    const lv_area_t *clip = &t->clip_area;
    const lv_area_t *layer_area = &t->target_layer->buf_area;
    int32_t width = dsc->width;

    if(width > radius_out) width = radius_out;

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

void lv_draw_eve5_hal_draw_arc(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    lv_layer_t *layer = t->target_layer;
    lv_draw_arc_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(dsc->start_angle == dsc->end_angle) return;

    int32_t cx = dsc->center.x - layer->buf_area.x1;
    int32_t cy = dsc->center.y - layer->buf_area.y1;
    int32_t radius_out = dsc->radius;
    int32_t radius_in = dsc->radius - dsc->width;

    if(radius_in < 0) radius_in = 0;

    int32_t start_angle = ((int32_t)dsc->start_angle) % 360;
    int32_t end_angle = ((int32_t)dsc->end_angle) % 360;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(u->hal, dsc->opa);

#if EVE_SUPPORT_CHIPID >= EVE_BT820
    /*
     * Use CMD_ARC (BT820+) when possible:
     * - Chip must be BT820 or newer (runtime check for multi-target builds)
     * - Radii must be within 1-511 range
     * - Arc must have rounded ends (CMD_ARC always draws rounded caps)
     */
    bool use_cmd_arc = (EVE_CHIPID >= EVE_BT820) &&
                       (radius_out <= 511) &&
                       (radius_in >= 1 || radius_in == 0) &&
                       (dsc->rounded);

    if(use_cmd_arc) {
        /* Handle filled arc (radius_in == 0) - CMD_ARC requires r0 >= 1 */
        int32_t r_in = (radius_in == 0) ? 1 : radius_in;

        uint16_t a0 = degrees_to_furmans(start_angle);
        uint16_t a1 = degrees_to_furmans(end_angle);

        EVE_CoCmd_arc(u->hal, cx, cy, r_in, radius_out, a0, a1);
        return;
    }
#endif

    /* Fallback: stencil-based rendering for pre-BT820 or unsupported cases */
    draw_arc_stencil(u, t, cx, cy, radius_out, radius_in, start_angle, end_angle);
}

#endif /* LV_USE_DRAW_EVE5 */
