/**
 * @file lv_draw_eve5_box_shadow.c
 *
 * Box shadow rendering for EVE5/BT820 using pre-generated Gaussian textures.
 *
 * == 9-Slice Shadow Layout ==
 *
 * The shadow is rendered as 9 regions around the widget:
 *
 *     +------+----------------+------+
 *     |  TL  |      TOP       |  TR  |  <- corner_size height
 *     +------+----------------+------+
 *     |      |                |      |
 *     | LEFT |     CENTER     | RIGHT|  <- solid fill
 *     |      |                |      |
 *     +------+----------------+------+
 *     |  BL  |     BOTTOM     |  BR  |  <- corner_size height
 *     +------+----------------+------+
 *        ^                        ^
 *     corner_size              corner_size
 *
 * - Corners use a 2D texture (64x64 L8) for rounded Gaussian falloff
 * - Edges use a 1D texture (64x1 L8) stretched along the edge
 * - Center is a solid fill at full opacity
 *
 * == Shape ==
 *
 * Start with the core rectangle: the widget expanded by the shadow spread,
 * translated by the shadow offset, and rounded to the shadow radius. Blur
 * its boundary over the shadow width, as in LVGL's software renderer.
 * Coverage is roughly half at the core edge, fading to zero outside and full
 * coverage inside over a distance of blur = width / 2 in each direction.
 * Corner and edge slices span corner_size = 2 * blur + radius, from the
 * shadow's outer edge to its solid interior. If the shadow is too narrow for
 * two full slices, divide the available width between them.
 *
 * == Texture Layout ==
 *
 *     (0,0) transparent -----> X
 *       |
 *       v    +---------+
 *       Y    |    .--' |  <- falloff around the core's edge
 *            |  .'     |     (approximately blur units from the outer edge)
 *            | |  solid|
 *            +---------+
 *                    (SIZE-1, SIZE-1) = alpha 255
 *
 * == Blur ==
 *
 * The software renderer applies two box blurs along each axis of the core's
 * corner buffer. Each filter is approximately blur pixels wide. The textures
 * approximate this at their own resolution, using fractional filter widths.
 * This separable box blur produces squarer corners than a Gaussian blur.
 *
 * Each even-width filter window is offset outward by half a pixel. Mirroring
 * the corner buffer to all four corners shifts the shadow inward by half a
 * pixel per even-width pass. Compensate by sampling the textures farther
 * outward. For shadow widths below 4, the software renderer skips blurring;
 * draw the core as a rounded rectangle instead.
 *
 * The filter width and core edge position were fitted to the software renderer
 * across multiple sizes, radii, and shadow widths using tests/tools/shadow_fit
 * in eve_apps. The fitted filter width is 0.94 * blur, and the core edge is
 * 0.965 * blur from the texture's outer edge.
 *
 * == Texture Caching ==
 *
 * Textures are cached by ratio_idx = (radius × 63) / corner_size, giving
 * 64 possible texture pairs. Same ratio = same visual proportions at any size.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

/*********************
 *      DEFINES
 *********************/

#define SHADOW_TEX_SIZE      EVE5_SHADOW_TEX_SIZE
#define SHADOW_BITMAP_HANDLE EVE_CO_SCRATCH_HANDLE
#define SHADOW_BOX_PERMILLE  940   /* Box width, in thousandths of the blur */
#define SHADOW_EDGE_PERMILLE 965   /* Core's edge from the outer end, in thousandths of the blur */
#define SHADOW_MIN_BLURRED   4     /* Narrower shadows are the core, as in the software renderer */
#define SHADOW_COV_ONE       4096  /* Coverage 1.0 while filtering (Q12) */

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool generate_corner_texture(uint8_t * buf, int32_t ratio_idx);
static void generate_edge_texture(uint8_t * buf, int32_t ratio_idx);
static bool ensure_shadow_textures(lv_draw_eve5_unit_t * u, int32_t ratio_idx);
static int32_t calc_ratio_index(int32_t radius, int32_t corner_size);

/**********************
 *   TEXTURE GENERATION
 **********************/

