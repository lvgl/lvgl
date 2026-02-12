/**
 * @file lv_draw_eve5_text.c
 *
 * EVE5 (BT820) Label/Text Drawing for LVGL Draw Unit
 *
 * This file contains:
 * - Glyph bitmap upload with aligned stride
 * - Label drawing with LVGL character iteration
 * - Letter drawing with per-glyph transforms
 * - Alpha correction pass for text
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw.h"
#include "../lv_draw_label.h"

/**********************
 * STATIC VARIABLES
 **********************/

/* Used to pass unit context to label/letter glyph callback (LVGL is single-threaded) */
static lv_draw_eve5_unit_t *s_current_unit = NULL;
static lv_layer_t *s_current_layer = NULL;
static const lv_draw_letter_dsc_t *s_current_letter_dsc = NULL;

/* Alpha pass glyph callback context */
static lv_draw_eve5_unit_t *s_alpha_unit = NULL;
static lv_layer_t *s_alpha_layer = NULL;
static const lv_draw_letter_dsc_t *s_alpha_letter_dsc = NULL;

/**********************
 * STATIC PROTOTYPES
 **********************/

/* Font/glyph helpers */
static bool glyph_bitmap_to_ramg_aligned(lv_draw_eve5_unit_t *u, uint32_t addr,
                                          const uint8_t *src, uint32_t width,
                                          uint32_t height, uint32_t eve_stride,
                                          uint8_t src_stride_align);
static uint32_t upload_glyph(lv_draw_eve5_unit_t *u, const lv_font_fmt_txt_dsc_t *font_dsc,
                              uint32_t gid, uint16_t *out_stride);
static void emit_glyph_vertex(lv_draw_eve5_unit_t *u, lv_layer_t *layer,
                               lv_draw_task_t *t, lv_draw_glyph_dsc_t *glyph_dsc,
                               const lv_draw_letter_dsc_t *letter_dsc,
                               uint16_t g_w, uint16_t g_h,
                               int32_t x, int32_t y);
static void draw_glyph_cb(lv_draw_task_t *t, lv_draw_glyph_dsc_t *glyph_dsc,
                          lv_draw_fill_dsc_t *fill_dsc, const lv_area_t *fill_area);
static void alpha_glyph_cb(lv_draw_task_t *t, lv_draw_glyph_dsc_t *glyph_dsc,
                            lv_draw_fill_dsc_t *fill_dsc, const lv_area_t *fill_area);

/**********************
 * GLYPH UPLOAD
 **********************/

