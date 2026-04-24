/**
 * @file lv_draw_eve5_line.c
 *
 * EVE5 (BT820) Line Drawing Implementation
 *
 * Hardware-accelerated rendering for lines:
 * - Horizontal, vertical, diagonal
 * - Round and flat end caps (flat caps via alpha-channel masking)
 * - Dashed lines with configurable dash/gap widths
 * - H/V scissor optimization for axis-aligned flat-cap lines
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#if !LV_DRAW_EVE5_NO_FLOAT
    #include <math.h>
#endif
#include "../lv_draw.h"
#include "../lv_draw_line.h"

/**********************
 * ALPHA ACCURACY PREDICATE
 **********************/

/**
 * Returns true when the line requires L8 render-target alpha recovery.
 *
 * Diagonal flat caps and dashes use alpha-as-scratch masking in the RGB pass,
 * which cannot be recovered by the direct-to-alpha replay. H/V flat caps use
 * the scissor optimization (sharp edges without masking) and don't need this.
 */
static bool line_segment_needs_alpha_rendertarget(const lv_draw_line_dsc_t * dsc, int32_t dx, int32_t dy);

bool lv_draw_eve5_line_needs_alpha_rendertarget(const lv_draw_task_t * t)
{
    const lv_draw_line_dsc_t * dsc = t->draw_dsc;
    if(dsc->width == 0) return false;
    if(dsc->opa <= LV_OPA_MIN) return false;

    /* Polyline: check each segment */
    if(dsc->points != NULL) {
        for(int32_t i = 0; i < dsc->point_cnt - 1; i++) {
            if(dsc->points[i].x == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i].y == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i + 1].x == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i + 1].y == LV_DRAW_LINE_POINT_NONE) {
                continue;
            }
            int32_t dx = (int32_t)(dsc->points[i + 1].x - dsc->points[i].x);
            int32_t dy = (int32_t)(dsc->points[i + 1].y - dsc->points[i].y);
            if(dx == 0 && dy == 0) continue;
            if(line_segment_needs_alpha_rendertarget(dsc, dx, dy)) return true;
        }
        return false;
    }

    if(dsc->p1.x == dsc->p2.x && dsc->p1.y == dsc->p2.y) return false;

    return line_segment_needs_alpha_rendertarget(dsc, dsc->p2.x - dsc->p1.x, dsc->p2.y - dsc->p1.y);
}

static bool line_segment_needs_alpha_rendertarget(const lv_draw_line_dsc_t * dsc, int32_t dx, int32_t dy)
{
    bool is_hv = (dx == 0 || dy == 0);
    bool is_hv_flat = is_hv && (dsc->raw_end || (!dsc->round_start && !dsc->round_end));

    bool need_flat_start = !dsc->round_start && !dsc->raw_end && !is_hv_flat;
    bool need_flat_end = !dsc->round_end && !dsc->raw_end && !is_hv_flat;
    bool has_dashes = (dsc->dash_gap > 0 && dsc->dash_width > 0);

    return need_flat_start || need_flat_end || has_dashes;
}

/**********************
 * LINE DRAWING
 **********************/

/**
 * Draw a line with hardware acceleration.
 *
 * @param alpha_to_rgb  When true, renders the line's alpha contribution as white
 *                      luminance for L8 render-target alpha recovery.
 * @param use_hv_opt    Enable H/V scissor optimization for axis-aligned lines.
 */
static void draw_line_segment(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                              const lv_draw_line_dsc_t * dsc, bool alpha_to_rgb, bool use_hv_opt);

