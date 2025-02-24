/**
 * @file lv_vglite_buf.h
 *
 */

/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_VGLITE_BUF_H
#define LV_VGLITE_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_VGLITE
#include "../../lv_draw.h"

#include "vg_lite.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get vglite destination buffer pointer.
 *
 * @retval The vglite destination buffer
 *
 */
vg_lite_buffer_t * vglite_get_dest_buf(void);

/**
 * Get vglite source buffer pointer.
 *
 * @retval The vglite source buffer
 *
 */
vg_lite_buffer_t * vglite_get_src_buf(void);

/**
 * Set vglite destination buffer address only.
 *
 * @param[in] buf Destination buffer address (does not require alignment for VG_LITE_LINEAR mode)
 *
 */
void vglite_set_dest_buf_ptr(void * buf);

/**
 * Set vglite source buffer address only.
 *
 * @param[in] buf Source buffer address
 *
 */
void vglite_set_src_buf_ptr(const void * buf);

/**
 * Set vglite destination buffer.
 *
 * @param[in] buf Destination buffer address
 * @param[in] width Destination buffer width
 * @param[in] height Destination buffer height
 * @param[in] stride Destination buffer stride in bytes
 * @param[in] cf Destination buffer color format
 *
 */
void vglite_set_dest_buf(const void * buf, uint32_t width, uint32_t height, uint32_t stride,
                         lv_color_format_t cf);

/**
 * Set vglite source buffer.
 *
 * @param[in] buf Source buffer address
 * @param[in] width Source buffer width
 * @param[in] height Source buffer height
 * @param[in] stride Source buffer stride in bytes
 * @param[in] cf Source buffer color format
 *
 */
void vglite_set_src_buf(const void * buf, uint32_t width, uint32_t height, uint32_t stride,
                        lv_color_format_t cf);

/**
 * Set vglite buffer.
 *
 * @param[in] vgbuf Address of the VGLite buffer object
 * @param[in] buf Address of the memory for the VGLite buffer
 * @param[in] width Buffer width
 * @param[in] height Buffer height
 * @param[in] stride Buffer stride in bytes
 * @param[in] cf Buffer color format
 *
 */
void vglite_set_buf(vg_lite_buffer_t * vgbuf, void * buf,
                    uint32_t width, uint32_t height, uint32_t stride,
                    lv_color_format_t cf);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_VGLITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VGLITE_BUF_H*/
