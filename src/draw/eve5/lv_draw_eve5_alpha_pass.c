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
            if(setup_gradient_bitmap(u, &dsc->grad, dsc->opa, w, h)) {
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
            if(setup_gradient_bitmap(u, &dsc->grad, dsc->opa, w, h)) {
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
            if(setup_gradient_bitmap(u, &dsc->grad, dsc->opa, w, h)) {
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

void lv_draw_eve5_alpha_draw_line(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    lv_layer_t *layer = t->target_layer;
    const lv_draw_line_dsc_t *dsc = t->draw_dsc;

    if(dsc->width == 0) return;
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->p1.x == dsc->p2.x && dsc->p1.y == dsc->p2.y) return;

    int32_t x1 = dsc->p1.x - layer->buf_area.x1;
    int32_t y1 = dsc->p1.y - layer->buf_area.y1;
    int32_t x2 = dsc->p2.x - layer->buf_area.x1;
    int32_t y2 = dsc->p2.y - layer->buf_area.y1;

    uint32_t line_w = dsc->width * 8;
    int32_t off = (dsc->width & 1) ? 0 : -1;

    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;

    /* H/V scissor optimization (same as normal draw) */
    bool is_hv_flat = (dx == 0 || dy == 0) && (dsc->raw_end || (!dsc->round_start && !dsc->round_end));
    lv_area_t hv_scissor;

    if(is_hv_flat) {
        int32_t wm = dsc->width - 1;
        int32_t w_half0 = wm >> 1;
        int32_t w_half1 = w_half0 + (wm & 1);
        int32_t rx1, ry1, rx2, ry2;
        if(dy == 0) {
            rx1 = LV_MIN(x1, x2);      rx2 = LV_MAX(x1, x2) - 1;
            ry1 = y1 - w_half1;         ry2 = y1 + w_half0;
        }
        else {
            rx1 = x1 - w_half1;         rx2 = x1 + w_half0;
            ry1 = LV_MIN(y1, y2);       ry2 = LV_MAX(y1, y2) - 1;
        }
        lv_area_t line_screen = { rx1 + layer->buf_area.x1, ry1 + layer->buf_area.y1,
                                   rx2 + layer->buf_area.x1, ry2 + layer->buf_area.y1 };
        if(!lv_area_intersect(&hv_scissor, &line_screen, &t->clip_area)) return;
        lv_draw_eve5_set_scissor(u, &hv_scissor, &layer->buf_area);

        line_w += 16;
        off = 0;
    }
    else {
        lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);
    }

#if EVE5_ALPHA_STENCIL_APPROX
    /* Stencil-based masking for diagonal flat caps and/or dashes.
     * Build the line body shape in stencil (draw line, erase caps/gaps),
     * then draw the round-cap line through the stencil at opa.
     * H/V flat caps are already handled by scissor optimization above. */
    {
        bool need_flat_start = !dsc->round_start && !dsc->raw_end && !is_hv_flat;
        bool need_flat_end = !dsc->round_end && !dsc->raw_end && !is_hv_flat;
        bool has_dashes = (dsc->dash_gap > 0 && dsc->dash_width > 0);

        if(need_flat_start || need_flat_end || has_dashes) {
            EVE_HalContext *phost = u->hal;

            /* Direction vectors for masking geometry (same as normal draw) */
#if LV_DRAW_EVE5_NO_FLOAT
            int32_t len = lv_sqrt32((uint32_t)((int64_t)dx * dx + (int64_t)dy * dy));
            if(len == 0) len = 1;
            int32_t perp_off_x_16 = (int32_t)(-(int64_t)dy * dsc->width * 16 / len);
            int32_t perp_off_y_16 = (int32_t)((int64_t)dx * dsc->width * 16 / len);
#else
            float off_px = -0.5f;
            float fx1f = x1 + off_px;
            float fy1f = y1 + off_px;
            float fx2f = x2 + off_px;
            float fy2f = y2 + off_px;
            float len = sqrtf((float)((int64_t)dx * dx + (int64_t)dy * dy));
            float inv_len = (len > 0.0f) ? 1.0f / len : 0.0f;
            float dir_x = dx * inv_len;
            float dir_y = dy * inv_len;
            float perp_x = -dir_y;
            float perp_y = dir_x;
            float perp_ext = (float)dsc->width;
#endif

            {
                int32_t margin = dsc->width;
                lv_draw_eve5_clear_stencil(u,
                    LV_MIN(x1, x2) - margin, LV_MIN(y1, y2) - margin,
                    LV_MAX(x1, x2) + margin, LV_MAX(y1, y2) + margin,
                    &t->clip_area, &layer->buf_area);
            }

            /* clear_stencil restores scissor to t->clip_area; re-apply the
             * tight H/V scissor so the stencil build and final draw clip
             * the round caps correctly. */
            if(is_hv_flat) {
                lv_draw_eve5_set_scissor(u, &hv_scissor, &layer->buf_area);
            }

            EVE_CoDl_saveContext(phost);

            /* Draw round-cap line into stencil (INCR).
             * colorMask(0,0,0,0) prevents stencil build from polluting alpha. */
            EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
            EVE_CoDl_stencilOp(phost, KEEP, INCR);
            EVE_CoDl_lineWidth(phost, line_w);
            EVE_CoDl_begin(phost, LINES);
            EVE_CoDl_vertex2f_1(phost, x1 * 2 + off, y1 * 2 + off);
            EVE_CoDl_vertex2f_1(phost, x2 * 2 + off, y2 * 2 + off);
            EVE_CoDl_end(phost);

            /* Phase 3: Erase flat caps and/or dash gaps from stencil (ZERO).
             * Uses the same perpendicular masking line geometry as the normal draw. */
            EVE_CoDl_stencilOp(phost, KEEP, ZERO);

            if(need_flat_start || need_flat_end) {
                /* Vertex positions use the same center offset as the RGB path.
                 * lineWidth is 0.5px smaller because the stencil ZERO's AA fringe
                 * extends ~0.5px beyond geometric, so shrinking aligns the
                 * effective stencil boundary with the RGB path's smooth edge. */
                uint32_t cap_hw_16 = LV_MAX(dsc->width * 4 + 24, 16);
                uint32_t cap_lw_16 = cap_hw_16 > 8 ? cap_hw_16 - 8 : cap_hw_16;

                EVE_CoDl_lineWidth(phost, cap_lw_16);
                EVE_CoDl_begin(phost, LINES);

#if LV_DRAW_EVE5_NO_FLOAT
                if(need_flat_start) {
                    int32_t cx_16 = x1 * 16 - 8 - (int32_t)((int64_t)dx * (int32_t)cap_hw_16 / len);
                    int32_t cy_16 = y1 * 16 - 8 - (int32_t)((int64_t)dy * (int32_t)cap_hw_16 / len);
                    EVE_CoDl_vertex2f_4(phost, cx_16 + perp_off_x_16, cy_16 + perp_off_y_16);
                    EVE_CoDl_vertex2f_4(phost, cx_16 - perp_off_x_16, cy_16 - perp_off_y_16);
                }
                if(need_flat_end) {
                    int32_t cx_16 = x2 * 16 - 8 + (int32_t)((int64_t)dx * (int32_t)cap_hw_16 / len);
                    int32_t cy_16 = y2 * 16 - 8 + (int32_t)((int64_t)dy * (int32_t)cap_hw_16 / len);
                    EVE_CoDl_vertex2f_4(phost, cx_16 + perp_off_x_16, cy_16 + perp_off_y_16);
                    EVE_CoDl_vertex2f_4(phost, cx_16 - perp_off_x_16, cy_16 - perp_off_y_16);
                }
#else
                float cap_hw_px = cap_hw_16 / 16.0f;

                if(need_flat_start) {
                    float cx = fx1f - dir_x * cap_hw_px;
                    float cy = fy1f - dir_y * cap_hw_px;
                    EVE_CoDl_vertex2f_4(phost,
                        (int32_t)((cx + perp_x * perp_ext) * 16),
                        (int32_t)((cy + perp_y * perp_ext) * 16));
                    EVE_CoDl_vertex2f_4(phost,
                        (int32_t)((cx - perp_x * perp_ext) * 16),
                        (int32_t)((cy - perp_y * perp_ext) * 16));
                }
                if(need_flat_end) {
                    float cx = fx2f + dir_x * cap_hw_px;
                    float cy = fy2f + dir_y * cap_hw_px;
                    EVE_CoDl_vertex2f_4(phost,
                        (int32_t)((cx + perp_x * perp_ext) * 16),
                        (int32_t)((cy + perp_y * perp_ext) * 16));
                    EVE_CoDl_vertex2f_4(phost,
                        (int32_t)((cx - perp_x * perp_ext) * 16),
                        (int32_t)((cy - perp_y * perp_ext) * 16));
                }
#endif

                EVE_CoDl_end(phost);
            }

            if(has_dashes) {
                /* 0.5px smaller than the RGB path because the stencil ZERO
                 * extends ~0.5px beyond geometric due to AA fringe. */
                uint32_t gap_hw_16 = dsc->dash_gap > 1 ? dsc->dash_gap * 8 - 8 : dsc->dash_gap * 8;
                int32_t period_sw = dsc->dash_gap + dsc->dash_width;

#if LV_DRAW_EVE5_NO_FLOAT
                int32_t abs_proj = (int32_t)(((int64_t)LV_MIN(dsc->p1.x, dsc->p2.x) * LV_ABS(dx)
                                  + (int64_t)LV_MIN(dsc->p1.y, dsc->p2.y) * LV_ABS(dy)) / len);
                int32_t phase = abs_proj % period_sw;
                if(phase < 0) phase += period_sw;

                int32_t period_x2 = 2 * (dsc->dash_width + dsc->dash_gap);
                int32_t gap_center_x2 = 2 * dsc->dash_width + 1 + dsc->dash_gap - 2 * phase;
                while(gap_center_x2 < -(int32_t)dsc->dash_gap) gap_center_x2 += period_x2;

                EVE_CoDl_lineWidth(phost, gap_hw_16);
                EVE_CoDl_begin(phost, LINES);

                while(gap_center_x2 < 2 * len + (int32_t)dsc->dash_gap) {
                    int32_t cx_16 = x1 * 16 + (int32_t)((int64_t)dx * gap_center_x2 * 8 / len);
                    int32_t cy_16 = y1 * 16 + (int32_t)((int64_t)dy * gap_center_x2 * 8 / len);
                    EVE_CoDl_vertex2f_4(phost, cx_16 + perp_off_x_16, cy_16 + perp_off_y_16);
                    EVE_CoDl_vertex2f_4(phost, cx_16 - perp_off_x_16, cy_16 - perp_off_y_16);
                    gap_center_x2 += period_x2;
                }
#else
                float period = (float)(dsc->dash_width + dsc->dash_gap);

                float abs_proj = (float)LV_MIN(dsc->p1.x, dsc->p2.x) * fabsf(dir_x)
                               + (float)LV_MIN(dsc->p1.y, dsc->p2.y) * fabsf(dir_y);
                int32_t phase = ((int32_t)floorf(abs_proj)) % period_sw;
                if(phase < 0) phase += period_sw;

                float gap_center_pat = (float)(dsc->dash_width) + 0.5f + (float)(dsc->dash_gap) * 0.5f;

                EVE_CoDl_lineWidth(phost, gap_hw_16);
                EVE_CoDl_begin(phost, LINES);

                float gap_center = gap_center_pat - (float)phase;
                while(gap_center < -(float)(dsc->dash_gap) * 0.5f) gap_center += period;
                float gap_hw_px = (float)(dsc->dash_gap) * 0.5f;
                while(gap_center < len + gap_hw_px) {
                    float cx = (float)x1 + dir_x * gap_center;
                    float cy = (float)y1 + dir_y * gap_center;
                    EVE_CoDl_vertex2f_4(phost,
                        (int32_t)((cx + perp_x * perp_ext) * 16),
                        (int32_t)((cy + perp_y * perp_ext) * 16));
                    EVE_CoDl_vertex2f_4(phost,
                        (int32_t)((cx - perp_x * perp_ext) * 16),
                        (int32_t)((cy - perp_y * perp_ext) * 16));
                    gap_center += period;
                }
#endif

                EVE_CoDl_end(phost);
            }

            /* Phase 4: Draw round-cap line through stencil at opa (alpha-only) */
            EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
            EVE_CoDl_stencilFunc(phost, NOTEQUAL, 0, 255);
            EVE_CoDl_stencilOp(phost, KEEP, KEEP);
            EVE_CoDl_colorA(phost, dsc->opa);
            EVE_CoDl_lineWidth(phost, line_w);
            EVE_CoDl_begin(phost, LINES);
            EVE_CoDl_vertex2f_1(phost, x1 * 2 + off, y1 * 2 + off);
            EVE_CoDl_vertex2f_1(phost, x2 * 2 + off, y2 * 2 + off);
            EVE_CoDl_end(phost);

            EVE_CoDl_restoreContext(phost);

            if(is_hv_flat) {
                lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);
            }
            return;
        }
    }
#endif

    /* Draw the line with round caps at opa */
    EVE_CoDl_colorA(u->hal, dsc->opa);
    EVE_CoDl_lineWidth(u->hal, line_w);
    EVE_CoDl_begin(u->hal, LINES);
    EVE_CoDl_vertex2f_1(u->hal, x1 * 2 + off, y1 * 2 + off);
    EVE_CoDl_vertex2f_1(u->hal, x2 * 2 + off, y2 * 2 + off);
    EVE_CoDl_end(u->hal);

    /* Restore scissor if H/V optimization was used */
    if(is_hv_flat) {
        lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);
    }
}

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

            if(setup_gradient_bitmap(u, &dsc->grad, dsc->opa, w, h)) {
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

void lv_draw_eve5_alpha_draw_box_shadow(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t *layer = t->target_layer;
    const lv_draw_box_shadow_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width <= 0) return;

    /* Shadow is drawn as a 9-slice of L8 textures. For the alpha pass,
     * draw a solid rect covering the shadow bounding box at opa.
     * The shadow's Gaussian falloff already produced correct RGB;
     * the alpha just needs to show the shape's coverage. */
    const lv_area_t *coords = &t->area;
    lv_area_t core_area;
    core_area.x1 = coords->x1 + dsc->ofs_x - dsc->spread;
    core_area.x2 = coords->x2 + dsc->ofs_x + dsc->spread;
    core_area.y1 = coords->y1 + dsc->ofs_y - dsc->spread;
    core_area.y2 = coords->y2 + dsc->ofs_y + dsc->spread;

    int32_t r_sh = dsc->radius;
    int32_t short_side = LV_MIN(lv_area_get_width(&core_area), lv_area_get_height(&core_area));
    if(r_sh > short_side / 2) r_sh = short_side / 2;

    int32_t blur_radius = (dsc->width + 1) / 2;
    int32_t corner_size = blur_radius + r_sh;
    if(corner_size <= 0) return;

    int32_t ratio_idx = (r_sh * (EVE5_SHADOW_TEX_SIZE - 1)) / corner_size;
    if(ratio_idx < 0) ratio_idx = 0;
    if(ratio_idx >= EVE5_SHADOW_TEX_SIZE) ratio_idx = EVE5_SHADOW_TEX_SIZE - 1;

    lv_draw_eve5_shadow_slot_t *slot = &u->shadow_slots[ratio_idx];
    uint32_t corner_addr = Esd_GpuAlloc_Get(u->allocator, slot->corner_handle);
    uint32_t edge_addr = Esd_GpuAlloc_Get(u->allocator, slot->edge_handle);

    if(corner_addr == GA_INVALID || edge_addr == GA_INVALID) return;

    /* Shadow bounding box in layer-local coordinates */
    int32_t lx = layer->buf_area.x1;
    int32_t ly = layer->buf_area.y1;
    int32_t sx1 = core_area.x1 - blur_radius - lx;
    int32_t sy1 = core_area.y1 - blur_radius - ly;
    int32_t sx2 = core_area.x2 + blur_radius - lx;
    int32_t sy2 = core_area.y2 + blur_radius - ly;

    int32_t shadow_w = sx2 - sx1 + 1;
    int32_t shadow_h = sy2 - sy1 + 1;

    int32_t render_corner_w = LV_MIN(corner_size, shadow_w / 2);
    int32_t render_corner_h = LV_MIN(corner_size, shadow_h / 2);

    int32_t scale = (EVE5_SHADOW_TEX_SIZE * 256) / corner_size;
    int32_t tex_max = (EVE5_SHADOW_TEX_SIZE - 1) * 256;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorA(phost, dsc->opa);

    /* Re-draw the 9-slice shadow textures (same layout as normal draw) */
    EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);
    EVE_CoDl_bitmapSource(phost, corner_addr);
    EVE_CoDl_bitmapLayout(phost, L8, EVE5_SHADOW_TEX_SIZE, EVE5_SHADOW_TEX_SIZE);
    EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, render_corner_w, render_corner_h);

    EVE_CoDl_begin(phost, BITMAPS);

    /* Top-left corner */
    EVE_CoDl_bitmapTransformA(phost, scale);
    EVE_CoDl_bitmapTransformB(phost, 0);
    EVE_CoDl_bitmapTransformC(phost, 0);
    EVE_CoDl_bitmapTransformD(phost, 0);
    EVE_CoDl_bitmapTransformE(phost, scale);
    EVE_CoDl_bitmapTransformF(phost, 0);
    EVE_CoDl_vertex2f_0(phost, sx1, sy1);

    /* Top-right corner */
    EVE_CoDl_bitmapTransformA(phost, -scale);
    EVE_CoDl_bitmapTransformC(phost, tex_max);
    EVE_CoDl_vertex2f_0(phost, sx2 + 1 - render_corner_w, sy1);

    /* Bottom-left corner */
    EVE_CoDl_bitmapTransformA(phost, scale);
    EVE_CoDl_bitmapTransformC(phost, 0);
    EVE_CoDl_bitmapTransformE(phost, -scale);
    EVE_CoDl_bitmapTransformF(phost, tex_max);
    EVE_CoDl_vertex2f_0(phost, sx1, sy2 + 1 - render_corner_h);

    /* Bottom-right corner */
    EVE_CoDl_bitmapTransformA(phost, -scale);
    EVE_CoDl_bitmapTransformC(phost, tex_max);
    EVE_CoDl_vertex2f_0(phost, sx2 + 1 - render_corner_w, sy2 + 1 - render_corner_h);

    EVE_CoDl_end(phost);

    /* Edges */
    int32_t edge_h_len = shadow_w - 2 * render_corner_w;
    int32_t edge_v_len = shadow_h - 2 * render_corner_h;
    int32_t edge_scale = (EVE5_SHADOW_TEX_SIZE * 256) / corner_size;
    int32_t edge_tex_max = (EVE5_SHADOW_TEX_SIZE - 1) * 256;

    EVE_CoDl_bitmapSource(phost, edge_addr);
    EVE_CoDl_bitmapLayout(phost, L8, EVE5_SHADOW_TEX_SIZE, 1);

    if(edge_h_len > 0) {
        EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, edge_h_len, render_corner_h);

        /* Top edge */
        EVE_CoDl_bitmapTransformA(phost, 0);
        EVE_CoDl_bitmapTransformB(phost, edge_scale);
        EVE_CoDl_bitmapTransformC(phost, 0);
        EVE_CoDl_bitmapTransformD(phost, 0);
        EVE_CoDl_bitmapTransformE(phost, 0);
        EVE_CoDl_bitmapTransformF(phost, 0);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, sx1 + render_corner_w, sy1);
        EVE_CoDl_end(phost);

        /* Bottom edge */
        EVE_CoDl_bitmapTransformB(phost, -edge_scale);
        EVE_CoDl_bitmapTransformC(phost, edge_tex_max);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, sx1 + render_corner_w, sy2 + 1 - render_corner_h);
        EVE_CoDl_end(phost);
    }

    if(edge_v_len > 0) {
        EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, render_corner_w, edge_v_len);

        /* Left edge */
        EVE_CoDl_bitmapTransformA(phost, edge_scale);
        EVE_CoDl_bitmapTransformB(phost, 0);
        EVE_CoDl_bitmapTransformC(phost, 0);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, sx1, sy1 + render_corner_h);
        EVE_CoDl_end(phost);

        /* Right edge */
        EVE_CoDl_bitmapTransformA(phost, -edge_scale);
        EVE_CoDl_bitmapTransformC(phost, edge_tex_max);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, sx2 + 1 - render_corner_w, sy1 + render_corner_h);
        EVE_CoDl_end(phost);
    }

    /* Center fill */
    int32_t cx1 = sx1 + render_corner_w;
    int32_t cy1 = sy1 + render_corner_h;
    int32_t cx2 = sx2 + 1 - render_corner_w;
    int32_t cy2 = sy2 + 1 - render_corner_h;

    if(cx2 > cx1 && cy2 > cy1) {
        lv_area_t center_screen;
        center_screen.x1 = cx1 + layer->buf_area.x1;
        center_screen.y1 = cy1 + layer->buf_area.y1;
        center_screen.x2 = cx2 + layer->buf_area.x1 - 1;
        center_screen.y2 = cy2 + layer->buf_area.y1 - 1;

        lv_area_t center_scissor;
        if(lv_area_intersect(&center_scissor, &center_screen, &t->clip_area)) {
            lv_draw_eve5_set_scissor(u, &center_scissor, &layer->buf_area);

            int32_t radius = 1;
            EVE_CoDl_lineWidth(phost, radius * 16);
            EVE_CoDl_begin(phost, RECTS);
            EVE_CoDl_vertex2f_0(phost, cx1 - 1, cy1 - 1);
            EVE_CoDl_vertex2f_0(phost, cx2, cy2);
            EVE_CoDl_end(phost);
        }
    }
}

