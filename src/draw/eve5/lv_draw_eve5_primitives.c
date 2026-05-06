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
 *
 * Uses EVE's stencil buffer for complex shapes and alpha channel for masking.
 * Arc drawing is in lv_draw_eve5_arc.c.
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
#include "../lv_draw_rect.h"
#include "../lv_draw_line.h"
#include "../lv_draw_triangle.h"

/**********************
 * STATIC PROTOTYPES
 **********************/

void draw_circle_subpx(lv_draw_eve5_unit_t * u, int32_t cx2, int32_t cy2, int32_t r16);
bool setup_gradient_bitmap(lv_draw_eve5_unit_t * u, const lv_grad_dsc_t * grad,
                           lv_opa_t opa, int32_t w, int32_t h, bool alpha_to_rgb);
static bool draw_gradient_fill(lv_draw_eve5_unit_t * u, const lv_draw_fill_dsc_t * dsc,
                               int32_t x, int32_t y, int32_t w, int32_t h,
                               int32_t radius, const lv_area_t * clip, const lv_area_t * layer_area,
                               bool alpha_to_rgb);

/**********************
 * PRIMITIVE HELPERS
 **********************/

/**
 * Half-pixel precision circle.
 * @param cx2 Center X in 1/2 pixel units
 * @param cy2 Center Y in 1/2 pixel units
 * @param r16 Radius in 1/16 pixel units
 */
void draw_circle_subpx(lv_draw_eve5_unit_t * u, int32_t cx2, int32_t cy2, int32_t r16)
{
    if(r16 <= 0) return;

    EVE_CoDl_pointSize(u->hal, r16);
    EVE_CoDl_begin(u->hal, POINTS);
    EVE_CoDl_vertex2f_1(u->hal, cx2, cy2);
    EVE_CoDl_end(u->hal);
}

void lv_draw_eve5_draw_rect(lv_draw_eve5_unit_t * u, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                            int32_t radius, const lv_area_t * clip_area, const lv_area_t * layer_area)
{
    /* x1/y1/x2/y2 are inclusive (LVGL convention). w/h here are "size minus 1"
     * — the rendering math (vertices, scissor width via lv_area_get_width,
     * circle center as x1+x2) consistently uses this convention. A degenerate
     * rect has x2 < x1 (so w < 0); a 1-pixel rect has x2 == x1 (w == 0) and
     * must still render. */
    int32_t w = x2 - x1;
    int32_t h = y2 - y1;

    if(w < 0 || h < 0) return;

    int32_t max_r = LV_MIN(w, h) / 2;
    if(radius > max_r) radius = max_r;
    if(radius < 0) radius = 0;

    /* Perfect circle: use POINTS with sub-pixel precision when the shape
     * is a fully-rounded square. RECTS can't achieve this because its
     * vertex pair would need to cross (radius > coord_diff/2). */
    if(w == h && radius == max_r && w >= 3) {
        draw_circle_subpx(u, x1 * 2 + w, y1 * 2 + h, (w + 1) * 8);
        return;
    }

    /* EVE quirk: RECTS rendering derives from the POINTS path, so
     * lineWidth < 16 (1px) causes the entire rect to fade out (become
     * transparent) rather than producing sharp corners. Use scissor
     * clipping for hard-edged (radius 0) rectangles instead. */
    bool needs_scissor_fix = (radius < 1);

    if(needs_scissor_fix) {
        if(clip_area && layer_area) {
            lv_area_t rect_screen;
            rect_screen.x1 = x1 + layer_area->x1;
            rect_screen.y1 = y1 + layer_area->y1;
            rect_screen.x2 = x2 + layer_area->x1;
            rect_screen.y2 = y2 + layer_area->y1;

            lv_area_t scissor_area;
            if(!lv_area_intersect(&scissor_area, &rect_screen, clip_area)) {
                return;
            }

            lv_draw_eve5_set_scissor(u, &scissor_area, layer_area);
        }
        else {
            EVE_CoDl_scissorXY(u->hal, x1, y1);
            EVE_CoDl_scissorSize(u->hal, w, h);
        }
        EVE_CoDl_lineWidth(u->hal, 16);
        EVE_CoDl_begin(u->hal, RECTS);
        EVE_CoDl_vertex2f_0(u->hal, x1 - 2, y1 - 2);
        EVE_CoDl_vertex2f_0(u->hal, x2 + 2, y2 + 2);
        EVE_CoDl_end(u->hal);
    }
    else {
        EVE_CoDl_lineWidth(u->hal, radius * 16 + 8);
        EVE_CoDl_begin(u->hal, RECTS);
        EVE_CoDl_vertex2f_0(u->hal, x1 + radius, y1 + radius);
        EVE_CoDl_vertex2f_0(u->hal, x2 - radius, y2 - radius);
        EVE_CoDl_end(u->hal);
    }

    if(needs_scissor_fix && clip_area && layer_area) {
        lv_draw_eve5_set_scissor(u, clip_area, layer_area);
    }
}

