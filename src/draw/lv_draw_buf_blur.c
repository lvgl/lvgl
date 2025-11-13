/**
 * @file lv_draw_buf_blur.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "../stdlib/lv_sprintf.h"
#include "lv_draw_buf.h"
#include "lv_draw_buf_blur.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void exp_blur(uint8_t * dst,
                     const uint8_t * src,
                     int32_t width,
                     int32_t height,
                     int32_t stride,
                     int32_t radius,
                     int32_t aprec,
                     int32_t zprec);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_buf_blur_args_init(lv_draw_buf_blur_args_t * args)
{
    LV_ASSERT_NULL(args);
    lv_memzero(args, sizeof(lv_draw_buf_blur_args_t));
    args->type = LV_DRAW_BUF_BLUR_TYPE_EXP;
    args->radius = 10;
    args->aprec = 16;
    args->zprec = 7;
}

lv_result_t lv_draw_buf_blur(lv_draw_buf_t * dst_buf, const lv_draw_buf_t * src_buf,
                             const lv_draw_buf_blur_args_t * args)
{
    LV_ASSERT_NULL(dst_buf);
    LV_ASSERT_NULL(src_buf);
    LV_ASSERT_NULL(args);

    if(src_buf->header.cf != LV_COLOR_FORMAT_ARGB8888 && src_buf->header.cf != LV_COLOR_FORMAT_XRGB8888) {
        LV_LOG_WARN("Unsupported color format: %d", src_buf->header.cf);
        return LV_RESULT_INVALID;
    }

    if(src_buf->header.w != dst_buf->header.w || src_buf->header.h != dst_buf->header.h
       || src_buf->header.stride != dst_buf->header.stride || src_buf->header.cf != dst_buf->header.cf) {
        LV_LOG_WARN("The header info is different of src and dst");
        return LV_RESULT_INVALID;
    }

    if(args->radius < 1) {
        /* When the user does not set the blur radius, the image needs to be copied. */
        if(dst_buf != src_buf) {
            lv_draw_buf_copy(dst_buf, NULL, src_buf, NULL);
        }

        return LV_RESULT_OK;
    }

    switch(args->type) {
        case LV_DRAW_BUF_BLUR_TYPE_EXP:
            exp_blur(
                dst_buf->data,
                src_buf->data,
                src_buf->header.w,
                src_buf->header.h,
                src_buf->header.stride,
                args->radius,
                args->aprec,
                args->zprec);
            break;

        default:
            LV_LOG_WARN("Unsupported blur type: %d", args->type);
            return LV_RESULT_INVALID;
    }

    lv_draw_buf_flush_cache(dst_buf, NULL);
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void exp_blur_inner(
    uint8_t * dst,
    const uint8_t * src,
    int32_t * zB,
    int32_t * zG,
    int32_t * zR,
    int32_t * zA,
    int32_t alpha,
    int32_t aprec,
    int32_t zprec)
{
    int32_t B = *src;
    int32_t G = *(src + 1);
    int32_t R = *(src + 2);
    int32_t A = *(src + 3);

    *zB += (alpha * ((B << zprec) - *zB)) >> aprec;
    *zG += (alpha * ((G << zprec) - *zG)) >> aprec;
    *zR += (alpha * ((R << zprec) - *zR)) >> aprec;
    *zA += (alpha * ((A << zprec) - *zA)) >> aprec;

    *dst = *zB >> zprec;
    *(dst + 1) = *zG >> zprec;
    *(dst + 2) = *zR >> zprec;
    *(dst + 3) = *zA >> zprec;
}

static inline void exp_blur_row(
    uint8_t * dst,
    const uint8_t * src,
    int32_t width,
    int32_t height,
    int32_t stride,
    int32_t line,
    int32_t alpha,
    int32_t aprec,
    int32_t zprec)
{
    LV_UNUSED(height);
    const uint8_t * input = &(src[line * stride]);
    uint8_t * output = &(dst[line * stride]);
    int32_t zB = *input << zprec;
    int32_t zG = *(input + 1) << zprec;
    int32_t zR = *(input + 2) << zprec;
    int32_t zA = *(input + 3) << zprec;

    for(int32_t index = 0; index < width; index++) {
        exp_blur_inner(
            &output[index * sizeof(uint32_t)],
            &input[index * sizeof(uint32_t)],
            &zB, &zG, &zR, &zA,
            alpha, aprec, zprec);
    }

    for(int32_t index = width - 2; index >= 0; index--) {
        exp_blur_inner(
            &output[index * sizeof(uint32_t)],
            &output[index * sizeof(uint32_t)],
            &zB, &zG, &zR, &zA,
            alpha, aprec, zprec);
    }
}

