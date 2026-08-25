/**
 * @file lv_draw_sw_blend_sve2_to_rgb888.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_sve2_to_rgb888.h"
#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_SVE2 && defined(__ARM_FEATURE_SVE2)

#include "../lv_draw_sw_blend_private.h"
#include LV_STDINT_INCLUDE
#include "lv_sve2_extension.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/* *INDENT-OFF* */

#define lv_sve_rgb32_stride_impl(...)                         \
    lv_sve_stride_loop_pixel(uStride, vTailPred)              \
    {                                                         \
                                                              \
        svuint16x4_t vSourceLow16x4 = svundef4_u16();         \
        svuint16x4_t vSourceHigh16x4 = svundef4_u16();        \
                                                              \
        svuint16x4_t vTargetLow16x4 = svundef4_u16();         \
        svuint16x4_t vTargetHigh16x4 = svundef4_u16();        \
                                                              \
        svld4ub_u16(vTailPred,                                \
                    (uint8_t *)pwSource,                      \
                    &vSourceLow16x4,                          \
                    &vSourceHigh16x4);                        \
                                                              \
        svld4ub_u16(vTailPred,                                \
                    (uint8_t *)pwTarget,                      \
                    &vTargetLow16x4,                          \
                    &vTargetHigh16x4);                        \
                                                              \
        __VA_ARGS__;                                          \
                                                              \
        svst4ub_u16(vTailPred,                                \
                    (uint8_t *)pwTarget,                      \
                    vTargetLow16x4,                           \
                    vTargetHigh16x4);                         \
                                                              \
        pwSource += sve_iteration_advance;                    \
        pwTarget += sve_iteration_advance;                    \
    }

#define lv_sve_rgb24_stride_impl(...)                         \
    lv_sve_stride_loop_pixel(uStride, vTailPred)              \
    {                                                         \
                                                              \
        svuint16x3_t vSourceLow16x3 = svundef3_u16();         \
        svuint16x3_t vSourceHigh16x3 = svundef3_u16();        \
                                                              \
        svuint16x3_t vTargetLow16x3 = svundef3_u16();         \
        svuint16x3_t vTargetHigh16x3 = svundef3_u16();        \
                                                              \
        svld3ub_u16(vTailPred,                                \
                    (uint8_t *)pchSource,                     \
                    &vSourceLow16x3,                          \
                    &vSourceHigh16x3);                        \
                                                              \
        svld3ub_u16(vTailPred,                                \
                    (uint8_t *)pchTarget,                     \
                    &vTargetLow16x3,                          \
                    &vTargetHigh16x3);                        \
                                                              \
        __VA_ARGS__;                                          \
                                                              \
        svst3ub_u16(vTailPred,                                \
                    (uint8_t *)pchTarget,                     \
                    vTargetLow16x3,                           \
                    vTargetHigh16x3);                         \
                                                              \
        pchSource += sve_iteration_advance * 3;               \
        pchTarget += sve_iteration_advance * 3;               \
    }

#define lv_sve_rgb32_to_rgb24_stride_impl(...)                \
    lv_sve_stride_loop_pixel(uStride, vTailPred)              \
    {                                                         \
        svuint16x4_t vSourceLow16x4 = svundef4_u16();         \
        svuint16x4_t vSourceHigh16x4 = svundef4_u16();        \
                                                              \
        svuint16x3_t vTargetLow16x3 = svundef3_u16();         \
        svuint16x3_t vTargetHigh16x3 = svundef3_u16();        \
                                                              \
        svld4ub_u16(vTailPred,                                \
                    (uint8_t *)pwSource,                      \
                    &vSourceLow16x4,                          \
                    &vSourceHigh16x4);                        \
                                                              \
        svld3ub_u16(vTailPred,                                \
                    (uint8_t *)pchTarget,                     \
                    &vTargetLow16x3,                          \
                    &vTargetHigh16x3);                        \
                                                              \
        __VA_ARGS__;                                          \
                                                              \
        svst3ub_u16(vTailPred,                                \
                    (uint8_t *)pchTarget,                     \
                    vTargetLow16x3,                           \
                    vTargetHigh16x3);                         \
                                                              \
        pwSource += sve_iteration_advance;                    \
        pchTarget += sve_iteration_advance * 3;               \
    }

#define lv_sve_rgb24_to_rgb32_stride_impl(...)                \
    lv_sve_stride_loop_pixel(uStride, vTailPred)              \
    {                                                         \
                                                              \
        svuint16x3_t vSourceLow16x3 = svundef3_u16();         \
        svuint16x3_t vSourceHigh16x3 = svundef3_u16();        \
                                                              \
        svuint16x4_t vTargetLow16x4 = svundef4_u16();         \
        svuint16x4_t vTargetHigh16x4 = svundef4_u16();        \
                                                              \
        svld3ub_u16(vTailPred,                                \
                    (uint8_t *)pchSource,                     \
                    &vSourceLow16x3,                          \
                    &vSourceHigh16x3);                        \
                                                              \
        svld4ub_u16(vTailPred,                                \
                    (uint8_t *)pwTarget,                      \
                    &vTargetLow16x4,                          \
                    &vTargetHigh16x4);                        \
                                                              \
        __VA_ARGS__;                                          \
                                                              \
        svst4ub_u16(vTailPred,                                \
                    (uint8_t *)pwTarget,                      \
                    vTargetLow16x4,                           \
                    vTargetHigh16x4);                         \
                                                              \
        pchSource += sve_iteration_advance * 3;               \
        pwTarget += sve_iteration_advance;                    \
    }

#define lv_sve_rgb32_no_alpha_stride_impl(                    \
    ma_alpha_idx,                                             \
    ma_sve_chn_iterator,                                      \
    ...)                                                      \
    lv_sve_stride_loop_pixel(uStride, vTailPred)              \
    {                                                         \
                                                              \
        svuint16x4_t vSourceLow16x4 = svundef4_u16();         \
        svuint16x4_t vSourceHigh16x4 = svundef4_u16();        \
                                                              \
        svuint16x4_t vTargetLow16x4 = svundef4_u16();         \
        svuint16x4_t vTargetHigh16x4 = svundef4_u16();        \
                                                              \
        svld4ub_u16(vTailPred,                                \
                    (uint8_t *)pwSource,                      \
                    &vSourceLow16x4,                          \
                    &vSourceHigh16x4);                        \
                                                              \
        svld4ub_u16(vTailPred,                                \
                    (uint8_t *)pwTarget,                      \
                    &vTargetLow16x4,                          \
                    &vTargetHigh16x4);                        \
                                                              \
        vSourceLow16x4 = svset4(vSourceLow16x4,               \
                                (ma_alpha_idx),               \
                                svdup_u16(0xFF));             \
        vSourceHigh16x4 = svset4(vSourceHigh16x4,             \
                                 (ma_alpha_idx),              \
                                 svdup_u16(0xFF));            \
                                                              \
        /* process low half */                                \
        ma_sve_chn_iterator(vSourceLow16x4, vTargetLow16x4,   \
                            __VA_ARGS__);                     \
                                                              \
        /* process high half */                               \
        ma_sve_chn_iterator(vSourceHigh16x4, vTargetHigh16x4, \
                            __VA_ARGS__);                     \
                                                              \
        svst4ub_u16(vTailPred,                                \
                    (uint8_t *)pwTarget,                      \
                    vTargetLow16x4,                           \
                    vTargetHigh16x4);                         \
                                                              \
        pwSource += sve_iteration_advance;                    \
        pwTarget += sve_iteration_advance;                    \
    }

#define lv_sve_rgb32_no_src_stride_impl(                      \
    ma_sve_chn_iterator,                                      \
    ma_vSource16x4,                                           \
    ...)                                                      \
    lv_sve_stride_loop_pixel(uStride, vTailPred) {            \
                                                              \
        svuint16x4_t vTargetLow16x4 = svundef4_u16();         \
        svuint16x4_t vTargetHigh16x4 = svundef4_u16();        \
                                                              \
        svld4ub_u16(vTailPred,                                \
                    (uint8_t *)pwTarget,                      \
                    &vTargetLow16x4,                          \
                    &vTargetHigh16x4);                        \
                                                              \
        /* process low half */                                \
        ma_sve_chn_iterator(ma_vSource16x4, vTargetLow16x4,   \
                            __VA_ARGS__);                     \
                                                              \
        /* process high half */                               \
        ma_sve_chn_iterator(ma_vSource16x4, vTargetHigh16x4,  \
                            __VA_ARGS__);                     \
                                                              \
        svst4ub_u16(vTailPred,                                \
                    (uint8_t *)pwTarget,                      \
                    vTargetLow16x4,                           \
                    vTargetHigh16x4);                         \
                                                              \
        pwTarget += sve_iteration_advance;                    \
    }

#define lv_sve_rgb24_no_src_stride_impl(                      \
    ma_sve_chn_iterator,                                      \
    ma_vSource16x3,                                           \
    ...)                                                      \
    lv_sve_stride_loop_pixel(uStride, vTailPred) {            \
                                                              \
        svuint16x3_t vTargetLow16x3 = svundef3_u16();         \
        svuint16x3_t vTargetHigh16x3= svundef3_u16();         \
                                                              \
        svld3ub_u16(vTailPred,                                \
                    (uint8_t *)pchTarget,                     \
                    &vTargetLow16x3,                          \
                    &vTargetHigh16x3);                        \
                                                              \
        /* process low half */                                \
        ma_sve_chn_iterator(ma_vSource16x3, vTargetLow16x3,   \
                            __VA_ARGS__);                     \
                                                              \
        /* process high half */                               \
        ma_sve_chn_iterator(ma_vSource16x3, vTargetHigh16x3,  \
                            __VA_ARGS__);                     \
                                                              \
        svst3ub_u16(vTailPred,                                \
                    (uint8_t *)pchTarget,                     \
                    vTargetLow16x3,                           \
                    vTargetHigh16x3);                         \
                                                              \
        pchTarget += sve_iteration_advance * 3;               \
    }

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