/**********************
 * GRADIENT BITMAP FILL
 **********************/

static inline uint32_t pack_argb8(lv_color_t c, lv_opa_t a)
{
    return ((uint32_t)a << 24) | ((uint32_t)c.red << 16) | ((uint32_t)c.green << 8) | c.blue;
}

static inline uint32_t lerp_argb8(uint32_t c0, uint32_t c1, uint8_t t)
{
    uint32_t a = ((c0 >> 24) * (255 - t) + (c1 >> 24) * t) / 255;
    uint32_t r = (((c0 >> 16) & 0xFF) * (255 - t) + ((c1 >> 16) & 0xFF) * t) / 255;
    uint32_t g = (((c0 >> 8) & 0xFF) * (255 - t) + ((c1 >> 8) & 0xFF) * t) / 255;
    uint32_t b = ((c0 & 0xFF) * (255 - t) + (c1 & 0xFF) * t) / 255;
    return (a << 24) | (r << 16) | (g << 8) | b;
}

/**
 * Build a gradient bitmap and configure bitmap DL state.
 *
 * Generates gradient pixels, allocates GPU memory, uploads bitmap data,
 * and sets up bitmap handle, layout, size, source, and transform.
 * After this returns true, the caller can draw BITMAPS at the desired
 * position with whatever masking is appropriate.
 *
 * BT820 bitmap transform precision issue:
 * The X transform splits into high/low parts, causing visible 16-pixel-wide
 * banding when the bitmap is small (2 pixels). VER gradients use Y transform
 * which doesn't have this issue. For HOR gradients, we use a 16-pixel
 * pre-rendered strip to avoid banding (or 2-pixel for narrow fills < 16px
 * where banding isn't visible).
 *
 * Returns true if gradient was set up, false if not a supported type.
 */
