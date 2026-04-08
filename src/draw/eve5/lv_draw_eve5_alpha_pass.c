/**
 * @file lv_draw_eve5_alpha_pass.c
 *
 * Direct-to-Alpha Correction Pass for EVE5 (BT820) Draw Unit
 *
 * Implements the direct-to-alpha recovery method: re-iterates tasks after
 * RGB rendering and writes correct Porter-Duff alpha using blend(ONE,
 * ONE_MINUS_SRC_ALPHA) with colorMask(0,0,0,1).
 *
 * Also contains the alpha accuracy predicates that determine whether a task
 * can use this pass or requires L8 render-target recovery instead.
 *
 * See lv_draw_eve5.c header for full alpha recovery architecture.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw.h"
#include "../lv_draw_image.h"
#include "../lv_draw_rect.h"
#include "../lv_draw_line.h"
#include "../lv_draw_triangle.h"
#include "../lv_draw_arc.h"
#include "../lv_draw_mask_private.h"

#if !LV_DRAW_EVE5_NO_FLOAT
#include <math.h>
#endif

/**********************
 * ALPHA ACCURACY PREDICATES
 *
 * Each predicate returns true when the task uses alpha-as-scratch masking
 * during RGB rendering, making direct-to-alpha replay impossible. These
 * tasks require L8 render-target recovery for accurate alpha.
 *
 * Tasks that only suffer from squared-alpha (standard blending) return
 * false, since direct-to-alpha replay can correct them accurately.
 **********************/

/**
 * FILL: true when rounded-corner + gradient with semi-transparent stops.
 * This path uses alpha-as-scratch masking (clear alpha, stamp rounded rect,
 * multiply gradient bitmap, draw through DST_ALPHA blend).
 */
bool lv_draw_eve5_fill_needs_alpha_rendertarget(const lv_draw_task_t *t)
{
    const lv_draw_fill_dsc_t *dsc = t->draw_dsc;
    if(dsc->opa <= LV_OPA_MIN) return false;

    int32_t w = lv_area_get_width(&t->area);
    int32_t h = lv_area_get_height(&t->area);
    int32_t real_radius = LV_MIN3((w - 1) / 2, (h - 1) / 2, dsc->radius);
    if(real_radius == 0) return false;

    if(dsc->grad.dir != LV_GRAD_DIR_HOR && dsc->grad.dir != LV_GRAD_DIR_VER) return false;
    if(dsc->grad.stops_count < 2) return false;

    for(uint8_t i = 0; i < dsc->grad.stops_count; i++) {
        if(dsc->grad.stops[i].opa < LV_OPA_MAX) return true;
    }
    return false;
}

/**
 * BORDER: true when using the alpha-as-scratch masking path.
 * Two RGB paths exist:
 *   - LINES path (rout==0 + full opa, or all corners clipped): no alpha trashing
 *   - Masking path (outer rect minus inner rect via alpha): trashes alpha
 */
bool lv_draw_eve5_border_needs_alpha_rendertarget(const lv_draw_task_t *t)
{
    const lv_draw_border_dsc_t *dsc = t->draw_dsc;
    if(dsc->opa <= LV_OPA_MIN) return false;
    if(dsc->width == 0) return false;
    if(dsc->side == LV_BORDER_SIDE_NONE) return false;

    lv_layer_t *layer = t->target_layer;
    const lv_area_t *layer_area = &layer->buf_area;

    int32_t x1 = t->area.x1 - layer_area->x1;
    int32_t y1 = t->area.y1 - layer_area->y1;
    int32_t x2 = t->area.x2 - layer_area->x1;
    int32_t y2 = t->area.y2 - layer_area->y1;
    int32_t w = x2 - x1 + 1;
    int32_t h = y2 - y1 + 1;

    int32_t rout = dsc->radius;
    int32_t max_r = (LV_MIN(w, h) - 1) / 2;
    if(rout > max_r) rout = max_r;

    /* LINES path: no alpha trashing */
    if(rout == 0 && dsc->opa >= LV_OPA_MAX) return false;

    /* Check if all corners are clipped away (falls back to LINES path) */
    int32_t clip_x1 = t->clip_area.x1 - layer_area->x1;
    int32_t clip_y1 = t->clip_area.y1 - layer_area->y1;
    int32_t clip_x2 = t->clip_area.x2 - layer_area->x1;
    int32_t clip_y2 = t->clip_area.y2 - layer_area->y1;

    bool corners_clipped = (rout > 0) &&
        (clip_x1 > x1 + rout || clip_x2 < x1 + rout || clip_y1 > y1 + rout) &&
        (clip_x1 > x2 - rout || clip_x2 < x2 - rout || clip_y1 > y1 + rout) &&
        (clip_x1 > x1 + rout || clip_x2 < x1 + rout || clip_y2 < y2 - rout) &&
        (clip_x1 > x2 - rout || clip_x2 < x2 - rout || clip_y2 < y2 - rout);

    if(corners_clipped) return false;

    return true;
}

