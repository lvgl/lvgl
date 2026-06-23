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
#define lv_sve_rgb32_no_alpha_stride_impl(                    \
    ma_alpha_idx,                                             \
    ma_sve_chn_iterator,                                      \
    ...)                                                      \
    lv_sve_stride_loop_rgb32(uStride, vTailPred)              \
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


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

LV_NONNULL(1)
static inline
void lv_sve_cccn888_stride_fill_colour( uint32_t *LV_RESTRICT pwTarget,
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
    } while (n < uStride);
}

LV_NONNULL(1)
static inline
void lv_sve_ccc888_stride_fill_colour(  uint8_t *LV_RESTRICT pchTarget,
                                        size_t uStride,
                                        svuint8x3_t tPixel8x3)
{
    size_t n = 0;

    do {
        svbool_t vTailPred = svwhilelt_b8((uint32_t)n, (uint32_t)uStride);

        svst3_u8(vTailPred, pchTarget, tPixel8x3);

        pchTarget += svlenu8() * 3;
        n += svlenu8();
    } while (n < uStride);
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

    if (3 == dest_px_size) {
        svuint8x3_t vPixels = svcreate3_u8( svdup_u8(dsc->color.blue), 
                                            svdup_u8(dsc->color.green), 
                                            svdup_u8(dsc->color.red));

        for(int32_t y = 0; y < h; y++) {

            lv_sve_ccc888_stride_fill_colour(dest_buf_8, w, vPixels);

            dest_buf_8 += dest_stride;
        }
    }
    else if (4 == dest_px_size) {
        uint32_t wColour = lv_color_to_u32(dsc->color);
        for(int32_t y = 0; y < h; y++) {
            
            lv_sve_cccn888_stride_fill_colour(  (uint32_t *)dest_buf_8, 
                                                w, 
                                                wColour);

            dest_buf_8 += dest_stride;
        }
    }

    return LV_RES_OK;
}

LV_NONNULL(1)
static inline
void lv_sve_cccn888_stride_fill_colour_with_opacity( 
                                        uint32_t *LV_RESTRICT pwTarget,
                                        size_t uStride,
                                        uint32_t wColour,
                                        uint16_t hwOpacity)
{
    size_t n = 0;

    svuint32_t vColour = svdup_u32(wColour);
    do {
        svbool_t vTailPred = svwhilelt_b32((uint32_t)n, (uint32_t)uStride);

        svst1_u32(vTailPred, pwTarget, vColour);

        pwTarget += svlenu32();
        n += svlenu32();
    } while (n < uStride);
}

LV_NONNULL(1)
static inline
void lv_sve_ccc888_stride_fill_colour_with_opacity(  
                                        uint8_t *LV_RESTRICT pchTarget,
                                        size_t uStride,
                                        svuint8x3_t tPixel8x3,
                                        uint16_t hwOpacity)
{
    size_t n = 0;

    do {
        svbool_t vTailPred = svwhilelt_b8((uint32_t)n, (uint32_t)uStride);

        svst3_u8(vTailPred, pchTarget, tPixel8x3);

        pchTarget += svlenu8() * 3;
        n += svlenu8();
    } while (n < uStride);
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

    if (3 == dest_px_size) {
        svuint8x3_t vPixels = svcreate3_u8( svdup_u8(dsc->color.blue), 
                                            svdup_u8(dsc->color.green), 
                                            svdup_u8(dsc->color.red));

        for(int32_t y = 0; y < h; y++) {

            lv_sve_ccc888_stride_fill_colour_with_opacity(
                                                    dest_buf_8, 
                                                    w, 
                                                    vPixels,
                                                    opa);

            dest_buf_8 += dest_stride;
        }
    }
    else if (4 == dest_px_size) {
        uint32_t wColour = lv_color_to_u32(dsc->color);
        for(int32_t y = 0; y < h; y++) {
            
            lv_sve_cccn888_stride_fill_colour_with_opacity( 
                                                    (uint32_t *)dest_buf_8,
                                                    w, 
                                                    wColour,
                                                    opa);

            dest_buf_8 += dest_stride;
        }

        return LV_RES_OK;
    }

    return LV_RESULT_INVALID;
}

lv_result_t lv_draw_sw_blend_sve2_color_to_rgb888_with_mask(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;

    
    return LV_RESULT_INVALID;
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
    const uint8_t opa = dsc->opa;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;

    

    return LV_RESULT_INVALID;
}

#ifdef __aarch64__ /* vqtbl1q_u8 is only available on arm64 */
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

    
    return LV_RESULT_INVALID;
}
#endif

