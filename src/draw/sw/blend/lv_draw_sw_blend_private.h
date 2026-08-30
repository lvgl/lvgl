/**
 * @file lv_draw_sw_blend_private.h
 *
 */

#ifndef LV_DRAW_SW_BLEND_PRIVATE_H
#define LV_DRAW_SW_BLEND_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_sw_blend.h"
#if LV_USE_DRAW_SW

#include "../lv_draw_sw_mask.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_sw_blend_dsc_t {
    const lv_area_t * blend_area;   /**< The area with absolute coordinates to draw on `layer->buf`
                                     *   will be clipped to `layer->clip_area` */
    const void * src_buf;           /**< Pointer to an image to blend. If set `fill_color` is ignored */
    uint32_t src_stride;
    lv_color_format_t src_color_format;
    const lv_area_t * src_area;
    lv_opa_t opa;                   /**< The overall opacity*/
    lv_color_t color;               /**< Fill color*/
    const lv_opa_t * mask_buf;      /**< NULL if ignored, or an alpha mask to apply on `blend_area`*/
    lv_draw_sw_mask_res_t mask_res; /**< The result of the previous mask operation */
    const lv_area_t * mask_area;    /**< The area of `mask_buf` with absolute coordinates*/
    int32_t mask_stride;
    lv_blend_mode_t blend_mode;     /**< E.g. LV_BLEND_MODE_ADDITIVE*/
};

struct _lv_draw_sw_blend_fill_dsc_t {
    void * dest_buf;
    int32_t dest_w;
    int32_t dest_h;
    int32_t dest_stride;
    const lv_opa_t * mask_buf;
    int32_t mask_stride;
    lv_color_t color;
    lv_opa_t opa;
    lv_area_t relative_area;
};

struct _lv_draw_sw_blend_image_dsc_t {
    void * dest_buf;
    int32_t dest_w;
    int32_t dest_h;
    int32_t dest_stride;
    const lv_opa_t * mask_buf;
    int32_t mask_stride;
    const void * src_buf;
    int32_t src_stride;
    lv_color_format_t src_color_format;
    lv_opa_t opa;
    lv_blend_mode_t blend_mode;
    lv_area_t relative_area;    /**< The blend area relative to the layer's buffer area. */
    lv_area_t src_area;             /**< The original src area. */
};


/**********************
 *      MACROS
 **********************/

/**
 * Read or write four bytes of a mask or pixel buffer as one word. A plain `uint32_t *` cast
 * breaks the aliasing rules and optimizers do act on that, so where the compiler offers
 * `may_alias` the access goes through it: same single instruction, without the assumption.
 * `__builtin_memcpy` would also be correct but RISC-V expands it to four byte loads, which
 * costs more than the batching saves.
 */
#if defined(__GNUC__) || defined(__clang__)
typedef uint32_t lv_u32_alias_t __attribute__((__may_alias__));
#define LV_LOAD_U32(res, src)  do { (res) = *((const lv_u32_alias_t *)(src)); } while(0)
#define LV_STORE_U32(dst, val) do { *((lv_u32_alias_t *)(dst)) = (val); } while(0)
#else
#define LV_LOAD_U32(res, src)  do { (res) = *((const uint32_t *)(src)); } while(0)
#define LV_STORE_U32(dst, val) do { *((uint32_t *)(dst)) = (val); } while(0)
#endif

/**
 * Prepare an RGB565 color for mixing: spread it over 32 bits so each channel gets room to
 * grow and a whole pixel can be mixed with one multiplication.
 */
#define LV_COLOR_MIX_16_PREPARE(c) ((((uint32_t)(c)) | (((uint32_t)(c)) << 16)) & 0x07E0F81Fu)

/**
 * Convert a blue, green, red byte triplet to RGB565.
 */
#define LV_COLOR_24_TO_16(c1) \
    ((uint16_t)((((c1)[2] & 0xF8) << 8) + (((c1)[1] & 0xFC) << 3) + (((c1)[0] & 0xF8) >> 3)))

/**
 * Mix a prepared foreground into an RGB565 background. `mix` must be 1..254, the
 * callers handle 0 and 255 themselves. `res` may be the same variable as `bg`.
 * A macro and not a function because -Os inlines neither, and this runs on every pixel.
 */
#define LV_COLOR_MIX_16_TO_16_PREPARED(res, fg_prep, bg, mix)                               \
    do {                                                                                    \
        uint32_t bg_prep_ = LV_COLOR_MIX_16_PREPARE(bg);                                    \
        uint32_t mix5_ = ((uint32_t)(mix) + 4) >> 3;                                        \
        uint32_t res_ = (((((fg_prep) - bg_prep_) * mix5_) >> 5) + bg_prep_) & 0x07E0F81Fu; \
        (res) = (uint16_t)((res_ >> 16) | res_);                                            \
    } while(0)

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Undo the alpha scaling of a premultiplied pixel, giving back a straight color.
 * Shared so the output formats that need it can't drift apart.
 * @param c     a premultiplied pixel
 * @return      the same pixel with straight channels
 */
