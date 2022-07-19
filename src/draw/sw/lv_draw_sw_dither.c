/**
 * @file lv_draw_sw_dither.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_dither.h"
#if LV_USE_DRAW_SW

#include "lv_draw_sw_gradient.h"
#include "../../misc/lv_color.h"

/**********************
 *   STATIC FUNCTIONS
 **********************/


#if _DITHER_GRADIENT

LV_ATTRIBUTE_FAST_MEM void lv_dither_none(lv_grad_t * grad, lv_coord_t x, lv_coord_t y, lv_coord_t w)
{
    LV_UNUSED(x);
    LV_UNUSED(y);
    if(grad == NULL || grad->filled) return;
    for(lv_coord_t i = 0; i < w; i++) {
        grad->map[i] = lv_color_hex(grad->hmap[i].full);
    }
    grad->filled = 1;
}

static const uint8_t dither_ordered_threshold_matrix[8 * 8] = {
    0,  48, 12, 60,  3, 51, 15, 63,
    32, 16, 44, 28, 35, 19, 47, 31,
    8,  56,  4, 52, 11, 59,  7, 55,
    40, 24, 36, 20, 43, 27, 39, 23,
    2,  50, 14, 62,  1, 49, 13, 61,
    34, 18, 46, 30, 33, 17, 45, 29,
    10, 58,  6, 54,  9, 57,  5, 53,
    42, 26, 38, 22, 41, 25, 37, 21
}; /* Shift by 6 to normalize */


LV_ATTRIBUTE_FAST_MEM void lv_dither_ordered_hor(lv_grad_t * grad, lv_coord_t x, lv_coord_t y, lv_coord_t w)
{
    LV_UNUSED(x);
    /* For vertical dithering, the error is spread on the next column (and not next line).
       Since the renderer is scanline based, it's not obvious what could be used to perform the rendering efficiently.
       The algorithm below is based on few assumptions:
         1. An error diffusion algorithm (like Floyd Steinberg) here would be hard to implement since it means that a pixel on column n depends on the pixel on row n
         2. Instead an ordered dithering algorithm shift the value a bit, but the influence only spread from the matrix size (used 8x8 here)
         3. It means that a pixel i,j only depends on the value of a pixel i-7, j-7 to i,j and no other one.
       Then we compute a complete row of ordered dither and store it in out. */

    /*The apply the algorithm for this patch*/
    for(lv_coord_t j = 0; j < w; j++) {
        int8_t factor = dither_ordered_threshold_matrix[(y & 7) * 8 + ((j) & 7)] - 32;
        lv_color32_t tmp = grad->hmap[LV_CLAMP(0, j - 4, grad->size)];
        lv_color32_t t;
        t.ch.red   = LV_CLAMP(0, tmp.ch.red + factor, 255);
        t.ch.green = LV_CLAMP(0, tmp.ch.green + factor, 255);
        t.ch.blue  = LV_CLAMP(0, tmp.ch.blue + factor, 255);

        grad->map[j] = lv_color_hex(t.full);
    }
}
LV_ATTRIBUTE_FAST_MEM void lv_dither_ordered_ver(lv_grad_t * grad, lv_coord_t x, lv_coord_t y, lv_coord_t w)
{
    /* For vertical dithering, the error is spread on the next column (and not next line).
       Since the renderer is scanline based, it's not obvious what could be used to perform the rendering efficiently.
       The algorithm below is based on few assumptions:
         1. An error diffusion algorithm (like Floyd Steinberg) here would be hard to implement since it means that a pixel on column n depends on the pixel on row n
         2. Instead an ordered dithering algorithm shift the value a bit, but the influence only spread from the matrix size (used 8x8 here)
         3. It means that a pixel i,j only depends on the value of a pixel i-7, j-7 to i,j and no other one.
       Then we compute a complete row of ordered dither and store it in out. */

    /*Extract patch for working with, selected pseudo randomly*/
    lv_color32_t tmp = grad->hmap[LV_CLAMP(0, y - 4, grad->size)];

    /*The apply the algorithm for this patch*/
    for(lv_coord_t j = 0; j < 8; j++) {
        int8_t factor = dither_ordered_threshold_matrix[(y & 7) * 8 + ((j + x) & 7)] - 32;
        lv_color32_t t;
        t.ch.red   = LV_CLAMP(0, tmp.ch.red + factor, 255);
        t.ch.green = LV_CLAMP(0, tmp.ch.green + factor, 255);
        t.ch.blue  = LV_CLAMP(0, tmp.ch.blue + factor, 255);

        grad->map[j] = lv_color_hex(t.full);
    }
    /*Finally fill the line*/
    lv_coord_t j = 8;
    for(; j < w - 8; j += 8) {
        lv_memcpy(grad->map + j, grad->map, 8 * sizeof(*grad->map));
    }
    /* Prevent overwriting */
    for(; j < w; j++) {
        grad->map[j] = grad->map[j & 7];
    }
}