lv_result_t lv_draw_sw_blend_sve2_rgb565_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    int32_t w                    = dsc->dest_w;
    int32_t h                    = dsc->dest_h;
    uint8_t * dest_buf_u8      = dsc->dest_buf;
    int32_t dest_stride          = dsc->dest_stride;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    int32_t src_stride           = dsc->src_stride;

    
    return LV_RESULT_INVALID;
}
lv_result_t lv_draw_sw_blend_sve2_rgb565_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{

    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t opa = dsc->opa;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;

    

    return LV_RESULT_INVALID;
}
lv_result_t lv_draw_sw_blend_sve2_rgb565_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t * mask_buf_u8 = dsc->mask_buf;
    const int32_t mask_stride = dsc->mask_stride;

    

    return LV_RESULT_INVALID;
}
lv_result_t lv_draw_sw_blend_sve2_rgb565_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint32_t dest_px_size)
{
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    const int32_t dest_stride    = dsc->dest_stride;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    const int32_t src_stride     = dsc->src_stride;
    const uint8_t * mask_buf_u8  = dsc->mask_buf;
    const int32_t mask_stride    = dsc->mask_stride;
    const uint8_t opa    = dsc->opa;

    
    return LV_RESULT_INVALID;
}
lv_result_t lv_draw_sw_blend_sve2_rgb888_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size,
                                                   uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    uint8_t * dest_buf_u8 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_u8 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;

    /* Fallback to sw implementation*/
    if(src_px_size != dest_px_size) {
        return LV_RESULT_INVALID;
    }


    return LV_RESULT_INVALID;
}

lv_result_t lv_draw_sw_blend_sve2_rgb888_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size,
                                                            uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t opa = dsc->opa;

    /* Fallback to sw implementation*/
    if(dest_px_size != src_px_size) {
        return LV_RESULT_INVALID;
    }

    
    return LV_RESULT_INVALID;
}
lv_result_t lv_draw_sw_blend_sve2_rgb888_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size,
                                                             uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;

    /* Fallback to sw implementation*/
    if(dest_px_size != src_px_size) {
        return LV_RESULT_INVALID;
    }

    
    return LV_RESULT_INVALID;
}
lv_result_t lv_draw_sw_blend_sve2_rgb888_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint32_t dest_px_size, uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->src_stride;
    const uint8_t opa = dsc->opa;
    const uint8_t * mask_buf_8 = dsc->mask_buf;

    /* Fallback to sw implementation*/
    if(dest_px_size != src_px_size) {
        return LV_RESULT_INVALID;
    }

    
    return LV_RESULT_INVALID;
}

lv_result_t lv_draw_sw_blend_sve2_argb888_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    int32_t w                  = dsc->dest_w;
    int32_t h                  = dsc->dest_h;
    int32_t dest_stride        = dsc->dest_stride;
    int32_t src_stride         = dsc->src_stride;

    /* Fallback to sw implementation*/
    if(dest_px_size == 3) {
        return LV_RESULT_INVALID;
    }
    
    
    return LV_RESULT_INVALID;
}

lv_result_t lv_draw_sw_blend_sve2_argb888_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t opa = dsc->opa;

    /* Fallback to sw implementation*/
    if(dest_px_size == 3) {
        return LV_RESULT_INVALID;
    }
    
    
    return LV_RESULT_INVALID;
}
lv_result_t lv_draw_sw_blend_sve2_argb888_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;
    const int32_t mask_stride = dsc->mask_stride;

    /* Fallback to sw implementation*/
    if(dest_px_size == 3) {
        return LV_RESULT_INVALID;
    }
    
    
    return LV_RESULT_INVALID;
}
lv_result_t lv_draw_sw_blend_sve2_argb888_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                  uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;
    const int32_t mask_stride = dsc->mask_stride;

    /* Fallback to sw implementation*/
    if(dest_px_size == 3) {
        return LV_RESULT_INVALID;
    }
    
    
    return LV_RESULT_INVALID;
}

#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
lv_result_t lv_draw_sw_blend_sve2_argb888_premultiplied_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                  uint32_t dest_px_size)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    int32_t w                  = dsc->dest_w;
    int32_t h                  = dsc->dest_h;
    int32_t dest_stride        = dsc->dest_stride;
    int32_t src_stride         = dsc->src_stride;

    /* Fallback to sw implementation*/
    if(dest_px_size == 3) {
        return LV_RESULT_INVALID;
    }
    
    
    return LV_RESULT_INVALID;
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/


#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_SVE2*/
