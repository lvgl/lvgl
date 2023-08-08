/**
 * @file lv_draw_pxp_buffer_copy.c
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

void lv_draw_pxp_buffer_copy(lv_layer_t * layer,
                             void * dest_buf, lv_coord_t dest_stride, const lv_area_t * dest_area,
                             void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area)
{
    lv_color_format_t cf = layer->color_format;
    lv_coord_t src_width = lv_area_get_width(src_area);
    lv_coord_t src_height = lv_area_get_height(src_area);

    lv_pxp_reset();

    uint8_t px_size = lv_color_format_get_size(cf);

    const pxp_pic_copy_config_t picCopyConfig = {
        .srcPicBaseAddr = (uint32_t)src_buf,
        .srcPitchBytes = src_stride * px_size,
        .srcOffsetX = src_area->x1,
        .srcOffsetY = src_area->y1,
        .destPicBaseAddr = (uint32_t)dest_buf,
        .destPitchBytes = dest_stride * px_size,
        .destOffsetX = dest_area->x1,
        .destOffsetY = dest_area->y1,
        .width = src_width,
        .height = src_height,
        .pixelFormat = pxp_get_as_px_format(cf)
    };

    PXP_StartPictureCopy(PXP_ID, &picCopyConfig);

    lv_pxp_run();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_PXP*/