bool setup_gradient_bitmap(lv_draw_eve5_unit_t * u, const lv_grad_dsc_t * grad,
                           lv_opa_t opa, int32_t w, int32_t h, bool alpha_to_rgb)
{
    EVE_HalContext *phost = u->hal;

    if(grad->dir != LV_GRAD_DIR_HOR && grad->dir != LV_GRAD_DIR_VER) return false;
    if(grad->stops_count < 2) return false;

    bool is_ver = (grad->dir == LV_GRAD_DIR_VER);

    bool simple = (grad->stops_count == 2
                   && grad->stops[0].frac == 0
                   && grad->stops[1].frac == 255);

    uint32_t pixels[256];
    uint32_t pixel_count;

    if(simple) {
        lv_color_t white = { .red = 255, .green = 255, .blue = 255 };
        uint32_t c0 = pack_argb8(alpha_to_rgb ? white : grad->stops[0].color,
                                 LV_OPA_MIX2(opa, grad->stops[0].opa));
        uint32_t c1 = pack_argb8(alpha_to_rgb ? white : grad->stops[1].color,
                                 LV_OPA_MIX2(opa, grad->stops[1].opa));

        if(is_ver || w < 16) {
            pixel_count = 2;
            pixels[0] = c0;
            pixels[1] = c1;
        }
        else {
            pixel_count = 16;
            for(uint32_t i = 0; i < 16; i++) {
                pixels[i] = lerp_argb8(c0, c1, (uint8_t)(i * 255 / 15));
            }
        }
    }
    else {
        pixel_count = 256;

        uint32_t stop_colors[LV_GRADIENT_MAX_STOPS];
        lv_color_t white = { .red = 255, .green = 255, .blue = 255 };
        for(uint8_t i = 0; i < grad->stops_count; i++) {
            stop_colors[i] = pack_argb8(alpha_to_rgb ? white : grad->stops[i].color,
                                        LV_OPA_MIX2(opa, grad->stops[i].opa));
        }

        uint8_t si = 0;
        for(uint32_t i = 0; i < 256; i++) {
            while(si < grad->stops_count - 2 && i >= grad->stops[si + 1].frac) {
                si++;
            }

            uint8_t frac0 = grad->stops[si].frac;
            uint8_t frac1 = grad->stops[si + 1].frac;

            if(frac1 == frac0) {
                pixels[i] = stop_colors[si + 1];
            }
            else {
                uint8_t t = (uint8_t)(((uint32_t)(i - frac0) * 255) / (frac1 - frac0));
                pixels[i] = lerp_argb8(stop_colors[si], stop_colors[si + 1], t);
            }
        }
    }

    /* ARGB8 is BT820-only (format 20). On pre-EVE5 we drop to a 16bpp format:
     *   - ARGB4 (format 6) when the gradient carries alpha (any stop opa < max,
     *     parent opa < max, or alpha_to_rgb mode using alpha as the gradient).
     *   - RGB565 (format 7) when fully opaque — gives 5-6-5 RGB precision vs
     *     ARGB4's 4-4-4, so smooth gradients band less.
     * pixel_count is 2/16/256 — always even, so 16bpp stride is 4-byte aligned
     * and we can write two pixels per memWrite word. */
    bool is_argb8 = (EVE_GEN >= EVE5);
    bool need_alpha = alpha_to_rgb || opa < LV_OPA_MAX;
    if(!need_alpha) {
        for(uint8_t i = 0; i < grad->stops_count; i++) {
            if(grad->stops[i].opa < LV_OPA_MAX) { need_alpha = true; break; }
        }
    }
    uint8_t bpp = is_argb8 ? 4 : 2;
    /* ARGB8 (format 20) is BT820-only; symbol doesn't exist on earlier-gen
     * single-target builds. is_argb8 evaluates to false there at compile time
     * via EVE_GEN, but the symbol still needs to be hidden from the compiler. */
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    uint16_t eve_fmt = is_argb8 ? ARGB8 : (need_alpha ? ARGB4 : RGB565);
#else
    uint16_t eve_fmt = need_alpha ? ARGB4 : RGB565;
#endif
    uint32_t byte_count = pixel_count * bpp;
    uint32_t byte_count_aligned = ALIGN_UP(byte_count, 4);

    /* Pre-BT820: serve from the scratch ring (the underlying allocator caps
     * live handles at 64, which a frame full of gradients can blow past).
     * BT820 uses Esd_GpuAlloc directly with PendingFree → deferred reclaim. */
    uint32_t addr;
    if(is_argb8) {
        Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, byte_count_aligned, 0);
        addr = Esd_GpuAlloc_Get(u->allocator, handle);
        if(addr == GA_INVALID) {
            LV_LOG_WARN("EVE5: Failed to allocate GPU memory for gradient bitmap");
            return false;
        }
        Esd_GpuAlloc_PendingFree(u->allocator, handle);
    }
    else {
        addr = lv_draw_eve5_ring_alloc(u, byte_count_aligned, 4);
        if(addr == GA_INVALID) {
            LV_LOG_WARN("EVE5: Failed to allocate gradient bitmap from scratch ring");
            return false;
        }
    }

    EVE_CoCmd_memWrite(phost, addr, byte_count);
    if(is_argb8) {
        for(uint32_t i = 0; i < pixel_count; i++) {
            EVE_Cmd_wr32(phost, pixels[i]);
        }
    }
    else if(need_alpha) {
        /* ARGB4 layout (matches convert_argb8888_to_argb4):
         *   16-bit value, little-endian, A in MSB nibble:
         *     bits 15-12 = A4, 11-8 = R4, 7-4 = G4, 3-0 = B4. */
        for(uint32_t i = 0; i < pixel_count; i += 2) {
            uint32_t p0 = pixels[i];
            uint32_t p1 = pixels[i + 1];
            uint16_t v0 = (uint16_t)((((p0 >> 24) & 0xF0) << 8)
                                     | (((p0 >> 16) & 0xF0) << 4)
                                     | ((p0 >> 8) & 0xF0)
                                     | ((p0 & 0xFF) >> 4));
            uint16_t v1 = (uint16_t)((((p1 >> 24) & 0xF0) << 8)
                                     | (((p1 >> 16) & 0xF0) << 4)
                                     | ((p1 >> 8) & 0xF0)
                                     | ((p1 & 0xFF) >> 4));
            EVE_Cmd_wr32(phost, ((uint32_t)v1 << 16) | v0);
        }
    }
    else {
        /* RGB565 layout (matches convert_rgb888_to_rgb565):
         *   bits 15-11 = R5, 10-5 = G6, 4-0 = B5. */
        for(uint32_t i = 0; i < pixel_count; i += 2) {
            uint32_t p0 = pixels[i];
            uint32_t p1 = pixels[i + 1];
            uint16_t v0 = (uint16_t)((((p0 >> 16) & 0xF8) << 8)
                                     | (((p0 >> 8) & 0xFC) << 3)
                                     | ((p0 & 0xFF) >> 3));
            uint16_t v1 = (uint16_t)((((p1 >> 16) & 0xF8) << 8)
                                     | (((p1 >> 8) & 0xFC) << 3)
                                     | ((p1 & 0xFF) >> 3));
            EVE_Cmd_wr32(phost, ((uint32_t)v1 << 16) | v0);
        }
    }
    EVE_Hal_requestFenceBeforeSwap(phost);

    int32_t bmp_w = is_ver ? 1 : (int32_t)pixel_count;
    int32_t bmp_h = is_ver ? (int32_t)pixel_count : 1;
    int32_t stride = bmp_w * bpp;

    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);
    /* Use the CoDl wrappers — they emit BITMAP_{LAYOUT,SIZE,SOURCE}_H only on
     * chips that support them (LAYOUT_H/SIZE_H: FT810+, SOURCE_H: BT820+). */
    EVE_CoDl_bitmapLayout(phost, eve_fmt, (uint16_t)stride, (uint16_t)bmp_h);
    if(is_ver) {
        EVE_CoDl_bitmapSize(phost, BILINEAR, REPEAT, BORDER,
                            (uint16_t)w, (uint16_t)h);
    }
    else {
        EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, REPEAT,
                            (uint16_t)w, (uint16_t)h);
    }
    EVE_CoDl_bitmapSource(phost, addr);

    /* Scale bitmap: map (pixel_count - 1) texels to the gradient dimension.
     * BT815+ supports s1.15 precision (p=1); pre-BT815 (FT80x/FT81x) only
     * supports s8.8 (p=0) — the `p` bit at position 17 of BITMAP_TRANSFORM_*
     * is reserved on those chips. Use s1.15 when available for sharper
     * gradients; fall back to s8.8 on overflow or older silicon. */
    bool can_s1_15 = (EVE_GEN >= EVE3);
    int32_t unity = can_s1_15 ? 0x8000 : 0x100;
    bool xform_p = can_s1_15;
    int32_t grad_dim = is_ver ? h : w;
    int32_t grad_coeff = (int32_t)(pixel_count - 1) * unity / grad_dim;
    bool grad_p = xform_p;
    if(grad_coeff > 0xFFFF) {
        /* Overflow only happens in s1.15; drop to s8.8 for this axis. */
        grad_coeff = (int32_t)(pixel_count - 1) * 0x0100 / grad_dim;
        grad_p = 0;
    }
    if(is_ver) {
        EVE_CoDl_bitmapTransformA_ex(phost, xform_p, unity / w);
        EVE_CoDl_bitmapTransformE_ex(phost, grad_p, grad_coeff);
    }
    else {
        EVE_CoDl_bitmapTransformA_ex(phost, grad_p, grad_coeff);
        EVE_CoDl_bitmapTransformE_ex(phost, xform_p, unity / h);
    }

    return true;
}

