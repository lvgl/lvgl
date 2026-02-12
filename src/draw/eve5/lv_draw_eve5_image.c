/**
 * @file lv_draw_eve5_image.c
 *
 * EVE5 (BT820) Image and Layer Drawing
 *
 * Image format conversion, upload, and rendering for:
 * - Regular images (variable sources)
 * - Child layer compositing (render targets)
 * - Bitmap mask support
 * - Colorkey stencil masking
 * - Affine transforms (rotation, scale, skew)
 * - Alpha correction pass
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw.h"
#include "../lv_draw_image.h"

#if !LV_DRAW_EVE5_NO_FLOAT
#include <math.h>
#endif

/**********************
 * STATIC PROTOTYPES
 **********************/

static void convert_rgb565a8_to_argb8(const uint8_t *rgb, const uint8_t *alpha,
                                       uint8_t *dst, uint32_t w);
static void convert_xrgb8888_to_rgb8(const uint8_t *src, uint8_t *dst, uint32_t w);
static bool get_eve_format_info(lv_color_format_t src_cf,
                                 uint8_t *eve_format,
                                 uint8_t *bytes_per_pixel,
                                 bool *needs_conversion);
static uint32_t upload_image(lv_draw_eve5_unit_t *u, const lv_image_dsc_t *img_dsc,
                              uint8_t *out_eve_format, int32_t *out_eve_stride);
static void build_colorkey_stencil(EVE_HalContext *phost,
                                    const lv_image_colorkey_t *colorkey,
                                    int32_t vx, int32_t vy);

/**********************
 * IMAGE FORMAT CONVERSION
 **********************/

static void convert_rgb565a8_to_argb8(const uint8_t *rgb, const uint8_t *alpha,
                                       uint8_t *dst, uint32_t w)
{
    for(uint32_t x = 0; x < w; x++) {
        uint16_t rgb565 = ((const uint16_t *)rgb)[x];

        /* Expand 5/6/5 bits to 8 bits with proper bit replication for accuracy */
        uint8_t r5 = (rgb565 >> 11) & 0x1F;
        uint8_t g6 = (rgb565 >> 5) & 0x3F;
        uint8_t b5 = rgb565 & 0x1F;

        uint8_t r = (r5 << 3) | (r5 >> 2);  /* 5-bit to 8-bit */
        uint8_t g = (g6 << 2) | (g6 >> 4);  /* 6-bit to 8-bit */
        uint8_t b = (b5 << 3) | (b5 >> 2);  /* 5-bit to 8-bit */

        /* EVE ARGB8 is BGRA in memory (little-endian) */
        dst[4 * x + 0] = b;
        dst[4 * x + 1] = g;
        dst[4 * x + 2] = r;
        dst[4 * x + 3] = alpha[x];
    }
}

static void convert_xrgb8888_to_rgb8(const uint8_t *src, uint8_t *dst, uint32_t w)
{
    for(uint32_t x = 0; x < w; x++) {
        /* LVGL XRGB8888 is BGRX in memory */
        dst[3 * x + 0] = src[4 * x + 0];  /* B */
        dst[3 * x + 1] = src[4 * x + 1];  /* G */
        dst[3 * x + 2] = src[4 * x + 2];  /* R */
        /* Skip X (alpha) byte */
    }
}

static bool get_eve_format_info(lv_color_format_t src_cf,
                                 uint8_t *eve_format,
                                 uint8_t *bytes_per_pixel,
                                 bool *needs_conversion)
{
    *needs_conversion = false;

    switch(src_cf) {
        case LV_COLOR_FORMAT_L8:
            *eve_format = L8;
            *bytes_per_pixel = 1;
            break;

        case LV_COLOR_FORMAT_A8:
            *eve_format = L8;
            *bytes_per_pixel = 1;
            break;

        case LV_COLOR_FORMAT_RGB565:
            *eve_format = RGB565;
            *bytes_per_pixel = 2;
            break;

        case LV_COLOR_FORMAT_RGB888:
            *eve_format = RGB8;
            *bytes_per_pixel = 3;
            break;

        case LV_COLOR_FORMAT_XRGB8888:
            *eve_format = RGB8;
            *bytes_per_pixel = 3;
            *needs_conversion = true;
            break;

        case LV_COLOR_FORMAT_ARGB8888:
            *eve_format = ARGB8;
            *bytes_per_pixel = 4;
            break;

        case LV_COLOR_FORMAT_RGB565A8:
            *eve_format = ARGB8;
            *bytes_per_pixel = 4;
            *needs_conversion = true;
            break;

        default:
            LV_LOG_WARN("EVE5: Unsupported image format %d", src_cf);
            return false;
    }

    return true;
}

/**********************
 * IMAGE UPLOAD
 **********************/

static uint32_t upload_image(lv_draw_eve5_unit_t *u, const lv_image_dsc_t *img_dsc,
                              uint8_t *out_eve_format, int32_t *out_eve_stride)
{
    const uint8_t *src_buf = img_dsc->data;
    int32_t src_w = img_dsc->header.w;
    int32_t src_h = img_dsc->header.h;
    int32_t src_stride = img_dsc->header.stride;
    lv_color_format_t src_cf = img_dsc->header.cf;

    if(src_stride == 0) {
        src_stride = src_w * lv_color_format_get_size(src_cf);
    }

    uint8_t eve_format;
    uint8_t bpp;
    bool needs_conversion;

    if(!get_eve_format_info(src_cf, &eve_format, &bpp, &needs_conversion)) {
        return GA_INVALID;
    }

    int32_t eve_stride = ALIGN_UP(src_w * bpp, 4);
    int32_t eve_size = eve_stride * src_h;

    *out_eve_format = eve_format;
    *out_eve_stride = eve_stride;

    /* Check cache first */
    uint32_t cached_addr = lv_draw_eve5_image_cache_lookup(u, img_dsc, out_eve_format, out_eve_stride);
    if(cached_addr != GA_INVALID) {
        return cached_addr;
    }

    /* Allocate RAM_G space */
    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, eve_size, GA_ALIGN_4);
    uint32_t ram_g_addr = Esd_GpuAlloc_Get(u->allocator, handle);

    if(ram_g_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to allocate image in RAM_G (%d bytes)", eve_size);
        return GA_INVALID;
    }

    /* Upload based on format */
    switch(src_cf) {
        case LV_COLOR_FORMAT_L8:
        case LV_COLOR_FORMAT_A8: {
            /* 1 byte per pixel, simple copy */
            if(eve_stride == src_stride) {
                EVE_Hal_wrMem(u->hal, ram_g_addr, src_buf, eve_size);
            }
            else {
                for(int32_t y = 0; y < src_h; y++) {
                    EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                                  src_buf + y * src_stride, src_w);
                }
            }
            break;
        }

        case LV_COLOR_FORMAT_RGB565: {
            /* 2 bytes per pixel, direct copy */
            int32_t row_bytes = src_w * 2;
            if(eve_stride == src_stride) {
                EVE_Hal_wrMem(u->hal, ram_g_addr, src_buf, eve_size);
            }
            else {
                for(int32_t y = 0; y < src_h; y++) {
                    EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                                  src_buf + y * src_stride, row_bytes);
                }
            }
            break;
        }

        case LV_COLOR_FORMAT_RGB888: {
            /* 3 bytes per pixel, direct copy */
            int32_t row_bytes = src_w * 3;
            if(eve_stride == src_stride) {
                EVE_Hal_wrMem(u->hal, ram_g_addr, src_buf, eve_size);
            }
            else {
                for(int32_t y = 0; y < src_h; y++) {
                    EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                                  src_buf + y * src_stride, row_bytes);
                }
            }
            break;
        }

        case LV_COLOR_FORMAT_ARGB8888: {
            /* 4 bytes per pixel - DIRECT COPY, no conversion needed! */
            int32_t row_bytes = src_w * 4;
            if(eve_stride == src_stride) {
                EVE_Hal_wrMem(u->hal, ram_g_addr, src_buf, eve_size);
            }
            else {
                for(int32_t y = 0; y < src_h; y++) {
                    EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                                  src_buf + y * src_stride, row_bytes);
                }
            }
            break;
        }

        case LV_COLOR_FORMAT_XRGB8888: {
            /* Convert XRGB8888 to RGB8 (strip alpha) */
            uint8_t *tmp_buf = lv_malloc(eve_stride);
            if(!tmp_buf) {
                LV_LOG_ERROR("EVE5: Failed to allocate conversion buffer");
                Esd_GpuAlloc_Free(u->allocator, handle);
                return GA_INVALID;
            }

            for(int32_t y = 0; y < src_h; y++) {
                convert_xrgb8888_to_rgb8(src_buf + y * src_stride, tmp_buf, src_w);
                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
            }
            lv_free(tmp_buf);
            break;
        }

        case LV_COLOR_FORMAT_RGB565A8: {
            /* Convert RGB565+A8 to ARGB8 */
            uint8_t *tmp_buf = lv_malloc(eve_stride);
            if(!tmp_buf) {
                LV_LOG_ERROR("EVE5: Failed to allocate conversion buffer");
                Esd_GpuAlloc_Free(u->allocator, handle);
                return GA_INVALID;
            }

            /* Alpha plane follows RGB data */
            const uint8_t *alpha_buf = src_buf + src_h * src_stride;
            int32_t alpha_stride = src_stride / 2;  /* Alpha is 1 byte per pixel vs 2 for RGB565 */

            for(int32_t y = 0; y < src_h; y++) {
                convert_rgb565a8_to_argb8(src_buf + y * src_stride,
                                           alpha_buf + y * alpha_stride,
                                           tmp_buf, src_w);
                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
            }
            lv_free(tmp_buf);
            break;
        }

        default:
            /* Should not reach here due to earlier check */
            Esd_GpuAlloc_Free(u->allocator, handle);
            return GA_INVALID;
    }

    /* Insert into cache (allocator owns the memory, we just index it) */
    lv_draw_eve5_image_cache_insert(u, img_dsc, handle, eve_format, eve_stride);

    LV_LOG_TRACE("EVE5: Uploaded image %dx%d cf=%d as EVE format %d at 0x%08X",
                 src_w, src_h, src_cf, eve_format, ram_g_addr);

    return ram_g_addr;
}