void lv_draw_eve5_hal_draw_line(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb, bool use_hv_opt)
{
    lv_draw_line_dsc_t * dsc = t->draw_dsc;

    if(dsc->width == 0) return;
    if(dsc->opa <= LV_OPA_MIN) return;

    /* Polyline: iterate segments */
    if(dsc->points != NULL) {
        /* Disable H/V optimization if any segment is diagonal */
        bool poly_hv_opt = true;
        for(int32_t i = 0; i < dsc->point_cnt - 1; i++) {
            if(dsc->points[i].x == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i].y == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i + 1].x == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i + 1].y == LV_DRAW_LINE_POINT_NONE) {
                continue;
            }
            int32_t sdx = (int32_t)(dsc->points[i + 1].x - dsc->points[i].x);
            int32_t sdy = (int32_t)(dsc->points[i + 1].y - dsc->points[i].y);
            if(sdx != 0 && sdy != 0) {
                poly_hv_opt = false;
                break;
            }
        }
        lv_draw_line_dsc_t seg = *dsc;
        seg.points = NULL;
        seg.point_cnt = 0;
        for(int32_t i = 0; i < dsc->point_cnt - 1; i++) {
            if(dsc->points[i].x == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i].y == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i + 1].x == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i + 1].y == LV_DRAW_LINE_POINT_NONE) {
                continue;
            }
            seg.p1 = dsc->points[i];
            seg.p2 = dsc->points[i + 1];
            draw_line_segment(u, t, &seg, alpha_to_rgb, poly_hv_opt);
        }
        return;
    }

    if(dsc->p1.x == dsc->p2.x && dsc->p1.y == dsc->p2.y) return;
    draw_line_segment(u, t, dsc, alpha_to_rgb, use_hv_opt);
}

