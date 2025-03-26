/**
 * @file lv_draw_sw_blend_to_argb8888_premultiplied.c
 * @brief Implementation of ARGB8888 Premultiplied blending for LVGL.
 */

/*********************
*      INCLUDES
*********************/
#include "lv_draw_sw_blend_to_argb8888_premultiplied.h"
#if LV_USE_DRAW_SW

#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED

#include "lv_draw_sw_blend_private.h"
#include "../../../misc/lv_math.h"
#include "../../../display/lv_display.h"
#include "../../../core/lv_refr.h"
#include "../../../misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
*      TYPEDEFS
**********************/

typedef struct {
    lv_color32_t fg_saved;
    lv_color32_t bg_saved;
    lv_color32_t res_saved;
    lv_opa_t res_alpha_saved;
    lv_opa_t ratio_saved;
} lv_color_mix_alpha_cache_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static inline lv_color32_t lv_color_32_32_mix_premul(lv_color32_t fg, lv_color32_t bg,
                                                     lv_color_mix_alpha_cache_t * cache);

static void lv_color_mix_with_alpha_cache_init(lv_color_mix_alpha_cache_t * cache);

static inline void * /* LV_ATTRIBUTE_FAST_MEM */ drawbuf_next_row(const void * buf, uint32_t stride);

static void lv_color_mix_with_alpha_cache_init(lv_color_mix_alpha_cache_t * cache);

static lv_color32_t lv_color_mix32_premul(lv_color32_t fg, lv_color32_t bg);

/**********************
*  STATIC VARIABLES
**********************/

/**********************
 *      MACROS
 **********************/


/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/**
 * @brief Blend a solid color into an ARGB8888 premultiplied buffer.
 *
 * This function applies a solid color to the destination buffer with optional
 * opacity and masking. The input color is first converted to a premultiplied
 * alpha format before blending.
 *
 * @param dsc Blending descriptor containing destination buffer, color, and opacity
 */
void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_color_to_argb8888_premultiplied(lv_draw_sw_blend_fill_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    const lv_opa_t * mask = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;
    int32_t dest_stride = dsc->dest_stride;

    lv_color_mix_alpha_cache_t cache;
    lv_color_mix_with_alpha_cache_init(&cache);

    int32_t x
    int32_t y;

    /* Convert source color to premultiplied */
    lv_color32_t color_argb = lv_color_to_32(dsc->color, opa);
    color_argb.red   = (color_argb.red   * color_argb.alpha) >> 8;
    color_argb.green = (color_argb.green * color_argb.alpha) >> 8;
    color_argb.blue  = (color_argb.blue  * color_argb.alpha) >> 8;


    /* Simple fill */
    if(mask == NULL && opa >= LV_OPA_MAX) {
        uint32_t color32 = lv_color_to_u32(dsc->color);
        uint32_t * dest_buf = dsc->dest_buf;
        for(y = 0; y < h; y++) {
            for(x = 0; x < w - 16; x += 16) {
                dest_buf[x + 0] = color32;
                dest_buf[x + 1] = color32;
                dest_buf[x + 2] = color32;
                dest_buf[x + 3] = color32;

                dest_buf[x + 4] = color32;
                dest_buf[x + 5] = color32;
                dest_buf[x + 6] = color32;
                dest_buf[x + 7] = color32;

                dest_buf[x + 8] = color32;
                dest_buf[x + 9] = color32;
                dest_buf[x + 10] = color32;
                dest_buf[x + 11] = color32;

                dest_buf[x + 12] = color32;
                dest_buf[x + 13] = color32;
                dest_buf[x + 14] = color32;
                dest_buf[x + 15] = color32;
            }
            for(; x < w; x ++) {
                dest_buf[x] = color32;
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        }
    }
    /* Opacity only */
    else if(mask == NULL && opa < LV_OPA_MAX) {
        lv_color32_t * dest_buf = dsc->dest_buf;
        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                dest_buf[x] = lv_color_32_32_mix_premul(color_argb, dest_buf[x], &cache);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        }
    }
    /* Masked fill */
    else if(mask && opa >= LV_OPA_MAX) {
        lv_color32_t * dest_buf = dsc->dest_buf;
        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                lv_color32_t color_premul = color_argb;
                color_premul.alpha = mask[x];
                color_premul.red   = (color_premul.red   * color_premul.alpha) >> 8;
                color_premul.green = (color_premul.green * color_premul.alpha) >> 8;
                color_premul.blue  = (color_premul.blue  * color_premul.alpha) >> 8;
                dest_buf[x] = lv_color_32_32_mix_premul(color_premul, dest_buf[x], &cache);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            mask += mask_stride;
        }
    }
    /* Masked with opacity */
    else {
        lv_color32_t * dest_buf = dsc->dest_buf;
        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                lv_color32_t color_premul = color_argb;
                color_premul.alpha = LV_OPA_MIX2(mask[x], opa);
                color_premul.red   = (color_premul.red   * color_premul.alpha) >> 8;
                color_premul.green = (color_premul.green * color_premul.alpha) >> 8;
                color_premul.blue  = (color_premul.blue  * color_premul.alpha) >> 8;
                dest_buf[x] = lv_color_32_32_mix_premul(color_premul, dest_buf[x], &cache);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            mask += mask_stride;
        }
    }
}

/**
 * @brief Blend an image into an ARGB8888 premultiplied buffer.
 *
 * This function blends an image stored in ARGB8888 premultiplied format
 * into the destination buffer. It accounts for opacity and optional masking.
 *
 * @param dsc Blending descriptor containing source and destination buffer information
 */
