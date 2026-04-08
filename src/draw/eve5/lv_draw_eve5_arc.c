/**
 * @file lv_draw_eve5_arc.c
 *
 * EVE5 (BT820) Arc Drawing Implementation
 *
 * Algorithm:
 * 1. Clear alpha in arc bbox (preserves existing RGB)
 * 2. Build stencil wedge mask via EDGE_STRIP_R + INCR (partial arcs only)
 * 3. Paint annulus into alpha channel (paintalpha/clearalpha compositing)
 * 4. Draw round end caps into alpha (paintalpha, unconditional stencil)
 * 5. Reveal: composite arc color over existing content via DST_ALPHA blend
 *
 * Special cases:
 * - Full circle: skip stencil, just annulus + reveal
 * - Arc > 180°: "reverse trick" stencils the complement sector (always <= 180°)
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

/* CMD_ARC (BT820+) has slight rendering alignment issues.
 * Set to 1 to use CMD_ARC anyway for testing or if firmware is fixed. */
#ifndef LV_DRAW_EVE5_USE_CMD_ARC
    #define LV_DRAW_EVE5_USE_CMD_ARC 0
#endif

/**********************
 * STATIC HELPERS
 **********************/

/* Center offset by -0.5px to match LVGL's pixel grid convention. */
static void draw_circle(lv_draw_eve5_unit_t * u, int32_t cx, int32_t cy, int32_t radius)
{
    draw_circle_subpx(u, cx * 2 - 1, cy * 2 - 1, radius * 16);
}

#if EVE_SUPPORT_CHIPID >= EVE_BT820 && LV_DRAW_EVE5_USE_CMD_ARC
/* Convert degrees (0-360) to furmans (0x0000-0xFFFF).
 * LVGL: 0° at 3 o'clock; EVE: 0 furmans at 12 o'clock. */
static uint16_t degrees_to_furmans(int32_t degrees)
{
    degrees = (degrees - 90) % 360;
    if(degrees < 0) degrees += 360;
    return (uint16_t)((degrees * 65536UL) / 360);
}
#endif

/**
 * Set up bitmap state for arc image source.
 * Caller must then begin(BITMAPS) + vertex2f + end.
 */
static void setup_arc_image(EVE_HalContext *phost, uint32_t addr, uint16_t eve_format,
                            int32_t stride, int32_t w, int32_t h, uint32_t palette_addr)
{
    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
    EVE_CoDl_bitmapSource(phost, addr);
    set_palette_if_needed(phost, eve_format, palette_addr);
    eve5_set_bitmap_layout(phost, eve_format, stride, h);
    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, w, h);
    EVE_CoDl_bitmapTransform_identity(phost);
}

/**********************
 * STENCIL ARC
 **********************/

/**
 * Arc rendering using alpha-channel masking.
 * Matches CMD_ARC algorithm but fixes alignment and supports non-rounded caps.
 *
 * Uses a 3-phase alpha-channel masking technique:
 * - paintalpha: blend(ONE, ONE_MINUS_SRC_ALPHA), composites the shape mask
 * - clearalpha: blend(ZERO, ONE_MINUS_SRC_ALPHA), punches holes in the mask
 * - reveal: blend(DST_ALPHA, ONE_MINUS_DST_ALPHA), draws color where alpha exists
 *
 * Angular clipping uses EDGE_STRIP_R with stencil INCR to create a wedge mask.
 * The "reverse trick" ensures the stencil wedge is always <= 180°.
 *
 * Alpha channel is used as scratch space, tracked for repair by the alpha pass.
 */