LV_NONNULL(1)
static inline
void lv_sve_cccn888_stride_fill_colour(uint32_t * LV_RESTRICT pwTarget,
                                       size_t uStride,
                                       uint32_t wColour)
{
    size_t n = 0;

    svuint32_t vColour = svdup_u32(wColour);
    do {
        svbool_t vTailPred = svwhilelt_b32((uint32_t)n, (uint32_t)uStride);

        svst1_u32(vTailPred, pwTarget, vColour);

        pwTarget += svlenu32();
        n += svlenu32();
    } while(n < uStride);
}

LV_NONNULL(1)
static inline
void lv_sve_ccc888_stride_fill_colour(uint8_t * LV_RESTRICT pchTarget,
                                      size_t uStride,
                                      svuint8x3_t tPixel8x3)
{
    size_t n = 0;

    do {
        svbool_t vTailPred = svwhilelt_b8((uint32_t)n, (uint32_t)uStride);

        svst3_u8(vTailPred, pchTarget, tPixel8x3);

        pchTarget += svlenu8() * 3;
        n += svlenu8();
    } while(n < uStride);
}

lv_result_t lv_draw_sw_blend_sve2_color_to_rgb888(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    const int32_t dest_stride    = dsc->dest_stride;
    uint8_t * dest_buf_8      = dsc->dest_buf;

    if(3 == dest_px_size) {
        svuint8x3_t vPixels = svcreate3_u8(svdup_u8(dsc->color.blue),
                                           svdup_u8(dsc->color.green),
                                           svdup_u8(dsc->color.red));

        for(int32_t y = 0; y < h; y++) {

            lv_sve_ccc888_stride_fill_colour(dest_buf_8, w, vPixels);

            dest_buf_8 += dest_stride;
        }
    }
    else if(4 == dest_px_size) {
        uint32_t wColour = lv_color_to_u32(dsc->color);
        for(int32_t y = 0; y < h; y++) {

            lv_sve_cccn888_stride_fill_colour((uint32_t *)dest_buf_8,
                                              w,
                                              wColour);

            dest_buf_8 += dest_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_cccn888_stride_fill_colour_with_opacity(
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride,
    svuint16x4_t vColour16x4,
    uint16_t hwOpacity)
{
    lv_sve_rgb32_no_src_stride_impl(
        lv_sve_pixel_ccca_foreach_chn012,
        vColour16x4, {
            sve_target_u16 = lv_sve_chn_blend_with_opacity(
                sve_source_u16,
                sve_target_u16,
                hwOpacity);
        });
}

LV_NONNULL(1)
static inline
void lv_sve_ccc888_stride_fill_colour_with_opacity(
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride,
    svuint16x3_t tPixel16x3,
    uint16_t hwOpacity)
{
    lv_sve_rgb24_no_src_stride_impl(
        lv_sve_pixel_ccc_foreach_chn,
        tPixel16x3, {
            sve_target_u16 = lv_sve_chn_blend_with_opacity(
                sve_source_u16,
                sve_target_u16,
                hwOpacity);
        });
}

lv_result_t lv_draw_sw_blend_sve2_color_to_rgb888_with_opa(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w             = dsc->dest_w;
    const int32_t h             = dsc->dest_h;
    const int32_t dest_stride   = dsc->dest_stride;
    uint8_t * dest_buf_8        = dsc->dest_buf;
    uint16_t opa                = dsc->opa;

    opa += opa == 255;

    if(3 == dest_px_size) {
        svuint16x3_t vColour16x3 = svcreate3_u16(svdup_u16(dsc->color.blue),
                                                 svdup_u16(dsc->color.green),
                                                 svdup_u16(dsc->color.red));

        for(int32_t y = 0; y < h; y++) {

            lv_sve_ccc888_stride_fill_colour_with_opacity(
                dest_buf_8,
                w,
                vColour16x3,
                opa);

            dest_buf_8 += dest_stride;
        }
    }
    else if(4 == dest_px_size) {
        svuint16x4_t vColour16x4 = svcreate4_u16(
                                        svdup_u16(dsc->color.blue),
                                        svdup_u16(dsc->color.green),
                                        svdup_u16(dsc->color.red),
                                        svdup_u16(0xFF));

        for(int32_t y = 0; y < h; y++) {

            lv_sve_cccn888_stride_fill_colour_with_opacity(
                (uint32_t *)dest_buf_8,
                w,
                vColour16x4,
                opa);

            dest_buf_8 += dest_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_cccn888_stride_fill_colour_with_mask(
    uint32_t * LV_RESTRICT pwTarget,
    const uint8_t * LV_RESTRICT pchMask,
    size_t uStride,
    svuint16x4_t vColour16x4)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x4_t vTargetLow16x4 = svundef4_u16();
        svuint16x4_t vTargetHigh16x4 = svundef4_u16();

        svld4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    &vTargetLow16x4,
                    &vTargetHigh16x4);

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        lv_sve_pixel_ccca_foreach_chn012(vColour16x4, vTargetLow16x4, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                sve_source_u16,
                                sve_target_u16,
                                vMaskLow);
        });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        lv_sve_pixel_ccca_foreach_chn012(vColour16x4, vTargetHigh16x4, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                sve_source_u16,
                                sve_target_u16,
                                vMaskHigh);
        });

        svst4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    vTargetLow16x4,
                    vTargetHigh16x4);

        pwTarget += sve_iteration_advance;
        pchMask += sve_iteration_advance;
    }
}

LV_NONNULL(1)
static inline
void lv_sve_ccc888_stride_fill_colour_with_mask(
    uint8_t * LV_RESTRICT pchTarget,
    const uint8_t * LV_RESTRICT pchMask,
    size_t uStride,
    svuint16x3_t tPixel16x3)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x3_t vTargetLow16x3 = svundef3_u16();
        svuint16x3_t vTargetHigh16x3 = svundef3_u16();

        svld3ub_u16(vTailPred,
                    (uint8_t *)pchTarget,
                    &vTargetLow16x3,
                    &vTargetHigh16x3);

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        lv_sve_pixel_ccc_foreach_chn(tPixel16x3, vTargetLow16x3, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                sve_source_u16,
                                sve_target_u16,
                                vMaskLow);
        });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        lv_sve_pixel_ccc_foreach_chn(tPixel16x3, vTargetHigh16x3, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                sve_source_u16,
                                sve_target_u16,
                                vMaskHigh);
        });

        svst3ub_u16(vTailPred,
                    pchTarget,
                    vTargetLow16x3,
                    vTargetHigh16x3);

        pchTarget += sve_iteration_advance * 3;
        pchMask += sve_iteration_advance;
    }
}

lv_result_t lv_draw_sw_blend_sve2_color_to_rgb888_with_mask(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;

    if(3 == dest_px_size) {
        svuint16x3_t vColour16x3 = svcreate3_u16(   svdup_u16(dsc->color.blue),
                                                    svdup_u16(dsc->color.green),
                                                    svdup_u16(dsc->color.red));

        for(int32_t y = 0; y < h; y++) {

            lv_sve_ccc888_stride_fill_colour_with_mask( dest_buf_8,
                                                        mask_buf_8,
                                                        w,
                                                        vColour16x3);

            dest_buf_8 += dest_stride;
            mask_buf_8 += mask_stride;
        }
    }
    else if(4 == dest_px_size) {
        svuint16x4_t vColour16x4 = svcreate4_u16(
            svdup_u16(dsc->color.blue),
            svdup_u16(dsc->color.green),
            svdup_u16(dsc->color.red),
            svdup_u16(0xFF));

        for(int32_t y = 0; y < h; y++) {

            lv_sve_cccn888_stride_fill_colour_with_mask(
                (uint32_t *)dest_buf_8,
                mask_buf_8,
                w,
                vColour16x4);

            dest_buf_8 += dest_stride;
            mask_buf_8 += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_cccn888_stride_fill_colour_with_mask_and_opacity(
    uint32_t * LV_RESTRICT pwTarget,
    const uint8_t * LV_RESTRICT pchMask,
    size_t uStride,
    svuint16x4_t vColour16x4,
    uint16_t hwOpacity)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x4_t vTargetLow16x4 = svundef4_u16();
        svuint16x4_t vTargetHigh16x4 = svundef4_u16();

        svld4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    &vTargetLow16x4,
                    &vTargetHigh16x4);

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        lv_sve_pixel_ccca_foreach_chn012(vColour16x4, vTargetLow16x4, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                sve_source_u16,
                sve_target_u16,
                vMaskLow,
                hwOpacity);
        });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        lv_sve_pixel_ccca_foreach_chn012(vColour16x4, vTargetHigh16x4, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                sve_source_u16,
                sve_target_u16,
                vMaskHigh,
                hwOpacity);
        });

        svst4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    vTargetLow16x4,
                    vTargetHigh16x4);

        pwTarget += sve_iteration_advance;
        pchMask += sve_iteration_advance;
    }
}

LV_NONNULL(1)
static inline
void lv_sve_ccc888_stride_fill_colour_with_mask_and_opacity(
    uint8_t * LV_RESTRICT pchTarget,
    const uint8_t * LV_RESTRICT pchMask,
    size_t uStride,
    svuint16x3_t tPixel16x3,
    uint16_t hwOpacity)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x3_t vTargetLow16x3 = svundef3_u16();
        svuint16x3_t vTargetHigh16x3 = svundef3_u16();

        svld3ub_u16(vTailPred,
                    pchTarget,
                    &vTargetLow16x3,
                    &vTargetHigh16x3);

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        lv_sve_pixel_ccc_foreach_chn(tPixel16x3, vTargetLow16x3, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                sve_source_u16,
                sve_target_u16,
                vMaskLow,
                hwOpacity);
        });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        lv_sve_pixel_ccc_foreach_chn(tPixel16x3, vTargetHigh16x3, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                sve_source_u16,
                sve_target_u16,
                vMaskHigh,
                hwOpacity);
        });

        svst3ub_u16(vTailPred,
                    pchTarget,
                    vTargetLow16x3,
                    vTargetHigh16x3);

        pchTarget += sve_iteration_advance * 3;
        pchMask += sve_iteration_advance;
    }
}

