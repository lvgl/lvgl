/**
 * @file lv_gpu_arm2d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#if defined(__clang__)
    #pragma clang diagnostic ignored "-Wunknown-warning-option"
    #pragma clang diagnostic ignored "-Wreserved-identifier"
    #pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
    #pragma clang diagnostic ignored "-Wmissing-variable-declarations"
    #pragma clang diagnostic ignored "-Wcast-qual"
    #pragma clang diagnostic ignored "-Wcast-align"
    #pragma clang diagnostic ignored "-Wextra-semi-stmt"
    #pragma clang diagnostic ignored "-Wsign-conversion"
    #pragma clang diagnostic ignored "-Wunused-function"
    #pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
    #pragma clang diagnostic ignored "-Wdouble-promotion"
    #pragma clang diagnostic ignored "-Wunused-parameter"
    #pragma clang diagnostic ignored "-Wimplicit-float-conversion"
    #pragma clang diagnostic ignored "-Wimplicit-int-conversion"
    #pragma clang diagnostic ignored "-Wtautological-pointer-compare"
    #pragma clang diagnostic ignored "-Wsign-compare"
    #pragma clang diagnostic ignored "-Wfloat-conversion"
    #pragma clang diagnostic ignored "-Wmissing-prototypes"
    #pragma clang diagnostic ignored "-Wpadded"
    #pragma clang diagnostic ignored "-Wundef"
    #pragma clang diagnostic ignored "-Wdeclaration-after-statement"
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    #pragma clang diagnostic ignored "-Wunused-variable"
    #pragma clang diagnostic ignored "-Wunused-but-set-variable"
    #pragma clang diagnostic ignored "-Wint-conversion"
#endif


#include "lv_gpu_arm2d.h"
#include "../../core/lv_refr.h"

#if LV_USE_GPU_ARM2D
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
#if (   !defined(__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__)                  \
    ||  !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__)                          \
&&  LV_COLOR_DEPTH == 32                                                        \
&&  !defined(__ARM_2D_LVGL_CFG_NO_WARNING__)
#warning Please set macro __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__ to 1 to get more acceleration opportunities. Or you can define macro __ARM_2D_LVGL_CFG_NO_WARNING__ to suppress this warning.
#endif

#define MAX_BUF_SIZE (uint32_t) lv_disp_get_hor_res(_lv_refr_get_disp_refreshing())

#if LV_COLOR_DEPTH == 16
#define arm_2d_fill_colour              arm_2d_rgb16_fill_colour
#define arm_2d_fill_colour_with_alpha   arm_2d_rgb565_fill_colour_with_alpha
#define arm_2d_fill_colour_with_mask    arm_2d_rgb565_fill_colour_with_mask
#define arm_2d_fill_colour_with_mask_and_opacity                                \
    arm_2d_rgb565_fill_colour_with_mask_and_opacity
#define arm_2d_tile_copy                arm_2d_rgb16_tile_copy
#define arm_2d_alpha_blending           arm_2d_rgb565_alpha_blending
#define arm_2d_tile_copy_with_src_mask  arm_2d_rgb565_tile_copy_with_src_mask
#define arm_2d_color_t                  arm_2d_color_rgb565_t

/* arm-2d direct mode apis */
#define __arm_2d_impl_colour_filling    __arm_2d_impl_rgb16_colour_filling
#define __arm_2d_impl_colour_filling_with_opacity                               \
    __arm_2d_impl_rgb565_colour_filling_with_opacity
#define __arm_2d_impl_colour_filling_mask                                       \
    __arm_2d_impl_rgb565_colour_filling_mask
#define __arm_2d_impl_colour_filling_mask_opacity                               \
    __arm_2d_impl_rgb565_colour_filling_mask_opacity
#define __arm_2d_impl_copy              __arm_2d_impl_rgb16_copy
#define __arm_2d_impl_alpha_blending    __arm_2d_impl_rgb565_alpha_blending
#define __arm_2d_impl_src_msk_copy      __arm_2d_impl_rgb565_src_msk_copy
#define __arm_2d_impl_src_chn_msk_copy  __arm_2d_impl_rgb565_src_chn_msk_copy
#define __arm_2d_impl_cl_key_copy       __arm_2d_impl_rgb16_cl_key_copy
#define __arm_2d_impl_alpha_blending_colour_keying                              \
    __arm_2d_impl_rgb565_alpha_blending_colour_keying
#define arm_2d_tile_transform_with_src_mask_and_opacity                         \
    arm_2d_rgb565_tile_transform_with_src_mask_and_opacity
#define arm_2d_tile_transform_with_opacity                                      \
    arm_2d_rgb565_tile_transform_with_opacity

#define __ARM_2D_PIXEL_BLENDING_OPA     __ARM_2D_PIXEL_BLENDING_OPA_RGB565

#define color_int                       uint16_t

#elif LV_COLOR_DEPTH == 32
#define arm_2d_fill_colour              arm_2d_rgb32_fill_colour
#define arm_2d_fill_colour_with_alpha   arm_2d_cccn888_fill_colour_with_alpha
#define arm_2d_fill_colour_with_mask    arm_2d_cccn888_fill_colour_with_mask
#define arm_2d_fill_colour_with_mask_and_opacity                                \
    arm_2d_cccn888_fill_colour_with_mask_and_opacity
#define arm_2d_tile_copy                arm_2d_rgb32_tile_copy
#define arm_2d_alpha_blending           arm_2d_cccn888_alpha_blending
#define arm_2d_tile_copy_with_src_mask  arm_2d_cccn888_tile_copy_with_src_mask
#define arm_2d_color_t                  arm_2d_color_cccn888_t

/* arm-2d direct mode apis */
#define __arm_2d_impl_colour_filling    __arm_2d_impl_rgb32_colour_filling
#define __arm_2d_impl_colour_filling_with_opacity                               \
    __arm_2d_impl_cccn888_colour_filling_with_opacity
#define __arm_2d_impl_colour_filling_mask                                       \
    __arm_2d_impl_cccn888_colour_filling_mask
#define __arm_2d_impl_colour_filling_mask_opacity                               \
    __arm_2d_impl_cccn888_colour_filling_mask_opacity
