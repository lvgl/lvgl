/**
 * @file lv_draw_eve5_arc.c
 *
 * EVE5 (BT820) Arc Drawing Implementation
 *
 * Algorithm matches the CMD_ARC firmware implementation:
 * 1. Clear alpha in arc bbox (preserves existing RGB)
 * 2. Build stencil wedge mask via EDGE_STRIP_R + INCR (partial arcs only)
 * 3. Paint annulus into alpha channel (paintalpha/clearalpha compositing)
 * 4. Draw round end caps into alpha (paintalpha, unconditional stencil)
 * 5. Reveal: composite arc color over existing content via DST_ALPHA blend
 *
 * Special cases:
 * - Full circle: skip stencil, just annulus + reveal
 * - Arc > 180°: "reverse trick" — stencil the complement sector (always <= 180°)
 *   and invert the selection via ClearStencil(1) + EQUAL(1)
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw.h"
#include "../lv_draw_arc.h"

/* Set to 0 to force the stencil-based software path even on BT820+.
 * Useful for testing/comparing the SW arc implementation against CMD_ARC. */
#ifndef LV_DRAW_EVE5_USE_CMD_ARC
#define LV_DRAW_EVE5_USE_CMD_ARC 0
#endif

/**********************
 * STATIC HELPERS
 **********************/

/* Arc circle: center offset by -0.5px to match LVGL's pixel grid convention.
 * Radius is exact (no expansion) — the -0.5px center aligns coverage to
 * the expected 2*radius pixel bounding box. */
static void draw_circle(lv_draw_eve5_unit_t *u, int32_t cx, int32_t cy, int32_t radius)
{
    draw_circle_subpx(u, cx * 2 - 1, cy * 2 - 1, radius * 16);
}

#if EVE_SUPPORT_CHIPID >= EVE_BT820 && LV_DRAW_EVE5_USE_CMD_ARC
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

/**********************
 * STENCIL ARC
 **********************/

/**
 * Stencil-based arc rendering matching CMD_ARC.
 *
 * Uses a 3-phase alpha-channel masking technique:
 * - paintalpha: blend(ONE, ONE_MINUS_SRC_ALPHA) into alpha — composites the shape mask
 * - clearalpha: blend(ZERO, ONE_MINUS_SRC_ALPHA) into alpha — punches holes in the mask
 * - reveal: blend(DST_ALPHA, ONE_MINUS_DST_ALPHA) into RGB — draws color where alpha exists
 *
 * Angular clipping uses EDGE_STRIP_R with stencil INCR to create a wedge mask.
 * The "reverse trick" ensures the stencil wedge is always <= 180°.
 *
 * Alpha channel is used as scratch space — tracked for repair by the alpha pass.
 */