lv_result_t lv_draw_sw_blend_sve2_color_to_rgb888_with_opa_mask(lv_draw_sw_blend_fill_dsc_t * dsc,
                                                                uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;
    uint8_t opa = dsc->opa;

    opa += opa == 255;

    if(3 == dest_px_size) {
        svuint16x3_t vColour16x3 = svcreate3_u16(svdup_u16(dsc->color.blue),
                                                    svdup_u16(dsc->color.green),
                                                    svdup_u16(dsc->color.red));

        for(int32_t y = 0; y < h; y++) {

            lv_sve_ccc888_stride_fill_colour_with_mask_and_opacity(
                dest_buf_8,
                mask_buf_8,
                w,
                vColour16x3,
                opa);

            dest_buf_8 += dest_stride;
            mask_buf_8 += mask_stride;
        }
    }
    else if(4 == dest_px_size) {
        svuint16x4_t vColour16x4 = svcreate4_u16(
            svdup_u16(dsc->color.blue),
            svdup_u16(dsc->color.green),
            svdup_u16(dsc->color.red),
            svdup_u16(0xFF));

        for(int32_t y = 0; y < h; y++) {

            lv_sve_cccn888_stride_fill_colour_with_mask_and_opacity(
                (uint32_t *)dest_buf_8,
                mask_buf_8,
                w,
                vColour16x4,
                opa);

            dest_buf_8 += dest_stride;
            mask_buf_8 += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_gray8_stride_blend_to_cccn888(
    const uint8_t * LV_RESTRICT pchSource,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x4_t vTargetLow16x4 = svundef4_u16();
        svuint16x4_t vTargetHigh16x4 = svundef4_u16();

        svld4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    &vTargetLow16x4,
                    &vTargetHigh16x4);

        svuint8_t vu8Source = svld1_u8(vTailPred, pchSource);

        /* process low half */
        svuint16_t vSourceLow = svunpklo_u16(vu8Source);
        vTargetLow16x4 = svset4(vTargetLow16x4, 0, vSourceLow);
        vTargetLow16x4 = svset4(vTargetLow16x4, 1, vSourceLow);
        vTargetLow16x4 = svset4(vTargetLow16x4, 2, vSourceLow);

        /* process high half */
        svuint16_t vSourceHigh = svunpkhi_u16(vu8Source);
        vTargetHigh16x4 = svset4(vTargetHigh16x4, 0, vSourceHigh);
        vTargetHigh16x4 = svset4(vTargetHigh16x4, 1, vSourceHigh);
        vTargetHigh16x4 = svset4(vTargetHigh16x4, 2, vSourceHigh);

        svst4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    vTargetLow16x4,
                    vTargetHigh16x4);

        pwTarget += sve_iteration_advance;
        pchSource += sve_iteration_advance;
    }
}

LV_NONNULL(1)
static inline
void lv_sve_gray8_stride_blend_to_ccc888(
    const uint8_t * LV_RESTRICT pchSource,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x3_t vTargetLow16x3 = svundef3_u16();
        svuint16x3_t vTargetHigh16x3 = svundef3_u16();

        svld3ub_u16(vTailPred,
                    pchTarget,
                    &vTargetLow16x3,
                    &vTargetHigh16x3);

        svuint8_t vu8Source = svld1_u8(vTailPred, pchSource);

        /* process low half */
        svuint16_t vSourceLow = svunpklo_u16(vu8Source);
        vTargetLow16x3 = svset3(vTargetLow16x3, 0, vSourceLow);
        vTargetLow16x3 = svset3(vTargetLow16x3, 1, vSourceLow);
        vTargetLow16x3 = svset3(vTargetLow16x3, 2, vSourceLow);

        /* process high half */
        svuint16_t vSourceHigh = svunpkhi_u16(vu8Source);
        vTargetHigh16x3 = svset3(vTargetHigh16x3, 0, vSourceHigh);
        vTargetHigh16x3 = svset3(vTargetHigh16x3, 1, vSourceHigh);
        vTargetHigh16x3 = svset3(vTargetHigh16x3, 2, vSourceHigh);

        svst3ub_u16(vTailPred,
                    pchTarget,
                    vTargetLow16x3,
                    vTargetHigh16x3);

        pchTarget += sve_iteration_advance * 3;
        pchSource += sve_iteration_advance;
    }
}

lv_result_t lv_draw_sw_blend_sve2_l8_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t src_stride = dsc->src_stride;
    const int32_t dest_stride = dsc->dest_stride;
    uint8_t * dest_buf_8 = dsc->dest_buf;
    const uint8_t * src_buf_l8 = dsc->src_buf;

    if(3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_gray8_stride_blend_to_ccc888(src_buf_l8, dest_buf_8, w);

            dest_buf_8 += dest_stride;
            src_buf_l8 += src_stride;
        }
    }
    else if(4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_gray8_stride_blend_to_cccn888(src_buf_l8,
                                                 (uint32_t *)dest_buf_8,
                                                 w);

            dest_buf_8 += dest_stride;
            src_buf_l8 += src_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_gray8_stride_blend_to_cccn888_with_opacity(
    const uint8_t * LV_RESTRICT pchSource,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x4_t vTargetLow16x4 = svundef4_u16();
        svuint16x4_t vTargetHigh16x4 = svundef4_u16();

        svld4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    &vTargetLow16x4,
                    &vTargetHigh16x4);

        svuint8_t vu8Source = svld1_u8(vTailPred, pchSource);

        /* process low half */
        svuint16_t vSourceLow = svunpklo_u16(vu8Source);
        svuint16x4_t vSourceLow16x4 = svcreate4(vSourceLow,
                                                vSourceLow,
                                                vSourceLow,
                                                vSourceLow);
        lv_sve_pixel_ccca_foreach_chn012(vSourceLow16x4, vTargetLow16x4, {
            sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                sve_source_u16,
                sve_target_u16,
                hwOpacity);
        });

        /* process high half */
        svuint16_t vSourceHigh = svunpkhi_u16(vu8Source);
        svuint16x4_t vSourceHigh16x4 = svcreate4(   vSourceHigh,
                                                    vSourceHigh,
                                                    vSourceHigh,
                                                    vSourceHigh);
        lv_sve_pixel_ccca_foreach_chn012(vSourceHigh16x4, vTargetHigh16x4, {
            sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                sve_source_u16,
                sve_target_u16,
                hwOpacity);
        });

        svst4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    vTargetLow16x4,
                    vTargetHigh16x4);

        pwTarget += sve_iteration_advance;
        pchSource += sve_iteration_advance;
    }
}

LV_NONNULL(1)
static inline
void lv_sve_gray8_stride_blend_to_ccc888_with_opacity(
    const uint8_t * LV_RESTRICT pchSource,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x3_t vTargetLow16x3 = svundef3_u16();
        svuint16x3_t vTargetHigh16x3 = svundef3_u16();

        svld3ub_u16(vTailPred,
                    pchTarget,
                    &vTargetLow16x3,
                    &vTargetHigh16x3);

        svuint8_t vu8Source = svld1_u8(vTailPred, pchSource);

        /* process low half */
        svuint16_t vSourceLow = svunpklo_u16(vu8Source);
        svuint16x3_t vSourceLow16x3 = svcreate3(vSourceLow,
                                                vSourceLow,
                                                vSourceLow);
        lv_sve_pixel_ccc_foreach_chn(vSourceLow16x3, vTargetLow16x3, {
            sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                sve_source_u16,
                sve_target_u16,
                hwOpacity);
        });

        /* process high half */
        svuint16_t vSourceHigh = svunpkhi_u16(vu8Source);
        svuint16x3_t vSourceHigh16x3 = svcreate3(   vSourceHigh,
                                                    vSourceHigh,
                                                    vSourceHigh);
        lv_sve_pixel_ccc_foreach_chn(vSourceHigh16x3, vTargetHigh16x3, {
            sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                sve_source_u16,
                sve_target_u16,
                hwOpacity);
        });

        svst3ub_u16(vTailPred,
                    pchTarget,
                    vTargetLow16x3,
                    vTargetHigh16x3);

        pchTarget += sve_iteration_advance * 3;
        pchSource += sve_iteration_advance;
    }
}

lv_result_t lv_draw_sw_blend_sve2_l8_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t src_stride = dsc->src_stride;
    const int32_t dest_stride = dsc->dest_stride;
    uint8_t * dest_buf_8 = dsc->dest_buf;
    const uint8_t * src_buf_l8 = dsc->src_buf;
    uint8_t opa = dsc->opa;
    opa += opa == 255;

    if(3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_gray8_stride_blend_to_ccc888_with_opacity(   
                src_buf_l8,
                dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_l8 += src_stride;
        }
    }
    else if(4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_gray8_stride_blend_to_cccn888_with_opacity(
                src_buf_l8,
                (uint32_t *)dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_l8 += src_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_gray8_stride_blend_to_cccn888_with_mask(
    const uint8_t * LV_RESTRICT pchSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x4_t vTargetLow16x4 = svundef4_u16();
        svuint16x4_t vTargetHigh16x4 = svundef4_u16();

        svld4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    &vTargetLow16x4,
                    &vTargetHigh16x4);

        svuint8_t vu8Source = svld1_u8(vTailPred, pchSource);
        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        svuint16_t vSourceLow = svunpklo_u16(vu8Source);
        svuint16x4_t vSourceLow16x4 = svcreate4(vSourceLow,
                                                vSourceLow,
                                                vSourceLow,
                                                vSourceLow);
        lv_sve_pixel_ccca_foreach_chn012(vSourceLow16x4, vTargetLow16x4, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                sve_source_u16,
                                sve_target_u16,
                                vMaskLow);
        });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        svuint16_t vSourceHigh = svunpkhi_u16(vu8Source);
        svuint16x4_t vSourceHigh16x4 = svcreate4(   vSourceHigh,
                                                    vSourceHigh,
                                                    vSourceHigh,
                                                    vSourceHigh);
        lv_sve_pixel_ccca_foreach_chn012(vSourceHigh16x4, vTargetHigh16x4, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                sve_source_u16,
                                sve_target_u16,
                                vMaskHigh);
        });

        svst4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    vTargetLow16x4,
                    vTargetHigh16x4);

        pwTarget += sve_iteration_advance;
        pchSource += sve_iteration_advance;
        pchMask += sve_iteration_advance;
    }
}