#define __arm_2d_impl_copy              __arm_2d_impl_rgb32_copy
#define __arm_2d_impl_alpha_blending    __arm_2d_impl_cccn888_alpha_blending
#define __arm_2d_impl_src_msk_copy      __arm_2d_impl_cccn888_src_msk_copy
#define __arm_2d_impl_src_chn_msk_copy  __arm_2d_impl_cccn888_src_chn_msk_copy
#define __arm_2d_impl_cl_key_copy       __arm_2d_impl_rgb32_cl_key_copy
#define __arm_2d_impl_alpha_blending_colour_keying                              \
    __arm_2d_impl_cccn888_alpha_blending_colour_keying
#define arm_2d_tile_transform_with_src_mask_and_opacity                         \
    arm_2d_cccn888_tile_transform_with_src_mask_and_opacity
#define arm_2d_tile_transform_with_opacity                                      \
    arm_2d_cccn888_tile_transform_with_opacity

#define __ARM_2D_PIXEL_BLENDING_OPA     __ARM_2D_PIXEL_BLENDING_OPA_CCCN888

#define color_int                       uint32_t

#else
#error The specified LV_COLOR_DEPTH is not supported by this version of lv_gpu_arm2d.c.
#endif

/* *INDENT-OFF* */
#define __PREPARE_LL_ACCELERATION__()                                           \
            int32_t src_stride = lv_area_get_width(coords);                     \
                                                                                \
            uint8_t px_size_byte = cf == LV_IMG_CF_TRUE_COLOR_ALPHA             \
                                      ?  LV_IMG_PX_SIZE_ALPHA_BYTE              \
                                      :  sizeof(lv_color_t);                    \
                                                                                \
            const uint8_t * src_buf_tmp = src_buf;                              \
            src_buf_tmp += src_stride                                           \
                         * (draw_area.y1 - coords->y1)                          \
                         * px_size_byte;                                        \
            src_buf_tmp += (draw_area.x1 - coords->x1) * px_size_byte;          \
                                                                                \
            lv_area_t blend_area2;                                              \
            if(!_lv_area_intersect(&blend_area2,                                \
                                   &draw_area,                                  \
                                   draw_ctx->clip_area)) return;                \
                                                                                \
            int32_t w = lv_area_get_width(&blend_area2);                        \
            int32_t h = lv_area_get_height(&blend_area2);                       \
                                                                                \
            lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);     \
                                                                                \
            lv_color_t * dest_buf = draw_ctx->buf;                              \
            dest_buf += dest_stride * (blend_area2.y1 - draw_ctx->buf_area->y1) \
                        + (blend_area2.x1 - draw_ctx->buf_area->x1);            \
                                                                                \
            arm_2d_size_t copy_size = {                                         \
                .iWidth = lv_area_get_width(&blend_area2),                      \
                .iHeight = lv_area_get_height(&blend_area2),                    \
            }

#define __PREPARE_TARGET_TILE__(__blend_area)                                   \
        static arm_2d_tile_t target_tile;                                       \
        static arm_2d_region_t target_region;                                   \
                                                                                \
        lv_color_t * dest_buf = draw_ctx->buf;                                  \
                                                                                \
        target_tile = (arm_2d_tile_t) {                                         \
            .tRegion = {                                                        \
                .tSize = {                                                      \
                    .iWidth = lv_area_get_width(draw_ctx->buf_area),            \
                    .iHeight = lv_area_get_height(draw_ctx->buf_area),          \
                },                                                              \
            },                                                                  \
            .tInfo.bIsRoot = true,                                              \
            .phwBuffer = (uint16_t *)draw_ctx->buf,                             \
        };                                                                      \
                                                                                \
        target_region = (arm_2d_region_t) {                                     \
            .tLocation = {                                                      \
                .iX = (__blend_area).x1 - draw_ctx->buf_area->x1,               \
                .iY = (__blend_area).y1 - draw_ctx->buf_area->y1,               \
            },                                                                  \
            .tSize = {                                                          \
                .iWidth = lv_area_get_width(&(__blend_area)),                   \
                .iHeight = lv_area_get_height(&(__blend_area)),                 \
            },                                                                  \
        }

#define __PREPARE_SOURCE_TILE__(__dsc, __blend_area)                            \
        static arm_2d_tile_t source_tile_orig;                                  \
        static arm_2d_tile_t source_tile;                                       \
        const lv_color_t * src_buf = (__dsc)->src_buf;                          \
        if (src_buf) {                                                          \
            source_tile_orig = (arm_2d_tile_t) {                                \
                .tRegion = {                                                    \
                    .tSize = {                                                  \
                        .iWidth = lv_area_get_width((__dsc)->blend_area),       \
                        .iHeight = lv_area_get_height((__dsc)->blend_area),     \
                    },                                                          \
                },                                                              \
                .tInfo.bIsRoot = true,                                          \
                .phwBuffer = (uint16_t *)src_buf,                               \
            };                                                                  \
                                                                                \
            arm_2d_tile_generate_child(                                         \
                &source_tile_orig,                                              \
                (arm_2d_region_t []) {                                          \
                    {                                                           \
                        .tLocation = {                                          \
                            .iX = (__blend_area).x1 - (__dsc)->blend_area->x1,  \
                            .iY = (__blend_area).y1 - (__dsc)->blend_area->y1,  \
                        },                                                      \
                        .tSize = source_tile_orig.tRegion.tSize,                \
                    }                                                           \
                },                                                              \
                &source_tile,                                                   \
                false);                                                         \
            source_tile.tInfo.bDerivedResource = true;                          \
        }

#define __PREPARE_MASK_TILE__(__dsc, __blend_area, __mask, __is_chn)            \
        static arm_2d_tile_t mask_tile_orig;                                    \
        static arm_2d_tile_t mask_tile;                                         \
        if(NULL != (__mask)) {                                                  \
            mask_tile_orig = (arm_2d_tile_t) {                                  \
                .tRegion = {                                                    \
                    .tSize = {                                                  \
                        .iWidth = lv_area_get_width((__dsc)->mask_area),        \
                        .iHeight = lv_area_get_height((__dsc)->mask_area),      \
                    },                                                          \
                },                                                              \
                .tInfo = {                                                      \
                    .bIsRoot = true,                                            \
                    .bHasEnforcedColour = true,                                 \
                    .tColourInfo = {                                            \
                        .chScheme = (__is_chn) ? ARM_2D_CHANNEL_8in32           \
                                               : ARM_2D_COLOUR_8BIT,            \
                    },                                                          \
                },                                                              \
                .pchBuffer = ((uint8_t *)(__mask)) + (__is_chn) ? 3 : 0,        \
            };                                                                  \
                                                                                \
            arm_2d_tile_generate_child(                                         \
                &mask_tile_orig,                                                \
                (arm_2d_region_t []) {                                          \
                    {                                                           \
                        .tLocation = {                                          \
                            .iX = (__dsc)->mask_area->x1 - (__blend_area).x1,   \
                            .iY = (__dsc)->mask_area->y1 - (__blend_area).y1,   \
                        },                                                      \
                        .tSize = mask_tile_orig.tRegion.tSize,                  \
                    }                                                           \
                },                                                              \
                &mask_tile,                                                     \
                false);                                                         \
            mask_tile.tInfo.bDerivedResource = true;                            \
        }
