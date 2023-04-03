/**
 * @file lv_draw_sw_tranform.c
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
#include LV_COLOR_EXTERN_INCLUDE

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
    int32_t zoom;
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

static void argb8888_no_aa(const uint8_t * src, lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                           int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                           int32_t x_end, lv_color_t * cbuf, uint8_t * abuf);

static void rgb_no_aa(const uint8_t * src, lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                      int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                      int32_t x_end, lv_color_t * cbuf, uint8_t * abuf);

static void rgb_no_aa_chroma_key(const uint8_t * src, lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                                 int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                                 int32_t x_end, lv_color_t * cbuf, uint8_t * abuf, lv_color_t chroma_key_color);

#if LV_COLOR_DEPTH == 16
static void rgb565a8_no_aa(const uint8_t * src, lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                           int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                           int32_t x_end, lv_color_t * cbuf, uint8_t * abuf);
#endif

static void rgb_aa(const uint8_t * src, lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                   int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                   int32_t x_end, lv_color_t * cbuf, uint8_t * abuf);


static void argb8888_aa(const uint8_t * src, lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                        int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                        int32_t x_end, lv_color_t * cbuf, uint8_t * abuf);

static void a8_aa(const uint8_t * src, lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                  int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                  int32_t x_end, uint8_t * abuf);


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
                          lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                          const lv_draw_img_dsc_t * draw_dsc, const lv_draw_img_sup_t * sup, lv_color_format_t cf, lv_color_t * cbuf,
                          lv_opa_t * abuf)
{

    LV_UNUSED(draw_unit);

    point_transform_dsc_t tr_dsc;
    tr_dsc.angle = -draw_dsc->angle;
    tr_dsc.zoom = (256 * 256) / draw_dsc->zoom;
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

    lv_coord_t dest_w = lv_area_get_width(dest_area);
    lv_coord_t dest_h = lv_area_get_height(dest_area);
    lv_coord_t y;
    for(y = 0; y < dest_h; y++) {
        int32_t xs1_ups, ys1_ups, xs2_ups, ys2_ups;

        transform_point_upscaled(&tr_dsc, dest_area->x1, dest_area->y1 + y, &xs1_ups, &ys1_ups);
        transform_point_upscaled(&tr_dsc, dest_area->x2, dest_area->y1 + y, &xs2_ups, &ys2_ups);

        int32_t xs_diff = xs2_ups - xs1_ups;
        int32_t ys_diff = ys2_ups - ys1_ups;
        int32_t xs_step_256 = 0;
        int32_t ys_step_256 = 0;
        if(dest_w > 1) {
            xs_step_256 = (256 * xs_diff) / (dest_w - 1);
            ys_step_256 = (256 * ys_diff) / (dest_w - 1);
        }

        int32_t xs_ups = xs1_ups + 0x80;
        int32_t ys_ups = ys1_ups + 0x80;

        if(draw_dsc->antialias == 0) {
            switch(cf) {
                case LV_COLOR_FORMAT_NATIVE:
                    if(sup->chroma_keyed) {
                        rgb_no_aa_chroma_key(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w, cbuf, abuf,
                                             sup->chroma_key_color);
                    }
                    else {
                        rgb_no_aa(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w, cbuf, abuf);
                    }
                    break;
                case LV_COLOR_FORMAT_ARGB8888:
                    argb8888_no_aa(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w, cbuf, abuf);
                    break;

#if LV_COLOR_DEPTH == 16
                case LV_COLOR_FORMAT_RGB565A8:
                    rgb565a8_no_aa(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w, cbuf, abuf);
                    break;
#endif
                default:
                    break;
            }
        }
        else {
            switch(cf) {
                case LV_COLOR_FORMAT_NATIVE:
                    rgb_aa(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w, cbuf, abuf);
                    break;
                case LV_COLOR_FORMAT_A8:
                    a8_aa(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w, abuf);
                    break;
                case LV_COLOR_FORMAT_ARGB8888:
                    argb8888_aa(src_buf, src_w, src_h, src_stride, xs_ups, ys_ups, xs_step_256, ys_step_256, dest_w, cbuf, abuf);
                    break;
                default:
                    break;
            }
        }

        cbuf += dest_w;
        abuf += dest_w;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void rgb_no_aa(const uint8_t * src, lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                      int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                      int32_t x_end, lv_color_t * cbuf, uint8_t * abuf)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;

    lv_memset(abuf, 0xff, x_end);

    lv_coord_t x;
    for(x = 0; x < x_end; x++) {
        xs_ups = xs_ups_start + ((xs_step * x) >> 8);
        ys_ups = ys_ups_start + ((ys_step * x) >> 8);

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            abuf[x] = 0x00;
        }
        else {
#if LV_COLOR_DEPTH == 8
            const uint8_t * src_tmp = src;
            src_tmp += ys_int * src_stride + xs_int;
            lv_color_set_int(&cbuf[x], src_tmp[0]);
#elif LV_COLOR_DEPTH == 16
            const lv_color_t * src_tmp = (const lv_color_t *)src;
            src_tmp += ys_int * src_stride + xs_int;
            cbuf[x] = *src_tmp;
#elif LV_COLOR_DEPTH == 24
            const lv_color_t * src_tmp = (const lv_color_t *)src;
            src_tmp += ys_int * src_stride + xs_int;
            cbuf[x].blue = src_tmp->blue ;
            cbuf[x].green = src_tmp->green;
            cbuf[x].red = src_tmp->red;
#elif LV_COLOR_DEPTH == 32
            const uint8_t * src_tmp = src;
            src_tmp += (ys_int * src_stride * sizeof(lv_color_t)) + xs_int * sizeof(lv_color_t);
            cbuf[x] = *((lv_color_t *)src_tmp);
#endif
        }
    }
}

static void rgb_no_aa_chroma_key(const uint8_t * src, lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                                 int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                                 int32_t x_end, lv_color_t * cbuf, uint8_t * abuf, lv_color_t chroma_key_color)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;

    lv_memset(abuf, 0xff, x_end);

    lv_coord_t x;
    for(x = 0; x < x_end; x++) {
        xs_ups = xs_ups_start + ((xs_step * x) >> 8);
        ys_ups = ys_ups_start + ((ys_step * x) >> 8);

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            abuf[x] = 0x00;
        }
        else if(lv_color_eq(cbuf[x], chroma_key_color)) {
            abuf[x] = 0x00;
        }
        else {
#if LV_COLOR_DEPTH == 8
            const uint8_t * src_tmp = src;
            src_tmp += ys_int * src_stride + xs_int;
            lv_color_set_int(&cbuf[x], src_tmp[0]);
#elif LV_COLOR_DEPTH == 16
            const lv_color_t * src_tmp = (const lv_color_t *)src;
            src_tmp += ys_int * src_stride + xs_int;
            cbuf[x] = *src_tmp;
#elif LV_COLOR_DEPTH == 24
            const lv_color_t * src_tmp = (const lv_color_t *)src;
            src_tmp += ys_int * src_stride + xs_int;
            cbuf[x].blue = src_tmp->blue ;
            cbuf[x].green = src_tmp->green;
            cbuf[x].red = src_tmp->red;
#elif LV_COLOR_DEPTH == 32
            const uint8_t * src_tmp = src;
            src_tmp += (ys_int * src_stride * sizeof(lv_color_t)) + xs_int * sizeof(lv_color_t);
            cbuf[x] = *((lv_color_t *)src_tmp);
#endif
        }
    }
}

static void argb8888_no_aa(const uint8_t * src, lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                           int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                           int32_t x_end, lv_color_t * cbuf, uint8_t * abuf)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;

    lv_coord_t x;
    for(x = 0; x < x_end; x++) {
        xs_ups = xs_ups_start + ((xs_step * x) >> 8);
        ys_ups = ys_ups_start + ((ys_step * x) >> 8);

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            abuf[x] = 0;
        }
        else {
            const lv_color32_t * src32 = (const lv_color32_t *)src;
            src32 += (ys_int * src_stride) + xs_int;

            cbuf[x] = lv_color_from_xrgb8888(*src32);
            abuf[x] = src32->alpha;
        }
    }
}

#if LV_COLOR_DEPTH == 16
static void rgb565a8_no_aa(const uint8_t * src, lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                           int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                           int32_t x_end, lv_color_t * cbuf, uint8_t * abuf)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;

    lv_coord_t x;
    for(x = 0; x < x_end; x++) {
        xs_ups = xs_ups_start + ((xs_step * x) >> 8);
        ys_ups = ys_ups_start + ((ys_step * x) >> 8);

        int32_t xs_int = xs_ups >> 8;
        int32_t ys_int = ys_ups >> 8;
        if(xs_int < 0 || xs_int >= src_w || ys_int < 0 || ys_int >= src_h) {
            abuf[x] = 0;
        }
        else {
            const lv_color_t * src_tmp = (const lv_color_t *)src;
            src_tmp += ys_int * src_stride + xs_int;
            cbuf[x] = *src_tmp;

            const lv_opa_t * a_tmp = src + src_stride * src_h * sizeof(lv_color_t);
            a_tmp += ys_int * src_stride + xs_int;
            abuf[x] = *a_tmp;
        }
    }
}
#endif


static void rgb_aa(const uint8_t * src, lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                   int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                   int32_t x_end, lv_color_t * cbuf, uint8_t * abuf)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;

    lv_coord_t x;
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

        const lv_color_t * src_tmp = (const lv_color_t *) src;
        src_tmp += (ys_int * src_stride) + xs_int;


        if(xs_int + x_next >= 0 &&
           xs_int + x_next <= src_w - 1 &&
           ys_int + y_next >= 0 &&
           ys_int + y_next <= src_h - 1) {

            lv_color_t px_base = src_tmp[0];
            lv_color_t px_hor = src_tmp[x_next];
            lv_color_t px_ver = src_tmp[y_next * src_stride];

            abuf[x] = 0xff;

            if(lv_color_eq(px_base, px_ver) && lv_color_eq(px_base, px_hor)) {
                cbuf[x] = px_base;
            }
            else {
                px_ver = LV_COLOR_MIX(px_ver, px_base, ys_fract);
                px_hor = LV_COLOR_MIX(px_hor, px_base, xs_fract);
                cbuf[x] = LV_COLOR_MIX(px_hor, px_ver, LV_OPA_50);
            }
        }
        /*Partially out of the image*/
        else {
            lv_opa_t a = 0xff;

            if((xs_int == 0 && x_next < 0) || (xs_int == src_w - 1 && x_next > 0))  {
                abuf[x] = (a * (0xFF - xs_fract)) >> 8;
            }
            else if((ys_int == 0 && y_next < 0) || (ys_int == src_h - 1 && y_next > 0))  {
                abuf[x] = (a * (0xFF - ys_fract)) >> 8;
            }
            else {
                abuf[x] = 0x00;
            }
        }
    }
}

