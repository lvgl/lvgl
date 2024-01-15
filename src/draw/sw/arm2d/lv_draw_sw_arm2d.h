/**
 * @file lv_draw_sw_arm2d.h
 *
 */

#ifndef LV_DRAW_SW_ARM2D_H
#define LV_DRAW_SW_ARM2D_H

#ifdef __cplusplus
extern "C" {
#endif

/* *INDENT-OFF* */

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_ARM2D_SYNC

#define __ARM_2D_IMPL__
#include "arm_2d.h"
#include "__arm_2d_impl.h"

#if defined(__IS_COMPILER_ARM_COMPILER_5__)
#pragma diag_suppress 174,177,188,68,513,144,1296
#elif defined(__IS_COMPILER_IAR__)
#pragma diag_suppress=Pa093
#elif defined(__IS_COMPILER_GCC__)
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#endif

/*********************
 *      DEFINES
 *********************/
#ifndef LV_DRAW_SW_RGB565_SWAP
    #define LV_DRAW_SW_RGB565_SWAP(__buf_ptr, __buf_size_px)                    \
        _lv_draw_sw_rgb565_swap_helium((__buf_ptr), (__buf_size_px))
#endif

#ifndef LV_DRAW_SW_IMAGE
    #define LV_DRAW_SW_IMAGE(__transformed,                                     \
                         __cf,                                                  \
                         __src_buf,                                             \
                         __img_coords,                                          \
                         __src_stride,                                          \
                         __blend_area,                                          \
                         __draw_unit,                                           \
                         __draw_dsc)                                            \
        _lv_draw_sw_image_helium(   (__transformed),                            \
                                    (__cf),                                     \
                                    (uint8_t *)(__src_buf),                     \
                                    (__img_coords),                             \
                                    (__src_stride),                             \
                                    (__blend_area),                             \
                                    (__draw_unit),                              \
                                    (__draw_dsc))
#endif

#ifndef LV_DRAW_SW_RGB565_RECOLOR
    #define LV_DRAW_SW_RGB565_RECOLOR(__src_buf, __blend_area, __color, __opa)  \
        _lv_draw_sw_image_recolor_rgb565(   (__src_buf),                        \
                                            &(__blend_area),                    \
                                            (__color),                          \
                                            (__opa))
#endif

#ifndef LV_DRAW_SW_RGB888_RECOLOR
    #define LV_DRAW_SW_RGB888_RECOLOR(  __src_buf,                              \
                                        __blend_area,                           \
                                        __color,                                \
                                        __opa,                                  \
                                        __cf)                                   \
        _lv_draw_sw_image_recolor_rgb888(   (__src_buf),                        \
                                            &(__blend_area),                    \
                                            (__color),                          \
                                            (__opa),                            \
                                            (__cf))
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
extern void arm_2d_helper_swap_rgb16(uint16_t * phwBuffer, uint32_t wCount);

/**********************
 *      MACROS
 **********************/