static inline void exp_blur_col(
    uint8_t * dst,
    int32_t width,
    int32_t height,
    int32_t stride,
    int32_t x,
    int32_t alpha,
    int32_t aprec,
    int32_t zprec)
{
    LV_UNUSED(width);
    uint8_t * ptr = dst + x * sizeof(uint32_t);
    int32_t zB = *((uint8_t *)ptr) << zprec;
    int32_t zG = *((uint8_t *)ptr + 1) << zprec;
    int32_t zR = *((uint8_t *)ptr + 2) << zprec;
    int32_t zA = *((uint8_t *)ptr + 3) << zprec;

    for(int32_t index = 1; index < height; index++) {
        exp_blur_inner(
            &ptr[index * stride],
            &ptr[index * stride],
            &zB, &zG, &zR, &zA,
            alpha, aprec, zprec);
    }

    for(int32_t index = height - 2; index >= 0; index--) {
        exp_blur_inner(
            &ptr[index * stride],
            &ptr[index * stride],
            &zB, &zG, &zR, &zA,
            alpha, aprec, zprec);
    }
}

static int32_t exp_get_alpha(int32_t radius, int32_t aprec)
{
    /**
     * Optimize expression calculation using table lookup method:
     * alpha = (int32_t)((1 << aprec) * (1.0f - expf(-2.3f / (radius + 1.f))))
     *
     * Table generation algorithm:
     * for(int r = 0; r < 128; r++) {
     *    float v = (1.0f - expf(-2.3f / (r + 1.f)));
     *    printf("%d,", (int)(v * 100000000.0f));
     *    if(r % 4 == 3) printf("\n");
     * }
     */

    static const int32_t exp_table[] = {
        89974120, 68336320, 53544100, 43729512,
        36871636, 31841434, 28004848, 24986344,
        22551388, 20546638, 18867850, 17441798,
        16215575, 15150040, 14215630, 13389575,
        12654102, 11995107, 11401266, 10863388,
        10373920, 9926617, 9516257, 9138453,
        8789486, 8466166, 8165776, 7885962,
        7624680, 7380146, 7150805, 6935280,
        6732356, 6540972, 6360161, 6189078,
        6026953, 5873102, 5726903, 5587810,
        5455309, 5328947, 5208301, 5093002,
        4982692, 4877054, 4775810, 4678678,
        4585421, 4495805, 4409623, 4326683,
        4246807, 4169827, 4095584, 4023945,
        3954762, 3887921, 3823298, 3760790,
        3700292, 3641712, 3584957, 3529942,
        3476590, 3424829, 3374582, 3325790,
        3278392, 3232318, 3187531, 3143960,
        3101569, 3060305, 3020119, 2980983,
        2942842, 2905666, 2869421, 2834063,
        2799570, 2765905, 2733039, 2700948,
        2669602, 2638971, 2609038, 2579778,
        2551162, 2523177, 2495801, 2469009,
        2442789, 2417117, 2391982, 2367359,
        2343243, 2319610, 2296453, 2273750,
        2251494, 2229666, 2208262, 2187264,
        2166658, 2146440, 2126592, 2107113,
        2087986, 2069199, 2050751, 2032631,
        2014821, 1997327, 1980131, 1963234,
        1946616, 1930278, 1914215, 1898413,
        1882875, 1867586, 1852542, 1837742,
        1823175, 1808840, 1794725, 1780825,
    };
    static const int32_t exp_table_size = sizeof(exp_table) / sizeof(exp_table[0]);

    if(radius >= exp_table_size) {
        LV_LOG_WARN("Radius: %" LV_PRIu32 " is too large, clamping to %" LV_PRIu32, radius, exp_table_size - 1);
        radius = exp_table_size - 1;
    }

    return (int32_t)(((int64_t)(1 << aprec) * exp_table[radius]) / 100000000);
}

static void exp_blur(uint8_t * dst,
                     const uint8_t * src,
                     int32_t width,
                     int32_t height,
                     int32_t stride,
                     int32_t radius,
                     int32_t aprec,
                     int32_t zprec)
{
    /**
     * calculate the alpha such that 90% of
     * the kernel is within the radius.
     * (Kernel extends to infinity)
     */
    const int32_t alpha = exp_get_alpha(radius, aprec);

    for(int32_t row = 0; row < height; row++) {
        exp_blur_row(dst, src, width, height, stride, row, alpha, aprec, zprec);
    }

    for(int32_t col = 0; col < width; col++) {
        exp_blur_col(dst, width, height, stride, col, alpha, aprec, zprec);
    }
}