/* *INDENT-ON* */

/* *INDENT-OFF* */
#define __RECOLOUR_WRAPPER(...)                                                 \
    do {                                                                        \
        lv_color_t *rgb_tmp_buf = NULL;                                         \
        if(draw_dsc->recolor_opa > LV_OPA_MIN) {                                \
            rgb_tmp_buf                                                         \
                = lv_malloc(src_w * src_h * sizeof(lv_color_t));           \
            if (NULL == rgb_tmp_buf) {                                          \
                LV_LOG_WARN(                                                    \
                    "Failed to allocate memory for accelerating recolour, "     \
                    "use normal route instead.");                               \
                break;                                                          \
            }                                                                   \
            lv_memcpy(rgb_tmp_buf, src_buf, src_w * src_h * sizeof(lv_color_t));\
            arm_2d_size_t copy_size = {                                         \
                .iWidth = src_w,                                                \
                .iHeight = src_h,                                               \
            };                                                                  \
            /* apply re-colour */                                               \
            __arm_2d_impl_colour_filling_with_opacity(                          \
                (color_int *)rgb_tmp_buf,                                       \
                src_w,                                                          \
                &copy_size,                                                     \
                (color_int)draw_dsc->recolor.full,                              \
                draw_dsc->recolor_opa);                                         \
                                                                                \
            /* replace src_buf for the following operation */                   \
            src_buf = (const uint8_t *)rgb_tmp_buf;                             \
        }                                                                       \
        __VA_ARGS__                                                             \
        if (NULL != rgb_tmp_buf) {                                              \
            lv_free(rgb_tmp_buf);                                    \
        }                                                                       \
    } while(0);
/* *INDENT-ON* */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if __ARM_2D_HAS_HW_ACC__
LV_ATTRIBUTE_FAST_MEM
static bool lv_draw_arm2d_fill_colour(const arm_2d_tile_t * target_tile,
                                      const arm_2d_region_t * region,
                                      lv_color_t color,
                                      lv_opa_t opa,
                                      const arm_2d_tile_t * mask_tile);

LV_ATTRIBUTE_FAST_MEM
static bool lv_draw_arm2d_tile_copy(const arm_2d_tile_t * target_tile,
                                    const arm_2d_region_t * region,
                                    arm_2d_tile_t * source_tile,
                                    lv_opa_t opa,
                                    arm_2d_tile_t * mask_tile);
#else

static void convert_cb(const lv_area_t * dest_area,
                       const void * src_buf,
                       lv_coord_t src_w,
                       lv_coord_t src_h,
                       lv_coord_t src_stride,
                       const lv_draw_img_dsc_t * draw_dsc,
                       lv_img_cf_t cf,
                       lv_color_t * cbuf,
                       lv_opa_t * abuf);

LV_ATTRIBUTE_FAST_MEM
static bool arm_2d_fill_normal(lv_color_t * dest_buf,
                               const lv_area_t * dest_area,
                               lv_coord_t dest_stride,
                               lv_color_t color,
                               lv_opa_t opa,
                               const lv_opa_t * mask,
                               lv_coord_t mask_stride);

LV_ATTRIBUTE_FAST_MEM
static bool arm_2d_copy_normal(lv_color_t * dest_buf,
                               const lv_area_t * dest_area,
                               lv_coord_t dest_stride,
                               const lv_color_t * src_buf,
                               lv_coord_t src_stride,
                               lv_opa_t opa,
                               const lv_opa_t * mask,
                               lv_coord_t mask_stride);
#endif

LV_ATTRIBUTE_FAST_MEM
static void lv_draw_arm2d_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);
LV_ATTRIBUTE_FAST_MEM
static void lv_gpu_arm2d_wait_cb(lv_draw_ctx_t * draw_ctx);
LV_ATTRIBUTE_FAST_MEM
static void lv_draw_arm2d_img_decoded(struct _lv_draw_ctx_t * draw_ctx,
                                      const lv_draw_img_dsc_t * draw_dsc,
                                      const lv_area_t * coords,
                                      const uint8_t * src_buf,
                                      lv_img_cf_t cf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_arm2d_ctx_init(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    arm_2d_init();

    lv_draw_sw_init_ctx(drv, draw_ctx);

    lv_draw_arm2d_ctx_t * arm2d_draw_ctx = (lv_draw_sw_ctx_t *)draw_ctx;

    arm2d_draw_ctx->blend = lv_draw_arm2d_blend;
    arm2d_draw_ctx->base_draw.wait_for_finish = lv_gpu_arm2d_wait_cb;

#if !__ARM_2D_HAS_HW_ACC__
    arm2d_draw_ctx->base_draw.draw_img_decoded = lv_draw_arm2d_img_decoded;
#endif

}

void lv_draw_arm2d_ctx_deinit(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    LV_UNUSED(drv);
    LV_UNUSED(draw_ctx);
}

extern void test_flush(lv_color_t * color_p);

#if __ARM_2D_HAS_HW_ACC__
LV_ATTRIBUTE_FAST_MEM
static void lv_draw_arm2d_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    const lv_opa_t * mask;
    if(dsc->mask_buf == NULL) mask = NULL;
    if(dsc->mask_buf && dsc->mask_res == LV_DRAW_MASK_RES_TRANSP) return;
    else if(dsc->mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask = NULL;
    else mask = dsc->mask_buf;


    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area)) {
        return;
    }

    bool is_accelerated = false;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL
       &&    lv_area_get_size(&blend_area) > 100) {

        __PREPARE_TARGET_TILE__(blend_area);
        __PREPARE_SOURCE_TILE__(dsc, blend_area);
        __PREPARE_MASK_TILE__(dsc, blend_area, mask, false);

        if(src_buf) {
            is_accelerated = lv_draw_arm2d_tile_copy(
                                 &target_tile,
                                 &target_region,
                                 &source_tile,
                                 dsc->opa,
                                 (NULL == mask) ? NULL : &mask_tile);
        }
        else {
            is_accelerated = lv_draw_arm2d_fill_colour(
                                 &target_tile,
                                 &target_region,
                                 dsc->color,
                                 dsc->opa,
                                 (NULL == mask) ? NULL : &mask_tile);
        }
    }

    if(!is_accelerated) {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
    }
}