/**********************
 * IMAGE DRAWING
 **********************/

/**
 * Build a colorkey stencil mask using 6-pass BITMAP_SWIZZLE approach.
 * Routes each color channel to alpha via BITMAP_SWIZZLE, then uses
 * ALPHA_FUNC for range testing. After 6 passes (2 per channel),
 * stencil == 6 at pixels matching the colorkey range.
 *
 * Caller must:
 * - Set up bitmap handle/source/layout/size and any transforms beforehand
 * - Clear stencil (CLEAR(0,1,0)) before calling
 * - Be inside a saveContext block
 *
 * On return: colorMask=(0,0,0,0), stencil accumulation state set.
 */
static void build_colorkey_stencil(EVE_HalContext *phost,
                                    const lv_image_colorkey_t *colorkey,
                                    int32_t vx, int32_t vy)
{
    /* Suppress pixel output, accumulate stencil on alpha-test pass */
    EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
    EVE_CoDl_stencilFunc(phost, ALWAYS, 0, 255);
    EVE_CoDl_stencilOp(phost, KEEP, INCR);
    EVE_CoDl_colorA(phost, 255);
    EVE_CoDl_colorRgb(phost, 255, 255, 255);

    /* Red channel: stencil++ where low.red <= R <= high.red */
    EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, RED);
    EVE_CoDl_alphaFunc(phost, GEQUAL, colorkey->low.red);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, vx, vy);
    EVE_CoDl_end(phost);
    EVE_CoDl_alphaFunc(phost, LEQUAL, colorkey->high.red);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, vx, vy);
    EVE_CoDl_end(phost);

    /* Green channel: stencil++ where low.green <= G <= high.green */
    EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, GREEN);
    EVE_CoDl_alphaFunc(phost, GEQUAL, colorkey->low.green);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, vx, vy);
    EVE_CoDl_end(phost);
    EVE_CoDl_alphaFunc(phost, LEQUAL, colorkey->high.green);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, vx, vy);
    EVE_CoDl_end(phost);

    /* Blue channel: stencil++ where low.blue <= B <= high.blue */
    EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, BLUE);
    EVE_CoDl_alphaFunc(phost, GEQUAL, colorkey->low.blue);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, vx, vy);
    EVE_CoDl_end(phost);
    EVE_CoDl_alphaFunc(phost, LEQUAL, colorkey->high.blue);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, vx, vy);
    EVE_CoDl_end(phost);
}

/**
 * Compute inverse affine transform for skewed image rendering.
 * Produces the six bitmap transform coefficients (A-F) and the
 * transformed bounding box needed for BITMAP_SIZE.
 * Returns false if the transform matrix is degenerate.
 */