/* Line alpha accuracy predicate is in lv_draw_eve5_line.c */

/**
 * ARC: always true for visible arcs.
 * RGB pass uses multi-phase stencil or CMD_ARC, both of which trash the alpha channel.
 */
bool lv_draw_eve5_arc_needs_alpha_rendertarget(const lv_draw_task_t *t)
{
    const lv_draw_arc_dsc_t *dsc = t->draw_dsc;
    if(dsc->opa <= LV_OPA_MIN) return false;
    if(dsc->width == 0) return false;
    if(dsc->start_angle == dsc->end_angle) return false;
    return true;
}

/**********************
 * ALPHA-ONLY DRAW FUNCTIONS
 **********************/

void lv_draw_eve5_alpha_draw_fill(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    lv_layer_t *layer = t->target_layer;
    const lv_draw_fill_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;

    int32_t x1 = t->area.x1 - layer->buf_area.x1;
    int32_t y1 = t->area.y1 - layer->buf_area.y1;
    int32_t x2 = t->area.x2 - layer->buf_area.x1;
    int32_t y2 = t->area.y2 - layer->buf_area.y1;
    int32_t w = x2 - x1 + 1;
    int32_t h = y2 - y1 + 1;

    int32_t radius = dsc->radius;
    int32_t real_radius = LV_MIN3((w - 1) / 2, (h - 1) / 2, radius);

    uint8_t opa = dsc->opa;

    /* Gradient with per-stop opacity and no rounded corners:
     * re-draw gradient ARGB8 strip for exact per-pixel alpha. */
    if(dsc->grad.dir != LV_GRAD_DIR_NONE && dsc->grad.stops_count >= 2 && real_radius == 0) {
        bool has_varying_opa = false;
        for(uint8_t i = 0; i < dsc->grad.stops_count; i++) {
            if(dsc->grad.stops[i].opa < LV_OPA_MAX) { has_varying_opa = true; break; }
        }
        if(has_varying_opa) {
            lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);
            EVE_CoDl_vertexFormat(u->hal, 0);
            EVE_CoDl_saveContext(u->hal);
            if(setup_gradient_bitmap(u, &dsc->grad, dsc->opa, w, h, false)) {
                EVE_CoDl_begin(u->hal, BITMAPS);
                EVE_CoDl_vertex2f_0(u->hal, x1, y1);
                EVE_CoDl_end(u->hal);
                EVE_CoDl_restoreContext(u->hal);
                return;
            }
            EVE_CoDl_restoreContext(u->hal);
        }
    }

    /* Gradient with per-stop opacity and rounded corners:
     * Use stencil approximation or weighted average opacity. */
    if(dsc->grad.dir != LV_GRAD_DIR_NONE && dsc->grad.stops_count >= 2 && real_radius > 0) {
        bool has_varying_opa = false;
        for(uint8_t i = 0; i < dsc->grad.stops_count; i++) {
            if(dsc->grad.stops[i].opa < LV_OPA_MAX) { has_varying_opa = true; break; }
        }
        if(has_varying_opa) {
#if EVE5_ALPHA_STENCIL_APPROX
            lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);
            EVE_CoDl_vertexFormat(u->hal, 0);
            EVE_CoDl_saveContext(u->hal);

            lv_draw_eve5_clear_stencil(u, x1, y1, x2, y2,
                                        &t->clip_area, &layer->buf_area);

#if EVE5_ALPHA_STENCIL_MULTISTEP
            /* Multi-step stencil AA: 4 concentric boundaries creating discrete
             * coverage zones. Draw gradient once per zone with colorA modulation.
             * Coverage values derived from hardware AA coverage table (s_CovTab). */
            EVE_CoDl_colorMask(u->hal, 0, 0, 0, 0);
            EVE_CoDl_stencilOp(u->hal, KEEP, INCR);
            if(w == h && radius == LV_RADIUS_CIRCLE && w >= 4) {
                int32_t cx2 = x1 * 2 + (w - 1);
                int32_t cy2 = y1 * 2 + (h - 1);
                int32_t ps[4] = { w * 8 - 4, w * 8 - 8, w * 8 - 12, w * 8 - 16 };
                for(int i = 0; i < 4; i++) {
                    if(ps[i] > 0) {
                        EVE_CoDl_pointSize(u->hal, ps[i]);
                        EVE_CoDl_begin(u->hal, POINTS);
                        EVE_CoDl_vertex2f_1(u->hal, cx2, cy2);
                        EVE_CoDl_end(u->hal);
                    }
                }
            }
            else {
                int32_t lw[4] = { real_radius * 16 + 4, real_radius * 16,
                                   real_radius * 16 - 4, real_radius * 16 - 8 };
                for(int i = 0; i < 4; i++) {
                    if(lw[i] > 0) {
                        EVE_CoDl_lineWidth(u->hal, lw[i]);
                        EVE_CoDl_begin(u->hal, RECTS);
                        EVE_CoDl_vertex2f_0(u->hal, x1 + real_radius, y1 + real_radius);
                        EVE_CoDl_vertex2f_0(u->hal, x2 - real_radius, y2 - real_radius);
                        EVE_CoDl_end(u->hal);
                    }
                }
            }

            /* Draw gradient through stencil at 4 coverage levels */
            EVE_CoDl_colorMask(u->hal, 0, 0, 0, 1);
            EVE_CoDl_stencilOp(u->hal, KEEP, KEEP);
            if(setup_gradient_bitmap(u, &dsc->grad, dsc->opa, w, h, false)) {
                /* Interior: stencil >= 4, full gradient alpha */
                EVE_CoDl_colorA(u->hal, 255);
                EVE_CoDl_stencilFunc(u->hal, GEQUAL, 4, 255);
                EVE_CoDl_begin(u->hal, BITMAPS);
                EVE_CoDl_vertex2f_0(u->hal, x1, y1);
                EVE_CoDl_end(u->hal);

                /* Inner fringe: stencil == 3, coverage ~120/255 */
                EVE_CoDl_colorA(u->hal, 120);
                EVE_CoDl_stencilFunc(u->hal, EQUAL, 3, 255);
                EVE_CoDl_begin(u->hal, BITMAPS);
                EVE_CoDl_vertex2f_0(u->hal, x1, y1);
                EVE_CoDl_end(u->hal);

                /* Middle fringe: stencil == 2, coverage ~65/255 */
                EVE_CoDl_colorA(u->hal, 65);
                EVE_CoDl_stencilFunc(u->hal, EQUAL, 2, 255);
                EVE_CoDl_begin(u->hal, BITMAPS);
                EVE_CoDl_vertex2f_0(u->hal, x1, y1);
                EVE_CoDl_end(u->hal);

                /* Outer fringe: stencil == 1, coverage ~26/255 */
                EVE_CoDl_colorA(u->hal, 26);
                EVE_CoDl_stencilFunc(u->hal, EQUAL, 1, 255);
                EVE_CoDl_begin(u->hal, BITMAPS);
                EVE_CoDl_vertex2f_0(u->hal, x1, y1);
                EVE_CoDl_end(u->hal);
            }
            else {
                /* Gradient allocation failed; uniform opa through stencil */
                EVE_CoDl_stencilFunc(u->hal, NOTEQUAL, 0, 255);
                EVE_CoDl_colorA(u->hal, opa);
                EVE_CoDl_lineWidth(u->hal, 16);
                EVE_CoDl_begin(u->hal, RECTS);
                EVE_CoDl_vertex2f_0(u->hal, x1, y1);
                EVE_CoDl_vertex2f_0(u->hal, x2, y2);
                EVE_CoDl_end(u->hal);
            }
#else
            /* Single-step stencil: binary threshold at AA midpoint */
            EVE_CoDl_colorMask(u->hal, 0, 0, 0, 0);
            EVE_CoDl_stencilOp(u->hal, KEEP, INCR);
            if(w == h && radius == LV_RADIUS_CIRCLE && w >= 4) {
                EVE_CoDl_pointSize(u->hal, w * 8 - 8);
                EVE_CoDl_begin(u->hal, POINTS);
                EVE_CoDl_vertex2f_1(u->hal, x1 * 2 + (w - 1), y1 * 2 + (h - 1));
                EVE_CoDl_end(u->hal);
            }
            else {
                EVE_CoDl_lineWidth(u->hal, real_radius * 16);
                EVE_CoDl_begin(u->hal, RECTS);
                EVE_CoDl_vertex2f_0(u->hal, x1 + real_radius, y1 + real_radius);
                EVE_CoDl_vertex2f_0(u->hal, x2 - real_radius, y2 - real_radius);
                EVE_CoDl_end(u->hal);
            }

            EVE_CoDl_colorMask(u->hal, 0, 0, 0, 1);
            EVE_CoDl_stencilFunc(u->hal, NOTEQUAL, 0, 255);
            EVE_CoDl_stencilOp(u->hal, KEEP, KEEP);
            if(setup_gradient_bitmap(u, &dsc->grad, dsc->opa, w, h, false)) {
                EVE_CoDl_begin(u->hal, BITMAPS);
                EVE_CoDl_vertex2f_0(u->hal, x1, y1);
                EVE_CoDl_end(u->hal);
            }
            else {
                EVE_CoDl_colorA(u->hal, opa);
                EVE_CoDl_lineWidth(u->hal, 16);
                EVE_CoDl_begin(u->hal, RECTS);
                EVE_CoDl_vertex2f_0(u->hal, x1, y1);
                EVE_CoDl_vertex2f_0(u->hal, x2, y2);
                EVE_CoDl_end(u->hal);
            }
#endif /* EVE5_ALPHA_STENCIL_MULTISTEP */

            EVE_CoDl_restoreContext(u->hal);
            return;
#else
            /* Compute weighted average stop opacity as approximation */
            {
                int32_t sum = 0;
                for(uint8_t i = 0; i + 1 < dsc->grad.stops_count; i++) {
                    int32_t span = dsc->grad.stops[i + 1].frac - dsc->grad.stops[i].frac;
                    sum += span * ((int32_t)dsc->grad.stops[i].opa + dsc->grad.stops[i + 1].opa);
                }
                int32_t total_span = dsc->grad.stops[dsc->grad.stops_count - 1].frac
                                   - dsc->grad.stops[0].frac;
                if(total_span > 0) {
                    opa = LV_OPA_MIX2(dsc->opa, (uint8_t)(sum / (2 * total_span)));
                }
            }
#endif
        }
    }

    /* Uniform opacity shape */
    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);
    EVE_CoDl_colorA(u->hal, opa);

    if(w == h && radius == LV_RADIUS_CIRCLE && w >= 4) {
        int32_t cx2 = x1 * 2 + (w - 1);
        int32_t cy2 = y1 * 2 + (h - 1);
        int32_t r16 = w * 8;
        draw_circle_subpx(u, cx2, cy2, r16);
    }
    else {
        lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, real_radius,
                                &t->clip_area, &layer->buf_area);
    }
}

