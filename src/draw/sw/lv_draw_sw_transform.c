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
    int32_t sxx = (int32_t)(((int64_t)cosma << 14) / tr_dsc.scale_x);
    int32_t sxy = (int32_t)((-(int64_t)sinma << 14) / tr_dsc.scale_x);
    int32_t syx = (int32_t)(((int64_t)sinma << 14) / tr_dsc.scale_y);
    int32_t syy = (int32_t)(((int64_t)cosma << 14) / tr_dsc.scale_y);

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

#if LV_DRAW_SW_SUPPORT_RGB888 || LV_DRAW_SW_SUPPORT_XRGB8888

static void rgb888_row_checked(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                               int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                               int32_t x_from, int32_t x_to, int32_t xs_clamp_ups,
                               lv_color32_t * dest_c32, bool aa, uint32_t px_size)
{
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t xs_ups = xs_base + ((xs_step * x) >> 8);
        int32_t ys_ups = ys_base + ((ys_step * x) >> 8);
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
                                int32_t x_from, int32_t x_to, lv_color32_t * dest_c32, uint32_t px_size)
{
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t xs_ups = xs_base + ((xs_step * x) >> 8) - 0x80;
        int32_t ys_ups = ys_base + ((ys_step * x) >> 8) - 0x80;
        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        const uint8_t * p = src + ys_int * src_stride + xs_int * px_size;
        uint32_t p00, p01, p10, p11;
        if(px_size == 4) {
            p00 = *(const uint32_t *)p;
            p01 = *(const uint32_t *)(p + 4);
            p10 = *(const uint32_t *)(p + src_stride);
            p11 = *(const uint32_t *)(p + src_stride + 4);
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
        *(uint32_t *)&dest_c32[x] = rb | g | 0xFF000000;
    }
}

static void transform_rgb888(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                             int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                             int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                             uint8_t * dest_buf, bool aa, uint32_t px_size)
{
    lv_color32_t * dest_c32 = (lv_color32_t *) dest_buf - x_start;

    /*In the middle of the row the pixel and all its neighbors are inside the source image,
     *so neither bounds checking nor edge handling is needed there*/
    int32_t fast_from, fast_to;
    transform_safe_range(xs_ups, ys_ups, xs_step, ys_step, src_w, src_h, x_start, x_end, aa, &fast_from, &fast_to);

    rgb888_row_checked(src, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step, ys_step,
                       x_start, fast_from, xs_clamp_ups, dest_c32, aa, px_size);

    if(aa) {
        rgb888_row_bilinear(src, src_stride, xs_ups, ys_ups, xs_step, ys_step, fast_from, fast_to, dest_c32, px_size);
    }
    else {
        int32_t x;
        for(x = fast_from; x < fast_to; x++) {
            int32_t xs_int = (xs_ups + ((xs_step * x) >> 8)) >> 8;
            int32_t ys_int = (ys_ups + ((ys_step * x) >> 8)) >> 8;
            const uint8_t * src_u8 = &src[ys_int * src_stride + xs_int * px_size];
            dest_c32[x].red = src_u8[2];
            dest_c32[x].green = src_u8[1];
            dest_c32[x].blue = src_u8[0];
            dest_c32[x].alpha = 0xff;
        }
    }

    rgb888_row_checked(src, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step, ys_step,
                       fast_to, x_end, xs_clamp_ups, dest_c32, aa, px_size);
}

#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888

/**
 * The direction based anti-aliasing of one ARGB8888 pixel whose neighbors are surely valid.
 * It's used instead of bilinear interpolation when some involved pixels are not opaque
 * to avoid bleeding in the color of transparent pixels.
 */
static inline lv_color32_t argb8888_px_aa_inside(const lv_color32_t * src_px, int32_t src_stride,
                                                 int32_t xs_fract_raw, int32_t ys_fract_raw)
{
    int32_t x_next;
    int32_t y_next;
    int32_t xs_fract;
    int32_t ys_fract;
    if(xs_fract_raw < 0x80) {
        x_next = -1;
        xs_fract = 0x7F - xs_fract_raw;
    }
    else {
        x_next = 1;
        xs_fract = xs_fract_raw - 0x80;
    }
    if(ys_fract_raw < 0x80) {
        y_next = -1;
        ys_fract = 0x7F - ys_fract_raw;
    }
    else {
        y_next = 1;
        ys_fract = ys_fract_raw - 0x80;
    }

    lv_color32_t d = src_px[0];
    lv_color32_t px_hor = src_px[x_next];
    lv_color32_t px_ver = *(const lv_color32_t *)((const uint8_t *)src_px + y_next * src_stride);

    if(px_ver.alpha == 0) {
        d.alpha = (d.alpha * (0xFF - ys_fract)) >> 8;
    }
    else if(!lv_color32_eq(d, px_ver)) {
        if(d.alpha) d.alpha = ((px_ver.alpha * ys_fract) + (d.alpha * (0xFF - ys_fract))) >> 8;
        px_ver.alpha = ys_fract;
        d = lv_color_mix32_inlined(px_ver, d);
    }

    if(px_hor.alpha == 0) {
        d.alpha = (d.alpha * (0xFF - xs_fract)) >> 8;
    }
    else if(!lv_color32_eq(d, px_hor)) {
        if(d.alpha) d.alpha = ((px_hor.alpha * xs_fract) + (d.alpha * (0xFF - xs_fract))) >> 8;
        px_hor.alpha = xs_fract;
        d = lv_color_mix32_inlined(px_hor, d);
    }
    return d;
}