bool compute_image_skew(image_skew_t *out,
                        int32_t rotation, int32_t scale_x, int32_t scale_y,
                        int32_t skew_x, int32_t skew_y,
                        int32_t pivot_x, int32_t pivot_y,
                        int32_t src_w, int32_t src_h,
                        int32_t img_x, int32_t img_y,
                        int32_t draw_vx, int32_t draw_vy)
{
#if LV_DRAW_EVE5_NO_FLOAT
    /* Integer-only path using lv_trigo_sin/cos (1-degree resolution)
     * and 16.16 fixed-point matrix math. */

    /* Round deci-degrees to nearest integer degree */
    int16_t rot_deg = (int16_t)((rotation + 5) / 10);
    int16_t skx_deg = (int16_t)((skew_x + 5) / 10);
    int16_t sky_deg = (int16_t)((skew_y + 5) / 10);

    /* lv_trigo_sin/cos return Q15 (1.15 format, ±32768).
     * Shift left 1 to get 16.16 fixed-point (±65536 = ±1.0). */
    int32_t cos_r = lv_trigo_cos(rot_deg) << 1;  /* 16.16 */
    int32_t sin_r = lv_trigo_sin(rot_deg) << 1;

    /* tan(skew) = sin/cos in 16.16. Clamp when |cos| < 5% to avoid overflow. */
    int32_t tan_skx, tan_sky;
    {
        int32_t cos_skx = lv_trigo_cos(skx_deg) << 1;
        int32_t sin_skx = lv_trigo_sin(skx_deg) << 1;
        if(LV_ABS(cos_skx) < 3277)  /* ~5% of 65536 */
            tan_skx = (cos_skx >= 0) ? 0x7FFFFFFF : (int32_t)0x80000001;
        else
            tan_skx = (int32_t)(((int64_t)sin_skx << 16) / cos_skx);
    }
    {
        int32_t cos_sky = lv_trigo_cos(sky_deg) << 1;
        int32_t sin_sky = lv_trigo_sin(sky_deg) << 1;
        if(LV_ABS(cos_sky) < 3277)
            tan_sky = (cos_sky >= 0) ? 0x7FFFFFFF : (int32_t)0x80000001;
        else
            tan_sky = (int32_t)(((int64_t)sin_sky << 16) / cos_sky);
    }

    /* Scale: 8.8 to 16.16 */
    int32_t fsx = (int32_t)scale_x << 8;  /* 16.16 */
    int32_t fsy = (int32_t)scale_y << 8;

    /* 16.16 × 16.16 multiply helper macro */
    #define FP_MUL(a, b) ((int32_t)(((int64_t)(a) * (b)) >> 16))

    /* Forward matrix M = Scale * Skew * Rotation (all 16.16) */
    int32_t ma = FP_MUL(fsx, cos_r) + FP_MUL(FP_MUL(tan_skx, fsy), sin_r);
    int32_t mb = FP_MUL(-fsx, sin_r) + FP_MUL(FP_MUL(tan_skx, fsy), cos_r);
    int32_t md = FP_MUL(FP_MUL(tan_sky, fsx), cos_r) + FP_MUL(fsy, sin_r);
    int32_t me = FP_MUL(FP_MUL(-tan_sky, fsx), sin_r) + FP_MUL(fsy, cos_r);

    /* Determinant in 16.16 (result of 16.16 × 16.16) */
    int32_t det = FP_MUL(ma, me) - FP_MUL(mb, md);
    if(LV_ABS(det) < 66)  /* ~1e-6 in 16.16 */
        return false;

    /* Inverse matrix coefficients in 16.16: ia = me/det, etc. */
    out->ia = (int32_t)(((int64_t)me << 16) / det);
    out->ib = (int32_t)(-((int64_t)mb << 16) / det);
    out->i_d = (int32_t)(-((int64_t)md << 16) / det);
    out->ie = (int32_t)(((int64_t)ma << 16) / det);

    /* Translation: C = px - (ia * local_piv_x + ib * local_piv_y)
     * Pivot and offsets are integers, convert to 16.16 for multiply. */
    int32_t ox_fp = (img_x - draw_vx) << 16;
    int32_t local_piv_x_fp = ox_fp + (pivot_x << 16);
    int32_t local_piv_y_fp = ((img_y - draw_vy) << 16) + (pivot_y << 16);
    out->ic = (pivot_x << 16) - (FP_MUL(out->ia, local_piv_x_fp) + FP_MUL(out->ib, local_piv_y_fp));
    out->i_f = (pivot_y << 16) - (FP_MUL(out->i_d, local_piv_x_fp) + FP_MUL(out->ie, local_piv_y_fp));

    /* Format selection: if any |coeff| >= 2.0 (0x20000 in 16.16), use 8.8 */
    out->p = 1;  /* signed 1.15 */
    if(LV_ABS(out->ia) >= 0x20000 || LV_ABS(out->ib) >= 0x20000
       || LV_ABS(out->i_d) >= 0x20000 || LV_ABS(out->ie) >= 0x20000) {
        out->p = 0;  /* unsigned 8.8 */
    }

    /* Bounding box: transform corners with forward matrix (16.16),
     * then shift >> 16 for pixels. */
    int32_t cx[4], cy[4];
    cx[0] = -(pivot_x << 16);              cy[0] = -(pivot_y << 16);
    cx[1] = (src_w - pivot_x) << 16;       cy[1] = -(pivot_y << 16);
    cx[2] = -(pivot_x << 16);              cy[2] = (src_h - pivot_y) << 16;
    cx[3] = (src_w - pivot_x) << 16;       cy[3] = (src_h - pivot_y) << 16;
    int32_t bx_min = 0, bx_max = 0, by_min = 0, by_max = 0;
    for(int i = 0; i < 4; i++) {
        int32_t sx = FP_MUL(ma, cx[i]) + FP_MUL(mb, cy[i]);
        int32_t sy = FP_MUL(md, cx[i]) + FP_MUL(me, cy[i]);
        if(i == 0) { bx_min = bx_max = sx; by_min = by_max = sy; }
        else {
            if(sx < bx_min) bx_min = sx;
            if(sx > bx_max) bx_max = sx;
            if(sy < by_min) by_min = sy;
            if(sy > by_max) by_max = sy;
        }
    }
    out->bmp_w = ((bx_max - bx_min) >> 16) + 2;
    out->bmp_h = ((by_max - by_min) >> 16) + 2;
    if(out->bmp_w > 2048) out->bmp_w = 2048;
    if(out->bmp_h > 2048) out->bmp_h = 2048;

    #undef FP_MUL
#else
    float px = (float)pivot_x;
    float py = (float)pivot_y;
    float rot_rad = rotation * ((float)M_PI / 1800.0f);
    float cos_r = cosf(rot_rad);
    float sin_r = sinf(rot_rad);
    float fsx = scale_x / 256.0f;
    float fsy = scale_y / 256.0f;
    float tan_skx = tanf(skew_x * ((float)M_PI / 1800.0f));
    float tan_sky = tanf(skew_y * ((float)M_PI / 1800.0f));

    float ma = fsx * cos_r + tan_skx * fsy * sin_r;
    float mb = -fsx * sin_r + tan_skx * fsy * cos_r;
    float md = tan_sky * fsx * cos_r + fsy * sin_r;
    float me = -tan_sky * fsx * sin_r + fsy * cos_r;

    float det = ma * me - mb * md;
    if(fabsf(det) < 1e-6f)
        return false;
    float inv_det = 1.0f / det;
    out->ia = me * inv_det;
    out->ib = -mb * inv_det;
    out->i_d = -md * inv_det;
    out->ie = ma * inv_det;

    float ox = (float)(img_x - draw_vx);
    float local_piv_x = ox + px;
    float local_piv_y = (float)(img_y - draw_vy) + py;
    out->ic = px - (out->ia * local_piv_x + out->ib * local_piv_y);
    out->i_f = py - (out->i_d * local_piv_x + out->ie * local_piv_y);

    out->p = 1;
    out->unity = 32768.0f;
    if(fabsf(out->ia) >= 2.0f || fabsf(out->ib) >= 2.0f
       || fabsf(out->i_d) >= 2.0f || fabsf(out->ie) >= 2.0f) {
        out->p = 0;
        out->unity = 256.0f;
    }

    /* Compute transformed bounding box for BITMAP_SIZE */
    float corners_x[4], corners_y[4];
    corners_x[0] = -px;             corners_y[0] = -py;
    corners_x[1] = src_w - px;      corners_y[1] = -py;
    corners_x[2] = -px;             corners_y[2] = src_h - py;
    corners_x[3] = src_w - px;      corners_y[3] = src_h - py;
    float bx_min = 0, bx_max = 0, by_min = 0, by_max = 0;
    for(int i = 0; i < 4; i++) {
        float sx2 = ma * corners_x[i] + mb * corners_y[i];
        float sy2 = md * corners_x[i] + me * corners_y[i];
        if(i == 0) { bx_min = bx_max = sx2; by_min = by_max = sy2; }
        else {
            if(sx2 < bx_min) bx_min = sx2;
            if(sx2 > bx_max) bx_max = sx2;
            if(sy2 < by_min) by_min = sy2;
            if(sy2 > by_max) by_max = sy2;
        }
    }
    out->bmp_w = (int32_t)(bx_max - bx_min + 1.5f);
    out->bmp_h = (int32_t)(by_max - by_min + 1.5f);
    if(out->bmp_w > 2048) out->bmp_w = 2048;
    if(out->bmp_h > 2048) out->bmp_h = 2048;
#endif

    return true;
}

/**
 * Apply computed skew transform: override BITMAP_SIZE for the transformed
 * bounding box (or tile extent) and set BITMAP_TRANSFORM A-F coefficients.
 */
void apply_image_skew(EVE_HalContext *phost, const image_skew_t *skew,
                      uint8_t bmp_filter, int32_t tile_w, int32_t tile_h)
{
    if(tile_w > 0 && tile_h > 0) {
        EVE_CoDl_bitmapSize(phost, bmp_filter, REPEAT, REPEAT, tile_w, tile_h);
    }
    else {
        EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER, skew->bmp_w, skew->bmp_h);
    }
#if LV_DRAW_EVE5_NO_FLOAT
    /* Convert 16.16 fixed-point coefficients to EVE register format.
     * Signed 1.15 (p=1): coeff >> 1.  Unsigned 8.8 (p=0): coeff >> 8.
     * C/F translation: always >> 8 for EVE's 8.8 register format. */
    if(skew->p) {
        EVE_CoDl_bitmapTransformA_ex(phost, 1, skew->ia >> 1);
        EVE_CoDl_bitmapTransformB_ex(phost, 1, skew->ib >> 1);
        EVE_CoDl_bitmapTransformD_ex(phost, 1, skew->i_d >> 1);
        EVE_CoDl_bitmapTransformE_ex(phost, 1, skew->ie >> 1);
    }
    else {
        EVE_CoDl_bitmapTransformA_ex(phost, 0, skew->ia >> 8);
        EVE_CoDl_bitmapTransformB_ex(phost, 0, skew->ib >> 8);
        EVE_CoDl_bitmapTransformD_ex(phost, 0, skew->i_d >> 8);
        EVE_CoDl_bitmapTransformE_ex(phost, 0, skew->ie >> 8);
    }
    EVE_CoDl_bitmapTransformC(phost, skew->ic >> 8);
    EVE_CoDl_bitmapTransformF(phost, skew->i_f >> 8);
