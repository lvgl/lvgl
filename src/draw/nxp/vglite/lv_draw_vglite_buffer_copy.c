/**
 * @file lv_draw_vglite_buffer_copy.c
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

#include "lv_draw_vglite.h"

#if LV_USE_DRAW_VGLITE
#include "lv_vglite_buf.h"
#include "lv_vglite_matrix.h"
#include "lv_vglite_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * BLock Image Transfer - simple copy of rectangular image from source to destination.
 *
 * @param[in] dest_buf Destination buffer
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] dest_stride Stride of destination buffer in pixels
 * @param[in] src_buf Source buffer
 * @param[in] src_area Source area with relative coordinates of source buffer
 * @param[in] src_stride Stride of source buffer in pixels
 * @param[in] Color format
 *
 */
static void _vglite_buffer_copy(uint8_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                const uint8_t * src_buf, const lv_area_t * src_area, lv_coord_t src_stride,
                                lv_color_format_t cf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vglite_buffer_copy(lv_layer_t * layer,
                                void * dest_buf, lv_coord_t dest_stride, const lv_area_t * dest_area,
                                void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area)
{
    lv_color_format_t cf = layer->color_format;

    _vglite_buffer_copy(dest_buf, dest_area, dest_stride, src_buf, src_area, src_stride, cf);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _vglite_buffer_copy(uint8_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                const uint8_t * src_buf, const lv_area_t * src_area, lv_coord_t src_stride,
                                lv_color_format_t cf)
{
    vg_lite_buffer_format_t vgformat = vglite_get_buf_format(cf);

    /* Set src_vgbuf structure. */
    vg_lite_buffer_t src_vgbuf;
    vglite_set_buf(&src_vgbuf, src_buf, src_area, src_stride, vgformat);

    /* Set dest_vgbuf structure. */
    vg_lite_buffer_t dest_vgbuf;
    vglite_set_buf(&dest_vgbuf, dest_buf, dest_area, dest_stride, vgformat);

    uint32_t rect[] = {
        (uint32_t)src_area->x1, /* start x */
        (uint32_t)src_area->y1, /* start y */
        (uint32_t)lv_area_get_width(src_area), /* width */
        (uint32_t)lv_area_get_height(src_area) /* height */
    };

    /* Set scissor. */
    vglite_set_scissor(dest_area);

    /* Set vgmatrix. */
    vglite_set_translation_matrix(dest_area);
    vg_lite_matrix_t * vgmatrix = vglite_get_matrix();

    vg_lite_error_t err = vg_lite_blit_rect(&dest_vgbuf, &src_vgbuf, rect, vgmatrix,
                                            VG_LITE_BLEND_NONE, 0xFFFFFFFFU, VG_LITE_FILTER_POINT);
    LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Blit rectangle failed.");

    vglite_run();

    /* Disable scissor. */
    vglite_disable_scissor();
}

#endif /*LV_USE_DRAW_VGLITE*/