void lv_draw_eve5_alpha_draw_fill_with_border(lv_draw_eve5_unit_t *u,
                                               const lv_draw_task_t *fill_task,
                                               const lv_draw_task_t *border_task)
{
    lv_layer_t *layer = fill_task->target_layer;
    const lv_draw_border_dsc_t *border_dsc = border_task->draw_dsc;

    /* Single shape at alpha=255 covering the border's outer area */
    int32_t x1 = border_task->area.x1 - layer->buf_area.x1;
    int32_t y1 = border_task->area.y1 - layer->buf_area.y1;
    int32_t x2 = border_task->area.x2 - layer->buf_area.x1;
    int32_t y2 = border_task->area.y2 - layer->buf_area.y1;
    int32_t w = x2 - x1 + 1;
    int32_t h = y2 - y1 + 1;

    lv_draw_eve5_set_scissor(u, &border_task->clip_area, &layer->buf_area);
    EVE_CoDl_colorA(u->hal, 255);

    if(w == h && border_dsc->radius == LV_RADIUS_CIRCLE && w >= 4) {
        draw_circle_subpx(u, x1 * 2 + (w - 1), y1 * 2 + (h - 1), w * 8);
    }
    else {
        int32_t r = LV_MIN3((w - 1) / 2, (h - 1) / 2, (int32_t)border_dsc->radius);
        lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, r,
                                &border_task->clip_area, &layer->buf_area);
    }
}

