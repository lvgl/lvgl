/**
 * @file lv_gpu_arm2d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gpu_arm2d.h"
#include "../../core/lv_refr.h"

#if LV_USE_GPU_ARM2D
#include "arm_2d.h"

/*********************
 *      DEFINES
 *********************/

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
#define color_int                       uint32_t

#else
#error The specified LV_COLOR_DEPTH is not supported by this version of lv_gpu_arm2d.c.
#endif

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

/* arm-2d direct mode APIs */
extern
void __arm_2d_impl_colour_filling(color_int * __RESTRICT pTarget,
                                  int16_t iTargetStride,
                                  arm_2d_size_t * __RESTRICT ptCopySize,
                                  color_int Colour);

extern
void __arm_2d_impl_colour_filling_with_opacity(
    color_int * __RESTRICT pTargetBase,
    int16_t iTargetStride,
    arm_2d_size_t * __RESTRICT ptCopySize,
    color_int Colour,
    uint_fast8_t chRatio);

extern
void __arm_2d_impl_colour_filling_mask(
    color_int * __RESTRICT pTarget,
    int16_t iTargetStride,
    uint8_t * __RESTRICT pchAlpha,
    int16_t iAlphaStride,
    arm_2d_size_t * __RESTRICT ptCopySize,
    color_int Colour);

extern
void __arm_2d_impl_colour_filling_mask_opacity(
    color_int * __RESTRICT pTarget,
    int16_t iTargetStride,
    uint8_t * __RESTRICT pchAlpha,
    int16_t iAlphaStride,
    arm_2d_size_t * __RESTRICT ptCopySize,
    color_int Colour,
    uint8_t chOpacity);

extern
void __arm_2d_impl_copy(color_int * __RESTRICT pSource,
                        int16_t iSourceStride,
                        color_int * __RESTRICT pTarget,
                        int16_t iTargetStride,
                        arm_2d_size_t * __RESTRICT ptCopySize);

extern
void __arm_2d_impl_alpha_blending(color_int * __RESTRICT pSource,
                                  int16_t iSourceStride,
                                  color_int * __RESTRICT pTarget,
                                  int16_t iTargetStride,
                                  arm_2d_size_t * __RESTRICT ptCopySize,
                                  uint_fast8_t chRatio);

extern
void __arm_2d_impl_gray8_alpha_blending(uint8_t * __RESTRICT pSource,
                                        int16_t iSourceStride,
                                        uint8_t * __RESTRICT pTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t * __RESTRICT ptCopySize,
                                        uint_fast8_t chRatio);

extern
void __arm_2d_impl_src_msk_copy(color_int * __RESTRICT pSourceBase,
                                int16_t iSourceStride,
                                uint8_t * __RESTRICT ptSourceMaskBase,
                                int16_t iSourceMaskStride,
                                arm_2d_size_t * __RESTRICT ptSourceMaskSize,
                                color_int * __RESTRICT pTargetBase,
                                int16_t iTargetStride,
                                arm_2d_size_t * __RESTRICT ptCopySize);