static bool glyph_bitmap_to_ramg_aligned(lv_draw_eve5_unit_t *u, uint32_t addr,
                                          const uint8_t *src, uint32_t width,
                                          uint32_t height, uint32_t eve_stride,
                                          uint8_t src_stride_align, uint8_t bpp)
{
    uint32_t natural_stride = (width * bpp + 7) / 8;

    /* Allocate row buffer for aligned stride */
    uint8_t *row_buf = lv_malloc(eve_stride);
    if(!row_buf) {
        LV_LOG_ERROR("EVE5: Failed to allocate glyph row buffer");
        return false;
    }

    /* Clear buffer to ensure padding bytes are zero */
    lv_memzero(row_buf, eve_stride);

    /* For 8bpp and whole-byte-aligned sub-byte formats, or when source
     * has no special alignment, use simple row copy */
    bool simple_copy = (bpp == 8)
        || (src_stride_align == 1)
        || (src_stride_align == 0 && (width * bpp) % 8 == 0);

    if(simple_copy) {
        for(uint32_t y = 0; y < height; y++) {
            lv_memcpy(row_buf, src + y * natural_stride, natural_stride);
            EVE_Hal_wrMem(u->hal, addr + y * eve_stride, row_buf, eve_stride);
        }
        lv_free(row_buf);
        return true;
    }

    /* Aligned source stride case */
    if(src_stride_align > 0) {
        uint32_t src_stride = ALIGN_UP(natural_stride, src_stride_align);
        for(uint32_t y = 0; y < height; y++) {
            lv_memcpy(row_buf, src + y * src_stride, natural_stride);
            EVE_Hal_wrMem(u->hal, addr + y * eve_stride, row_buf, eve_stride);
        }
        lv_free(row_buf);
        return true;
    }

    /* Complex case: need to repack nibbles (4bpp, src_stride_align == 0 and odd width) */
    if(bpp == 4) {
        uint32_t src_i = 0;
        uint8_t key = 0;

        for(uint32_t y = 0; y < height; y++) {
            lv_memzero(row_buf, eve_stride);  /* Clear for each row */

            uint32_t row_i;
            for(row_i = 0; row_i < (width / 2); ++row_i) {
                uint8_t n1, n2;
                if(key == 0) {
                    n1 = GET_NIBBLE_HI(src[src_i]);
                    n2 = GET_NIBBLE_LO(src[src_i]);
                }
                else {
                    n1 = GET_NIBBLE_LO(src[src_i - 1]);
                    n2 = GET_NIBBLE_HI(src[src_i]);
                }
                row_buf[row_i] = (n1 << 4) | n2;
                src_i++;
            }

            /* Last nibble if odd width */
            if(width % 2 != 0) {
                row_buf[row_i] = (key == 0) ?
                                 (GET_NIBBLE_HI(src[src_i]) << 4) :
                                 (GET_NIBBLE_LO(src[src_i - 1]) << 4);
            }

            key = (key == 0) ? 1 : 0;
            src_i += (key == 1) ? 1 : 0;

            EVE_Hal_wrMem(u->hal, addr + y * eve_stride, row_buf, eve_stride);
        }

        lv_free(row_buf);
        return true;
    }

    /* For 1bpp and 2bpp with unaligned bit streams, fall back to row copy
     * (sub-byte formats with stride==0 pack bits continuously across rows,
     * but LVGL built-in fonts typically use stride==1 or aligned strides) */
    for(uint32_t y = 0; y < height; y++) {
        lv_memcpy(row_buf, src + y * natural_stride, natural_stride);
        EVE_Hal_wrMem(u->hal, addr + y * eve_stride, row_buf, eve_stride);
    }

    lv_free(row_buf);
    return true;
}

/* Map LVGL font bpp to EVE bitmap format */
static uint32_t bpp_to_eve_format(uint8_t bpp)
{
    switch(bpp) {
        case 1: return L1;
        case 2: return L2;
        case 4: return L4;
        case 8: return L8;
        default: return L4;
    }
}

/* Check if a font's bpp is supported */
static bool is_bpp_supported(uint8_t bpp)
{
    return bpp == 1 || bpp == 2 || bpp == 4 || bpp == 8;
}

static uint32_t upload_glyph(lv_draw_eve5_unit_t *u, const lv_font_fmt_txt_dsc_t *font_dsc,
                              uint32_t gid, uint16_t *out_stride)
{
    const lv_font_fmt_txt_glyph_dsc_t *glyph_dsc = &font_dsc->glyph_dsc[gid];
    const uint8_t *glyph_bitmap = &font_dsc->glyph_bitmap[glyph_dsc->bitmap_index];

    uint16_t g_w = glyph_dsc->box_w;
    uint16_t g_h = glyph_dsc->box_h;
    uint8_t bpp = font_dsc->bpp;

    /* Stride = ceil(width * bpp / 8), aligned to 4 bytes for optimal memory access */
    uint16_t g_stride_natural = (g_w * bpp + 7) / 8;
    uint16_t g_stride = ALIGN_UP(g_stride_natural, 4);
    uint32_t glyph_size = g_stride * g_h;

    *out_stride = g_stride;

    /* Check cache first */
    uint32_t cached_addr = lv_draw_eve5_glyph_cache_lookup(u, glyph_bitmap, out_stride);
    if(cached_addr != GA_INVALID) {
        return cached_addr;
    }

    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, glyph_size, GA_ALIGN_4);
    uint32_t ram_g_addr = Esd_GpuAlloc_Get(u->allocator, handle);

    if(ram_g_addr == GA_INVALID) {
        return GA_INVALID;
    }

    /* Upload glyph data with aligned stride */
    if(!glyph_bitmap_to_ramg_aligned(u, ram_g_addr, glyph_bitmap, g_w, g_h,
                                      g_stride, font_dsc->stride, bpp)) {
        Esd_GpuAlloc_Free(u->allocator, handle);
        return GA_INVALID;
    }

    /* Insert into cache */
    lv_draw_eve5_glyph_cache_insert(u, glyph_bitmap, handle, g_w, g_h, g_stride);

    return ram_g_addr;
}