static int32_t calc_ratio_index(int32_t radius, int32_t corner_size)
{
    if(corner_size <= 0) return 0;
    if(radius <= 0) return 0;
    if(radius >= corner_size) return SHADOW_TEX_SIZE - 1;

    return (radius * (SHADOW_TEX_SIZE - 1)) / corner_size;
}

/* The textures span corner_size pixels in SHADOW_TEX_SIZE texels. In 1/256
 * texel from the outer end: the radius, the core's edge, and the box width */
static void texture_shape(int32_t ratio_idx, int32_t * radius_256, int32_t * edge_256, int32_t * box_256)
{
    *radius_256 = ratio_idx * SHADOW_TEX_SIZE * 256 / (SHADOW_TEX_SIZE - 1);
    int32_t blur_256 = (SHADOW_TEX_SIZE * 256 - *radius_256) / 2;
    *edge_256 = blur_256 * SHADOW_EDGE_PERMILLE / 1000;
    *box_256 = blur_256 * SHADOW_BOX_PERMILLE / 1000;
}

/*
 * Integrate a line of Q12 texels from 0 to t, where t is in 1/256 texel units. The result is in Q12
 * times 1/256 units. Beyond either end of the line, repeat the endpoint texel value.
 */
static int32_t line_integral(const int32_t * v, const int32_t * prefix, int32_t t)
{
    if(t <= 0) return t * v[0];
    if(t >= SHADOW_TEX_SIZE * 256) return prefix[SHADOW_TEX_SIZE] * 256 + (t - SHADOW_TEX_SIZE * 256) * v[SHADOW_TEX_SIZE - 1];
    return prefix[t >> 8] * 256 + (t & 255) * v[t >> 8];
}

/* Box filter of width_256 (1/256 texel) over a line of texels, in place */
static void box_filter_line(int32_t * v, int32_t width_256)
{
    int32_t prefix[SHADOW_TEX_SIZE + 1];
    int32_t out[SHADOW_TEX_SIZE];
    if(width_256 <= 0) return;
    prefix[0] = 0;
    for(int32_t i = 0; i < SHADOW_TEX_SIZE; i++) prefix[i + 1] = prefix[i] + v[i];
    for(int32_t i = 0; i < SHADOW_TEX_SIZE; i++) {
        int32_t a = i * 256 + 128 - width_256 / 2;
        int32_t b = a + width_256;
        out[i] = (line_integral(v, prefix, b) - line_integral(v, prefix, a) + width_256 / 2) / width_256;
    }
    lv_memcpy(v, out, sizeof(out));
}

static uint8_t coverage_to_alpha(int32_t v)
{
    return (uint8_t)((v * 255 + SHADOW_COV_ONE / 2) / SHADOW_COV_ONE);
}

/**
 * Generate a 2D texture for the shadow's corner falloff. The outer corner is at (0, 0), and the
 * rounded corner's center is toward (SIZE-1, SIZE-1). Estimate core coverage with 4 by 4 samples
 * per texel, then apply two box blurs along each axis. Extend each row and column beyond the
 * texture by repeating its endpoint value.
 */