extern
void __arm_2d_impl_src_chn_msk_copy(color_int * __RESTRICT pSourceBase,
                                    int16_t iSourceStride,
                                    uint32_t * __RESTRICT ptSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,
                                    color_int * __RESTRICT pTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t * __RESTRICT ptCopySize);

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

    static arm_2d_tile_t target_tile;
    static arm_2d_tile_t source_tile_orig;
    static arm_2d_tile_t source_tile;
    static arm_2d_tile_t mask_tile_orig;
    static arm_2d_tile_t mask_tile;
    static arm_2d_region_t target_region;

    bool is_accelerated = false;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL
       &&    lv_area_get_size(&blend_area) > 100) {

        lv_color_t * dest_buf = draw_ctx->buf;


        target_tile = (arm_2d_tile_t) {
            .tRegion = {
                .tSize = {
                    .iWidth = lv_area_get_width(draw_ctx->buf_area),
                    .iHeight = lv_area_get_height(draw_ctx->buf_area),
                },
            },
            .tInfo.bIsRoot = true,
            .phwBuffer = (uint16_t *)draw_ctx->buf,
        };

        target_region = (arm_2d_region_t) {
            .tLocation = {
                .iX = blend_area.x1 - draw_ctx->buf_area->x1,
                .iY = blend_area.y1 - draw_ctx->buf_area->y1,
            },
            .tSize = {
                .iWidth = lv_area_get_width(&blend_area),
                .iHeight = lv_area_get_height(&blend_area),
            },
        };

        if(NULL != mask) {
            mask_tile_orig = (arm_2d_tile_t) {
                .tRegion = {
                    .tSize = {
                        .iWidth = lv_area_get_width(dsc->mask_area),
                        .iHeight = lv_area_get_height(dsc->mask_area),
                    },
                },
                .tInfo = {
                    .bIsRoot = true,
                    .bHasEnforcedColour = true,
                    .tColourInfo = {
                        .chScheme = ARM_2D_COLOUR_8BIT,
                    },
                },
                .pchBuffer = (uint8_t *)mask,
            };

            arm_2d_tile_generate_child(
                &mask_tile_orig,
            (arm_2d_region_t []) {
                {
                    .tLocation = {
                        .iX = dsc->mask_area->x1 - blend_area.x1,
                        .iY = dsc->mask_area->y1 - blend_area.y1,
                    },
                    .tSize = mask_tile_orig.tRegion.tSize,
                }
            },
            &mask_tile,
            false);
            mask_tile.tInfo.bDerivedResource = true;
        }


        const lv_color_t * src_buf = dsc->src_buf;
        if(src_buf) {
            source_tile_orig = (arm_2d_tile_t) {
                .tRegion = {
                    .tSize = {
                        .iWidth = lv_area_get_width(dsc->blend_area),
                        .iHeight = lv_area_get_height(dsc->blend_area),
                    },
                },
                .tInfo.bIsRoot = true,
                .phwBuffer = (uint16_t *)src_buf,
            };

            arm_2d_tile_generate_child(
                &source_tile_orig,
            (arm_2d_region_t []) {
                {
                    .tLocation = {
                        .iX = blend_area.x1 - dsc->blend_area->x1,
                        .iY = blend_area.y1 - dsc->blend_area->y1,
                    },
                    .tSize = source_tile_orig.tRegion.tSize,
                }
            },
            &source_tile,
            false);
            source_tile.tInfo.bDerivedResource = true;

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
            mask += mask_stride * (dsc->mask_area->y1 - blend_area.y1) + (dsc->mask_area->x1 - blend_area.x1);
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
#if LV_DRAW_COMPLEX
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
#if LV_DRAW_COMPLEX
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
    bool mask_any = lv_draw_mask_is_any(draw_ctx->clip_area);

    /*The simplest case just copy the pixels into the draw_buf*/
    if(!mask_any && draw_dsc->angle == 0 && draw_dsc->zoom == LV_IMG_ZOOM_NONE &&
       cf == LV_IMG_CF_TRUE_COLOR && draw_dsc->recolor_opa == LV_OPA_TRANSP) {

        lv_draw_sw_img_decoded(draw_ctx, draw_dsc, coords, src_buf, cf);
        return ;
    }
    /*In the other cases every pixel need to be checked one-by-one*/
    else {
        /*Use the clip area as draw area*/
        lv_area_t draw_area;
        lv_area_copy(&draw_area, draw_ctx->clip_area);

        lv_draw_sw_blend_dsc_t blend_dsc;
        lv_memset_00(&blend_dsc, sizeof(blend_dsc));
        blend_dsc.opa = draw_dsc->opa;
        blend_dsc.blend_mode = draw_dsc->blend_mode;


        //#if LV_DRAW_COMPLEX
        /*The pixel size in byte is different if an alpha byte is added too*/
        uint8_t px_size_byte = cf == LV_IMG_CF_TRUE_COLOR_ALPHA ? LV_IMG_PX_SIZE_ALPHA_BYTE : sizeof(lv_color_t);

        /*Go to the first displayed pixel of the map*/
        int32_t src_stride = lv_area_get_width(coords);

        lv_color_t c;
        lv_color_t chroma_keyed_color = LV_COLOR_CHROMA_KEY;
        uint32_t px_i = 0;

        const uint8_t * map_px;

        lv_coord_t draw_area_h = lv_area_get_height(&draw_area);
        lv_coord_t draw_area_w = lv_area_get_width(&draw_area);

        lv_area_t blend_area;
        blend_area.x1 = draw_area.x1;
        blend_area.x2 = draw_area.x2;
        blend_area.y1 = draw_area.y1;
        blend_area.y2 = blend_area.y1;
        blend_dsc.blend_area = &blend_area;

        bool transform = draw_dsc->angle != 0 || draw_dsc->zoom != LV_IMG_ZOOM_NONE ? true : false;
        /*Simple ARGB image. Handle it as special case because it's very common*/
        if(!mask_any && !transform && cf == LV_IMG_CF_TRUE_COLOR_ALPHA && draw_dsc->recolor_opa == LV_OPA_TRANSP) {

            const uint8_t * src_buf_tmp = src_buf;
            src_buf_tmp += src_stride * (draw_area.y1 - coords->y1) * px_size_byte;
            src_buf_tmp += (draw_area.x1 - coords->x1) * px_size_byte;

#if LV_COLOR_DEPTH == 32
            if(blend_dsc.opa >= LV_OPA_MAX) {
                lv_area_t blend_area2;
                if(!_lv_area_intersect(&blend_area2, &draw_area, draw_ctx->clip_area)) return;

                int32_t w = lv_area_get_width(&blend_area2);
                int32_t h = lv_area_get_height(&blend_area2);

                lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);

                lv_color_t * dest_buf = draw_ctx->buf;
                dest_buf += dest_stride * (blend_area2.y1 - draw_ctx->buf_area->y1)
                            + (blend_area2.x1 - draw_ctx->buf_area->x1);

                arm_2d_size_t copy_size = {
                    .iWidth = lv_area_get_width(&blend_area2),
                    .iHeight = lv_area_get_height(&blend_area2),
                };

                //lv_area_move(&blend_area2, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

                __arm_2d_impl_src_chn_msk_copy(
                    (color_int *)src_buf_tmp,
                    src_stride,
                    (uint32_t *)((uintptr_t)src_buf_tmp + LV_IMG_PX_SIZE_ALPHA_BYTE - 1),
                    src_stride,
                    &copy_size,
                    (color_int *)dest_buf,
                    dest_stride,
                    &copy_size);
            }
            else
#endif
            {
                lv_draw_sw_img_decoded(draw_ctx, draw_dsc, coords, src_buf, cf);
                return ;
            }

        }
        /*Most complicated case: transform or other mask or chroma keyed*/
        else {
            /*Build the image and a mask line-by-line*/
            uint32_t hor_res = (uint32_t) lv_disp_get_hor_res(_lv_refr_get_disp_refreshing());
            uint32_t mask_buf_size = lv_area_get_size(&draw_area) > hor_res ? hor_res : lv_area_get_size(&draw_area);
            lv_color_t * src_buf_rgb = lv_mem_buf_get(mask_buf_size * sizeof(lv_color_t));
            lv_opa_t * mask_buf = lv_mem_buf_get(mask_buf_size);
            blend_dsc.mask_buf = mask_buf;
            blend_dsc.mask_area = &blend_area;
            blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
            blend_dsc.src_buf = src_buf_rgb;

            const uint8_t * src_buf_tmp = NULL;
#if LV_DRAW_COMPLEX
            lv_img_transform_dsc_t trans_dsc;
            lv_memset_00(&trans_dsc, sizeof(lv_img_transform_dsc_t));
            if(transform) {
                trans_dsc.cfg.angle = draw_dsc->angle;
                trans_dsc.cfg.zoom = draw_dsc->zoom;
                trans_dsc.cfg.src = src_buf;
                trans_dsc.cfg.src_w = src_stride;
                trans_dsc.cfg.src_h = lv_area_get_height(coords);
                trans_dsc.cfg.cf = cf;
                trans_dsc.cfg.pivot_x = draw_dsc->pivot.x;
                trans_dsc.cfg.pivot_y = draw_dsc->pivot.y;
                trans_dsc.cfg.color = draw_dsc->recolor;
                trans_dsc.cfg.antialias = draw_dsc->antialias;

                _lv_img_buf_transform_init(&trans_dsc);
            }
            else
#endif
            {
                src_buf_tmp = src_buf;
                src_buf_tmp += src_stride * (draw_area.y1 - coords->y1) * px_size_byte;
                src_buf_tmp += (draw_area.x1 - coords->x1) * px_size_byte;
            }

            uint16_t recolor_premult[3] = {0};
            lv_opa_t recolor_opa_inv = 255 - draw_dsc->recolor_opa;
            if(draw_dsc->recolor_opa != 0) {
                lv_color_premult(draw_dsc->recolor, draw_dsc->recolor_opa, recolor_premult);
            }

            blend_dsc.mask_res = (cf != LV_IMG_CF_TRUE_COLOR || draw_dsc->angle ||
                                  draw_dsc->zoom != LV_IMG_ZOOM_NONE) ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;

            /*Prepare the `mask_buf`if there are other masks*/
            if(mask_any) {
                lv_memset_ff(mask_buf, mask_buf_size);
            }

            int32_t x;
            int32_t y;
#if LV_DRAW_COMPLEX
            int32_t rot_y = blend_area.y1 - coords->y1;
#endif
            for(y = 0; y < draw_area_h; y++) {
                map_px = src_buf_tmp;
#if LV_DRAW_COMPLEX
                uint32_t px_i_start = px_i;
                int32_t rot_x = blend_area.x1 - coords->x1;
#endif

                for(x = 0; x < draw_area_w; x++, px_i++, map_px += px_size_byte) {

#if LV_DRAW_COMPLEX
                    if(transform) {

                        /*Transform*/
                        bool ret;
                        ret = _lv_img_buf_transform(&trans_dsc, rot_x + x, rot_y + y);
                        if(ret == false) {
                            mask_buf[px_i] = LV_OPA_TRANSP;
                            continue;
                        }
                        else {
                            mask_buf[px_i] = trans_dsc.res.opa;
                            c.full = trans_dsc.res.color.full;
                        }
                    }
                    /*No transform*/
                    else
#endif
                    {
                        if(cf == LV_IMG_CF_TRUE_COLOR_ALPHA) {
                            lv_opa_t px_opa = map_px[LV_IMG_PX_SIZE_ALPHA_BYTE - 1];
                            mask_buf[px_i] = px_opa;
                            if(px_opa == 0) {
#if  LV_COLOR_DEPTH == 32
                                src_buf_rgb[px_i].full = 0;
#endif
                                continue;
                            }
                        }
                        else {
                            mask_buf[px_i] = 0xFF;
                        }

#if LV_COLOR_DEPTH == 1
                        c.full = map_px[0];
#elif LV_COLOR_DEPTH == 8
                        c.full = map_px[0];
#elif LV_COLOR_DEPTH == 16
                        c.full = map_px[0] + (map_px[1] << 8);
#elif LV_COLOR_DEPTH == 32
                        c.full = *((uint32_t *)map_px);
                        c.ch.alpha = 0xFF;
#endif
                        if(cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED) {
                            if(c.full == chroma_keyed_color.full) {
                                mask_buf[px_i] = LV_OPA_TRANSP;
#if  LV_COLOR_DEPTH == 32
                                src_buf_rgb[px_i].full = 0;
#endif
                                continue;
                            }
                        }

                    }
                    if(draw_dsc->recolor_opa != 0) {
                        c = lv_color_mix_premult(recolor_premult, c, recolor_opa_inv);
                    }

                    src_buf_rgb[px_i].full = c.full;
                }
#if LV_DRAW_COMPLEX
                /*Apply the masks if any*/
                if(mask_any) {
                    lv_draw_mask_res_t mask_res_sub;
                    mask_res_sub = lv_draw_mask_apply(mask_buf + px_i_start, blend_area.x1,
                                                      y + draw_area.y1, draw_area_w);
                    if(mask_res_sub == LV_DRAW_MASK_RES_TRANSP) {
                        lv_memset_00(mask_buf + px_i_start, draw_area_w);
                        blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
                    }
                    else if(mask_res_sub == LV_DRAW_MASK_RES_CHANGED) {
                        blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
                    }
                }
#endif

                src_buf_tmp += src_stride * px_size_byte;
                if(px_i + draw_area_w < mask_buf_size) {
                    blend_area.y2 ++;
                }
                else {
                    lv_draw_sw_blend(draw_ctx, &blend_dsc);

                    blend_area.y1 = blend_area.y2 + 1;
                    blend_area.y2 = blend_area.y1;

                    px_i = 0;
                    blend_dsc.mask_res = (cf != LV_IMG_CF_TRUE_COLOR || draw_dsc->angle ||
                                          draw_dsc->zoom != LV_IMG_ZOOM_NONE) ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;

                    /*Prepare the `mask_buf`if there are other masks*/
                    if(mask_any) {
                        lv_memset_ff(mask_buf, mask_buf_size);
                    }
                }
            }

            /*Flush the last part*/
            if(blend_area.y1 != blend_area.y2) {
                blend_area.y2--;
                lv_draw_sw_blend(draw_ctx, &blend_dsc);
            }

            lv_mem_buf_release(mask_buf);
            lv_mem_buf_release(src_buf_rgb);
        }
    }
}



static void lv_gpu_arm2d_wait_cb(lv_draw_ctx_t * draw_ctx)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();

    if(disp->driver && disp->driver->wait_cb) {
        disp->driver->wait_cb(disp->driver);
    }
    lv_draw_sw_wait_for_finish(draw_ctx);
}


#endif


/**********************
 *   STATIC FUNCTIONS
 **********************/

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