static void draw_arc_stencil(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t,
                             int32_t cx, int32_t cy, int32_t radius_out, int32_t radius_in,
                             int32_t start_angle, int32_t end_angle, bool alpha_to_rgb)
{
    EVE_HalContext *phost = u->hal;
    lv_draw_arc_dsc_t *dsc = t->draw_dsc;
    const lv_area_t *clip = &t->clip_area;
    const lv_area_t *layer_area = &t->target_layer->buf_area;

    /* Compute arc span in degrees, handling wrap-around.
     * start_angle and end_angle are 0-359 from caller. */
    int32_t arc_span;
    if(end_angle > start_angle)
        arc_span = end_angle - start_angle;
    else if(end_angle < start_angle)
        arc_span = 360 - start_angle + end_angle;
    else
        arc_span = 360;  /* same angle after mod = full circle */

    /* Tighten scissor to arc bounding box — limits EDGE_STRIP_R fills
     * and the alpha clear to the arc region only. */
    lv_area_t arc_bbox = {
        cx - radius_out - 1 + layer_area->x1,
        cy - radius_out - 1 + layer_area->y1,
        cx + radius_out + 1 + layer_area->x1,
        cy + radius_out + 1 + layer_area->y1
    };
    lv_area_t arc_scissor;
    if(!lv_area_intersect(&arc_scissor, &arc_bbox, clip)) return;
    lv_draw_eve5_set_scissor(u, &arc_scissor, layer_area);

    /* Set vertexFormat and colorA before saveContext so restoreContext pops
     * back to clean values. Masking phases require alpha=255 for a full-strength
     * mask; opacity is applied only at the reveal pass. */
    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_colorA(phost, 255);
    EVE_CoDl_saveContext(phost);

    /* ========== Special case: full circle (no stencil needed) ========== */
    if(arc_span >= 360) {
        /* Zero alpha in bbox. EVE's CLEAR ignores ColorMask, so we use a
         * blend-based clear: blend(ZERO, ZERO) forces dst=0 for all channels,
         * and ColorMask(0,0,0,1) restricts the write to alpha only. */
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
        EVE_CoDl_blendFunc(phost, ZERO, ZERO);
        EVE_CoDl_lineWidth(phost, 16);
        EVE_CoDl_begin(phost, RECTS);
        EVE_CoDl_vertex2f_0(phost, 0, 0);
        EVE_CoDl_vertex2f_0(phost, 2048, 2048);
        EVE_CoDl_end(phost);

        /* Paint annulus alpha: outer circle via paintalpha compositing. */
        EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_SRC_ALPHA);
        draw_circle(u, cx, cy, radius_out);

        /* Punch inner circle hole via clearalpha. */
        if(radius_in > 0) {
            EVE_CoDl_blendFunc(phost, ZERO, ONE_MINUS_SRC_ALPHA);
            draw_circle(u, cx, cy, radius_in);
        }

        /* Modulate mask by opacity: dst_a *= opa/255.
         * blend(ZERO, SRC_ALPHA) with a full-coverage RECT scales the alpha
         * channel without affecting RGB (still masked to alpha-only writes). */
        if(dsc->opa < LV_OPA_MAX) {
            EVE_CoDl_colorA(phost, dsc->opa);
            EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
            EVE_CoDl_lineWidth(phost, 16);
            EVE_CoDl_begin(phost, RECTS);
            EVE_CoDl_vertex2f_0(phost, 0, 0);
            EVE_CoDl_vertex2f_0(phost, 2048, 2048);
            EVE_CoDl_end(phost);
        }

        /* Reveal: draw RGB using the opacity-scaled alpha mask.
         * blend(DST_ALPHA, ONE_MINUS_DST_ALPHA) composites arc color over
         * existing layer content. */
        EVE_CoDl_colorMask(phost, 1, 1, 1, 0);
        EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
        draw_circle(u, cx, cy, radius_out);

        EVE_CoDl_restoreContext(phost);
        goto track_alpha;
    }

    /* ========== Main arc: stencil-based angular clipping ========== */

    {
        /* Reverse trick: for arcs > 180°, stencil the complement sector
         * (always <= 180°) and invert the selection. This bounds the stencil
         * wedge to at most 5-6 EDGE_STRIP_R vertices regardless of arc angle. */
        bool reverse = (arc_span > 180);
        int32_t sa = start_angle, ea = end_angle;
        if(reverse) {
            sa = end_angle;
            ea = start_angle;
        }

        /* Clear stencil in bbox.
         * Normal:  stencil=0, INCR→sector=1,     EQUAL 1 selects sector.
         * Reverse: stencil=1, INCR→complement=2,  EQUAL 1 selects everything
         *          EXCEPT the complement = the actual large arc. */
        EVE_CoDl_clearStencil(phost, reverse ? 0x01 : 0x00);
        EVE_CoDl_clear(phost, 0, 1, 0);  /* clear stencil only */

        /* Zero alpha in bbox (blend-based, preserves RGB). */
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
        EVE_CoDl_blendFunc(phost, ZERO, ZERO);
        EVE_CoDl_lineWidth(phost, 16);
        EVE_CoDl_begin(phost, RECTS);
        EVE_CoDl_vertex2f_0(phost, 0, 0);
        EVE_CoDl_vertex2f_0(phost, 2048, 2048);
        EVE_CoDl_end(phost);

        /* Build stencil wedge mask using EDGE_STRIP_R with INCR.
         * ColorMask(0,0,0,0): write nothing to framebuffer, only stencil.
         * r2 oversized (1.6×) to ensure full coverage beyond arc AA fringe. */
        EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
        EVE_CoDl_stencilMask(phost, 0x01);
        EVE_CoDl_stencilOp(phost, INCR, INCR);

        int32_t r2 = radius_out * 8 / 5;
        if(r2 < radius_out + 4) r2 = radius_out + 4;

        /* Compute wedge span (sa→ea, the potentially-swapped angles). */
        int32_t wedge_span;
        if(ea > sa)
            wedge_span = ea - sa;
        else if(ea < sa)
            wedge_span = 360 - sa + ea;
        else
            wedge_span = 0;

        /* Arc center in 1/16 pixel units with -0.5px offset. */
        int32_t cx16 = cx * 16 - 8;
        int32_t cy16 = cy * 16 - 8;

        /* Vertex positions for the stencil wedge fan in 1/16 pixel units.
         * Uses lv_trigo_cos/sin in LVGL coordinates (0° = 3 o'clock, CW, Y-down). */
        int32_t v_sa_x16 = cx16 + (((int32_t)lv_trigo_cos(sa) * r2) >> (LV_TRIGO_SHIFT - 4));
        int32_t v_sa_y16 = cy16 + (((int32_t)lv_trigo_sin(sa) * r2) >> (LV_TRIGO_SHIFT - 4));
        int32_t v_ea_x16 = cx16 + (((int32_t)lv_trigo_cos(ea) * r2) >> (LV_TRIGO_SHIFT - 4));
        int32_t v_ea_y16 = cy16 + (((int32_t)lv_trigo_sin(ea) * r2) >> (LV_TRIGO_SHIFT - 4));
		
        /* EDGE_STRIP_R sub-pixel correction: +1px to x coordinates.
         * BT820 EDGE_STRIP rasterization expects vertices at (+0.5px, -0.5px)
         * relative to integer pixel coords. Our vertices are already at
         * (-0.5px, -0.5px) from the geometric center offset (cx16/cy16),
         * so y is already correct but x needs +1.0px (= +16 in 1/16 units). */
        EVE_CoDl_begin(phost, EDGE_STRIP_R);
        EVE_CoDl_vertex2f_4(phost, v_sa_x16 + 16, v_sa_y16);  /* V0: start, far out */
        EVE_CoDl_vertex2f_4(phost, cx16 + 16, cy16);            /* V1: center */
        EVE_CoDl_vertex2f_4(phost, v_ea_x16 + 16, v_ea_y16);   /* V2: end, far out */
        if(wedge_span > 90) {
            /* Intermediate vertex at sa+90° for arcs > quarter turn,
             * needed to maintain correct odd-even stencil fill. */
            int32_t mid_deg = (sa + 90) % 360;
            int32_t v_mx16 = cx16 + (((int32_t)lv_trigo_cos(mid_deg) * r2) >> (LV_TRIGO_SHIFT - 4));
            int32_t v_my16 = cy16 + (((int32_t)lv_trigo_sin(mid_deg) * r2) >> (LV_TRIGO_SHIFT - 4));
            EVE_CoDl_vertex2f_4(phost, v_mx16 + 16, v_my16);   /* V3: intermediate */
        }
        EVE_CoDl_vertex2f_4(phost, v_sa_x16 + 16, v_sa_y16);  /* V4: close to start */
        EVE_CoDl_end(phost);

        /* Draw stencil-clipped annulus into alpha channel.
         * StencilMask(0x00): prevent annulus drawing from modifying stencil.
         * StencilFunc(EQUAL, 1): only draw where stencil == 1 (the wedge). */
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
        EVE_CoDl_stencilMask(phost, 0x00);
        EVE_CoDl_stencilFunc(phost, EQUAL, 0x01, 0x01);

        /* Paint outer circle alpha (paintalpha compositing). */
        EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_SRC_ALPHA);
        draw_circle(u, cx, cy, radius_out);

        /* Punch inner circle hole (clearalpha). */
        if(radius_in > 0) {
            EVE_CoDl_blendFunc(phost, ZERO, ONE_MINUS_SRC_ALPHA);
            draw_circle(u, cx, cy, radius_in);
        }

        /* Round end caps: drawn unconditionally (ALWAYS stencil) using
         * paintalpha, so they blend smoothly with the annulus AA at the
         * junction points instead of creating a hard seam. */
        if(dsc->rounded) {
            EVE_CoDl_stencilFunc(phost, ALWAYS, 0x01, 0x01);
            EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_SRC_ALPHA);

            int32_t stroke_width = radius_out - radius_in;
            int32_t sum_radius = radius_in + radius_out;

            /* Cap positions in 1/16 pixel units, with -0.5px center offset.
             * Using sum of radii (= 2 * mid_radius) with >> (TRIGO_SHIFT - 3) gives
             * displacement in 1/16 px, avoiding truncation for odd stroke widths. */
            int32_t cap0_x16 = cx16 + (((int32_t)lv_trigo_cos(start_angle) * sum_radius) >> (LV_TRIGO_SHIFT - 3));
            int32_t cap0_y16 = cy16 + (((int32_t)lv_trigo_sin(start_angle) * sum_radius) >> (LV_TRIGO_SHIFT - 3));
            int32_t cap1_x16 = cx16 + (((int32_t)lv_trigo_cos(end_angle) * sum_radius) >> (LV_TRIGO_SHIFT - 3));
            int32_t cap1_y16 = cy16 + (((int32_t)lv_trigo_sin(end_angle) * sum_radius) >> (LV_TRIGO_SHIFT - 3));

            EVE_CoDl_pointSize(phost, stroke_width * 8);
            EVE_CoDl_begin(phost, POINTS);
            EVE_CoDl_vertex2f_4(phost, cap0_x16, cap0_y16);
            EVE_CoDl_vertex2f_4(phost, cap1_x16, cap1_y16);
            EVE_CoDl_end(phost);
        }
    }

    /* Modulate mask by opacity: dst_a *= opa/255.
     * blend(ZERO, SRC_ALPHA) with a full-coverage RECT scales the alpha
     * channel without affecting RGB (still masked to alpha-only writes).
     * Use ALWAYS stencil to cover caps that extend beyond the wedge. */
    if(dsc->opa < LV_OPA_MAX) {
        EVE_CoDl_stencilFunc(phost, ALWAYS, 0x01, 0x01);
        EVE_CoDl_colorA(phost, dsc->opa);
        EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
        EVE_CoDl_lineWidth(phost, 16);
        EVE_CoDl_begin(phost, RECTS);
        EVE_CoDl_vertex2f_0(phost, 0, 0);
        EVE_CoDl_vertex2f_0(phost, 2048, 2048);
        EVE_CoDl_end(phost);
    }

    /* Reveal: draw RGB using the opacity-scaled alpha mask.
     * blend(DST_ALPHA, ONE_MINUS_DST_ALPHA) composites the arc color over
     * existing layer content. */
    EVE_CoDl_colorMask(phost, 1, 1, 1, 0);
    EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
    draw_circle(u, cx, cy, radius_out);

    EVE_CoDl_restoreContext(phost);