#if LV_DRAW_SW_GRADIENT_DITHER_ERROR_DIFFUSION == 1
LV_ATTRIBUTE_FAST_MEM void lv_dither_err_diff_hor(lv_grad_t * grad, lv_coord_t xs, lv_coord_t y, lv_coord_t w)
{
    LV_UNUSED(xs);
    LV_UNUSED(y);
    LV_UNUSED(w);

    /* Implement Floyd Steinberg algorithm, see https://surma.dev/things/ditherpunk/
        Coefs are:   x 7
                   3 5 1
                   / 16
        Can be implemented as:            x       (x<<3 - x)
                              (x<<2 - x) (x<<2+x)   x
    */
    int coef[4] = {0, 0, 0, 0};
#define FS_COMPUTE_ERROR(e) { coef[0] = (e<<3) - e; coef[1] = (e<<2) - e; coef[2] = (e<<2) + e; coef[3] = e; }
#define FS_COMPONENTS(A, OP, B, C) A.ch.red = LV_CLAMP(0, A.ch.red OP B.r OP C.r, 255); A.ch.green = LV_CLAMP(0, A.ch.green OP B.g OP C.g, 255); A.ch.blue = LV_CLAMP(0, A.ch.blue OP B.b OP C.b, 255);
#define FS_QUANT_ERROR(e, t, q) { lv_color32_t u; u.full = lv_color_to32(q); e.r = (int8_t)(t.ch.red - u.ch.red); e.g = (int8_t)(t.ch.green - u.ch.green); e.b = (int8_t)(t.ch.blue - u.ch.blue); }
    lv_scolor24_t next_px_err = {0}, next_l = {0}, error;
    /*First last pixel are not dithered */
    grad->map[0] = lv_color_hex(grad->hmap[0].full);
    for(lv_coord_t x = 1; x < grad->size - 1; x++) {
        lv_color32_t t = grad->hmap[x];
        lv_color_t q;
        /*Add error term*/
        FS_COMPONENTS(t, +, next_px_err, next_l);
        next_l = grad->error_acc[x + 1];
        /*Quantify*/
        q = lv_color_hex(t.full);
        /*Then compute error*/
        FS_QUANT_ERROR(error, t, q);
        /*Dither the error*/
        FS_COMPUTE_ERROR(error.r);
        next_px_err.r      = coef[0] >> 4;
        grad->error_acc[x - 1].r += coef[1] >> 4;
        grad->error_acc[x].r     += coef[2] >> 4;
        grad->error_acc[x + 1].r = coef[3] >> 4;

        FS_COMPUTE_ERROR(error.g);
        next_px_err.g      = coef[0] >> 4;
        grad->error_acc[x - 1].g += coef[1] >> 4;
        grad->error_acc[x].g     += coef[2] >> 4;
        grad->error_acc[x + 1].g = coef[3] >> 4;

        FS_COMPUTE_ERROR(error.b);
        next_px_err.b      = coef[0] >> 4;
        grad->error_acc[x - 1].b += coef[1] >> 4;
        grad->error_acc[x].b     += coef[2] >> 4;
        grad->error_acc[x + 1].b = coef[3] >> 4;

        grad->map[x] = q;
    }
    grad->map[grad->size - 1] = lv_color_hex(grad->hmap[grad->size - 1].full);
}

