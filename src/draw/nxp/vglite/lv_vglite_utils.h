/**
 * @file lv_vglite_utils.h
 *
 */

/**
 * Copyright 2022-2024 NXP
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
#include "../../lv_draw.h"

#include "vg_lite.h"
#include "vg_lite_options.h"

/*********************
 *      DEFINES
 *********************/

#define ENUM_TO_STRING(e) \
    case (e):             \
    return #e

#if LV_USE_VGLITE_ASSERT
#define VGLITE_ASSERT(expr) LV_ASSERT(expr)
#else
#define VGLITE_ASSERT(expr)
#endif

#define VGLITE_ASSERT_MSG(expr, msg)                                 \
    do {                                                             \
        if(!(expr)) {                                                \
            LV_LOG_ERROR(msg);                                       \
            VGLITE_ASSERT(false);                                    \
        }                                                            \
    } while(0)

#if LV_USE_VGLITE_CHECK_ERROR
#define VGLITE_CHECK_ERROR(function)                                 \
    do {                                                             \
        vg_lite_error_t error = function;                            \
        if(error != VG_LITE_SUCCESS) {                               \
            LV_LOG_ERROR("Execute '" #function "' error(%d): %s",    \
                         (int)error, vglite_error_to_string(error)); \
            VGLITE_ASSERT(false);                                    \
        }                                                            \
    } while (0)
#else
#define VGLITE_CHECK_ERROR(function) function
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * Set the clipping box.
 *
 * @param[in] clip_area Clip area with relative coordinates of destination buffer
 *
 */
static inline void vglite_set_scissor(const lv_area_t * clip_area);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

const char * vglite_error_to_string(vg_lite_error_t error);

#if LV_USE_VGLITE_DRAW_ASYNC
/**
 * Get VG-Lite command buffer flushed status.
 *
 */
bool vglite_cmd_buf_is_flushed(void);
#endif

/**
 * Flush command to VG-Lite.
 *
 */
void vglite_run(void);

/**
 * Wait for VG-Lite finish.
 *
 */
#if LV_USE_VGLITE_DRAW_ASYNC
void vglite_wait_for_finish(void);
#endif

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
 * Get vglite stride alignment.
 *
 * @param[in] cf Color format
 *
 * @retval Alignment requirement in bytes
 *
 */
uint8_t vglite_get_stride_alignment(lv_color_format_t cf);

/**
 * Check source start address and stride alignment.
 *
 * @param[in] buf Buffer address
 * @param[in] stride Stride of buffer in bytes
 * @param[in] cf Color format - to calculate the expected alignment
 *
 * @retval true Alignment OK
 *
 */
bool vglite_src_buf_aligned(const void * buf, uint32_t stride, lv_color_format_t cf);

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void vglite_set_scissor(const lv_area_t * clip_area)
{
    vg_lite_set_scissor(clip_area->x1, clip_area->y1, clip_area->x2 + 1, clip_area->y2 + 1);
}

#endif /*LV_USE_DRAW_VGLITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VGLITE_UTILS_H*/
