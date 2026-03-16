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
 * Uses EVE's stencil buffer for complex shapes and masking operations.
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

/* Primitive helpers */
void draw_circle_subpx(lv_draw_eve5_unit_t *u, int32_t cx2, int32_t cy2, int32_t r16);
static void draw_circle(lv_draw_eve5_unit_t *u, int32_t cx, int32_t cy, int32_t radius);
/* Gradient bitmap helpers */
bool setup_gradient_bitmap(lv_draw_eve5_unit_t *u, const lv_grad_dsc_t *grad,
                            lv_opa_t opa, int32_t w, int32_t h, bool alpha_to_rgb);
static bool draw_gradient_fill(lv_draw_eve5_unit_t *u, const lv_draw_fill_dsc_t *dsc,
                               int32_t x, int32_t y, int32_t w, int32_t h,
                               int32_t radius, const lv_area_t *clip, const lv_area_t *layer_area,
                               bool alpha_to_rgb);

/**********************
 * PRIMITIVE HELPERS
 **********************/

/* Half-pixel precision circle.
 * cx2/cy2: center in 1/2 pixel units.
 * r16: radius in 1/16 pixel units. */
void draw_circle_subpx(lv_draw_eve5_unit_t *u, int32_t cx2, int32_t cy2, int32_t r16)
{
    if(r16 <= 0) return;

    EVE_CoDl_pointSize(u->hal, r16);
    EVE_CoDl_begin(u->hal, POINTS);
    EVE_CoDl_vertex2f_1(u->hal, cx2, cy2);
    EVE_CoDl_end(u->hal);
}

/* Integer pixel circle (center in pixels, radius in pixels).
 * Adds +0.5px for anti-aliased edge coverage. */
static void draw_circle(lv_draw_eve5_unit_t *u, int32_t cx, int32_t cy, int32_t radius)
{
    draw_circle_subpx(u, cx * 2, cy * 2, radius * 16 + 8);
}