static bool generate_corner_texture(uint8_t * buf, int32_t ratio_idx)
{
    int32_t radius_256, edge_256, box_256;
    texture_shape(ratio_idx, &radius_256, &edge_256, &box_256);
    int32_t centre_256 = edge_256 + radius_256;

    int16_t * g = lv_malloc(SHADOW_TEX_SIZE * SHADOW_TEX_SIZE * sizeof(int16_t));
    if(g == NULL) return false;

    for(int32_t y = 0; y < SHADOW_TEX_SIZE; y++) {
        for(int32_t x = 0; x < SHADOW_TEX_SIZE; x++) {
            int32_t n = 0;
            for(int32_t sy = 0; sy < 4; sy++) {
                for(int32_t sx = 0; sx < 4; sx++) {
                    int32_t px = x * 256 + sx * 64 + 32;
                    int32_t py = y * 256 + sy * 64 + 32;
                    int32_t dx = centre_256 - px;
                    int32_t dy = centre_256 - py;
                    if(dx > 0 && dy > 0) n += dx * dx + dy * dy <= radius_256 * radius_256;
                    else n += px >= edge_256 && py >= edge_256;
                }
            }
            g[y * SHADOW_TEX_SIZE + x] = (int16_t)(n * (SHADOW_COV_ONE / 16));
        }
    }

    int32_t line[SHADOW_TEX_SIZE];
    for(int32_t pass = 0; pass < 2; pass++) {
        for(int32_t y = 0; y < SHADOW_TEX_SIZE; y++) {
            for(int32_t x = 0; x < SHADOW_TEX_SIZE; x++) line[x] = g[y * SHADOW_TEX_SIZE + x];
            box_filter_line(line, box_256);
            for(int32_t x = 0; x < SHADOW_TEX_SIZE; x++) g[y * SHADOW_TEX_SIZE + x] = (int16_t)line[x];
        }
        for(int32_t x = 0; x < SHADOW_TEX_SIZE; x++) {
            for(int32_t y = 0; y < SHADOW_TEX_SIZE; y++) line[y] = g[y * SHADOW_TEX_SIZE + x];
            box_filter_line(line, box_256);
            for(int32_t y = 0; y < SHADOW_TEX_SIZE; y++) g[y * SHADOW_TEX_SIZE + x] = (int16_t)line[y];
        }
    }

    for(int32_t i = 0; i < SHADOW_TEX_SIZE * SHADOW_TEX_SIZE; i++) buf[i] = coverage_to_alpha(g[i]);
    lv_free(g);
    return true;
}

/**
 * Generate 1D edge texture for straight shadow edges: transparent at x = 0,
 * solid at x = SIZE-1, the same falloff across the core's edge as the corner.
 */
static void generate_edge_texture(uint8_t * buf, int32_t ratio_idx)
{
    int32_t radius_256, edge_256, box_256;
    texture_shape(ratio_idx, &radius_256, &edge_256, &box_256);

    int32_t line[SHADOW_TEX_SIZE];
    for(int32_t x = 0; x < SHADOW_TEX_SIZE; x++) {
        int32_t n = 0;
        for(int32_t sx = 0; sx < 4; sx++) n += x * 256 + sx * 64 + 32 >= edge_256;
        line[x] = n * (SHADOW_COV_ONE / 4);
    }
    box_filter_line(line, box_256);
    box_filter_line(line, box_256);
    for(int32_t x = 0; x < SHADOW_TEX_SIZE; x++) buf[x] = coverage_to_alpha(line[x]);
}

/**
 * Ensure shadow textures exist for the given ratio index.
 * Generates and uploads if not present or if evicted by GPU allocator.
 */