LV_ATTRIBUTE_FAST_MEM
static bool lv_draw_arm2d_fill_colour(const arm_2d_tile_t * target_tile,
                                      const arm_2d_region_t * region,
                                      lv_color_t color,
                                      lv_opa_t opa,
                                      const arm_2d_tile_t * mask_tile)
{
    arm_fsm_rt_t result = (arm_fsm_rt_t)ARM_2D_ERR_NONE;

    if(NULL == mask_tile) {
        if(opa >= LV_OPA_MAX) {
            result = arm_2d_fill_colour(target_tile, region, color.full);
        }
        else {
#if LV_COLOR_SCREEN_TRANSP
            return false;
#else
            result = arm_2d_fill_colour_with_alpha(
                         target_tile,
                         region,
            (arm_2d_color_t) {
                color.full
            },
            opa);
#endif
        }
    }
    else {

        if(opa >= LV_OPA_MAX) {
            result = arm_2d_fill_colour_with_mask(
                         target_tile,
                         region,
                         mask_tile,
            (arm_2d_color_t) {
                color.full
            });
        }
        else {
#if LV_COLOR_SCREEN_TRANSP
            return false;
#else
            result = arm_2d_fill_colour_with_mask_and_opacity(
                         target_tile,
                         region,
                         mask_tile,
            (arm_2d_color_t) {
                color.full
            },
            opa);
#endif
        }
    }

    if(result < 0) {
        /* error detected */
        return false;
    }

    return true;

}

LV_ATTRIBUTE_FAST_MEM
static bool lv_draw_arm2d_tile_copy(const arm_2d_tile_t * target_tile,
                                    const arm_2d_region_t * region,
                                    arm_2d_tile_t * source_tile,
                                    lv_opa_t opa,
                                    arm_2d_tile_t * mask_tile)
{
    arm_fsm_rt_t result = (arm_fsm_rt_t)ARM_2D_ERR_NONE;

    if(NULL == mask_tile) {
        if(opa >= LV_OPA_MAX) {
            result = arm_2d_tile_copy(source_tile,
                                      target_tile,
                                      region,
                                      ARM_2D_CP_MODE_COPY);
        }
#if LV_COLOR_SCREEN_TRANSP
        else {
            return false;  /* not supported */
        }
#else
        else {
            result = arm_2d_alpha_blending(source_tile,
                                           target_tile,
                                           region,
                                           opa);
        }
#endif
    }
    else {
#if LV_COLOR_SCREEN_TRANSP
        return false;       /* not support */
#else

        if(opa >= LV_OPA_MAX) {
            result = arm_2d_tile_copy_with_src_mask(source_tile,
                                                    mask_tile,
                                                    target_tile,
                                                    region,
                                                    ARM_2D_CP_MODE_COPY);
        }
        else {
            return false;
        }
#endif
    }

    if(result < 0) {
        /* error detected */
        return false;
    }

    return true;
}

static void lv_gpu_arm2d_wait_cb(lv_draw_ctx_t * draw_ctx)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();

    arm_2d_op_wait_async(NULL);
    if(disp->driver && disp->driver->wait_cb) {
        disp->driver->wait_cb(disp->driver);
    }
    lv_draw_sw_wait_for_finish(draw_ctx);
}
#else


LV_ATTRIBUTE_FAST_MEM
static void lv_draw_arm2d_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    const lv_opa_t * mask;
    if(dsc->mask_buf == NULL) mask = NULL;
    if(dsc->mask_buf && dsc->mask_res == LV_DRAW_MASK_RES_TRANSP) return;
    else if(dsc->mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask = NULL;
    else mask = dsc->mask_buf;

    lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);

    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area)) return;

    lv_disp_t * disp = _lv_refr_get_disp_refreshing();

    bool is_accelerated = false;
    do {
        if(NULL != disp->driver->set_px_cb) {
            break;
        }

        lv_color_t * dest_buf = draw_ctx->buf;
        dest_buf += dest_stride * (blend_area.y1 - draw_ctx->buf_area->y1)
                    + (blend_area.x1 - draw_ctx->buf_area->x1);

        const lv_color_t * src_buf = dsc->src_buf;
        lv_coord_t src_stride;
        if(src_buf) {
            src_stride = lv_area_get_width(dsc->blend_area);
            src_buf += src_stride * (blend_area.y1 - dsc->blend_area->y1) + (blend_area.x1 - dsc->blend_area->x1);
        }
        else {
            src_stride = 0;
        }

        lv_coord_t mask_stride;
        if(mask) {
            mask_stride = lv_area_get_width(dsc->mask_area);
            mask += mask_stride * (blend_area.y1 - dsc->mask_area->y1) + (blend_area.x1 - dsc->mask_area->x1);
        }
        else {
            mask_stride = 0;
        }

        lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);


        if(dsc->src_buf == NULL) {
            if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
                is_accelerated = arm_2d_fill_normal(dest_buf,
                                                    &blend_area,
                                                    dest_stride,
                                                    dsc->color,
                                                    dsc->opa,
                                                    mask,
                                                    mask_stride);
            }
#if LV_USE_DRAW_MASKS
            else {
                break;
            }
#endif
        }
        else {

            if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
                is_accelerated = arm_2d_copy_normal(dest_buf,
                                                    &blend_area,
                                                    dest_stride,
                                                    src_buf,
                                                    src_stride,
                                                    dsc->opa,
                                                    mask,
                                                    mask_stride);
            }
#if LV_USE_DRAW_MASKS
            else {
                break;
            }
#endif
        }
    } while(0);

    if(!is_accelerated) lv_draw_sw_blend_basic(draw_ctx, dsc);
}

