/**
 * @file lv_draw_sw_transform.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW

#include "../../misc/lv_assert.h"
#include "../../misc/lv_area.h"
#include "../../core/lv_refr.h"
#include "../../misc/lv_color.h"
#include "../../stdlib/lv_string.h"

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
 * Transform a point with 1/256 precision (the output coordinates are upscaled by 256)
 * @param t         pointer to n initialized `point_transform_dsc_t` structure
 * @param xin       X coordinate to rotate
 * @param yin       Y coordinate to rotate
 * @param xout      upscaled, transformed X
 * @param yout      upscaled, transformed Y
 */
static void transform_point_upscaled(point_transform_dsc_t * t, int32_t xin, int32_t yin, int32_t * xout,
                                     int32_t * yout);

#if LV_DRAW_SW_SUPPORT_RGB888
static void transform_rgb888(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                             int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                             int32_t x_end, uint8_t * dest_buf, bool aa, uint32_t px_size);
#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888
static void transform_argb8888(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                               int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                               int32_t x_end, uint8_t * dest_buf, bool aa);
#endif

#if LV_DRAW_SW_SUPPORT_RGB565A8
static void transform_rgb565a8(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                               int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                               int32_t x_end, uint16_t * cbuf, uint8_t * abuf, bool src_has_a8, bool aa);
#endif

#if LV_DRAW_SW_SUPPORT_A8
static void transform_a8(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                         int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                         int32_t x_end, uint8_t * abuf, bool aa);
#endif

#if LV_DRAW_SW_SUPPORT_L8
static void transform_l8_to_al88(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                                 int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                                 int32_t x_end, uint8_t * abuf, bool aa);

static void transform_l8_to_argb8888(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                                     int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                                     int32_t x_end, uint8_t * abuf, bool aa);
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

