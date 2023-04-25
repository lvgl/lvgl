/**
 * @file lv_draw_sw_blend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW

#include "../../misc/lv_math.h"
#include "../../core/lv_disp.h"
#include "../../core/lv_refr.h"
#include LV_COLOR_EXTERN_INCLUDE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

LV_ATTRIBUTE_FAST_MEM static void fill_normal(lv_color_t * dest_buf, const lv_area_t * dest_area,
                                              lv_coord_t dest_stride, lv_color_t color, lv_opa_t opa, const lv_opa_t * mask, lv_coord_t mask_stride);


LV_ATTRIBUTE_FAST_MEM static void fill_argb8888(lv_color32_t * dest_buf, const lv_area_t * dest_area,
                                                lv_coord_t dest_stride, lv_color32_t color32, const lv_opa_t * mask, lv_coord_t mask_stride);


LV_ATTRIBUTE_FAST_MEM static void map_normal(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                             const lv_color_t * src_buf, lv_coord_t src_stride, lv_opa_t opa, const lv_opa_t * mask, lv_coord_t mask_stride);

LV_ATTRIBUTE_FAST_MEM static void map_to_argb8888(lv_color32_t * dest_buf, const lv_area_t * dest_area,
                                                  lv_coord_t dest_stride,
                                                  const lv_color_t * src_buf, lv_coord_t src_stride, lv_opa_t opa,
                                                  const lv_opa_t * mask, lv_coord_t mask_stride, lv_blend_mode_t blend_mode);

static void map_blended(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                        const lv_color_t * src_buf, lv_coord_t src_stride, lv_opa_t opa,
                        const lv_opa_t * mask, lv_coord_t mask_stride, lv_blend_mode_t blend_mode);


static inline lv_color32_t lv_color_mix_with_alpha_blend_mode(lv_color32_t fg, lv_color32_t bg,
                                                              lv_blend_mode_t blend_mode, lv_color_mix_alpha_cache_t * cache);

static inline lv_color_t color_blend_true_color_additive(lv_color_t fg, lv_color_t bg, lv_opa_t opa);
static inline lv_color_t color_blend_true_color_subtractive(lv_color_t fg, lv_color_t bg, lv_opa_t opa);
static inline lv_color_t color_blend_true_color_multiply(lv_color_t fg, lv_color_t bg, lv_opa_t opa);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#define FILL_NORMAL_MASK_PX(color)                                                          \
    if(*mask == LV_OPA_COVER) *dest_buf = color;                                 \
    else *dest_buf = LV_COLOR_MIX(color, *dest_buf, *mask);            \
    mask++;                                                         \
    dest_buf++;

#define MAP_NORMAL_MASK_PX(x)                                                          \
    if(*mask_tmp_x) {          \
        if(*mask_tmp_x == LV_OPA_COVER) dest_buf[x] = src_buf[x];                                 \
        else dest_buf[x] = LV_COLOR_MIX(src_buf[x], dest_buf[x], *mask_tmp_x);            \
    }                                                                                               \
    mask_tmp_x++;


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_blend(lv_draw_unit_t * draw_unit, const lv_draw_sw_blend_dsc_t * dsc)
{
    /*Do not draw transparent things*/
    if(dsc->opa <= LV_OPA_MIN) return;

    lv_layer_t * layer = draw_unit->target_layer;

    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_unit->clip_area)) return;

    const lv_opa_t * mask;
    if(dsc->mask_buf == NULL) mask = NULL;
    if(dsc->mask_buf && dsc->mask_res == LV_DRAW_SW_MASK_RES_TRANSP) return;
    else if(dsc->mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) mask = NULL;
    else mask = dsc->mask_buf;

    lv_coord_t dest_stride = lv_area_get_width(&layer->buf_area);

    lv_color_t * dest_buf = layer->buf;
    if(layer->color_format == LV_COLOR_FORMAT_NATIVE || layer->color_format == LV_COLOR_FORMAT_NATIVE_REVERSED) {
        dest_buf += dest_stride * (blend_area.y1 - layer->buf_area.y1) + (blend_area.x1 - layer->buf_area.x1);
    }
    else if(layer->color_format == LV_COLOR_FORMAT_ARGB8888) {
        lv_color32_t * dest_buf32 = (lv_color32_t *)dest_buf;

        dest_buf32 += dest_stride * (blend_area.y1 - layer->buf_area.y1);
        dest_buf32 += (blend_area.x1 - layer->buf_area.x1);
        dest_buf = (lv_color_t *)dest_buf32;
    }
    else {
        LV_LOG_WARN("Not supported color format");
        return;
    }


    const lv_color_t * src_buf = dsc->src_buf;
    lv_coord_t src_stride;
    if(src_buf) {
        src_stride = lv_area_get_width(dsc->blend_area);
        src_buf += src_stride * (blend_area.y1 - dsc->blend_area->y1) + (blend_area.x1 - dsc->blend_area->x1);
    }
    else {
        src_stride = 0;
    }

    lv_coord_t mask_stride;
    if(mask) {
        mask_stride = lv_area_get_width(dsc->mask_area);
        mask += mask_stride * (blend_area.y1 - dsc->mask_area->y1) + (blend_area.x1 - dsc->mask_area->x1);
    }
    else {
        mask_stride = 0;
    }

    lv_area_move(&blend_area, -layer->buf_area.x1, -layer->buf_area.y1);


    if(layer->color_format == LV_COLOR_FORMAT_ARGB8888) {
        if(dsc->src_buf == NULL) {
            lv_color32_t color32 = lv_color_to_xrgb8888(dsc->color);
            color32.alpha = dsc->opa;
            fill_argb8888(dest_buf, &blend_area, dest_stride, color32, mask, mask_stride);
        }
        else {
            map_to_argb8888(dest_buf, &blend_area, dest_stride, src_buf, src_stride, dsc->opa, mask, mask_stride, dsc->blend_mode);
        }
    }
    else {
        if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
            if(dsc->src_buf == NULL) {
                fill_normal(dest_buf, &blend_area, dest_stride, dsc->color, dsc->opa, mask, mask_stride);
            }
            else {
                map_normal(dest_buf, &blend_area, dest_stride, src_buf, src_stride, dsc->opa, mask, mask_stride);
            }
        }
        else {
            if(dsc->src_buf == NULL) {
                LV_LOG_WARN("Fill with non LV_BLEND_MODE_NORMAL is not supported");
            }
            else {
                map_blended(dest_buf, &blend_area, dest_stride, src_buf, src_stride, dsc->opa, mask, mask_stride, dsc->blend_mode);
            }
        }
    }
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Fill an area with a color.
 * Supports normal fill, fill with opacity, fill with mask, and fill with mask and opacity.
 * dest_buf and color have native color depth. (RGB565, RGB888, XRGB8888)
 * The background (dest_buf) cannot have alpha channel
 * @param dest_buf
 * @param dest_area
 * @param dest_stride
 * @param color
 * @param opa
 * @param mask
 * @param mask_stride
 */