/**********************
 * GLYPH TRANSFORM + DRAW
 **********************/

/**
 * Emit a glyph vertex with optional affine transform.
 * Shared by both normal and alpha glyph callbacks.
 * Caller must have already set bitmapSource, bitmapLayout, and color/alpha.
 */
static void emit_glyph_vertex(lv_draw_eve5_unit_t *u, lv_layer_t *layer,
                               lv_draw_task_t *t, lv_draw_glyph_dsc_t *glyph_dsc,
                               const lv_draw_letter_dsc_t *letter_dsc,
                               uint16_t g_w, uint16_t g_h,
                               int32_t x, int32_t y)
{
    bool has_letter_transform = (letter_dsc != NULL)
        && (letter_dsc->rotation != 0
            || letter_dsc->scale_x != LV_SCALE_NONE
            || letter_dsc->scale_y != LV_SCALE_NONE
            || letter_dsc->skew_x != 0
            || letter_dsc->skew_y != 0);
    bool has_label_rotation = (letter_dsc == NULL)
        && (glyph_dsc->rotation % 3600 != 0);

    if(has_letter_transform) {
        int32_t draw_vx = t->clip_area.x1 - layer->buf_area.x1;
        int32_t draw_vy = t->clip_area.y1 - layer->buf_area.y1;

        image_skew_t xform;
        if(!compute_image_skew(&xform,
                               letter_dsc->rotation, letter_dsc->scale_x, letter_dsc->scale_y,
                               letter_dsc->skew_x, letter_dsc->skew_y,
                               letter_dsc->pivot.x, letter_dsc->pivot.y,
                               g_w, g_h, x, y, draw_vx, draw_vy))
            return;

        EVE_CoDl_saveContext(u->hal);
        apply_image_skew(u->hal, &xform, NEAREST, 0, 0);
        EVE_CoDl_vertex2f_0(u->hal, draw_vx, draw_vy);
        EVE_CoDl_restoreContext(u->hal);
    }
    else if(has_label_rotation) {
        /* LABEL rotation: pivot.x from glyph_dsc (0 for labels),
         * pivot.y = box_h + ofs_y (matching SW renderer convention). */
        int32_t draw_vx = t->clip_area.x1 - layer->buf_area.x1;
        int32_t draw_vy = t->clip_area.y1 - layer->buf_area.y1;
        int32_t pivot_x = glyph_dsc->pivot.x;
        int32_t pivot_y = glyph_dsc->g->box_h + glyph_dsc->g->ofs_y;

        image_skew_t xform;
        if(!compute_image_skew(&xform,
                               glyph_dsc->rotation, LV_SCALE_NONE, LV_SCALE_NONE,
                               0, 0,
                               pivot_x, pivot_y,
                               g_w, g_h, x, y, draw_vx, draw_vy))
            return;

        EVE_CoDl_saveContext(u->hal);
        apply_image_skew(u->hal, &xform, NEAREST, 0, 0);
        EVE_CoDl_vertex2f_0(u->hal, draw_vx, draw_vy);
        EVE_CoDl_restoreContext(u->hal);
    }
    else {
        EVE_CoDl_bitmapSize(u->hal, NEAREST, BORDER, BORDER, g_w, g_h);
        EVE_CoDl_vertex2f_0(u->hal, x, y);
    }
}

/**********************
 * LABEL DRAWING
 **********************/