static bool ensure_shadow_textures(lv_draw_eve5_unit_t * u, int32_t ratio_idx)
{
    if(ratio_idx < 0 || ratio_idx >= SHADOW_TEX_SIZE) {
        LV_LOG_ERROR("EVE5: Invalid shadow ratio index %"PRId32, ratio_idx);
        return false;
    }

    lv_draw_eve5_shadow_slot_t * slot = &u->shadow_slots[ratio_idx];

    /* Corner texture (64×64 L8 = 4KB). GC-flagged: this function regenerates
     * the texture whenever the handle goes invalid (sweep on pre-BT820,
     * pressure eviction on BT820+). */
    uint32_t corner_addr = EVE_GpuAlloc_Get(u->allocator, slot->corner_handle);
    if(corner_addr == GA_INVALID) {
        uint32_t corner_bytes = SHADOW_TEX_SIZE * SHADOW_TEX_SIZE;

        slot->corner_handle = EVE_GpuAlloc_Alloc(u->allocator, corner_bytes, GA_GC_FLAG);
        corner_addr = EVE_GpuAlloc_Get(u->allocator, slot->corner_handle);

        if(corner_addr == GA_INVALID) {
            LV_LOG_ERROR("EVE5: Failed to allocate shadow corner texture");
            return false;
        }

        uint8_t * buf = lv_malloc(corner_bytes);
        if(!buf) {
            LV_LOG_ERROR("EVE5: Failed to allocate corner generation buffer");
            EVE_GpuAlloc_Free(u->allocator, slot->corner_handle);
            slot->corner_handle = GA_HANDLE_INVALID;
            return false;
        }

        if(!generate_corner_texture(buf, ratio_idx)) {
            LV_LOG_ERROR("EVE5: Failed to allocate corner generation buffer");
            lv_free(buf);
            EVE_GpuAlloc_Free(u->allocator, slot->corner_handle);
            slot->corner_handle = GA_HANDLE_INVALID;
            return false;
        }
        EVE_Hal_wrMem(u->hal, corner_addr, buf, corner_bytes);
        lv_free(buf);
        EVE_Hal_requestFenceBeforeSwap(u->hal);

        LV_LOG_INFO("EVE5: Generated shadow corner texture for ratio %"PRId32" at 0x%08"PRIx32,
                    ratio_idx, corner_addr);
    }

    /* Edge texture (64×1 L8 = 64 bytes, aligned to 4). GC-flagged like the
     * corner texture above. */
    uint32_t edge_addr = EVE_GpuAlloc_Get(u->allocator, slot->edge_handle);
    if(edge_addr == GA_INVALID) {
        uint32_t edge_bytes = ALIGN_UP(SHADOW_TEX_SIZE, 4);

        slot->edge_handle = EVE_GpuAlloc_Alloc(u->allocator, edge_bytes, GA_GC_FLAG);
        edge_addr = EVE_GpuAlloc_Get(u->allocator, slot->edge_handle);

        if(edge_addr == GA_INVALID) {
            LV_LOG_ERROR("EVE5: Failed to allocate shadow edge texture");
            return false;
        }

        uint8_t * buf = lv_malloc(edge_bytes);
        if(!buf) {
            LV_LOG_ERROR("EVE5: Failed to allocate edge generation buffer");
            EVE_GpuAlloc_Free(u->allocator, slot->edge_handle);
            slot->edge_handle = GA_HANDLE_INVALID;
            return false;
        }

        lv_memzero(buf, edge_bytes);
        generate_edge_texture(buf, ratio_idx);
        EVE_Hal_wrMem(u->hal, edge_addr, buf, edge_bytes);
        lv_free(buf);
        EVE_Hal_requestFenceBeforeSwap(u->hal);

        LV_LOG_INFO("EVE5: Generated shadow edge texture for ratio %"PRId32" at 0x%08"PRIx32,
                    ratio_idx, edge_addr);
    }

    return true;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_eve5_box_shadow_init(lv_draw_eve5_unit_t * u)
{
    for(int32_t i = 0; i < SHADOW_TEX_SIZE; i++) {
        u->shadow_slots[i].corner_handle = GA_HANDLE_INVALID;
        u->shadow_slots[i].edge_handle = GA_HANDLE_INVALID;
    }
}

/**
 * Exclude the widget's interior from the shadow using the stencil buffer. Shrink the widget's
 * rounded rectangle by one pixel so the shadow extends under its antialiased edge, matching the
 * software renderer. Leave the stencil test enabled to draw only outside that rectangle; the caller
 * restores the saved stencil state.
 * The stencil has no antialiasing. Use it only in the direct-to-alpha pass, where the alpha channel
 * cannot also serve as a temporary mask (see draw_shadow_outside_widget).
 */
static void exclude_widget_area(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                                const lv_draw_box_shadow_dsc_t * dsc)
{
    EVE_HalContext * phost = u->hal;
    lv_layer_t * layer = t->target_layer;
    lv_area_t bg = t->area;
    lv_area_increase(&bg, -1, -1);
    int32_t lx = layer->buf_area.x1;
    int32_t ly = layer->buf_area.y1;

    int32_t r_bg = dsc->radius;
    int32_t short_side = LV_MIN(lv_area_get_width(&bg), lv_area_get_height(&bg));
    if(r_bg > short_side / 2) r_bg = short_side / 2;

    lv_draw_eve5_clear_stencil(u, t->_real_area.x1 - lx, t->_real_area.y1 - ly,
                               t->_real_area.x2 - lx, t->_real_area.y2 - ly,
                               &t->clip_area, &layer->buf_area);
    EVE_CoDl_saveContext(phost);
    EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
    EVE_CoDl_stencilOp(phost, KEEP, REPLACE);
    EVE_CoDl_stencilFunc(phost, ALWAYS, 255, 255);
    lv_draw_eve5_draw_rect(u, bg.x1 - lx, bg.y1 - ly, bg.x2 - lx, bg.y2 - ly, r_bg,
                           &t->clip_area, &layer->buf_area);
    EVE_CoDl_restoreContext(phost);
    EVE_CoDl_stencilOp(phost, KEEP, KEEP);
    EVE_CoDl_stencilFunc(phost, NOTEQUAL, 255, 255);
}

/* The 9 slices of a shadow, layer-local */
typedef struct {
    int32_t sx1, sy1, sx2, sy2;             /**< Shadow bounds, inclusive */
    int32_t corner_size;                    /**< Pixels the textures span */
    int32_t left, right, top, bottom;       /**< Widths of the slices on each side */
    int32_t ratio_idx;
    int32_t in_halves;                      /**< Inward shadow offset in half-pixel units */
    bool core_only;                         /**< Too narrow to blur: the core alone */
    int32_t cx1, cy1, cx2, cy2, radius;     /**< The core, inclusive, and its radius */
} shadow_slices_t;

static bool shadow_slices(const lv_draw_task_t * t, const lv_draw_box_shadow_dsc_t * dsc, shadow_slices_t * sl)
{
    const lv_area_t * coords = &t->area;
    const lv_area_t * layer_area = &t->target_layer->buf_area;

    /* Core area: widget bounds with offset and spread applied */
    lv_area_t core_area;
    core_area.x1 = coords->x1 + dsc->ofs_x - dsc->spread;
    core_area.x2 = coords->x2 + dsc->ofs_x + dsc->spread;
    core_area.y1 = coords->y1 + dsc->ofs_y - dsc->spread;
    core_area.y2 = coords->y2 + dsc->ofs_y + dsc->spread;

    int32_t r_sh = dsc->radius;
    int32_t short_side = LV_MIN(lv_area_get_width(&core_area), lv_area_get_height(&core_area));
    if(r_sh > short_side / 2) r_sh = short_side / 2;
    if(r_sh < 0) r_sh = 0;

    sl->cx1 = core_area.x1 - layer_area->x1;
    sl->cy1 = core_area.y1 - layer_area->y1;
    sl->cx2 = core_area.x2 - layer_area->x1;
    sl->cy2 = core_area.y2 - layer_area->y1;
    sl->radius = r_sh;
    sl->core_only = dsc->width < SHADOW_MIN_BLURRED;
    if(sl->core_only) {
        /* The shadow is the core */
        sl->sx1 = sl->cx1;
        sl->sy1 = sl->cy1;
        sl->sx2 = sl->cx2;
        sl->sy2 = sl->cy2;
        return true;
    }

    /*
     * The two software box blurs have widths width / 2 and (width + 1) / 2. Each even-width blur
     * shifts the shadow inward by half a pixel.
     */
    int32_t box1 = dsc->width >> 1;
    int32_t box2 = box1 + (dsc->width & 1);
    sl->in_halves = (box1 % 2 == 0) + (box2 % 2 == 0);

    int32_t blur = (dsc->width + 1) / 2;
    sl->corner_size = 2 * blur + r_sh;
    if(sl->corner_size <= 0) return false;
    sl->ratio_idx = calc_ratio_index(r_sh, sl->corner_size);

    sl->sx1 = core_area.x1 - blur - layer_area->x1;
    sl->sy1 = core_area.y1 - blur - layer_area->y1;
    sl->sx2 = core_area.x2 + blur - layer_area->x1;
    sl->sy2 = core_area.y2 + blur - layer_area->y1;

    /* A shadow narrower than two slices is split between them */
    int32_t w = sl->sx2 - sl->sx1 + 1;
    int32_t h = sl->sy2 - sl->sy1 + 1;
    sl->left = LV_MIN(sl->corner_size, w / 2);
    sl->right = LV_MIN(sl->corner_size, w - sl->left);
    sl->top = LV_MIN(sl->corner_size, h / 2);
    sl->bottom = LV_MIN(sl->corner_size, h - sl->top);
    return true;
}

/* One slice of the bitmap set up, with its texture transform (s8.8, 15.8) */
static void shadow_slice(EVE_HalContext * phost, int32_t x, int32_t y, int32_t w, int32_t h,
                         int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f)
{
    if(w <= 0 || h <= 0) return;
    EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, w, h);
    EVE_CoDl_bitmapTransformA(phost, a);
    EVE_CoDl_bitmapTransformB(phost, b);
    EVE_CoDl_bitmapTransformC(phost, c);
    EVE_CoDl_bitmapTransformD(phost, d);
    EVE_CoDl_bitmapTransformE(phost, e);
    EVE_CoDl_bitmapTransformF(phost, f);
    EVE_CoDl_vertex2f_0(phost, x, y);
}