static void argb8888_row_checked(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                                 int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                                 int32_t x_from, int32_t x_to, int32_t xs_clamp_ups,
                                 lv_color32_t * dest_c32, bool aa)
{
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t xs_ups = xs_base + ((xs_step * x) >> 8);
        int32_t ys_ups = ys_base + ((ys_step * x) >> 8);
        if(xs_ups > xs_clamp_ups) xs_ups = xs_clamp_ups;

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        /*Fully out of the image*/
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            *(uint32_t *)&dest_c32[x] = 0x00000000;
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
        }
        /*Partially out of the image*/
        else {
            if((xs_int == 0 && x_next < 0) || (xs_int == src_w - 1 && x_next > 0))  {
                dest_c32[x].alpha = (dest_c32[x].alpha * (0x7F - xs_fract)) >> 7;
            }
            else if((ys_int == 0 && y_next < 0) || (ys_int == src_h - 1 && y_next > 0))  {
                dest_c32[x].alpha = (dest_c32[x].alpha * (0x7F - ys_fract)) >> 7;
            }
        }
    }
}

static void argb8888_row_fast(const uint8_t * src, int32_t src_stride,
                              int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                              int32_t x_from, int32_t x_to, lv_color32_t * dest_c32)
{
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t xs_ups = xs_base + ((xs_step * x) >> 8);
        int32_t ys_ups = ys_base + ((ys_step * x) >> 8);
        int32_t xs_ups_ofs = xs_ups - 0x80;
        int32_t ys_ups_ofs = ys_ups - 0x80;
        int32_t xs_int = xs_ups_ofs >> 8;
        int32_t ys_int = ys_ups_ofs >> 8;

        const uint8_t * p = src + ys_int * src_stride + xs_int * 4;
        uint32_t p00 = *(const uint32_t *)p;
        uint32_t p01 = *(const uint32_t *)(p + 4);
        uint32_t p10 = *(const uint32_t *)(p + src_stride);
        uint32_t p11 = *(const uint32_t *)(p + src_stride + 4);

        if(((p00 & p01 & p10 & p11) >> 24) == 0xFF) {
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
            *(uint32_t *)&dest_c32[x] = rb | g | 0xFF000000;
        }
        else {
            /*There are non opaque pixels involved: use the direction based mixing
             *to avoid bleeding in the color of transparent pixels*/
            const lv_color32_t * src_px = (const lv_color32_t *)(src + (ys_ups >> 8) * src_stride + (xs_ups >> 8) * 4);
            dest_c32[x] = argb8888_px_aa_inside(src_px, src_stride, xs_ups & 0xFF, ys_ups & 0xFF);
        }
    }
}

static void transform_argb8888(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                               int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                               int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                               uint8_t * dest_buf, bool aa)
{
    lv_color32_t * dest_c32 = (lv_color32_t *) dest_buf - x_start;

    /*In the middle of the row the pixel and all its neighbors are inside the source image,
     *so neither bounds checking nor edge handling is needed there*/
    int32_t fast_from, fast_to;
    transform_safe_range(xs_ups, ys_ups, xs_step, ys_step, src_w, src_h, x_start, x_end, aa, &fast_from, &fast_to);

    argb8888_row_checked(src, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step, ys_step,
                         x_start, fast_from, xs_clamp_ups, dest_c32, aa);

    if(aa) {
        argb8888_row_fast(src, src_stride, xs_ups, ys_ups, xs_step, ys_step, fast_from, fast_to, dest_c32);
    }
    else {
        int32_t x;
        for(x = fast_from; x < fast_to; x++) {
            int32_t xs_int = (xs_ups + ((xs_step * x) >> 8)) >> 8;
            int32_t ys_int = (ys_ups + ((ys_step * x) >> 8)) >> 8;
            *(uint32_t *)&dest_c32[x] = *(const uint32_t *)(src + ys_int * src_stride + xs_int * 4);
        }
    }

    argb8888_row_checked(src, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step, ys_step,
                         fast_to, x_end, xs_clamp_ups, dest_c32, aa);
}