static void draw_arc_stencil(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                             int32_t cx, int32_t cy, int32_t radius_out, int32_t radius_in,
                             int32_t start_angle, int32_t end_angle, bool alpha_to_rgb)
{
    EVE_HalContext *phost = u->hal;
    lv_draw_arc_dsc_t * dsc = t->draw_dsc;
    const lv_area_t * clip = &t->clip_area;
    const lv_area_t * layer_area = &t->target_layer->buf_area;

    /* Resolve image source if set */
    uint32_t img_addr = GA_INVALID;
    uint32_t img_palette_addr = GA_INVALID;
    uint16_t img_eve_format = 0;
    int32_t img_eve_stride = 0;
    int32_t img_w = 0, img_h = 0;
    bool has_img = false;
    bool img_has_alpha = false;

    if(dsc->img_src != NULL) {
        lv_eve5_vram_res_t * img_vr = lv_draw_eve5_resolve_to_gpu(u, dsc->img_src);
        if(img_vr != NULL) {
            eve5_vram_res_resolve(u->allocator, img_vr, &img_addr, &img_palette_addr);
            if(img_addr != GA_INVALID) {
                img_eve_format = img_vr->eve_format;
                img_eve_stride = (int32_t)img_vr->stride;
                img_w = img_vr->width;
                img_h = img_vr->height;
                has_img = true;
                img_has_alpha = (img_eve_format == ARGB8);
            }
        }
    }

    /* Image centered at arc center (matches SW renderer) */
    int32_t img_x = cx - img_w / 2;
    int32_t img_y = cy - img_h / 2;

    int32_t arc_span;
    if(end_angle > start_angle)
        arc_span = end_angle - start_angle;
    else if(end_angle < start_angle)
        arc_span = 360 - start_angle + end_angle;
    else
        arc_span = 360;

    /* Tighten scissor to arc bounding box */
    lv_area_t arc_bbox = {
        cx - radius_out - 1 + layer_area->x1,
        cy - radius_out - 1 + layer_area->y1,
        cx + radius_out + 1 + layer_area->x1,
        cy + radius_out + 1 + layer_area->y1
    };
    lv_area_t arc_scissor;
    if(!lv_area_intersect(&arc_scissor, &arc_bbox, clip)) return;
    lv_draw_eve5_set_scissor(u, &arc_scissor, layer_area);

    /* Alpha=255 for full-strength mask; opacity applied at reveal pass */
    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_colorA(phost, 255);
    EVE_CoDl_saveContext(phost);

    /* ========== Full circle: no stencil needed ========== */
    if(arc_span >= 360) {
        /* Zero alpha in bbox using blend-based clear (CLEAR ignores ColorMask) */
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
        EVE_CoDl_blendFunc(phost, ZERO, ZERO);
        EVE_CoDl_lineWidth(phost, 16);
        EVE_CoDl_begin(phost, RECTS);
        EVE_CoDl_vertex2f_0(phost, 0, 0);
        EVE_CoDl_vertex2f_0(phost, 2048, 2048);
        EVE_CoDl_end(phost);

        /* Paint outer circle (paintalpha) */
        EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_SRC_ALPHA);
        draw_circle(u, cx, cy, radius_out);

        /* Punch inner circle hole (clearalpha) */
        if(radius_in > 0) {
            EVE_CoDl_blendFunc(phost, ZERO, ONE_MINUS_SRC_ALPHA);
            draw_circle(u, cx, cy, radius_in);
        }

        /* Modulate mask by opacity: dst_a *= opa/255 */
        if(dsc->opa < LV_OPA_MAX) {
            EVE_CoDl_colorA(phost, dsc->opa);
            EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
            EVE_CoDl_lineWidth(phost, 16);
            EVE_CoDl_begin(phost, RECTS);
            EVE_CoDl_vertex2f_0(phost, 0, 0);
            EVE_CoDl_vertex2f_0(phost, 2048, 2048);
            EVE_CoDl_end(phost);
        }

        /* Modulate mask by image alpha (ARGB sources only) */
        if(has_img && img_has_alpha) {
            EVE_CoDl_colorA(phost, 255);
            EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
            setup_arc_image(phost, img_addr, img_eve_format, img_eve_stride, img_w, img_h, img_palette_addr);
            EVE_CoDl_begin(phost, BITMAPS);
            EVE_CoDl_vertex2f_0(phost, img_x, img_y);
            EVE_CoDl_end(phost);
        }

        /* Reveal: composite arc over existing content */
        EVE_CoDl_colorMask(phost, 1, 1, 1, 0);
        EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
        if(has_img && !alpha_to_rgb) {
            EVE_CoDl_colorRgb(phost, 255, 255, 255);
            EVE_CoDl_colorA(phost, 255);
            setup_arc_image(phost, img_addr, img_eve_format, img_eve_stride, img_w, img_h, img_palette_addr);
            EVE_CoDl_begin(phost, BITMAPS);
            EVE_CoDl_vertex2f_0(phost, img_x, img_y);
            EVE_CoDl_end(phost);
        }
        else {
            draw_circle(u, cx, cy, radius_out);
        }

        EVE_CoDl_restoreContext(phost);
        goto track_alpha;
    }

    /* ========== Partial arc: stencil-based angular clipping ========== */

    {
        /* Reverse trick: for arcs > 180°, stencil the complement (always <= 180°) */
        bool reverse = (arc_span > 180);
        int32_t sa = start_angle, ea = end_angle;
        if(reverse) {
            sa = end_angle;
            ea = start_angle;
        }

        /* Clear stencil: Normal=0 (INCR marks sector), Reverse=1 (INCR marks complement) */
        EVE_CoDl_clearStencil(phost, reverse ? 0x01 : 0x00);
        EVE_CoDl_clear(phost, 0, 1, 0);

        /* Zero alpha in bbox */
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
        EVE_CoDl_blendFunc(phost, ZERO, ZERO);
        EVE_CoDl_lineWidth(phost, 16);
        EVE_CoDl_begin(phost, RECTS);
        EVE_CoDl_vertex2f_0(phost, 0, 0);
        EVE_CoDl_vertex2f_0(phost, 2048, 2048);
        EVE_CoDl_end(phost);

        /* Build stencil wedge mask using EDGE_STRIP_R */
        EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
        EVE_CoDl_stencilMask(phost, 0x01);
        EVE_CoDl_stencilOp(phost, INCR, INCR);

        /* Oversized radius (1.6×) ensures full coverage beyond AA fringe */
        int32_t r2 = radius_out * 8 / 5;
        if(r2 < radius_out + 4) r2 = radius_out + 4;

        int32_t wedge_span;
        if(ea > sa)
            wedge_span = ea - sa;
        else if(ea < sa)
            wedge_span = 360 - sa + ea;
        else
            wedge_span = 0;

        /* Arc center in 1/16 pixel units with -0.5px offset */
        int32_t cx16 = cx * 16 - 8;
        int32_t cy16 = cy * 16 - 8;

        int32_t v_sa_x16 = cx16 + (((int32_t)lv_trigo_cos(sa) * r2) >> (LV_TRIGO_SHIFT - 4));
        int32_t v_sa_y16 = cy16 + (((int32_t)lv_trigo_sin(sa) * r2) >> (LV_TRIGO_SHIFT - 4));
        int32_t v_ea_x16 = cx16 + (((int32_t)lv_trigo_cos(ea) * r2) >> (LV_TRIGO_SHIFT - 4));
        int32_t v_ea_y16 = cy16 + (((int32_t)lv_trigo_sin(ea) * r2) >> (LV_TRIGO_SHIFT - 4));

        /* EDGE_STRIP_R expects vertices at (+0.5px, -0.5px); ours are at (-0.5px, -0.5px),
         * so x needs +1.0px correction */
        EVE_CoDl_begin(phost, EDGE_STRIP_R);
        EVE_CoDl_vertex2f_4(phost, v_sa_x16 + 16, v_sa_y16);
        EVE_CoDl_vertex2f_4(phost, cx16 + 16, cy16);
        EVE_CoDl_vertex2f_4(phost, v_ea_x16 + 16, v_ea_y16);
        if(wedge_span > 90) {
            /* Intermediate vertex for arcs > 90° to maintain correct odd-even fill */
            int32_t mid_deg = (sa + 90) % 360;
            int32_t v_mx16 = cx16 + (((int32_t)lv_trigo_cos(mid_deg) * r2) >> (LV_TRIGO_SHIFT - 4));
            int32_t v_my16 = cy16 + (((int32_t)lv_trigo_sin(mid_deg) * r2) >> (LV_TRIGO_SHIFT - 4));
            EVE_CoDl_vertex2f_4(phost, v_mx16 + 16, v_my16);
        }
        EVE_CoDl_vertex2f_4(phost, v_sa_x16 + 16, v_sa_y16);
        EVE_CoDl_end(phost);

        /* Draw stencil-clipped annulus into alpha */
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
        EVE_CoDl_stencilMask(phost, 0x00);
        EVE_CoDl_stencilFunc(phost, EQUAL, 0x01, 0x01);

        EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_SRC_ALPHA);
        draw_circle(u, cx, cy, radius_out);

        if(radius_in > 0) {
            EVE_CoDl_blendFunc(phost, ZERO, ONE_MINUS_SRC_ALPHA);
            draw_circle(u, cx, cy, radius_in);
        }

        /* Round end caps: ALWAYS stencil so they blend smoothly with annulus AA */
        if(dsc->rounded) {
            EVE_CoDl_stencilFunc(phost, ALWAYS, 0x01, 0x01);
            EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_SRC_ALPHA);

            int32_t stroke_width = radius_out - radius_in;
            int32_t sum_radius = radius_in + radius_out;

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

    /* Modulate mask by opacity */
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

    /* Modulate mask by image alpha (ARGB sources only) */
    if(has_img && img_has_alpha) {
        EVE_CoDl_stencilFunc(phost, ALWAYS, 0x01, 0x01);
        EVE_CoDl_colorA(phost, 255);
        EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
        setup_arc_image(phost, img_addr, img_eve_format, img_eve_stride, img_w, img_h, img_palette_addr);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, img_x, img_y);
        EVE_CoDl_end(phost);
    }

    /* Reveal: composite arc over existing content */
    EVE_CoDl_colorMask(phost, 1, 1, 1, 0);
    EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
    if(has_img && !alpha_to_rgb) {
        EVE_CoDl_colorRgb(phost, 255, 255, 255);
        EVE_CoDl_colorA(phost, 255);
        setup_arc_image(phost, img_addr, img_eve_format, img_eve_stride, img_w, img_h, img_palette_addr);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, img_x, img_y);
        EVE_CoDl_end(phost);
    }
    else {
        draw_circle(u, cx, cy, radius_out);
    }

    EVE_CoDl_restoreContext(phost);