static void draw_line_segment(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                              const lv_draw_line_dsc_t * dsc, bool alpha_to_rgb, bool use_hv_opt)
{
    lv_layer_t * layer = t->target_layer;

    int32_t x1 = dsc->p1.x - layer->buf_area.x1;
    int32_t y1 = dsc->p1.y - layer->buf_area.y1;
    int32_t x2 = dsc->p2.x - layer->buf_area.x1;
    int32_t y2 = dsc->p2.y - layer->buf_area.y1;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    uint32_t line_w = dsc->width * 8; /* EVE LINE_WIDTH is half-width in 1/16 pixel units */

    /* Half-pixel alignment: SW uses asymmetric w_half0/w_half1 for odd widths */
    int32_t off = (dsc->width & 1) ? 0 : -1;

    bool need_flat_start = !dsc->round_start && !dsc->raw_end;
    bool need_flat_end = !dsc->round_end && !dsc->raw_end;
    bool has_dashes = (dsc->dash_gap > 0 && dsc->dash_width > 0);
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;

    /* H/V scissor optimization: for axis-aligned lines without round caps,
     * draw an oversized line and let the scissor clip to exact pixels. */
    bool is_hv_flat = use_hv_opt && (dx == 0 || dy == 0) && (dsc->raw_end || (!dsc->round_start && !dsc->round_end));
    if(is_hv_flat) {
        int32_t wm = dsc->width - 1;
        int32_t w_half0 = wm >> 1;
        int32_t w_half1 = w_half0 + (wm & 1);
        int32_t rx1, ry1, rx2, ry2;
        if(dy == 0) {
            rx1 = LV_MIN(x1, x2);
            rx2 = LV_MAX(x1, x2) - 1;
            ry1 = y1 - w_half1;
            ry2 = y1 + w_half0;
        }
        else {
            rx1 = x1 - w_half1;
            rx2 = x1 + w_half0;
            ry1 = LV_MIN(y1, y2);
            ry2 = LV_MAX(y1, y2) - 1;
        }
        lv_area_t line_screen = { rx1 + layer->buf_area.x1, ry1 + layer->buf_area.y1,
                                  rx2 + layer->buf_area.x1, ry2 + layer->buf_area.y1
                                };
        lv_area_t line_scissor;
        if(!lv_area_intersect(&line_scissor, &line_screen, &t->clip_area)) return;
        lv_draw_eve5_set_scissor(u, &line_scissor, &layer->buf_area);

        line_w += 16;
        off = 0;
    }

    if((!is_hv_flat && (need_flat_start || need_flat_end)) || has_dashes) {
        /* Alpha-channel masking for flat end caps and/or dashed lines.
         *
         * EVE LINES always draws round caps. To create flat ends or dash gaps:
         * 1) Draw line (with round caps) into alpha channel
         * 2) Erase unwanted regions with perpendicular masking lines
         * 3) Draw color through the resulting alpha mask */
        EVE_HalContext *phost = u->hal;
#if LV_DRAW_EVE5_NO_FLOAT
        int32_t len = lv_sqrt32((uint32_t)((int64_t)dx * dx + (int64_t)dy * dy));
        if(len == 0) len = 1;
        int32_t perp_off_x_16 = (int32_t)(-(int64_t)dy * dsc->width * 16 / len);
        int32_t perp_off_y_16 = (int32_t)((int64_t)dx * dsc->width * 16 / len);
#else
        float off_px = -0.5f;
        float fx1 = x1 + off_px;
        float fy1 = y1 + off_px;
        float fx2 = x2 + off_px;
        float fy2 = y2 + off_px;
        float len = sqrtf((float)((int64_t)dx * dx + (int64_t)dy * dy));
        float inv_len = (len > 0.0f) ? 1.0f / len : 0.0f;
        float dir_x = dx * inv_len;
        float dir_y = dy * inv_len;
        float perp_x = -dir_y;
        float perp_y = dir_x;
        float perp_ext = (float)dsc->width;
#endif

        int32_t margin = dsc->width;
        int32_t bx1 = LV_MIN(x1, x2) - margin;
        int32_t by1 = LV_MIN(y1, y2) - margin;
        int32_t bx2 = LV_MAX(x1, x2) + margin;
        int32_t by2 = LV_MAX(y1, y2) + margin;

        /* Track extended alpha trashing from masking lines beyond the clear bbox */
        int32_t track_margin = margin;
        if(!is_hv_flat && (need_flat_start || need_flat_end)) {
            uint32_t chw16 = LV_MAX(dsc->width * 4 + 24, 16);
            int32_t chw = (int32_t)((chw16 + 15) / 16);
            track_margin = LV_MAX(track_margin, LV_MAX(dsc->width + chw, 2 * chw));
        }
        if(has_dashes) {
            int32_t ghw = (dsc->dash_gap + 1) / 2;
            track_margin = LV_MAX(track_margin, dsc->width + ghw);
        }

        EVE_CoDl_saveContext(phost);

        /* Phase 1a: Clear bbox alpha to 0 (+1px for RECTS AA feather) */
        EVE_CoDl_colorArgb_ex(phost, 0x00000000);
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        EVE_CoDl_lineWidth(phost, 16);
        EVE_CoDl_begin(phost, RECTS);
        EVE_CoDl_vertex2f_0(phost, bx1 - 1, by1 - 1);
        EVE_CoDl_vertex2f_0(phost, bx2 + 1, by2 + 1);
        EVE_CoDl_end(phost);

        /* Phase 1b: Draw line into alpha (includes round caps) */
        EVE_CoDl_colorA(phost, dsc->opa);
        EVE_CoDl_lineWidth(phost, line_w);
        EVE_CoDl_begin(phost, LINES);
        EVE_CoDl_vertex2f_1(phost, x1 * 2 + off, y1 * 2 + off);
        EVE_CoDl_vertex2f_1(phost, x2 * 2 + off, y2 * 2 + off);
        EVE_CoDl_end(phost);

        /* Phase 1c: Erase unwanted regions with blend(ZERO, ONE_MINUS_SRC_ALPHA) */
        EVE_CoDl_colorA(phost, 255);
        EVE_CoDl_blendFunc(phost, ZERO, ONE_MINUS_SRC_ALPHA);

        /* Flat end cap masks */
        if(!is_hv_flat && (need_flat_start || need_flat_end)) {
            /* Half-width = W/4 + 1.5px to cover round cap AA fringe */
            uint32_t cap_hw_16 = LV_MAX(dsc->width * 4 + 24, 16);

            EVE_CoDl_lineWidth(phost, cap_hw_16);
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
                float cx = fx1 - dir_x * cap_hw_px;
                float cy = fy1 - dir_y * cap_hw_px;
                EVE_CoDl_vertex2f_4(phost,
                                    (int32_t)((cx + perp_x * perp_ext) * 16),
                                    (int32_t)((cy + perp_y * perp_ext) * 16));
                EVE_CoDl_vertex2f_4(phost,
                                    (int32_t)((cx - perp_x * perp_ext) * 16),
                                    (int32_t)((cy - perp_y * perp_ext) * 16));
            }
            if(need_flat_end) {
                float cx = fx2 + dir_x * cap_hw_px;
                float cy = fy2 + dir_y * cap_hw_px;
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

        /* Dash gap masks */
        if(has_dashes) {
            /* SW dash pattern: dash is dash_width+1 pixels, gap is dash_gap pixels,
             * period = dash_width+1+dash_gap. Aligns to absolute coordinates. */
            uint32_t gap_hw_16 = dsc->dash_gap * 8;
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

        /* Phase 2: Draw line color through the alpha mask */
        EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
        if(alpha_to_rgb)
            EVE_CoDl_colorRgb(phost, 255, 255, 255);
        else
            EVE_CoDl_colorRgb(phost, dsc->color.red, dsc->color.green, dsc->color.blue);
        EVE_CoDl_colorA(phost, 255);
        EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
        EVE_CoDl_lineWidth(phost, 16);
        EVE_CoDl_begin(phost, RECTS);
        EVE_CoDl_vertex2f_0(phost, bx1, by1);
        EVE_CoDl_vertex2f_0(phost, bx2, by2);
        EVE_CoDl_end(phost);

        EVE_CoDl_restoreContext(phost);

        if(!alpha_to_rgb) {
            int32_t tx1 = LV_MIN(x1, x2) - track_margin - 1;
            int32_t ty1 = LV_MIN(y1, y2) - track_margin - 1;
            int32_t tx2 = LV_MAX(x1, x2) + track_margin + 1;
            int32_t ty2 = LV_MAX(y1, y2) + track_margin + 1;
            lv_draw_eve5_track_alpha_trashed(u, tx1, ty1, tx2, ty2);
        }
    }
    else {
        /* Both ends rounded (or raw_end), no dashes: simple direct draw */
        if(alpha_to_rgb)
            EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
        else
            EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
        EVE_CoDl_colorA(u->hal, dsc->opa);
        EVE_CoDl_lineWidth(u->hal, line_w);
        EVE_CoDl_begin(u->hal, LINES);
        EVE_CoDl_vertex2f_1(u->hal, x1 * 2 + off, y1 * 2 + off);
        EVE_CoDl_vertex2f_1(u->hal, x2 * 2 + off, y2 * 2 + off);
        EVE_CoDl_end(u->hal);
    }

    if(is_hv_flat) {
        lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);
    }
}

/**********************
 * ALPHA PASS: LINE
 **********************/

/**
 * Direct-to-alpha pass for lines.
 *
 * For diagonal flat caps and dashes, uses stencil-based approximation which
 * produces binary edges at cap/dash boundaries instead of smooth AA. For
 * accurate alpha in these cases, use the L8 render-target path instead.
 */
static void alpha_draw_line_segment(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                                    const lv_draw_line_dsc_t * dsc, bool use_hv_opt);

void lv_draw_eve5_alpha_draw_line(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool use_hv_opt)
{
    const lv_draw_line_dsc_t * dsc = t->draw_dsc;

    if(dsc->width == 0) return;
    if(dsc->opa <= LV_OPA_MIN) return;

    /* Polyline: iterate segments */
    if(dsc->points != NULL) {
        /* Disable H/V optimization if any segment is diagonal */
        bool poly_hv_opt = true;
        for(int32_t i = 0; i < dsc->point_cnt - 1; i++) {
            if(dsc->points[i].x == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i].y == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i + 1].x == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i + 1].y == LV_DRAW_LINE_POINT_NONE) {
                continue;
            }
            int32_t sdx = (int32_t)(dsc->points[i + 1].x - dsc->points[i].x);
            int32_t sdy = (int32_t)(dsc->points[i + 1].y - dsc->points[i].y);
            if(sdx != 0 && sdy != 0) {
                poly_hv_opt = false;
                break;
            }
        }
        lv_draw_line_dsc_t seg = *dsc;
        seg.points = NULL;
        seg.point_cnt = 0;
        for(int32_t i = 0; i < dsc->point_cnt - 1; i++) {
            if(dsc->points[i].x == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i].y == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i + 1].x == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i + 1].y == LV_DRAW_LINE_POINT_NONE) {
                continue;
            }
            seg.p1 = dsc->points[i];
            seg.p2 = dsc->points[i + 1];
            alpha_draw_line_segment(u, t, &seg, poly_hv_opt);
        }
        return;
    }

    if(dsc->p1.x == dsc->p2.x && dsc->p1.y == dsc->p2.y) return;
    alpha_draw_line_segment(u, t, dsc, use_hv_opt);
}