#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED

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

static void transform_argb8888_premultiplied(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                                             int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                                             int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                                             uint8_t * dest_buf, bool aa)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;
    lv_color32_t * dest_c32 = (lv_color32_t *) dest_buf - x_start;

    int32_t x;
    for(x = x_start; x < x_end; x++) {
        xs_ups = xs_ups_start + ((xs_step * x) >> 8);
        ys_ups = ys_ups_start + ((ys_step * x) >> 8);
        if(xs_ups > xs_clamp_ups) xs_ups = xs_clamp_ups;

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        /*Fully out of the image*/
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            *(uint32_t *)&dest_c32[x] = 0x00000000;
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

/**
 * The direction based anti-aliasing of one RGB565(A8) pixel whose neighbors are surely valid.
 * It's used instead of bilinear interpolation when some involved pixels are not opaque
 * to avoid bleeding in the color of transparent pixels.
 */
static inline void rgb565a8_px_aa_inside(const uint16_t * src_px, int32_t src_stride,
                                         const lv_opa_t * alpha_px, int32_t alpha_stride,
                                         int32_t xs_fract_raw, int32_t ys_fract_raw,
                                         uint16_t * c_out, uint8_t * a_out)
{
    int32_t x_next;
    int32_t y_next;
    int32_t xs_fract;
    int32_t ys_fract;
    if(xs_fract_raw < 0x80) {
        x_next = -1;
        xs_fract = (0x7F - xs_fract_raw) * 2;
    }
    else {
        x_next = 1;
        xs_fract = (xs_fract_raw - 0x80) * 2;
    }
    if(ys_fract_raw < 0x80) {
        y_next = -1;
        ys_fract = (0x7F - ys_fract_raw) * 2;
    }
    else {
        y_next = 1;
        ys_fract = (ys_fract_raw - 0x80) * 2;
    }

    uint16_t c = src_px[0];

    uint8_t a = alpha_px[0];
    lv_opa_t a_hor = alpha_px[x_next];
    lv_opa_t a_ver = alpha_px[y_next * alpha_stride];
    if(a_ver != a) a_ver = ((a_ver * ys_fract) + (a * (0x100 - ys_fract))) >> 8;
    if(a_hor != a) a_hor = ((a_hor * xs_fract) + (a * (0x100 - xs_fract))) >> 8;
    a = (a_ver + a_hor) >> 1;
    *a_out = a;
    if(a == 0x00) return;

    uint16_t px_hor = src_px[x_next];
    uint16_t px_ver = *(const uint16_t *)((const uint8_t *)src_px + y_next * src_stride);
    if(c != px_ver || c != px_hor) {
        uint16_t v = lv_color_16_16_mix_inlined(px_ver, c, ys_fract);
        uint16_t h = lv_color_16_16_mix_inlined(px_hor, c, xs_fract);
        c = lv_color_16_16_mix_inlined(h, v, LV_OPA_50);
    }
    *c_out = c;
}

static void rgb565a8_row_fast(const uint8_t * src, int32_t src_stride,
                              const lv_opa_t * src_alpha, int32_t alpha_stride,
                              int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                              int32_t x_from, int32_t x_to, uint16_t * cbuf, uint8_t * abuf, bool src_has_a8)
{
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t xs_ups = xs_base + ((xs_step * x) >> 8);
        int32_t ys_ups = ys_base + ((ys_step * x) >> 8);
        int32_t xs_ups_ofs = xs_ups - 0x80;
        int32_t ys_ups_ofs = ys_ups - 0x80;
        int32_t xs_int = xs_ups_ofs >> 8;
        int32_t ys_int = ys_ups_ofs >> 8;
        uint32_t fx = xs_ups_ofs & 0xFF;
        uint32_t fy = ys_ups_ofs & 0xFF;

        if(src_has_a8) {
            const lv_opa_t * pa = src_alpha + ys_int * alpha_stride + xs_int;
            uint32_t a00 = pa[0];
            uint32_t a01 = pa[1];
            uint32_t a10 = pa[alpha_stride];
            uint32_t a11 = pa[alpha_stride + 1];
            if((a00 & a01 & a10 & a11) != 0xFF) {
                /*There are non opaque pixels involved: use the direction based mixing
                 *to avoid bleeding in the color of transparent pixels*/
                rgb565a8_px_aa_inside((const uint16_t *)(src + (ys_ups >> 8) * src_stride) + (xs_ups >> 8),
                                      src_stride, src_alpha + (ys_ups >> 8) * alpha_stride + (xs_ups >> 8), alpha_stride,
                                      xs_ups & 0xFF, ys_ups & 0xFF, &cbuf[x], &abuf[x]);
                continue;
            }
        }
        abuf[x] = 0xFF;

        /*All the 4 neighbors are opaque: use true bilinear interpolation.
         *The RGB565 colors are expanded to 32 bits (0x07E0F81F mask) so all channels
         *can be interpolated with a single multiplication. 5 bit weights are used
         *so the 11 bit lanes can't overflow (63 * 32 < 2048).*/
        const uint16_t * p = (const uint16_t *)(src + ys_int * src_stride) + xs_int;
        const uint16_t * p2 = (const uint16_t *)((const uint8_t *)p + src_stride);
        uint32_t e00 = (p[0] | ((uint32_t)p[0] << 16)) & 0x7E0F81F;
        uint32_t e01 = (p[1] | ((uint32_t)p[1] << 16)) & 0x7E0F81F;
        uint32_t e10 = (p2[0] | ((uint32_t)p2[0] << 16)) & 0x7E0F81F;
        uint32_t e11 = (p2[1] | ((uint32_t)p2[1] << 16)) & 0x7E0F81F;
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
                                 int32_t x_from, int32_t x_to, int32_t xs_clamp_ups,
                                 uint16_t * cbuf, uint8_t * abuf, bool src_has_a8, bool aa)
{
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t xs_ups = xs_base + ((xs_step * x) >> 8);
        int32_t ys_ups = ys_base + ((ys_step * x) >> 8);
        if(xs_ups > xs_clamp_ups) xs_ups = xs_clamp_ups;

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        /*Fully out of the image*/
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            abuf[x] = 0x00;
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
            xs_fract = (0x7F - xs_fract) * 2;
        }
        else {
            x_next = 1;
            xs_fract = (xs_fract - 0x80) * 2;
        }
        if(ys_fract < 0x80) {
            y_next = -1;
            ys_fract = (0x7F - ys_fract) * 2;
        }
        else {
            y_next = 1;
            ys_fract = (ys_fract - 0x80) * 2;
        }

        const uint16_t * src_tmp_u16 = (const uint16_t *)(src + (ys_int * src_stride) + xs_int * 2);
        cbuf[x] = src_tmp_u16[0];

        if(aa &&
           xs_int + x_next >= 0 &&
           xs_int + x_next <= src_w - 1 &&
           ys_int + y_next >= 0 &&
           ys_int + y_next <= src_h - 1) {

            uint16_t px_hor = src_tmp_u16[x_next];
            uint16_t px_ver = *(const uint16_t *)((uint8_t *)src_tmp_u16 + (y_next * src_stride));

            if(src_has_a8) {
                const lv_opa_t * src_alpha_tmp = src_alpha;
                src_alpha_tmp += (ys_int * alpha_stride) + xs_int;
                abuf[x] = src_alpha_tmp[0];

                lv_opa_t a_hor = src_alpha_tmp[x_next];
                lv_opa_t a_ver = src_alpha_tmp[y_next * alpha_stride];

                if(a_ver != abuf[x]) a_ver = ((a_ver * ys_fract) + (abuf[x] * (0x100 - ys_fract))) >> 8;
                if(a_hor != abuf[x]) a_hor = ((a_hor * xs_fract) + (abuf[x] * (0x100 - xs_fract))) >> 8;
                abuf[x] = (a_ver + a_hor) >> 1;

                if(abuf[x] == 0x00) continue;
            }
            else {
                abuf[x] = 0xff;
            }

            if(cbuf[x] != px_ver || cbuf[x] != px_hor) {
                uint16_t v = lv_color_16_16_mix_inlined(px_ver, cbuf[x], ys_fract);
                uint16_t h = lv_color_16_16_mix_inlined(px_hor, cbuf[x], xs_fract);
                cbuf[x] = lv_color_16_16_mix_inlined(h, v, LV_OPA_50);
            }
        }
        /*Partially out of the image*/
        else {
            lv_opa_t a;
            if(src_has_a8) {
                const lv_opa_t * src_alpha_tmp = src_alpha;
                src_alpha_tmp += (ys_int * alpha_stride) + xs_int;
                a = src_alpha_tmp[0];
            }
            else {
                a = 0xff;
            }

            if((xs_int == 0 && x_next < 0) || (xs_int == src_w - 1 && x_next > 0))  {
                abuf[x] = (a * (0xFF - xs_fract)) >> 8;
            }
            else if((ys_int == 0 && y_next < 0) || (ys_int == src_h - 1 && y_next > 0))  {
                abuf[x] = (a * (0xFF - ys_fract)) >> 8;
            }
            else {
                abuf[x] = a;
            }
        }
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

    cbuf -= x_start;
    abuf -= x_start;

    /*In the middle of the row the pixel and all its neighbors are inside the source image,
     *so neither bounds checking nor edge handling is needed there*/
    int32_t fast_from, fast_to;
    transform_safe_range(xs_ups, ys_ups, xs_step, ys_step, src_w, src_h, x_start, x_end, aa, &fast_from, &fast_to);

    rgb565a8_row_checked(src, src_w, src_h, src_stride, src_alpha, alpha_stride, xs_ups, ys_ups, xs_step, ys_step,
                         x_start, fast_from, xs_clamp_ups, cbuf, abuf, src_has_a8, aa);

    if(aa) {
        rgb565a8_row_fast(src, src_stride, src_alpha, alpha_stride, xs_ups, ys_ups, xs_step, ys_step,
                          fast_from, fast_to, cbuf, abuf, src_has_a8);
    }
    else {
        int32_t x;
        for(x = fast_from; x < fast_to; x++) {
            int32_t xs_int = (xs_ups + ((xs_step * x) >> 8)) >> 8;
            int32_t ys_int = (ys_ups + ((ys_step * x) >> 8)) >> 8;
            cbuf[x] = *((const uint16_t *)(src + ys_int * src_stride) + xs_int);
            abuf[x] = src_has_a8 ? src_alpha[ys_int * alpha_stride + xs_int] : 0xFF;
        }
    }

    rgb565a8_row_checked(src, src_w, src_h, src_stride, src_alpha, alpha_stride, xs_ups, ys_ups, xs_step, ys_step,
                         fast_to, x_end, xs_clamp_ups, cbuf, abuf, src_has_a8, aa);
}


