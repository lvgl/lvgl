/**
 * @file lv_draw_sw_transform.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#include "blend/lv_draw_sw_blend_private.h"
#if LV_USE_DRAW_SW

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    int32_t x_in;
    int32_t y_in;
    int32_t x_out;
    int32_t y_out;
    int32_t sinma;
    int32_t cosma;
    int32_t scale_x;
    int32_t scale_y;
    int32_t angle;
    int32_t pivot_x_256;
    int32_t pivot_y_256;
    lv_point_t pivot;
} point_transform_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
/**
 * Find the sub-range of [x_from, x_to) where lo <= base + ((step * x) >> 8) <= hi.
 * As the coordinate is a linear (monotonic) function of `x` the result is an interval.
 */
static void transform_safe_interval(int32_t base, int32_t step, int32_t lo, int32_t hi,
                                    int32_t x_from, int32_t x_to, int32_t * res_from, int32_t * res_to);

/**
 * Find the destination x range where the source coordinates of the pixels
 * (and with anti-aliasing their neighbors too) are surely inside the source image,
 * so neither bounds checking nor edge handling is needed for them.
 */
static void transform_safe_range(int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                                 int32_t src_w, int32_t src_h, int32_t x_from, int32_t x_to,
                                 bool aa, int32_t * res_from, int32_t * res_to);

#if LV_DRAW_SW_SUPPORT_RGB888 || LV_DRAW_SW_SUPPORT_XRGB8888
static void transform_rgb888(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                             int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                             int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                             uint8_t * dest_buf, bool aa, uint32_t px_size);
#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888 || LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
/**
 * Undo the alpha scaling of a premultiplied pixel, giving back a straight color.
 */
static lv_color32_t unpremultiply(lv_color32_t c)
{
    if(c.alpha == 0) {
        c.red = 0;
        c.green = 0;
        c.blue = 0;
    }
    else {
        uint16_t reciprocal_alpha = (255 * 256) / c.alpha;
        c.red = (c.red * reciprocal_alpha) >> 8;
        c.green = (c.green * reciprocal_alpha) >> 8;
        c.blue = (c.blue  * reciprocal_alpha) >> 8;
    }

    return c;
}

#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888
static void transform_argb8888(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                               int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                               int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                               uint8_t * dest_buf, bool aa);
#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
static void transform_argb8888_premultiplied(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                                             int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                                             int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                                             uint8_t * dest_buf, bool aa);
#endif

#if LV_DRAW_SW_SUPPORT_RGB565A8
static void transform_rgb565a8(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                               int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                               int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                               uint16_t * cbuf, uint8_t * abuf, bool src_has_a8, bool aa);
#endif

#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
static void transform_rgb565a8_swapped(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                                       int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                                       int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                                       uint16_t * cbuf, uint8_t * abuf, bool src_has_a8, bool aa);
#endif

#if LV_DRAW_SW_SUPPORT_A8
static void transform_a8(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                         int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                         int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                         uint8_t * abuf, bool aa);
#endif