void lv_draw_eve5_alpha_draw_border(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    lv_layer_t *layer = t->target_layer;
    const lv_draw_border_dsc_t *dsc = t->draw_dsc;

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
    int32_t max_r = (LV_MIN(w, h) - 1) / 2;
    if(rout > max_r) rout = max_r;

    const lv_area_t *clip = &t->clip_area;
    const lv_area_t *layer_area = &layer->buf_area;

    int32_t clip_x1 = clip->x1 - layer_area->x1;
    int32_t clip_y1 = clip->y1 - layer_area->y1;
    int32_t clip_x2 = clip->x2 - layer_area->x1;
    int32_t clip_y2 = clip->y2 - layer_area->y1;

    bool corners_clipped = (rout > 0) &&
                           (clip_x1 > x1 + rout || clip_x2 < x1 + rout ||
                            clip_y1 > y1 + rout) &&
                           (clip_x1 > x2 - rout || clip_x2 < x2 - rout ||
                            clip_y1 > y1 + rout) &&
                           (clip_x1 > x1 + rout || clip_x2 < x1 + rout ||
                            clip_y2 < y2 - rout) &&
                           (clip_x1 > x2 - rout || clip_x2 < x2 - rout ||
                            clip_y2 < y2 - rout);

    lv_draw_eve5_set_scissor(u, clip, layer_area);

    if(corners_clipped || (rout == 0 && dsc->opa >= LV_OPA_MAX)) {
        /* LINES path: redraw the border lines */
        EVE_CoDl_colorA(u->hal, dsc->opa);
        EVE_CoDl_lineWidth(u->hal, dsc->width * 8);

        int32_t off = (dsc->width & 1) ? 0 : -1;
        LV_UNUSED(off);

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
    }
    else {
#if EVE5_ALPHA_STENCIL_APPROX
        /* Stencil-based interior mask: mark inner area, draw outer excluding it */
        int32_t inner_x1 = x1 + ((dsc->side & LV_BORDER_SIDE_LEFT)   ? dsc->width : -dsc->width);
        int32_t inner_x2 = x2 - ((dsc->side & LV_BORDER_SIDE_RIGHT)  ? dsc->width : -dsc->width);
        int32_t inner_y1 = y1 + ((dsc->side & LV_BORDER_SIDE_TOP)    ? dsc->width : -dsc->width);
        int32_t inner_y2 = y2 - ((dsc->side & LV_BORDER_SIDE_BOTTOM) ? dsc->width : -dsc->width);
        int32_t rin = rout - dsc->width;
        if(rin < 0) rin = 0;

        bool is_circle = (w == h && dsc->radius == LV_RADIUS_CIRCLE &&
                          dsc->side == LV_BORDER_SIDE_FULL && w >= 4);
        int32_t bcx2 = 0, bcy2 = 0;
        if(is_circle) {
            bcx2 = x1 * 2 + (w - 1);
            bcy2 = y1 * 2 + (h - 1);
        }

        EVE_CoDl_vertexFormat(u->hal, 0);
        EVE_CoDl_saveContext(u->hal);

        lv_draw_eve5_clear_stencil(u, x1, y1, x2, y2,
                                    &t->clip_area, &layer->buf_area);

#if EVE5_ALPHA_STENCIL_MULTISTEP
        /* Multi-step inner edge: 4 concentric INCR passes */
        EVE_CoDl_colorMask(u->hal, 0, 0, 0, 0);
        EVE_CoDl_stencilOp(u->hal, KEEP, INCR);
        if(is_circle) {
            int32_t inner_d = w - 2 * dsc->width;
            int32_t ps[4] = { inner_d * 8 - 4, inner_d * 8 - 8,
                               inner_d * 8 - 12, inner_d * 8 - 16 };
            for(int i = 0; i < 4; i++) {
                if(ps[i] > 0)
                    draw_circle_subpx(u, bcx2, bcy2, ps[i]);
            }
        }
        else if(rin > 0) {
            int32_t lw[4] = { rin * 16 + 4, rin * 16,
                               rin * 16 - 4, rin * 16 - 8 };
            for(int i = 0; i < 4; i++) {
                if(lw[i] > 0) {
                    EVE_CoDl_lineWidth(u->hal, lw[i]);
                    EVE_CoDl_begin(u->hal, RECTS);
                    EVE_CoDl_vertex2f_0(u->hal, inner_x1 + rin, inner_y1 + rin);
                    EVE_CoDl_vertex2f_0(u->hal, inner_x2 - rin, inner_y2 - rin);
                    EVE_CoDl_end(u->hal);
                }
            }
        }
        else {
            /* rin == 0: sharp inner edge, single INCR */
            EVE_CoDl_begin(u->hal, RECTS);
            EVE_CoDl_vertex2f_0(u->hal, inner_x1, inner_y1);
            EVE_CoDl_vertex2f_0(u->hal, inner_x2, inner_y2);
            EVE_CoDl_end(u->hal);
        }

        /* Draw outer shape at inverted coverage levels.
         * Border remaining = 255 - inner coverage. */
        EVE_CoDl_colorMask(u->hal, 0, 0, 0, 1);
        EVE_CoDl_stencilOp(u->hal, KEEP, KEEP);

        /* Stencil 0: full border */
        EVE_CoDl_colorA(u->hal, dsc->opa);
        EVE_CoDl_stencilFunc(u->hal, EQUAL, 0, 255);
        if(is_circle)
            draw_circle_subpx(u, bcx2, bcy2, w * 8);
        else
            lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, rout, clip, layer_area);

        if(is_circle || rin > 0) {
            /* Stencil 1: coverage ~229/255 */
            EVE_CoDl_colorA(u->hal, (uint8_t)((uint16_t)dsc->opa * 229 / 255));
            EVE_CoDl_stencilFunc(u->hal, EQUAL, 1, 255);
            if(is_circle) draw_circle_subpx(u, bcx2, bcy2, w * 8);
            else lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, rout, clip, layer_area);

            /* Stencil 2: coverage ~190/255 */
            EVE_CoDl_colorA(u->hal, (uint8_t)((uint16_t)dsc->opa * 190 / 255));
            EVE_CoDl_stencilFunc(u->hal, EQUAL, 2, 255);
            if(is_circle) draw_circle_subpx(u, bcx2, bcy2, w * 8);
            else lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, rout, clip, layer_area);

            /* Stencil 3: coverage ~135/255 */
            EVE_CoDl_colorA(u->hal, (uint8_t)((uint16_t)dsc->opa * 135 / 255));
            EVE_CoDl_stencilFunc(u->hal, EQUAL, 3, 255);
            if(is_circle) draw_circle_subpx(u, bcx2, bcy2, w * 8);
            else lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, rout, clip, layer_area);
        }
