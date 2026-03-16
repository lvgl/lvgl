/**
 * @file lv_draw_eve5_alpha_pass.c
 *
 * Alpha Correction Second Pass for EVE5 (BT820) Draw Unit
 *
 * EVE hardware applies blend factors uniformly to all 4 RGBA channels.
 * With blend(SRC_ALPHA, ONE_MINUS_SRC_ALPHA) on a cleared layer (alpha=0),
 * the alpha channel gets squared: result.a = src.a * src.a / 255.
 * Anti-aliased edges retain this squared alpha, causing incorrect
 * compositing when the layer is blitted onto its parent.
 *
 * This pass corrects the alpha channel after all RGB-producing tasks have
 * finished. It clears the layer alpha to 0, then re-iterates the task list
 * calling alpha-only draw functions with blend(ONE, ONE_MINUS_SRC_ALPHA)
 * and colorMask(0,0,0,1). This produces correct alpha coverage:
 *   result.a = src.a + dst.a * (1 - src.a/255)
 * which is the standard Porter-Duff "over" operator for alpha.
 *
 * An opaque-area skip optimization avoids redundant alpha draws for tasks
 * that are fully inside the largest opaque fill's interior.
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
 * See lv_draw_eve5_private.h for the full contract these functions satisfy.
 * In short: each predicate returns true when (1) the task requires alpha
 * recovery, AND (2) the direct-to-alpha pass cannot accurately reconstruct
 * it — requiring the L8 render-target alpha path for exact results.
 **********************/

/**
 * FILL: returns true when rounded-corner + HOR/VER gradient with semi-transparent
 * stop(s).  This is the only FILL path that uses alpha-as-scratch masking
 * (clear alpha → stamp rounded rect → multiply gradient bitmap alpha → draw
 * through blend(DST_ALPHA, ONE_MINUS_DST_ALPHA)).  Solid fills and fully-opaque
 * gradients use simpler paths that don't trash alpha.
 *
 * Mirrors: RGB pass in lv_draw_eve5_hal_draw_fill (gradient + real_radius > 0
 *          + varying stop opa → mask_rect path that sets has_alpha_trashed).
 *          Direct alpha pass in lv_draw_eve5_alpha_draw_fill (stencil
 *          approximation only — no exact recovery exists).
 */
bool lv_draw_eve5_fill_needs_alpha_rendertarget(const lv_draw_task_t *t)
{
    const lv_draw_fill_dsc_t *dsc = t->draw_dsc;
    if(dsc->opa <= LV_OPA_MIN) return false;

    int32_t w = lv_area_get_width(&t->area);
    int32_t h = lv_area_get_height(&t->area);
    int32_t real_radius = LV_MIN3((w - 1) / 2, (h - 1) / 2, dsc->radius);
    if(real_radius == 0) return false;

    /* Only HOR/VER gradients use the alpha-as-scratch masking path */
    if(dsc->grad.dir != LV_GRAD_DIR_HOR && dsc->grad.dir != LV_GRAD_DIR_VER) return false;
    if(dsc->grad.stops_count < 2) return false;

    for(uint8_t i = 0; i < dsc->grad.stops_count; i++) {
        if(dsc->grad.stops[i].opa < LV_OPA_MAX) return true;
    }
    return false;
}