/**
 * Draw a gradient fill, optionally masked to a rounded rect.
 * Returns true if gradient was drawn, false if not a supported gradient type.
 */
static bool draw_gradient_fill(lv_draw_eve5_unit_t * u, const lv_draw_fill_dsc_t * dsc,
                               int32_t x, int32_t y, int32_t w, int32_t h,
                               int32_t radius, const lv_area_t * clip, const lv_area_t * layer_area,
                               bool alpha_to_rgb)
{
    EVE_HalContext *phost = u->hal;

    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_saveContext(phost);

    if(!setup_gradient_bitmap(u, &dsc->grad, dsc->opa, w, h, alpha_to_rgb)) {
        EVE_CoDl_restoreContext(phost);
        return false;
    }

    if(radius > 0) {
        bool grad_has_alpha = (dsc->opa < LV_OPA_MAX);
        if(!grad_has_alpha) {
            for(uint8_t i = 0; i < dsc->grad.stops_count; i++) {
                if(dsc->grad.stops[i].opa < LV_OPA_MAX) {
                    grad_has_alpha = true;
                    break;
                }
            }
        }

        /* Phase 1a: Clear bbox alpha to 0 */
        EVE_CoDl_colorArgb_ex(phost, 0x00000000);
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        lv_draw_eve5_draw_rect(u, x, y, x + w - 1, y + h - 1, 0, clip, layer_area);

        /* Phase 1b: Write rounded rect mask (alpha=255 inside) */
        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
        lv_draw_eve5_draw_rect(u, x, y, x + w - 1, y + h - 1, radius, clip, layer_area);

        if(grad_has_alpha) {
            /* Phase 1c: Multiply mask by gradient bitmap alpha */
            EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
            EVE_CoDl_begin(phost, BITMAPS);
            EVE_CoDl_vertex2f_0(phost, x, y);
            EVE_CoDl_end(phost);
        }

        /* Phase 2: Draw gradient RGB using the combined mask */
        EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
        EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, x, y);
        EVE_CoDl_end(phost);
    }
    else {
        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, x, y);
        EVE_CoDl_end(phost);
    }

    EVE_CoDl_restoreContext(phost);

    return true;
}


