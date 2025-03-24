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
    lv_color32_t * dest_buf = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const lv_opa_t * mask = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    lv_color_mix_alpha_cache_t cache;
    lv_color_mix_with_alpha_cache_init(&cache);

    /* Convert the input color to premultiplied alpha */
    lv_color32_t premul_color = lv_color_to_32(dsc->color, opa);
    if(opa < 255) {
        premul_color.red   = (premul_color.red   * opa) / 255;
        premul_color.green = (premul_color.green * opa) / 255;
        premul_color.blue  = (premul_color.blue  * opa) / 255;
    }

    /* Iterate over each pixel in the destination buffer */
    for(int32_t y = 0; y < h; y++) {
        for(int32_t x = 0; x < w; x++) {
            /* If a mask is present, modify the alpha accordingly */
            if(mask) {
                premul_color.alpha = (premul_color.alpha * mask[x]) / 255;
            }
            /* Blend the premultiplied color with the destination buffer */
            dest_buf[x] = lv_color_32_32_mix_premul(premul_color, dest_buf[x], &cache);
        }
        /* Move to the next row in the buffer */
        dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        if(mask) mask += mask_stride;
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
            else if(opa < 255) {
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
    LV_UNUSED(cache);

    /* If foreground is fully opaque, return it directly */
    if(fg.alpha == 255) return fg;

    /* If foreground is fully transparent, return the background */
    if(fg.alpha == 0) return bg;

    lv_color32_t res;

    /* Compute resulting alpha channel */
    res.alpha = 255 - ((255 - fg.alpha) * (255 - bg.alpha) / 255);

    /* Perform alpha blending on RGB channels */
    res.red   = fg.red   + ((bg.red   * (255 - fg.alpha)) / 255);
    res.green = fg.green + ((bg.green * (255 - fg.alpha)) / 255);
    res.blue  = fg.blue  + ((bg.blue  * (255 - fg.alpha)) / 255);

    return res;
}

void lv_color_mix_with_alpha_cache_init(lv_color_mix_alpha_cache_t * cache)
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


#endif /* LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED */

#endif /* LV_USE_DRAW_SW */