#define __RECOLOUR_BEGIN()                                                      \
    do {                                                                        \
        lv_color_t *rgb_tmp_buf = NULL;                                         \
        if(draw_dsc->recolor_opa > LV_OPA_MIN) {                                \
            if(LV_COLOR_FORMAT_RGB565 == des_cf) {                              \
                rgb_tmp_buf                                                     \
                    = lv_malloc(src_w * src_h * sizeof(uint16_t));              \
                if (NULL == rgb_tmp_buf) {                                      \
                    LV_LOG_WARN(                                                \
                        "Failed to allocate memory for accelerating recolor, "  \
                        "use normal route instead.");                           \
                    break;                                                      \
                }                                                               \
                lv_memcpy(  rgb_tmp_buf,                                        \
                            src_buf,                                            \
                            src_w * src_h * sizeof(uint16_t));                  \
                arm_2d_size_t copy_size = {                                     \
                    .iWidth = src_w,                                            \
                    .iHeight = src_h,                                           \
                };                                                              \
                /* apply re-color */                                            \
                __arm_2d_impl_rgb565_colour_filling_with_opacity(               \
                    (uint16_t *)rgb_tmp_buf,                                    \
                    src_w,                                                      \
                    &copy_size,                                                 \
                    lv_color_to_u16(draw_dsc->recolor),                         \
                    draw_dsc->recolor_opa);                                     \
                                                                                \
                /* replace src_buf for the following operation */               \
                src_buf = (const uint8_t *)rgb_tmp_buf;                         \
            }                                                                   \
            else if(LV_COLOR_FORMAT_XRGB8888 == des_cf) {                       \
                rgb_tmp_buf                                                     \
                    = lv_malloc(src_w * src_h * sizeof(uint32_t));              \
                if (NULL == rgb_tmp_buf) {                                      \
                    LV_LOG_WARN(                                                \
                        "Failed to allocate memory for accelerating recolor, "  \
                        "use normal route instead.");                           \
                    break;                                                      \
                }                                                               \
                lv_memcpy(  rgb_tmp_buf,                                        \
                            src_buf,                                            \
                            src_w * src_h * sizeof(uint32_t));                  \
                arm_2d_size_t copy_size = {                                     \
                    .iWidth = src_w,                                            \
                    .iHeight = src_h,                                           \
                };                                                              \
                /* apply re-color */                                            \
                __arm_2d_impl_cccn888_colour_filling_with_opacity(              \
                    (uint32_t *)rgb_tmp_buf,                                    \
                    src_w,                                                      \
                    &copy_size,                                                 \
                    lv_color_to_u32(draw_dsc->recolor),                         \
                    draw_dsc->recolor_opa);                                     \
                                                                                \
                /* replace src_buf for the following operation */               \
                src_buf = (const uint8_t *)rgb_tmp_buf;                         \
            }                                                                   \
        }                                                                       \
        do {

#define __RECOLOUR_END()                                                        \
        } while(0);                                                             \
        if (NULL != rgb_tmp_buf) {                                              \
            lv_free(rgb_tmp_buf);                                               \
        }                                                                       \
    } while(0);

static inline lv_result_t _lv_draw_sw_rgb565_swap_helium(void * buf, uint32_t buf_size_px)
{
    arm_2d_helper_swap_rgb16((uint16_t *)buf, buf_size_px);
    return LV_RESULT_OK;
}