/**********************
 * FILL DRAWING
 **********************/

/**
 * Draw a filled rectangle.
 *
 * @param alpha_to_rgb When false, normal RGB pass with configured color/gradient.
 *                     Rounded gradient fills use alpha-as-scratch masking.
 *                     When true, renders alpha contribution as grayscale luminance
 *                     for L8 render-target path. Caller must use default blend mode.
 */
void lv_draw_eve5_hal_draw_fill(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb)
{
    lv_layer_t * layer = t->target_layer;
    lv_draw_fill_dsc_t * dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;

    int32_t x1 = t->area.x1 - layer->buf_area.x1;
    int32_t y1 = t->area.y1 - layer->buf_area.y1;
    int32_t x2 = t->area.x2 - layer->buf_area.x1;
    int32_t y2 = t->area.y2 - layer->buf_area.y1;
    int32_t w = x2 - x1 + 1;
    int32_t h = y2 - y1 + 1;

    const lv_area_t * clip = &t->clip_area;
    const lv_area_t * layer_area = &layer->buf_area;

    lv_draw_eve5_set_scissor(u, clip, layer_area);

    int32_t radius = dsc->radius;
    int32_t real_radius = LV_MIN3((w - 1) / 2, (h - 1) / 2, radius);

    if(dsc->grad.dir != LV_GRAD_DIR_NONE && dsc->grad.stops_count >= 2) {
        if(draw_gradient_fill(u, dsc, x1, y1, w, h, real_radius, clip, layer_area, alpha_to_rgb)) {
            if(!alpha_to_rgb && real_radius > 0) {
                lv_draw_eve5_track_alpha_trashed(u, x1, y1, x2, y2);
            }
            return;
        }
    }

    /* Solid fill or unsupported gradient fallback */
    lv_color_t fill_color;
    uint8_t fill_opa = dsc->opa;

    if(dsc->grad.dir != LV_GRAD_DIR_NONE && dsc->grad.stops_count > 0) {
        if(dsc->grad.stops_count == 1) {
            fill_color = dsc->grad.stops[0].color;
            fill_opa = LV_OPA_MIX2(dsc->opa, dsc->grad.stops[0].opa);
        }
        else {
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
        fill_color = dsc->color;
    }

    if(alpha_to_rgb)
        EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
    else
        EVE_CoDl_colorRgb(u->hal, fill_color.red, fill_color.green, fill_color.blue);
    EVE_CoDl_colorA(u->hal, fill_opa);

    /* Perfect circle: use POINTS with half-pixel precision.
     * Skip for w < 4 because AA makes tiny circles too transparent. */
    if(w == h && radius == LV_RADIUS_CIRCLE && w >= 4) {
        int32_t cx2 = x1 * 2 + (w - 1);
        int32_t cy2 = y1 * 2 + (h - 1);
        int32_t r16 = w * 8;
        draw_circle_subpx(u, cx2, cy2, r16);
    }
    else {
        lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, real_radius, clip, layer_area);
    }
}

/**********************
 * BORDER DRAWING
 **********************/

/**
 * Draw a border (hollow rounded rectangle).
 *
 * Uses alpha-channel masking for visible rounded corners: builds a "donut" mask
 * (outer minus inner rounded rect), then composites the border color through it.
 *
 * @param alpha_to_rgb See lv_draw_eve5_hal_draw_fill.
 */