static void argb8888_aa(const uint8_t * src, lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                        int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                        int32_t x_end, lv_color_t * cbuf, uint8_t * abuf)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;

    lv_coord_t x;
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

        const lv_color32_t * src_tmp = (const lv_color32_t *)src;
        src_tmp += (ys_int * src_stride) + xs_int;


        if(xs_int + x_next >= 0 &&
           xs_int + x_next <= src_w - 1 &&
           ys_int + y_next >= 0 &&
           ys_int + y_next <= src_h - 1) {

            lv_color32_t px_base = src_tmp[0];
            lv_color32_t px_hor = src_tmp[x_next];
            lv_color32_t px_ver = src_tmp[y_next * src_stride];

            if(px_ver.alpha != px_base.alpha) px_ver.alpha = ((px_ver.alpha * ys_fract) + (px_base.alpha *
                                                                                               (0x100 - ys_fract))) >> 8;
            if(px_hor.alpha != px_base.alpha) px_hor.alpha = ((px_hor.alpha * xs_fract) + (px_base.alpha *
                                                                                               (0x100 - xs_fract))) >> 8;
            abuf[x] = (px_ver.alpha + px_hor.alpha) >> 1;

            if(abuf[x] == 0x00) continue;



            if(lv_color32_eq(px_base, px_ver) && lv_color32_eq(px_base, px_hor)) {
                cbuf[x] = lv_color_from_xrgb8888(px_base);
            }
            else {
                lv_color_t v = lv_color_from_xrgb8888(px_ver);
                lv_color_t h = lv_color_from_xrgb8888(px_hor);
                lv_color_t b = lv_color_from_xrgb8888(px_base);
                v = LV_COLOR_MIX(v, b, ys_fract);
                h = LV_COLOR_MIX(h, b, xs_fract);
                cbuf[x] = LV_COLOR_MIX(h, v, LV_OPA_50);
            }
        }
        /*Partially out of the image*/
        else {
            cbuf[x] = lv_color_from_xrgb8888(*src_tmp);
            lv_opa_t a = src_tmp->alpha;

            if((xs_int == 0 && x_next < 0) || (xs_int == src_w - 1 && x_next > 0))  {
                abuf[x] = (a * (0xFF - xs_fract)) >> 8;
            }
            else if((ys_int == 0 && y_next < 0) || (ys_int == src_h - 1 && y_next > 0))  {
                abuf[x] = (a * (0xFF - ys_fract)) >> 8;
            }
            else {
                abuf[x] = 0x00;
            }
        }
    }
}