#if LV_DRAW_SW_SUPPORT_L8 || LV_DRAW_SW_SUPPORT_AL88
static void transform_al88(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                           int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                           int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                           uint8_t * cbuf, uint8_t * abuf, bool src_has_a8, bool aa);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_transform(const lv_area_t * dest_area, const void * src_buf,
                          int32_t src_w, int32_t src_h, int32_t src_stride,
                          const lv_draw_image_dsc_t * draw_dsc, const lv_draw_image_sup_t * sup, lv_color_format_t src_cf, void * dest_buf)
{
    LV_UNUSED(sup);

    point_transform_dsc_t tr_dsc;
    tr_dsc.angle = -draw_dsc->rotation;
    tr_dsc.scale_x = draw_dsc->scale_x;
    tr_dsc.scale_y = draw_dsc->scale_y;
    tr_dsc.pivot = draw_dsc->pivot;

    int32_t angle_low = tr_dsc.angle / 10;
    int32_t angle_high = angle_low + 1;
    int32_t angle_rem = tr_dsc.angle  - (angle_low * 10);

    int32_t s1 = lv_trigo_sin(angle_low);
    int32_t s2 = lv_trigo_sin(angle_high);

    int32_t c1 = lv_trigo_sin(angle_low + 90);
    int32_t c2 = lv_trigo_sin(angle_high + 90);

    tr_dsc.sinma = (s1 * (10 - angle_rem) + s2 * angle_rem) / 10;
    tr_dsc.cosma = (c1 * (10 - angle_rem) + c2 * angle_rem) / 10;
    tr_dsc.sinma = tr_dsc.sinma >> (LV_TRIGO_SHIFT - 10);
    tr_dsc.cosma = tr_dsc.cosma >> (LV_TRIGO_SHIFT - 10);
    tr_dsc.pivot_x_256 = tr_dsc.pivot.x * 256;
    tr_dsc.pivot_y_256 = tr_dsc.pivot.y * 256;

    int32_t dest_w = lv_area_get_width(dest_area);
    int32_t dest_h = lv_area_get_height(dest_area);

    int32_t dest_stride_a8 = dest_w;
    int32_t dest_stride;
    if(src_cf == LV_COLOR_FORMAT_RGB888) {
        dest_stride = dest_w * lv_color_format_get_size(LV_COLOR_FORMAT_ARGB8888);
    }
    else if(src_cf == LV_COLOR_FORMAT_RGB565A8) {
        dest_stride = dest_w * 2;
    }
    else if(src_cf == LV_COLOR_FORMAT_L8 || src_cf == LV_COLOR_FORMAT_AL88) {
        dest_stride = dest_w;
    }
    else {
        dest_stride = dest_w * lv_color_format_get_size(src_cf);
    }

    uint8_t * alpha_buf;
    if(src_cf == LV_COLOR_FORMAT_RGB565 || src_cf == LV_COLOR_FORMAT_RGB565_SWAPPED || src_cf == LV_COLOR_FORMAT_RGB565A8) {
        alpha_buf = dest_buf;
        alpha_buf += dest_stride * dest_h;
    }
    else if(src_cf == LV_COLOR_FORMAT_L8 || src_cf == LV_COLOR_FORMAT_AL88) {
        alpha_buf = dest_buf;
        alpha_buf += dest_w * dest_h;
    }
    else {
        alpha_buf = NULL;
    }

    bool aa = (bool) draw_dsc->antialias;
    bool is_rotated = draw_dsc->rotation;

    int32_t xs_ups = 0, ys_ups = 0;
    int32_t xs_step_256 = 0, ys_step_256 = 0;

    /*When some of the color formats are disabled, these variables could be unused, avoid warning here*/
    LV_UNUSED(aa);
    LV_UNUSED(xs_ups);
    LV_UNUSED(ys_ups);
    LV_UNUSED(xs_step_256);
    LV_UNUSED(ys_step_256);

    /*Compute the steps and the anchor point of the inverse transformation analytically.
     *The source coordinate of a destination pixel is
     *    xs_ups(x, y) = x0_ups + ((sxy * y) >> 8) + ((sxx * x) >> 8)
     *where x and y are the absolute (image local) destination coordinates.
     *As it doesn't depend on the rendered area, rendering only a part of the image
     *results in exactly the same pixels as rendering the whole image
     *(needed for deterministic partial rendering).*/
    int32_t sinma = tr_dsc.sinma;
    int32_t cosma = tr_dsc.cosma;
    if(tr_dsc.angle == 0) {
        /*Use the exact values to make the no-rotation case bit-exact (the table gives 1023 for cos(0))*/
        sinma = 0;
        cosma = 1024;
    }
    /*Multiply instead of shifting: `sinma` and `cosma` can be negative and
     *left shifting a negative value is undefined behavior*/
    int32_t sxx = (int32_t)(((int64_t)cosma * 16384) / tr_dsc.scale_x);
    int32_t sxy = (int32_t)((-(int64_t)sinma * 16384) / tr_dsc.scale_x);
    int32_t syx = (int32_t)(((int64_t)sinma * 16384) / tr_dsc.scale_y);
    int32_t syy = (int32_t)(((int64_t)cosma * 16384) / tr_dsc.scale_y);

    int64_t xin0 = -(int64_t)tr_dsc.pivot.x;
    int64_t yin0 = -(int64_t)tr_dsc.pivot.y;
    int32_t x0_ups = (int32_t)(((cosma * xin0 - sinma * yin0) * 64) / tr_dsc.scale_x) + tr_dsc.pivot_x_256 + 0x80;
    int32_t y0_ups = (int32_t)(((sinma * xin0 + cosma * yin0) * 64) / tr_dsc.scale_y) + tr_dsc.pivot_y_256 + 0x80;

    /*If scaled only, keep the source coordinates inside the image to avoid
     *fading out the last column/row due to the rounding of the steps.
     *For example with 300% zoom the last destination pixel would be on the 99.67
     *source coordinate (instead of 99) and as it's larger than 99.5 the next,
     *out of image pixel would be mixed in, making the edge more transparent.*/
    int32_t xs_clamp_ups = INT32_MAX;
    int32_t ys_clamp_ups = INT32_MAX;
    if(is_rotated == false) {
        xs_clamp_ups = (src_w - 1) * 256 + 0x80;
        ys_clamp_ups = (src_h - 1) * 256 + 0x80;
    }

    int32_t x_start = dest_area->x1;
    int32_t x_end_abs = dest_area->x1 + dest_w;
    int32_t y;
    for(y = 0; y < dest_h; y++) {
        int32_t y_abs = dest_area->y1 + y;
        xs_ups = x0_ups + (int32_t)(((int64_t)sxy * y_abs) >> 8);
        ys_ups = y0_ups + (int32_t)(((int64_t)syy * y_abs) >> 8);
        /*If scaled only, `ys` is constant in the row (syx == 0) so it can be clamped here*/
        if(ys_ups > ys_clamp_ups) ys_ups = ys_clamp_ups;
        xs_step_256 = sxx;
        ys_step_256 = syx;

        switch(src_cf) {
#if LV_DRAW_SW_SUPPORT_XRGB8888
            case LV_COLOR_FORMAT_XRGB8888:
                transform_rgb888(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256,
                                 x_start, x_end_abs, xs_clamp_ups, dest_buf, aa, 4);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB888
            case LV_COLOR_FORMAT_RGB888:
                transform_rgb888(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256,
                                 x_start, x_end_abs, xs_clamp_ups, dest_buf, aa, 3);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_A8
            case LV_COLOR_FORMAT_A8:
                transform_a8(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256,
                             x_start, x_end_abs, xs_clamp_ups, dest_buf, aa);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888
            case LV_COLOR_FORMAT_ARGB8888:
                transform_argb8888(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256,
                                   x_start, x_end_abs, xs_clamp_ups, dest_buf, aa);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
            case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
                transform_argb8888_premultiplied(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256,
                                                 x_start, x_end_abs, xs_clamp_ups, dest_buf, aa);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565 && LV_DRAW_SW_SUPPORT_RGB565A8
            case LV_COLOR_FORMAT_RGB565:
                transform_rgb565a8(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256,
                                   x_start, x_end_abs, xs_clamp_ups, dest_buf, alpha_buf, false, aa);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
            case LV_COLOR_FORMAT_RGB565_SWAPPED:
                transform_rgb565a8_swapped(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256,
                                           x_start, x_end_abs, xs_clamp_ups, dest_buf, alpha_buf, false, aa);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565A8
            case LV_COLOR_FORMAT_RGB565A8:
                transform_rgb565a8(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256,
                                   x_start, x_end_abs, xs_clamp_ups, (uint16_t *)dest_buf, alpha_buf, true, aa);
                break;
#endif

#if LV_DRAW_SW_SUPPORT_L8
            case LV_COLOR_FORMAT_L8:
                transform_al88(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256,
                               x_start, x_end_abs, xs_clamp_ups, dest_buf, alpha_buf, false, aa);
                break;
#endif

#if LV_DRAW_SW_SUPPORT_AL88
            case LV_COLOR_FORMAT_AL88:
                transform_al88(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256,
                               x_start, x_end_abs, xs_clamp_ups, dest_buf, alpha_buf, true, aa);
                break;
#endif

            default:
                LV_LOG_WARN("Color format 0x%02X is not enabled. "
                            "See lv_color.h to find the name of the color formats and "
                            "enable the related LV_DRAW_SW_SUPPORT_* in lv_conf.h.",
                            src_cf);
                return;
        }

        dest_buf = (uint8_t *)dest_buf + dest_stride;
        if(alpha_buf) alpha_buf += dest_stride_a8;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * `(32 << 8) / alpha_sum` for every possible alpha sum, so the alpha weighted color
 * interpolation can normalize without a division. Most MCU cores, Xtensa included, have no
 * integer divide instruction, so a 514 byte table is far cheaper than the division it replaces.
 */
static const uint16_t transform_norm_inv[257] = {
    0, 8192, 4096, 2730, 2048, 1638, 1365, 1170, 1024, 910, 819, 744, 682, 630, 585, 546, 512, 481, 455,
    431, 409, 390, 372, 356, 341, 327, 315, 303, 292, 282, 273, 264, 256, 248, 240, 234, 227, 221, 215,
    210, 204, 199, 195, 190, 186, 182, 178, 174, 170, 167, 163, 160, 157, 154, 151, 148, 146, 143, 141,
    138, 136, 134, 132, 130, 128, 126, 124, 122, 120, 118, 117, 115, 113, 112, 110, 109, 107, 106, 105,
    103, 102, 101, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 81, 80,
    79, 78, 78, 77, 76, 75, 75, 74, 73, 73, 72, 71, 71, 70, 70, 69, 68, 68, 67, 67, 66, 66, 65, 65, 64,
    64, 63, 63, 62, 62, 61, 61, 60, 60, 59, 59, 58, 58, 58, 57, 57, 56, 56, 56, 55, 55, 54, 54, 54, 53,
    53, 53, 52, 52, 52, 51, 51, 51, 50, 50, 50, 49, 49, 49, 49, 48, 48, 48, 47, 47, 47, 47, 46, 46, 46,
    46, 45, 45, 45, 45, 44, 44, 44, 44, 43, 43, 43, 43, 42, 42, 42, 42, 42, 41, 41, 41, 41, 40, 40, 40,
    40, 40, 39, 39, 39, 39, 39, 39, 38, 38, 38, 38, 38, 37, 37, 37, 37, 37, 37, 36, 36, 36, 36, 36, 36,
    35, 35, 35, 35, 35, 35, 35, 34, 34, 34, 34, 34, 34, 33, 33, 33, 33, 33, 33, 33, 33, 32, 32, 32, 32,
    32, 32, 32, 32
};

/**
 * Fold the bilinear weights of a 2x2 neighborhood together with its alphas.
 * Used where color and alpha live in separate planes (RGB565A8, AL88): the color has to be
 * weighted with alpha so a transparent neighbor cannot bleed its color in, then normalized
 * back, because the consumer expects a straight and not a premultiplied color.
 * @param a00..a11  the four alphas, 0..255
 * @param fx        weight of the right column, 0..255
 * @param fy        weight of the bottom row, 0..255
 * @param t         out: the four normalized 5 bit color weights, adding up to at most 32
 * @return          the interpolated alpha, 0..255
 */
static inline uint32_t LV_ATTRIBUTE_FAST_MEM transform_alpha_weights(uint32_t a00, uint32_t a01,
                                                                     uint32_t a10, uint32_t a11,
                                                                     uint32_t fx, uint32_t fy, uint32_t * t)
{
    uint32_t ifx = 256 - fx;
    uint32_t ify = 256 - fy;

    /*Scale each alpha to 0..256 so an opaque neighbor keeps its full weight*/
    uint32_t u00 = (((ifx * ify) >> 8) * (a00 + (a00 >> 7))) >> 8;
    uint32_t u01 = (((fx * ify) >> 8) * (a01 + (a01 >> 7))) >> 8;
    uint32_t u10 = (((ifx * fy) >> 8) * (a10 + (a10 >> 7))) >> 8;
    uint32_t u11 = (((fx * fy) >> 8) * (a11 + (a11 >> 7))) >> 8;

    uint32_t sum = u00 + u01 + u10 + u11;
    if(sum == 0) {
        t[0] = t[1] = t[2] = t[3] = 0;
        return 0;
    }

    /*Normalize on the running total instead of each weight on its own. Scaling the four
     *weights separately rounds each one down, which left them adding up to 30 of 32 in most
     *cases and darkened the pixel even where all four colors were the same. Differences of
     *the scaled partial sums spread that rounding out instead.*/
    uint32_t inv = transform_norm_inv[sum];
    uint32_t c1 = (u00 * inv) >> 8;
    uint32_t c2 = ((u00 + u01) * inv) >> 8;
    uint32_t c3 = ((u00 + u01 + u10) * inv) >> 8;
    uint32_t c4 = (sum * inv) >> 8;
    t[0] = c1;
    t[1] = c2 - c1;
    t[2] = c3 - c2;
    t[3] = c4 - c3;

    /*Whatever the rounding left over goes to the neighbor that already weighs the most.
     *It must not go to a fixed one: if that neighbor is transparent it would bleed its
     *color in, which is exactly what weighting by alpha is meant to prevent.*/
    uint32_t rem = 32 - c4;
    if(rem) {
        uint32_t m = 0;
        uint32_t best = u00;
        if(u01 > best) {
            best = u01;
            m = 1;
        }
        if(u10 > best) {
            best = u10;
            m = 2;
        }
        if(u11 > best) {
            m = 3;
        }
        t[m] += rem;
    }

    return sum > 255 ? 255 : sum;
}

/**
 * Premultiply an ARGB8888 pixel. The red/blue lanes and the green lane are scaled with one
 * multiplication each. Alpha is scaled to 0..256 first so an opaque pixel comes back unchanged.
 */
static inline uint32_t LV_ATTRIBUTE_FAST_MEM argb8888_premul(uint32_t p)
{
    uint32_t a = p >> 24;
    uint32_t a256 = a + (a >> 7);
    uint32_t rb = (((p & 0x00FF00FF) * a256) >> 8) & 0x00FF00FF;
    uint32_t g = (((p & 0x0000FF00) * a256) >> 8) & 0x0000FF00;
    return rb | g | (a << 24);
}

/**
 * Bilinear filtering of a 2x2 ARGB8888 neighborhood, computed in premultiplied alpha space.
 * That is the only space where bilinear filtering of an image with an alpha channel is correct:
 * a transparent neighbor carries no color weight, so it cannot bleed into the result and no
 * special case is needed for it. The kernel is therefore fully branchless.
 *
 * Each neighbor's bilinear weight is folded into its alpha up front, which spares a separate
 * interpolation of the alpha channel and keeps the packed sums in range: the four weights add
 * up to 256 and every alpha is at most 255, so no 16 bit lane can overflow.
 * @return          the filtered pixel, premultiplied
 */
static inline uint32_t LV_ATTRIBUTE_FAST_MEM argb8888_bilinear_premul(uint32_t p00, uint32_t p01,
                                                                      uint32_t p10, uint32_t p11,
                                                                      uint32_t fx, uint32_t fy)
{
    uint32_t ifx = 256 - fx;
    uint32_t ify = 256 - fy;

    uint32_t a00 = p00 >> 24, a01 = p01 >> 24, a10 = p10 >> 24, a11 = p11 >> 24;
    uint32_t u00 = (((ifx * ify) >> 8) * (a00 + (a00 >> 7))) >> 8;
    uint32_t u01 = (((fx * ify) >> 8) * (a01 + (a01 >> 7))) >> 8;
    uint32_t u10 = (((ifx * fy) >> 8) * (a10 + (a10 >> 7))) >> 8;
    uint32_t u11 = (((fx * fy) >> 8) * (a11 + (a11 >> 7))) >> 8;

    uint32_t rb = (p00 & 0x00FF00FF) * u00 + (p01 & 0x00FF00FF) * u01
                  + (p10 & 0x00FF00FF) * u10 + (p11 & 0x00FF00FF) * u11;
    uint32_t g = (p00 & 0x0000FF00) * u00 + (p01 & 0x0000FF00) * u01
                 + (p10 & 0x0000FF00) * u10 + (p11 & 0x0000FF00) * u11;

    /*In premultiplied space the resulting alpha is just the sum of the folded weights*/
    uint32_t a = u00 + u01 + u10 + u11;
    if(a > 255) a = 255;

    return ((rb >> 8) & 0x00FF00FF) | ((g >> 8) & 0x0000FF00) | (a << 24);
}


#if LV_DRAW_SW_SUPPORT_RGB888 || LV_DRAW_SW_SUPPORT_XRGB8888

static void rgb888_row_checked(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                               int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                               int32_t x_from, int32_t x_to, int32_t x_offs, int32_t xs_clamp_ups,
                               lv_color32_t * dest_c32, bool aa, uint32_t px_size)
{
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t x_abs = x + x_offs;
        int32_t xs_ups = xs_base + ((xs_step * x_abs) >> 8);
        int32_t ys_ups = ys_base + ((ys_step * x_abs) >> 8);
        if(xs_ups > xs_clamp_ups) xs_ups = xs_clamp_ups;

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        /*Fully out of the image*/
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            dest_c32[x].alpha = 0x00;
            continue;
        }

        /*Get the direction the hor and ver neighbor
         *`fract` will be in range of 0x00..0xFF and `next` (+/-1) indicates the direction*/
        int32_t xs_fract = xs_ups & 0xFF;
        int32_t ys_fract = ys_ups & 0xFF;

        int32_t x_next;
        int32_t y_next;
        if(xs_fract < 0x80) {
            x_next = -1;
            xs_fract = 0x7F - xs_fract;
        }
        else {
            x_next = 1;
            xs_fract = xs_fract - 0x80;
        }
        if(ys_fract < 0x80) {
            y_next = -1;
            ys_fract = 0x7F - ys_fract;
        }
        else {
            y_next = 1;
            ys_fract = ys_fract - 0x80;
        }

        const uint8_t * src_u8 = &src[ys_int * src_stride + xs_int * px_size];

        dest_c32[x].red = src_u8[2];
        dest_c32[x].green = src_u8[1];
        dest_c32[x].blue = src_u8[0];
        dest_c32[x].alpha = 0xff;

        if(aa &&
           xs_int + x_next >= 0 &&
           xs_int + x_next <= src_w - 1 &&
           ys_int + y_next >= 0 &&
           ys_int + y_next <= src_h - 1) {
            const uint8_t * px_hor_u8 = src_u8 + (int32_t)(x_next * px_size);
            lv_color32_t px_hor;
            px_hor.red = px_hor_u8[2];
            px_hor.green = px_hor_u8[1];
            px_hor.blue = px_hor_u8[0];
            px_hor.alpha = 0xff;

            const uint8_t * px_ver_u8 = src_u8 + (int32_t)(y_next * src_stride);
            lv_color32_t px_ver;
            px_ver.red = px_ver_u8[2];
            px_ver.green = px_ver_u8[1];
            px_ver.blue = px_ver_u8[0];
            px_ver.alpha = 0xff;

            if(!lv_color32_eq(dest_c32[x], px_ver)) {
                px_ver.alpha = ys_fract;
                dest_c32[x] = lv_color_mix32_inlined(px_ver, dest_c32[x]);
            }

            if(!lv_color32_eq(dest_c32[x], px_hor)) {
                px_hor.alpha = xs_fract;
                dest_c32[x] = lv_color_mix32_inlined(px_hor, dest_c32[x]);
            }
        }
        /*Partially out of the image*/
        else {
            lv_opa_t a = 0xff;

            if((xs_int == 0 && x_next < 0) || (xs_int == src_w - 1 && x_next > 0))  {
                dest_c32[x].alpha = (a * (0xFF - xs_fract)) >> 8;
            }
            else if((ys_int == 0 && y_next < 0) || (ys_int == src_h - 1 && y_next > 0))  {
                dest_c32[x].alpha = (a * (0xFF - ys_fract)) >> 8;
            }
        }
    }
}