#else
        /* Single-step stencil: binary inner edge at AA midpoint */
        EVE_CoDl_colorMask(u->hal, 0, 0, 0, 0);
        EVE_CoDl_stencilOp(u->hal, KEEP, INCR);
        if(is_circle) {
            int32_t inner_d = w - 2 * dsc->width;
            int32_t stencil_r16 = inner_d * 8 - 8;
            if(stencil_r16 > 0)
                draw_circle_subpx(u, bcx2, bcy2, stencil_r16);
        }
        else if(rin > 0) {
            EVE_CoDl_lineWidth(u->hal, rin * 16);
            EVE_CoDl_begin(u->hal, RECTS);
            EVE_CoDl_vertex2f_0(u->hal, inner_x1 + rin, inner_y1 + rin);
            EVE_CoDl_vertex2f_0(u->hal, inner_x2 - rin, inner_y2 - rin);
            EVE_CoDl_end(u->hal);
        }
        else {
            EVE_CoDl_begin(u->hal, RECTS);
            EVE_CoDl_vertex2f_0(u->hal, inner_x1, inner_y1);
            EVE_CoDl_vertex2f_0(u->hal, inner_x2, inner_y2);
            EVE_CoDl_end(u->hal);
        }

        EVE_CoDl_colorMask(u->hal, 0, 0, 0, 1);
        EVE_CoDl_stencilFunc(u->hal, EQUAL, 0, 255);
        EVE_CoDl_stencilOp(u->hal, KEEP, KEEP);
        EVE_CoDl_colorA(u->hal, dsc->opa);
        if(is_circle)
            draw_circle_subpx(u, bcx2, bcy2, w * 8);
        else
            lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, rout, clip, layer_area);