static inline lv_result_t _lv_draw_sw_image_helium( 
                                        bool is_transform, 
                                        lv_color_format_t src_cf,
                                        const uint8_t *src_buf,
                                        const lv_area_t * coords,
                                        int32_t src_stride,
                                        const lv_area_t * des_area,
                                        lv_draw_unit_t * draw_unit,
                                        const lv_draw_image_dsc_t * draw_dsc)
{
    lv_result_t result = LV_RESULT_INVALID;
    lv_layer_t * layer = draw_unit->target_layer;
    lv_color_format_t des_cf = layer->color_format;
    static bool arm_2d_initialized = false;

    if (!arm_2d_initialized) {
        arm_2d_initialized = true;
        arm_2d_init();
    }

    do {
        if (!is_transform) {
            break;
        }
        if(draw_dsc->scale_x != draw_dsc->scale_y) {
            break;
        }
        /* filter the unsupported colour format combination */
        if((LV_COLOR_FORMAT_RGB565 == des_cf)
        && !(  (LV_COLOR_FORMAT_RGB565 == src_cf)
           ||  (LV_COLOR_FORMAT_RGB565A8 == src_cf))) {
            break;
        }
        if((LV_COLOR_FORMAT_XRGB8888 == des_cf)
        && !(  (LV_COLOR_FORMAT_ARGB8888 == src_cf)
           ||  (LV_COLOR_FORMAT_XRGB8888 == src_cf))) {
            break;
        }

        /* ------------- prepare parameters for arm-2d APIs - BEGIN --------- */

        lv_area_t blend_area;
        if(!_lv_area_intersect(&blend_area, des_area, draw_unit->clip_area)) {
            break;
        }

        int32_t src_w = lv_area_get_width(coords);
        int32_t src_h = lv_area_get_height(coords);

        arm_2d_size_t src_size = {
            .iWidth = (int16_t)src_w,
            .iHeight = (int16_t)src_h,
        };

//        arm_2d_size_t des_size;

//        do{
//            int32_t des_w = lv_area_get_width(&blend_area);
//            int32_t des_h = lv_area_get_height(&blend_area);

//            LV_ASSERT(des_w <= INT16_MAX);
//            LV_ASSERT(des_h <= INT16_MAX);

//            des_size.iWidth = (int16_t)des_w;
//            des_size.iHeight = (int16_t)des_h;
//        } while(0);
//        
//        arm_2d_size_t copy_size = {
//            .iWidth = MIN(des_size.iWidth, src_size.iWidth),
//            .iHeight = MIN(des_size.iHeight, src_size.iHeight),
//        };
//        
//        int32_t des_stride = lv_draw_buf_width_to_stride(
//                                lv_area_get_width(&layer->buf_area), 
//                                des_cf);
//        uint8_t *des_buf_moved = (uint8_t *)lv_draw_layer_go_to_xy(
//                                            layer, 
//                                            blend_area.x1 - layer->buf_area.x1,
//                                            blend_area.y1 - layer->buf_area.y1);
        uint8_t *des_buf = (uint8_t *)lv_draw_layer_go_to_xy(layer, 0, 0);
        uint8_t opa = draw_dsc->opa;

        /* ------------- prepare parameters for arm-2d APIs - END ----------- */
        __RECOLOUR_BEGIN()

        static arm_2d_tile_t target_tile_origin;
        static arm_2d_tile_t target_tile;
        arm_2d_region_t clip_region;
        static arm_2d_region_t target_region;

        target_region = (arm_2d_region_t) {
            .tLocation = {
                .iX = (int16_t)(coords->x1 - draw_unit->clip_area->x1),
                .iY = (int16_t)(coords->y1 - draw_unit->clip_area->y1),
            },
            .tSize = src_size,
        };

        target_tile_origin = (arm_2d_tile_t) {
            .tRegion = {
                .tSize = {
                    .iWidth = (int16_t)lv_area_get_width(&layer->buf_area),
                    .iHeight = (int16_t)lv_area_get_height(&layer->buf_area),
                },
            },
            .tInfo = {
                .bIsRoot = true,
            },
            .phwBuffer = (uint16_t *)des_buf,
        };

        clip_region = (arm_2d_region_t) {
            .tLocation = {
                .iX = (int16_t)(draw_unit->clip_area->x1 - layer->buf_area.x1),
                .iY = (int16_t)(draw_unit->clip_area->y1 - layer->buf_area.y1),
            },
            .tSize = {
                .iWidth = (int16_t)lv_area_get_width(draw_unit->clip_area),
                .iHeight = (int16_t)lv_area_get_height(draw_unit->clip_area),
            },
        };

        arm_2d_tile_generate_child(&target_tile_origin,
                                   &clip_region,
                                   &target_tile,
                                   false);

        static arm_2d_tile_t source_tile;

        source_tile = (arm_2d_tile_t) {
            .tRegion = {
                .tSize = {
                    .iWidth = (int16_t)src_w,
                    .iHeight = (int16_t)src_h,
                },
            },
            .tInfo = {
                .bIsRoot = true,
            },
            .pchBuffer = (uint8_t *)src_buf,
        };

        static arm_2d_location_t source_center, target_center;
        source_center.iX = draw_dsc->pivot.x;
        source_center.iY = draw_dsc->pivot.y;
        target_center = target_region.tLocation;
        target_center.iX += draw_dsc->pivot.x;
        target_center.iY += draw_dsc->pivot.y;

        if(LV_COLOR_FORMAT_A8 == src_cf) {

            source_tile.tInfo.bHasEnforcedColour = true;
            source_tile.tInfo.tColourInfo.chScheme = ARM_2D_COLOUR_GRAY8;

            if(LV_COLOR_FORMAT_RGB565 == des_cf) {

                arm_2d_rgb565_fill_colour_with_mask_opacity_and_transform(
                    &source_tile,
                    &target_tile,
                    NULL,
                    source_center,
                    ARM_2D_ANGLE((draw_dsc->rotation / 10.0f)),
                    draw_dsc->scale_x / 256.0f,
                    lv_color_to_u16(draw_dsc->recolor),
                    opa,
                    &target_center
                    );

            }
            else if(LV_COLOR_FORMAT_XRGB8888 == des_cf) {
                arm_2d_cccn888_fill_colour_with_mask_opacity_and_transform(
                    &source_tile,
                    &target_tile,
                    NULL,
                    source_center,
                    ARM_2D_ANGLE((draw_dsc->rotation / 10.0f)),
                    draw_dsc->scale_x / 256.0f,
                    lv_color_to_int(draw_dsc->recolor),
                    opa,
                    &target_center
                    );
            }
            else {
                break;
            }

        }
        else if(LV_COLOR_FORMAT_RGB565A8 == src_cf) {
            LV_ASSERT(LV_COLOR_FORMAT_RGB565 == des_cf);

            /* mask_buf = src_buf + src_stride * src_w / header->w * src_h; */
            const uint8_t *mask_buf = src_buf + src_stride * src_h;
            int32_t mask_stride = src_stride / 2;

            static arm_2d_tile_t mask_tile;
            mask_tile = source_tile;

            mask_tile.tInfo.bHasEnforcedColour = true;
            mask_tile.tInfo.tColourInfo.chScheme = ARM_2D_COLOUR_GRAY8;
            mask_tile.pchBuffer = (uint8_t *)mask_buf;

            if(opa >= LV_OPA_MAX) {
                arm_2d_rgb565_tile_transform_with_src_mask(
                    &source_tile,
                    &mask_tile,
                    &target_tile,
                    NULL,
                    source_center,
                    ARM_2D_ANGLE((draw_dsc->rotation / 10.0f)),
                    draw_dsc->scale_x / 256.0f,
                    &target_center
                    );
            }
            else {
                arm_2d_rgb565_tile_transform_with_src_mask_and_opacity(
                    &source_tile,
                    &mask_tile,
                    &target_tile,
                    NULL,
                    source_center,
                    ARM_2D_ANGLE((draw_dsc->rotation / 10.0f)),
                    draw_dsc->scale_x / 256.0f,
                    opa,
                    &target_center
                    );
            }

        }
        else if(LV_COLOR_FORMAT_RGB565 == src_cf) {
            LV_ASSERT(LV_COLOR_FORMAT_RGB565 == des_cf);

            if(opa >= LV_OPA_MAX) {
                arm_2d_rgb565_tile_transform_only(
                    &source_tile,
                    &target_tile,
                    NULL,
                    source_center,
                    ARM_2D_ANGLE((draw_dsc->rotation / 10.0f)),
                    draw_dsc->scale_x / 256.0f,
                    &target_center
                    );
            }
            else {
                arm_2d_rgb565_tile_transform_only_with_opacity(
                    &source_tile,
                    &target_tile,
                    NULL,
                    source_center,
                    ARM_2D_ANGLE((draw_dsc->rotation / 10.0f)),
                    draw_dsc->scale_x / 256.0f,
                    opa,
                    &target_center
                    );
            }

        }
        else if(LV_COLOR_FORMAT_ARGB8888 == src_cf) {
            LV_ASSERT(LV_COLOR_FORMAT_XRGB8888 == des_cf);

            static arm_2d_tile_t mask_tile;
            mask_tile = source_tile;

            mask_tile.tInfo.bHasEnforcedColour = true;
            mask_tile.tInfo.tColourInfo.chScheme = ARM_2D_CHANNEL_8in32;
            mask_tile.pchBuffer = (uint8_t *)src_buf + 3;

            if(opa >= LV_OPA_MAX) {
                arm_2d_cccn888_tile_transform_with_src_mask(
                    &source_tile,
                    &mask_tile,
                    &target_tile,
                    NULL,
                    source_center,
                    ARM_2D_ANGLE((draw_dsc->rotation / 10.0f)),
                    draw_dsc->scale_x / 256.0f,
                    &target_center
                    );
            }
            else {
                arm_2d_cccn888_tile_transform_with_src_mask_and_opacity(
                    &source_tile,
                    &mask_tile,
                    &target_tile,
                    NULL,
                    source_center,
                    ARM_2D_ANGLE((draw_dsc->rotation / 10.0f)),
                    draw_dsc->scale_x / 256.0f,
                    opa,
                    &target_center
                    );
            }

        }
        else if(LV_COLOR_FORMAT_XRGB8888 == src_cf) {
            LV_ASSERT(LV_COLOR_FORMAT_XRGB8888 == des_cf);

            if(opa >= LV_OPA_MAX) {
                arm_2d_cccn888_tile_transform_only(
                    &source_tile,
                    &target_tile,
                    NULL,
                    source_center,
                    ARM_2D_ANGLE((draw_dsc->rotation / 10.0f)),
                    draw_dsc->scale_x / 256.0f,
                    &target_center
                    );
            }
            else {
                arm_2d_cccn888_tile_transform_only_with_opacity(
                    &source_tile,
                    &target_tile,
                    NULL,
                    source_center,
                    ARM_2D_ANGLE((draw_dsc->rotation / 10.0f)),
                    draw_dsc->scale_x / 256.0f,
                    opa,
                    &target_center
                    );
            }

        }
        else {
            break;
        }

        result = LV_RESULT_OK;

        __RECOLOUR_END()
    } while(0);

    return result;
}