#else
    EVE_CoDl_bitmapTransformA_ex(phost, skew->p, (int32_t)(skew->ia * skew->unity));
    EVE_CoDl_bitmapTransformB_ex(phost, skew->p, (int32_t)(skew->ib * skew->unity));
    EVE_CoDl_bitmapTransformC(phost, (int32_t)(skew->ic * 256.0f));
    EVE_CoDl_bitmapTransformD_ex(phost, skew->p, (int32_t)(skew->i_d * skew->unity));
    EVE_CoDl_bitmapTransformE_ex(phost, skew->p, (int32_t)(skew->ie * skew->unity));
    EVE_CoDl_bitmapTransformF(phost, (int32_t)(skew->i_f * 256.0f));
#endif
}

void lv_draw_eve5_hal_draw_image(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t *layer = t->target_layer;
    lv_draw_image_dsc_t *dsc = t->draw_dsc;

    uint32_t ram_g_addr;
    uint8_t eve_format;
    int32_t eve_stride;
    int32_t src_w, src_h;
    int32_t layout_h;  /* Height for bitmapLayout (may differ from src_h for render targets) */
    Esd_GpuHandle child_handle = GA_HANDLE_INVALID;

    /* Resolve bitmap source.
     * LAYER: GPU-rendered child layer (ARGB8 render target in RAM_G).
     * NOTE: Only GPU-rendered layers are supported (user_data holds GPU handle).
     * CPU-rendered layers (draw_buf only) are filtered out by the dispatcher. */
    if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
        lv_layer_t *child_layer = (lv_layer_t *)dsc->src;
        child_handle = Esd_GpuHandle_FromPtrType(child_layer->user_data);
        ram_g_addr = Esd_GpuAlloc_Get(u->allocator, child_handle);
        if(ram_g_addr == GA_INVALID) {
            LV_LOG_WARN("EVE5: Child layer %p texture invalid, handle id %i",
                child_layer, (int)child_handle.Id);
            return;
        }
        src_w = lv_area_get_width(&child_layer->buf_area);
        src_h = lv_area_get_height(&child_layer->buf_area);
        eve_format = ARGB8;
        eve_stride = ALIGN_UP(src_w, 16) * 4;
        layout_h = ALIGN_UP(src_h, 16);
    }
    else {
        /* Regular image */
        if(lv_image_src_get_type(dsc->src) != LV_IMAGE_SRC_VARIABLE) {
            LV_LOG_WARN("EVE5: Only variable images supported");
            return;
        }
        const lv_image_dsc_t *img_dsc = dsc->src;
        src_w = img_dsc->header.w;
        src_h = img_dsc->header.h;
        ram_g_addr = upload_image(u, img_dsc, &eve_format, &eve_stride);
        if(ram_g_addr == GA_INVALID)
            return;
        layout_h = src_h;
    }

    /* Upload bitmap mask if set (A8/L8 → EVE L8, cached in image cache) */
    bool has_bitmap_mask = false;
    uint32_t mask_ram_g_addr = GA_INVALID;
    int32_t mask_w = 0, mask_h = 0;
    int32_t mask_eve_stride = 0;
    if(dsc->bitmap_mask_src != NULL) {
        const lv_image_dsc_t *mask_dsc = dsc->bitmap_mask_src;
        uint8_t mask_eve_format;
        mask_w = mask_dsc->header.w;
        mask_h = mask_dsc->header.h;
        mask_ram_g_addr = upload_image(u, mask_dsc, &mask_eve_format, &mask_eve_stride);
        if(mask_ram_g_addr != GA_INVALID) {
            has_bitmap_mask = true;
        }
        else {
            LV_LOG_WARN("EVE5: Failed to upload bitmap mask");
        }
    }

    /* Calculate position in layer coordinates */
    int32_t x = t->area.x1 - layer->buf_area.x1;
    int32_t y = t->area.y1 - layer->buf_area.y1;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    /* Layer content is premultiplied (from SRC_ALPHA blend during rendering):
     * layer.rgb = actual_color * alpha, layer.a = alpha².
     * For compositing, use blend(ONE, ONE_MINUS_SRC_ALPHA) to avoid
     * double-applying alpha to RGB. Scale vertex color by opa so the
     * premultiplied content is properly attenuated by layer opacity.
     * Regular images use standard SRC_ALPHA blend with unscaled colors. */
    bool is_layer = (t->type == LV_DRAW_TASK_TYPE_LAYER);

    if(is_layer) {
        uint8_t opa = dsc->opa;
        if(dsc->recolor_opa > LV_OPA_MIN) {
            lv_color_t mixed = lv_color_mix(dsc->recolor, lv_color_white(), dsc->recolor_opa);
            EVE_CoDl_colorRgb(u->hal,
                (uint8_t)(mixed.red * opa / 255),
                (uint8_t)(mixed.green * opa / 255),
                (uint8_t)(mixed.blue * opa / 255));
        }
        else {
            EVE_CoDl_colorRgb(u->hal, opa, opa, opa);
        }
        EVE_CoDl_colorA(u->hal, opa);
    }
    else {
        EVE_CoDl_colorA(u->hal, dsc->opa);
        if(dsc->recolor_opa > LV_OPA_MIN) {
            lv_color_t mixed = lv_color_mix(dsc->recolor, lv_color_white(), dsc->recolor_opa);
            EVE_CoDl_colorRgb(u->hal, mixed.red, mixed.green, mixed.blue);
        }
        else {
            EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
        }
    }

    /* Set up bitmap */
    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
    EVE_CoDl_bitmapSource(u->hal, ram_g_addr);
    EVE_CoDl_bitmapLayout(u->hal, eve_format, eve_stride, layout_h);
    uint8_t bmp_filter = dsc->antialias ? BILINEAR : NEAREST;
    if(dsc->tile) {
        int32_t tile_w = lv_area_get_width(&dsc->image_area);
        int32_t tile_h = lv_area_get_height(&dsc->image_area);
        EVE_CoDl_bitmapSize(u->hal, bmp_filter, REPEAT, REPEAT, tile_w, tile_h);
    }
    else {
        EVE_CoDl_bitmapSize(u->hal, bmp_filter, BORDER, BORDER, src_w, src_h);
    }

    /* Alpha-channel masking for clip_radius and/or bitmap_mask_src.
     * Uses the same multi-phase approach as gradient fill masking:
     * Phase 1a: clear bbox alpha, 1b: write rounded rect mask (if clip_radius),
     * 1b2: apply bitmap mask (if bitmap_mask_src),
     * 1c: multiply mask by image alpha, 2: draw image through mask. */
    if(dsc->clip_radius > 0 || has_bitmap_mask) {
        /* Convert image_area from absolute to layer-local coordinates */
        int32_t mask_x1 = dsc->image_area.x1 - layer->buf_area.x1;
        int32_t mask_y1 = dsc->image_area.y1 - layer->buf_area.y1;
        int32_t mask_x2 = dsc->image_area.x2 - layer->buf_area.x1;
        int32_t mask_y2 = dsc->image_area.y2 - layer->buf_area.y1;

        /* Bitmap transform may be non-identity from a previous draw —
         * mask bitmap phases below need identity, so set it before save. */
        EVE_CoDl_bitmapTransform_identity(phost);
        EVE_CoDl_vertexFormat(phost, 0);
        EVE_CoDl_saveContext(phost);

        /* Phase 1a: Clear bbox alpha to 0 */
        EVE_CoDl_colorArgb_ex(phost, 0x00000000);
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2, 0,
                                &t->clip_area, &layer->buf_area);

        /* Phase 1b: Write rounded rect mask (alpha=255 inside) */
        if(dsc->clip_radius > 0) {
            EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
            lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2,
                                    dsc->clip_radius, &t->clip_area, &layer->buf_area);
        }

        /* Phase 1b2: Apply bitmap mask (A8/L8 texture as per-pixel alpha).
         * Combined with clip_radius: multiplies existing rounded rect alpha.
         * Standalone: overwrites alpha directly (ONE,ZERO carries from 1a). */
        if(has_bitmap_mask) {
            if(dsc->clip_radius > 0) {
                EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
            }
            /* else: ONE,ZERO from Phase 1a overwrites alpha with mask values */

            /* Center-align mask on image_area */
            int32_t img_w = lv_area_get_width(&dsc->image_area);
            int32_t img_h = lv_area_get_height(&dsc->image_area);
            int32_t mask_draw_x = mask_x1 + (img_w - mask_w) / 2;
            int32_t mask_draw_y = mask_y1 + (img_h - mask_h) / 2;

            /* Nested save/restore to preserve main image bitmap config */
            EVE_CoDl_saveContext(phost);
            EVE_CoDl_bitmapSource(phost, mask_ram_g_addr);
            EVE_CoDl_bitmapLayout(phost, L8, mask_eve_stride, mask_h);
            EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, mask_w, mask_h);
            EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, RED);  /* L8: route luminance→alpha */
            EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
            EVE_CoDl_begin(phost, BITMAPS);
            EVE_CoDl_vertex2f_0(phost, mask_draw_x, mask_draw_y);
            EVE_CoDl_end(phost);
            EVE_CoDl_restoreContext(phost);
        }

        /* Set up image transform (if any) — stays active for phases 1c and 2.
         * For tiled images, use image_area origin so tile pattern aligns. */
        int32_t draw_x = dsc->tile ? (dsc->image_area.x1 - layer->buf_area.x1) : x;
        int32_t draw_y = dsc->tile ? (dsc->image_area.y1 - layer->buf_area.y1) : y;
        if(dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE
           || dsc->skew_x != 0 || dsc->skew_y != 0) {
            int32_t clip_x = t->clip_area.x1 - layer->buf_area.x1;
            int32_t clip_y = t->clip_area.y1 - layer->buf_area.y1;
            draw_x = clip_x;
            draw_y = clip_y;

            if(dsc->skew_x != 0 || dsc->skew_y != 0) {
                image_skew_t skew;
                if(!compute_image_skew(&skew, dsc->rotation, dsc->scale_x, dsc->scale_y,
                                       dsc->skew_x, dsc->skew_y, dsc->pivot.x, dsc->pivot.y,
                                       src_w, src_h, x, y, clip_x, clip_y)) {
                    EVE_CoDl_restoreContext(phost);
                    goto cleanup;
                }
                apply_image_skew(phost, &skew, bmp_filter,
                    dsc->tile ? lv_area_get_width(&dsc->image_area) : 0,
                    dsc->tile ? lv_area_get_height(&dsc->image_area) : 0);
            }
            else {
                /* Rotation and/or scaling only — CoCmd matrix pipeline */
                EVE_CoCmd_loadIdentity(u->hal);
                EVE_CoCmd_translate(u->hal, F16(x - clip_x + dsc->pivot.x), F16(y - clip_y + dsc->pivot.y));
                if(dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) {
                    EVE_CoCmd_scale(u->hal, F16_SCALE_DIV_256(dsc->scale_x), F16_SCALE_DIV_256(dsc->scale_y));
                }
                if(dsc->rotation != 0) {
                    EVE_CoCmd_rotate(u->hal, DEGREES(dsc->rotation));
                }
                EVE_CoCmd_translate(u->hal, -F16(dsc->pivot.x), -F16(dsc->pivot.y));
                EVE_CoCmd_setMatrix(u->hal);
                /* Expand bitmapSize to cover rotated/scaled output —
                 * source dimensions would clip the transformed bounding box. */
                EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER,
                    LV_MIN(lv_area_get_width(&t->clip_area), 2048),
                    LV_MIN(lv_area_get_height(&t->clip_area), 2048));
            }
        }

        /* Colorkey + clip_radius: build stencil mask and punch alpha holes.
         * After the 6-pass stencil (stencil==6 at colorkey pixels), zero the
         * alpha mask at those positions so phases 1c and 2 naturally skip them. */
        if(dsc->colorkey != NULL) {
            /* clearStencil defaults to 0 and nothing changes it */
            EVE_CoDl_clear(phost, 0, 1, 0);
            build_colorkey_stencil(phost, dsc->colorkey, draw_x, draw_y);
            /* Punch: zero alpha where stencil == 6 (colorkey match) */
            EVE_CoDl_bitmapSwizzle(phost, RED, GREEN, BLUE, ALPHA);
            EVE_CoDl_alphaFunc(phost, ALWAYS, 0);
            EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
            EVE_CoDl_stencilFunc(phost, EQUAL, 6, 0xFF);
            EVE_CoDl_stencilOp(phost, KEEP, KEEP);
            EVE_CoDl_colorA(phost, 0);
            EVE_CoDl_blendFunc(phost, ONE, ZERO);
            EVE_CoDl_begin(phost, RECTS);
            EVE_CoDl_lineWidth(phost, 16);
            EVE_CoDl_vertex2f_0(phost, mask_x1, mask_y1);
            EVE_CoDl_vertex2f_0(phost, mask_x2, mask_y2);
            EVE_CoDl_end(phost);
            /* Disable stencil for subsequent phases */
            EVE_CoDl_stencilFunc(phost, ALWAYS, 0, 0);
            EVE_CoDl_stencilOp(phost, KEEP, KEEP);
        }

        /* Phase 1c: Multiply mask by image alpha.
         * colorA = opa so the mask includes opacity scaling.
         * Unlike gradient fill (which bakes opa into bitmap pixels),
         * image opa is applied via colorA during the alpha multiply. */
        EVE_CoDl_colorA(phost, dsc->opa);
        EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, draw_x, draw_y);
        EVE_CoDl_end(phost);

        /* Phase 2: Draw image RGB through the alpha mask.
         * For regular images: DST_ALPHA multiplies by mask, standard compositing.
         * For premultiplied layers: ONE avoids re-multiplying already-premultiplied
         * RGB by the mask (which contains squared alpha from SRC_ALPHA rendering).
         * Layer opa is applied via colorRgb scaling. */
        EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
        if(is_layer) {
            if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE)
                EVE_CoDl_blendFunc(phost, ONE, ONE);
            else
                EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_DST_ALPHA);
            uint8_t opa = dsc->opa;
            if(dsc->recolor_opa > LV_OPA_MIN) {
                lv_color_t mixed = lv_color_mix(dsc->recolor, lv_color_white(), dsc->recolor_opa);
                EVE_CoDl_colorRgb(phost,
                    (uint8_t)(mixed.red * opa / 255),
                    (uint8_t)(mixed.green * opa / 255),
                    (uint8_t)(mixed.blue * opa / 255));
            }
            else {
                EVE_CoDl_colorRgb(phost, opa, opa, opa);
            }
            EVE_CoDl_colorA(phost, opa);
        }
        else {
            if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE)
                EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE);
            else
                EVE_CoDl_blendFunc(phost, DST_ALPHA, ONE_MINUS_DST_ALPHA);
            if(dsc->recolor_opa > LV_OPA_MIN) {
                lv_color_t mixed = lv_color_mix(dsc->recolor, lv_color_white(), dsc->recolor_opa);
                EVE_CoDl_colorRgb(phost, mixed.red, mixed.green, mixed.blue);
            }
            else {
                EVE_CoDl_colorRgb(phost, 255, 255, 255);
            }
            EVE_CoDl_colorA(phost, 255);
        }
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, draw_x, draw_y);
        EVE_CoDl_end(phost);

        EVE_CoDl_restoreContext(phost);

        /* Image masking uses alpha-as-scratch */
        lv_draw_eve5_track_alpha_trashed(u, mask_x1, mask_y1, mask_x2, mask_y2);
    }
    else {
        /* No clip radius or bitmap mask — standard draw path.
         * Structured as linear phases: compute → save → transform → colorkey → draw → restore.
         * Colorkey uses stencil test directly (no alpha masking needed). */
        bool has_colorkey = (dsc->colorkey != NULL);
        bool has_transform = (dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE
                             || dsc->scale_y != LV_SCALE_NONE
                             || dsc->skew_x != 0 || dsc->skew_y != 0);
        bool has_skew = (dsc->skew_x != 0 || dsc->skew_y != 0);

        /* Compute draw vertex position */
        int32_t draw_vx, draw_vy;
        if(has_transform) {
            draw_vx = t->clip_area.x1 - layer->buf_area.x1;
            draw_vy = t->clip_area.y1 - layer->buf_area.y1;
        }
        else {
            draw_vx = dsc->tile ? (dsc->image_area.x1 - layer->buf_area.x1) : x;
            draw_vy = dsc->tile ? (dsc->image_area.y1 - layer->buf_area.y1) : y;
        }

        /* Skew: compute inverse affine transform.
         * Done before saveContext so degenerate matrix can early-return cleanly. */
        image_skew_t skew;
        if(has_skew) {
            if(!compute_image_skew(&skew, dsc->rotation, dsc->scale_x, dsc->scale_y,
                                   dsc->skew_x, dsc->skew_y, dsc->pivot.x, dsc->pivot.y,
                                   src_w, src_h, x, y, draw_vx, draw_vy))
                goto cleanup;
        }

        /* Bitmap transform may be non-identity from a previous draw —
         * when not applying our own transform, reset it before use. */
        if(!has_skew && !has_transform) {
            EVE_CoDl_bitmapTransform_identity(phost);
        }

        /* Save context for non-optimized state (transform, colorkey stencil) */
        if(has_skew || has_colorkey || has_transform) {
            EVE_CoDl_vertexFormat(phost, 0);
            EVE_CoDl_saveContext(phost);
        }

        /* Apply bitmap transform */
        if(has_skew) {
            apply_image_skew(phost, &skew, bmp_filter,
                dsc->tile ? lv_area_get_width(&dsc->image_area) : 0,
                dsc->tile ? lv_area_get_height(&dsc->image_area) : 0);
        }
        else if(has_transform) {
            /* Rotation and/or scaling only — CoCmd matrix pipeline */
            EVE_CoCmd_loadIdentity(u->hal);
            EVE_CoCmd_translate(u->hal, F16(x - draw_vx + dsc->pivot.x), F16(y - draw_vy + dsc->pivot.y));
            if(dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) {
                EVE_CoCmd_scale(u->hal, F16_SCALE_DIV_256(dsc->scale_x), F16_SCALE_DIV_256(dsc->scale_y));
            }
            if(dsc->rotation != 0) {
                EVE_CoCmd_rotate(u->hal, DEGREES(dsc->rotation));
            }
            EVE_CoCmd_translate(u->hal, -F16(dsc->pivot.x), -F16(dsc->pivot.y));
            EVE_CoCmd_setMatrix(u->hal);
            EVE_CoCmd_loadIdentity(u->hal);
            /* Expand bitmapSize to cover rotated/scaled output —
             * source dimensions would clip the transformed bounding box. */
            EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER,
                LV_MIN(lv_area_get_width(&t->clip_area), 2048),
                LV_MIN(lv_area_get_height(&t->clip_area), 2048));
        }

        /* Build colorkey stencil mask (transform is active, stencil samples match draw) */
        if(has_colorkey) {
            /* clearStencil defaults to 0 and nothing changes it */
            EVE_CoDl_clear(phost, 0, 1, 0);
            build_colorkey_stencil(phost, dsc->colorkey, draw_vx, draw_vy);
            /* Transition to draw: restore swizzle, alpha test, color mask */
            EVE_CoDl_bitmapSwizzle(phost, RED, GREEN, BLUE, ALPHA);
            EVE_CoDl_alphaFunc(phost, ALWAYS, 0);
            EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
            EVE_CoDl_stencilFunc(phost, NOTEQUAL, 6, 0xFF);
            EVE_CoDl_stencilOp(phost, KEEP, KEEP);
            /* Restore colors overwritten by stencil passes */
            if(is_layer) {
                uint8_t opa = dsc->opa;
                if(dsc->recolor_opa > LV_OPA_MIN) {
                    lv_color_t mixed = lv_color_mix(dsc->recolor, lv_color_white(), dsc->recolor_opa);
                    EVE_CoDl_colorRgb(phost,
                        (uint8_t)(mixed.red * opa / 255),
                        (uint8_t)(mixed.green * opa / 255),
                        (uint8_t)(mixed.blue * opa / 255));
                }
                else {
                    EVE_CoDl_colorRgb(phost, opa, opa, opa);
                }
                EVE_CoDl_colorA(phost, opa);
            }
            else {
                EVE_CoDl_colorA(phost, dsc->opa);
                if(dsc->recolor_opa > LV_OPA_MIN) {
                    lv_color_t mixed = lv_color_mix(dsc->recolor, lv_color_white(), dsc->recolor_opa);
                    EVE_CoDl_colorRgb(phost, mixed.red, mixed.green, mixed.blue);
                }
                else {
                    EVE_CoDl_colorRgb(phost, 255, 255, 255);
                }
            }
        }

        /* Handle blend mode */
        if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE) {
            EVE_CoDl_blendFunc(phost, is_layer ? ONE : SRC_ALPHA, ONE);
        }
        else if(is_layer) {
            /* Premultiplied compositing: layer RGB already contains color*alpha,
             * so source factor = ONE (don't re-multiply by alpha).
             * Destination fades out via ONE_MINUS_SRC_ALPHA.
             * Note: layer alpha is squared (alpha² from SRC_ALPHA rendering),
             * causing slightly more parent bleed-through at AA edges. */
            EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_SRC_ALPHA);
        }

        /* Draw bitmap */
        EVE_CoDl_begin(u->hal, BITMAPS);
        EVE_CoDl_vertex2f_0(u->hal, draw_vx, draw_vy);
        EVE_CoDl_end(u->hal);

        /* Restore state */
        if(has_skew || has_colorkey || has_transform) {
            EVE_CoDl_restoreContext(phost);
        }
        else if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE || is_layer) {
            EVE_CoDl_blendFunc_default(phost);
        }
    }