/**
 * BORDER: returns true when the border uses the alpha-as-scratch masking path.
 * The RGB pass has two rendering paths:
 *   - LINES path (4 straight LINE_STRIP segments): used when rout==0 + full opa,
 *     or when all 4 corners are clipped away.  Does NOT trash alpha.
 *   - Masking path (outer rounded rect minus inner rounded rect via alpha channel):
 *     used otherwise.  Trashes alpha.
 * The direct alpha correction pass uses stencil for exactly the masking-path
 * cases, but can only approximate the result (binary in/out).
 *
 * Mirrors: RGB pass in lv_draw_eve5_hal_draw_border (LINES vs masking split).
 *          Direct alpha pass in lv_draw_eve5_alpha_draw_border (stencil
 *          approximation only — no exact recovery exists).
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

    /* LINES path: no alpha trashing, no stencil needed */
    if(rout == 0 && dsc->opa >= LV_OPA_MAX) return false;

    /* Check if all corners are clipped away — falls back to LINES path */
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
 * ARC: always returns true for visible arcs.
 * The RGB pass uses multi-phase stencil (EDGE_STRIP_R wedge mask, DECR for
 * annulus, REPLACE for round/flat caps) or CMD_ARC — both trash the alpha
 * channel.  The direct alpha correction pass uses stencil to approximate
 * recovery, and there is currently no means to reconstruct the exact alpha.
 *
 * Mirrors: RGB pass in lv_draw_eve5_hal_draw_arc (stencil + CMD_ARC paths,
 *          both set has_alpha_trashed).
 *          Direct alpha pass in lv_draw_eve5_alpha_draw_arc (stencil
 *          approximation only — no exact recovery exists).
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
 * ALPHA-ONLY FUNCTIONS
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
     * re-draw gradient ARGB8 strip for exact per-pixel alpha correction.
     * The strip's per-pixel alpha (LV_OPA_MIX2(opa, stop_opa)) is drawn
     * through the alpha pass blend, producing correct Porter-Duff coverage.
     * Rounded gradient fills are handled separately below. */
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
            /* Fall through to uniform opa shape */
        }
    }

    /* Gradient with per-stop opacity and rounded corners:
     * With stencil: build rounded rect stencil, draw gradient strip through it
     * for per-stop alpha with binary edges at the rounded corners.
     * Without stencil: compute weighted average stop opacity for the uniform shape. */
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
            /* Multi-step stencil AA: 4 concentric stencil boundaries spread
             * across the ~1px AA transition, creating 4 zones with discrete
             * coverage levels (1/4, 2/4, 3/4, full interior). Draws the
             * gradient bitmap once per zone with colorA modulation.
             * Stencil build uses INCR from outermost to innermost. */
            EVE_CoDl_colorMask(u->hal, 0, 0, 0, 0);
            EVE_CoDl_stencilOp(u->hal, KEEP, INCR);
            if(w == h && radius == LV_RADIUS_CIRCLE && w >= 4) {
                int32_t cx2 = x1 * 2 + (w - 1);
                int32_t cy2 = y1 * 2 + (h - 1);
                /* pointSize steps: -4, -8, -12, -16 relative to w*8 (RGB path).
                 * Each step is 0.25px smaller. The outermost INCR boundary
                 * lands ~0.25px inside the RGB path's outer AA extent — the
                 * very outermost fringe pixels have too little coverage for
                 * even the 1/4 discrete level, so truncating avoids haloing. */
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
                /* lineWidth steps: +4, 0, -4, -8 relative to real_radius*16.
                 * RGB path uses real_radius*16+8. Outermost step is 0.25px
                 * inside the RGB edge — matching the RGB size would over-
                 * estimate the barely-covered outermost pixels at 1/4 alpha,
                 * causing a visible dark halo. Truncating the outermost
                 * ~0.25px of the AA transition avoids this artifact. */
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

            /* Draw gradient through stencil at 4 coverage levels.
             * colorA values derived from the hardware AA coverage table
             * (s_CovTab): average coverage in each 4-sub-pixel zone,
             * measured at d_rgb from the RGB geometric edge.
             * Cross-radius average of rows 16/32/64:
             *   Zone 1 (d_rgb 7-10):  ~26  (cols 23-26)
             *   Zone 2 (d_rgb 3-6):   ~65  (cols 19-22)
             *   Zone 3 (d_rgb -1..2): ~120 (cols 15-18)
             *   Zone 4 (d_rgb <= -2):  255  (interior) */
            EVE_CoDl_colorMask(u->hal, 0, 0, 0, 1);
            EVE_CoDl_stencilOp(u->hal, KEEP, KEEP);
            if(setup_gradient_bitmap(u, &dsc->grad, dsc->opa, w, h, false)) {
                /* Interior: stencil >= 4, full gradient alpha */
                EVE_CoDl_colorA(u->hal, 255);
                EVE_CoDl_stencilFunc(u->hal, GEQUAL, 4, 255);
                EVE_CoDl_begin(u->hal, BITMAPS);
                EVE_CoDl_vertex2f_0(u->hal, x1, y1);
                EVE_CoDl_end(u->hal);

                /* Inner fringe: stencil == 3, avg coverage ~120 */
                EVE_CoDl_colorA(u->hal, 120);
                EVE_CoDl_stencilFunc(u->hal, EQUAL, 3, 255);
                EVE_CoDl_begin(u->hal, BITMAPS);
                EVE_CoDl_vertex2f_0(u->hal, x1, y1);
                EVE_CoDl_end(u->hal);

                /* Middle fringe: stencil == 2, avg coverage ~65 */
                EVE_CoDl_colorA(u->hal, 65);
                EVE_CoDl_stencilFunc(u->hal, EQUAL, 2, 255);
                EVE_CoDl_begin(u->hal, BITMAPS);
                EVE_CoDl_vertex2f_0(u->hal, x1, y1);
                EVE_CoDl_end(u->hal);

                /* Outer fringe: stencil == 1, avg coverage ~26 */
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
            /* Single-step stencil: binary threshold at AA midpoint.
             * Uses real_radius * 16 (no +8) for 0.5px AA midpoint alignment.
             * colorMask(0,0,0,0) prevents stencil build from polluting alpha. */
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

            /* Draw gradient strip through stencil */
            EVE_CoDl_colorMask(u->hal, 0, 0, 0, 1);
            EVE_CoDl_stencilFunc(u->hal, NOTEQUAL, 0, 255);
            EVE_CoDl_stencilOp(u->hal, KEEP, KEEP);
            if(setup_gradient_bitmap(u, &dsc->grad, dsc->opa, w, h, false)) {
                EVE_CoDl_begin(u->hal, BITMAPS);
                EVE_CoDl_vertex2f_0(u->hal, x1, y1);
                EVE_CoDl_end(u->hal);
            }
            else {
                /* Gradient allocation failed; uniform opa through stencil */
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
            /* Compute weighted average stop opacity for better approximation.
             * Integrates the piecewise-linear opacity over the gradient span:
             * avg = sum(span_i * (opa_i + opa_{i+1})) / (2 * total_span) */
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
            /* Fall through to uniform opa shape */
#endif
        }
    }

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

    /* One shape at alpha=255 covering the border's outer area */
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

    /* Convert clip to layer coordinates */
    int32_t clip_x1 = clip->x1 - layer_area->x1;
    int32_t clip_y1 = clip->y1 - layer_area->y1;
    int32_t clip_x2 = clip->x2 - layer_area->x1;
    int32_t clip_y2 = clip->y2 - layer_area->y1;

    /* Check if corners are clipped (same logic as normal draw) */
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
        /* Stencil-based interior mask: marks the inner area in the stencil,
         * then draws the outer shape excluding it. This limits alpha
         * writes to the border ring only, preventing interior over-coverage
         * for semi-transparent borders. The outer edge preserves the stamp's
         * AA. The inner edge is binary. INCR saturates at 255, so areas
         * where the inner rect extends beyond the outer (disabled sides)
         * are harmless — phase 3 draws through stencil == 0 only. */
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
        /* Multi-step inner edge: 4 concentric INCR passes for the inner
         * shape (circle / rounded rect), creating a 4-zone transition at
         * the inner boundary. rin == 0 has no AA, so single INCR suffices.
         * colorMask(0,0,0,0) prevents stencil build from polluting alpha. */
        EVE_CoDl_colorMask(u->hal, 0, 0, 0, 0);
        EVE_CoDl_stencilOp(u->hal, KEEP, INCR);
        if(is_circle) {
            int32_t inner_d = w - 2 * dsc->width;
            /* pointSize steps: -4, -8, -12, -16 relative to inner_d*8 (RGB).
             * Same zone layout as fill multi-step. */
            int32_t ps[4] = { inner_d * 8 - 4, inner_d * 8 - 8,
                               inner_d * 8 - 12, inner_d * 8 - 16 };
            for(int i = 0; i < 4; i++) {
                if(ps[i] > 0)
                    draw_circle_subpx(u, bcx2, bcy2, ps[i]);
            }
        }
        else if(rin > 0) {
            /* lineWidth steps: +4, 0, -4, -8 relative to rin*16.
             * Same zone layout as fill multi-step. */
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
            /* rin == 0: sharp inner edge, no AA fringe. Single INCR. */
            EVE_CoDl_begin(u->hal, RECTS);
            EVE_CoDl_vertex2f_0(u->hal, inner_x1, inner_y1);
            EVE_CoDl_vertex2f_0(u->hal, inner_x2, inner_y2);
            EVE_CoDl_end(u->hal);
        }

        /* Draw outer shape at inverted coverage levels.
         * Stencil zones represent inner shape coverage from s_CovTab:
         * {26, 65, 120, 255}. Border remaining = 255 - inner = {229, 190, 135, 0}.
         * colorA = dsc->opa * remaining / 255 for each zone.
         * Stencil >= 4 (deep inside inner) is not drawn (fully excluded). */
        EVE_CoDl_colorMask(u->hal, 0, 0, 0, 1);
        EVE_CoDl_stencilOp(u->hal, KEEP, KEEP);

        /* Stencil 0: full border — no inner coverage */
        EVE_CoDl_colorA(u->hal, dsc->opa);
        EVE_CoDl_stencilFunc(u->hal, EQUAL, 0, 255);
        if(is_circle)
            draw_circle_subpx(u, bcx2, bcy2, w * 8);
        else
            lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, rout, clip, layer_area);

        if(is_circle || rin > 0) {
            /* Stencil 1: outer inner fringe, border coverage ~229/255 */
            EVE_CoDl_colorA(u->hal, (uint8_t)((uint16_t)dsc->opa * 229 / 255));
            EVE_CoDl_stencilFunc(u->hal, EQUAL, 1, 255);
            if(is_circle) draw_circle_subpx(u, bcx2, bcy2, w * 8);
            else lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, rout, clip, layer_area);

            /* Stencil 2: middle inner fringe, border coverage ~190/255 */
            EVE_CoDl_colorA(u->hal, (uint8_t)((uint16_t)dsc->opa * 190 / 255));
            EVE_CoDl_stencilFunc(u->hal, EQUAL, 2, 255);
            if(is_circle) draw_circle_subpx(u, bcx2, bcy2, w * 8);
            else lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, rout, clip, layer_area);

            /* Stencil 3: inner fringe, border coverage ~135/255 */
            EVE_CoDl_colorA(u->hal, (uint8_t)((uint16_t)dsc->opa * 135 / 255));
            EVE_CoDl_stencilFunc(u->hal, EQUAL, 3, 255);
            if(is_circle) draw_circle_subpx(u, bcx2, bcy2, w * 8);
            else lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, rout, clip, layer_area);
        }