track_alpha:
    if(!alpha_to_rgb) {
        lv_draw_eve5_track_alpha_trashed(u,
                                         cx - radius_out - 1, cy - radius_out - 1,
                                         cx + radius_out + 1, cy + radius_out + 1);
    }
}

/**********************
 * PUBLIC API
 **********************/

/**
 * Draw arc.
 *
 * alpha_to_rgb=false: normal RGB pass with alpha-as-scratch masking.
 * alpha_to_rgb=true: renders alpha contribution as grayscale luminance.
 */
void lv_draw_eve5_hal_draw_arc(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb)
{
    lv_layer_t * layer = t->target_layer;
    lv_draw_arc_dsc_t * dsc = t->draw_dsc;

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
    /* CMD_ARC requirements:
     * - Radii within 1-511, rounded ends, fully opaque
     * - Arc bbox fully inside clip (CMD_ARC manages its own scissor) */
    if(!alpha_to_rgb &&
       dsc->img_src == NULL &&
       (EVE_CHIPID >= EVE_BT820) &&
       (radius_out <= 511) &&
       (radius_in >= 1) &&
       (dsc->rounded) &&
       (dsc->opa >= LV_OPA_MAX) &&
       (t->clip_area.x1 <= dsc->center.x - radius_out - 1) &&
       (t->clip_area.y1 <= dsc->center.y - radius_out - 1) &&
       (t->clip_area.x2 >= dsc->center.x + radius_out + 1) &&
       (t->clip_area.y2 >= dsc->center.y + radius_out + 1)) {
        uint16_t a0, a1;
        if(start_angle == end_angle) {
            a0 = 0;
            a1 = 0xFFFF;
        }
        else {
            a0 = degrees_to_furmans(start_angle);
            a1 = degrees_to_furmans(end_angle);
        }

        EVE_CoDl_colorA(u->hal, 255);

        /* Offset by -0.5px via VERTEX_TRANSLATE (CMD_ARC only takes integers) */
        EVE_CoDl_vertexTranslateX(u->hal, -8);
        EVE_CoDl_vertexTranslateY(u->hal, -8);
        EVE_CoCmd_arc(u->hal, cx, cy, radius_in, radius_out, a0, a1);
        EVE_CoDl_vertexTranslateX(u->hal, 0);
        EVE_CoDl_vertexTranslateY(u->hal, 0);

        lv_draw_eve5_track_alpha_trashed(u,
                                         cx - radius_out - 1, cy - radius_out - 1,
                                         cx + radius_out + 1, cy + radius_out + 1);
        return;
    }
#endif

    draw_arc_stencil(u, t, cx, cy, radius_out, radius_in, start_angle, end_angle, alpha_to_rgb);
}