static void draw_glyph_cb(lv_draw_task_t *t, lv_draw_glyph_dsc_t *glyph_dsc,
                          lv_draw_fill_dsc_t *fill_dsc, const lv_area_t *fill_area)
{
    lv_draw_eve5_unit_t *u = s_current_unit;
    lv_layer_t *layer = s_current_layer;

    if(u == NULL || layer == NULL) return;

    /* Draw underline/strikethrough */
    if(fill_dsc && fill_area) {
        int32_t x1 = fill_area->x1 - layer->buf_area.x1;
        int32_t y1 = fill_area->y1 - layer->buf_area.y1;
        int32_t x2 = fill_area->x2 - layer->buf_area.x1;
        int32_t y2 = fill_area->y2 - layer->buf_area.y1;

        EVE_CoDl_begin(u->hal, RECTS);
        EVE_CoDl_vertex2f_0(u->hal, x1, y1);
        EVE_CoDl_vertex2f_0(u->hal, x2, y2);
        EVE_CoDl_end(u->hal);
    }

    if(glyph_dsc == NULL) return;

    const lv_font_t *font = glyph_dsc->g->resolved_font;

    if(!font) {
		LV_LOG_WARN("EVE5: Font not resolved");
        return;
    }

    /* Check font compatibility */
    if(font->get_glyph_bitmap != lv_font_get_bitmap_fmt_txt) {
        LV_LOG_WARN("EVE5: Only static fonts supported");
        return;
    }

    const lv_font_fmt_txt_dsc_t *font_dsc = font->dsc;
    if(!is_bpp_supported(font_dsc->bpp)) {
        LV_LOG_WARN("EVE5: Unsupported font bpp: %d", font_dsc->bpp);
        return;
    }

    uint32_t gid = glyph_dsc->g->gid.index;
    const lv_font_fmt_txt_glyph_dsc_t *g_dsc = &font_dsc->glyph_dsc[gid];

    uint16_t g_w = g_dsc->box_w;
    uint16_t g_h = g_dsc->box_h;
    uint16_t g_stride;

    uint32_t ram_g_addr = upload_glyph(u, font_dsc, gid, &g_stride);
    if(ram_g_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to upload glyph");
        return;
    }

    int32_t x = glyph_dsc->letter_coords->x1 - layer->buf_area.x1;
    int32_t y = glyph_dsc->letter_coords->y1 - layer->buf_area.y1;

    EVE_CoDl_colorRgb(u->hal, glyph_dsc->color.red, glyph_dsc->color.green, glyph_dsc->color.blue);
    EVE_CoDl_colorA(u->hal, glyph_dsc->opa);

    EVE_CoDl_bitmapSource(u->hal, ram_g_addr);
    EVE_CoDl_bitmapLayout(u->hal, bpp_to_eve_format(font_dsc->bpp), g_stride, g_h);

    emit_glyph_vertex(u, layer, t, glyph_dsc, s_current_letter_dsc, g_w, g_h, x, y);
}

void lv_draw_eve5_hal_draw_label(lv_draw_eve5_unit_t *u, lv_draw_task_t *t)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t *layer = t->target_layer;
    lv_draw_label_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->text == NULL || dsc->text[0] == '\0') return;

    /* Check if font is compatible with bitmap rendering */
    bool use_bitmap_font = false;
    if(dsc->font && dsc->font->get_glyph_bitmap == lv_font_get_bitmap_fmt_txt) {
        const lv_font_fmt_txt_dsc_t *font_dsc = dsc->font->dsc;
        if(is_bpp_supported(font_dsc->bpp)) {
            use_bitmap_font = true;
        }
    }

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(u->hal, dsc->opa);

    if(use_bitmap_font) {
        /* Use LVGL's character iteration with our glyph callback.
         * Bitmap transform may be non-identity from a previous draw. */
        EVE_CoDl_bitmapTransform_identity(u->hal);
        EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
        EVE_CoDl_begin(u->hal, BITMAPS);

        /* Store context for callback (LVGL is single-threaded) */
        s_current_unit = u;
        s_current_layer = layer;

        lv_draw_label_iterate_characters(t, dsc, &t->area, draw_glyph_cb);

        s_current_unit = NULL;
        s_current_layer = NULL;

        EVE_CoDl_end(u->hal);
    }
    else {
        /* Fallback to EVE ROM fonts */
        int32_t x = t->area.x1 - layer->buf_area.x1;
        int32_t y = t->area.y1 - layer->buf_area.y1;

        uint8_t eve_font = 31;
        if(dsc->font) {
            int32_t font_h = lv_font_get_line_height(dsc->font);
            if(font_h <= 8) eve_font = 16;
            else if(font_h <= 13) eve_font = 26;
            else if(font_h <= 16) eve_font = 27;
            else if(font_h <= 20) eve_font = 29;
            else if(font_h <= 25) eve_font = 30;
            else eve_font = 31;
        }

        EVE_CoCmd_text(u->hal, x, y, eve_font, 0, dsc->text);
    }
}