static void rgb888_row_bilinear(const uint8_t * src, int32_t src_stride,
                                int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                                int32_t x_from, int32_t x_to, int32_t x_offs,
                                lv_color32_t * dest_c32, uint32_t px_size)
{
    lv_draw_sw_word_t * dest_w = (lv_draw_sw_word_t *)dest_c32;
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t x_abs = x + x_offs;
        int32_t xs_ups = xs_base + ((xs_step * x_abs) >> 8) - 0x80;
        int32_t ys_ups = ys_base + ((ys_step * x_abs) >> 8) - 0x80;
        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        const uint8_t * p = src + ys_int * src_stride + xs_int * px_size;
        uint32_t p00, p01, p10, p11;
        if(px_size == 4) {
            const lv_draw_sw_word_t * w = (const lv_draw_sw_word_t *)p;
            const lv_draw_sw_word_t * w2 = (const lv_draw_sw_word_t *)(p + src_stride);
            p00 = w[0].u32;
            p01 = w[1].u32;
            p10 = w2[0].u32;
            p11 = w2[1].u32;
        }
        else {
            const uint8_t * p2 = p + src_stride;
            p00 = p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16);
            p01 = p[3] | ((uint32_t)p[4] << 8) | ((uint32_t)p[5] << 16);
            p10 = p2[0] | ((uint32_t)p2[1] << 8) | ((uint32_t)p2[2] << 16);
            p11 = p2[3] | ((uint32_t)p2[4] << 8) | ((uint32_t)p2[5] << 16);
        }

        /*True bilinear interpolation. The red and blue channels are interpolated together
         *in one 32 bit value with a single multiplication each.
         *The 16 bit lanes can't overflow as their max value is 255 * 256 < 65536*/
        uint32_t fx = xs_ups & 0xFF;
        uint32_t fx_inv = 256 - fx;
        uint32_t t_rb = (((p00 & 0xFF00FF) * fx_inv + (p01 & 0xFF00FF) * fx) >> 8) & 0xFF00FF;
        uint32_t t_g = (((p00 & 0xFF00) * fx_inv + (p01 & 0xFF00) * fx) >> 8) & 0xFF00;
        uint32_t b_rb = (((p10 & 0xFF00FF) * fx_inv + (p11 & 0xFF00FF) * fx) >> 8) & 0xFF00FF;
        uint32_t b_g = (((p10 & 0xFF00) * fx_inv + (p11 & 0xFF00) * fx) >> 8) & 0xFF00;
        uint32_t fy = ys_ups & 0xFF;
        uint32_t fy_inv = 256 - fy;
        uint32_t rb = ((t_rb * fy_inv + b_rb * fy) >> 8) & 0xFF00FF;
        uint32_t g = ((t_g * fy_inv + b_g * fy) >> 8) & 0xFF00;
        dest_w[x].u32 = rb | g | 0xFF000000;
    }
}

static void transform_rgb888(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                             int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                             int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                             uint8_t * dest_buf, bool aa, uint32_t px_size)
{
    lv_color32_t * dest_c32 = (lv_color32_t *) dest_buf;

    /*The destination is indexed from zero while the source coordinates are calculated from the
     *absolute (image local) x coordinate to keep the partial rendering deterministic*/
    int32_t w = x_end - x_start;

    /*In the middle of the row the pixel and all its neighbors are inside the source image,
     *so neither bounds checking nor edge handling is needed there*/
    int32_t fast_from, fast_to;
    transform_safe_range(xs_ups, ys_ups, xs_step, ys_step, src_w, src_h, x_start, x_end, aa, &fast_from, &fast_to);
    fast_from -= x_start;
    fast_to -= x_start;

    rgb888_row_checked(src, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step, ys_step,
                       0, fast_from, x_start, xs_clamp_ups, dest_c32, aa, px_size);

    if(aa) {
        rgb888_row_bilinear(src, src_stride, xs_ups, ys_ups, xs_step, ys_step, fast_from, fast_to, x_start,
                            dest_c32, px_size);
    }
    else {
        int32_t x;
        for(x = fast_from; x < fast_to; x++) {
            int32_t x_abs = x + x_start;
            int32_t xs_int = (xs_ups + ((xs_step * x_abs) >> 8)) >> 8;
            int32_t ys_int = (ys_ups + ((ys_step * x_abs) >> 8)) >> 8;
            const uint8_t * src_u8 = &src[ys_int * src_stride + xs_int * px_size];
            dest_c32[x].red = src_u8[2];
            dest_c32[x].green = src_u8[1];
            dest_c32[x].blue = src_u8[0];
            dest_c32[x].alpha = 0xff;
        }
    }

    rgb888_row_checked(src, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step, ys_step,
                       fast_to, w, x_start, xs_clamp_ups, dest_c32, aa, px_size);
}