/**
 * Draw the slices in the current color and opacity, within the scissor set to
 * the clip area. The textures are sampled from the shadow's outer edge, and
 * mirrored for the right and bottom slices.
 */
static void draw_shadow_slices(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, const shadow_slices_t * sl,
                               uint32_t corner_addr, uint32_t edge_addr)
{
    EVE_HalContext * phost = u->hal;
    lv_layer_t * layer = t->target_layer;

    int32_t scale = (SHADOW_TEX_SIZE * 256) / sl->corner_size;
    int32_t xl = sl->sx1 + sl->left;           /* Right of the left slices */
    int32_t xr = sl->sx2 + 1 - sl->right;      /* Left of the right slices */
    int32_t yt = sl->sy1 + sl->top;            /* Bottom of the top slices */
    int32_t yb = sl->sy2 + 1 - sl->bottom;     /* Top of the bottom slices */
    /*
     * Pixel and texel centers lie at integer coordinates, so each mirrored slice starts at the
     * texture coordinate of its outermost pixel. Offset both sides' texture coordinates outward to
     * compensate for the blur's inward shift.
     */
    int32_t in = sl->in_halves * (scale / 2);
    int32_t cn = -in;
    int32_t cr = scale * (sl->right - 1) - in;
    int32_t cb = scale * (sl->bottom - 1) - in;

    /* CORNERS */
    EVE_CoDl_bitmapHandle(phost, SHADOW_BITMAP_HANDLE);
    EVE_CoDl_bitmapSource(phost, corner_addr);
    EVE_CoDl_bitmapLayout(phost, L8, SHADOW_TEX_SIZE, SHADOW_TEX_SIZE);
    EVE_CoDl_begin(phost, BITMAPS);
    shadow_slice(phost, sl->sx1, sl->sy1, sl->left, sl->top, scale, 0, cn, 0, scale, cn);
    shadow_slice(phost, xr, sl->sy1, sl->right, sl->top, -scale, 0, cr, 0, scale, cn);
    shadow_slice(phost, sl->sx1, yb, sl->left, sl->bottom, scale, 0, cn, 0, -scale, cb);
    shadow_slice(phost, xr, yb, sl->right, sl->bottom, -scale, 0, cr, 0, -scale, cb);

    /* EDGES: the one-row texture across the edge, rotated for the top and
     * bottom */
    EVE_CoDl_bitmapSource(phost, edge_addr);
    EVE_CoDl_bitmapLayout(phost, L8, SHADOW_TEX_SIZE, 1);
    shadow_slice(phost, xl, sl->sy1, xr - xl, sl->top, 0, scale, cn, 0, 0, 0);
    shadow_slice(phost, xl, yb, xr - xl, sl->bottom, 0, -scale, cb, 0, 0, 0);
    shadow_slice(phost, sl->sx1, yt, sl->left, yb - yt, scale, 0, cn, 0, 0, 0);
    shadow_slice(phost, xr, yt, sl->right, yb - yt, -scale, 0, cr, 0, 0, 0);
    EVE_CoDl_end(phost);

    /* CENTER: solid, as a scissored rect to avoid EVE RECTS alpha artifacts */
    if(xr > xl && yb > yt) {
        lv_area_t center_screen;
        center_screen.x1 = xl + layer->buf_area.x1;
        center_screen.y1 = yt + layer->buf_area.y1;
        center_screen.x2 = xr + layer->buf_area.x1 - 1;
        center_screen.y2 = yb + layer->buf_area.y1 - 1;

        lv_area_t center_scissor;
        if(lv_area_intersect(&center_scissor, &center_screen, &t->clip_area)) {
            lv_draw_eve5_set_scissor(u, &center_scissor, &layer->buf_area);
            EVE_CoDl_lineWidth(phost, 16);
            EVE_CoDl_begin(phost, RECTS);
            EVE_CoDl_vertex2f_0(phost, xl - 1, yt - 1);
            EVE_CoDl_vertex2f_0(phost, xr, yb);
            EVE_CoDl_end(phost);
            lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);
        }
    }
}