LV_NONNULL(1)
static inline
void lv_sve_gray8_stride_blend_to_ccc888_with_mask(
    const uint8_t * LV_RESTRICT pchSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x3_t vTargetLow16x3 = svundef3_u16();
        svuint16x3_t vTargetHigh16x3 = svundef3_u16();

        svld3ub_u16(vTailPred,
                    pchTarget,
                    &vTargetLow16x3,
                    &vTargetHigh16x3);

        svuint8_t vu8Source = svld1_u8(vTailPred, pchSource);
        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        svuint16_t vSourceLow = svunpklo_u16(vu8Source);
        svuint16x3_t vSourceLow16x3 = svcreate3(vSourceLow,
                                                vSourceLow,
                                                vSourceLow);

        lv_sve_pixel_ccc_foreach_chn(vSourceLow16x3, vTargetLow16x3, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                sve_source_u16,
                                sve_target_u16,
                                vMaskLow);
        });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        svuint16_t vSourceHigh = svunpkhi_u16(vu8Source);
        svuint16x3_t vSourceHigh16x3 = svcreate3(vSourceHigh,
                                                    vSourceHigh,
                                                    vSourceHigh);

        lv_sve_pixel_ccc_foreach_chn(vSourceHigh16x3, vTargetHigh16x3, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                sve_source_u16,
                                sve_target_u16,
                                vMaskHigh);
        });

        svst3ub_u16(vTailPred,
                    pchTarget,
                    vTargetLow16x3,
                    vTargetHigh16x3);

        pchTarget += sve_iteration_advance * 3;
        pchSource += sve_iteration_advance;
        pchMask += sve_iteration_advance;
    }
}

lv_result_t lv_draw_sw_blend_sve2_l8_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t src_stride = dsc->src_stride;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t mask_stride = dsc->mask_stride;
    uint8_t * dest_buf_8 = dsc->dest_buf;
    const uint8_t * src_buf_l8 = dsc->src_buf;
    const uint8_t * mask_buf_8 = dsc->mask_buf;

    if(3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_gray8_stride_blend_to_ccc888_with_mask(  src_buf_l8,
                                                            mask_buf_8,
                                                            dest_buf_8,
                                                            w);

            dest_buf_8 += dest_stride;
            src_buf_l8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }
    else if(4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_gray8_stride_blend_to_cccn888_with_mask(
                src_buf_l8,
                mask_buf_8,
                (uint32_t *)dest_buf_8,
                w);

            dest_buf_8 += dest_stride;
            src_buf_l8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }

    return LV_RESULT_INVALID;
}

LV_NONNULL(1)
static inline
void lv_sve_gray8_stride_blend_to_cccn888_with_mask_and_opacity(
    const uint8_t * LV_RESTRICT pchSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x4_t vTargetLow16x4 = svundef4_u16();
        svuint16x4_t vTargetHigh16x4 = svundef4_u16();

        svld4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    &vTargetLow16x4,
                    &vTargetHigh16x4);

        svuint8_t vu8Source = svld1_u8(vTailPred, pchSource);
        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        svuint16_t vSourceLow = svunpklo_u16(vu8Source);
        svuint16x4_t vSourceLow16x4 = svcreate4(vSourceLow,
                                                vSourceLow,
                                                vSourceLow,
                                                vSourceLow);
        lv_sve_pixel_ccca_foreach_chn012(vSourceLow16x4, vTargetLow16x4, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                sve_source_u16,
                                sve_target_u16,
                                vMaskLow,
                                hwOpacity);
        });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        svuint16_t vSourceHigh = svunpkhi_u16(vu8Source);
        svuint16x4_t vSourceHigh16x4 = svcreate4(   vSourceHigh,
                                                    vSourceHigh,
                                                    vSourceHigh,
                                                    vSourceHigh);
        lv_sve_pixel_ccca_foreach_chn012(vSourceHigh16x4, vTargetHigh16x4, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                                    sve_source_u16,
                                                    sve_target_u16,
                                                    vMaskHigh,
                                                    hwOpacity);
        });

        svst4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    vTargetLow16x4,
                    vTargetHigh16x4);

        pwTarget += sve_iteration_advance;
        pchSource += sve_iteration_advance;
        pchMask += sve_iteration_advance;
    }
}

LV_NONNULL(1)
static inline
void lv_sve_gray8_stride_blend_to_ccc888_with_mask_and_opacity(
    const uint8_t * LV_RESTRICT pchSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x3_t vTargetLow16x3 = svundef3_u16();
        svuint16x3_t vTargetHigh16x3 = svundef3_u16();

        svld3ub_u16(vTailPred,
                    pchTarget,
                    &vTargetLow16x3,
                    &vTargetHigh16x3);

        svuint8_t vu8Source = svld1_u8(vTailPred, pchSource);
        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        svuint16_t vSourceLow = svunpklo_u16(vu8Source);
        svuint16x3_t vSourceLow16x3 = svcreate3(vSourceLow,
                                                vSourceLow,
                                                vSourceLow);

        lv_sve_pixel_ccc_foreach_chn(vSourceLow16x3, vTargetLow16x3, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                sve_source_u16,
                                sve_target_u16,
                                vMaskLow,
                                hwOpacity);
        });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        svuint16_t vSourceHigh = svunpkhi_u16(vu8Source);
        svuint16x3_t vSourceHigh16x3 = svcreate3(vSourceHigh,
                                                    vSourceHigh,
                                                    vSourceHigh);

        lv_sve_pixel_ccc_foreach_chn(vSourceHigh16x3, vTargetHigh16x3, {
            sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                sve_source_u16,
                                sve_target_u16,
                                vMaskHigh,
                                hwOpacity);
        });

        svst3ub_u16(vTailPred,
                    pchTarget,
                    vTargetLow16x3,
                    vTargetHigh16x3);

        pchTarget += sve_iteration_advance * 3;
        pchSource += sve_iteration_advance;
        pchMask += sve_iteration_advance;
    }
}

lv_result_t lv_draw_sw_blend_sve2_l8_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t src_stride = dsc->src_stride;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t mask_stride = dsc->mask_stride;
    uint8_t * dest_buf_8 = dsc->dest_buf;
    const uint8_t * src_buf_l8 = dsc->src_buf;
    const uint8_t * mask_buf_8 = dsc->mask_buf;

    uint8_t opa = dsc->opa;
    opa += opa == 255;

    if(3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_gray8_stride_blend_to_ccc888_with_mask_and_opacity(
                src_buf_l8,
                mask_buf_8,
                dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_l8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }
    else if(4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_gray8_stride_blend_to_cccn888_with_mask_and_opacity(
                src_buf_l8,
                mask_buf_8,
                (uint32_t *)dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_l8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_rgb565_stride_blend_to_cccn888(
    const uint16_t * LV_RESTRICT phwSource,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x4_t vTargetLow16x4 = svundef4_u16();
        svuint16x4_t vTargetHigh16x4 = svundef4_u16();

        svld4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    &vTargetLow16x4,
                    &vTargetHigh16x4);

        svuint16x3_t vSourceLow16x3 = svundef3_u16();
        svuint16x3_t vSourceHigh16x3 = svundef3_u16();

        svld3rgb565_u16(vTailPred, 
                        (uint16_t *)phwSource, 
                        &vSourceLow16x3, 
                        &vSourceHigh16x3);

        /* process low half */
        lv_sve_pixel_u16x3_foreach_chn_to_cccn( vSourceLow16x3, 
                                                vTargetLow16x4, 
            {
                sve_target_u16 = sve_source_u16;
            });

        /* process high half */
        lv_sve_pixel_u16x3_foreach_chn_to_cccn( vSourceHigh16x3, 
                                                vTargetHigh16x4, 
            {
                sve_target_u16 = sve_source_u16;
            });

        svst4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    vTargetLow16x4,
                    vTargetHigh16x4);

        pwTarget += sve_iteration_advance;
        phwSource += sve_iteration_advance;
    }
}

LV_NONNULL(1)
static inline
void lv_sve_rgb565_stride_blend_to_ccc888(
    const uint16_t * LV_RESTRICT phwSource,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x3_t vTargetLow16x3 = svundef3_u16();
        svuint16x3_t vTargetHigh16x3 = svundef3_u16();

        svld3ub_u16(vTailPred,
                    pchTarget,
                    &vTargetLow16x3,
                    &vTargetHigh16x3);

        svuint16x3_t vSourceLow16x3 = svundef3_u16();
        svuint16x3_t vSourceHigh16x3 = svundef3_u16();

        svld3rgb565_u16(vTailPred, 
                        (uint16_t *)phwSource, 
                        &vSourceLow16x3, 
                        &vSourceHigh16x3);

        /* process low half */
        lv_sve_pixel_ccc_foreach_chn( vSourceLow16x3, 
                                      vTargetLow16x3, 
            {
                sve_target_u16 = sve_source_u16;
            });

        /* process high half */
        lv_sve_pixel_ccc_foreach_chn(   vSourceHigh16x3, 
                                        vTargetHigh16x3, 
            {
                sve_target_u16 = sve_source_u16;
            });

        svst3ub_u16(vTailPred,
                    pchTarget,
                    vTargetLow16x3,
                    vTargetHigh16x3);

        pchTarget += sve_iteration_advance * 3;
        phwSource += sve_iteration_advance;
    }
}

lv_result_t lv_draw_sw_blend_sve2_rgb565_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, 
                                                   uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride    = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride     = dsc->src_stride;

    if(3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_rgb565_stride_blend_to_ccc888(
                (uint16_t *)src_buf_8,
                dest_buf_8,
                w);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
        }
    }
    else if(4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_rgb565_stride_blend_to_cccn888(
                (uint16_t *)src_buf_8,
                (uint32_t *)dest_buf_8,
                w);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_rgb565_stride_blend_to_cccn888_with_opacity(
    const uint16_t * LV_RESTRICT phwSource,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x4_t vTargetLow16x4 = svundef4_u16();
        svuint16x4_t vTargetHigh16x4 = svundef4_u16();

        svld4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    &vTargetLow16x4,
                    &vTargetHigh16x4);

        svuint16x3_t vSourceLow16x3 = svundef3_u16();
        svuint16x3_t vSourceHigh16x3 = svundef3_u16();

        svld3rgb565_u16(vTailPred, 
                        (uint16_t *)phwSource, 
                        &vSourceLow16x3, 
                        &vSourceHigh16x3);

        /* process low half */
        lv_sve_pixel_u16x3_foreach_chn_to_cccn( vSourceLow16x3, 
                                                vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    hwOpacity);
            });

        /* process high half */
        lv_sve_pixel_u16x3_foreach_chn_to_cccn( vSourceHigh16x3, 
                                                vTargetHigh16x4, 
        {
            sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                                                    sve_source_u16,
                                                    sve_target_u16,
                                                    hwOpacity);
        });

        svst4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    vTargetLow16x4,
                    vTargetHigh16x4);

        pwTarget += sve_iteration_advance;
        phwSource += sve_iteration_advance;
    }
}