LV_ATTRIBUTE_FAST_MEM static void fill_normal(lv_color_t * dest_buf, const lv_area_t * dest_area,
                                              lv_coord_t dest_stride, lv_color_t color, lv_opa_t opa, const lv_opa_t * mask, lv_coord_t mask_stride)
{
    int32_t w = lv_area_get_width(dest_area);
    int32_t h = lv_area_get_height(dest_area);

    int32_t x;
    int32_t y;

    if(mask == NULL) {
        /*No mask, full opacity*/
        if(opa >= LV_OPA_MAX) {
            for(y = 0; y < h; y++) {
#if LV_COLOR_DEPTH == 32
                color.alpha = 0xff; /*Jus to be sure*/
#endif
                lv_color_buf_fill(dest_buf, color, w);
                dest_buf += dest_stride;
            }
        }
        /*No mask with opacity*/
        else if(mask == NULL && opa < LV_OPA_MAX) {
            lv_color_t last_dest_color = lv_color_black();
            lv_color_t last_res_color = LV_COLOR_MIX(color, last_dest_color, opa);

#if LV_COLOR_MIX_ROUND_OFS == 0 && LV_COLOR_DEPTH == 16
            /*lv_color_mix work with an optimized algorithm with 16 bit color depth.
             *However, it introduces some rounded error on opa.
             *Introduce the same error here too to make lv_color_premult produces the same result */
            opa = (uint32_t)((uint32_t)opa + 4) >> 3;
            opa = opa << 3;
#endif

            uint16_t color_premult[3];
            LV_COLOR_PREMULT(color, opa, color_premult);
            lv_opa_t opa_inv = 255 - opa;

            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    if(!lv_color_eq(last_dest_color, dest_buf[x])) {
                        last_dest_color = dest_buf[x];
                        last_res_color = LV_COLOR_MIX_PREMULT(color_premult, dest_buf[x], opa_inv);
                    }
                    dest_buf[x] = last_res_color;
                }
                dest_buf += dest_stride;
            }
        }
    }
    /*Masked*/
    else {
        /*Only the mask matters*/
        if(opa >= LV_OPA_MAX) {
#if LV_COLOR_DEPTH == 16
            uint16_t cint16 = lv_color_to_int(color);
            uint32_t c32 = cint16 + ((uint32_t)cint16 << 16);
#endif
            int32_t x_end4 = w - 4;
            for(y = 0; y < h; y++) {
                for(x = 0; x < w && ((lv_uintptr_t)(mask) & 0x3); x++) {
                    FILL_NORMAL_MASK_PX(color)
                }

                for(; x <= x_end4; x += 4) {
                    uint32_t mask32 = *((uint32_t *)mask);
                    if(mask32 == 0xFFFFFFFF) {
#if LV_COLOR_DEPTH == 16
                        if((lv_uintptr_t)dest_buf & 0x3) {
                            *(dest_buf + 0) = color;
                            uint32_t * d = (uint32_t *)(dest_buf + 1);
                            *d = c32;
                            *(dest_buf + 3) = color;
                        }
                        else {
                            uint32_t * d = (uint32_t *)dest_buf;
                            *d = c32;
                            *(d + 1) = c32;
                        }
#else
                        dest_buf[0] = color;
                        dest_buf[1] = color;
                        dest_buf[2] = color;
                        dest_buf[3] = color;
#endif
                        dest_buf += 4;
                        mask += 4;
                    }
                    else if(mask32) {
                        FILL_NORMAL_MASK_PX(color)
                        FILL_NORMAL_MASK_PX(color)
                        FILL_NORMAL_MASK_PX(color)
                        FILL_NORMAL_MASK_PX(color)
                    }
                    else {
                        mask += 4;
                        dest_buf += 4;
                    }
                }

                for(; x < w ; x++) {
                    FILL_NORMAL_MASK_PX(color)
                }
                dest_buf += (dest_stride - w);
                mask += (mask_stride - w);
            }
        }
        /*With opacity*/
        else {
            /*Buffer the result color to avoid recalculating the same color*/
            lv_color_t last_dest_color;
            lv_color_t last_res_color;
            lv_opa_t last_mask = LV_OPA_TRANSP;
            last_dest_color = dest_buf[0];
            last_res_color = dest_buf[0];
            lv_opa_t opa_tmp = LV_OPA_TRANSP;

            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    if(*mask) {
                        if(*mask != last_mask) opa_tmp = *mask == LV_OPA_COVER ? opa :
                                                             (uint32_t)((uint32_t)(*mask) * opa) >> 8;
                        if(*mask != last_mask || !lv_color_eq(last_dest_color, dest_buf[x])) {
                            if(opa_tmp == LV_OPA_COVER) last_res_color = color;
                            else last_res_color = LV_COLOR_MIX(color, dest_buf[x], opa_tmp);
                            last_mask = *mask;
                            last_dest_color = dest_buf[x];
                        }
                        dest_buf[x] = last_res_color;
                    }
                    mask++;
                }
                dest_buf += dest_stride;
                mask += (mask_stride - w);
            }
        }
    }
}