void lv_draw_eve5_hal_draw_border(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb)
{
    lv_layer_t * layer = t->target_layer;
    lv_draw_border_dsc_t * dsc = t->draw_dsc;

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

    const lv_area_t * clip = &t->clip_area;
    const lv_area_t * layer_area = &layer->buf_area;

    int32_t clip_x1 = clip->x1 - layer_area->x1;
    int32_t clip_y1 = clip->y1 - layer_area->y1;
    int32_t clip_x2 = clip->x2 - layer_area->x1;
    int32_t clip_y2 = clip->y2 - layer_area->y1;

    /* Fast path: if all corners are outside clip, draw simple lines */
    bool corners_clipped = (rout > 0) &&
                           (clip_x1 > x1 + rout || clip_x2 < x1 + rout ||
                            clip_y1 > y1 + rout) &&
                           (clip_x1 > x2 - rout || clip_x2 < x2 - rout ||
                            clip_y1 > y1 + rout) &&
                           (clip_x1 > x1 + rout || clip_x2 < x1 + rout ||
                            clip_y2 < y2 - rout) &&
                           (clip_x1 > x2 - rout || clip_x2 < x2 - rout ||
                            clip_y2 < y2 - rout);

    if(corners_clipped || (rout == 0 && dsc->opa >= LV_OPA_MAX)) {
        /* Draw borders as simple lines (safe when corners aren't visible or at full opacity) */
        lv_draw_eve5_set_scissor(u, clip, layer_area);

        if(alpha_to_rgb)
            EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
        else
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

    /* Alpha-channel masking path for visible rounded corners */

    /* When a side is disabled, extend inner rect past outer to zero the mask there */
    int32_t inner_x1 = x1 + ((dsc->side & LV_BORDER_SIDE_LEFT)   ? dsc->width : -dsc->width);
    int32_t inner_x2 = x2 - ((dsc->side & LV_BORDER_SIDE_RIGHT)  ? dsc->width : -dsc->width);
    int32_t inner_y1 = y1 + ((dsc->side & LV_BORDER_SIDE_TOP)    ? dsc->width : -dsc->width);
    int32_t inner_y2 = y2 - ((dsc->side & LV_BORDER_SIDE_BOTTOM) ? dsc->width : -dsc->width);

    int32_t rin = rout - dsc->width;
    if(rin < 0) rin = 0;

    /* Perfect circle detection */
    bool is_circle = (w == h && dsc->radius == LV_RADIUS_CIRCLE &&
                      dsc->side == LV_BORDER_SIDE_FULL && w >= 4);
    int32_t cx2 = 0, cy2 = 0;
    if(is_circle) {
        cx2 = x1 * 2 + (w - 1);
        cy2 = y1 * 2 + (h - 1);
    }

    lv_draw_eve5_set_scissor(u, clip, layer_area);

    EVE_CoDl_vertexFormat(u->hal, 0);
    EVE_CoDl_saveContext(u->hal);

    /* Phase 1a: Clear bbox alpha to 0 */
    EVE_CoDl_colorArgb_ex(u->hal, 0x00000000);
    EVE_CoDl_colorMask(u->hal, 0, 0, 0, 1);
    EVE_CoDl_blendFunc(u->hal, ONE, ZERO);
    lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, 0, clip, layer_area);

    /* Phase 1b: Write outer shape into alpha */
    EVE_CoDl_colorA(u->hal, dsc->opa);
    if(is_circle)
        draw_circle_subpx(u, cx2, cy2, w * 8);
    else
        lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, rout, clip, layer_area);

    /* Phase 1c: Subtract inner shape from alpha */
    EVE_CoDl_colorA(u->hal, 255);
    EVE_CoDl_blendFunc(u->hal, ZERO, ONE_MINUS_SRC_ALPHA);
    if(is_circle) {
        int32_t inner_d = w - 2 * dsc->width;
        if(inner_d > 0)
            draw_circle_subpx(u, cx2, cy2, inner_d * 8);
    }
    else
        lv_draw_eve5_draw_rect(u, inner_x1, inner_y1, inner_x2, inner_y2, rin, clip, layer_area);

    /* Phase 2: Draw border color using the mask */
    EVE_CoDl_colorMask(u->hal, 1, 1, 1, 1);
    if(alpha_to_rgb)
        EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
    else
        EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(u->hal, 255);
    EVE_CoDl_blendFunc(u->hal, DST_ALPHA, ONE_MINUS_DST_ALPHA);
    if(is_circle)
        draw_circle_subpx(u, cx2, cy2, w * 8);
    else
        lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, rout, clip, layer_area);

    EVE_CoDl_restoreContext(u->hal);

    if(!alpha_to_rgb) {
        lv_draw_eve5_track_alpha_trashed(u, x1, y1, x2, y2);
    }
}

/**********************
 * FILL + BORDER UNIFIED
 **********************/

/**
 * Optimized combined fill+border rendering.
 *
 * When fill is opaque with no gradient and border covers all sides, draws
 * outer rect (blended border color) + inner rect (fill color) instead of
 * using the expensive alpha masking path.
 */
