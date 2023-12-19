/**
 * @file lv_draw_buf_pxp.c
 *
 */

/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_pxp.h"

#if LV_USE_DRAW_PXP
#include "lv_pxp_cfg.h"
#include "lv_pxp_utils.h"

#include "lvgl_support.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _invalidate_cache(lv_draw_buf_t * draw_buf, const char * area);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_buf_pxp_init_handlers(void)
{
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();

    handlers->invalidate_cache_cb = _invalidate_cache;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _invalidate_cache(lv_draw_buf_t * draw_buf, const char * area)
{
    LV_UNUSED(draw_buf);
    LV_UNUSED(area);

    DEMO_CleanInvalidateCache();
}

#if 0
/**
 * @todo
 * LVGL needs to use hardware acceleration for buf_copy and do not affect GPU rendering.
 */

void _pxp_buf_copy(void * dest_buf, uint32_t dest_stride, const lv_area_t * dest_area,
                   void * src_buf, uint32_t src_stride, const lv_area_t * src_area,
                   lv_color_format_t cf)
{
    lv_pxp_reset();

    const pxp_pic_copy_config_t picCopyConfig = {
        .srcPicBaseAddr = (uint32_t)src_buf,
        .srcPitchBytes = src_stride,
        .srcOffsetX = src_area->x1,
        .srcOffsetY = src_area->y1,
        .destPicBaseAddr = (uint32_t)dest_buf,
        .destPitchBytes = dest_stride,
        .destOffsetX = dest_area->x1,
        .destOffsetY = dest_area->y1,
        .width = lv_area_get_width(src_area),
        .height = lv_area_get_height(src_area),
        .pixelFormat = pxp_get_as_px_format(cf)
    };

    PXP_StartPictureCopy(PXP_ID, &picCopyConfig);

    lv_pxp_run();
}
#endif

#endif /*LV_USE_DRAW_PXP*/