/**********************
 * ALPHA PASS
 **********************/

/**
 * Alpha correction pass for arcs.
 * Uses stencil to build arc shape mask, then draws alpha through it.
 */
void lv_draw_eve5_alpha_draw_arc(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t * layer = t->target_layer;
    const lv_draw_arc_dsc_t * dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(dsc->start_angle == dsc->end_angle) return;

    int32_t cx = dsc->center.x - layer->buf_area.x1;
    int32_t cy = dsc->center.y - layer->buf_area.y1;
    int32_t radius_out = dsc->radius;
    int32_t radius_in = dsc->radius - dsc->width;
    if(radius_in < 0) radius_in = 0;

    /* Resolve image source for ARGB alpha pass */
    uint32_t img_addr = GA_INVALID;
    uint32_t img_palette_addr = GA_INVALID;
    uint16_t img_eve_format = 0;
    int32_t img_eve_stride = 0;
    int32_t img_w = 0, img_h = 0;
    bool img_has_alpha = false;

    if(dsc->img_src != NULL) {
        lv_eve5_vram_res_t * img_vr = lv_draw_eve5_resolve_to_gpu(u, dsc->img_src);
        if(img_vr != NULL) {
            eve5_vram_res_resolve(u->allocator, img_vr, &img_addr, &img_palette_addr);
            if(img_addr != GA_INVALID) {
                img_eve_format = img_vr->eve_format;
                img_eve_stride = (int32_t)img_vr->stride;
                img_w = img_vr->width;
                img_h = img_vr->height;
                img_has_alpha = (img_eve_format == ARGB8);
            }
        }
    }

    int32_t img_x = cx - img_w / 2;
    int32_t img_y = cy - img_h / 2;

    int32_t start_angle = ((int32_t)dsc->start_angle) % 360;
    int32_t end_angle = ((int32_t)dsc->end_angle) % 360;

    int32_t arc_span;
    if(end_angle > start_angle)
        arc_span = end_angle - start_angle;
    else if(end_angle < start_angle)
        arc_span = 360 - start_angle + end_angle;
    else
        arc_span = 360;

    const lv_area_t * layer_area = &layer->buf_area;
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

    /* Phase 1: Clear stencil */
    EVE_CoDl_clearStencil(phost, (is_full || reverse) ? 0xFF : 0x00);
    EVE_CoDl_clear(phost, 0, 1, 0);

    EVE_CoDl_colorMask(phost, 0, 0, 0, 0);

    /* Phase 2: Build angular wedge mask (partial arcs only) */
    if(!is_full) {
        int32_t sa = start_angle, ea = end_angle;
        if(reverse) {
            sa = end_angle;
            ea = start_angle;
        }

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
     * Result: arc annulus = 0xFE, inner = 0xFD, non-arc = 0x00/0xFF.
     * Circle sizes use -8 (0.5px shrink) so stencil boundary lands at AA midpoint. */
    EVE_CoDl_stencilFunc(phost, EQUAL, 0xFF, 0xFF);
    EVE_CoDl_stencilOp(phost, KEEP, DECR);
    draw_circle_subpx(u, cx * 2 - 1, cy * 2 - 1, radius_out * 16 - 8);

    if(radius_in > 0) {
        EVE_CoDl_stencilFunc(phost, EQUAL, 0xFE, 0xFF);
        draw_circle_subpx(u, cx * 2 - 1, cy * 2 - 1, radius_in * 16 - 8);
    }

    /* Caps: REPLACE 0xFE so they pass the final stencil test */
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

    /* Phase 4: Draw alpha through stencil */
    EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
    EVE_CoDl_stencilFunc(phost, EQUAL, 0xFE, 0xFF);
    EVE_CoDl_stencilOp(phost, KEEP, KEEP);
    EVE_CoDl_colorA(phost, dsc->opa);
    if(img_has_alpha) {
        /* ARGB image: blit through stencil so per-pixel alpha modulates coverage */
        setup_arc_image(phost, img_addr, img_eve_format, img_eve_stride, img_w, img_h, img_palette_addr);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, img_x, img_y);
        EVE_CoDl_end(phost);
    }
    else {
        /* Solid or opaque image: flat rect at opa */
        EVE_CoDl_lineWidth(phost, 16);
        EVE_CoDl_begin(phost, RECTS);
        EVE_CoDl_vertex2f_0(phost, 0, 0);
        EVE_CoDl_vertex2f_0(phost, 2048, 2048);
        EVE_CoDl_end(phost);
    }

    EVE_CoDl_restoreContext(phost);
}

#endif /* LV_USE_DRAW_EVE5 */