#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888


static void argb8888_row_checked(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                                 int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                                 int32_t x_from, int32_t x_to, int32_t x_offs, int32_t xs_clamp_ups,
                                 lv_color32_t * dest_c32, bool aa)
{
    lv_draw_sw_word_t * dest_w = (lv_draw_sw_word_t *)dest_c32;
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t x_abs = x + x_offs;
        int32_t xs_ups = xs_base + ((xs_step * x_abs) >> 8);
        int32_t ys_ups = ys_base + ((ys_step * x_abs) >> 8);
        if(xs_ups > xs_clamp_ups) xs_ups = xs_clamp_ups;

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        /*Fully out of the image*/
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            dest_w[x].u32 = 0x00000000;
            continue;
        }

        if(!aa) {
            dest_w[x].u32 = ((const lv_draw_sw_word_t *)(src + ys_int * src_stride + xs_int * 4))->u32;
            continue;
        }

        /*Sample the 2x2 neighborhood. Taps outside the image count as fully transparent,
         *so the edge fades out on its own and needs no dedicated handling.*/
        int32_t xs_ofs = xs_ups - 0x80;
        int32_t ys_ofs = ys_ups - 0x80;
        int32_t x0 = xs_ofs >> 8;
        int32_t y0 = ys_ofs >> 8;
        uint32_t p[4] = {0, 0, 0, 0};
        int32_t i;
        for(i = 0; i < 4; i++) {
            int32_t sx = x0 + (i & 1);
            int32_t sy = y0 + (i >> 1);
            if(sx >= 0 && sx < src_w && sy >= 0 && sy < src_h) {
                p[i] = ((const lv_draw_sw_word_t *)(src + sy * src_stride + sx * 4))->u32;
            }
        }

        dest_w[x].u32 = argb8888_bilinear_premul(p[0], p[1], p[2], p[3],
                                                 xs_ofs & 0xFF, ys_ofs & 0xFF);
    }
}

static void argb8888_row_fast(const uint8_t * src, int32_t src_stride,
                              int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                              int32_t x_from, int32_t x_to, int32_t x_offs, lv_color32_t * dest_c32)
{
    lv_draw_sw_word_t * dest_w = (lv_draw_sw_word_t *)dest_c32;
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t x_abs = x + x_offs;
        int32_t xs_ups = xs_base + ((xs_step * x_abs) >> 8);
        int32_t ys_ups = ys_base + ((ys_step * x_abs) >> 8);
        int32_t xs_ups_ofs = xs_ups - 0x80;
        int32_t ys_ups_ofs = ys_ups - 0x80;
        int32_t xs_int = xs_ups_ofs >> 8;
        int32_t ys_int = ys_ups_ofs >> 8;

        const uint8_t * pb = src + ys_int * src_stride + xs_int * 4;
        const lv_draw_sw_word_t * p = (const lv_draw_sw_word_t *)pb;
        const lv_draw_sw_word_t * p2 = (const lv_draw_sw_word_t *)(pb + src_stride);
        uint32_t p00 = p[0].u32;
        uint32_t p01 = p[1].u32;
        uint32_t p10 = p2[0].u32;
        uint32_t p11 = p2[1].u32;

        if(p00 == p01 && p00 == p10 && p00 == p11) {
            /*Flat neighborhood: the filter is the identity. Real UI images are mostly flat,
             *so this fires often and predicts well.*/
            dest_w[x].u32 = argb8888_premul(p00);
        }
        else if(((p00 & p01 & p10 & p11) >> 24) == 0xFF) {
            /*All the 4 neighbors are opaque: use true bilinear interpolation.
             *The red and blue channels are interpolated together in one 32 bit value
             *with a single multiplication each. The 16 bit lanes can't overflow
             *as their max value is 255 * 256 < 65536*/
            uint32_t fx = xs_ups_ofs & 0xFF;
            uint32_t fx_inv = 256 - fx;
            uint32_t t_rb = (((p00 & 0xFF00FF) * fx_inv + (p01 & 0xFF00FF) * fx) >> 8) & 0xFF00FF;
            uint32_t t_g = (((p00 & 0xFF00) * fx_inv + (p01 & 0xFF00) * fx) >> 8) & 0xFF00;
            uint32_t b_rb = (((p10 & 0xFF00FF) * fx_inv + (p11 & 0xFF00FF) * fx) >> 8) & 0xFF00FF;
            uint32_t b_g = (((p10 & 0xFF00) * fx_inv + (p11 & 0xFF00) * fx) >> 8) & 0xFF00;
            uint32_t fy = ys_ups_ofs & 0xFF;
            uint32_t fy_inv = 256 - fy;
            uint32_t rb = ((t_rb * fy_inv + b_rb * fy) >> 8) & 0xFF00FF;
            uint32_t g = ((t_g * fy_inv + b_g * fy) >> 8) & 0xFF00;
            dest_w[x].u32 = rb | g | 0xFF000000;
        }
        else {
            /*Non opaque pixels are involved: filter in premultiplied space, where a
             *transparent neighbor simply carries no weight*/
            dest_w[x].u32 = argb8888_bilinear_premul(p00, p01, p10, p11,
                                                     xs_ups_ofs & 0xFF, ys_ups_ofs & 0xFF);
        }
    }
}

static void transform_argb8888(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                               int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                               int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                               uint8_t * dest_buf, bool aa)
{
    lv_color32_t * dest_c32 = (lv_color32_t *) dest_buf;
    lv_draw_sw_word_t * dest_w = (lv_draw_sw_word_t *)dest_buf;

    /*The destination is indexed from zero while the source coordinates are calculated from the
     *absolute (image local) x coordinate to keep the partial rendering deterministic*/
    int32_t w = x_end - x_start;

    /*In the middle of the row the pixel and all its neighbors are inside the source image,
     *so neither bounds checking nor edge handling is needed there*/
    int32_t fast_from, fast_to;
    transform_safe_range(xs_ups, ys_ups, xs_step, ys_step, src_w, src_h, x_start, x_end, aa, &fast_from, &fast_to);
    fast_from -= x_start;
    fast_to -= x_start;

    argb8888_row_checked(src, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step, ys_step,
                         0, fast_from, x_start, xs_clamp_ups, dest_c32, aa);

    if(aa) {
        argb8888_row_fast(src, src_stride, xs_ups, ys_ups, xs_step, ys_step, fast_from, fast_to, x_start, dest_c32);
    }
    else {
        int32_t x;
        for(x = fast_from; x < fast_to; x++) {
            int32_t x_abs = x + x_start;
            int32_t xs_int = (xs_ups + ((xs_step * x_abs) >> 8)) >> 8;
            int32_t ys_int = (ys_ups + ((ys_step * x_abs) >> 8)) >> 8;
            dest_w[x].u32 = ((const lv_draw_sw_word_t *)(src + ys_int * src_stride + xs_int * 4))->u32;
        }
    }

    argb8888_row_checked(src, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step, ys_step,
                         fast_to, w, x_start, xs_clamp_ups, dest_c32, aa);

#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED == 0
    /*The antialiasing filter works in premultiplied space. Without the premultiplied blenders
     *the result is consumed as straight ARGB8888, so convert it back.*/
    if(aa) {
        int32_t i;
        for(i = 0; i < w; i++) dest_c32[i] = unpremultiply(dest_c32[i]);
    }
#endif
}


#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED

static void transform_argb8888_premultiplied(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                                             int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                                             int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                                             uint8_t * dest_buf, bool aa)
{
    lv_draw_sw_word_t * dest_w = (lv_draw_sw_word_t *)dest_buf;
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;
    lv_color32_t * dest_c32 = (lv_color32_t *) dest_buf;
    int32_t w = x_end - x_start;

    int32_t x;
    for(x = 0; x < w; x++) {
        int32_t x_abs = x + x_start;
        xs_ups = xs_ups_start + ((xs_step * x_abs) >> 8);
        ys_ups = ys_ups_start + ((ys_step * x_abs) >> 8);
        if(xs_ups > xs_clamp_ups) xs_ups = xs_clamp_ups;

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        /*Fully out of the image*/
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            dest_w[x].u32 = 0x00000000;
            continue;
        }

        /*Get the direction the hor and ver neighbor
         *`fract` will be in range of 0x00..0xFF and `next` (+/-1) indicates the direction*/
        int32_t xs_fract = xs_ups & 0xFF;
        int32_t ys_fract = ys_ups & 0xFF;

        int32_t x_next;
        int32_t y_next;
        if(xs_fract < 0x80) {
            x_next = -1;
            xs_fract = 0x7F - xs_fract;
        }
        else {
            x_next = 1;
            xs_fract = xs_fract - 0x80;
        }
        if(ys_fract < 0x80) {
            y_next = -1;
            ys_fract = 0x7F - ys_fract;
        }
        else {
            y_next = 1;
            ys_fract = ys_fract - 0x80;
        }

        const lv_color32_t * src_c32 = (const lv_color32_t *)(src + ys_int * src_stride + xs_int * 4);

        dest_c32[x] = src_c32[0];

        if(aa &&
           xs_int + x_next >= 0 &&
           xs_int + x_next <= src_w - 1 &&
           ys_int + y_next >= 0 &&
           ys_int + y_next <= src_h - 1) {

            lv_color32_t px_hor = src_c32[x_next];
            lv_color32_t px_ver = *(const lv_color32_t *)((uint8_t *)src_c32 + y_next * src_stride);

            /*Have the non-premultiplied colors first, mix them as needed,
             *and premultiply again*/
            dest_c32[x] = unpremultiply(dest_c32[x]);
            px_hor = unpremultiply(px_hor);
            px_ver = unpremultiply(px_ver);

            if(px_ver.alpha == 0) {
                dest_c32[x].alpha = (dest_c32[x].alpha * (0xFF - ys_fract)) >> 8;

            }
            else if(!lv_color32_eq(dest_c32[x], px_ver)) {
                if(dest_c32[x].alpha) dest_c32[x].alpha = ((px_ver.alpha * ys_fract) + (dest_c32[x].alpha * (0xFF - ys_fract))) >> 8;
                px_ver.alpha = ys_fract;
                dest_c32[x] = lv_color_mix32_inlined(px_ver, dest_c32[x]);
            }

            if(px_hor.alpha == 0) {
                dest_c32[x].alpha = (dest_c32[x].alpha * (0xFF - xs_fract)) >> 8;
            }
            else if(!lv_color32_eq(dest_c32[x], px_hor)) {
                if(dest_c32[x].alpha) dest_c32[x].alpha = ((px_hor.alpha * xs_fract) + (dest_c32[x].alpha * (0xFF - xs_fract))) >> 8;
                px_hor.alpha = xs_fract;
                dest_c32[x] = lv_color_mix32_inlined(px_hor, dest_c32[x]);
            }

            dest_c32[x].red = (dest_c32[x].red * dest_c32[x].alpha) >> 8;
            dest_c32[x].green = (dest_c32[x].green * dest_c32[x].alpha) >> 8;
            dest_c32[x].blue = (dest_c32[x].blue * dest_c32[x].alpha) >> 8;

        }
        /*Partially out of the image*/
        else {
            if((xs_int == 0 && x_next < 0) || (xs_int == src_w - 1 && x_next > 0))  {
                dest_c32[x] = unpremultiply(dest_c32[x]);
                lv_opa_t alpha = (dest_c32[x].alpha * (0x7F - xs_fract)) >> 7;
                dest_c32[x].alpha = alpha;
                dest_c32[x].red = (dest_c32[x].red * dest_c32[x].alpha) >> 8;
                dest_c32[x].green = (dest_c32[x].green * dest_c32[x].alpha) >> 8;
                dest_c32[x].blue = (dest_c32[x].blue * dest_c32[x].alpha) >> 8;

            }
            else if((ys_int == 0 && y_next < 0) || (ys_int == src_h - 1 && y_next > 0))  {
                dest_c32[x] = unpremultiply(dest_c32[x]);
                lv_opa_t alpha = (dest_c32[x].alpha * (0x7F - ys_fract)) >> 7;
                dest_c32[x].alpha = alpha;
                dest_c32[x].red = (dest_c32[x].red * dest_c32[x].alpha) >> 8;
                dest_c32[x].green = (dest_c32[x].green * dest_c32[x].alpha) >> 8;
                dest_c32[x].blue = (dest_c32[x].blue * dest_c32[x].alpha) >> 8;
            }
        }
    }
}
#endif

#if LV_DRAW_SW_SUPPORT_RGB565A8


static void rgb565a8_row_fast(const uint8_t * src, int32_t src_stride,
                              const lv_opa_t * src_alpha, int32_t alpha_stride,
                              int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                              int32_t x_from, int32_t x_to, int32_t x_offs,
                              uint16_t * cbuf, uint8_t * abuf, bool src_has_a8)
{
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t x_abs = x + x_offs;
        int32_t xs_ups = xs_base + ((xs_step * x_abs) >> 8);
        int32_t ys_ups = ys_base + ((ys_step * x_abs) >> 8);
        int32_t xs_ups_ofs = xs_ups - 0x80;
        int32_t ys_ups_ofs = ys_ups - 0x80;
        int32_t xs_int = xs_ups_ofs >> 8;
        int32_t ys_int = ys_ups_ofs >> 8;
        uint32_t fx = xs_ups_ofs & 0xFF;
        uint32_t fy = ys_ups_ofs & 0xFF;

        uint32_t alpha_out = 0xFF;
        if(src_has_a8) {
            const lv_opa_t * pa = src_alpha + ys_int * alpha_stride + xs_int;
            uint32_t a00 = pa[0];
            uint32_t a01 = pa[1];
            uint32_t a10 = pa[alpha_stride];
            uint32_t a11 = pa[alpha_stride + 1];
            if(a00 == a01 && a00 == a10 && a00 == a11) {
                /*Uniform alpha: the weighting cancels out, so the plain interpolation below is
                 *already exact. Covers opaque interiors and fully transparent areas alike.*/
                alpha_out = a00;
                if(a00 == 0) {
                    abuf[x] = 0;
                    cbuf[x] = 0;
                    continue;
                }
            }
            else {
                /*Mixed alpha: weight the colors with their alpha so a transparent neighbor
                 *cannot bleed its color in, then normalize back to a straight color*/
                uint32_t t[4];
                uint32_t a = transform_alpha_weights(a00, a01, a10, a11, fx, fy, t);
                abuf[x] = (uint8_t)a;
                if(a == 0) {
                    cbuf[x] = 0;
                    continue;
                }
                const uint8_t * pcb = src + ys_int * src_stride + xs_int * 2;
                const lv_draw_sw_halfword_t * pc = (const lv_draw_sw_halfword_t *)pcb;
                const lv_draw_sw_halfword_t * pc2 = (const lv_draw_sw_halfword_t *)(pcb + src_stride);
                uint16_t s00 = pc[0].u16;
                uint16_t s01 = pc[1].u16;
                uint16_t s10 = pc2[0].u16;
                uint16_t s11 = pc2[1].u16;
                uint32_t w00 = (s00 | ((uint32_t)s00 << 16)) & 0x7E0F81F;
                uint32_t w01 = (s01 | ((uint32_t)s01 << 16)) & 0x7E0F81F;
                uint32_t w10 = (s10 | ((uint32_t)s10 << 16)) & 0x7E0F81F;
                uint32_t w11 = (s11 | ((uint32_t)s11 << 16)) & 0x7E0F81F;
                uint32_t acc = ((w00 * t[0] + w01 * t[1] + w10 * t[2] + w11 * t[3]) >> 5) & 0x7E0F81F;
                cbuf[x] = (uint16_t)(acc | (acc >> 16));
                continue;
            }
        }
        abuf[x] = (uint8_t)alpha_out;

        /*All the 4 neighbors are opaque: use true bilinear interpolation.
         *The RGB565 colors are expanded to 32 bits (0x07E0F81F mask) so all channels
         *can be interpolated with a single multiplication. 5 bit weights are used
         *so the 11 bit lanes can't overflow (63 * 32 < 2048).*/
        const uint8_t * pb = src + ys_int * src_stride + xs_int * 2;
        const lv_draw_sw_halfword_t * p = (const lv_draw_sw_halfword_t *)pb;
        const lv_draw_sw_halfword_t * p2 = (const lv_draw_sw_halfword_t *)(pb + src_stride);
        uint16_t q00 = p[0].u16;
        uint16_t q01 = p[1].u16;
        uint16_t q10 = p2[0].u16;
        uint16_t q11 = p2[1].u16;
        uint32_t e00 = (q00 | ((uint32_t)q00 << 16)) & 0x7E0F81F;
        uint32_t e01 = (q01 | ((uint32_t)q01 << 16)) & 0x7E0F81F;
        uint32_t e10 = (q10 | ((uint32_t)q10 << 16)) & 0x7E0F81F;
        uint32_t e11 = (q11 | ((uint32_t)q11 << 16)) & 0x7E0F81F;
        uint32_t fx5 = (fx + 4) >> 3;
        uint32_t fy5 = (fy + 4) >> 3;
        uint32_t top = ((e00 * (32 - fx5) + e01 * fx5) >> 5) & 0x7E0F81F;
        uint32_t bot = ((e10 * (32 - fx5) + e11 * fx5) >> 5) & 0x7E0F81F;
        uint32_t res = ((top * (32 - fy5) + bot * fy5) >> 5) & 0x7E0F81F;
        cbuf[x] = (uint16_t)(res | (res >> 16));
    }
}