/* The shadow in the current color and opacity: the slices, or the core alone */
static void draw_shadow(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, const shadow_slices_t * sl,
                        uint32_t corner_addr, uint32_t edge_addr)
{
    if(sl->core_only) {
        lv_draw_eve5_draw_rect(u, sl->cx1, sl->cy1, sl->cx2, sl->cy2, sl->radius, &t->clip_area,
                               &t->target_layer->buf_area);
    }
    else {
        draw_shadow_slices(u, t, sl, corner_addr, edge_addr);
    }
}

/**
 * Draw the shadow outside the widget, matching the software renderer. Build a mask in the alpha
 * channel over the shadow's area: clear alpha, draw the shadow coverage at the requested opacity,
 * then multiply by the inverse of the widget's antialiased rounded rectangle. Shrink that rectangle
 * by one pixel, as in lv_draw_sw_mask_radius_init with inversion enabled.
 * Draw through the completed mask using the shadow color in the RGB pass or white in the L8 alpha
 * pass. This overwrites the target's alpha channel. The caller must save the context and set the
 * vertex format.
 */
static void draw_shadow_outside_widget(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, const shadow_slices_t * sl,
                                       uint32_t corner_addr, uint32_t edge_addr, lv_color_t color)
{
    EVE_HalContext * phost = u->hal;
    const lv_draw_box_shadow_dsc_t * dsc = t->draw_dsc;
    lv_layer_t * layer = t->target_layer;
    lv_area_t bg = t->area;
    lv_area_increase(&bg, -1, -1);
    int32_t lx = layer->buf_area.x1;
    int32_t ly = layer->buf_area.y1;

    int32_t r_bg = dsc->radius;
    int32_t short_side = LV_MIN(lv_area_get_width(&bg), lv_area_get_height(&bg));
    if(r_bg > short_side / 2) r_bg = short_side / 2;

    /* Cleared. A rectangle without radius clips with the scissor, which it
     * leaves set. */
    EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
    EVE_CoDl_blendFunc(phost, ZERO, ZERO);
    lv_draw_eve5_draw_rect(u, sl->sx1, sl->sy1, sl->sx2, sl->sy2, 0, &t->clip_area, &layer->buf_area);
    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    /* The shadow at its opacity */
    EVE_CoDl_colorA(phost, dsc->opa);
    EVE_CoDl_blendFunc(phost, ONE, ZERO);
    draw_shadow(u, t, sl, corner_addr, edge_addr);
    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    /* Times the inverse of the widget */
    EVE_CoDl_colorA(phost, 255);
    EVE_CoDl_blendFunc(phost, ZERO, ONE_MINUS_SRC_ALPHA);
    lv_draw_eve5_draw_rect(u, bg.x1 - lx, bg.y1 - ly, bg.x2 - lx, bg.y2 - ly, r_bg, &t->clip_area, &layer->buf_area);
    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    /* The color through the mask */
    EVE_CoDl_colorMask(phost, 1, 1, 1, 0);
    EVE_CoDl_colorRgb(phost, color.red, color.green, color.blue);
    EVE_CoDl_colorA(phost, 255);
    EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
    lv_draw_eve5_draw_rect(u, sl->sx1, sl->sy1, sl->sx2, sl->sy2, 0, &t->clip_area, &layer->buf_area);
}