/**
 * Fill an ARGB888 area with a color.
 * Supports normal fill, fill with opacity, fill with mask, and fill with mask and opacity.
 * dest_buf and the color have must have ARGB8888 format.
 * The background (dest_buf) can have alpha channel not set to 255.
 * @param dest_buf
 * @param dest_area
 * @param dest_stride
 * @param color
 * @param opa
 * @param mask
 * @param mask_stride
 */
LV_ATTRIBUTE_FAST_MEM static void fill_argb8888(lv_color32_t * dest_buf, const lv_area_t * dest_area,
                                                lv_coord_t dest_stride, lv_color32_t color32, const lv_opa_t * mask, lv_coord_t mask_stride)
{
    int32_t w = lv_area_get_width(dest_area);
    int32_t h = lv_area_get_height(dest_area);

    int32_t x;
    int32_t y;

    lv_color_mix_alpha_cache_t cache;
    lv_color_mix_with_alpha_cache_init(&cache);

    /*No mask*/
    if(mask == NULL) {
        /*No matter what the background is, overwrite it with fully covering color*/
        if(color32.alpha >= LV_OPA_MAX) {
            lv_color32_t * dest_buf_ori = dest_buf;
            for(x = 0; x < w; x++) {
                dest_buf[x] = color32;
            }

            for(y = 1; y < h; y++) {
                dest_buf += dest_stride;
                lv_memcpy(dest_buf, dest_buf_ori, w * sizeof(lv_color32_t));
            }
        }
        /*Has opacity*/
        else {
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    dest_buf[x] = lv_color_mix_with_alpha(dest_buf[x], color32, &cache);
                }
                dest_buf += dest_stride;
            }
        }
    }
    /*Masked*/
    else {
        /*Only the mask matters*/
        if(color32.alpha >= LV_OPA_MAX) {
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    color32.alpha = mask[x];
                    dest_buf[x] = lv_color_mix_with_alpha(dest_buf[x], color32, &cache);
                }
                dest_buf += dest_stride;
                mask += mask_stride;
            }
        }
        /*With opacity*/
        else {
            /*Buffer the result opacity to avoid recalculating the same color*/
            lv_opa_t last_mask = LV_OPA_TRANSP;
            lv_opa_t opa_original = color32.alpha;

            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    if(mask[x]) {
                        if(mask[x] != last_mask) {
                            color32.alpha = mask[x] == LV_OPA_COVER ?
                                            opa_original : (uint32_t)((uint32_t)(mask[x]) * opa_original) >> 8;
                        }

                        dest_buf[x] = lv_color_mix_with_alpha(dest_buf[x], color32, &cache);
                    }
                }
                dest_buf += dest_stride;
                mask += mask_stride;
            }
        }
    }
}