#endif

#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED

/**
 * The direction based anti-aliasing of one byte-swapped RGB565(A8) pixel whose neighbors
 * are surely valid. It's used instead of bilinear interpolation when some involved pixels
 * are not opaque to avoid bleeding in the color of transparent pixels.
 */
static inline void rgb565a8_swapped_px_aa_inside(const uint16_t * src_px, int32_t src_stride,
                                                 const lv_opa_t * alpha_px, int32_t alpha_stride,
                                                 int32_t xs_fract_raw, int32_t ys_fract_raw,
                                                 uint16_t * c_out, uint8_t * a_out)
{
    int32_t x_next;
    int32_t y_next;
    int32_t xs_fract;
    int32_t ys_fract;
    if(xs_fract_raw < 0x80) {
        x_next = -1;
        xs_fract = (0x7F - xs_fract_raw) * 2;
    }
    else {
        x_next = 1;
        xs_fract = (xs_fract_raw - 0x80) * 2;
    }
    if(ys_fract_raw < 0x80) {
        y_next = -1;
        ys_fract = (0x7F - ys_fract_raw) * 2;
    }
    else {
        y_next = 1;
        ys_fract = (ys_fract_raw - 0x80) * 2;
    }

    uint16_t c = lv_color_swap_16(src_px[0]);

    uint8_t a = alpha_px[0];
    lv_opa_t a_hor = alpha_px[x_next];
    lv_opa_t a_ver = alpha_px[y_next * alpha_stride];
    if(a_ver != a) a_ver = ((a_ver * ys_fract) + (a * (0x100 - ys_fract))) >> 8;
    if(a_hor != a) a_hor = ((a_hor * xs_fract) + (a * (0x100 - xs_fract))) >> 8;
    a = (a_ver + a_hor) >> 1;
    *a_out = a;
    if(a == 0x00) return;

    uint16_t px_hor = lv_color_swap_16(src_px[x_next]);
    uint16_t px_ver = lv_color_swap_16(*(const uint16_t *)((const uint8_t *)src_px + y_next * src_stride));
    if(c != px_ver || c != px_hor) {
        uint16_t v = lv_color_16_16_mix_inlined(px_ver, c, ys_fract);
        uint16_t h = lv_color_16_16_mix_inlined(px_hor, c, xs_fract);
        c = lv_color_16_16_mix_inlined(h, v, LV_OPA_50);
    }
    *c_out = c;
}