void lv_draw_eve5_draw_rect(lv_draw_eve5_unit_t *u, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                             int32_t radius, const lv_area_t *clip_area, const lv_area_t *layer_area)
{
    int32_t w = x2 - x1;
    int32_t h = y2 - y1;

    if(w <= 0 || h <= 0) return;

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

/**********************
 * GRADIENT BITMAP FILL
 **********************/

/* Pack LVGL color+opa into an ARGB8 pixel (BT820 format: A[31:24] R[23:16] G[15:8] B[7:0]) */
static inline uint32_t pack_argb8(lv_color_t c, lv_opa_t a)
{
    return ((uint32_t)a << 24) | ((uint32_t)c.red << 16) | ((uint32_t)c.green << 8) | c.blue;
}

/* Linearly interpolate between two ARGB8 values at position t (0-255) */
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
 * and sets up bitmap handle, layout, size, source, and transform in the
 * display list. After this returns true, the caller can draw BITMAPS at
 * the desired position with whatever masking is appropriate.
 *
 * The hardware's bitmap transform X computation splits into high/low parts:
 *   xosH = (xs16 & ~255) - px;  xosL = xs16 & 255;
 *   xx = (xosH * A) >> 7 + xosL * (A >> 7)
 * When A is small (2-pixel bitmap), A >> 7 truncates to nearly zero,
 * causing visible 16-pixel-wide banding in horizontal gradients.
 * VER gradients don't have this issue — Y uses a single yo*E multiply
 * without the xosH/xosL split.
 *
 * Minimum bitmap width to avoid visible banding: n=16 (worst case
 * banding ≤ 1 color level = imperceptible). For w < 16, the 2-pixel
 * path is fine since the entire fill is smaller than one 16-pixel band.
 *
 * Returns true if gradient was set up, false if not a supported type.
 */
bool setup_gradient_bitmap(lv_draw_eve5_unit_t *u, const lv_grad_dsc_t *grad,
                           lv_opa_t opa, int32_t w, int32_t h, bool alpha_to_rgb)
{
    EVE_HalContext *phost = u->hal;

    /* Only handle HOR and VER gradients */
    if(grad->dir != LV_GRAD_DIR_HOR && grad->dir != LV_GRAD_DIR_VER) return false;
    if(grad->stops_count < 2) return false;

    bool is_ver = (grad->dir == LV_GRAD_DIR_VER);

    bool simple = (grad->stops_count == 2
                   && grad->stops[0].frac == 0
                   && grad->stops[1].frac == 255);

    /* Build bitmap pixels */
    uint32_t pixels[256];
    uint32_t pixel_count;

    if(simple) {
        lv_color_t white = { .red = 255, .green = 255, .blue = 255 };
        uint32_t c0 = pack_argb8(alpha_to_rgb ? white : grad->stops[0].color,
                                  LV_OPA_MIX2(opa, grad->stops[0].opa));
        uint32_t c1 = pack_argb8(alpha_to_rgb ? white : grad->stops[1].color,
                                  LV_OPA_MIX2(opa, grad->stops[1].opa));

        if(is_ver || w < 16) {
            /* VER: 2-pixel path (Y transform has no precision issue).
             * Also used for narrow HOR fills where banding isn't visible. */
            pixel_count = 2;
            pixels[0] = c0;
            pixels[1] = c1;
        }
        else {
            /* HOR: 16-pixel pre-rendered strip avoids banding (64 bytes). */
            pixel_count = 16;
            for(uint32_t i = 0; i < 16; i++) {
                pixels[i] = lerp_argb8(c0, c1, (uint8_t)(i * 255 / 15));
            }
        }
    }
    else {
        /* Multi-stop: pre-render 256-pixel strip */
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

    /* Allocate temporary GPU memory */
    uint32_t byte_count = pixel_count * 4;
    uint32_t byte_count_aligned = ALIGN_UP(byte_count, 4);
    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, byte_count_aligned, 0);
    uint32_t addr = Esd_GpuAlloc_Get(u->allocator, handle);
    if(addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to allocate GPU memory for gradient bitmap");
        return false;
    }

    if(u->frame_alloc_count < EVE5_MAX_FRAME_ALLOCS) {
        u->frame_allocs[u->frame_alloc_count++] = handle;
    }

    EVE_CoCmd_memWrite(phost, addr, byte_count);
    for(uint32_t i = 0; i < pixel_count; i++) {
        EVE_Cmd_wr32(phost, pixels[i]);
    }

    /* Configure bitmap in display list */
    int32_t bmp_w = is_ver ? 1 : (int32_t)pixel_count;
    int32_t bmp_h = is_ver ? (int32_t)pixel_count : 1;
    int32_t stride = bmp_w * 4;

    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);
    /* BT820: BITMAP_SWIZZLE only applies in GLFORMAT mode — identity swizzle is a no-op. */
    /* EVE_CoDl_bitmapSwizzle(phost, RED, GREEN, BLUE, ALPHA); */
    EVE_CoCmd_dl(phost, BITMAP_LAYOUT_H(0, 0));
    EVE_CoCmd_dl(phost, BITMAP_SIZE_H(w >> 9, h >> 9));
    EVE_CoCmd_dl(phost, BITMAP_LAYOUT(ARGB8, stride, bmp_h));
    if(is_ver) {
        EVE_CoCmd_dl(phost, BITMAP_SIZE(BILINEAR, REPEAT, BORDER, w, h));
    }
    else {
        EVE_CoCmd_dl(phost, BITMAP_SIZE(BILINEAR, BORDER, REPEAT, w, h));
    }
    EVE_CoCmd_dl(phost, BITMAP_SOURCE_H(addr >> 24));
    EVE_CoCmd_dl(phost, BITMAP_SOURCE(addr));

    /* Scale bitmap: map (pixel_count - 1) texels to the gradient dimension.
     * Use signed 1.15 for best precision; fall back to unsigned 8.8 on overflow. */
    int32_t grad_dim = is_ver ? h : w;
    int32_t grad_coeff = (int32_t)(pixel_count - 1) * 0x8000 / grad_dim;
    bool grad_p = 1;
    if(grad_coeff > 0xFFFF) {
        grad_coeff = (int32_t)(pixel_count - 1) * 0x0100 / grad_dim;
        grad_p = 0;
    }
    if(is_ver) {
        EVE_CoDl_bitmapTransformA_ex(phost, 1, 0x8000 / w);
        EVE_CoDl_bitmapTransformE_ex(phost, grad_p, grad_coeff);
    }
    else {
        EVE_CoDl_bitmapTransformA_ex(phost, grad_p, grad_coeff);
        EVE_CoDl_bitmapTransformE_ex(phost, 1, 0x8000 / h);
    }

    return true;
}

