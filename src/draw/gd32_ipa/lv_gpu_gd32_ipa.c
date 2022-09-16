/**
 * @file lv_gpu_gd32_ipa.c
 *
 * Support for GD32F4x0 IPA 2D accelerator.
 *
 * Hardware is BUGGY and in FILL mode has an errata where it adds one pixel at
 * the end of the fill buffer, but in reality, it can also add two pixels,
 * if the end of buffer is aligned on 32bits or not. If its not, only one
 * additional pixel will be clobbered. If it is, two pixels will be added.
 * This likely only affects 16 and 24 bit modes (since 32bit/ARGB will always
 * be aligned). This bug is present in GD32F450, and is not present on GD32F470.
 *
 * Solution is during the fill to backup 2 memory locations, do the fill,
 * do NOT call any other LVGL functions, and restore affected memory block.
 * The more complex solution of checking whether end address is aligned or not
 * is likely to be slower than simply backing up 2 locations regardless.
 *
 * Errata: https://www.gd32mcu.com/download/down/document_id/378/path_type/1
 * Alpha blend/2D copy is unaffected.
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gpu_gd32_ipa.h"
#include "../../core/lv_refr.h"

#if LV_USE_GPU_GD32_IPA

#include "gd32f4xx.h"

/*********************
 *      DEFINES
 *********************/

#if LV_COLOR_16_SWAP
    #error "Can't use IPA with LV_COLOR_16_SWAP 1"
#endif

#if LV_COLOR_DEPTH == 8
    #error "Can't use IPA with LV_COLOR_DEPTH == 8"
#endif

#if LV_COLOR_DEPTH == 16
    #define LV_IPA_COLOR_FORMAT LV_IPA_RGB565
#elif LV_COLOR_DEPTH == 32
    #define LV_IPA_COLOR_FORMAT LV_IPA_ARGB8888
#else
    /*Can't use GPU with other formats*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_draw_gd32_ipa_blend_fill(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area,
                                        lv_color_t color);


static void lv_draw_gd32_ipa_blend_map(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                       const lv_color_t * src_buf, lv_coord_t src_stride, lv_opa_t opa);

static void lv_draw_gd32_ipa_img_decoded(lv_draw_ctx_t * draw, const lv_draw_img_dsc_t * dsc,
                                         const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t color_format);


static void lv_gpu_gd32_ipa_wait_cb(lv_draw_ctx_t * draw_ctx);
static void invalidate_cache(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Turn on the peripheral and set output color mode, this only needs to be done once
 */
void lv_draw_gd32_ipa_init(void)
{
    /*Enable IPA clock*/
#if defined(GD32F450) || defined(GD32F470)
    RCU_REG_VAL(RCU_IPA) |= BIT(RCU_BIT_POS(RCU_IPA));
    __DSB();
    RCU_REG_VAL(RCU_IPARST) |= BIT(RCU_BIT_POS(RCU_IPARST));
    RCU_REG_VAL(RCU_IPARST) &= ~BIT(RCU_BIT_POS(RCU_IPARST));
#else
# #error "LVGL IPA support is only available on GD32F450 and GD32F470"
#endif
}


void lv_draw_gd32_ipa_ctx_init(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{

    lv_draw_sw_init_ctx(drv, draw_ctx);

    lv_draw_gd32_ipa_ctx_t * ipa_draw_ctx = (lv_draw_sw_ctx_t *)draw_ctx;

    ipa_draw_ctx->blend = lv_draw_gd32_ipa_blend;
    ipa_draw_ctx->base_draw.wait_for_finish = lv_gpu_gd32_ipa_wait_cb;
    ipa_draw_ctx->base_draw.buffer_copy = lv_draw_gd32_ipa_buffer_copy;

}

void lv_draw_gd32_ipa_ctx_deinit(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    LV_UNUSED(drv);
    LV_UNUSED(draw_ctx);
}


void lv_draw_gd32_ipa_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area)) return;

    bool done = false;

    if(dsc->mask_buf == NULL && dsc->blend_mode == LV_BLEND_MODE_NORMAL && lv_area_get_size(&blend_area) > 100) {
        lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);

        lv_color_t * dest_buf = draw_ctx->buf;
        dest_buf += dest_stride * (blend_area.y1 - draw_ctx->buf_area->y1) + (blend_area.x1 - draw_ctx->buf_area->x1);

        const lv_color_t * src_buf = dsc->src_buf;
        if(src_buf) {
            lv_draw_sw_blend_basic(draw_ctx, dsc);
            lv_coord_t src_stride;
            src_stride = lv_area_get_width(dsc->blend_area);
            src_buf += src_stride * (blend_area.y1 - dsc->blend_area->y1) + (blend_area.x1 -  dsc->blend_area->x1);
            lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
            lv_draw_gd32_ipa_blend_map(dest_buf, &blend_area, dest_stride, src_buf, src_stride, dsc->opa);
            done = true;
        }
        else if(dsc->opa >= LV_OPA_MAX) {
            lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
            lv_draw_gd32_ipa_blend_fill(dest_buf, dest_stride, &blend_area, dsc->color);
            done = true;
        }
    }

    if(!done) lv_draw_sw_blend_basic(draw_ctx, dsc);
}