LV_ATTRIBUTE_FAST_MEM
static bool arm_2d_fill_normal(lv_color_t * dest_buf,
                               const lv_area_t * dest_area,
                               lv_coord_t dest_stride,
                               lv_color_t color,
                               lv_opa_t opa,
                               const lv_opa_t * mask,
                               lv_coord_t mask_stride)
{
    arm_2d_size_t target_size = {
        .iWidth = lv_area_get_width(dest_area),
        .iHeight = lv_area_get_height(dest_area),
    };

    /*No mask*/
    if(mask == NULL) {
        if(opa >= LV_OPA_MAX) {
            __arm_2d_impl_colour_filling((color_int *)dest_buf,
                                         dest_stride,
                                         &target_size,
                                         color.full);
        }
        /*Has opacity*/
        else {
#if LV_COLOR_SCREEN_TRANSP
            return false;
#else
            __arm_2d_impl_colour_filling_with_opacity((color_int *)dest_buf,
                                                      dest_stride,
                                                      &target_size,
                                                      color.full,
                                                      opa);
#endif
        }
    }
    /*Masked*/
    else {
        /*Only the mask matters*/
        if(opa >= LV_OPA_MAX) {
            __arm_2d_impl_colour_filling_mask((color_int *)dest_buf,
                                              dest_stride,
                                              (uint8_t *)mask,
                                              mask_stride,
                                              &target_size,
                                              color.full);
        }
        /*With opacity*/
        else {
#if LV_COLOR_SCREEN_TRANSP
            return false;
#else
            __arm_2d_impl_colour_filling_mask_opacity((color_int *)dest_buf,
                                                      dest_stride,
                                                      (uint8_t *)mask,
                                                      mask_stride,
                                                      &target_size,
                                                      color.full,
                                                      opa);
#endif
        }
    }

    return true;
}


LV_ATTRIBUTE_FAST_MEM
static bool arm_2d_copy_normal(lv_color_t * dest_buf,
                               const lv_area_t * dest_area,
                               lv_coord_t dest_stride,
                               const lv_color_t * src_buf,
                               lv_coord_t src_stride,
                               lv_opa_t opa,
                               const lv_opa_t * mask,
                               lv_coord_t mask_stride)

{
    int32_t w = lv_area_get_width(dest_area);
    int32_t h = lv_area_get_height(dest_area);

    arm_2d_size_t copy_size = {
        .iWidth = lv_area_get_width(dest_area),
        .iHeight = lv_area_get_height(dest_area),
    };

#if LV_COLOR_SCREEN_TRANSP
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
#endif

    /*Simple fill (maybe with opacity), no masking*/
    if(mask == NULL) {
        if(opa >= LV_OPA_MAX) {
            __arm_2d_impl_copy((color_int *)src_buf,
                               src_stride,
                               (color_int *)dest_buf,
                               dest_stride,
                               &copy_size);
        }
        else {
#if LV_COLOR_SCREEN_TRANSP
            return false;
#else
            __arm_2d_impl_alpha_blending((color_int *)src_buf,
                                         src_stride,
                                         (color_int *)dest_buf,
                                         dest_stride,
                                         &copy_size,
                                         opa);
#endif
        }
    }
    /*Masked*/
    else {
        /*Only the mask matters*/
        if(opa > LV_OPA_MAX) {
#if LV_COLOR_SCREEN_TRANSP
            return false;
#else
            __arm_2d_impl_src_msk_copy((color_int *)src_buf,
                                       src_stride,
                                       (uint8_t *)mask,
                                       mask_stride,
                                       &copy_size,
                                       (color_int *)dest_buf,
                                       dest_stride,
                                       &copy_size);
#endif
        }
        /*Handle opa and mask values too*/
        else {
#if LV_COLOR_SCREEN_TRANSP
            return false;
#else
            __arm_2d_impl_gray8_alpha_blending((uint8_t *)mask,
                                               mask_stride,
                                               (uint8_t *)mask,
                                               mask_stride,
                                               &copy_size,
                                               opa);

            __arm_2d_impl_src_msk_copy((color_int *)src_buf,
                                       src_stride,
                                       (uint8_t *)mask,
                                       mask_stride,
                                       &copy_size,
                                       (color_int *)dest_buf,
                                       dest_stride,
                                       &copy_size);
#endif
        }
    }

    return true;
}