void lv_draw_eve5_hal_draw_fill_with_border(lv_draw_eve5_unit_t * u,
                                            const lv_draw_task_t * fill_task,
                                            const lv_draw_task_t * border_task)
{
    const lv_draw_fill_dsc_t * fill_dsc = fill_task->draw_dsc;
    const lv_draw_border_dsc_t * border_dsc = border_task->draw_dsc;

    if(fill_dsc->opa <= LV_OPA_MIN) {
        if(border_dsc->opa <= LV_OPA_MIN) {
            return;
        }
        lv_draw_eve5_hal_draw_border(u, border_task, false);
        return;
    }
    if(border_dsc->opa <= LV_OPA_MIN) {
        lv_draw_eve5_hal_draw_fill(u, fill_task, false);
        return;
    }

    bool can_optimize = (fill_dsc->opa >= LV_OPA_MAX) &&
                        (border_dsc->side == LV_BORDER_SIDE_FULL) &&
                        (border_dsc->width > 0) &&
                        (fill_dsc->grad.dir == LV_GRAD_DIR_NONE);

    if(!can_optimize) {
        lv_draw_eve5_hal_draw_fill(u, fill_task, false);
        lv_draw_eve5_hal_draw_border(u, border_task, false);
        return;
    }

    lv_layer_t * layer = border_task->target_layer;
    const lv_area_t * layer_area = &layer->buf_area;

    int32_t x1 = border_task->area.x1 - layer_area->x1;
    int32_t y1 = border_task->area.y1 - layer_area->y1;
    int32_t x2 = border_task->area.x2 - layer_area->x1;
    int32_t y2 = border_task->area.y2 - layer_area->y1;
    int32_t w = x2 - x1 + 1;
    int32_t h = y2 - y1 + 1;

    int32_t rout = border_dsc->radius;
    int32_t max_r = (LV_MIN(w, h) - 1) / 2;
    if(rout > max_r) rout = max_r;

    int32_t rin = rout - border_dsc->width;
    if(rin < 0) rin = 0;

    lv_area_t clip;
    if(!lv_area_intersect(&clip, &fill_task->clip_area, &border_task->clip_area)) {
        return;
    }

    lv_draw_eve5_set_scissor(u, &clip, layer_area);

    /* Pre-blend border color with fill for the border region */
    uint8_t border_r, border_g, border_b;
    if(border_dsc->opa >= LV_OPA_MAX) {
        border_r = border_dsc->color.red;
        border_g = border_dsc->color.green;
        border_b = border_dsc->color.blue;
    }
    else {
        uint8_t opa = border_dsc->opa;
        uint8_t inv_opa = 255 - opa;
        border_r = (border_dsc->color.red * opa + fill_dsc->color.red * inv_opa) / 255;
        border_g = (border_dsc->color.green * opa + fill_dsc->color.green * inv_opa) / 255;
        border_b = (border_dsc->color.blue * opa + fill_dsc->color.blue * inv_opa) / 255;
    }

    if(w == h && border_dsc->radius == LV_RADIUS_CIRCLE && w >= 4) {
        int32_t cx2 = x1 * 2 + (w - 1);
        int32_t cy2 = y1 * 2 + (h - 1);

        EVE_CoDl_colorRgb(u->hal, border_r, border_g, border_b);
        EVE_CoDl_colorA(u->hal, 255);
        draw_circle_subpx(u, cx2, cy2, w * 8);

        int32_t inner_d = w - 2 * border_dsc->width;
        if(inner_d >= 4) {
            EVE_CoDl_colorRgb(u->hal, fill_dsc->color.red, fill_dsc->color.green, fill_dsc->color.blue);
            draw_circle_subpx(u, cx2, cy2, inner_d * 8);
        }
    }
    else {
        EVE_CoDl_colorRgb(u->hal, border_r, border_g, border_b);
        EVE_CoDl_colorA(u->hal, 255);
        lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, rout, &clip, layer_area);

        int32_t bw = border_dsc->width;
        int32_t inner_x1 = x1 + bw;
        int32_t inner_y1 = y1 + bw;
        int32_t inner_x2 = x2 - bw;
        int32_t inner_y2 = y2 - bw;

        if(inner_x2 > inner_x1 && inner_y2 > inner_y1) {
            EVE_CoDl_colorRgb(u->hal, fill_dsc->color.red, fill_dsc->color.green, fill_dsc->color.blue);
            lv_draw_eve5_draw_rect(u, inner_x1, inner_y1, inner_x2, inner_y2, rin, &clip, layer_area);
        }
    }
}

/**********************
 * TRIANGLE DRAWING
 **********************/

/**
 * Build the EDGE_STRIP_B stencil mask for a triangle.
 * Caller must set colorMask, stencilOp, stencilFunc before calling.
 */