static void rgb565a8_swapped_row_fast(const uint8_t * src, int32_t src_stride,
                                      const lv_opa_t * src_alpha, int32_t alpha_stride,
                                      int32_t xs_base, int32_t ys_base, int32_t xs_step, int32_t ys_step,
                                      int32_t x_from, int32_t x_to, uint16_t * cbuf, uint8_t * abuf, bool src_has_a8)
{
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t xs_ups = xs_base + ((xs_step * x) >> 8);
        int32_t ys_ups = ys_base + ((ys_step * x) >> 8);
        int32_t xs_ups_ofs = xs_ups - 0x80;
        int32_t ys_ups_ofs = ys_ups - 0x80;
        int32_t xs_int = xs_ups_ofs >> 8;
        int32_t ys_int = ys_ups_ofs >> 8;
        uint32_t fx = xs_ups_ofs & 0xFF;
        uint32_t fy = ys_ups_ofs & 0xFF;

        if(src_has_a8) {
            const lv_opa_t * pa = src_alpha + ys_int * alpha_stride + xs_int;
            uint32_t a00 = pa[0];
            uint32_t a01 = pa[1];
            uint32_t a10 = pa[alpha_stride];
            uint32_t a11 = pa[alpha_stride + 1];
            if((a00 & a01 & a10 & a11) != 0xFF) {
                /*There are non opaque pixels involved: use the direction based mixing
                 *to avoid bleeding in the color of transparent pixels*/
                rgb565a8_swapped_px_aa_inside((const uint16_t *)(src + (ys_ups >> 8) * src_stride) + (xs_ups >> 8),
                                              src_stride, src_alpha + (ys_ups >> 8) * alpha_stride + (xs_ups >> 8), alpha_stride,
                                              xs_ups & 0xFF, ys_ups & 0xFF, &cbuf[x], &abuf[x]);
                continue;
            }
        }
        abuf[x] = 0xFF;

        /*All the 4 neighbors are opaque: use true bilinear interpolation.
         *The RGB565 colors are expanded to 32 bits (0x07E0F81F mask) so all channels
         *can be interpolated with a single multiplication. 5 bit weights are used
         *so the 11 bit lanes can't overflow (63 * 32 < 2048).*/
        const uint16_t * p = (const uint16_t *)(src + ys_int * src_stride) + xs_int;
        const uint16_t * p2 = (const uint16_t *)((const uint8_t *)p + src_stride);
        uint16_t c00 = lv_color_swap_16(p[0]);
        uint16_t c01 = lv_color_swap_16(p[1]);
        uint16_t c10 = lv_color_swap_16(p2[0]);
        uint16_t c11 = lv_color_swap_16(p2[1]);
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
                                         int32_t x_from, int32_t x_to, int32_t xs_clamp_ups,
                                         uint16_t * cbuf, uint8_t * abuf, bool src_has_a8, bool aa)
{
    int32_t x;
    for(x = x_from; x < x_to; x++) {
        int32_t xs_ups = xs_base + ((xs_step * x) >> 8);
        int32_t ys_ups = ys_base + ((ys_step * x) >> 8);
        if(xs_ups > xs_clamp_ups) xs_ups = xs_clamp_ups;

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        /*Fully out of the image*/
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            abuf[x] = 0x00;
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
            xs_fract = (0x7F - xs_fract) * 2;
        }
        else {
            x_next = 1;
            xs_fract = (xs_fract - 0x80) * 2;
        }
        if(ys_fract < 0x80) {
            y_next = -1;
            ys_fract = (0x7F - ys_fract) * 2;
        }
        else {
            y_next = 1;
            ys_fract = (ys_fract - 0x80) * 2;
        }

        const uint16_t * src_tmp_u16 = (const uint16_t *)(src + (ys_int * src_stride) + xs_int * 2);
        cbuf[x] = lv_color_swap_16(src_tmp_u16[0]); /* swap the src pixels */

        if(aa &&
           xs_int + x_next >= 0 &&
           xs_int + x_next <= src_w - 1 &&
           ys_int + y_next >= 0 &&
           ys_int + y_next <= src_h - 1) {

            /* swap the src pixels */
            uint16_t px_hor = lv_color_swap_16(src_tmp_u16[x_next]);
            uint16_t px_ver = lv_color_swap_16(*(const uint16_t *)((uint8_t *)src_tmp_u16 + (y_next * src_stride)));

            if(src_has_a8) {
                const lv_opa_t * src_alpha_tmp = src_alpha;
                src_alpha_tmp += (ys_int * alpha_stride) + xs_int;
                abuf[x] = src_alpha_tmp[0];

                lv_opa_t a_hor = src_alpha_tmp[x_next];
                lv_opa_t a_ver = src_alpha_tmp[y_next * alpha_stride];

                if(a_ver != abuf[x]) a_ver = ((a_ver * ys_fract) + (abuf[x] * (0x100 - ys_fract))) >> 8;
                if(a_hor != abuf[x]) a_hor = ((a_hor * xs_fract) + (abuf[x] * (0x100 - xs_fract))) >> 8;
                abuf[x] = (a_ver + a_hor) >> 1;

                if(abuf[x] == 0x00) continue;
            }
            else {
                abuf[x] = 0xff;
            }

            if(cbuf[x] != px_ver || cbuf[x] != px_hor) {
                uint16_t v = lv_color_16_16_mix_inlined(px_ver, cbuf[x], ys_fract);
                uint16_t h = lv_color_16_16_mix_inlined(px_hor, cbuf[x], xs_fract);
                cbuf[x] =  lv_color_16_16_mix_inlined(h, v, LV_OPA_50);
            }
        }
        /*Partially out of the image*/
        else {
            lv_opa_t a;
            if(src_has_a8) {
                const lv_opa_t * src_alpha_tmp = src_alpha;
                src_alpha_tmp += (ys_int * alpha_stride) + xs_int;
                a = src_alpha_tmp[0];
            }
            else {
                a = 0xff;
            }

            if((xs_int == 0 && x_next < 0) || (xs_int == src_w - 1 && x_next > 0))  {
                abuf[x] = (a * (0xFF - xs_fract)) >> 8;
            }
            else if((ys_int == 0 && y_next < 0) || (ys_int == src_h - 1 && y_next > 0))  {
                abuf[x] = (a * (0xFF - ys_fract)) >> 8;
            }
            else {
                abuf[x] = a;
            }
        }
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

    cbuf -= x_start;
    abuf -= x_start;

    /*In the middle of the row the pixel and all its neighbors are inside the source image,
     *so neither bounds checking nor edge handling is needed there*/
    int32_t fast_from, fast_to;
    transform_safe_range(xs_ups, ys_ups, xs_step, ys_step, src_w, src_h, x_start, x_end, aa, &fast_from, &fast_to);

    rgb565a8_swapped_row_checked(src, src_w, src_h, src_stride, src_alpha, alpha_stride, xs_ups, ys_ups, xs_step, ys_step,
                                 x_start, fast_from, xs_clamp_ups, cbuf, abuf, src_has_a8, aa);

    if(aa) {
        rgb565a8_swapped_row_fast(src, src_stride, src_alpha, alpha_stride, xs_ups, ys_ups, xs_step, ys_step,
                                  fast_from, fast_to, cbuf, abuf, src_has_a8);
    }
    else {
        int32_t x;
        for(x = fast_from; x < fast_to; x++) {
            int32_t xs_int = (xs_ups + ((xs_step * x) >> 8)) >> 8;
            int32_t ys_int = (ys_ups + ((ys_step * x) >> 8)) >> 8;
            cbuf[x] = lv_color_swap_16(*((const uint16_t *)(src + ys_int * src_stride) + xs_int));
            abuf[x] = src_has_a8 ? src_alpha[ys_int * alpha_stride + xs_int] : 0xFF;
        }
    }

    rgb565a8_swapped_row_checked(src, src_w, src_h, src_stride, src_alpha, alpha_stride, xs_ups, ys_ups, xs_step, ys_step,
                                 fast_to, x_end, xs_clamp_ups, cbuf, abuf, src_has_a8, aa);
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
    abuf -= x_start;

    int32_t x;
    for(x = x_start; x < x_end; x++) {
        xs_ups = xs_ups_start + ((xs_step * x) >> 8);
        ys_ups = ys_ups_start + ((ys_step * x) >> 8);
        if(xs_ups > xs_clamp_ups) xs_ups = xs_clamp_ups;

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        /*Fully out of the image*/
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            abuf[x] = 0x00;
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
            xs_fract = (0x7F - xs_fract) * 2;
        }
        else {
            x_next = 1;
            xs_fract = (xs_fract - 0x80) * 2;
        }
        if(ys_fract < 0x80) {
            y_next = -1;
            ys_fract = (0x7F - ys_fract) * 2;
        }
        else {
            y_next = 1;
            ys_fract = (ys_fract - 0x80) * 2;
        }

        const uint8_t * src_tmp = src;
        src_tmp += ys_int * src_stride + xs_int;
        abuf[x] = src_tmp[0];

        if(aa &&
           xs_int + x_next >= 0 &&
           xs_int + x_next <= src_w - 1 &&
           ys_int + y_next >= 0 &&
           ys_int + y_next <= src_h - 1) {

            lv_opa_t a_ver = src_tmp[x_next];
            lv_opa_t a_hor = src_tmp[y_next * src_stride];

            if(a_ver != abuf[x]) a_ver = ((a_ver * ys_fract) + (abuf[x] * (0x100 - ys_fract))) >> 8;
            if(a_hor != abuf[x]) a_hor = ((a_hor * xs_fract) + (abuf[x] * (0x100 - xs_fract))) >> 8;
            abuf[x] = (a_ver + a_hor) >> 1;
        }
        else {
            /*Partially out of the image*/
            if((xs_int == 0 && x_next < 0) || (xs_int == src_w - 1 && x_next > 0))  {
                abuf[x] = (src_tmp[0] * (0xFF - xs_fract)) >> 8;
            }
            else if((ys_int == 0 && y_next < 0) || (ys_int == src_h - 1 && y_next > 0))  {
                abuf[x] = (src_tmp[0] * (0xFF - ys_fract)) >> 8;
            }
        }
    }
}