LV_ATTRIBUTE_FAST_MEM
static void lv_draw_arm2d_img_decoded(struct _lv_draw_ctx_t * draw_ctx,
                                      const lv_draw_img_dsc_t * draw_dsc,
                                      const lv_area_t * coords,
                                      const uint8_t * src_buf,
                                      lv_img_cf_t cf)
{
    /*Use the clip area as draw area*/
    lv_area_t draw_area;
    lv_area_copy(&draw_area, draw_ctx->clip_area);

    bool mask_any = lv_draw_mask_is_any(&draw_area);
    bool transform = draw_dsc->angle != 0 || draw_dsc->zoom != LV_IMG_ZOOM_NONE ? true : false;

    lv_area_t blend_area;
    lv_draw_sw_blend_dsc_t blend_dsc;

    lv_memzero(&blend_dsc, sizeof(lv_draw_sw_blend_dsc_t));
    blend_dsc.opa = draw_dsc->opa;
    blend_dsc.blend_mode = draw_dsc->blend_mode;
    blend_dsc.blend_area = &blend_area;

    /*The simplest case just copy the pixels into the draw_buf*/
    if(!mask_any && !transform && cf == LV_IMG_CF_TRUE_COLOR && draw_dsc->recolor_opa == LV_OPA_TRANSP) {
        blend_dsc.src_buf = (const lv_color_t *)src_buf;

        blend_dsc.blend_area = coords;
        lv_draw_sw_blend(draw_ctx, &blend_dsc);
    }
    else if(!mask_any && !transform && cf == LV_IMG_CF_ALPHA_8BIT) {
        blend_dsc.mask_buf = (lv_opa_t *)src_buf;
        blend_dsc.mask_area = coords;
        blend_dsc.src_buf = NULL;
        blend_dsc.color = draw_dsc->recolor;
        blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;

        blend_dsc.blend_area = coords;
        lv_draw_sw_blend(draw_ctx, &blend_dsc);
    }
#if LV_COLOR_DEPTH == 16
    else if(!mask_any && !transform && cf == LV_IMG_CF_RGB565A8 && draw_dsc->recolor_opa == LV_OPA_TRANSP) {
        lv_coord_t src_w = lv_area_get_width(coords);
        lv_coord_t src_h = lv_area_get_height(coords);
        blend_dsc.src_buf = (const lv_color_t *)src_buf;
        blend_dsc.mask_buf = (lv_opa_t *)src_buf;
        blend_dsc.mask_buf += sizeof(lv_color_t) * src_w * src_h;
        blend_dsc.blend_area = coords;
        blend_dsc.mask_area = coords;
        blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
        lv_draw_sw_blend(draw_ctx, &blend_dsc);
    }
#endif
    /*In the other cases every pixel need to be checked one-by-one*/
    else {
        blend_area.x1 = draw_ctx->clip_area->x1;
        blend_area.x2 = draw_ctx->clip_area->x2;
        blend_area.y1 = draw_ctx->clip_area->y1;
        blend_area.y2 = draw_ctx->clip_area->y2;

        lv_coord_t src_w = lv_area_get_width(coords);
        lv_coord_t src_h = lv_area_get_height(coords);
        lv_coord_t blend_h = lv_area_get_height(&blend_area);
        lv_coord_t blend_w = lv_area_get_width(&blend_area);

        uint32_t max_buf_size = MAX_BUF_SIZE;
        uint32_t blend_size = lv_area_get_size(&blend_area);
        uint32_t buf_h;
        uint32_t buf_w = blend_w;
        if(blend_size <= max_buf_size) {
            buf_h = blend_h;
        }
        else {
            /*Round to full lines*/
            buf_h = max_buf_size / blend_w;
        }

        /*Create buffers and masks*/
        uint32_t buf_size = buf_w * buf_h;

        lv_color_t * rgb_buf = lv_malloc(buf_size * sizeof(lv_color_t));
        lv_opa_t * mask_buf = lv_malloc(buf_size);
        blend_dsc.mask_buf = mask_buf;
        blend_dsc.mask_area = &blend_area;
        blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
        blend_dsc.src_buf = rgb_buf;
        lv_coord_t y_last = blend_area.y2;
        blend_area.y2 = blend_area.y1 + buf_h - 1;

        lv_draw_mask_res_t mask_res_def = (cf != LV_IMG_CF_TRUE_COLOR || draw_dsc->angle ||
                                           draw_dsc->zoom != LV_IMG_ZOOM_NONE) ?
                                          LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;
        blend_dsc.mask_res = mask_res_def;

        bool is_accelerated = false;

        if(!transform) {
            if(LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED == cf)  {
                /* copy with colour keying */

                /* *INDENT-OFF* */
                __RECOLOUR_WRAPPER(

                    lv_color_t chrome_key = LV_COLOR_CHROMA_KEY;
                    /* calculate new chrome-key colour */
                    if(draw_dsc->recolor_opa > LV_OPA_MIN) {
                    __ARM_2D_PIXEL_BLENDING_OPA(
                        (color_int *) & (draw_dsc->recolor.full),
                        (color_int *) & (chrome_key.full),
                        draw_dsc->recolor_opa
                    );
                    }

                    __PREPARE_LL_ACCELERATION__();

                    if(blend_dsc.opa >= LV_OPA_MAX) {
                    __arm_2d_impl_cl_key_copy(
                        (color_int *)src_buf_tmp,
                        src_stride,
                        (color_int *)dest_buf,
                        dest_stride,
                        &copy_size,
                        (color_int)chrome_key.full);
                    }
                    else {
                        __arm_2d_impl_alpha_blending_colour_keying(
                            (color_int *)src_buf_tmp,
                            src_stride,
                            (color_int *)dest_buf,
                            dest_stride,
                            &copy_size,
                            blend_dsc.opa,
                            (color_int)chrome_key.full);
                    }
                    is_accelerated = true;
                )
                /* *INDENT-ON* */
            }
            else if((LV_COLOR_DEPTH == 32)
                    &&     !mask_any
                    && (cf == LV_IMG_CF_TRUE_COLOR_ALPHA)) {
                /* accelerate copy-with-source-masks-and-opacity */

                /* *INDENT-OFF* */
                __RECOLOUR_WRAPPER(
                    __PREPARE_LL_ACCELERATION__();

                    uint8_t * mask_temp_buf = NULL;
                    if(blend_dsc.opa < LV_OPA_MAX) {
                        mask_temp_buf = lv_malloc(copy_size.iHeight * copy_size.iWidth);
                        if(NULL == mask_temp_buf) {
                            LV_LOG_WARN(
                                "Failed to allocate memory for alpha mask,"
                                " use normal route instead.");
                            break;
                        }
                        lv_memzero(mask_temp_buf, copy_size.iHeight * copy_size.iWidth);

                        __arm_2d_impl_gray8_colour_filling_channel_mask_opacity(
                            mask_temp_buf,
                            src_stride,
                            (uint32_t *)
                            ((uintptr_t)src_buf_tmp + LV_IMG_PX_SIZE_ALPHA_BYTE - 1),
                            src_stride,
                            &copy_size,
                            0xFF,
                            blend_dsc.opa);

                        __arm_2d_impl_src_msk_copy(
                            (color_int *)src_buf_tmp,
                            src_stride,
                            mask_temp_buf,
                            src_stride,
                            &copy_size,
                            (color_int *)dest_buf,
                            dest_stride,
                            &copy_size);

                        lv_free(mask_temp_buf);
                    }
                    else {
                        __arm_2d_impl_src_chn_msk_copy(
                            (color_int *)src_buf_tmp,
                            src_stride,
                            (uint32_t *)
                            ((uintptr_t)src_buf_tmp + LV_IMG_PX_SIZE_ALPHA_BYTE - 1),
                            src_stride,
                            &copy_size,
                            (color_int *)dest_buf,
                            dest_stride,
                            &copy_size);
                    }

                    is_accelerated = true;
                )
                /* *INDENT-ON* */
            }
            else if(!mask_any && (cf == LV_IMG_CF_TRUE_COLOR)) {
                /* accelerate copy-with-source-masks-and-opacity */

                /* *INDENT-OFF* */
                __RECOLOUR_WRAPPER(
                    __PREPARE_LL_ACCELERATION__();

                    if(blend_dsc.opa >= LV_OPA_MAX) {
                    __arm_2d_impl_copy(
                        (color_int *)src_buf_tmp,
                        src_stride,
                        (color_int *)dest_buf,
                        dest_stride,
                        &copy_size);
                    }
                    else {
                        __arm_2d_impl_alpha_blending(
                            (color_int *)src_buf_tmp,
                            src_stride,
                            (color_int *)dest_buf,
                            dest_stride,
                            &copy_size,
                            blend_dsc.opa);
                    }
                    is_accelerated = true;
                )
                /* *INDENT-ON* */
            }
        }
        else if(!mask_any
#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) && __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
                && (draw_dsc->antialias == 1)
#else
                && (draw_dsc->antialias == 0)
#endif
                && (draw_dsc->recolor_opa == LV_OPA_TRANSP)
                && (((LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED == cf)
                     || (LV_IMG_CF_TRUE_COLOR == cf))
#if defined(__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__) &&  __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
                    || ((LV_IMG_CF_TRUE_COLOR_ALPHA == cf)
                        && (LV_COLOR_DEPTH == 32))
#endif
                   )
               ) {

            /* *INDENT-OFF* */
            __RECOLOUR_WRAPPER(
                /* accelerate transform without re-color */

                static arm_2d_tile_t target_tile_origin;
                static arm_2d_tile_t target_tile;
                arm_2d_region_t clip_region;
                static arm_2d_region_t target_region;

                lv_color_t * dest_buf = draw_ctx->buf;

                target_tile_origin = (arm_2d_tile_t) {
                    .tRegion = {
                        .tSize = {
                            .iWidth = lv_area_get_width(draw_ctx->buf_area),
                            .iHeight = lv_area_get_height(draw_ctx->buf_area),
                        },
                    },
                    .tInfo.bIsRoot = true,
                    .phwBuffer = (uint16_t *)draw_ctx->buf,
                };

                clip_region = (arm_2d_region_t) {
                    .tLocation = {
                        .iX = draw_ctx->clip_area->x1 - draw_ctx->buf_area->x1,
                        .iY = draw_ctx->clip_area->y1 - draw_ctx->buf_area->y1,
                    },
                    .tSize = {
                        .iWidth = lv_area_get_width(draw_ctx->clip_area),
                        .iHeight = lv_area_get_height(draw_ctx->clip_area),
                    },
                };

                arm_2d_tile_generate_child(&target_tile_origin,
                                           &clip_region,
                                           &target_tile,
                                           false);

                target_region = (arm_2d_region_t) {
                    .tLocation = {
                        .iX = coords->x1 - draw_ctx->clip_area->x1,
                        .iY = coords->y1 - draw_ctx->clip_area->y1,
                    },
                    .tSize = {
                        .iWidth = lv_area_get_width(coords),
                        .iHeight = lv_area_get_height(coords),
                    },
                };

                static arm_2d_tile_t source_tile;

                source_tile = (arm_2d_tile_t) {
                    .tRegion = {
                        .tSize = {
                            .iWidth = src_w,
                            .iHeight = src_h,
                        },
                    },
                    .tInfo.bIsRoot = true,
                    .pchBuffer = (uint8_t *)src_buf,
                };

                static arm_2d_tile_t mask_tile;
                mask_tile = source_tile;

                mask_tile.tInfo.bHasEnforcedColour = true;
                mask_tile.tInfo.tColourInfo.chScheme = ARM_2D_CHANNEL_8in32;
                mask_tile.pchBuffer += 3;

                static arm_2d_location_t source_center, target_center;
                source_center.iX = draw_dsc->pivot.x;
                source_center.iY = draw_dsc->pivot.y;


                if((LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED == cf) ||
                   (LV_IMG_CF_TRUE_COLOR == cf)) {
                    arm_2d_tile_transform_with_opacity(
                        &source_tile,
                        &target_tile,
                        &target_region,
                        source_center,
                        ARM_2D_ANGLE((draw_dsc->angle / 10.0f)),
                        draw_dsc->zoom / 256.0f,
                        (color_int)LV_COLOR_CHROMA_KEY.full,
                        blend_dsc.opa);

                    is_accelerated = true;
                }
    #if defined(__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__)          \
                &&  __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
                else if((LV_IMG_CF_TRUE_COLOR_ALPHA == cf) &&
                        (LV_COLOR_DEPTH == 32)) {
                    arm_2d_tile_transform_with_src_mask_and_opacity(
                        &source_tile,
                        &mask_tile,
                        &target_tile,
                        &target_region,
                        source_center,
                        ARM_2D_ANGLE((draw_dsc->angle / 10.0f)),
                        draw_dsc->zoom / 256.0f,
                        blend_dsc.opa);

                    is_accelerated = true;
                }
    #endif
            )
            /* *INDENT-ON* */
        }

        /* *INDENT-OFF* */
        if(!is_accelerated) while(blend_area.y1 <= y_last) {
            /*Apply transformations if any or separate the channels*/
            lv_area_t transform_area;
            lv_area_copy(&transform_area, &blend_area);
            lv_area_move(&transform_area, -coords->x1, -coords->y1);
            if(transform) {
                lv_draw_transform(draw_ctx, &transform_area, src_buf, src_w, src_h, src_w,
                                  draw_dsc, cf, rgb_buf, mask_buf);
            }
            else {
                convert_cb(&transform_area, src_buf, src_w, src_h, src_w, draw_dsc, cf, rgb_buf, mask_buf);
            }

            /*Apply recolor*/
            if(draw_dsc->recolor_opa > LV_OPA_MIN) {
                arm_2d_size_t copy_size = {
                    .iWidth = buf_w,
                    .iHeight = buf_h,
                };

                /* apply re-colour */
                __arm_2d_impl_colour_filling_with_opacity(
                    (color_int *)rgb_buf,
                    buf_w,
                    &copy_size,
                    (color_int)draw_dsc->recolor.full,
                    draw_dsc->recolor_opa);
            }
#if LV_USE_DRAW_MASKS
            /*Apply the masks if any*/
            if(mask_any) {
                lv_coord_t y;
                lv_opa_t * mask_buf_tmp = mask_buf;
                for(y = blend_area.y1; y <= blend_area.y2; y++) {
                    lv_draw_mask_res_t mask_res_line;
                    mask_res_line = lv_draw_mask_apply(mask_buf_tmp, blend_area.x1, y, blend_w);

                    if(mask_res_line == LV_DRAW_MASK_RES_TRANSP) {
                        lv_memzero(mask_buf_tmp, blend_w);
                        blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
                    }
                    else if(mask_res_line == LV_DRAW_MASK_RES_CHANGED) {
                        blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
                    }
                    mask_buf_tmp += blend_w;
                }
            }
#endif

            /*Blend*/
            lv_draw_sw_blend(draw_ctx, &blend_dsc);

            /*Go the the next lines*/
            blend_area.y1 = blend_area.y2 + 1;
            blend_area.y2 = blend_area.y1 + buf_h - 1;
            if(blend_area.y2 > y_last) blend_area.y2 = y_last;
        }

        lv_free(mask_buf);
        lv_free(rgb_buf);
    }
}