LV_NONNULL(1)
static inline
void lv_sve_rgb565_stride_blend_to_ccc888_with_opacity(
    const uint16_t * LV_RESTRICT phwSource,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x3_t vTargetLow16x3 = svundef3_u16();
        svuint16x3_t vTargetHigh16x3 = svundef3_u16();

        svld3ub_u16(vTailPred,
                    pchTarget,
                    &vTargetLow16x3,
                    &vTargetHigh16x3);

        svuint16x3_t vSourceLow16x3 = svundef3_u16();
        svuint16x3_t vSourceHigh16x3 = svundef3_u16();

        svld3rgb565_u16(vTailPred, 
                        (uint16_t *)phwSource, 
                        &vSourceLow16x3, 
                        &vSourceHigh16x3);

        /* process low half */
        lv_sve_pixel_ccc_foreach_chn(   vSourceLow16x3, 
                                        vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    hwOpacity);
            });

        /* process high half */
        lv_sve_pixel_ccc_foreach_chn(   vSourceHigh16x3, 
                                        vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    hwOpacity);
            });

        svst3ub_u16(vTailPred,
                    pchTarget,
                    vTargetLow16x3,
                    vTargetHigh16x3);

        pchTarget += sve_iteration_advance * 3;
        phwSource += sve_iteration_advance;
    }
}

lv_result_t lv_draw_sw_blend_sve2_rgb565_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, 
                                                            uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride    = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride     = dsc->src_stride;
    uint8_t opa    = dsc->opa;
    opa += opa == 255;

    if(3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_rgb565_stride_blend_to_ccc888_with_opacity(
                (uint16_t *)src_buf_8,
                dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
        }
    }
    else if(4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_rgb565_stride_blend_to_cccn888_with_opacity(
                (uint16_t *)src_buf_8,
                (uint32_t *)dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_rgb565_stride_blend_to_cccn888_with_mask(
    const uint16_t * LV_RESTRICT phwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x4_t vTargetLow16x4 = svundef4_u16();
        svuint16x4_t vTargetHigh16x4 = svundef4_u16();

        svld4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    &vTargetLow16x4,
                    &vTargetHigh16x4);

        svuint16x3_t vSourceLow16x3 = svundef3_u16();
        svuint16x3_t vSourceHigh16x3 = svundef3_u16();

        svld3rgb565_u16(vTailPred, 
                        (uint16_t *)phwSource, 
                        &vSourceLow16x3, 
                        &vSourceHigh16x3);

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);

        lv_sve_pixel_u16x3_foreach_chn_to_cccn( vSourceLow16x3, 
                                                vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskLow);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);

        lv_sve_pixel_u16x3_foreach_chn_to_cccn( vSourceHigh16x3, 
                                                vTargetHigh16x4, 
        {
            sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                                    sve_source_u16,
                                                    sve_target_u16,
                                                    vMaskHigh);
        });

        svst4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    vTargetLow16x4,
                    vTargetHigh16x4);

        pwTarget += sve_iteration_advance;
        phwSource += sve_iteration_advance;
        pchMask += sve_iteration_advance;
    }
}

LV_NONNULL(1)
static inline
void lv_sve_rgb565_stride_blend_to_ccc888_with_mask(
    const uint16_t * LV_RESTRICT phwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x3_t vTargetLow16x3 = svundef3_u16();
        svuint16x3_t vTargetHigh16x3 = svundef3_u16();

        svld3ub_u16(vTailPred,
                    pchTarget,
                    &vTargetLow16x3,
                    &vTargetHigh16x3);

        svuint16x3_t vSourceLow16x3 = svundef3_u16();
        svuint16x3_t vSourceHigh16x3 = svundef3_u16();

        svld3rgb565_u16(vTailPred, 
                        (uint16_t *)phwSource, 
                        &vSourceLow16x3, 
                        &vSourceHigh16x3);

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);

        lv_sve_pixel_ccc_foreach_chn(   vSourceLow16x3, 
                                        vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskLow);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);

        lv_sve_pixel_ccc_foreach_chn(   vSourceHigh16x3, 
                                        vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskHigh);
            });

        svst3ub_u16(vTailPred,
                    pchTarget,
                    vTargetLow16x3,
                    vTargetHigh16x3);

        pchTarget += sve_iteration_advance * 3;
        phwSource += sve_iteration_advance;
        pchMask += sve_iteration_advance;
    }
}


lv_result_t lv_draw_sw_blend_sve2_rgb565_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc, 
                                                             uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride    = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride     = dsc->src_stride;
    const uint8_t * mask_buf_8  = dsc->mask_buf;
    const int32_t mask_stride    = dsc->mask_stride;

    if(3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_rgb565_stride_blend_to_ccc888_with_mask(
                (uint16_t *)src_buf_8,
                mask_buf_8,
                dest_buf_8,
                w);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }
    else if(4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_rgb565_stride_blend_to_cccn888_with_mask(
                (uint16_t *)src_buf_8,
                mask_buf_8,
                (uint32_t *)dest_buf_8,
                w);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_rgb565_stride_blend_to_cccn888_with_mask_and_opacity(
    const uint16_t * LV_RESTRICT phwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x4_t vTargetLow16x4 = svundef4_u16();
        svuint16x4_t vTargetHigh16x4 = svundef4_u16();

        svld4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    &vTargetLow16x4,
                    &vTargetHigh16x4);

        svuint16x3_t vSourceLow16x3 = svundef3_u16();
        svuint16x3_t vSourceHigh16x3 = svundef3_u16();

        svld3rgb565_u16(vTailPred, 
                        (uint16_t *)phwSource, 
                        &vSourceLow16x3, 
                        &vSourceHigh16x3);

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);

        lv_sve_pixel_u16x3_foreach_chn_to_cccn( vSourceLow16x3, 
                                                vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskLow,
                                    hwOpacity);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);

        lv_sve_pixel_u16x3_foreach_chn_to_cccn( vSourceHigh16x3, 
                                                vTargetHigh16x4, 
        {
            sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                                    sve_source_u16,
                                                    sve_target_u16,
                                                    vMaskHigh,
                                                    hwOpacity);
        });

        svst4ub_u16(vTailPred,
                    (uint8_t *)pwTarget,
                    vTargetLow16x4,
                    vTargetHigh16x4);

        pwTarget += sve_iteration_advance;
        phwSource += sve_iteration_advance;
        pchMask += sve_iteration_advance;
    }
}

LV_NONNULL(1)
static inline
void lv_sve_rgb565_stride_blend_to_ccc888_with_mask_and_opacity(
    const uint16_t * LV_RESTRICT phwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_stride_loop_pixel(uStride, vTailPred) {

        svuint16x3_t vTargetLow16x3 = svundef3_u16();
        svuint16x3_t vTargetHigh16x3 = svundef3_u16();

        svld3ub_u16(vTailPred,
                    pchTarget,
                    &vTargetLow16x3,
                    &vTargetHigh16x3);

        svuint16x3_t vSourceLow16x3 = svundef3_u16();
        svuint16x3_t vSourceHigh16x3 = svundef3_u16();

        svld3rgb565_u16(vTailPred, 
                        (uint16_t *)phwSource, 
                        &vSourceLow16x3, 
                        &vSourceHigh16x3);

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);

        lv_sve_pixel_ccc_foreach_chn(   vSourceLow16x3, 
                                        vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskLow,
                                    hwOpacity);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);

        lv_sve_pixel_ccc_foreach_chn(   vSourceHigh16x3, 
                                        vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskHigh,
                                    hwOpacity);
            });

        svst3ub_u16(vTailPred,
                    pchTarget,
                    vTargetLow16x3,
                    vTargetHigh16x3);

        pchTarget += sve_iteration_advance * 3;
        phwSource += sve_iteration_advance;
        pchMask += sve_iteration_advance;
    }
}

