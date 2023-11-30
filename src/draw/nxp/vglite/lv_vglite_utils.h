/**
 * @file lv_vglite_utils.h
 *
 */

/**
 * Copyright 2022, 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_VGLITE_UTILS_H
#define LV_VGLITE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_VGLITE
#include "../../sw/lv_draw_sw.h"

#include "vg_lite.h"

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
 * Enable scissor and set the clipping box.
 *
 * @param[in] clip_area Clip area with relative coordinates of destination buffer
 *
 */
static inline void vglite_set_scissor(const lv_area_t * clip_area);

/**
 * Disable scissor.
 *
 */
static inline void vglite_disable_scissor(void);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if LV_USE_OS
/**
 * Get VG-Lite command buffer flushed status.
 *
 */
bool vglite_cmd_buf_is_flushed(void);
#endif

/**
 * Clear cache and flush command to VG-Lite.
 *
 */
void vglite_run(void);

/**
 * Get vglite color. Premultiplies (if not hw already) and swizzles the given
 * LVGL 32bit color to obtain vglite color.
 *
 * @param[in] lv_col32 The initial LVGL 32bit color
 * @param[in] gradient True for gradient color
 *
 * @retval The vglite 32-bit color value:
 *
 */
vg_lite_color_t vglite_get_color(lv_color32_t lv_col32, bool gradient);

/**
 * Get vglite blend mode.
 *
 * @param[in] lv_blend_mode The LVGL blend mode
 *
 * @retval The vglite blend mode
 *
 */
vg_lite_blend_t vglite_get_blend_mode(lv_blend_mode_t lv_blend_mode);

/**
 * Get vglite buffer format.
 *
 * @param[in] cf Color format
 *
 * @retval The vglite buffer format
 *
 */
vg_lite_buffer_format_t vglite_get_buf_format(lv_color_format_t cf);

/**
 * Get vglite buffer pixel size.
 *
 * @param[in] cf Color format
 *
 * @retval Bits per pixel
 *
 */
uint8_t vglite_get_px_size(lv_color_format_t cf);

/**
 * Get vglite buffer alignment.
 *
 * @param[in] cf Color format
 *
 * @retval Alignment requirement in bytes
 *
 */
uint8_t vglite_get_alignment(lv_color_format_t cf);

/**
 * Check memory and stride alignment.
 *
 * @param[in] buf Buffer address
 * @param[in] stride Stride of buffer in bytes
 * @param[in] cf Color format - to calculate the expected alignment
 *
 * @retval true Alignment OK
 *
 */
bool vglite_buf_aligned(const void * buf, uint32_t stride, lv_color_format_t cf);

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void vglite_set_scissor(const lv_area_t * clip_area)
{
    vg_lite_enable_scissor();
    vg_lite_set_scissor((int32_t)clip_area->x1, (int32_t)clip_area->y1,
                        (int32_t)lv_area_get_width(clip_area),
                        (int32_t)lv_area_get_height(clip_area));
}

static inline void vglite_disable_scissor(void)
{
    vg_lite_disable_scissor();
}

#endif /*LV_USE_DRAW_VGLITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VGLITE_UTILS_H*/