static void alpha_draw_line_segment(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                                    const lv_draw_line_dsc_t * dsc, bool use_hv_opt)
{
    lv_layer_t * layer = t->target_layer;

    int32_t x1 = dsc->p1.x - layer->buf_area.x1;
    int32_t y1 = dsc->p1.y - layer->buf_area.y1;
    int32_t x2 = dsc->p2.x - layer->buf_area.x1;
    int32_t y2 = dsc->p2.y - layer->buf_area.y1;

    uint32_t line_w = dsc->width * 8;
    int32_t off = (dsc->width & 1) ? 0 : -1;

    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;

    bool is_hv_flat = use_hv_opt && (dx == 0 || dy == 0) && (dsc->raw_end || (!dsc->round_start && !dsc->round_end));
    lv_area_t hv_scissor;

    if(is_hv_flat) {
        int32_t wm = dsc->width - 1;
        int32_t w_half0 = wm >> 1;
        int32_t w_half1 = w_half0 + (wm & 1);
        int32_t rx1, ry1, rx2, ry2;
        if(dy == 0) {
            rx1 = LV_MIN(x1, x2);
            rx2 = LV_MAX(x1, x2) - 1;
            ry1 = y1 - w_half1;
            ry2 = y1 + w_half0;
        }
        else {
            rx1 = x1 - w_half1;
            rx2 = x1 + w_half0;
            ry1 = LV_MIN(y1, y2);
            ry2 = LV_MAX(y1, y2) - 1;
        }
        lv_area_t line_screen = { rx1 + layer->buf_area.x1, ry1 + layer->buf_area.y1,
                                  rx2 + layer->buf_area.x1, ry2 + layer->buf_area.y1
                                };
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
     * Produces binary edges at stencil boundaries (not smooth AA). */
    {
        bool need_flat_start = !dsc->round_start && !dsc->raw_end && !is_hv_flat;
        bool need_flat_end = !dsc->round_end && !dsc->raw_end && !is_hv_flat;
        bool has_dashes = (dsc->dash_gap > 0 && dsc->dash_width > 0);

        if(need_flat_start || need_flat_end || has_dashes) {
            EVE_HalContext *phost = u->hal;

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

            if(is_hv_flat) {
                lv_draw_eve5_set_scissor(u, &hv_scissor, &layer->buf_area);
            }

            EVE_CoDl_saveContext(phost);

            /* Draw round-cap line into stencil */
            EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
            EVE_CoDl_stencilOp(phost, KEEP, INCR);
            EVE_CoDl_lineWidth(phost, line_w);
            EVE_CoDl_begin(phost, LINES);
            EVE_CoDl_vertex2f_1(phost, x1 * 2 + off, y1 * 2 + off);
            EVE_CoDl_vertex2f_1(phost, x2 * 2 + off, y2 * 2 + off);
            EVE_CoDl_end(phost);

            /* Erase flat caps and/or dash gaps from stencil */
            EVE_CoDl_stencilOp(phost, KEEP, ZERO);

            if(need_flat_start || need_flat_end) {
                /* 0.5px smaller line width to align stencil boundary with RGB path */
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

            /* Draw round-cap line through stencil at opa */
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

    /* Simple round-cap line */
    EVE_CoDl_colorA(u->hal, dsc->opa);
    EVE_CoDl_lineWidth(u->hal, line_w);
    EVE_CoDl_begin(u->hal, LINES);
    EVE_CoDl_vertex2f_1(u->hal, x1 * 2 + off, y1 * 2 + off);
    EVE_CoDl_vertex2f_1(u->hal, x2 * 2 + off, y2 * 2 + off);
    EVE_CoDl_end(u->hal);

    if(is_hv_flat) {
        lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);
    }
}

#endif /* LV_USE_DRAW_EVE5 */