void lv_draw_sw_transform(lv_draw_unit_t * draw_unit, const lv_area_t * dest_area, const void * src_buf,
                          int32_t src_w, int32_t src_h, int32_t src_stride,
                          const lv_draw_image_dsc_t * draw_dsc, const lv_draw_image_sup_t * sup, lv_color_format_t src_cf, void * dest_buf)
{
    LV_UNUSED(draw_unit);
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
    if(src_cf == LV_COLOR_FORMAT_L8) {
        dest_stride = dest_w * ((draw_dsc->recolor_opa >= LV_OPA_MIN) ? 4 : 2);
    }
    else if(src_cf == LV_COLOR_FORMAT_RGB888) {
        dest_stride = dest_w * lv_color_format_get_size(LV_COLOR_FORMAT_ARGB8888);
    }
    else if((src_cf == LV_COLOR_FORMAT_RGB565A8) || (src_cf == LV_COLOR_FORMAT_L8)) {
        dest_stride = dest_w * 2;
    }
    else {
        dest_stride = dest_w * lv_color_format_get_size(src_cf);
    }

    uint8_t * alpha_buf;
    if(src_cf == LV_COLOR_FORMAT_RGB565 || src_cf == LV_COLOR_FORMAT_RGB565A8) {
        alpha_buf = dest_buf;
        alpha_buf += dest_stride * dest_h;
    }
    else {
        alpha_buf = NULL;
    }

    bool aa = (bool) draw_dsc->antialias;
    bool is_rotated = draw_dsc->rotation;

    int32_t xs_ups = 0, ys_ups = 0, ys_ups_start = 0, ys_step_256_original = 0;
    int32_t xs_step_256 = 0, ys_step_256 = 0;

    /*When some of the color formats are disabled, these variables could be unused, avoid warning here*/
    LV_UNUSED(aa);
    LV_UNUSED(xs_ups);
    LV_UNUSED(ys_ups);
    LV_UNUSED(xs_step_256);
    LV_UNUSED(ys_step_256);

    /*If scaled only make some simplification to avoid rounding errors.
     *For example if there is a 100x100 image zoomed to 300%
     *The destination area in X will be x1=0; x2=299
     *When the step is calculated below it will think that stepping
     *1/3 pixels on the original image will result in 300% zoom.
     *However this way the last pixel will be on the 99.67 coordinate.
     *As it's larger than 99.5 LVGL will start to mix the next coordinate
     *which is out of the image, so will make the pixel more transparent.
     *To avoid it in case of scale only limit the coordinates to the 0..297 range,
     *that is to 0..(src_w-1)*zoom */
    if(is_rotated == false) {
        int32_t xs1_ups, ys1_ups, xs2_ups, ys2_ups;

        int32_t x_max = (((src_w - 1 - draw_dsc->pivot.x) * draw_dsc->scale_x) >> 8) + draw_dsc->pivot.x;
        int32_t y_max = (((src_h - 1 - draw_dsc->pivot.y) * draw_dsc->scale_y) >> 8) + draw_dsc->pivot.y;

        lv_area_t dest_area_limited;
        dest_area_limited.x1 = dest_area->x1 > x_max ? x_max : dest_area->x1;
        dest_area_limited.x2 = dest_area->x2 > x_max ? x_max : dest_area->x2;
        dest_area_limited.y1 = dest_area->y1 > y_max ? y_max : dest_area->y1;
        dest_area_limited.y2 = dest_area->y2 > y_max ? y_max : dest_area->y2;

        transform_point_upscaled(&tr_dsc, dest_area_limited.x1, dest_area_limited.y1, &xs1_ups, &ys1_ups);
        transform_point_upscaled(&tr_dsc, dest_area_limited.x2, dest_area_limited.y2, &xs2_ups, &ys2_ups);

        int32_t xs_diff = xs2_ups - xs1_ups;
        int32_t ys_diff = ys2_ups - ys1_ups;
        xs_step_256 = 0;
        ys_step_256_original = 0;
        if(dest_w > 1) {
            xs_step_256 = (256 * xs_diff) / (dest_w - 1);
        }
        if(dest_h > 1) {
            ys_step_256_original = (256 * ys_diff) / (dest_h - 1);
        }

        xs_ups = xs1_ups + 0x80;
        ys_ups_start = ys1_ups + 0x80;
    }

    int32_t y;
    for(y = 0; y < dest_h; y++) {
        if(is_rotated == false) {
            ys_ups = ys_ups_start + ((ys_step_256_original * y) >> 8);
            ys_step_256 = 0;
        }
        else {
            int32_t xs1_ups, ys1_ups, xs2_ups, ys2_ups;
            transform_point_upscaled(&tr_dsc, dest_area->x1, dest_area->y1 + y, &xs1_ups, &ys1_ups);
            transform_point_upscaled(&tr_dsc, dest_area->x2, dest_area->y1 + y, &xs2_ups, &ys2_ups);

            int32_t xs_diff = xs2_ups - xs1_ups;
            int32_t ys_diff = ys2_ups - ys1_ups;
            xs_step_256 = 0;
            ys_step_256 = 0;
            if(dest_w > 1) {
                xs_step_256 = (256 * xs_diff) / (dest_w - 1);
                ys_step_256 = (256 * ys_diff) / (dest_w - 1);
            }

            xs_ups = xs1_ups + 0x80;
            ys_ups = ys1_ups + 0x80;
        }

        switch(src_cf) {
#if LV_DRAW_SW_SUPPORT_XRGB8888
            case LV_COLOR_FORMAT_XRGB8888:
                transform_rgb888(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w, dest_buf, aa,
                                 4);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB888
            case LV_COLOR_FORMAT_RGB888:
                transform_rgb888(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w, dest_buf, aa,
                                 3);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_A8
            case LV_COLOR_FORMAT_A8:
                transform_a8(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w, dest_buf, aa);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888
            case LV_COLOR_FORMAT_ARGB8888:
                transform_argb8888(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w, dest_buf,
                                   aa);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565 && LV_DRAW_SW_SUPPORT_RGB565A8
            case LV_COLOR_FORMAT_RGB565:
                transform_rgb565a8(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w, dest_buf,
                                   alpha_buf, false, aa);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565A8
            case LV_COLOR_FORMAT_RGB565A8:
                transform_rgb565a8(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w,
                                   (uint16_t *)dest_buf,
                                   alpha_buf, true, aa);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_L8
            case LV_COLOR_FORMAT_L8:
                if(draw_dsc->recolor_opa >= LV_OPA_MIN)
                    transform_l8_to_argb8888(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w, dest_buf,
                                             aa);
                else
                    transform_l8_to_al88(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w, dest_buf, aa);
                break;
#endif
            default:
                break;
        }

        dest_buf = (uint8_t *)dest_buf + dest_stride;
        if(alpha_buf) alpha_buf += dest_stride_a8;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_DRAW_SW_SUPPORT_RGB888

static void transform_rgb888(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                             int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                             int32_t x_end, uint8_t * dest_buf, bool aa, uint32_t px_size)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;
    lv_color32_t * dest_c32 = (lv_color32_t *) dest_buf;

    int32_t x;
    for(x = 0; x < x_end; x++) {
        xs_ups = xs_ups_start + ((xs_step * x) >> 8);
        ys_ups = ys_ups_start + ((ys_step * x) >> 8);

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
                dest_c32[x] = lv_color_mix32(px_ver, dest_c32[x]);
            }

            if(!lv_color32_eq(dest_c32[x], px_hor)) {
                px_hor.alpha = xs_fract;
                dest_c32[x] = lv_color_mix32(px_hor, dest_c32[x]);
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

#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888

static void transform_argb8888(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                               int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                               int32_t x_end, uint8_t * dest_buf, bool aa)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;
    lv_color32_t * dest_c32 = (lv_color32_t *) dest_buf;

    int32_t x;
    for(x = 0; x < x_end; x++) {
        xs_ups = xs_ups_start + ((xs_step * x) >> 8);
        ys_ups = ys_ups_start + ((ys_step * x) >> 8);

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        /*Fully out of the image*/
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            ((uint32_t *)dest_buf)[x] = 0x00000000;
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
                dest_c32[x] = lv_color_mix32(px_ver, dest_c32[x]);
            }

            if(px_hor.alpha == 0) {
                dest_c32[x].alpha = (dest_c32[x].alpha * (0xFF - xs_fract)) >> 8;
            }
            else if(!lv_color32_eq(dest_c32[x], px_hor)) {
                if(dest_c32[x].alpha) dest_c32[x].alpha = ((px_hor.alpha * xs_fract) + (dest_c32[x].alpha * (0xFF - xs_fract))) >> 8;
                px_hor.alpha = xs_fract;
                dest_c32[x] = lv_color_mix32(px_hor, dest_c32[x]);
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

#endif

#if LV_DRAW_SW_SUPPORT_RGB565A8

static void transform_rgb565a8(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                               int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                               int32_t x_end, uint16_t * cbuf, uint8_t * abuf, bool src_has_a8, bool aa)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;

    const lv_opa_t * src_alpha = src + src_stride * src_h;

    /*Must be signed type, because we would use negative array index calculated from stride*/
    int32_t alpha_stride = src_stride / 2; /*alpha map stride is always half of RGB map stride*/

    int32_t x;
    for(x = 0; x < x_end; x++) {
        xs_ups = xs_ups_start + ((xs_step * x) >> 8);
        ys_ups = ys_ups_start + ((ys_step * x) >> 8);

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
                uint16_t v = lv_color_16_16_mix(px_ver, cbuf[x], ys_fract);
                uint16_t h = lv_color_16_16_mix(px_hor, cbuf[x], xs_fract);
                cbuf[x] = lv_color_16_16_mix(h, v, LV_OPA_50);
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

#endif

#if LV_DRAW_SW_SUPPORT_A8

static void transform_a8(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                         int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                         int32_t x_end, uint8_t * abuf, bool aa)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;

    int32_t x;
    for(x = 0; x < x_end; x++) {
        xs_ups = xs_ups_start + ((xs_step * x) >> 8);
        ys_ups = ys_ups_start + ((ys_step * x) >> 8);

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

#if LV_DRAW_SW_SUPPORT_L8

#if LV_DRAW_SW_SUPPORT_AL88

/* L8 will be transformed into an AL88 buffer, because it will not be recolored */
static void transform_l8_to_al88(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                                 int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                                 int32_t x_end, uint8_t * dest_buf, bool aa)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;
    lv_color16a_t * dest_al88 = (lv_color16a_t *)dest_buf;

    int32_t x;
    for(x = 0; x < x_end; x++) {
        xs_ups = xs_ups_start + ((xs_step * x) >> 8);
        ys_ups = ys_ups_start + ((ys_step * x) >> 8);

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        /*Fully out of the image*/
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            dest_al88[x].lumi = 0x00;
            dest_al88[x].alpha = 0x00;
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
        dest_al88[x].lumi = src_tmp[0];
        dest_al88[x].alpha = 255;
        if(aa &&
           xs_int + x_next >= 0 &&
           xs_int + x_next <= src_w - 1 &&
           ys_int + y_next >= 0 &&
           ys_int + y_next <= src_h - 1) {

            lv_opa_t a_ver = src_tmp[x_next];
            lv_opa_t a_hor = src_tmp[y_next * src_stride];

            if(a_ver != dest_al88[x].lumi) a_ver = ((a_ver * ys_fract) + (dest_al88[x].lumi * (0x100 - ys_fract))) >> 8;
            if(a_hor != dest_al88[x].lumi) a_hor = ((a_hor * xs_fract) + (dest_al88[x].lumi * (0x100 - xs_fract))) >> 8;
            dest_al88[x].lumi = (a_ver + a_hor) >> 1;
        }
        else {
            /*Partially out of the image*/
            if((xs_int == 0 && x_next < 0) || (xs_int == src_w - 1 && x_next > 0)) {
                dest_al88[x].alpha = (src_tmp[0] * (0xFF - xs_fract)) >> 8;
            }
            else if((ys_int == 0 && y_next < 0) || (ys_int == src_h - 1 && y_next > 0)) {
                dest_al88[x].alpha = (src_tmp[0] * (0xFF - ys_fract)) >> 8;
            }
        }
    }
}

#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888

/* L8 has to be transformed into an ARGB8888 buffer, because it will be recolored as well */
static void transform_l8_to_argb8888(const uint8_t * src, int32_t src_w, int32_t src_h, int32_t src_stride,
                                     int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                                     int32_t x_end, uint8_t * dest_buf, bool aa)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;
    lv_color32_t * dest_c32 = (lv_color32_t *)dest_buf;

    int32_t x;
    for(x = 0; x < x_end; x++) {
        xs_ups = xs_ups_start + ((xs_step * x) >> 8);
        ys_ups = ys_ups_start + ((ys_step * x) >> 8);

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;

        /*Fully out of the image*/
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            *((uint32_t *)&dest_c32[x]) = 0L;
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
        dest_c32[x].red = dest_c32[x].green = dest_c32[x].blue = src_tmp[0];
        dest_c32[x].alpha = 255;
        if(aa &&
           xs_int + x_next >= 0 &&
           xs_int + x_next <= src_w - 1 &&
           ys_int + y_next >= 0 &&
           ys_int + y_next <= src_h - 1) {

            lv_opa_t a_ver = src_tmp[x_next];
            lv_opa_t a_hor = src_tmp[y_next * src_stride];

            if(a_ver != src_tmp[0]) a_ver = ((a_ver * ys_fract) + (src_tmp[0] * (0x100 - ys_fract))) >> 8;
            if(a_hor != src_tmp[0]) a_hor = ((a_hor * xs_fract) + (src_tmp[0] * (0x100 - xs_fract))) >> 8;
            dest_c32[x].red = dest_c32[x].green = dest_c32[x].blue = (a_ver + a_hor) >> 1;
        }
        else {
            /*Partially out of the image*/
            if((xs_int == 0 && x_next < 0) || (xs_int == src_w - 1 && x_next > 0)) {
                dest_c32[x].alpha = (src_tmp[0] * (0xFF - xs_fract)) >> 8;
            }
            else if((ys_int == 0 && y_next < 0) || (ys_int == src_h - 1 && y_next > 0)) {
                dest_c32[x].alpha = (src_tmp[0] * (0xFF - ys_fract)) >> 8;
            }
        }
    }
}

#endif

#endif

static void transform_point_upscaled(point_transform_dsc_t * t, int32_t xin, int32_t yin, int32_t * xout,
                                     int32_t * yout)
{
    if(t->angle == 0 && t->scale_x == LV_SCALE_NONE && t->scale_y == LV_SCALE_NONE) {
        *xout = xin * 256;
        *yout = yin * 256;
        return;
    }

    xin -= t->pivot.x;
    yin -= t->pivot.y;

    if(t->angle == 0) {
        *xout = ((int32_t)(xin * 256 * 256 / t->scale_x)) + (t->pivot_x_256);
        *yout = ((int32_t)(yin * 256 * 256 / t->scale_y)) + (t->pivot_y_256);
    }
    else if(t->scale_x == LV_SCALE_NONE && t->scale_y == LV_SCALE_NONE) {
        *xout = ((t->cosma * xin - t->sinma * yin) >> 2) + (t->pivot_x_256);
        *yout = ((t->sinma * xin + t->cosma * yin) >> 2) + (t->pivot_y_256);
    }
    else {
        *xout = (((t->cosma * xin - t->sinma * yin) * 256 / t->scale_x) >> 2) + (t->pivot_x_256);
        *yout = (((t->sinma * xin + t->cosma * yin) * 256 / t->scale_y) >> 2) + (t->pivot_y_256);
    }
}

#endif /*LV_USE_DRAW_SW*/