track_alpha:
    /* Alpha channel is used as scratch space; mark for repair. */
    if(!alpha_to_rgb) {
        lv_draw_eve5_track_alpha_trashed(u,
            cx - radius_out - 1, cy - radius_out - 1,
            cx + radius_out + 1, cy + radius_out + 1);
    }
}

/**********************
 * PUBLIC API
 **********************/

/* alpha_to_rgb=false: normal RGB pass. Draws arc with its configured color.
 * Uses alpha-as-scratch masking: the alpha channel is trashed and tracked
 * for repair by the alpha correction pass.
 *
 * alpha_to_rgb=true: renders the task's alpha contribution as grayscale luminance
 * into RGB, for later copying into a layer's alpha channel. Requires the
 * caller to use default blend mode: blend(SRC_ALPHA, ONE_MINUS_SRC_ALPHA)
 * with colorMask(1,1,1,1). The A channel is scratch space in this mode. */
void lv_draw_eve5_hal_draw_arc(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t, bool alpha_to_rgb)
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

    if(alpha_to_rgb)
        EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
    else
        EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);

#if EVE_SUPPORT_CHIPID >= EVE_BT820 && LV_DRAW_EVE5_USE_CMD_ARC
    /*
     * Use CMD_ARC (BT820+) when possible:
     * - Chip must be BT820 or newer (runtime check for multi-target builds)
     * - Radii must be within 1-511 range
     * - Arc must have rounded ends (CMD_ARC always draws rounded caps)
     * - Must be fully opaque (CMD_ARC draws directly to all channels;
     *   partial opacity requires the mask-then-reveal stencil path)
     * - Arc bbox must be fully inside clip area (CMD_ARC manages its own
     *   scissor internally, so external clipping doesn't work)
     */
    if(!alpha_to_rgb &&
       (EVE_CHIPID >= EVE_BT820) &&
       (radius_out <= 511) &&
       (radius_in >= 1) &&
       (dsc->rounded) &&
       (dsc->opa >= LV_OPA_MAX) &&
       /* CMD_ARC manages its own scissor internally, so it can't clip to
        * LVGL's clip_area. Fall through to stencil path when clipping needed. */
       (t->clip_area.x1 <= dsc->center.x - radius_out - 1) &&
       (t->clip_area.y1 <= dsc->center.y - radius_out - 1) &&
       (t->clip_area.x2 >= dsc->center.x + radius_out + 1) &&
       (t->clip_area.y2 >= dsc->center.y + radius_out + 1)) {
        uint16_t a0, a1;
        if(start_angle == end_angle) {
            /* Full circle: after % 360 both angles are equal, so furmans
             * would be identical giving l=0. Firmware needs l > 0xFFF0
             * to trigger the arc.ring (full circle) path. */
            a0 = 0;
            a1 = 0xFFFF;
        } else {
            a0 = degrees_to_furmans(start_angle);
            a1 = degrees_to_furmans(end_angle);
        }

        /* Ensure alpha is fully opaque for CMD_ARC. */
        EVE_CoDl_colorA(u->hal, 255);

        /* Offset by -0.5px to match LVGL's pixel grid convention.
         * CMD_ARC only takes integer coordinates, so use VERTEX_TRANSLATE. */
        EVE_CoDl_vertexTranslateX(u->hal, -8);
        EVE_CoDl_vertexTranslateY(u->hal, -8);
        EVE_CoCmd_arc(u->hal, cx, cy, radius_in, radius_out, a0, a1);
        EVE_CoDl_vertexTranslateX(u->hal, 0);
        EVE_CoDl_vertexTranslateY(u->hal, 0);

        /* CMD_ARC writes to all channels including alpha.
         * Track the trashed area for alpha repair at layer finish (+1px for AA). */
        lv_draw_eve5_track_alpha_trashed(u,
            cx - radius_out - 1, cy - radius_out - 1,
            cx + radius_out + 1, cy + radius_out + 1);
        return;
    }
#endif

    /* Stencil-based rendering: used for pre-BT820, non-rounded arcs, large
     * radii, or when LV_DRAW_EVE5_USE_CMD_ARC is disabled for testing.
     * Algorithm matches the CMD_ARC firmware implementation. */
    draw_arc_stencil(u, t, cx, cy, radius_out, radius_in, start_angle, end_angle, alpha_to_rgb);
}

/**********************
 * ALPHA PASS
 **********************/

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
