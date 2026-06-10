/**
 * @file lv_draw_sw_blend_sve2_to_rgb888.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_sve2_to_rgb888.h"
#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_SVE2

#include "../lv_draw_sw_blend_private.h"
#include LV_STDINT_INCLUDE
#include <arm_sve2.h>

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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_draw_sw_blend_sve2_color_to_rgb888(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    const int32_t dest_stride    = dsc->dest_stride;
    uint8_t * dest_buf_8      = dsc->dest_buf;

    

    return LV_RESULT_INVALID;
}
lv_result_t lv_draw_sw_blend_sve2_color_to_rgb888_with_opa(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t opa    = dsc->opa;


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
