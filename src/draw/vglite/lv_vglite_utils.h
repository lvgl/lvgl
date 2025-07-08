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
#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_VGLITE
#include "../lv_draw.h"

#if LV_USE_VG_LITE_THORVG
#include "../../others/vg_lite_tvg/vg_lite.h"
#else
#include "vg_lite.h"
#include "vg_lite_options.h"
#endif

/*********************
 *      DEFINES
 *********************/
#define VLC_GET_OP_CODE(ptr) (*((uint8_t*)ptr))

#define ENUM_TO_STRING(e) \
    case (e):             \
    return #e

#define VGLITE_ENUM_TO_STRING(e) \
    case (VG_LITE_##e):           \
    return #e

#define VLC_OP_ENUM_TO_STRING(e) \
    case (VLC_OP_##e):           \
    return #e

#define FEATURE_ENUM_TO_STRING(e) \
    case (gcFEATURE_BIT_VG_##e):  \
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

#define VGLITE_ASSERT_PATH(path) VGLITE_ASSERT(VGLITE_path_check(path))
#define VGLITE_ASSERT_SRC_BUFFER(buffer) VGLITE_ASSERT(VGLITE_buffer_check(buffer, true))
#define VGLITE_ASSERT_DEST_BUFFER(buffer) VGLITE_ASSERT(VGLITE_buffer_check(buffer, false))
#define VGLITE_ASSERT_MATRIX(matrix) VGLITE_ASSERT(VGLITE_matrix_check(matrix))

#define VGLITE_IS_INDEX_FMT(fmt) \
    ((fmt) == VG_LITE_INDEX_1        \
     || (fmt) == VG_LITE_INDEX_2  \
     || (fmt) == VG_LITE_INDEX_4  \
     || (fmt) == VG_LITE_INDEX_8)

#define VGLITE_ALIGN(number, align_bytes) \
    (((number) + ((align_bytes)-1)) & ~((align_bytes)-1))
#define VGLITE_IS_ALIGNED(num, align) (((uintptr_t)(num) & ((align)-1)) == 0)

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


/** Error and parameter dump functions */
void vglite_dump_info(void);
const char * vglite_error_string(vg_lite_error_t error);
const char * vglite_feature_string(vg_lite_feature_t feature);
const char * vglite_vlc_op_string(uint8_t vlc_op);
void vglite_path_dump_info(const vg_lite_path_t * path);
void vglite_stroke_dump_info(const vg_lite_stroke_t * stroke);
void vglite_buffer_dump_info(const vg_lite_buffer_t * buffer);
void vglite_matrix_dump_info(const vg_lite_matrix_t * matrix);

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

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/




#endif /*LV_USE_DRAW_VGLITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VGLITE_UTILS_H*/