void lv_draw_eve5_hal_draw_letter(lv_draw_eve5_unit_t *u, lv_draw_task_t *t)
{
    lv_layer_t *layer = t->target_layer;
    lv_draw_letter_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(u->hal, dsc->opa);
    /* Bitmap transform may be non-identity from a previous draw */
    EVE_CoDl_bitmapTransform_identity(u->hal);
    EVE_CoDl_bitmapHandle(u->hal, u->hal->CoScratchHandle);

    if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE) {
        EVE_CoDl_blendFunc(u->hal, SRC_ALPHA, ONE);
    }

    EVE_CoDl_begin(u->hal, BITMAPS);

    s_current_unit = u;
    s_current_layer = layer;
    s_current_letter_dsc = dsc;

    lv_draw_glyph_dsc_t glyph_dsc;
    lv_draw_glyph_dsc_init(&glyph_dsc);
    glyph_dsc.opa = dsc->opa;
    glyph_dsc.bg_coords = NULL;
    glyph_dsc.color = dsc->color;
    glyph_dsc.rotation = dsc->rotation;
    glyph_dsc.pivot = dsc->pivot;

    lv_draw_unit_draw_letter(t, &glyph_dsc, &(lv_point_t) {
        .x = t->area.x1, .y = t->area.y1
    }, dsc->font, dsc->unicode, draw_glyph_cb);

    s_current_unit = NULL;
    s_current_layer = NULL;
    s_current_letter_dsc = NULL;

    EVE_CoDl_end(u->hal);

    if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE) {
        EVE_CoDl_blendFunc_default(u->hal);
    }

    if(glyph_dsc._draw_buf) {
        lv_draw_buf_destroy(glyph_dsc._draw_buf);
        glyph_dsc._draw_buf = NULL;
    }
}

/**********************
 * ALPHA PASS — GLYPH CALLBACK
 **********************/

static void alpha_glyph_cb(lv_draw_task_t *t, lv_draw_glyph_dsc_t *glyph_dsc,
                            lv_draw_fill_dsc_t *fill_dsc, const lv_area_t *fill_area)
{
    lv_draw_eve5_unit_t *u = s_alpha_unit;
    lv_layer_t *layer = s_alpha_layer;

    if(u == NULL || layer == NULL) return;

    /* Underline/strikethrough fills */
    if(fill_dsc && fill_area) {
        int32_t x1 = fill_area->x1 - layer->buf_area.x1;
        int32_t y1 = fill_area->y1 - layer->buf_area.y1;
        int32_t x2 = fill_area->x2 - layer->buf_area.x1;
        int32_t y2 = fill_area->y2 - layer->buf_area.y1;

        EVE_CoDl_begin(u->hal, RECTS);
        EVE_CoDl_vertex2f_0(u->hal, x1, y1);
        EVE_CoDl_vertex2f_0(u->hal, x2, y2);
        EVE_CoDl_end(u->hal);
    }

    if(glyph_dsc == NULL) return;

    const lv_font_t *font = glyph_dsc->g->resolved_font;
    if(font->get_glyph_bitmap != lv_font_get_bitmap_fmt_txt) return;

    const lv_font_fmt_txt_dsc_t *font_dsc = font->dsc;
    if(!is_bpp_supported(font_dsc->bpp)) return;

    uint32_t gid = glyph_dsc->g->gid.index;
    const lv_font_fmt_txt_glyph_dsc_t *g_dsc = &font_dsc->glyph_dsc[gid];
    const uint8_t *glyph_bitmap = &font_dsc->glyph_bitmap[g_dsc->bitmap_index];

    uint16_t g_w = g_dsc->box_w;
    uint16_t g_h = g_dsc->box_h;

    /* Look up glyph in cache — it was uploaded during the normal pass */
    uint16_t g_stride;
    uint32_t ram_g_addr = lv_draw_eve5_glyph_cache_lookup(u, glyph_bitmap, &g_stride);
    if(ram_g_addr == GA_INVALID) return;

    int32_t x = glyph_dsc->letter_coords->x1 - layer->buf_area.x1;
    int32_t y = glyph_dsc->letter_coords->y1 - layer->buf_area.y1;

    /* Alpha only — set alpha to glyph opacity (no color needed) */
    EVE_CoDl_colorA(u->hal, glyph_dsc->opa);

    EVE_CoDl_bitmapSource(u->hal, ram_g_addr);
    EVE_CoDl_bitmapLayout(u->hal, bpp_to_eve_format(font_dsc->bpp), g_stride, g_h);

    emit_glyph_vertex(u, layer, t, glyph_dsc, s_alpha_letter_dsc, g_w, g_h, x, y);
}