void lv_draw_gd32_ipa_buffer_copy(lv_draw_ctx_t * draw_ctx,
                                  void * dest_buf, lv_coord_t dest_stride, const lv_area_t * dest_area,
                                  void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area)
{
    LV_UNUSED(draw_ctx);
    lv_draw_gd32_ipa_blend_map(dest_buf, dest_area, dest_stride, src_buf, src_stride, LV_OPA_MAX);
}


static void lv_draw_gd32_ipa_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                         const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t color_format)
{
    /*TODO basic ARGB8888 image can be handles here*/

    lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, color_format);
}

static void lv_draw_gd32_ipa_blend_fill(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area,
                                        lv_color_t color)
{
    /*Simply fill an area*/
    int32_t area_w = lv_area_get_width(fill_area);
    int32_t area_h = lv_area_get_height(fill_area);
#ifdef GD32F450
    volatile lv_color_t backup[2];
    uint32_t offset = ((area_w) + (dest_stride - area_w)) * area_h;
    volatile lv_color_t * end_ptr = (volatile lv_color_t *)(dest_buf + offset);
#endif

    invalidate_cache();

    IPA_CTL = IPA_FILL_UP_DE;
    IPA_DMADDR = (uint32_t)dest_buf;
    IPA_DPCTL = LV_IPA_COLOR_FORMAT;
    IPA_DPV = color.full;
    IPA_FLOFF = 0;
    IPA_BLOFF = 0;
    IPA_DLOFF = (dest_stride - area_w);
    IPA_IMS = ((area_w << 16U) | (area_h));

#ifdef GD32F450
    /*Work around hardware bug on GD32F450 IPA which clobbers 1 or 2 pixels after the fill*/
    backup[0] = end_ptr[0];
    backup[1] = end_ptr[1];
#endif

    /*start fill*/
    IPA_CTL |= IPA_CTL_TEN;

#ifdef GD32F450
    /*have to wait for draw to finish here, can't call external functions because IPA may trash stack or data behind buffer*/
    while(IPA_CTL & IPA_CTL_TEN);

    /*Restore two backed up pixels*/
    end_ptr[0] = backup[0];
    end_ptr[1] = backup[1];
#endif
}


static void lv_draw_gd32_ipa_blend_map(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                       const lv_color_t * src_buf, lv_coord_t src_stride, lv_opa_t opa)
{
    /*Simple copy*/
    int32_t dest_w = lv_area_get_width(dest_area);
    int32_t dest_h = lv_area_get_height(dest_area);

    invalidate_cache();
    if(opa >= LV_OPA_MAX) {
        IPA_CTL = IPA_FGTODE;
        IPA_FMADDR = (uint32_t)src_buf;
        IPA_DMADDR = (uint32_t)dest_buf;
        IPA_FLOFF = src_stride - dest_w;
        IPA_BLOFF = dest_stride - dest_w;
        IPA_DLOFF = dest_stride - dest_w;
        IPA_FPCTL = LV_IPA_COLOR_FORMAT;
        IPA_DPCTL = LV_IPA_COLOR_FORMAT;
        IPA_IMS = ((dest_w << 16U) | (dest_h));

        /*start transfer*/
        IPA_CTL |= IPA_CTL_TEN;
    }
    else {
        IPA_CTL = IPA_FGBGTODE;
        IPA_FMADDR = (uint32_t)src_buf;
        IPA_BMADDR = (uint32_t)dest_buf;
        IPA_DMADDR = (uint32_t)dest_buf;
        IPA_FLOFF = src_stride - dest_w;
        IPA_BLOFF = dest_stride - dest_w;
        IPA_DLOFF = dest_stride - dest_w;

        IPA_FPCTL = (opa << 24U) | /*alpha value*/
                    IPA_FG_ALPHA_MODE_2 | /*alpha mode 2, replace with foreground * alpha value*/
                    LV_IPA_COLOR_FORMAT;
        IPA_BPCTL = LV_IPA_COLOR_FORMAT;
        IPA_DPCTL = LV_IPA_COLOR_FORMAT;
        IPA_IMS = ((dest_w << 16U) | (dest_h));

        /*start transfer*/
        IPA_CTL |= IPA_CTL_TEN;
    }
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_gpu_gd32_ipa_wait_cb(lv_draw_ctx_t * draw_ctx)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    if(disp->driver && disp->driver->wait_cb) {
        while(IPA_CTL & IPA_CTL_TEN) {
            if(disp->driver->wait_cb) disp->driver->wait_cb(disp->driver);
        }
    }
    else {
        while(IPA_CTL & IPA_CTL_TEN);
    }
    lv_draw_sw_wait_for_finish(draw_ctx);
}


static void invalidate_cache(void)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    if(disp->driver->clean_dcache_cb) disp->driver->clean_dcache_cb(disp->driver);
}

#endif