/* Textures of a shadow's slices, made when missing */
static bool shadow_textures(lv_draw_eve5_unit_t * u, int32_t ratio_idx, uint32_t * corner_addr, uint32_t * edge_addr)
{
    if(!ensure_shadow_textures(u, ratio_idx)) return false;

    lv_draw_eve5_shadow_slot_t * slot = &u->shadow_slots[ratio_idx];
    *corner_addr = EVE_GpuAlloc_Get(u->allocator, slot->corner_handle);
    *edge_addr = EVE_GpuAlloc_Get(u->allocator, slot->edge_handle);
    if(*corner_addr == GA_INVALID || *edge_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Shadow textures evicted unexpectedly");
        return false;
    }
    return true;
}

/**
 * Render box shadow using 9-slice Gaussian textures.
 */
void lv_draw_eve5_hal_draw_box_shadow(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t)
{
    EVE_HalContext * phost = u->hal;
    const lv_draw_box_shadow_dsc_t * dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width <= 0) return;

    shadow_slices_t sl;
    uint32_t corner_addr = GA_INVALID, edge_addr = GA_INVALID;
    if(!shadow_slices(t, dsc, &sl)) return;
    if(!sl.core_only && !shadow_textures(u, sl.ratio_idx, &corner_addr, &edge_addr)) return;

    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_saveContext(phost);
    lv_draw_eve5_set_scissor(u, &t->clip_area, &t->target_layer->buf_area);

    if(dsc->bg_cover) {
        EVE_CoDl_colorRgb(phost, dsc->color.red, dsc->color.green, dsc->color.blue);
        EVE_CoDl_colorA(phost, dsc->opa);
        draw_shadow(u, t, &sl, corner_addr, edge_addr);
    }
    else {
        draw_shadow_outside_widget(u, t, &sl, corner_addr, edge_addr, dsc->color);
        lv_draw_eve5_track_alpha_trashed(u, sl.sx1, sl.sy1, sl.sx2, sl.sy2);
    }

    EVE_CoDl_restoreContext(phost);
}