static void lv_gpu_arm2d_wait_cb(lv_draw_ctx_t * draw_ctx)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();

    arm_2d_op_wait_async(NULL);
    if(disp->driver && disp->driver->wait_cb) {
        disp->driver->wait_cb(disp->driver);
    }
    lv_draw_sw_wait_for_finish(draw_ctx);
}


#endif


/**********************
 *   STATIC FUNCTIONS
 **********************/
/* Separate the image channels to RGB and Alpha to match LV_COLOR_DEPTH settings*/
static void convert_cb(const lv_area_t * dest_area, const void * src_buf, lv_coord_t src_w, lv_coord_t src_h,
                       lv_coord_t src_stride, const lv_draw_img_dsc_t * draw_dsc, lv_img_cf_t cf, lv_color_t * cbuf, lv_opa_t * abuf)
{
    LV_UNUSED(draw_dsc);
    LV_UNUSED(src_h);
    LV_UNUSED(src_w);

    const uint8_t * src_tmp8 = (const uint8_t *)src_buf;
    lv_coord_t y;
    lv_coord_t x;

    if(cf == LV_IMG_CF_TRUE_COLOR || cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED) {
        uint32_t px_cnt = lv_area_get_size(dest_area);
        lv_memset(abuf, 0xff, px_cnt);

        src_tmp8 += (src_stride * dest_area->y1 * sizeof(lv_color_t)) + dest_area->x1 * sizeof(lv_color_t);
        uint32_t dest_w = lv_area_get_width(dest_area);
        uint32_t dest_w_byte = dest_w * sizeof(lv_color_t);

        lv_coord_t src_stride_byte = src_stride * sizeof(lv_color_t);
        lv_color_t * cbuf_tmp = cbuf;
        for(y = dest_area->y1; y <= dest_area->y2; y++) {
            lv_memcpy(cbuf_tmp, src_tmp8, dest_w_byte);
            src_tmp8 += src_stride_byte;
            cbuf_tmp += dest_w;
        }

        /*Make "holes" for with Chroma keying*/
        if(cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED) {
            uint32_t i;
            lv_color_t chk = LV_COLOR_CHROMA_KEY;
#if LV_COLOR_DEPTH == 8 || LV_COLOR_DEPTH == 1
            uint8_t * cbuf_uint = (uint8_t *)cbuf;
            uint8_t chk_v = chk.full;
#elif LV_COLOR_DEPTH == 16
            uint16_t * cbuf_uint = (uint16_t *)cbuf;
            uint16_t chk_v = chk.full;
#elif LV_COLOR_DEPTH == 32
            uint32_t * cbuf_uint = (uint32_t *)cbuf;
            uint32_t chk_v = chk.full;
#endif
            for(i = 0; i < px_cnt; i++) {
                if(chk_v == cbuf_uint[i]) abuf[i] = 0x00;
            }
        }
    }
    else if(cf == LV_IMG_CF_TRUE_COLOR_ALPHA) {
        src_tmp8 += (src_stride * dest_area->y1 * LV_IMG_PX_SIZE_ALPHA_BYTE) + dest_area->x1 * LV_IMG_PX_SIZE_ALPHA_BYTE;

        lv_coord_t src_new_line_step_px = (src_stride - lv_area_get_width(dest_area));
        lv_coord_t src_new_line_step_byte = src_new_line_step_px * LV_IMG_PX_SIZE_ALPHA_BYTE;

        lv_coord_t dest_h = lv_area_get_height(dest_area);
        lv_coord_t dest_w = lv_area_get_width(dest_area);
        for(y = 0; y < dest_h; y++) {
            for(x = 0; x < dest_w; x++) {
                abuf[x] = src_tmp8[LV_IMG_PX_SIZE_ALPHA_BYTE - 1];
#if LV_COLOR_DEPTH == 8 || LV_COLOR_DEPTH == 1
                cbuf[x].full = *src_tmp8;
#elif LV_COLOR_DEPTH == 16
                cbuf[x].full = *src_tmp8 + ((*(src_tmp8 + 1)) << 8);
#elif LV_COLOR_DEPTH == 32
                cbuf[x] = *((lv_color_t *) src_tmp8);
                cbuf[x].ch.alpha = 0xff;
#endif
                src_tmp8 += LV_IMG_PX_SIZE_ALPHA_BYTE;

            }
            cbuf += dest_w;
            abuf += dest_w;
            src_tmp8 += src_new_line_step_byte;
        }
    }
    else if(cf == LV_IMG_CF_RGB565A8) {
        src_tmp8 += (src_stride * dest_area->y1 * sizeof(lv_color_t)) + dest_area->x1 * sizeof(lv_color_t);

        lv_coord_t src_stride_byte = src_stride * sizeof(lv_color_t);

        lv_coord_t dest_h = lv_area_get_height(dest_area);
        lv_coord_t dest_w = lv_area_get_width(dest_area);
        for(y = 0; y < dest_h; y++) {
            lv_memcpy(cbuf, src_tmp8, dest_w * sizeof(lv_color_t));
            cbuf += dest_w;
            src_tmp8 += src_stride_byte;
        }

        src_tmp8 = (const uint8_t *)src_buf;
        src_tmp8 += sizeof(lv_color_t) * src_w * src_h;
        src_tmp8 += src_stride * dest_area->y1 + dest_area->x1;
        for(y = 0; y < dest_h; y++) {
            lv_memcpy(abuf, src_tmp8, dest_w);
            abuf += dest_w;
            src_tmp8 += src_stride;
        }
    }
}

#if 0
static void invalidate_cache(void)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    if(disp->driver->clean_dcache_cb) disp->driver->clean_dcache_cb(disp->driver);
    else {
#if __CORTEX_M >= 0x07
        if((SCB->CCR) & (uint32_t)SCB_CCR_DC_Msk)
            SCB_CleanInvalidateDCache();
#endif
    }
}
#endif

#endif