static void rgb565a8_row_checked(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                                 const lv_opa_t * src_alpha, int32_t alpha_stride,
                                 int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                                 int32_t x_from, int32_t x_to, int32_t x_offs, int32_t xs_clamp_ups,
                                 uint16_t * cbuf, uint8_t * abuf, bool src_has_a8, bool aa)
{
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t x_abs = x + x_offs;
        int32_t xs_ups = xs_base + ((xs_step * x_abs) >> 8);
        int32_t ys_ups = ys_base + ((ys_step * x_abs) >> 8);
        if(xs_ups > xs_clamp_ups) xs_ups = xs_clamp_ups;

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        /*Fully out of the image*/
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            cbuf[x] = 0;
            abuf[x] = 0x00;
            continue;
        }

        if(!aa) {
            uint16_t c = ((const lv_draw_sw_halfword_t *)(src + ys_int * src_stride + xs_int * 2))->u16;
            cbuf[x] = c;
            abuf[x] = src_has_a8 ? src_alpha[ys_int * alpha_stride + xs_int] : 0xFF;
            continue;
        }

        /*Sample the 2x2 neighborhood. Taps outside the image count as fully transparent,
         *so the edge fades out on its own and needs no dedicated handling.*/
        int32_t xs_ofs = xs_ups - 0x80;
        int32_t ys_ofs = ys_ups - 0x80;
        int32_t x0 = xs_ofs >> 8;
        int32_t y0 = ys_ofs >> 8;

        uint32_t e[4] = {0, 0, 0, 0};
        uint32_t a[4] = {0, 0, 0, 0};
        int32_t i;
        for(i = 0; i < 4; i++) {
            int32_t sx = x0 + (i & 1);
            int32_t sy = y0 + (i >> 1);
            if(sx >= 0 && sx < src_w && sy >= 0 && sy < src_h) {
                uint16_t c = ((const lv_draw_sw_halfword_t *)(src + sy * src_stride + sx * 2))->u16;
                uint16_t cc = c;
                e[i] = (cc | ((uint32_t)cc << 16)) & 0x7E0F81F;
                a[i] = src_has_a8 ? src_alpha[sy * alpha_stride + sx] : 0xFF;
            }
        }

        uint32_t t[4];
        uint32_t av = transform_alpha_weights(a[0], a[1], a[2], a[3], xs_ofs & 0xFF, ys_ofs & 0xFF, t);
        abuf[x] = (uint8_t)av;
        if(av == 0) {
            cbuf[x] = 0;
            continue;
        }

        uint32_t acc = ((e[0] * t[0] + e[1] * t[1] + e[2] * t[2] + e[3] * t[3]) >> 5) & 0x7E0F81F;
        cbuf[x] = (uint16_t)(acc | (acc >> 16));
    }
}

static void transform_rgb565a8(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                               int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                               int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                               uint16_t * cbuf, uint8_t * abuf, bool src_has_a8, bool aa)
{
    const lv_opa_t * src_alpha = src + src_stride * src_h;

    /*Must be signed type, because we would use negative array index calculated from stride*/
    int32_t alpha_stride = src_stride / 2; /*alpha map stride is always half of RGB map stride*/

    /*The destination is indexed from zero while the source coordinates are calculated from the
     *absolute (image local) x coordinate to keep the partial rendering deterministic*/
    int32_t w = x_end - x_start;

    /*In the middle of the row the pixel and all its neighbors are inside the source image,
     *so neither bounds checking nor edge handling is needed there*/
    int32_t fast_from, fast_to;
    transform_safe_range(xs_ups, ys_ups, xs_step, ys_step, src_w, src_h, x_start, x_end, aa, &fast_from, &fast_to);
    fast_from -= x_start;
    fast_to -= x_start;

    rgb565a8_row_checked(src, src_w, src_h, src_stride, src_alpha, alpha_stride, xs_ups, ys_ups, xs_step, ys_step,
                         0, fast_from, x_start, xs_clamp_ups, cbuf, abuf, src_has_a8, aa);

    if(aa) {
        rgb565a8_row_fast(src, src_stride, src_alpha, alpha_stride, xs_ups, ys_ups, xs_step, ys_step,
                          fast_from, fast_to, x_start, cbuf, abuf, src_has_a8);
    }
    else {
        int32_t x;
        for(x = fast_from; x < fast_to; x++) {
            int32_t x_abs = x + x_start;
            int32_t xs_int = (xs_ups + ((xs_step * x_abs) >> 8)) >> 8;
            int32_t ys_int = (ys_ups + ((ys_step * x_abs) >> 8)) >> 8;
            cbuf[x] = ((const lv_draw_sw_halfword_t *)(src + ys_int * src_stride + xs_int * 2))->u16;
            abuf[x] = src_has_a8 ? src_alpha[ys_int * alpha_stride + xs_int] : 0xFF;
        }
    }

    rgb565a8_row_checked(src, src_w, src_h, src_stride, src_alpha, alpha_stride, xs_ups, ys_ups, xs_step, ys_step,
                         fast_to, w, x_start, xs_clamp_ups, cbuf, abuf, src_has_a8, aa);
}


#endif

#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED


static void rgb565a8_swapped_row_fast(const uint8_t * src, int32_t src_stride,
                                      const lv_opa_t * src_alpha, int32_t alpha_stride,
                                      int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                                      int32_t x_from, int32_t x_to, int32_t x_offs,
                                      uint16_t * cbuf, uint8_t * abuf, bool src_has_a8)
{
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t x_abs = x + x_offs;
        int32_t xs_ups = xs_base + ((xs_step * x_abs) >> 8);
        int32_t ys_ups = ys_base + ((ys_step * x_abs) >> 8);
        int32_t xs_ups_ofs = xs_ups - 0x80;
        int32_t ys_ups_ofs = ys_ups - 0x80;
        int32_t xs_int = xs_ups_ofs >> 8;
        int32_t ys_int = ys_ups_ofs >> 8;
        uint32_t fx = xs_ups_ofs & 0xFF;
        uint32_t fy = ys_ups_ofs & 0xFF;

        uint32_t alpha_out = 0xFF;
        if(src_has_a8) {
            const lv_opa_t * pa = src_alpha + ys_int * alpha_stride + xs_int;
            uint32_t a00 = pa[0];
            uint32_t a01 = pa[1];
            uint32_t a10 = pa[alpha_stride];
            uint32_t a11 = pa[alpha_stride + 1];
            if(a00 == a01 && a00 == a10 && a00 == a11) {
                /*Uniform alpha: the weighting cancels out, so the plain interpolation below is
                 *already exact. Covers opaque interiors and fully transparent areas alike.*/
                alpha_out = a00;
                if(a00 == 0) {
                    abuf[x] = 0;
                    cbuf[x] = 0;
                    continue;
                }
            }
            else {
                /*Mixed alpha: weight the colors with their alpha so a transparent neighbor
                 *cannot bleed its color in, then normalize back to a straight color*/
                uint32_t t[4];
                uint32_t a = transform_alpha_weights(a00, a01, a10, a11, fx, fy, t);
                abuf[x] = (uint8_t)a;
                if(a == 0) {
                    cbuf[x] = 0;
                    continue;
                }
                const uint8_t * pcb = src + ys_int * src_stride + xs_int * 2;
                const lv_draw_sw_halfword_t * pc = (const lv_draw_sw_halfword_t *)pcb;
                const lv_draw_sw_halfword_t * pc2 = (const lv_draw_sw_halfword_t *)(pcb + src_stride);
                uint16_t s00 = lv_color_swap_16(pc[0].u16);
                uint16_t s01 = lv_color_swap_16(pc[1].u16);
                uint16_t s10 = lv_color_swap_16(pc2[0].u16);
                uint16_t s11 = lv_color_swap_16(pc2[1].u16);
                uint32_t w00 = (s00 | ((uint32_t)s00 << 16)) & 0x7E0F81F;
                uint32_t w01 = (s01 | ((uint32_t)s01 << 16)) & 0x7E0F81F;
                uint32_t w10 = (s10 | ((uint32_t)s10 << 16)) & 0x7E0F81F;
                uint32_t w11 = (s11 | ((uint32_t)s11 << 16)) & 0x7E0F81F;
                uint32_t acc = ((w00 * t[0] + w01 * t[1] + w10 * t[2] + w11 * t[3]) >> 5) & 0x7E0F81F;
                cbuf[x] = (uint16_t)(acc | (acc >> 16));
                continue;
            }
        }
        abuf[x] = (uint8_t)alpha_out;

        /*All the 4 neighbors are opaque: use true bilinear interpolation.
         *The RGB565 colors are expanded to 32 bits (0x07E0F81F mask) so all channels
         *can be interpolated with a single multiplication. 5 bit weights are used
         *so the 11 bit lanes can't overflow (63 * 32 < 2048).*/
        const uint8_t * pb = src + ys_int * src_stride + xs_int * 2;
        const lv_draw_sw_halfword_t * p = (const lv_draw_sw_halfword_t *)pb;
        const lv_draw_sw_halfword_t * p2 = (const lv_draw_sw_halfword_t *)(pb + src_stride);
        uint16_t c00 = lv_color_swap_16(p[0].u16);
        uint16_t c01 = lv_color_swap_16(p[1].u16);
        uint16_t c10 = lv_color_swap_16(p2[0].u16);
        uint16_t c11 = lv_color_swap_16(p2[1].u16);
        uint32_t e00 = (c00 | ((uint32_t)c00 << 16)) & 0x7E0F81F;
        uint32_t e01 = (c01 | ((uint32_t)c01 << 16)) & 0x7E0F81F;
        uint32_t e10 = (c10 | ((uint32_t)c10 << 16)) & 0x7E0F81F;
        uint32_t e11 = (c11 | ((uint32_t)c11 << 16)) & 0x7E0F81F;
        uint32_t fx5 = (fx + 4) >> 3;
        uint32_t fy5 = (fy + 4) >> 3;
        uint32_t top = ((e00 * (32 - fx5) + e01 * fx5) >> 5) & 0x7E0F81F;
        uint32_t bot = ((e10 * (32 - fx5) + e11 * fx5) >> 5) & 0x7E0F81F;
        uint32_t res = ((top * (32 - fy5) + bot * fy5) >> 5) & 0x7E0F81F;
        cbuf[x] = (uint16_t)(res | (res >> 16));
    }
}