static inline lv_result_t _lv_draw_sw_image_recolor_rgb565(
                                                const uint8_t *src_buf,
                                                const lv_area_t * blend_area,
                                                lv_color_t color,
                                                lv_opa_t opa)
{
    int32_t src_w = lv_area_get_width(blend_area);
    int32_t src_h = lv_area_get_height(blend_area);

    arm_2d_size_t copy_size = {
        .iWidth = (int16_t)src_w,
        .iHeight = (int16_t)src_h,
    };

    __arm_2d_impl_rgb565_colour_filling_with_opacity(
                    (uint16_t *)src_buf,
                    src_w,
                    &copy_size,
                    lv_color_to_u16(color),
                    opa);

    return LV_RESULT_OK;
}

static inline lv_result_t _lv_draw_sw_image_recolor_rgb888(
                                                const uint8_t *src_buf,
                                                const lv_area_t * blend_area,
                                                lv_color_t color,
                                                lv_opa_t opa,
                                                lv_color_format_t src_cf)
{
    if(LV_COLOR_FORMAT_XRGB8888 != src_cf) {
        return LV_RESULT_INVALID;
    }

    int32_t src_w = lv_area_get_width(blend_area);
    int32_t src_h = lv_area_get_height(blend_area);

    arm_2d_size_t copy_size = {
        .iWidth = (int16_t)src_w,
        .iHeight = (int16_t)src_h,
    };

    __arm_2d_impl_cccn888_colour_filling_with_opacity(
                    (uint32_t *)src_buf,
                    src_w,
                    &copy_size,
                    lv_color_to_u32(color),
                    opa);

    return LV_RESULT_OK;
}

#endif /* LV_USE_DRAW_ARM2D_SYNC */

/* *INDENT-ON* */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_ARM2D_H */