void build_triangle_stencil(EVE_HalContext *phost, const lv_point_t p[3])
{
    /* Half-pixel precision offset to match LVGL's SW renderer:
     * x: +0.5px (right), y: -0.5px (up) due to BT820 EDGE_STRIP_B quirk. */
    EVE_CoDl_begin(phost, EDGE_STRIP_B);
    EVE_CoDl_vertex2f_1(phost, p[0].x * 2 + 1, p[0].y * 2 - 1);
    EVE_CoDl_vertex2f_1(phost, p[1].x * 2 + 1, p[1].y * 2 - 1);
    EVE_CoDl_vertex2f_1(phost, p[2].x * 2 + 1, p[2].y * 2 - 1);
    EVE_CoDl_vertex2f_1(phost, p[0].x * 2 + 1, p[0].y * 2 - 1);
    EVE_CoDl_end(phost);
}

static bool draw_gradient_triangle(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                                   const lv_draw_triangle_dsc_t * dsc,
                                   const lv_point_t p[3],
                                   int32_t xmin, int32_t ymin, int32_t xmax, int32_t ymax,
                                   bool alpha_to_rgb)
{
    EVE_HalContext *phost = u->hal;

    int32_t w = xmax - xmin + 1;
    int32_t h = ymax - ymin + 1;
    if(w <= 0 || h <= 0) return false;

    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_saveContext(phost);

    lv_draw_eve5_clear_stencil(u, xmin, ymin, xmax, ymax,
                               &t->clip_area, &t->target_layer->buf_area);

    EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
    EVE_CoDl_stencilOp(phost, KEEP, INVERT);
    EVE_CoDl_stencilFunc(phost, ALWAYS, 255, 255);

    build_triangle_stencil(phost, p);

    EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
    EVE_CoDl_stencilFunc(phost, EQUAL, 255, 255);

    if(!setup_gradient_bitmap(u, &dsc->grad, dsc->opa, w, h, alpha_to_rgb)) {
        EVE_CoDl_restoreContext(phost);
        return false;
    }

    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, xmin, ymin);
    EVE_CoDl_end(phost);

    EVE_CoDl_restoreContext(phost);

    return true;
}

/**
 * Draw a filled triangle.
 * @param alpha_to_rgb See lv_draw_eve5_hal_draw_fill.
 */
void lv_draw_eve5_hal_draw_triangle(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb)
{
    lv_layer_t * layer = t->target_layer;
    lv_draw_triangle_dsc_t * dsc = t->draw_dsc;

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

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    if(dsc->grad.dir != LV_GRAD_DIR_NONE && dsc->grad.stops_count >= 2) {
        if(draw_gradient_triangle(u, t, dsc, p, xmin, ymin, xmax, ymax, alpha_to_rgb)) {
            return;
        }
    }

    /* Solid fill or unsupported gradient fallback */
    lv_color_t fill_color;
    uint8_t fill_opa = dsc->opa;

    if(dsc->grad.dir != LV_GRAD_DIR_NONE && dsc->grad.stops_count > 0) {
        if(dsc->grad.stops_count == 1) {
            fill_color = dsc->grad.stops[0].color;
            fill_opa = LV_OPA_MIX2(dsc->opa, dsc->grad.stops[0].opa);
        }
        else {
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
        fill_color = dsc->color;
    }

    if(alpha_to_rgb)
        EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
    else
        EVE_CoDl_colorRgb(u->hal, fill_color.red, fill_color.green, fill_color.blue);
    EVE_CoDl_colorA(u->hal, fill_opa);

    /* Stencil with EDGE_STRIP_B for even-odd fill */
    EVE_CoDl_saveContext(u->hal);

    lv_draw_eve5_clear_stencil(u, xmin, ymin, xmax, ymax,
                               &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorMask(u->hal, 0, 0, 0, 0);
    EVE_CoDl_stencilOp(u->hal, KEEP, INVERT);
    EVE_CoDl_stencilFunc(u->hal, ALWAYS, 255, 255);

    build_triangle_stencil(u->hal, p);

    EVE_CoDl_colorMask(u->hal, 1, 1, 1, 1);
    EVE_CoDl_stencilFunc(u->hal, EQUAL, 255, 255);
    EVE_CoDl_lineWidth(u->hal, 16);

    EVE_CoDl_begin(u->hal, RECTS);
    EVE_CoDl_vertex2f_0(u->hal, xmin, ymin);
    EVE_CoDl_vertex2f_0(u->hal, xmax, ymax);
    EVE_CoDl_end(u->hal);

    EVE_CoDl_restoreContext(u->hal);
}

#endif /* LV_USE_DRAW_EVE5 */