static void rgb565a8_swapped_row_checked(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                                         const lv_opa_t * src_alpha, int32_t alpha_stride,
                                         int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                                         int32_t x_from, int32_t x_to, int32_t x_offs, int32_t xs_clamp_ups,
                                         uint16_t * cbuf, uint8_t * abuf, bool src_has_a8, bool aa)
{
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t x_abs = x + x_offs;
        int32_t xs_ups = xs_base + ((xs_step * x_abs) >> 8);
        int32_t ys_ups = ys_base + ((ys_step * x_abs) >> 8);
        if(xs_ups > xs_clamp_ups) xs_ups = xs_clamp_ups;

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        /*Fully out of the image*/
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            cbuf[x] = 0;
            abuf[x] = 0x00;
            continue;
        }

        if(!aa) {
            uint16_t c = ((const lv_draw_sw_halfword_t *)(src + ys_int * src_stride + xs_int * 2))->u16;
            cbuf[x] = lv_color_swap_16(c);
            abuf[x] = src_has_a8 ? src_alpha[ys_int * alpha_stride + xs_int] : 0xFF;
            continue;
        }

        /*Sample the 2x2 neighborhood. Taps outside the image count as fully transparent,
         *so the edge fades out on its own and needs no dedicated handling.*/
        int32_t xs_ofs = xs_ups - 0x80;
        int32_t ys_ofs = ys_ups - 0x80;
        int32_t x0 = xs_ofs >> 8;
        int32_t y0 = ys_ofs >> 8;

        uint32_t e[4] = {0, 0, 0, 0};
        uint32_t a[4] = {0, 0, 0, 0};
        int32_t i;
        for(i = 0; i < 4; i++) {
            int32_t sx = x0 + (i & 1);
            int32_t sy = y0 + (i >> 1);
            if(sx >= 0 && sx < src_w && sy >= 0 && sy < src_h) {
                uint16_t c = ((const lv_draw_sw_halfword_t *)(src + sy * src_stride + sx * 2))->u16;
                uint16_t cc = lv_color_swap_16(c);
                e[i] = (cc | ((uint32_t)cc << 16)) & 0x7E0F81F;
                a[i] = src_has_a8 ? src_alpha[sy * alpha_stride + sx] : 0xFF;
            }
        }

        uint32_t t[4];
        uint32_t av = transform_alpha_weights(a[0], a[1], a[2], a[3], xs_ofs & 0xFF, ys_ofs & 0xFF, t);
        abuf[x] = (uint8_t)av;
        if(av == 0) {
            cbuf[x] = 0;
            continue;
        }

        uint32_t acc = ((e[0] * t[0] + e[1] * t[1] + e[2] * t[2] + e[3] * t[3]) >> 5) & 0x7E0F81F;
        cbuf[x] = (uint16_t)(acc | (acc >> 16));
    }
}

static void transform_rgb565a8_swapped(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                                       int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                                       int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                                       uint16_t * cbuf, uint8_t * abuf, bool src_has_a8, bool aa)
{
    const lv_opa_t * src_alpha = src + src_stride * src_h;

    /*Must be signed type, because we would use negative array index calculated from stride*/
    int32_t alpha_stride = src_stride / 2; /*alpha map stride is always half of RGB map stride*/

    /*The destination is indexed from zero while the source coordinates are calculated from the
     *absolute (image local) x coordinate to keep the partial rendering deterministic*/
    int32_t w = x_end - x_start;

    /*In the middle of the row the pixel and all its neighbors are inside the source image,
     *so neither bounds checking nor edge handling is needed there*/
    int32_t fast_from, fast_to;
    transform_safe_range(xs_ups, ys_ups, xs_step, ys_step, src_w, src_h, x_start, x_end, aa, &fast_from, &fast_to);
    fast_from -= x_start;
    fast_to -= x_start;

    rgb565a8_swapped_row_checked(src, src_w, src_h, src_stride, src_alpha, alpha_stride, xs_ups, ys_ups, xs_step, ys_step,
                                 0, fast_from, x_start, xs_clamp_ups, cbuf, abuf, src_has_a8, aa);

    if(aa) {
        rgb565a8_swapped_row_fast(src, src_stride, src_alpha, alpha_stride, xs_ups, ys_ups, xs_step, ys_step,
                                  fast_from, fast_to, x_start, cbuf, abuf, src_has_a8);
    }
    else {
        int32_t x;
        for(x = fast_from; x < fast_to; x++) {
            int32_t x_abs = x + x_start;
            int32_t xs_int = (xs_ups + ((xs_step * x_abs) >> 8)) >> 8;
            int32_t ys_int = (ys_ups + ((ys_step * x_abs) >> 8)) >> 8;
            cbuf[x] = lv_color_swap_16(((const lv_draw_sw_halfword_t *)(src + ys_int * src_stride + xs_int * 2))->u16);
            abuf[x] = src_has_a8 ? src_alpha[ys_int * alpha_stride + xs_int] : 0xFF;
        }
    }

    rgb565a8_swapped_row_checked(src, src_w, src_h, src_stride, src_alpha, alpha_stride, xs_ups, ys_ups, xs_step, ys_step,
                                 fast_to, w, x_start, xs_clamp_ups, cbuf, abuf, src_has_a8, aa);
}

#endif

#if LV_DRAW_SW_SUPPORT_A8

static void transform_a8(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                         int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                         int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                         uint8_t * abuf, bool aa)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;
    int32_t w = x_end - x_start;

    int32_t x;
    for(x = 0; x < w; x++) {
        int32_t x_abs = x + x_start;
        int32_t xu = xs_ups_start + ((xs_step * x_abs) >> 8);
        int32_t yu = ys_ups_start + ((ys_step * x_abs) >> 8);
        if(xu > xs_clamp_ups) xu = xs_clamp_ups;

        int32_t xi = xu >> 8;
        int32_t yi = yu >> 8;
        if(xi < 0 || xi >= src_w || yi < 0 || yi >= src_h) {
            abuf[x] = 0x00;
            continue;
        }

        if(!aa) {
            abuf[x] = src[yi * src_stride + xi];
            continue;
        }

        int32_t xo = xu - 0x80;
        int32_t yo = yu - 0x80;
        int32_t x0 = xo >> 8;
        int32_t y0 = yo >> 8;
        uint32_t a[4] = {0, 0, 0, 0};
        int32_t i;
        for(i = 0; i < 4; i++) {
            int32_t sx = x0 + (i & 1);
            int32_t sy = y0 + (i >> 1);
            if(sx >= 0 && sx < src_w && sy >= 0 && sy < src_h) a[i] = src[sy * src_stride + sx];
        }

        /*An alpha only image carries no color, so nothing can bleed and plain bilinear
         *interpolation is already the correct filter*/
        uint32_t fx = xo & 0xFF;
        uint32_t fy = yo & 0xFF;
        uint32_t top = a[0] * (256 - fx) + a[1] * fx;
        uint32_t bot = a[2] * (256 - fx) + a[3] * fx;
        abuf[x] = (uint8_t)((top * (256 - fy) + bot * fy) >> 16);
    }
}

#endif

#if LV_DRAW_SW_SUPPORT_L8 || LV_DRAW_SW_SUPPORT_AL88

/*The interior of a row: every one of the four neighbors is inside the image, so no bounds
 *checking and no edge handling is needed*/
static void al88_row_fast(const uint8_t * src, int32_t src_stride,
                          int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                          int32_t x_from, int32_t x_to, int32_t x_offs,
                          uint8_t * cbuf, uint8_t * abuf, bool src_has_a8)
{
    int32_t px_size = src_has_a8 ? 2 : 1;
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t x_abs = x + x_offs;
        int32_t xs_ups_ofs = xs_base + ((xs_step * x_abs) >> 8) - 0x80;
        int32_t ys_ups_ofs = ys_base + ((ys_step * x_abs) >> 8) - 0x80;
        int32_t xs_int = xs_ups_ofs >> 8;
        int32_t ys_int = ys_ups_ofs >> 8;
        uint32_t fx = xs_ups_ofs & 0xFF;
        uint32_t fy = ys_ups_ofs & 0xFF;

        const uint8_t * p = src + ys_int * src_stride + xs_int * px_size;
        const uint8_t * p2 = p + src_stride;
        uint32_t l00 = p[0];
        uint32_t l01 = p[px_size];
        uint32_t l10 = p2[0];
        uint32_t l11 = p2[px_size];
        uint32_t a00, a01, a10, a11;
        if(src_has_a8) {
            a00 = p[1];
            a01 = p[px_size + 1];
            a10 = p2[1];
            a11 = p2[px_size + 1];
        }
        else {
            a00 = a01 = a10 = a11 = 0xFF;
        }

        if(a00 == a01 && a00 == a10 && a00 == a11) {
            /*Uniform alpha: the weighting cancels, plain bilinear on the luminance is exact*/
            abuf[x] = (uint8_t)a00;
            if(a00 == 0) {
                cbuf[x] = 0;
                continue;
            }
            uint32_t top = l00 * (256 - fx) + l01 * fx;
            uint32_t bot = l10 * (256 - fx) + l11 * fx;
            cbuf[x] = (uint8_t)((top * (256 - fy) + bot * fy) >> 16);
        }
        else {
            /*Mixed alpha: weight the luminance with alpha so a transparent neighbor cannot
             *bleed in, then normalize back*/
            uint32_t t[4];
            uint32_t av = transform_alpha_weights(a00, a01, a10, a11, fx, fy, t);
            abuf[x] = (uint8_t)av;
            if(av == 0) {
                cbuf[x] = 0;
                continue;
            }
            cbuf[x] = (uint8_t)((l00 * t[0] + l01 * t[1] + l10 * t[2] + l11 * t[3]) >> 5);
        }
    }
}