static void a8_aa(const uint8_t * src, lv_coord_t src_w, lv_coord_t src_h, lv_coord_t src_stride,
                  int32_t xs_ups, int32_t ys_ups, int32_t xs_step, int32_t ys_step,
                  int32_t x_end, uint8_t * abuf)
{
    int32_t xs_ups_start = xs_ups;
    int32_t ys_ups_start = ys_ups;

    lv_coord_t x;
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

        if(xs_int + x_next >= 0 &&
           xs_int + x_next <= src_w - 1 &&
           ys_int + y_next >= 0 &&
           ys_int + y_next <= src_h - 1) {

            lv_opa_t a_base = src_tmp[0];
            lv_opa_t a_ver = src_tmp[x_next];
            lv_opa_t a_hor = src_tmp[y_next * src_stride];

            if(a_ver != a_base) a_ver = ((a_ver * ys_fract) + (a_base * (0x100 - ys_fract))) >> 8;
            if(a_hor != a_base) a_hor = ((a_hor * xs_fract) + (a_base * (0x100 - xs_fract))) >> 8;
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
            else {
                abuf[x] = 0x00;
            }
        }
    }
}


static void transform_point_upscaled(point_transform_dsc_t * t, int32_t xin, int32_t yin, int32_t * xout,
                                     int32_t * yout)
{
    if(t->angle == 0 && t->zoom == LV_ZOOM_NONE) {
        *xout = xin * 256;
        *yout = yin * 256;
        return;
    }

    xin -= t->pivot.x;
    yin -= t->pivot.y;

    if(t->angle == 0) {
        *xout = ((int32_t)(xin * t->zoom)) + (t->pivot_x_256);
        *yout = ((int32_t)(yin * t->zoom)) + (t->pivot_y_256);
    }
    else if(t->zoom == LV_ZOOM_NONE) {
        *xout = ((t->cosma * xin - t->sinma * yin) >> 2) + (t->pivot_x_256);
        *yout = ((t->sinma * xin + t->cosma * yin) >> 2) + (t->pivot_y_256);
    }
    else {
        *xout = (((t->cosma * xin - t->sinma * yin) * t->zoom) >> 10) + (t->pivot_x_256);
        *yout = (((t->sinma * xin + t->cosma * yin) * t->zoom) >> 10) + (t->pivot_y_256);
    }
}

#endif /*LV_USE_DRAW_SW*/