/* The widget's area is kept out of the shadow by an antialiased mask in the
 * alpha channel, which the direct-to-alpha pass can't use: it writes the
 * alpha itself. The L8 render-target pass can. */
bool lv_draw_eve5_box_shadow_needs_alpha_rendertarget(const lv_draw_task_t * t)
{
    const lv_draw_box_shadow_dsc_t * dsc = t->draw_dsc;
    return !dsc->bg_cover && dsc->opa > LV_OPA_MIN && dsc->width > 0;
}

/**********************
 * ALPHA PASS
 **********************/

/**
 * Draw the same shadow slices for alpha recovery. L8 textures decode as (R=255, G=255, B=255, A=L),
 * so source alpha contains the shadow coverage. Draw in white to capture that coverage as luminance
 * in the L8 pass, or write only alpha in the direct-to-alpha pass.
 */
void lv_draw_eve5_alpha_draw_box_shadow(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb)
{
    EVE_HalContext * phost = u->hal;
    const lv_draw_box_shadow_dsc_t * dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width <= 0) return;

    shadow_slices_t sl;
    uint32_t corner_addr = GA_INVALID, edge_addr = GA_INVALID;
    if(!shadow_slices(t, dsc, &sl)) return;
    /* Create the shadow textures now: the L8 pass runs before the RGB pass, which normally creates them. */
    if(!sl.core_only && !shadow_textures(u, sl.ratio_idx, &corner_addr, &edge_addr)) return;

    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_saveContext(phost);
    if(!dsc->bg_cover && alpha_to_rgb) {
        /* Use the L8 target's alpha channel as a temporary mask. */
        lv_draw_eve5_set_scissor(u, &t->clip_area, &t->target_layer->buf_area);
        draw_shadow_outside_widget(u, t, &sl, corner_addr, edge_addr, lv_color_white());
    }
    else {
        if(!dsc->bg_cover) exclude_widget_area(u, t, dsc);
        lv_draw_eve5_set_scissor(u, &t->clip_area, &t->target_layer->buf_area);

        if(alpha_to_rgb) EVE_CoDl_colorRgb(phost, 255, 255, 255);
        EVE_CoDl_colorA(phost, dsc->opa);
        draw_shadow(u, t, &sl, corner_addr, edge_addr);
    }

    EVE_CoDl_restoreContext(phost);
}

#endif /* LV_USE_DRAW_EVE5 */