LV_ATTRIBUTE_FAST_MEM void lv_dither_err_diff_ver(lv_grad_t * grad, lv_coord_t xs, lv_coord_t y, lv_coord_t w)
{
    /* Try to implement error diffusion on a vertical gradient and an horizontal map using those tricks:
        Since the given hi-resolution gradient (in src) is vertical, the Floyd Steinberg algorithm pass need to be rotated,
        so we'll get this instead (from top to bottom):

          A   B   C
       1 [  ][  ][  ]
       2 [  ][  ][  ]     Pixel A2 will spread its error on pixel A3 with coefficient 7,
       3 [  ][  ][  ]     Pixel A2 will spread its error on pixel B1 with coefficient 3, B2 with coef 5 and B3 with coef 1

       When taking into account an arbitrary pixel P(i,j), its added error diffusion term is:
           e(i,j) = 1/16 * [ e(i-1,j) * 5 + e(i-1,j+1) * 3 + e(i-1,j-1) * 1 + e(i,j-1) * 7]

       This means that the error term depends on pixel W, NW, N and SW.
       If we consider that we are generating the error diffused gradient map from top to bottom, we can remember the previous
       line (N, NW) in the term above. Also, we remember the (W) term too since we are computing the gradient map from left to right.
       However, the SW term is painful for us, we can't support it (since to get it, we need its own SW term and so on).
       Let's remove it and re-dispatch the error factor accordingly so they stays normalized:
           e(i,j) ~= 1/16 * [ e(i-1,j) * 6 + e(i-1,j-1) * 1 + e(i,j-1) * 9]

       That's the idea of this pseudo Floyd Steinberg dithering */
#define FS_APPLY(d, s, c) d.r = (int8_t)(s.r * c) >> 4; d.g = (int8_t)(s.g * c) >> 4; d.b = (int8_t)(s.b * c) >> 4;
#define FS_COMPONENTS3(A, OP, B, b, C, c, D, d) \
    A.ch.red   = LV_CLAMP(0, A.ch.red   OP ((B.r * b OP C.r * c OP D.r * d) >> 4), 255); \
    A.ch.green = LV_CLAMP(0, A.ch.green OP ((B.r * b OP C.r * c OP D.r * d) >> 4), 255); \
    A.ch.blue  = LV_CLAMP(0, A.ch.blue  OP ((B.r * b OP C.r * c OP D.r * d) >> 4), 255);

    lv_scolor24_t next_px_err, prev_l = grad->error_acc[0];
    /*Compute the error term for the current pixel (first pixel is never dithered)*/
    if(xs == 0) {
        grad->map[0] = lv_color_hex(grad->hmap[y].full);
        FS_QUANT_ERROR(next_px_err, grad->hmap[y], grad->map[0]);
    }
    else {
        lv_color_t tmp = lv_color_hex(grad->hmap[y].full);
        lv_color32_t t = grad->hmap[y];
        FS_QUANT_ERROR(next_px_err, grad->hmap[y], tmp);
        FS_COMPONENTS3(t, +, next_px_err, 6, prev_l, 1, grad->error_acc[0], 9);
        grad->map[0] = lv_color_hex(t.full);
    }

    for(lv_coord_t x = 1; x < w; x++) {
        lv_color32_t t = grad->hmap[y];
        lv_color_t q;
        /*Add the current error term*/
        FS_COMPONENTS3(t, +, next_px_err, 6, prev_l, 1, grad->error_acc[x], 9);
        prev_l = grad->error_acc[x];
        /*Quantize and compute error term*/
        q = lv_color_hex(t.full);
        FS_QUANT_ERROR(next_px_err, t, q);
        /*Store error for next line computation*/
        grad->error_acc[x] = next_px_err;
        grad->map[x] = q;
    }
}
#endif
#endif
#endif /*LV_USE_DRAW_SW*/
