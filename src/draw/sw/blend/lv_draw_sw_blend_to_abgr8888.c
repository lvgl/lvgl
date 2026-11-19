/**
 * @file lv_draw_sw_blend_to_abgr8888.c
 *
 * Blending to ABGR8888: 0xAABBGGRR as a 32 bit word, i.e. R, G, B, A in memory on little
 * endian targets. It's the layout of a WebAssembly/HTML canvas `ImageData`.
 * The algorithm is the one of ARGB8888, only the pixel's byte order differs.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_to_abgr8888.h"
#if LV_USE_DRAW_SW

#if LV_DRAW_SW_SUPPORT_ABGR8888

#include "lv_draw_sw_blend_private.h"

/*********************
 *  TEMPLATE INSTANCE
 *********************/

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} dest_px_t;

#define LV_BLEND_COLOR_FN       lv_draw_sw_blend_color_to_abgr8888
#define LV_BLEND_IMAGE_FN       lv_draw_sw_blend_image_to_abgr8888

/*The NEON, Helium, etc. hooks are written for ARGB8888, so use the C implementation*/
#define LV_BLEND_NO_HOOK(...)   LV_RESULT_INVALID
#define LV_BLEND_HOOK(name)     LV_BLEND_NO_HOOK

#define DEST_PX_IS_ARGB8888     0

static inline dest_px_t LV_ATTRIBUTE_FAST_MEM dest_px_from_c32(lv_color32_t c)
{
    dest_px_t res;
    res.red = c.red;
    res.green = c.green;
    res.blue = c.blue;
    res.alpha = c.alpha;
    return res;
}

static inline dest_px_t LV_ATTRIBUTE_FAST_MEM dest_px_from_color(lv_color_t c, lv_opa_t opa)
{
    dest_px_t res;
    res.red = c.red;
    res.green = c.green;
    res.blue = c.blue;
    res.alpha = opa;
    return res;
}

static inline uint32_t LV_ATTRIBUTE_FAST_MEM dest_px_u32_from_color(lv_color_t c)
{
    dest_px_t px = dest_px_from_color(c, 0xff);
    uint32_t u32;
    lv_memcpy(&u32, &px, sizeof(u32));
    return u32;
}

static inline bool LV_ATTRIBUTE_FAST_MEM dest_px_eq(dest_px_t c1, dest_px_t c2)
{
    return c1.red == c2.red && c1.green == c2.green && c1.blue == c2.blue && c1.alpha == c2.alpha;
}

/**
 * lv_color_mix32_inlined() for ABGR8888
 */
static inline dest_px_t LV_ATTRIBUTE_FAST_MEM dest_px_mix(dest_px_t fg, dest_px_t bg)
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

#include "lv_draw_sw_blend_to_argb8888.inc"

#endif /* LV_DRAW_SW_SUPPORT_ABGR8888 */

#endif /* LV_USE_DRAW_SW */