/*The edges of a row, where a neighbor can fall outside the image. An outside tap counts as
 *fully transparent, which is what makes the edge fade out.*/
static void al88_row_checked(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                             int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                             int32_t x_from, int32_t x_to, int32_t x_offs, int32_t xs_clamp_ups,
                             uint8_t * cbuf, uint8_t * abuf, bool src_has_a8, bool aa)
{
    int32_t px_size = src_has_a8 ? 2 : 1;
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t x_abs = x + x_offs;
        int32_t xu = xs_base + ((xs_step * x_abs) >> 8);
        int32_t yu = ys_base + ((ys_step * x_abs) >> 8);
        if(xu > xs_clamp_ups) xu = xs_clamp_ups;

        int32_t xi = xu >> 8;
        int32_t yi = yu >> 8;
        if(xi < 0 || xi >= src_w || yi < 0 || yi >= src_h) {
            cbuf[x] = 0x00;
            abuf[x] = 0x00;
            continue;
        }

        if(!aa) {
            const uint8_t * pn = src + yi * src_stride + xi * px_size;
            cbuf[x] = pn[0];
            abuf[x] = src_has_a8 ? pn[1] : 0xFF;
            continue;
        }

        int32_t xo = xu - 0x80;
        int32_t yo = yu - 0x80;
        int32_t x0 = xo >> 8;
        int32_t y0 = yo >> 8;
        uint32_t fx = xo & 0xFF;
        uint32_t fy = yo & 0xFF;

        uint32_t l00 = 0, l01 = 0, l10 = 0, l11 = 0;
        uint32_t a00 = 0, a01 = 0, a10 = 0, a11 = 0;
        bool y0_in = y0 >= 0 && y0 < src_h;
        bool y1_in = y0 + 1 >= 0 && y0 + 1 < src_h;
        bool x0_in = x0 >= 0 && x0 < src_w;
        bool x1_in = x0 + 1 >= 0 && x0 + 1 < src_w;
        if(y0_in) {
            const uint8_t * pr = src + y0 * src_stride;
            if(x0_in) {
                l00 = pr[x0 * px_size];
                a00 = src_has_a8 ? pr[x0 * px_size + 1] : 0xFF;
            }
            if(x1_in) {
                l01 = pr[(x0 + 1) * px_size];
                a01 = src_has_a8 ? pr[(x0 + 1) * px_size + 1] : 0xFF;
            }
        }
        if(y1_in) {
            const uint8_t * pr = src + (y0 + 1) * src_stride;
            if(x0_in) {
                l10 = pr[x0 * px_size];
                a10 = src_has_a8 ? pr[x0 * px_size + 1] : 0xFF;
            }
            if(x1_in) {
                l11 = pr[(x0 + 1) * px_size];
                a11 = src_has_a8 ? pr[(x0 + 1) * px_size + 1] : 0xFF;
            }
        }

        if(a00 == a01 && a00 == a10 && a00 == a11) {
            abuf[x] = (uint8_t)a00;
            if(a00 == 0) {
                cbuf[x] = 0;
                continue;
            }
            uint32_t top = l00 * (256 - fx) + l01 * fx;
            uint32_t bot = l10 * (256 - fx) + l11 * fx;
            cbuf[x] = (uint8_t)((top * (256 - fy) + bot * fy) >> 16);
        }
        else {
            uint32_t t[4];
            uint32_t av = transform_alpha_weights(a00, a01, a10, a11, fx, fy, t);
            abuf[x] = (uint8_t)av;
            if(av == 0) {
                cbuf[x] = 0;
                continue;
            }
            cbuf[x] = (uint8_t)((l00 * t[0] + l01 * t[1] + l10 * t[2] + l11 * t[3]) >> 5);
        }
    }
}

static void transform_al88(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                           int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                           int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                           uint8_t * cbuf, uint8_t * abuf, bool src_has_a8, bool aa)
{
    int32_t w = x_end - x_start;
    int32_t px_size = src_has_a8 ? 2 : 1;

    /*In the middle of the row the pixel and all its neighbors are inside the source image,
     *so neither bounds checking nor edge handling is needed there*/
    int32_t fast_from, fast_to;
    transform_safe_range(xs_ups, ys_ups, xs_step, ys_step, src_w, src_h, x_start, x_end, aa, &fast_from, &fast_to);
    fast_from -= x_start;
    fast_to -= x_start;

    al88_row_checked(src, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step, ys_step,
                     0, fast_from, x_start, xs_clamp_ups, cbuf, abuf, src_has_a8, aa);

    if(aa) {
        al88_row_fast(src, src_stride, xs_ups, ys_ups, xs_step, ys_step,
                      fast_from, fast_to, x_start, cbuf, abuf, src_has_a8);
    }
    else {
        int32_t x;
        for(x = fast_from; x < fast_to; x++) {
            int32_t x_abs = x + x_start;
            int32_t xs_int = (xs_ups + ((xs_step * x_abs) >> 8)) >> 8;
            int32_t ys_int = (ys_ups + ((ys_step * x_abs) >> 8)) >> 8;
            const uint8_t * pn = src + ys_int * src_stride + xs_int * px_size;
            cbuf[x] = pn[0];
            abuf[x] = src_has_a8 ? pn[1] : 0xFF;
        }
    }

    al88_row_checked(src, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step, ys_step,
                     fast_to, w, x_start, xs_clamp_ups, cbuf, abuf, src_has_a8, aa);
}

#endif

static void transform_safe_interval(int32_t base, int32_t step, int32_t lo, int32_t hi,
                                    int32_t x_from, int32_t x_to, int32_t * res_from, int32_t * res_to)
{
    int32_t xa = x_from;
    int32_t xb = x_to;

    /* Solve lo <= base + ((step * x) >> 8) <= hi.
     * As `>> 8` is a floor operation:
     *    floor(t / 256) >= k  <=>  t >= 256 * k
     *    floor(t / 256) <= k  <=>  t <= 256 * k + 255  */
    /*Multiply instead of shifting: the difference can be negative and
     *left shifting a negative value is undefined behavior*/
    int64_t lo_n = (int64_t)(lo - base) * 256;
    int64_t hi_n = (int64_t)(hi - base) * 256 + 255;

    if(step == 0) {
        if(lo_n > 0 || hi_n < 0) xb = xa;
    }
    else if(step > 0) {
        int64_t from = lo_n > 0 ? (lo_n + step - 1) / step : -((-lo_n) / step);     /* ceil(lo_n / step) */
        int64_t to = hi_n >= 0 ? hi_n / step : -((-hi_n + step - 1) / step);        /* floor(hi_n / step) */
        if(from > x_to) from = x_to;
        if(to < x_from - 1) to = x_from - 1;
        if((int32_t)from > xa) xa = (int32_t)from;
        if((int32_t)(to + 1) < xb) xb = (int32_t)(to + 1);
    }
    else {
        int64_t step_p = -(int64_t)step;
        int64_t to = lo_n <= 0 ? (-lo_n) / step_p : -((lo_n + step_p - 1) / step_p);     /* floor(lo_n / step) */
        int64_t from = hi_n <= 0 ? ((-hi_n) + step_p - 1) / step_p : -(hi_n / step_p);   /* ceil(hi_n / step) */
        if(from > x_to) from = x_to;
        if(to < x_from - 1) to = x_from - 1;
        if((int32_t)from > xa) xa = (int32_t)from;
        if((int32_t)(to + 1) < xb) xb = (int32_t)(to + 1);
    }

    if(xb < xa) xb = xa;
    *res_from = xa;
    *res_to = xb;
}

static void transform_safe_range(int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                                 int32_t src_w, int32_t src_h, int32_t x_from, int32_t x_to,
                                 bool aa, int32_t * res_from, int32_t * res_to)
{
    /*With anti-aliasing both direct neighbors are needed, without it only the pixel itself*/
    int32_t lo = aa ? 256 : 0;
    int32_t hi_x = aa ? (src_w - 1) * 256 - 1 : src_w * 256 - 1;
    int32_t hi_y = aa ? (src_h - 1) * 256 - 1 : src_h * 256 - 1;

    int32_t xa, xb, ya, yb;
    transform_safe_interval(xs_ups, xs_step, lo, hi_x, x_from, x_to, &xa, &xb);
    transform_safe_interval(ys_ups, ys_step, lo, hi_y, x_from, x_to, &ya, &yb);
    if(ya > xa) xa = ya;
    if(yb < xb) xb = yb;
    if(xb < xa) xb = xa;
    *res_from = xa;
    *res_to = xb;
}

#endif /*LV_USE_DRAW_SW*/