lv_result_t lv_draw_sw_blend_sve2_rgb565_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride    = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride     = dsc->src_stride;
    const uint8_t * mask_buf_8  = dsc->mask_buf;
    const int32_t mask_stride    = dsc->mask_stride;
    uint8_t opa    = dsc->opa;
    opa += opa == 255;

    if(3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_rgb565_stride_blend_to_ccc888_with_mask_and_opacity(
                (uint16_t *)src_buf_8,
                mask_buf_8,
                dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }
    else if(4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {

            lv_sve_rgb565_stride_blend_to_cccn888_with_mask_and_opacity(
                (uint16_t *)src_buf_8,
                mask_buf_8,
                (uint32_t *)dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_ccc888_stride_blend(
    const uint8_t * LV_RESTRICT pchSource,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride)
{
    lv_sve_rgb24_stride_impl(

        /* process low half */
        lv_sve_pixel_ccc_foreach_chn(   vSourceLow16x3, 
                                        vTargetLow16x3, 
            {
                sve_target_u16 = sve_source_u16;
            });

        /* process high half */
        lv_sve_pixel_ccc_foreach_chn(   vSourceHigh16x3, 
                                        vTargetHigh16x3, 
            {
                sve_target_u16 = sve_source_u16;
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_cccn888_stride_blend(
    const uint32_t * LV_RESTRICT pwSource,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride)
{
    size_t n = 0;
    size_t uAdvance = svlenu32();
    do {
        svbool_t vTailPred = svwhilelt_b32((uint32_t)n, (uint32_t)uStride);

        svst1_u32(vTailPred, pwTarget, svld1_u32(vTailPred, pwSource));

        pwTarget += uAdvance;
        pwSource += uAdvance;
        n += uAdvance;
    } while(n < uStride);
}

LV_NONNULL(1)
static inline
void lv_sve_cccn888_stride_blend_to_ccc888(
    const uint32_t * LV_RESTRICT pwSource,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride)
{
    lv_sve_rgb32_to_rgb24_stride_impl(

        /* process low half */
        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceLow16x4, 
                                            vTargetLow16x3, 
            {
                sve_target_u16 = sve_source_u16;
            });

        /* process high half */
        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceHigh16x4, 
                                            vTargetHigh16x3, 
            {
                sve_target_u16 = sve_source_u16;
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_ccc888_stride_blend_to_cccn888(
    const uint8_t * LV_RESTRICT pchSource,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride)
{
    lv_sve_rgb24_to_rgb32_stride_impl(

        /* process low half */
        lv_sve_pixel_u16x3_foreach_chn_to_cccn(   
                                            vSourceLow16x3, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = sve_source_u16;
            });

        /* process high half */
        lv_sve_pixel_u16x3_foreach_chn_to_cccn(   
                                            vSourceHigh16x3, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = sve_source_u16;
            });
    );
}

lv_result_t lv_draw_sw_blend_sve2_rgb888_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, 
                                                   uint32_t dest_px_size,
                                                   uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;

    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;

    if (dest_px_size == src_px_size) {
        if (3 == dest_px_size) {

            for(int32_t y = 0; y < h; y++) {

                lv_sve_ccc888_stride_blend(
                    src_buf_8,
                    dest_buf_8,
                    w);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
            }
        }
        else if (4 == dest_px_size) {
            for(int32_t y = 0; y < h; y++) {

                lv_sve_cccn888_stride_blend(
                    (uint32_t *)src_buf_8,
                    (uint32_t *)dest_buf_8,
                    w);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
            }
        }
    }
    else {
        if (3 == dest_px_size) {
            /* source pixel size is 4 */
            for(int32_t y = 0; y < h; y++) {

                lv_sve_cccn888_stride_blend_to_ccc888(
                    (uint32_t *)src_buf_8,
                    dest_buf_8,
                    w);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
            }
        }
        else if (4 == dest_px_size) {
            /* source pixel size is 3 */
            for(int32_t y = 0; y < h; y++) {
                lv_sve_ccc888_stride_blend_to_cccn888(
                    src_buf_8,
                    (uint32_t *)dest_buf_8,
                    w);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
            }
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_ccc888_stride_blend_with_opacity(
    const uint8_t * LV_RESTRICT pchSource,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb24_stride_impl(

        /* process low half */
        lv_sve_pixel_ccc_foreach_chn(   vSourceLow16x3, 
                                        vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    hwOpacity);
            });

        /* process high half */
        lv_sve_pixel_ccc_foreach_chn(   vSourceHigh16x3, 
                                        vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    hwOpacity);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_cccn888_stride_blend_with_opacity(
    const uint32_t * LV_RESTRICT pwSource,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb32_stride_impl(
        /* process low half */
        lv_sve_pixel_ccca_foreach_chn012(   vSourceLow16x4, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    hwOpacity);
            });

        /* process high half */
        lv_sve_pixel_ccca_foreach_chn012(   vSourceHigh16x4, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    hwOpacity);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_cccn888_stride_blend_to_ccc888_with_opacity(
    const uint32_t * LV_RESTRICT pwSource,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb32_to_rgb24_stride_impl(

        /* process low half */
        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceLow16x4, 
                                            vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    hwOpacity);
            });

        /* process high half */
        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceHigh16x4, 
                                            vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    hwOpacity);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_ccc888_stride_blend_to_cccn888_with_opacity(
    const uint8_t * LV_RESTRICT pchSource,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb24_to_rgb32_stride_impl(

        /* process low half */
        lv_sve_pixel_u16x3_foreach_chn_to_cccn(   
                                            vSourceLow16x3, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    hwOpacity);
            });

        /* process high half */
        lv_sve_pixel_u16x3_foreach_chn_to_cccn(   
                                            vSourceHigh16x3, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_opacity_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    hwOpacity);
            });
    );
}

lv_result_t lv_draw_sw_blend_sve2_rgb888_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, 
                                                            uint32_t dest_px_size,
                                                            uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;

    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;

    uint8_t opa    = dsc->opa;
    opa += opa == 255;

    if (dest_px_size == src_px_size) {
        if (3 == dest_px_size) {

            for(int32_t y = 0; y < h; y++) {

                lv_sve_ccc888_stride_blend_with_opacity(
                    src_buf_8,
                    dest_buf_8,
                    w,
                    opa);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;

            }
        }
        else if (4 == dest_px_size) {
            for(int32_t y = 0; y < h; y++) {

                lv_sve_cccn888_stride_blend_with_opacity(
                    (uint32_t *)src_buf_8,
                    (uint32_t *)dest_buf_8,
                    w,
                    opa);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
            }
        }
    }
    else {
        if (3 == dest_px_size) {
            /* source pixel size is 4 */
            for(int32_t y = 0; y < h; y++) {

                lv_sve_cccn888_stride_blend_to_ccc888_with_opacity(
                    (uint32_t *)src_buf_8,
                    dest_buf_8,
                    w,
                    opa);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
            }
        }
        else if (4 == dest_px_size) {
            /* source pixel size is 3 */
            for(int32_t y = 0; y < h; y++) {
                lv_sve_ccc888_stride_blend_to_cccn888_with_opacity(
                    src_buf_8,
                    (uint32_t *)dest_buf_8,
                    w,
                    opa);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
            }
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_ccc888_stride_blend_with_mask(
    const uint8_t * LV_RESTRICT pchSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride)
{
    lv_sve_rgb24_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);

        lv_sve_pixel_ccc_foreach_chn(   vSourceLow16x3, 
                                        vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskLow);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);

        lv_sve_pixel_ccc_foreach_chn(   vSourceHigh16x3, 
                                        vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskHigh);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_cccn888_stride_blend_with_mask(
    const uint32_t * LV_RESTRICT pwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride)
{
    lv_sve_rgb32_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);

        lv_sve_pixel_ccca_foreach_chn012(   vSourceLow16x4, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskLow);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);

        lv_sve_pixel_ccca_foreach_chn012(   vSourceHigh16x4, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskHigh);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_cccn888_stride_blend_to_ccc888_with_mask(
    const uint32_t * LV_RESTRICT pwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride)
{
    lv_sve_rgb32_to_rgb24_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);

        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceLow16x4, 
                                            vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskLow);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);

        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceHigh16x4, 
                                            vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskHigh);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_ccc888_stride_blend_to_cccn888_with_mask(
    const uint8_t * LV_RESTRICT pchSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride)
{
    lv_sve_rgb24_to_rgb32_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);

        lv_sve_pixel_u16x3_foreach_chn_to_cccn(   
                                            vSourceLow16x3, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskLow);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);

        lv_sve_pixel_u16x3_foreach_chn_to_cccn(   
                                            vSourceHigh16x3, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskHigh);
            });
    );
}

lv_result_t lv_draw_sw_blend_sve2_rgb888_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc, 
                                                             uint32_t dest_px_size,
                                                             uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;

    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;

    if (dest_px_size == src_px_size) {
        if (3 == dest_px_size) {

            for(int32_t y = 0; y < h; y++) {

                lv_sve_ccc888_stride_blend_with_mask(
                    src_buf_8,
                    mask_buf_8,
                    dest_buf_8,
                    w);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
                mask_buf_8 += mask_stride;
            }
        }
        else if (4 == dest_px_size) {
            for(int32_t y = 0; y < h; y++) {

                lv_sve_cccn888_stride_blend_with_mask(
                    (uint32_t *)src_buf_8,
                    mask_buf_8,
                    (uint32_t *)dest_buf_8,
                    w);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
                mask_buf_8 += mask_stride;
            }
        }
    }
    else {
        if (3 == dest_px_size) {
            /* source pixel size is 4 */
            for(int32_t y = 0; y < h; y++) {

                lv_sve_cccn888_stride_blend_to_ccc888_with_mask(
                    (uint32_t *)src_buf_8,
                    mask_buf_8,
                    dest_buf_8,
                    w);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
                mask_buf_8 += mask_stride;
            }
        }
        else if (4 == dest_px_size) {
            /* source pixel size is 3 */
            for(int32_t y = 0; y < h; y++) {

                lv_sve_ccc888_stride_blend_to_cccn888_with_mask(
                    src_buf_8,
                    mask_buf_8,
                    (uint32_t *)dest_buf_8,
                    w);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
                mask_buf_8 += mask_stride;
            }
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_ccc888_stride_blend_with_mask_and_opacity(
    const uint8_t * LV_RESTRICT pchSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb24_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);

        lv_sve_pixel_ccc_foreach_chn(   vSourceLow16x3, 
                                        vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskLow,
                                    hwOpacity);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);

        lv_sve_pixel_ccc_foreach_chn(   vSourceHigh16x3, 
                                        vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskHigh,
                                    hwOpacity);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_cccn888_stride_blend_with_mask_and_opacity(
    const uint32_t * LV_RESTRICT pwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb32_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);

        lv_sve_pixel_ccca_foreach_chn012(   vSourceLow16x4, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskLow,
                                    hwOpacity);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);

        lv_sve_pixel_ccca_foreach_chn012(   vSourceHigh16x4, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskHigh,
                                    hwOpacity);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_cccn888_stride_blend_to_ccc888_with_mask_and_opacity(
    const uint32_t * LV_RESTRICT pwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb32_to_rgb24_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);

        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceLow16x4, 
                                            vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskLow,
                                    hwOpacity);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);

        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceHigh16x4, 
                                            vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskHigh,
                                    hwOpacity);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_ccc888_stride_blend_to_cccn888_with_mask_and_opacity(
    const uint8_t * LV_RESTRICT pchSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb24_to_rgb32_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);

        lv_sve_pixel_u16x3_foreach_chn_to_cccn(   
                                            vSourceLow16x3, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskLow,
                                    hwOpacity);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);

        lv_sve_pixel_u16x3_foreach_chn_to_cccn(   
                                            vSourceHigh16x3, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vMaskHigh,
                                    hwOpacity);
            });
    );
}