#endif

#if LV_DRAW_SW_SUPPORT_L8 || LV_DRAW_SW_SUPPORT_AL88

static void transform_al88(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                           int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                           int32_t x_start, int32_t x_end, int32_t xs_clamp_ups,
                           uint8_t * cbuf, uint8_t * abuf, bool src_has_a8, bool aa)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;
    cbuf -= x_start;
    abuf -= x_start;

    int32_t x;
    for(x = x_start; x < x_end; x++) {
        xs_ups = xs_ups_start + ((xs_step * x) >> 8);
        ys_ups = ys_ups_start + ((ys_step * x) >> 8);
        if(xs_ups > xs_clamp_ups) xs_ups = xs_clamp_ups;

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        /*Fully out of the image*/
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            cbuf[x] = 0x00;
            abuf[x] = 0x00;
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
            xs_fract = (0x7F - xs_fract) * 2;
        }
        else {
            x_next = 1;
            xs_fract = (xs_fract - 0x80) * 2;
        }
        if(ys_fract < 0x80) {
            y_next = -1;
            ys_fract = (0x7F - ys_fract) * 2;
        }
        else {
            y_next = 1;
            ys_fract = (ys_fract - 0x80) * 2;
        }

        if(src_has_a8) {
            const lv_color16a_t * src_tmp = (const lv_color16a_t *)(src + ys_int * src_stride + xs_int * 2);
            cbuf[x] = src_tmp[0].lumi;
            abuf[x] = src_tmp[0].alpha;

            if(aa &&
               xs_int + x_next >= 0 &&
               xs_int + x_next <= src_w - 1 &&
               ys_int + y_next >= 0 &&
               ys_int + y_next <= src_h - 1) {

                lv_color16a_t px_hor = src_tmp[x_next];
                lv_color16a_t px_ver = *(const lv_color16a_t *)((uint8_t *)src_tmp + (y_next * src_stride));

                /* Interpolate luminance */
                uint8_t l_ver = px_ver.lumi;
                uint8_t l_hor = px_hor.lumi;
                if(l_ver != cbuf[x]) l_ver = ((l_ver * ys_fract) + (cbuf[x] * (0x100 - ys_fract))) >> 8;
                if(l_hor != cbuf[x]) l_hor = ((l_hor * xs_fract) + (cbuf[x] * (0x100 - xs_fract))) >> 8;
                cbuf[x] = (l_ver + l_hor) >> 1;

                /* Interpolate alpha */
                uint8_t a_ver = px_ver.alpha;
                uint8_t a_hor = px_hor.alpha;
                if(a_ver != abuf[x]) a_ver = ((a_ver * ys_fract) + (abuf[x] * (0x100 - ys_fract))) >> 8;
                if(a_hor != abuf[x]) a_hor = ((a_hor * xs_fract) + (abuf[x] * (0x100 - xs_fract))) >> 8;
                abuf[x] = (a_ver + a_hor) >> 1;
            }
            else {
                /*Partially out of the image*/
                if((xs_int == 0 && x_next < 0) || (xs_int == src_w - 1 && x_next > 0)) {
                    abuf[x] = (abuf[x] * (0xFF - xs_fract)) >> 8;
                }
                else if((ys_int == 0 && y_next < 0) || (ys_int == src_h - 1 && y_next > 0)) {
                    abuf[x] = (abuf[x] * (0xFF - ys_fract)) >> 8;
                }
            }
        }
        else {
            /* L8 format: 1 byte per pixel, no separate alpha channel */
            const uint8_t * src_tmp = src + ys_int * src_stride + xs_int;
            cbuf[x] = src_tmp[0];
            abuf[x] = 0xff;

            if(aa &&
               xs_int + x_next >= 0 &&
               xs_int + x_next <= src_w - 1 &&
               ys_int + y_next >= 0 &&
               ys_int + y_next <= src_h - 1) {

                uint8_t l_ver = src_tmp[y_next * src_stride];
                uint8_t l_hor = src_tmp[x_next];

                if(l_ver != cbuf[x]) l_ver = ((l_ver * ys_fract) + (cbuf[x] * (0x100 - ys_fract))) >> 8;
                if(l_hor != cbuf[x]) l_hor = ((l_hor * xs_fract) + (cbuf[x] * (0x100 - xs_fract))) >> 8;
                cbuf[x] = (l_ver + l_hor) >> 1;
            }
            else {
                /*Partially out of the image - reduce alpha for edge pixels*/
                if((xs_int == 0 && x_next < 0) || (xs_int == src_w - 1 && x_next > 0)) {
                    abuf[x] = (0xff * (0xFF - xs_fract)) >> 8;
                }
                else if((ys_int == 0 && y_next < 0) || (ys_int == src_h - 1 && y_next > 0)) {
                    abuf[x] = (0xff * (0xFF - ys_fract)) >> 8;
                }
            }
        }
    }
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
    int64_t lo_n = ((int64_t)(lo - base)) << 8;
    int64_t hi_n = (((int64_t)(hi - base)) << 8) + 255;

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