#endif /* EVE5_ALPHA_STENCIL_MULTISTEP */

        EVE_CoDl_restoreContext(u->hal);
#else
        /* Without stencil: skip border alpha draw to avoid over-covering interior */
#endif
    }
}

/* Alpha pass for LINE is in lv_draw_eve5_line.c */

void lv_draw_eve5_alpha_draw_triangle(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    lv_layer_t *layer = t->target_layer;
    const lv_draw_triangle_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;

    lv_area_t tri_area;
    tri_area.x1 = (int32_t)LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y1 = (int32_t)LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    tri_area.x2 = (int32_t)LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y2 = (int32_t)LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    lv_area_t draw_area;
    if(!lv_area_intersect(&draw_area, &tri_area, &t->clip_area)) return;

    lv_point_t p[3];
    p[0] = lv_point_from_precise(&dsc->p[0]);
    p[1] = lv_point_from_precise(&dsc->p[1]);
    p[2] = lv_point_from_precise(&dsc->p[2]);

    for(int i = 0; i < 3; i++) {
        p[i].x -= layer->buf_area.x1;
        p[i].y -= layer->buf_area.y1;
    }

    int32_t xmin = LV_MIN3(p[0].x, p[1].x, p[2].x);
    int32_t ymin = LV_MIN3(p[0].y, p[1].y, p[2].y);
    int32_t xmax = LV_MAX3(p[0].x, p[1].x, p[2].x);
    int32_t ymax = LV_MAX3(p[0].y, p[1].y, p[2].y);

    int32_t w = xmax - xmin + 1;
    int32_t h = ymax - ymin + 1;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    /* Gradient with per-stop opacity: draw through triangle stencil */
    if(dsc->grad.dir != LV_GRAD_DIR_NONE && dsc->grad.stops_count >= 2) {
        bool has_varying_opa = false;
        for(uint8_t i = 0; i < dsc->grad.stops_count; i++) {
            if(dsc->grad.stops[i].opa < LV_OPA_MAX) { has_varying_opa = true; break; }
        }
        if(has_varying_opa) {
            EVE_CoDl_vertexFormat(u->hal, 0);
            EVE_CoDl_saveContext(u->hal);

            lv_draw_eve5_clear_stencil(u, xmin, ymin, xmax, ymax,
                                        &t->clip_area, &layer->buf_area);

            EVE_CoDl_colorMask(u->hal, 0, 0, 0, 0);
            EVE_CoDl_stencilOp(u->hal, KEEP, INVERT);
            EVE_CoDl_stencilFunc(u->hal, ALWAYS, 255, 255);

            build_triangle_stencil(u->hal, p);

            EVE_CoDl_colorMask(u->hal, 0, 0, 0, 1);
            EVE_CoDl_stencilFunc(u->hal, EQUAL, 255, 255);
            EVE_CoDl_stencilOp(u->hal, KEEP, KEEP);

            if(setup_gradient_bitmap(u, &dsc->grad, dsc->opa, w, h, false)) {
                EVE_CoDl_begin(u->hal, BITMAPS);
                EVE_CoDl_vertex2f_0(u->hal, xmin, ymin);
                EVE_CoDl_end(u->hal);
            }

            EVE_CoDl_restoreContext(u->hal);
            return;
        }
    }

    /* Uniform opacity: flat alpha through triangle stencil */
    EVE_CoDl_saveContext(u->hal);

    lv_draw_eve5_clear_stencil(u, xmin, ymin, xmax, ymax,
                                &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorMask(u->hal, 0, 0, 0, 0);
    EVE_CoDl_stencilOp(u->hal, KEEP, INVERT);
    EVE_CoDl_stencilFunc(u->hal, ALWAYS, 255, 255);

    build_triangle_stencil(u->hal, p);

    EVE_CoDl_colorMask(u->hal, 0, 0, 0, 1);
    EVE_CoDl_stencilFunc(u->hal, EQUAL, 255, 255);
    EVE_CoDl_lineWidth(u->hal, 16);
    EVE_CoDl_colorA(u->hal, dsc->opa);

    EVE_CoDl_begin(u->hal, RECTS);
    EVE_CoDl_vertex2f_0(u->hal, xmin, ymin);
    EVE_CoDl_vertex2f_0(u->hal, xmax, ymax);
    EVE_CoDl_end(u->hal);

    EVE_CoDl_restoreContext(u->hal);
}


#endif /* LV_USE_DRAW_EVE5 */