cleanup:
    /* Layer-specific cleanup: track child texture for deferred free.
     * Do NOT null user_data here — the alpha pass needs to re-access
     * the child texture. user_data is nulled after the alpha pass
     * in eve5_render_layer(). */
    if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
        track_frame_alloc(u, child_handle);
    }
}

/**********************
 * ALPHA PASS IMAGE/LAYER
 **********************/

/**
 * Alpha correction pass for IMAGE and LAYER tasks.
 *
 * Re-draws the bitmap with correct transforms, colorkey, and tiling
 * into the alpha channel only. The outer context already has
 * colorMask(0,0,0,1) and blend(ONE, ONE_MINUS_SRC_ALPHA) set.
 *
 * For clip_radius or bitmap_mask_src, draws the clip shape at opa
 * (exact for opaque formats, approximate for ARGB with per-pixel alpha).
 * For the standard path, draws the actual bitmap — exact for all formats.
 */
void lv_draw_eve5_hal_alpha_draw_image(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t *layer = t->target_layer;
    const lv_draw_image_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;

    uint32_t ram_g_addr;
    uint8_t eve_format;
    int32_t eve_stride;
    int32_t src_w, src_h;
    int32_t layout_h;

    /* Resolve bitmap source (same as normal draw) */
    if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
        lv_layer_t *child_layer = (lv_layer_t *)dsc->src;
        Esd_GpuHandle child_handle = Esd_GpuHandle_FromPtrType(child_layer->user_data);
        ram_g_addr = Esd_GpuAlloc_Get(u->allocator, child_handle);
        if(ram_g_addr == GA_INVALID) return;
        src_w = lv_area_get_width(&child_layer->buf_area);
        src_h = lv_area_get_height(&child_layer->buf_area);
        eve_format = ARGB8;
        eve_stride = ALIGN_UP(src_w, 16) * 4;
        layout_h = ALIGN_UP(src_h, 16);
    }
    else {
        if(lv_image_src_get_type(dsc->src) != LV_IMAGE_SRC_VARIABLE) return;
        const lv_image_dsc_t *img_dsc = dsc->src;
        src_w = img_dsc->header.w;
        src_h = img_dsc->header.h;
        ram_g_addr = upload_image(u, img_dsc, &eve_format, &eve_stride);
        if(ram_g_addr == GA_INVALID) return;
        layout_h = src_h;
    }

    int32_t x = t->area.x1 - layer->buf_area.x1;
    int32_t y = t->area.y1 - layer->buf_area.y1;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    /* Masking path: clip_radius or bitmap_mask — draw clip shape at opa.
     * Exact for opaque formats, approximate for ARGB with per-pixel alpha. */
    if(dsc->clip_radius > 0 || dsc->bitmap_mask_src != NULL) {
#if EVE5_ALPHA_STENCIL_APPROX
        /* Stencil-based clip: build clip shape in stencil, then draw through it.
         * Handles two cases:
         *   clip + mask: draw mask bitmap through stencil (exact for opaque, approx for ARGB)
         *   clip + ARGB (no mask): draw source bitmap through stencil (per-pixel alpha)
         * Colorkey excluded — its own 6-pass stencil would conflict. */
        if(dsc->clip_radius > 0 && dsc->colorkey == NULL) {
            /* Try to upload mask bitmap if present */
            uint32_t mask_bmp_addr = GA_INVALID;
            int32_t mask_bmp_stride = 0;
            int32_t mask_bmp_w = 0, mask_bmp_h = 0;
            if(dsc->bitmap_mask_src != NULL) {
                const lv_image_dsc_t *mask_bmp_dsc = dsc->bitmap_mask_src;
                if(lv_image_src_get_type(mask_bmp_dsc) == LV_IMAGE_SRC_VARIABLE) {
                    uint8_t mf;
                    mask_bmp_addr = upload_image(u, mask_bmp_dsc, &mf, &mask_bmp_stride);
                    mask_bmp_w = mask_bmp_dsc->header.w;
                    mask_bmp_h = mask_bmp_dsc->header.h;
                }
            }

            if(mask_bmp_addr != GA_INVALID || eve_format == ARGB8) {
                int32_t clip_x1 = dsc->image_area.x1 - layer->buf_area.x1;
                int32_t clip_y1 = dsc->image_area.y1 - layer->buf_area.y1;
                int32_t clip_x2 = dsc->image_area.x2 - layer->buf_area.x1;
                int32_t clip_y2 = dsc->image_area.y2 - layer->buf_area.y1;
                int32_t clip_w = clip_x2 - clip_x1 + 1;
                int32_t clip_h = clip_y2 - clip_y1 + 1;
                int32_t real_radius = LV_MIN3(clip_w / 2, clip_h / 2, (int32_t)dsc->clip_radius);

                lv_draw_eve5_clear_stencil(u, clip_x1, clip_y1, clip_x2, clip_y2,
                                            &t->clip_area, &layer->buf_area);

                /* Bitmap transform may be non-identity from a previous draw —
                 * bitmap draws inside this scope need identity, so set before save. */
                EVE_CoDl_bitmapTransform_identity(phost);
                EVE_CoDl_vertexFormat(phost, 0);
                EVE_CoDl_saveContext(phost);

                /* Phase 1: Draw clip shape into stencil (no color output) */
                EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
                EVE_CoDl_stencilOp(phost, KEEP, INCR);
                if(clip_w == clip_h && real_radius >= clip_w / 2) {
                    EVE_CoDl_pointSize(phost, clip_w * 8 - 8);
                    EVE_CoDl_begin(phost, POINTS);
                    EVE_CoDl_vertex2f_1(phost, clip_x1 * 2 + (clip_w - 1), clip_y1 * 2 + (clip_h - 1));
                    EVE_CoDl_end(phost);
                }
                else {
                    EVE_CoDl_lineWidth(phost, real_radius * 16);
                    EVE_CoDl_begin(phost, RECTS);
                    EVE_CoDl_vertex2f_0(phost, clip_x1 + real_radius, clip_y1 + real_radius);
                    EVE_CoDl_vertex2f_0(phost, clip_x2 - real_radius, clip_y2 - real_radius);
                    EVE_CoDl_end(phost);
                }

                /* Phase 2: Draw through stencil (alpha-only) */
                EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
                EVE_CoDl_stencilFunc(phost, NOTEQUAL, 0, 0xFF);
                EVE_CoDl_stencilOp(phost, KEEP, KEEP);
                EVE_CoDl_colorA(phost, dsc->opa);

                if(mask_bmp_addr != GA_INVALID) {
                    /* Draw mask bitmap through clip stencil.
                     * Exact for opaque formats; for ARGB, preserves mask + clip shape
                     * but loses source bitmap per-pixel alpha. */
                    int32_t img_w = lv_area_get_width(&dsc->image_area);
                    int32_t img_h = lv_area_get_height(&dsc->image_area);
                    int32_t mask_draw_x = clip_x1 + (img_w - mask_bmp_w) / 2;
                    int32_t mask_draw_y = clip_y1 + (img_h - mask_bmp_h) / 2;

                    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
                    EVE_CoDl_bitmapSource(phost, mask_bmp_addr);
                    EVE_CoDl_bitmapLayout(phost, L8, mask_bmp_stride, mask_bmp_h);
                    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, mask_bmp_w, mask_bmp_h);
                    EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, RED);  /* L8: luminance → alpha */
                    EVE_CoDl_begin(phost, BITMAPS);
                    EVE_CoDl_vertex2f_0(phost, mask_draw_x, mask_draw_y);
                    EVE_CoDl_end(phost);
                }
                else {
                    /* Draw ARGB source bitmap through clip stencil.
                     * Reproduces per-pixel alpha inside the clip region. */
                    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
                    EVE_CoDl_bitmapSource(phost, ram_g_addr);
                    EVE_CoDl_bitmapLayout(phost, eve_format, eve_stride, layout_h);
                    uint8_t bmp_filter = dsc->antialias ? BILINEAR : NEAREST;
                    if(dsc->tile) {
                        int32_t tile_w = lv_area_get_width(&dsc->image_area);
                        int32_t tile_h = lv_area_get_height(&dsc->image_area);
                        EVE_CoDl_bitmapSize(phost, bmp_filter, REPEAT, REPEAT, tile_w, tile_h);
                    }
                    else {
                        EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER, src_w, src_h);
                    }

                    /* Compute draw vertex and apply transforms (same as standard path) */
                    bool has_transform = (dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE
                                         || dsc->scale_y != LV_SCALE_NONE
                                         || dsc->skew_x != 0 || dsc->skew_y != 0);
                    bool has_skew = (dsc->skew_x != 0 || dsc->skew_y != 0);

                    int32_t draw_vx, draw_vy;
                    if(has_transform) {
                        draw_vx = t->clip_area.x1 - layer->buf_area.x1;
                        draw_vy = t->clip_area.y1 - layer->buf_area.y1;
                    }
                    else {
                        draw_vx = dsc->tile ? (dsc->image_area.x1 - layer->buf_area.x1) : x;
                        draw_vy = dsc->tile ? (dsc->image_area.y1 - layer->buf_area.y1) : y;
                    }

                    if(has_skew) {
                        image_skew_t skew;
                        if(!compute_image_skew(&skew, dsc->rotation, dsc->scale_x, dsc->scale_y,
                                               dsc->skew_x, dsc->skew_y, dsc->pivot.x, dsc->pivot.y,
                                               src_w, src_h, x, y, draw_vx, draw_vy)) {
                            EVE_CoDl_restoreContext(phost);
                            return;
                        }
                        apply_image_skew(phost, &skew, bmp_filter,
                            dsc->tile ? lv_area_get_width(&dsc->image_area) : 0,
                            dsc->tile ? lv_area_get_height(&dsc->image_area) : 0);
                    }
                    else if(has_transform) {
                        EVE_CoCmd_loadIdentity(phost);
                        EVE_CoCmd_translate(phost, F16(x - draw_vx + dsc->pivot.x),
                                            F16(y - draw_vy + dsc->pivot.y));
                        if(dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) {
                            EVE_CoCmd_scale(phost, F16_SCALE_DIV_256(dsc->scale_x),
                                            F16_SCALE_DIV_256(dsc->scale_y));
                        }
                        if(dsc->rotation != 0) {
                            EVE_CoCmd_rotate(phost, DEGREES(dsc->rotation));
                        }
                        EVE_CoCmd_translate(phost, -F16(dsc->pivot.x), -F16(dsc->pivot.y));
                        EVE_CoCmd_setMatrix(phost);
                        EVE_CoCmd_loadIdentity(phost);
                        /* Expand bitmapSize to cover rotated/scaled output */
                        EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER,
                            LV_MIN(lv_area_get_width(&t->clip_area), 2048),
                            LV_MIN(lv_area_get_height(&t->clip_area), 2048));
                    }

                    EVE_CoDl_begin(phost, BITMAPS);
                    EVE_CoDl_vertex2f_0(phost, draw_vx, draw_vy);
                    EVE_CoDl_end(phost);
                }

                EVE_CoDl_restoreContext(phost);
                return;
            }
        }