void lv_draw_eve5_alpha_draw_arc(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t *layer = t->target_layer;
    const lv_draw_arc_dsc_t *dsc = t->draw_dsc;

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

    int32_t arc_span;
    if(end_angle > start_angle)
        arc_span = end_angle - start_angle;
    else if(end_angle < start_angle)
        arc_span = 360 - start_angle + end_angle;
    else
        arc_span = 360;

    /* Tighten scissor to arc bounding box */
    const lv_area_t *layer_area = &layer->buf_area;
    lv_area_t arc_bbox = {
        cx - radius_out - 1 + layer_area->x1,
        cy - radius_out - 1 + layer_area->y1,
        cx + radius_out + 1 + layer_area->x1,
        cy + radius_out + 1 + layer_area->y1
    };
    lv_area_t arc_scissor;
    if(!lv_area_intersect(&arc_scissor, &arc_bbox, &t->clip_area)) return;
    lv_draw_eve5_set_scissor(u, &arc_scissor, layer_area);

    bool is_full = (arc_span >= 360);
    bool reverse = (!is_full && arc_span > 180);

    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_saveContext(phost);

    /* Phase 1: Clear stencil within arc bbox.
     * Full circle / reverse: clear to 0xFF (entire bbox is "in arc").
     * Normal partial: clear to 0x00 (only wedge will be marked 0xFF). */
    EVE_CoDl_clearStencil(phost, (is_full || reverse) ? 0xFF : 0x00);
    EVE_CoDl_clear(phost, 0, 1, 0);

    /* All stencil build phases: colorMask(0,0,0,0) to write stencil only. */
    EVE_CoDl_colorMask(phost, 0, 0, 0, 0);

    /* Phase 2: Build angular wedge mask (partial arcs only).
     * Uses EDGE_STRIP_R + INVERT for odd-even fill: arc sector flips
     * to 0xFF (from 0x00) or complement flips to 0x00 (from 0xFF).
     * Same vertex geometry as the RGB path. */
    if(!is_full) {
        int32_t sa = start_angle, ea = end_angle;
        if(reverse) { sa = end_angle; ea = start_angle; }

        int32_t wedge_span;
        if(ea > sa) wedge_span = ea - sa;
        else if(ea < sa) wedge_span = 360 - sa + ea;
        else wedge_span = 0;

        int32_t r2 = radius_out * 8 / 5;
        if(r2 < radius_out + 4) r2 = radius_out + 4;

        int32_t cx16 = cx * 16 - 8;
        int32_t cy16 = cy * 16 - 8;

        int32_t v_sa_x16 = cx16 + (((int32_t)lv_trigo_cos(sa) * r2) >> (LV_TRIGO_SHIFT - 4));
        int32_t v_sa_y16 = cy16 + (((int32_t)lv_trigo_sin(sa) * r2) >> (LV_TRIGO_SHIFT - 4));
        int32_t v_ea_x16 = cx16 + (((int32_t)lv_trigo_cos(ea) * r2) >> (LV_TRIGO_SHIFT - 4));
        int32_t v_ea_y16 = cy16 + (((int32_t)lv_trigo_sin(ea) * r2) >> (LV_TRIGO_SHIFT - 4));

        EVE_CoDl_stencilOp(phost, KEEP, INVERT);

        EVE_CoDl_begin(phost, EDGE_STRIP_R);
        EVE_CoDl_vertex2f_4(phost, v_sa_x16 + 16, v_sa_y16);
        EVE_CoDl_vertex2f_4(phost, cx16 + 16, cy16);
        EVE_CoDl_vertex2f_4(phost, v_ea_x16 + 16, v_ea_y16);
        if(wedge_span > 90) {
            int32_t mid_deg = (sa + 90) % 360;
            int32_t v_mx16 = cx16 + (((int32_t)lv_trigo_cos(mid_deg) * r2) >> (LV_TRIGO_SHIFT - 4));
            int32_t v_my16 = cy16 + (((int32_t)lv_trigo_sin(mid_deg) * r2) >> (LV_TRIGO_SHIFT - 4));
            EVE_CoDl_vertex2f_4(phost, v_mx16 + 16, v_my16);
        }
        EVE_CoDl_vertex2f_4(phost, v_sa_x16 + 16, v_sa_y16);
        EVE_CoDl_end(phost);
    }

    /* Phase 3: Mark arc body in stencil.
     * After phase 2: arc sector = 0xFF, non-arc = 0x00.
     * Outer circle DECR where 0xFF → 0xFE marks arc-inside-outer.
     * Inner circle DECR where 0xFE → 0xFD marks inner exclusion.
     * Result: arc annulus = 0xFE, inner = 0xFD, non-arc = 0x00/0xFF.
     *
     * Circle sizes use -8 (0.5px shrink) so the stencil boundary lands
     * at the AA midpoint rather than the outermost AA fringe. DECR
     * triggers on any non-zero coverage, and the fringe extends ~0.5px
     * beyond geometric — the -8 compensates, matching the fill/border
     * single-step stencil convention. */
    EVE_CoDl_stencilFunc(phost, EQUAL, 0xFF, 0xFF);
    EVE_CoDl_stencilOp(phost, KEEP, DECR);
    draw_circle_subpx(u, cx * 2 - 1, cy * 2 - 1, radius_out * 16 - 8);

    if(radius_in > 0) {
        EVE_CoDl_stencilFunc(phost, EQUAL, 0xFE, 0xFF);
        draw_circle_subpx(u, cx * 2 - 1, cy * 2 - 1, radius_in * 16 - 8);
    }

    /* Caps: REPLACE 0xFE so they pass the final stencil test.
     * Drawn unconditionally (ALWAYS) — caps may extend slightly beyond
     * the wedge, matching the RGB path's behavior. */
    if(!is_full && dsc->rounded) {
        int32_t stroke_width = radius_out - radius_in;
        int32_t sum_radius = radius_in + radius_out;
        int32_t cx16 = cx * 16 - 8;
        int32_t cy16 = cy * 16 - 8;

        int32_t cap0_x16 = cx16 + (((int32_t)lv_trigo_cos(start_angle) * sum_radius) >> (LV_TRIGO_SHIFT - 3));
        int32_t cap0_y16 = cy16 + (((int32_t)lv_trigo_sin(start_angle) * sum_radius) >> (LV_TRIGO_SHIFT - 3));
        int32_t cap1_x16 = cx16 + (((int32_t)lv_trigo_cos(end_angle) * sum_radius) >> (LV_TRIGO_SHIFT - 3));
        int32_t cap1_y16 = cy16 + (((int32_t)lv_trigo_sin(end_angle) * sum_radius) >> (LV_TRIGO_SHIFT - 3));

        EVE_CoDl_stencilFunc(phost, ALWAYS, 0xFE, 0xFF);
        EVE_CoDl_stencilOp(phost, KEEP, REPLACE);
        EVE_CoDl_pointSize(phost, stroke_width * 8 - 8);
        EVE_CoDl_begin(phost, POINTS);
        EVE_CoDl_vertex2f_4(phost, cap0_x16, cap0_y16);
        EVE_CoDl_vertex2f_4(phost, cap1_x16, cap1_y16);
        EVE_CoDl_end(phost);
    }

    /* Phase 4: Draw alpha through stencil.
     * Only pixels with stencil == 0xFE (arc body + caps) are drawn.
     * All edges are binary (single-step stencil). */
    EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
    EVE_CoDl_stencilFunc(phost, EQUAL, 0xFE, 0xFF);
    EVE_CoDl_stencilOp(phost, KEEP, KEEP);
    EVE_CoDl_colorA(phost, dsc->opa);
    EVE_CoDl_lineWidth(phost, 16);
    EVE_CoDl_begin(phost, RECTS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_vertex2f_0(phost, 2048, 2048);
    EVE_CoDl_end(phost);

    EVE_CoDl_restoreContext(phost);
}


#endif /* LV_USE_DRAW_EVE5 */