#endif

/**
 * Blend a pixel array (map) to destination buffer.
 * Supports normal copy, copy with opacity, copy with mask, and copy with mask and opacity.
 * dest_buf and color have native color depth. (RGB565, RGB888, XRGB8888)
 * The background (dest_buf) cannot have alpha channel
 * @param dest_buf
 * @param dest_area
 * @param dest_stride
 * @param src_buf
 * @param src_stride
 * @param opa
 * @param mask
 * @param mask_stride
 */
LV_ATTRIBUTE_FAST_MEM static void map_normal(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                             const lv_color_t * src_buf, lv_coord_t src_stride, lv_opa_t opa, const lv_opa_t * mask, lv_coord_t mask_stride)

{
    int32_t w = lv_area_get_width(dest_area);
    int32_t h = lv_area_get_height(dest_area);

    int32_t x;
    int32_t y;

    /*Simple fill (maybe with opacity), no masking*/
    if(mask == NULL) {
        if(opa >= LV_OPA_MAX) {
            for(y = 0; y < h; y++) {
                lv_memcpy(dest_buf, src_buf, w * sizeof(lv_color_t));
                dest_buf += dest_stride;
                src_buf += src_stride;
            }
        }
        else {
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    dest_buf[x] = LV_COLOR_MIX(src_buf[x], dest_buf[x], opa);
                }
                dest_buf += dest_stride;
                src_buf += src_stride;
            }
        }
    }
    /*Masked*/
    else {
        /*Only the mask matters*/
        if(opa > LV_OPA_MAX) {
            int32_t x_end4 = w - 4;

            for(y = 0; y < h; y++) {
                const lv_opa_t * mask_tmp_x = mask;
#if 0
                for(x = 0; x < w; x++) {
                    MAP_NORMAL_MASK_PX(x);
                }
#else
                for(x = 0; x < w && ((lv_uintptr_t)mask_tmp_x & 0x3); x++) {
                    MAP_NORMAL_MASK_PX(x)
                }

                uint32_t * mask32 = (uint32_t *)mask_tmp_x;
                for(; x < x_end4; x += 4) {
                    if(*mask32) {
                        if((*mask32) == 0xFFFFFFFF) {
                            dest_buf[x] = src_buf[x];
                            dest_buf[x + 1] = src_buf[x + 1];
                            dest_buf[x + 2] = src_buf[x + 2];
                            dest_buf[x + 3] = src_buf[x + 3];
                        }
                        else {
                            mask_tmp_x = (const lv_opa_t *)mask32;
                            MAP_NORMAL_MASK_PX(x)
                            MAP_NORMAL_MASK_PX(x + 1)
                            MAP_NORMAL_MASK_PX(x + 2)
                            MAP_NORMAL_MASK_PX(x + 3)
                        }
                    }
                    mask32++;
                }

                mask_tmp_x = (const lv_opa_t *)mask32;
                for(; x < w ; x++) {
                    MAP_NORMAL_MASK_PX(x)
                }
#endif
                dest_buf += dest_stride;
                src_buf += src_stride;
                mask += mask_stride;
            }
        }
        /*Handle opa and mask values too*/
        else {
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    if(mask[x]) {
                        lv_opa_t opa_tmp = mask[x] >= LV_OPA_MAX ? opa : ((opa * mask[x]) >> 8);
                        dest_buf[x] = LV_COLOR_MIX(src_buf[x], dest_buf[x], opa_tmp);
                    }
                }
                dest_buf += dest_stride;
                src_buf += src_stride;
                mask += mask_stride;
            }
        }
    }
}