#endif
        if(dsc->clip_radius > 0 && dsc->bitmap_mask_src == NULL) {
            /* clip_radius only — draw rounded rect at opa (exact for all formats) */
            int32_t mask_x1 = dsc->image_area.x1 - layer->buf_area.x1;
            int32_t mask_y1 = dsc->image_area.y1 - layer->buf_area.y1;
            int32_t mask_x2 = dsc->image_area.x2 - layer->buf_area.x1;
            int32_t mask_y2 = dsc->image_area.y2 - layer->buf_area.y1;
            EVE_CoDl_colorA(phost, dsc->opa);
            lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2,
                                    dsc->clip_radius, &t->clip_area, &layer->buf_area);
        }
        else {
            /* bitmap_mask (with or without clip_radius) — draw mask bitmap at opa.
             * For opaque formats without clip_radius: exact.
             * For opaque + clip_radius (stencil unavailable): loses clip rounding.
             * For ARGB: mask shape preserved, per-pixel bitmap alpha lost. */
            const lv_image_dsc_t *mask_dsc = dsc->bitmap_mask_src;
            if(mask_dsc != NULL && lv_image_src_get_type(mask_dsc) == LV_IMAGE_SRC_VARIABLE) {
                uint8_t mask_eve_format;
                int32_t mask_eve_stride;
                uint32_t mask_addr = upload_image(u, mask_dsc, &mask_eve_format, &mask_eve_stride);
                if(mask_addr != GA_INVALID) {
                    int32_t mask_w = mask_dsc->header.w;
                    int32_t mask_h = mask_dsc->header.h;

                    /* Center-align mask on image_area (same as normal draw) */
                    int32_t img_w = lv_area_get_width(&dsc->image_area);
                    int32_t img_h = lv_area_get_height(&dsc->image_area);
                    int32_t mask_x1 = dsc->image_area.x1 - layer->buf_area.x1;
                    int32_t mask_y1 = dsc->image_area.y1 - layer->buf_area.y1;
                    int32_t mask_draw_x = mask_x1 + (img_w - mask_w) / 2;
                    int32_t mask_draw_y = mask_y1 + (img_h - mask_h) / 2;

                    /* Bitmap transform may be non-identity from a previous draw —
                     * mask bitmap draws at identity. */
                    EVE_CoDl_bitmapTransform_identity(phost);
                    EVE_CoDl_vertexFormat(phost, 0);
                    EVE_CoDl_saveContext(phost);
                    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
                    EVE_CoDl_bitmapSource(phost, mask_addr);
                    EVE_CoDl_bitmapLayout(phost, L8, mask_eve_stride, mask_h);
                    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, mask_w, mask_h);
                    EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, RED);  /* L8: luminance → alpha */
                    EVE_CoDl_colorA(phost, dsc->opa);
                    EVE_CoDl_begin(phost, BITMAPS);
                    EVE_CoDl_vertex2f_0(phost, mask_draw_x, mask_draw_y);
                    EVE_CoDl_end(phost);
                    EVE_CoDl_restoreContext(phost);
                }
            }
        }
        return;
    }

    /* Standard path: re-draw bitmap with transforms for exact per-pixel alpha.
     * Mirrors the normal draw's standard path but skips color/blend setup
     * since the outer context handles colorMask and blendFunc. */
    bool has_colorkey = (dsc->colorkey != NULL);
    bool has_transform = (dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE
                         || dsc->scale_y != LV_SCALE_NONE
                         || dsc->skew_x != 0 || dsc->skew_y != 0);
    bool has_skew = (dsc->skew_x != 0 || dsc->skew_y != 0);

    /* Set up bitmap (same as normal draw) */
    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
    EVE_CoDl_bitmapSource(phost, ram_g_addr);
    EVE_CoDl_bitmapLayout(phost, eve_format, eve_stride, layout_h);
    uint8_t bmp_filter = dsc->antialias ? BILINEAR : NEAREST;
    if(dsc->tile) {
        int32_t tile_w = lv_area_get_width(&dsc->image_area);
        int32_t tile_h = lv_area_get_height(&dsc->image_area);
        EVE_CoDl_bitmapSize(phost, bmp_filter, REPEAT, REPEAT, tile_w, tile_h);
    }
    else {
        EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER, src_w, src_h);
    }

    /* Compute draw vertex position */
    int32_t draw_vx, draw_vy;
    if(has_transform) {
        draw_vx = t->clip_area.x1 - layer->buf_area.x1;
        draw_vy = t->clip_area.y1 - layer->buf_area.y1;
    }
    else {
        draw_vx = dsc->tile ? (dsc->image_area.x1 - layer->buf_area.x1) : x;
        draw_vy = dsc->tile ? (dsc->image_area.y1 - layer->buf_area.y1) : y;
    }

    /* Compute skew transform (before saveContext so degenerate can return) */
    image_skew_t skew;
    if(has_skew) {
        if(!compute_image_skew(&skew, dsc->rotation, dsc->scale_x, dsc->scale_y,
                               dsc->skew_x, dsc->skew_y, dsc->pivot.x, dsc->pivot.y,
                               src_w, src_h, x, y, draw_vx, draw_vy))
            return;
    }

    /* Bitmap transform may be non-identity from a previous draw —
     * when not applying our own transform, reset it before use. */
    if(!has_skew && !has_transform) {
        EVE_CoDl_bitmapTransform_identity(phost);
    }

    /* Save context for non-optimized state (transform, colorkey stencil) */
    if(has_skew || has_colorkey || has_transform) {
        EVE_CoDl_vertexFormat(phost, 0);
        EVE_CoDl_saveContext(phost);
    }

    /* Apply bitmap transform */
    if(has_skew) {
        apply_image_skew(phost, &skew, bmp_filter,
            dsc->tile ? lv_area_get_width(&dsc->image_area) : 0,
            dsc->tile ? lv_area_get_height(&dsc->image_area) : 0);
    }
    else if(has_transform) {
        EVE_CoCmd_loadIdentity(phost);
        EVE_CoCmd_translate(phost, F16(x - draw_vx + dsc->pivot.x), F16(y - draw_vy + dsc->pivot.y));
        if(dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) {
            EVE_CoCmd_scale(phost, F16_SCALE_DIV_256(dsc->scale_x), F16_SCALE_DIV_256(dsc->scale_y));
        }
        if(dsc->rotation != 0) {
            EVE_CoCmd_rotate(phost, DEGREES(dsc->rotation));
        }
        EVE_CoCmd_translate(phost, -F16(dsc->pivot.x), -F16(dsc->pivot.y));
        EVE_CoCmd_setMatrix(phost);
        EVE_CoCmd_loadIdentity(phost);
        /* Expand bitmapSize to cover rotated/scaled output —
         * source dimensions would clip the transformed bounding box. */
        EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER,
            LV_MIN(lv_area_get_width(&t->clip_area), 2048),
            LV_MIN(lv_area_get_height(&t->clip_area), 2048));
    }

    /* Build colorkey stencil mask */
    if(has_colorkey) {
        /* clearStencil defaults to 0 and nothing changes it */
        EVE_CoDl_clear(phost, 0, 1, 0);
        build_colorkey_stencil(phost, dsc->colorkey, draw_vx, draw_vy);
        /* Restore drawing state after stencil passes.
         * Only need swizzle, alphaFunc, stencilFunc — no colors since alpha-only. */
        EVE_CoDl_bitmapSwizzle(phost, RED, GREEN, BLUE, ALPHA);
        EVE_CoDl_alphaFunc(phost, ALWAYS, 0);
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1);  /* Restore alpha-only from outer context */
        EVE_CoDl_stencilFunc(phost, NOTEQUAL, 6, 0xFF);
        EVE_CoDl_stencilOp(phost, KEEP, KEEP);
    }

    /* Set alpha — the only "color" setup needed for the alpha pass */
    EVE_CoDl_colorA(phost, dsc->opa);

    /* Draw bitmap */
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, draw_vx, draw_vy);
    EVE_CoDl_end(phost);

    /* Restore state */
    if(has_skew || has_colorkey || has_transform) {
        EVE_CoDl_restoreContext(phost);
    }
}

#endif /* LV_USE_DRAW_EVE5 */