#else
        /* Single-step stencil: binary inner edge at AA midpoint.
         * Uses rin * 16 / (inner_d * 8 - 8) — 0.5px smaller than the normal
         * draw's inner shape — so the stencil boundary aligns with the
         * geometric edge (midpoint of the AA transition) rather than the
         * outer AA fringe. colorMask(0,0,0,0) prevents stencil build from
         * polluting alpha. */
        EVE_CoDl_colorMask(u->hal, 0, 0, 0, 0);
        EVE_CoDl_stencilOp(u->hal, KEEP, INCR);
        if(is_circle) {
            int32_t inner_d = w - 2 * dsc->width;
            int32_t stencil_r16 = inner_d * 8 - 8;  /* 0.5px smaller for AA midpoint */
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

        /* Draw outer shape at border_opa, excluding interior.
         * The stamp's AA provides smooth outer edges; the inner edge is binary. */
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
        /* Without stencil, skip the border alpha draw entirely. Drawing the
         * full outer rounded rect would over-cover the interior, adding
         * unwanted alpha on top of the fill's correct values. Doing nothing
         * preserves the interior; the border ring's alpha contribution is
         * lost, but the opaque fill step (alpha=255) covers it in the
         * common case. */
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

    /* Gradient with per-stop opacity: re-draw gradient bitmap through
     * triangle stencil for exact per-pixel alpha correction. */
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

    /* Uniform opacity: draw flat alpha through triangle stencil. */
    EVE_CoDl_saveContext(u->hal);

    lv_draw_eve5_clear_stencil(u, xmin, ymin, xmax, ymax,
                                &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorMask(u->hal, 0, 0, 0, 0);
    EVE_CoDl_stencilOp(u->hal, KEEP, INVERT);
    EVE_CoDl_stencilFunc(u->hal, ALWAYS, 255, 255);

    build_triangle_stencil(u->hal, p);

    /* Draw where stencil was inverted. Restore colorMask to alpha-only
     * (the outer context already has colorMask(0,0,0,1)). */
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