/**
 * Draw a gradient fill, optionally masked to a rounded rect.
 * Returns true if gradient was drawn, false if not a supported gradient type.
 */
static bool draw_gradient_fill(lv_draw_eve5_unit_t *u, const lv_draw_fill_dsc_t *dsc,
                               int32_t x, int32_t y, int32_t w, int32_t h,
                               int32_t radius, const lv_area_t *clip, const lv_area_t *layer_area,
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
        /* Check if gradient has any per-pixel alpha */
        bool grad_has_alpha = (dsc->opa < LV_OPA_MAX);
        if(!grad_has_alpha) {
            for(uint8_t i = 0; i < dsc->grad.stops_count; i++) {
                if(dsc->grad.stops[i].opa < LV_OPA_MAX) { grad_has_alpha = true; break; }
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
            /* Phase 1c: Multiply mask by gradient bitmap alpha.
             * blend(ZERO, SRC_ALPHA): result_a = (bitmap_a / 255) * mask_a */
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
        /* No radius: standard alpha blending */
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

/* alpha_to_rgb=false: normal RGB pass. Draws fill with its configured color/gradient.
 * Rounded gradient fills use alpha-as-scratch masking (tracked for alpha repair).
 *
 * alpha_to_rgb=true: renders the task's alpha contribution as grayscale luminance
 * into RGB, for later copying into a layer's alpha channel. Requires the
 * caller to use default blend mode: blend(SRC_ALPHA, ONE_MINUS_SRC_ALPHA)
 * with colorMask(1,1,1,1). The A channel is scratch space in this mode. */
void lv_draw_eve5_hal_draw_fill(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t, bool alpha_to_rgb)
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

    int32_t radius = dsc->radius;
    int32_t real_radius = LV_MIN3((w - 1) / 2, (h - 1) / 2, radius);

    /* Try gradient bitmap path for HOR/VER gradients */
    if(dsc->grad.dir != LV_GRAD_DIR_NONE && dsc->grad.stops_count >= 2) {
        if(draw_gradient_fill(u, dsc, x1, y1, w, h, real_radius, clip, layer_area, alpha_to_rgb)) {
            /* Rounded gradient fills use alpha-as-scratch masking */
            if(!alpha_to_rgb && real_radius > 0) {
                lv_draw_eve5_track_alpha_trashed(u, x1, y1, x2, y2);
            }
            return;
        }
        /* Fall through to solid fill approximation for unsupported gradient types */
    }

    /* Determine fill color */
    lv_color_t fill_color;
    uint8_t fill_opa = dsc->opa;

    if(dsc->grad.dir != LV_GRAD_DIR_NONE && dsc->grad.stops_count > 0) {
        /* Unsupported gradient type - pick a representative color */
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
        fill_color = dsc->color;
    }

    if(alpha_to_rgb)
        EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
    else
        EVE_CoDl_colorRgb(u->hal, fill_color.red, fill_color.green, fill_color.blue);
    EVE_CoDl_colorA(u->hal, fill_opa);

    /* Perfect circle: use POINTS with half-pixel precision for exact bounding box.
     * Skip for w < 4 (pixel radius < 2) — AA makes tiny circles too transparent. */
    if(w == h && radius == LV_RADIUS_CIRCLE && w >= 4) {
        /* Center at (x1 + (w-1)/2.0, y1 + (h-1)/2.0) in 1/2 px units */
        int32_t cx2 = x1 * 2 + (w - 1);
        int32_t cy2 = y1 * 2 + (h - 1);
        /* Radius = w/2.0 in 1/16 px units (covers outer edges of boundary pixels) */
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

/* alpha_to_rgb: renders the task's alpha contribution as grayscale luminance
 * into RGB, for later copying into a layer's alpha channel. Requires the
 * caller to use default blend mode: blend(SRC_ALPHA, ONE_MINUS_SRC_ALPHA)
 * with colorMask(1,1,1,1). The A channel is scratch space in this mode. */
void lv_draw_eve5_hal_draw_border(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t, bool alpha_to_rgb)
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
    int32_t max_r = (LV_MIN(w, h) - 1) / 2;
    if(rout > max_r) rout = max_r;

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

    if(corners_clipped || (rout == 0 && dsc->opa >= LV_OPA_MAX)) {
        /* Fast path: draw borders as simple lines.
         * Only safe when corners aren't visible (clipped) or at full opacity
         * (overlap at corners from LINES rounded caps is invisible at opa=255). */
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

    /* Alpha-channel masking path for visible rounded corners.
     *
     * Builds a "donut" mask in the alpha channel (outer minus inner rounded rect),
     * then composites the border color through it. Same technique as gradient fill.
     * Works for both full and partial sides — when a side is disabled, the inner
     * rect extends past the outer rect on that edge, zeroing the mask there.
     */

    /* Calculate inner area based on border sides */
    int32_t inner_x1 = x1 + ((dsc->side & LV_BORDER_SIDE_LEFT)   ? dsc->width : -dsc->width);
    int32_t inner_x2 = x2 - ((dsc->side & LV_BORDER_SIDE_RIGHT)  ? dsc->width : -dsc->width);
    int32_t inner_y1 = y1 + ((dsc->side & LV_BORDER_SIDE_TOP)    ? dsc->width : -dsc->width);
    int32_t inner_y2 = y2 - ((dsc->side & LV_BORDER_SIDE_BOTTOM) ? dsc->width : -dsc->width);

    int32_t rin = rout - dsc->width;
    if(rin < 0) rin = 0;

    /* Detect perfect circle: square widget, max radius, all sides.
     * Skip for w < 4 (pixel radius < 2) — AA makes tiny circles too transparent. */
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

    /* Phase 1b: Write outer shape into alpha (alpha = opa inside) */
    EVE_CoDl_colorA(u->hal, dsc->opa);
    if(is_circle)
        draw_circle_subpx(u, cx2, cy2, w * 8);
    else
        lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, rout, clip, layer_area);

    /* Phase 1c: Subtract inner shape from alpha.
     * dst_a = dst_a * (1 - src_a/255). With colorA=255, zeroes alpha inside
     * the inner shape. AA edge pixels get partial subtraction for smooth inner edges. */
    EVE_CoDl_colorA(u->hal, 255);
    EVE_CoDl_blendFunc(u->hal, ZERO, ONE_MINUS_SRC_ALPHA);
    if(is_circle) {
        int32_t inner_d = w - 2 * dsc->width;
        if(inner_d > 0)
            draw_circle_subpx(u, cx2, cy2, inner_d * 8);
    }
    else
        lv_draw_eve5_draw_rect(u, inner_x1, inner_y1, inner_x2, inner_y2, rin, clip, layer_area);

    /* Phase 2: Draw border color using the mask.
     * result = border_color * mask + background * (1 - mask) */
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
        /* Border masking trashes alpha in the bbox */
        lv_draw_eve5_track_alpha_trashed(u, x1, y1, x2, y2);
    }
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
        lv_draw_eve5_hal_draw_border(u, border_task, false);
        return;
    }
    if (border_dsc->opa <= LV_OPA_MIN) {
        lv_draw_eve5_hal_draw_fill(u, fill_task, false);
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
        lv_draw_eve5_hal_draw_fill(u, fill_task, false);
        lv_draw_eve5_hal_draw_border(u, border_task, false);
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
    int32_t max_r = (LV_MIN(w, h) - 1) / 2;
    if(rout > max_r) rout = max_r;

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

    if(w == h && border_dsc->radius == LV_RADIUS_CIRCLE && w >= 4) {
        /* Perfect circle: use POINTS with sub-pixel precision */
        int32_t cx2 = x1 * 2 + (w - 1);
        int32_t cy2 = y1 * 2 + (h - 1);

        /* Outer circle (blended border color) */
        EVE_CoDl_colorRgb(u->hal, border_r, border_g, border_b);
        EVE_CoDl_colorA(u->hal, 255);
        draw_circle_subpx(u, cx2, cy2, w * 8);

        /* Inner circle (fill color) */
        int32_t inner_d = w - 2 * border_dsc->width;
        if(inner_d >= 4) {
            EVE_CoDl_colorRgb(u->hal, fill_dsc->color.red, fill_dsc->color.green, fill_dsc->color.blue);
            draw_circle_subpx(u, cx2, cy2, inner_d * 8);
        }
    }
    else {
        /* Outer rectangle (blended border color) */
        EVE_CoDl_colorRgb(u->hal, border_r, border_g, border_b);
        EVE_CoDl_colorA(u->hal, 255);
        lv_draw_eve5_draw_rect(u, x1, y1, x2, y2, rout, &clip, layer_area);

        /* Inner rectangle (fill color) */
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

/* Line drawing is in lv_draw_eve5_line.c */

/**********************
 * TRIANGLE DRAWING
 **********************/

/* Build the EDGE_STRIP_B stencil mask for a triangle.
 * Caller must set colorMask, stencilOp, stencilFunc before calling. */
void build_triangle_stencil(EVE_HalContext *phost, const lv_point_t p[3])
{
    /* Use half-pixel precision to correct EDGE_STRIP_B sub-pixel alignment.
     * x: +0.5px (right) to center on pixel columns.
     * y: -0.5px (up) to match LVGL's SW renderer convention.
     * This asymmetry appears to be a BT820 EDGE_STRIP_B rasterization quirk. */
    EVE_CoDl_begin(phost, EDGE_STRIP_B);
    EVE_CoDl_vertex2f_1(phost, p[0].x * 2 + 1, p[0].y * 2 - 1);
    EVE_CoDl_vertex2f_1(phost, p[1].x * 2 + 1, p[1].y * 2 - 1);
    EVE_CoDl_vertex2f_1(phost, p[2].x * 2 + 1, p[2].y * 2 - 1);
    EVE_CoDl_vertex2f_1(phost, p[0].x * 2 + 1, p[0].y * 2 - 1);  /* Close the triangle */
    EVE_CoDl_end(phost);
}

/**
 * Draw a gradient-filled triangle using stencil mask + gradient bitmap.
 * Returns true if gradient was drawn, false if not a supported gradient type.
 */
static bool draw_gradient_triangle(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t,
                                   const lv_draw_triangle_dsc_t *dsc,
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

    /* Build stencil mask from triangle edges */
    EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
    EVE_CoDl_stencilOp(phost, KEEP, INVERT);
    EVE_CoDl_stencilFunc(phost, ALWAYS, 255, 255);

    build_triangle_stencil(phost, p);

    /* Set up gradient bitmap and draw through stencil */
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

/* alpha_to_rgb=false: normal RGB pass. Draws triangle with its configured color/gradient.
 *
 * alpha_to_rgb=true: renders the task's alpha contribution as grayscale luminance
 * into RGB, for later copying into a layer's alpha channel. Requires the
 * caller to use default blend mode: blend(SRC_ALPHA, ONE_MINUS_SRC_ALPHA)
 * with colorMask(1,1,1,1). The A channel is scratch space in this mode. */
void lv_draw_eve5_hal_draw_triangle(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t, bool alpha_to_rgb)
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

    /* Try gradient bitmap path for HOR/VER gradients */
    if(dsc->grad.dir != LV_GRAD_DIR_NONE && dsc->grad.stops_count >= 2) {
        if(draw_gradient_triangle(u, t, dsc, p, xmin, ymin, xmax, ymax, alpha_to_rgb)) {
            return;
        }
        /* Fall through to solid fill approximation for unsupported gradient types */
    }

    /* Determine fill color */
    lv_color_t fill_color;
    uint8_t fill_opa = dsc->opa;

    if(dsc->grad.dir != LV_GRAD_DIR_NONE && dsc->grad.stops_count > 0) {
        /* Unsupported gradient type - pick a representative color */
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

    /* Use stencil with edge strip to fill triangle (even-odd rule) */
    EVE_CoDl_saveContext(u->hal);

    lv_draw_eve5_clear_stencil(u, xmin, ymin, xmax, ymax,
                                &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorMask(u->hal, 0, 0, 0, 0);
    EVE_CoDl_stencilOp(u->hal, KEEP, INVERT);
    EVE_CoDl_stencilFunc(u->hal, ALWAYS, 255, 255);

    build_triangle_stencil(u->hal, p);

    /* Draw where stencil was inverted (inside triangle) using RECTS */
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
