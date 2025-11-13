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
    /* alpha = (int32_t)((1 << aprec) * (1.0f - expf(-2.3f / (radius + 1.f)))) */

    static const float alpha_table[] = {
        0.899741f, 0.683363f, 0.535441f, 0.437295f,
        0.368716f, 0.318414f, 0.280048f, 0.249863f,
        0.225514f, 0.205466f, 0.188679f, 0.174418f,
        0.162156f, 0.151500f, 0.142156f, 0.133896f,
        0.126541f, 0.119951f, 0.114013f, 0.108634f,
        0.103739f, 0.099266f, 0.095163f, 0.091385f,
        0.087895f, 0.084662f, 0.081658f, 0.078860f,
        0.076247f, 0.073801f, 0.071508f, 0.069353f,
        0.067324f, 0.065410f, 0.063602f, 0.061891f,
        0.060270f, 0.058731f, 0.057269f, 0.055878f,
        0.054553f, 0.053289f, 0.052083f, 0.050930f,
        0.049827f, 0.048771f, 0.047758f, 0.046787f,
        0.045854f, 0.044958f, 0.044096f, 0.043267f,
        0.042468f, 0.041698f, 0.040956f, 0.040239f,
        0.039548f, 0.038879f, 0.038233f, 0.037608f,
        0.037003f, 0.036417f, 0.035850f, 0.035299f,
        0.034766f, 0.034248f, 0.033746f, 0.033258f,
        0.032784f, 0.032323f, 0.031875f, 0.031440f,
        0.031016f, 0.030603f, 0.030201f, 0.029810f,
        0.029428f, 0.029057f, 0.028694f, 0.028341f,
        0.027996f, 0.027659f, 0.027330f, 0.027009f,
        0.026696f, 0.026390f, 0.026090f, 0.025798f,
        0.025512f, 0.025232f, 0.024958f, 0.024690f,
        0.024428f, 0.024171f, 0.023920f, 0.023674f,
        0.023432f, 0.023196f, 0.022965f, 0.022738f,
        0.022515f, 0.022297f, 0.022083f, 0.021873f,
        0.021667f, 0.021464f, 0.021266f, 0.021071f,
        0.020880f, 0.020692f, 0.020508f, 0.020326f,
        0.020148f, 0.019973f, 0.019801f, 0.019632f,
        0.019466f, 0.019303f, 0.019142f, 0.018984f,
        0.018829f, 0.018676f, 0.018525f, 0.018377f,
        0.018232f, 0.018088f, 0.017947f, 0.017808f,
    };
    static const int32_t alpha_table_size = sizeof(alpha_table) / sizeof(alpha_table[0]);

    if(radius >= alpha_table_size) {
        LV_LOG_WARN("Radius: %" LV_PRIu32 " is too large, clamping to %" LV_PRIu32, radius, alpha_table_size - 1);
        radius = alpha_table_size - 1;
    }

    return (int32_t)((1 << aprec) * alpha_table[radius]);
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