lv_result_t lv_draw_sw_blend_sve2_rgb888_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint32_t dest_px_size, 
                                                                 uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;

    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;
    uint8_t opa    = dsc->opa;
    opa += opa == 255;

    if (dest_px_size == src_px_size) {
        if (3 == dest_px_size) {

            for(int32_t y = 0; y < h; y++) {

                lv_sve_ccc888_stride_blend_with_mask_and_opacity(
                    src_buf_8,
                    mask_buf_8,
                    dest_buf_8,
                    w,
                    opa);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
                mask_buf_8 += mask_stride;
            }
        }
        else if (4 == dest_px_size) {
            for(int32_t y = 0; y < h; y++) {

                lv_sve_cccn888_stride_blend_with_mask_and_opacity(
                    (uint32_t *)src_buf_8,
                    mask_buf_8,
                    (uint32_t *)dest_buf_8,
                    w,
                    opa);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
                mask_buf_8 += mask_stride;
            }
        }
    }
    else {
        if (3 == dest_px_size) {
            /* source pixel size is 4 */
            for(int32_t y = 0; y < h; y++) {

                lv_sve_cccn888_stride_blend_to_ccc888_with_mask_and_opacity(
                    (uint32_t *)src_buf_8,
                    mask_buf_8,
                    dest_buf_8,
                    w,
                    opa);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
                mask_buf_8 += mask_stride;
            }
        }
        else if (4 == dest_px_size) {
            /* source pixel size is 3 */
            for(int32_t y = 0; y < h; y++) {

                lv_sve_ccc888_stride_blend_to_cccn888_with_mask_and_opacity(
                    src_buf_8,
                    mask_buf_8,
                    (uint32_t *)dest_buf_8,
                    w,
                    opa);

                dest_buf_8 += dest_stride;
                src_buf_8 += src_stride;
                mask_buf_8 += mask_stride;
            }
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_stride_blend_to_ccc888(
    const uint32_t * LV_RESTRICT pwSource,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride)
{
    lv_sve_rgb32_to_rgb24_stride_impl(

        /* process low half */
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);
        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceLow16x4, 
                                            vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskLow);
            });

        /* process high half */
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);
        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceHigh16x4, 
                                            vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskHigh);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_stride_blend_to_cccn888(
    const uint32_t * LV_RESTRICT pwSource,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride)
{
    lv_sve_rgb32_stride_impl(

        /* process low half */
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);
        lv_sve_pixel_ccca_foreach_chn012(   vSourceLow16x4, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskLow);
            });

        /* process high half */
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);
        lv_sve_pixel_ccca_foreach_chn012(   vSourceHigh16x4, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_fast(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskHigh);
            });
    );
}

lv_result_t lv_draw_sw_blend_sve2_argb888_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, 
                                                    uint32_t dest_px_size)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;

    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;

    if (3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_stride_blend_to_ccc888(
                (uint32_t *)src_buf_8,
                dest_buf_8,
                w);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
        }
    }
    else if (4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_stride_blend_to_cccn888(
                (uint32_t *)src_buf_8,
                (uint32_t *)dest_buf_8,
                w);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_stride_blend_to_ccc888_with_opacity(
    const uint32_t * LV_RESTRICT pwSource,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb32_to_rgb24_stride_impl(

        /* process low half */
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);
        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceLow16x4, 
                                            vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskLow,
                                    hwOpacity);
            });

        /* process high half */
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);
        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceHigh16x4, 
                                            vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskHigh,
                                    hwOpacity);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_stride_blend_to_cccn888_with_opacity(
    const uint32_t * LV_RESTRICT pwSource,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb32_stride_impl(

        /* process low half */
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);
        lv_sve_pixel_ccca_foreach_chn012(   vSourceLow16x4, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskLow,
                                    hwOpacity);
            });

        /* process high half */
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);
        lv_sve_pixel_ccca_foreach_chn012(   vSourceHigh16x4, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskHigh,
                                    hwOpacity);
            });
    );
}

lv_result_t lv_draw_sw_blend_sve2_argb888_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, 
                                                             uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;

    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;
    uint8_t opa    = dsc->opa;
    opa += opa == 255;

    if (3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_stride_blend_to_ccc888_with_opacity(
                (uint32_t *)src_buf_8,
                dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
        }
    }
    else if (4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_stride_blend_to_cccn888_with_opacity(
                (uint32_t *)src_buf_8,
                (uint32_t *)dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_stride_blend_to_ccc888_with_mask(
    const uint32_t * LV_RESTRICT pwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride)
{
    lv_sve_rgb32_to_rgb24_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);
        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceLow16x4, 
                                            vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_masks(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskLow,
                                    vMaskLow);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);
        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceHigh16x4, 
                                            vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_masks(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskHigh,
                                    vMaskHigh);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_stride_blend_to_cccn888_with_mask(
    const uint32_t * LV_RESTRICT pwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride)
{
    lv_sve_rgb32_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);
        lv_sve_pixel_ccca_foreach_chn012(   vSourceLow16x4, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_masks(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskLow,
                                    vMaskLow);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);
        lv_sve_pixel_ccca_foreach_chn012(   vSourceHigh16x4, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_masks(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskHigh,
                                    vMaskHigh);
            });
    );
}

lv_result_t lv_draw_sw_blend_sve2_argb888_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc, 
                                                              uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;

    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;

    if (3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_stride_blend_to_ccc888_with_mask(
                (uint32_t *)src_buf_8,
                mask_buf_8,
                dest_buf_8,
                w);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }
    else if (4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_stride_blend_to_cccn888_with_mask(
                (uint32_t *)src_buf_8,
                mask_buf_8,
                (uint32_t *)dest_buf_8,
                w);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_stride_blend_to_ccc888_with_mask_and_opacity(
    const uint32_t * LV_RESTRICT pwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb32_to_rgb24_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);
        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceLow16x4, 
                                            vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_masks_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskLow,
                                    vMaskLow,
                                    hwOpacity);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);
        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceHigh16x4, 
                                            vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_masks_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskHigh,
                                    vMaskHigh,
                                    hwOpacity);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_stride_blend_to_cccn888_with_mask_and_opacity(
    const uint32_t * LV_RESTRICT pwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb32_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);
        lv_sve_pixel_ccca_foreach_chn012(   vSourceLow16x4, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_masks_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskLow,
                                    vMaskLow,
                                    hwOpacity);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);
        lv_sve_pixel_ccca_foreach_chn012(   vSourceHigh16x4, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = lv_sve_chn_blend_with_masks_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskHigh,
                                    vMaskHigh,
                                    hwOpacity);
            });
    );
}

lv_result_t lv_draw_sw_blend_sve2_argb888_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                  uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;

    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;
    uint8_t opa    = dsc->opa;
    opa += opa == 255;

    if (3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_stride_blend_to_ccc888_with_mask_and_opacity(
                (uint32_t *)src_buf_8,
                mask_buf_8,
                dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }
    else if (4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_stride_blend_to_cccn888_with_mask_and_opacity(
                (uint32_t *)src_buf_8,
                mask_buf_8,
                (uint32_t *)dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_premultiplied_stride_blend_to_ccc888(
    const uint32_t * LV_RESTRICT pwSource,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride)
{
    lv_sve_rgb32_to_rgb24_stride_impl(

        /* process low half */
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);
        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceLow16x4, 
                                            vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_mask(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskLow);
            });

        /* process high half */
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);
        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceHigh16x4, 
                                            vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_mask(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskHigh);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_premultiplied_stride_blend_to_cccn888(
    const uint32_t * LV_RESTRICT pwSource,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride)
{
    lv_sve_rgb32_stride_impl(

        /* process low half */
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);
        lv_sve_pixel_ccca_foreach_chn012(   vSourceLow16x4, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_mask(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskLow);
            });

        /* process high half */
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);
        lv_sve_pixel_ccca_foreach_chn012(   vSourceHigh16x4, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_mask(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vSoureMaskHigh);
            });
    );
}

lv_result_t lv_draw_sw_blend_sve2_argb888_premultiplied_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                    uint32_t dest_px_size)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    int32_t w                  = dsc->dest_w;
    int32_t h                  = dsc->dest_h;

    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;

    if (3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_premultiplied_stride_blend_to_ccc888(
                (uint32_t *)src_buf_8,
                dest_buf_8,
                w);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
        }
    }
    else if (4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_premultiplied_stride_blend_to_cccn888(
                (uint32_t *)src_buf_8,
                (uint32_t *)dest_buf_8,
                w);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_premultiplied_stride_blend_to_ccc888_with_opacity(
    const uint32_t * LV_RESTRICT pwSource,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb32_to_rgb24_stride_impl(

        /* process low half */
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);

        svuint16_t vReciprocalLow;
        do {
            svuint32_t vu32Low = svunpklo_u32(vSoureMaskLow);
            svuint32_t vu32High = svunpkhi_u32(vSoureMaskLow);

            vu32Low = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32Low); 
            vu32High = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32High); 

            vReciprocalLow = svuzp1_u16(svreinterpret_u16_u32(vu32Low), 
                                        svreinterpret_u16_u32(vu32High));
        } while(0);

        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceLow16x4, 
                                            vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vReciprocalLow,
                                    vSoureMaskLow,
                                    hwOpacity);
            });

        /* process high half */
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);

        svuint16_t vReciprocalHigh;
        do {
            svuint32_t vu32Low = svunpklo_u32(vSoureMaskHigh);
            svuint32_t vu32High = svunpkhi_u32(vSoureMaskHigh);

            vu32Low = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32Low); 
            vu32High = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32High); 

            vReciprocalHigh = svuzp1_u16(svreinterpret_u16_u32(vu32Low), 
                                        svreinterpret_u16_u32(vu32High));
        } while(0);

        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceHigh16x4, 
                                            vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vReciprocalHigh,
                                    vSoureMaskHigh,
                                    hwOpacity);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_premultiplied_stride_blend_to_cccn888_with_opacity(
    const uint32_t * LV_RESTRICT pwSource,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb32_stride_impl(

        /* process low half */
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);

        svuint16_t vReciprocalLow;
        do {
            svuint32_t vu32Low = svunpklo_u32(vSoureMaskLow);
            svuint32_t vu32High = svunpkhi_u32(vSoureMaskLow);

            vu32Low = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32Low); 
            vu32High = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32High); 

            vReciprocalLow = svuzp1_u16(svreinterpret_u16_u32(vu32Low), 
                                        svreinterpret_u16_u32(vu32High));
        } while(0);

        lv_sve_pixel_ccca_foreach_chn012(   vSourceLow16x4, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vReciprocalLow,
                                    vSoureMaskLow,
                                    hwOpacity);
            });

        /* process high half */
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);

        svuint16_t vReciprocalHigh;
        do {
            svuint32_t vu32Low = svunpklo_u32(vSoureMaskHigh);
            svuint32_t vu32High = svunpkhi_u32(vSoureMaskHigh);

            vu32Low = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32Low); 
            vu32High = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32High); 

            vReciprocalHigh = svuzp1_u16(svreinterpret_u16_u32(vu32Low), 
                                        svreinterpret_u16_u32(vu32High));
        } while(0);

        lv_sve_pixel_ccca_foreach_chn012(   vSourceHigh16x4, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_mask_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vReciprocalHigh,
                                    vSoureMaskHigh,
                                    hwOpacity);
            });
    );
}