/**
 * Blend a pixel array (map) to an ARGB888 destination buffer.
 * Supports normal copy, copy with opacity, copy with mask, and copy with mask and opacity.
 * The source image always has native RGB format (RGB565, RGB888 or XRGB8888)
 * @param dest_buf
 * @param dest_area
 * @param dest_stride
 * @param src_buf
 * @param src_stride
 * @param opa
 * @param mask
 * @param mask_stride
 * @param blend_mode
 */
LV_ATTRIBUTE_FAST_MEM static void map_to_argb8888(lv_color32_t * dest_buf, const lv_area_t * dest_area,
                                                  lv_coord_t dest_stride,
                                                  const lv_color_t * src_buf, lv_coord_t src_stride, lv_opa_t opa,
                                                  const lv_opa_t * mask, lv_coord_t mask_stride, lv_blend_mode_t blend_mode)

{
    int32_t w = lv_area_get_width(dest_area);
    int32_t h = lv_area_get_height(dest_area);

    int32_t x;
    int32_t y;
    lv_color32_t d;

    lv_color_mix_alpha_cache_t cache;
    lv_color_mix_with_alpha_cache_init(&cache);

    if(mask == NULL) {
        if(opa >= LV_OPA_MAX) {
            /*Simple image copy*/
            if(blend_mode == LV_BLEND_MODE_NORMAL) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        dest_buf[x] = lv_color_to_xrgb8888(src_buf[x]);
                    }
                    dest_buf += dest_stride;
                    src_buf += src_stride;
                }
            }
            else {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        d = lv_color_to_xrgb8888(src_buf[x]);
                        dest_buf[x] = lv_color_mix_with_alpha_blend_mode(d, dest_buf[x], blend_mode, &cache);
                    }
                    dest_buf += dest_stride;
                    src_buf += src_stride;
                }
            }
        }

        /*Opacity without mask*/
        else {
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    d = lv_color_to_xrgb8888(src_buf[x]);
                    d.alpha = opa;
                    dest_buf[x] = lv_color_mix_with_alpha_blend_mode(d, dest_buf[x], blend_mode, &cache);
                }
                dest_buf += dest_stride;
                src_buf += src_stride;
            }
        }
    }
    else {
        /*Mask but no opa*/
        if(opa >= LV_OPA_MAX) {
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    d = lv_color_to_xrgb8888(src_buf[x]);
                    d.alpha = mask[x];
                    dest_buf[x] = lv_color_mix_with_alpha_blend_mode(d, dest_buf[x], blend_mode, &cache);
                }
                mask += mask_stride;
                dest_buf += dest_stride;
                src_buf += src_stride;
            }
        }
        /*Both mask and opa*/
        else {
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    lv_opa_t opa_tmp = mask[x] >= LV_OPA_MAX ? opa : ((opa * mask[x]) >> 8);
                    d = lv_color_to_xrgb8888(src_buf[x]);
                    d.alpha = opa_tmp;
                    dest_buf[x] = lv_color_mix_with_alpha(d, dest_buf[x], &cache);
                }
                mask += mask_stride;
                dest_buf += dest_stride;
                src_buf += src_stride;

            }
        }
    }
}