/**********************
 * ALPHA PASS — LABEL/LETTER
 **********************/

void lv_draw_eve5_alpha_draw_label(lv_draw_eve5_unit_t *u, lv_draw_task_t *t)
{
    lv_layer_t *layer = t->target_layer;
    lv_draw_label_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->text == NULL || dsc->text[0] == '\0') return;

    /* Check if font is compatible with bitmap rendering */
    bool use_bitmap_font = false;
    if(dsc->font && dsc->font->get_glyph_bitmap == lv_font_get_bitmap_fmt_txt) {
        const lv_font_fmt_txt_dsc_t *font_dsc = dsc->font->dsc;
        if(is_bpp_supported(font_dsc->bpp)) {
            use_bitmap_font = true;
        }
    }

    if(!use_bitmap_font) {
        /* ROM font: re-issue EVE_CoCmd_text() with same parameters as normal draw.
         * CMD_TEXT generates display list entries that respect current colorMask/blend,
         * so the alpha pass context (colorMask(0,0,0,1), blend(ONE, ONE_MINUS_SRC_ALPHA))
         * produces correct per-glyph alpha coverage from the ROM font bitmaps. */
        lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

        int32_t x = t->area.x1 - layer->buf_area.x1;
        int32_t y = t->area.y1 - layer->buf_area.y1;

        uint8_t eve_font = 31;
        if(dsc->font) {
            int32_t font_h = lv_font_get_line_height(dsc->font);
            if(font_h <= 8) eve_font = 16;
            else if(font_h <= 13) eve_font = 26;
            else if(font_h <= 16) eve_font = 27;
            else if(font_h <= 20) eve_font = 29;
            else if(font_h <= 25) eve_font = 30;
            else eve_font = 31;
        }

        EVE_CoDl_colorA(u->hal, dsc->opa);
        EVE_CoCmd_text(u->hal, x, y, eve_font, 0, dsc->text);
        return;
    }

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorA(u->hal, dsc->opa);
    /* Bitmap transform may be non-identity from a previous draw */
    EVE_CoDl_bitmapTransform_identity(u->hal);
    EVE_CoDl_bitmapHandle(u->hal, u->hal->CoScratchHandle);
    EVE_CoDl_begin(u->hal, BITMAPS);

    s_alpha_unit = u;
    s_alpha_layer = layer;

    lv_draw_label_iterate_characters(t, dsc, &t->area, alpha_glyph_cb);

    s_alpha_unit = NULL;
    s_alpha_layer = NULL;

    EVE_CoDl_end(u->hal);
}

void lv_draw_eve5_alpha_draw_letter(lv_draw_eve5_unit_t *u, lv_draw_task_t *t)
{
    lv_layer_t *layer = t->target_layer;
    lv_draw_letter_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorA(u->hal, dsc->opa);
    /* Bitmap transform may be non-identity from a previous draw */
    EVE_CoDl_bitmapTransform_identity(u->hal);
    EVE_CoDl_bitmapHandle(u->hal, u->hal->CoScratchHandle);
    EVE_CoDl_begin(u->hal, BITMAPS);

    s_alpha_unit = u;
    s_alpha_layer = layer;
    s_alpha_letter_dsc = dsc;

    lv_draw_glyph_dsc_t glyph_dsc;
    lv_draw_glyph_dsc_init(&glyph_dsc);
    glyph_dsc.opa = dsc->opa;
    glyph_dsc.bg_coords = NULL;
    glyph_dsc.color = dsc->color;
    glyph_dsc.rotation = dsc->rotation;
    glyph_dsc.pivot = dsc->pivot;

    lv_draw_unit_draw_letter(t, &glyph_dsc, &(lv_point_t) {
        .x = t->area.x1, .y = t->area.y1
    }, dsc->font, dsc->unicode, alpha_glyph_cb);

    s_alpha_unit = NULL;
    s_alpha_layer = NULL;
    s_alpha_letter_dsc = NULL;

    EVE_CoDl_end(u->hal);

    if(glyph_dsc._draw_buf) {
        lv_draw_buf_destroy(glyph_dsc._draw_buf);
        glyph_dsc._draw_buf = NULL;
    }
}

#endif /* LV_USE_DRAW_EVE5 */