lv_result_t lv_draw_sw_blend_sve2_argb888_premultiplied_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                           uint32_t dest_px_size)
{
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    int32_t w                  = dsc->dest_w;
    int32_t h                  = dsc->dest_h;

    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;
    uint8_t opa    = dsc->opa;
    opa += opa == 255;

    if (3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_premultiplied_stride_blend_to_ccc888_with_opacity(
                (uint32_t *)src_buf_8,
                dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
        }
    }
    else if (4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_premultiplied_stride_blend_to_cccn888_with_opacity(
                (uint32_t *)src_buf_8,
                (uint32_t *)dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_premultiplied_stride_blend_to_ccc888_with_mask(
    const uint32_t * LV_RESTRICT pwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride)
{
    lv_sve_rgb32_to_rgb24_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);

        svuint16_t vReciprocalLow;
        do {
            svuint32_t vu32Low = svunpklo_u32(vSoureMaskLow);
            svuint32_t vu32High = svunpkhi_u32(vSoureMaskLow);

            vu32Low = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32Low); 
            vu32High = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32High); 

            vReciprocalLow = svuzp1_u16(svreinterpret_u16_u32(vu32Low), 
                                        svreinterpret_u16_u32(vu32High));
        } while(0);

        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceLow16x4, 
                                            vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_masks(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vReciprocalLow,
                                    vSoureMaskLow,
                                    vMaskLow);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);

        svuint16_t vReciprocalHigh;
        do {
            svuint32_t vu32Low = svunpklo_u32(vSoureMaskHigh);
            svuint32_t vu32High = svunpkhi_u32(vSoureMaskHigh);

            vu32Low = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32Low); 
            vu32High = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32High); 

            vReciprocalHigh = svuzp1_u16(svreinterpret_u16_u32(vu32Low), 
                                        svreinterpret_u16_u32(vu32High));
        } while(0);

        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceHigh16x4, 
                                            vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_masks(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vReciprocalHigh,
                                    vSoureMaskHigh,
                                    vMaskHigh);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_premultiplied_stride_blend_to_cccn888_with_mask(
    const uint32_t * LV_RESTRICT pwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride)
{
    lv_sve_rgb32_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);

        svuint16_t vReciprocalLow;
        do {
            svuint32_t vu32Low = svunpklo_u32(vSoureMaskLow);
            svuint32_t vu32High = svunpkhi_u32(vSoureMaskLow);

            vu32Low = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32Low); 
            vu32High = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32High); 

            vReciprocalLow = svuzp1_u16(svreinterpret_u16_u32(vu32Low), 
                                        svreinterpret_u16_u32(vu32High));
        } while(0);

        lv_sve_pixel_ccca_foreach_chn012(   vSourceLow16x4, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_masks(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vReciprocalLow,
                                    vSoureMaskLow,
                                    vMaskLow);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);

        svuint16_t vReciprocalHigh;
        do {
            svuint32_t vu32Low = svunpklo_u32(vSoureMaskHigh);
            svuint32_t vu32High = svunpkhi_u32(vSoureMaskHigh);

            vu32Low = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32Low); 
            vu32High = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32High); 

            vReciprocalHigh = svuzp1_u16(svreinterpret_u16_u32(vu32Low), 
                                        svreinterpret_u16_u32(vu32High));
        } while(0);

        lv_sve_pixel_ccca_foreach_chn012(   vSourceHigh16x4, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_masks(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vReciprocalHigh,
                                    vSoureMaskHigh,
                                    vMaskHigh);
            });
    );
}

lv_result_t lv_draw_sw_blend_sve2_argb888_premultiplied_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                            uint32_t dest_px_size)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    int32_t w                  = dsc->dest_w;
    int32_t h                  = dsc->dest_h;
    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;
    uint8_t opa    = dsc->opa;
    opa += opa == 255;

    if (3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_premultiplied_stride_blend_to_ccc888_with_mask(
                (uint32_t *)src_buf_8,
                mask_buf_8,
                dest_buf_8,
                w);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }
    else if (4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_premultiplied_stride_blend_to_cccn888_with_mask(
                (uint32_t *)src_buf_8,
                mask_buf_8,
                (uint32_t *)dest_buf_8,
                w);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_premultiplied_stride_blend_to_ccc888_with_mask_and_opacity(
    const uint32_t * LV_RESTRICT pwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint8_t * LV_RESTRICT pchTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb32_to_rgb24_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);

        svuint16_t vReciprocalLow;
        do {
            svuint32_t vu32Low = svunpklo_u32(vSoureMaskLow);
            svuint32_t vu32High = svunpkhi_u32(vSoureMaskLow);

            vu32Low = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32Low); 
            vu32High = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32High); 

            vReciprocalLow = svuzp1_u16(svreinterpret_u16_u32(vu32Low), 
                                        svreinterpret_u16_u32(vu32High));
        } while(0);

        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceLow16x4, 
                                            vTargetLow16x3, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_masks_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vReciprocalLow,
                                    vSoureMaskLow,
                                    vMaskLow,
                                    hwOpacity);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);

        svuint16_t vReciprocalHigh;
        do {
            svuint32_t vu32Low = svunpklo_u32(vSoureMaskHigh);
            svuint32_t vu32High = svunpkhi_u32(vSoureMaskHigh);

            vu32Low = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32Low); 
            vu32High = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32High); 

            vReciprocalHigh = svuzp1_u16(svreinterpret_u16_u32(vu32Low), 
                                        svreinterpret_u16_u32(vu32High));
        } while(0);

        lv_sve_pixel_u16x4_foreach_chn012_to_ccc(   
                                            vSourceHigh16x4, 
                                            vTargetHigh16x3, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_masks_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vReciprocalHigh,
                                    vSoureMaskHigh,
                                    vMaskHigh,
                                    hwOpacity);
            });
    );
}

LV_NONNULL(1)
static inline
void lv_sve_ccca8888_premultiplied_stride_blend_to_cccn888_with_mask_and_opacity(
    const uint32_t * LV_RESTRICT pwSource,
    const uint8_t * LV_RESTRICT pchMask,
    uint32_t * LV_RESTRICT pwTarget,
    size_t uStride,
    uint16_t hwOpacity)
{
    lv_sve_rgb32_stride_impl(

        svuint8_t vu8Mask = svld1_u8(vTailPred, pchMask);
        pchMask += sve_iteration_advance;

        /* process low half */
        svuint16_t vMaskLow = svunpklo_u16(vu8Mask);
        svuint16_t vSoureMaskLow = svget4(vSourceLow16x4, 3);

        svuint16_t vReciprocalLow;
        do {
            svuint32_t vu32Low = svunpklo_u32(vSoureMaskLow);
            svuint32_t vu32High = svunpkhi_u32(vSoureMaskLow);

            vu32Low = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32Low); 
            vu32High = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32High); 

            vReciprocalLow = svuzp1_u16(svreinterpret_u16_u32(vu32Low), 
                                        svreinterpret_u16_u32(vu32High));
        } while(0);

        lv_sve_pixel_ccca_foreach_chn012(   vSourceLow16x4, 
                                            vTargetLow16x4, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_masks_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vReciprocalLow,
                                    vSoureMaskLow,
                                    vMaskLow,
                                    hwOpacity);
            });

        /* process high half */
        svuint16_t vMaskHigh = svunpkhi_u16(vu8Mask);
        svuint16_t vSoureMaskHigh = svget4(vSourceHigh16x4, 3);

        svuint16_t vReciprocalHigh;
        do {
            svuint32_t vu32Low = svunpklo_u32(vSoureMaskHigh);
            svuint32_t vu32High = svunpkhi_u32(vSoureMaskHigh);

            vu32Low = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32Low); 
            vu32High = svdiv_u32_x(svptrue_b32(), svdup_u32(0xFF00), vu32High); 

            vReciprocalHigh = svuzp1_u16(svreinterpret_u16_u32(vu32Low), 
                                        svreinterpret_u16_u32(vu32High));
        } while(0);

        lv_sve_pixel_ccca_foreach_chn012(   vSourceHigh16x4, 
                                            vTargetHigh16x4, 
            {
                sve_target_u16 = lv_sve_chn_premultiplied_blend_with_masks_and_opacity(
                                    sve_source_u16,
                                    sve_target_u16,
                                    vReciprocalHigh,
                                    vSoureMaskHigh,
                                    vMaskHigh,
                                    hwOpacity);
            });
    );
}

lv_result_t lv_draw_sw_blend_sve2_argb888_premultiplied_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                                uint32_t dest_px_size)
{
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    int32_t w                  = dsc->dest_w;
    int32_t h                  = dsc->dest_h;

    uint8_t * dest_buf_8 = dsc->dest_buf;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_8 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;
    uint8_t opa    = dsc->opa;
    opa += opa == 255;

    if (3 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_premultiplied_stride_blend_to_ccc888_with_mask_and_opacity(
                (uint32_t *)src_buf_8,
                mask_buf_8,
                dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }
    else if (4 == dest_px_size) {
        for(int32_t y = 0; y < h; y++) {
            lv_sve_ccca8888_premultiplied_stride_blend_to_cccn888_with_mask_and_opacity(
                (uint32_t *)src_buf_8,
                mask_buf_8,
                (uint32_t *)dest_buf_8,
                w,
                opa);

            dest_buf_8 += dest_stride;
            src_buf_8 += src_stride;
            mask_buf_8 += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* *INDENT-ON* */


#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_SVE2*/