static inline lv_color32_t lv_color_mix_with_alpha_blend_mode(lv_color32_t fg, lv_color32_t bg,
                                                              lv_blend_mode_t blend_mode, lv_color_mix_alpha_cache_t * cache)
{
    /*Get the result color*/
    if(!lv_color32_eq(bg, cache->bg_saved) || !lv_color32_eq(fg, cache->fg_saved)) {
        switch(blend_mode) {
            case LV_BLEND_MODE_NORMAL:
                return lv_color_mix_with_alpha(bg, fg, cache);

            case LV_BLEND_MODE_ADDITIVE:
                cache->res_saved.red = LV_MIN(bg.red + fg.red, 255);
                cache->res_saved.green = LV_MIN(bg.green + fg.green, 255);
                cache->res_saved.blue = LV_MIN(bg.blue + fg.blue, 255);
                break;
            case LV_BLEND_MODE_MULTIPLY:
                cache->res_saved.red = (fg.red * bg.red) >> 8;
                cache->res_saved.green = (fg.green * bg.green) >> 8;
                cache->res_saved.blue = (fg.blue * bg.blue) >> 8;
                break;
            case LV_BLEND_MODE_SUBTRACTIVE:
                cache->res_saved.red = LV_MAX((int32_t)bg.red - fg.red, 0);
                cache->res_saved.green = LV_MAX((int32_t)bg.green - fg.green, 0);
                cache->res_saved.blue = LV_MAX((int32_t)bg.blue - fg.blue, 0);
                break;
        }

        cache->bg_saved = bg;
        cache->fg_saved = fg;

        if(fg.alpha >= LV_OPA_MAX) {
            cache->res_saved.alpha = bg.alpha;
        }
        else {
            cache->res_saved = lv_color_mix32(fg, bg);
        }
        /*Always keep the original bg.alpha*/
        cache->res_saved.alpha = cache->bg_saved.alpha;
    }

    return cache->res_saved;
}