static inline lv_color32_t LV_ATTRIBUTE_FAST_MEM lv_color32_unpremultiply(lv_color32_t c)
{
    if(c.alpha == 0) {
        c.red = 0;
        c.green = 0;
        c.blue = 0;
    }
    else if(c.alpha != LV_OPA_COVER) {
        uint32_t reciprocal = (255u * 256u) / c.alpha;
        uint32_t r = (c.red * reciprocal) >> 8;
        uint32_t g = (c.green * reciprocal) >> 8;
        uint32_t b = (c.blue * reciprocal) >> 8;
        c.red = (uint8_t)(r > 255 ? 255 : r);
        c.green = (uint8_t)(g > 255 ? 255 : g);
        c.blue = (uint8_t)(b > 255 ? 255 : b);
    }

    return c;
}

/**
 * Luminance of an image pixel for the grayscale and indexed outputs. A premultiplied source
 * has its channels already scaled by its alpha, so undo that first to get the same value a
 * straight source would give.
 * @param c                 the pixel
 * @param premultiplied     true: `c` is premultiplied
 * @return                  the luminance, 0..255
 */
static inline uint8_t LV_ATTRIBUTE_FAST_MEM lv_color32_lumi_of(lv_color32_t c, bool premultiplied)
{
    if(!premultiplied || c.alpha == 0 || c.alpha == LV_OPA_COVER) return lv_color32_luminance(c);

    uint32_t lumi = (lv_color32_luminance(c) * ((255u * 256u) / c.alpha)) >> 8;
    return (uint8_t)(lumi > 255 ? 255 : lumi);
}

/**
 * Inlined version of lv_color_16_16_mix() for the per-pixel blend loops.
 * Identical to the public function, but being in a header it can be inlined
 * and loop-invariant parts (e.g. a constant fill color) can be hoisted out of the loops.
 * @param c1        the first color (typically the foreground color)
 * @param c2        the second color (typically the background color)
 * @param mix       0..255, the opacity of `c1`
 * @return          the mixed color
 */
static inline uint16_t LV_ATTRIBUTE_FAST_MEM lv_color_16_16_mix_inlined(uint16_t c1, uint16_t c2, uint8_t mix)
{
    if(mix == 255) return c1;
    if(mix == 0) return c2;
    if(c1 == c2) return c1;

    /* Source: https://stackoverflow.com/a/50012418/1999969*/
    uint32_t mix5 = ((uint32_t)mix + 4) >> 3;

    /*0x7E0F81F = 0b00000111111000001111100000011111*/
    uint32_t bg = (uint32_t)(c2 | ((uint32_t)c2 << 16)) & 0x7E0F81F;
    uint32_t fg = (uint32_t)(c1 | ((uint32_t)c1 << 16)) & 0x7E0F81F;
    uint32_t result = ((((fg - bg) * mix5) >> 5) + bg) & 0x7E0F81F;

    return (uint16_t)((result >> 16) | result);
}

/**
 * Inlined version of lv_color_mix32() for the per-pixel blend loops.
 * Identical to the public function, but being in a header it can be inlined,
 * sparing a function call per pixel.
 * The channels are mixed independently on purpose: superscalar CPUs can compute
 * them in parallel, which is faster than packing them into one 32 bit value.
 * @param fg        the foreground color, fg.alpha is the mix ratio
 * @param bg        the background color
 * @return          the mixed color, the alpha of `bg` is kept
 */
static inline lv_color32_t LV_ATTRIBUTE_FAST_MEM lv_color_mix32_inlined(lv_color32_t fg, lv_color32_t bg)
{
    if(fg.alpha >= LV_OPA_MAX) {
        fg.alpha = bg.alpha;
        return fg;
    }
    if(fg.alpha <= LV_OPA_MIN) {
        return bg;
    }

    uint32_t mix_inv = 255 - fg.alpha;
    bg.red = (uint8_t)LV_UDIV255((uint32_t)fg.red * fg.alpha + (uint32_t)bg.red * mix_inv);
    bg.green = (uint8_t)LV_UDIV255((uint32_t)fg.green * fg.alpha + (uint32_t)bg.green * mix_inv);
    bg.blue = (uint8_t)LV_UDIV255((uint32_t)fg.blue * fg.alpha + (uint32_t)bg.blue * mix_inv);
    return bg;
}

/**
 * Inlined version of lv_color_mix32_premultiplied() for the per-pixel blend loops.
 * The premultiplied foreground is added as is and the background is weighted
 * with the remaining alpha.
 * @param fg        the premultiplied foreground color, fg.alpha is the mix ratio
 * @param bg        the background color
 * @return          the mixed color with the alpha of `bg`, except for an (almost) opaque
 *                  foreground where `fg` is returned as it is, so its alpha is kept
 */
static inline lv_color32_t LV_ATTRIBUTE_FAST_MEM lv_color_mix32_premultiplied_inlined(lv_color32_t fg,
                                                                                      lv_color32_t bg)
{
    if(fg.alpha >= LV_OPA_MAX) {
        return fg;
    }
    if(fg.alpha <= LV_OPA_MIN) {
        return bg;
    }

    uint32_t mix_inv = LV_OPA_MAX - fg.alpha;
    bg.red = (uint8_t)(fg.red + (((uint32_t)bg.red * mix_inv) >> 8));
    bg.green = (uint8_t)(fg.green + (((uint32_t)bg.green * mix_inv) >> 8));
    bg.blue = (uint8_t)(fg.blue + (((uint32_t)bg.blue * mix_inv) >> 8));
    return bg;
}

#endif /* LV_USE_DRAW_SW */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_BLEND_PRIVATE_H*/