void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_image_to_argb8888_premultiplied(lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    lv_color32_t * dest_buf_c32 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const lv_color32_t * src_buf_c32 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    lv_color_mix_alpha_cache_t cache;
    lv_color_mix_with_alpha_cache_init(&cache);

    /* Iterate over every pixel in the image */
    for(int32_t y = 0; y < h; y++) {
        for(int32_t x = 0; x < w; x++) {
            lv_color32_t src = src_buf_c32[x];

            /* Convert from premultiplied alpha back to standard RGB */
            if(src.alpha > 0) {
                src.red   = (src.red   * 255) / src.alpha;
                src.green = (src.green * 255) / src.alpha;
                src.blue  = (src.blue  * 255) / src.alpha;
            }

            /* Apply mask and opacity adjustments */
            if(mask_buf) {
                src.alpha = LV_OPA_MIX2(src.alpha, mask_buf[x]);
            }
            else if(opa < LV_OPA_MAX) {
                src.alpha = LV_OPA_MIX2(src.alpha, opa);
            }

            /* Blend the source image pixel with the destination buffer */
            dest_buf_c32[x] = lv_color_32_32_mix_premul(src, dest_buf_c32[x], &cache);
        }

        /* Move to the next row in both the source and destination buffers */
        dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
        src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
        if(mask_buf) mask_buf += mask_stride;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief Mix two ARGB8888 premultiplied colors.
 *
 * This function blends the foreground (`fg`) and background (`bg`) colors.
 * The foreground color is assumed to be premultiplied.
 *
 * @param fg Foreground color (premultiplied alpha)
 * @param bg Background color
 * @param cache Alpha blending cache for optimization
 * @return lv_color32_t Blended color result
 */
static inline lv_color32_t lv_color_32_32_mix_premul(lv_color32_t fg, lv_color32_t bg,
                                                     lv_color_mix_alpha_cache_t * cache)
{
    /*Pick the foreground if it's fully opaque or the background is fully transparent*/
    if(fg.alpha >= LV_OPA_MAX || bg.alpha <= LV_OPA_MIN) {
        return fg;
    }
    /* Transparent foreground: use the background */
    else if(fg.alpha <= LV_OPA_MIN) {
        return bg;
    }
    /* Opaque background: use simple mix */
    else if(bg.alpha == 255) {
        return lv_color_mix32_premul(fg, bg);
    }
    else {
        /* Check cache to avoid redundant calculations */
        if(bg.alpha != cache->bg_saved.alpha || fg.alpha != cache->fg_saved.alpha) {
            /* Compute final alpha value */
            cache->res_alpha_saved = 255 - LV_OPA_MIX2(255 - fg.alpha, 255 - bg.alpha);
            LV_ASSERT(cache->res_alpha_saved != 0);

            /* Compute premultiplied blending ratio */
            cache->ratio_saved = (uint32_t)((uint32_t)fg.alpha * 255) / cache->res_alpha_saved;
        }

        /* Check if color blending is already cached */
        if(!lv_color32_eq(bg, cache->bg_saved) || !lv_color32_eq(fg, cache->fg_saved)) {
            cache->fg_saved = fg;
            cache->bg_saved = bg;

            /* Premultiply foreground color */
            lv_color32_t fg_premul = fg;
            fg_premul.red   = (fg.red   * fg.alpha) >> 8;
            fg_premul.green = (fg.green * fg.alpha) >> 8;
            fg_premul.blue  = (fg.blue  * fg.alpha) >> 8;

            /* Blend using premultiplied alpha */
            uint32_t inv_fg_alpha = 255 - fg.alpha;
            cache->res_saved.red   = fg_premul.red   + ((bg.red   * inv_fg_alpha) >> 8);
            cache->res_saved.green = fg_premul.green + ((bg.green * inv_fg_alpha) >> 8);
            cache->res_saved.blue  = fg_premul.blue  + ((bg.blue  * inv_fg_alpha) >> 8);
            cache->res_saved.alpha = cache->res_alpha_saved;
        }

        return cache->res_saved;
    }
}

static void lv_color_mix_with_alpha_cache_init(lv_color_mix_alpha_cache_t * cache)
{
    lv_memzero(&cache->fg_saved, sizeof(lv_color32_t));
    lv_memzero(&cache->bg_saved, sizeof(lv_color32_t));
    lv_memzero(&cache->res_saved, sizeof(lv_color32_t));
    cache->res_alpha_saved = 255;
    cache->ratio_saved = 255;
}


static inline void * LV_ATTRIBUTE_FAST_MEM drawbuf_next_row(const void * buf, uint32_t stride)
{
    return (void *)((uint8_t *)buf + stride);
}

static lv_color32_t lv_color_mix32_premul(lv_color32_t fg, lv_color32_t bg)
{
    if(fg.alpha >= LV_OPA_MAX) {
        return fg;  /* Fully opaque foreground replaces background */
    }
    if(fg.alpha <= LV_OPA_MIN) {
        return bg;  /* Fully transparent foreground, return background */
    }

    uint32_t inv_fg_alpha = LV_OPA_MAX - fg.alpha;

    /* Premultiplied blending */
    bg.red   = fg.red   + ((bg.red   * inv_fg_alpha) >> 8);
    bg.green = fg.green + ((bg.green * inv_fg_alpha) >> 8);
    bg.blue  = fg.blue  + ((bg.blue  * inv_fg_alpha) >> 8);
    bg.alpha = fg.alpha + ((bg.alpha * inv_fg_alpha) >> 8);

    return bg;
}




#endif /* LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED */

#endif /* LV_USE_DRAW_SW */