static void map_blended(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                        const lv_color_t * src_buf, lv_coord_t src_stride, lv_opa_t opa,
                        const lv_opa_t * mask, lv_coord_t mask_stride, lv_blend_mode_t blend_mode)
{

    int32_t w = lv_area_get_width(dest_area);
    int32_t h = lv_area_get_height(dest_area);

    int32_t x;
    int32_t y;

    lv_color_t (*blend_fp)(lv_color_t, lv_color_t, lv_opa_t);
    switch(blend_mode) {
        case LV_BLEND_MODE_ADDITIVE:
            blend_fp = color_blend_true_color_additive;
            break;
        case LV_BLEND_MODE_SUBTRACTIVE:
            blend_fp = color_blend_true_color_subtractive;
            break;
        case LV_BLEND_MODE_MULTIPLY:
            blend_fp = color_blend_true_color_multiply;
            break;
        default:
            LV_LOG_WARN("unsupported blend mode");
            return;
    }

    lv_color_t last_dest_color;
    lv_color_t last_src_color;
    /*Simple fill (maybe with opacity), no masking*/
    if(mask == NULL) {
        last_dest_color = dest_buf[0];
        last_src_color = src_buf[0];
        lv_color_t last_res_color = blend_fp(last_src_color, last_dest_color, opa);
        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                if(!lv_color_eq(last_src_color, src_buf[x]) || !lv_color_eq(last_dest_color, dest_buf[x])) {
                    last_dest_color = dest_buf[x];
                    last_src_color = src_buf[x];
                    last_res_color = blend_fp(last_src_color, last_dest_color, opa);
                }
                dest_buf[x] = last_res_color;
            }
            dest_buf += dest_stride;
            src_buf += src_stride;
        }
    }
    /*Masked*/
    else {
        last_dest_color = dest_buf[0];
        last_src_color = src_buf[0];
        lv_opa_t last_opa = mask[0] >= LV_OPA_MAX ? opa : ((opa * mask[0]) >> 8);
        lv_color_t last_res_color = blend_fp(last_src_color, last_dest_color, last_opa);
        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                if(mask[x] == 0) continue;
                lv_opa_t opa_tmp = mask[x] >= LV_OPA_MAX ? opa : ((opa * mask[x]) >> 8);
                if(!lv_color_eq(last_src_color, src_buf[x]) || !lv_color_eq(last_dest_color, dest_buf[x]) || last_opa != opa_tmp) {
                    last_dest_color = dest_buf[x];
                    last_src_color = src_buf[x];
                    last_opa = opa_tmp;
                    last_res_color = blend_fp(last_src_color, last_dest_color, last_opa);
                }
                dest_buf[x] = last_res_color;
            }
            dest_buf += dest_stride;
            src_buf += src_stride;
            mask += mask_stride;
        }
    }
}

static inline lv_color_t color_blend_true_color_additive(lv_color_t fg, lv_color_t bg, lv_opa_t opa)
{

    if(opa <= LV_OPA_MIN) return bg;

#if LV_COLOR_DEPTH == 8
    fg.red = LV_MIN(bg.level + fg.level, 7);
#elif LV_COLOR_DEPTH == 16
    fg.red = LV_MIN(bg.red + fg.red, 31);
    fg.green = LV_MIN(bg.green + fg.green, 63);
    fg.blue = LV_MIN(bg.blue + fg.blue, 31);
#elif LV_COLOR_DEPTH == 32 || LV_COLOR_DEPTH == 24
    fg.red = LV_MIN(bg.red + fg.red, 255);
    fg.green = LV_MIN(bg.green + fg.green, 255);
    fg.blue = LV_MIN(bg.blue + fg.blue, 255);
#endif

    if(opa == LV_OPA_COVER) return fg;
    else return LV_COLOR_MIX(fg, bg, opa);
}

static inline lv_color_t color_blend_true_color_subtractive(lv_color_t fg, lv_color_t bg, lv_opa_t opa)
{
    if(opa <= LV_OPA_MIN) return bg;

    fg.red = LV_MAX((int32_t)bg.red - fg.red, 0);
    fg.green = LV_MAX((int32_t)bg.green - fg.green, 0);
    fg.blue = LV_MAX((int32_t)bg.blue - fg.blue, 0);

    if(opa == LV_OPA_COVER) return fg;

    return LV_COLOR_MIX(fg, bg, opa);
}

static inline lv_color_t color_blend_true_color_multiply(lv_color_t fg, lv_color_t bg, lv_opa_t opa)
{
    if(opa <= LV_OPA_MIN) return bg;

#if LV_COLOR_DEPTH == 32
    fg.red = (fg.red * bg.red) >> 8;
    fg.green = (fg.green * bg.green) >> 8;
    fg.blue = (fg.blue * bg.blue) >> 8;
#elif LV_COLOR_DEPTH == 16
    fg.red = (fg.red * bg.red) >> 5;
    fg.green = (fg.green * bg.green) >> 6;
    fg.blue = (fg.blue * bg.blue) >> 5;
#elif LV_COLOR_DEPTH == 8
    fg.red = (fg.red * bg.red) >> 3;
    fg.green = (fg.green * bg.green) >> 3;
    fg.blue = (fg.blue * bg.blue) >> 2;
#endif

    if(opa == LV_OPA_COVER) return fg;

    return LV_COLOR_MIX(fg, bg, opa);
}
