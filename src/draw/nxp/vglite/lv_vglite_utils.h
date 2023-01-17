/**
 * @file lv_vglite_utils.h
 *
 */

/**
 * MIT License
 *
 * Copyright 2022, 2023 NXP
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next paragraph)
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
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

#if LV_USE_GPU_NXP_VG_LITE
#include "vg_lite.h"
#include "../../sw/lv_draw_sw.h"
#include "../../../misc/lv_log.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LV_GPU_NXP_VG_LITE_LOG_ERRORS
/** Enable logging of VG-Lite errors (\see LV_LOG_ERROR)*/
#define LV_GPU_NXP_VG_LITE_LOG_ERRORS 1
#endif

#ifndef LV_GPU_NXP_VG_LITE_LOG_TRACES
/** Enable logging of VG-Lite traces (\see LV_LOG_ERROR)*/
#define LV_GPU_NXP_VG_LITE_LOG_TRACES 0
#endif


/* The optimal Bezier control point offset for radial unit
 * see: https://spencermortensen.com/articles/bezier-circle/
 **/
#define BEZIER_OPTIM_CIRCLE 0.551915024494f

/* Draw lines for control points of Bezier curves */
#define BEZIER_DBG_CONTROL_POINTS   0

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Premultiplies and swizzles given LVGL 32bit color to obtain vglite color.
 *
 * @param[in/out] vg_col32 The obtained vglite color
 * @param[in] lv_col32 The initial LVGL 32bit color
 * @param[in] opa The opacity to premultiply with
 * @param[in] vg_col_format The format of the resulting vglite color
 *
 * @retval LV_RES_OK Operation completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 */
lv_res_t lv_vglite_premult_and_swizzle(vg_lite_color_t * vg_col32, lv_color32_t lv_col32, lv_opa_t opa,
                                       vg_lite_buffer_format_t vg_col_format);

/**
 * Get vglite blend mode.
 *
 * @param[in] lv_blend_mode The LVGL blend mode
 *
 * @retval The vglite blend mode
 */
vg_lite_blend_t lv_vglite_get_blend_mode(lv_blend_mode_t lv_blend_mode);

/**
 * Clear cache and flush command to VG-Lite.
 *
 * @retval LV_RES_OK Run completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 */
lv_res_t lv_vglite_run(void);

/**********************
 *      MACROS
 **********************/

#define VG_LITE_COND_STOP(cond, txt)          \
    do {                                      \
        if (cond) {                           \
            LV_LOG_ERROR("%s. STOP!", txt);   \
            for ( ; ; );                      \
        }                                     \
    } while(0)

#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
#define VG_LITE_ERR_RETURN_INV(err, fmt, ...) \
    do {                                      \
        if(err != VG_LITE_SUCCESS) {          \
            LV_LOG_ERROR(fmt" (err = %d)",    \
                         err, ##__VA_ARGS__); \
            return LV_RES_INV;                \
        }                                     \
    } while (0)
#else
#define VG_LITE_ERR_RETURN_INV(err, fmt, ...) \
    do {                                      \
        if(err != VG_LITE_SUCCESS) {          \
            return LV_RES_INV;                \
        }                                     \
    }while(0)
#endif /*LV_GPU_NXP_VG_LITE_LOG_ERRORS*/

#if LV_GPU_NXP_VG_LITE_LOG_TRACES
#define VG_LITE_LOG_TRACE(fmt, ...)           \
    do {                                      \
        LV_LOG(fmt, ##__VA_ARGS__);     \
    } while (0)

#define VG_LITE_RETURN_INV(fmt, ...)          \
    do {                                      \
        LV_LOG_ERROR(fmt, ##__VA_ARGS__);     \
        return LV_RES_INV;                    \
    } while (0)
#else
#define VG_LITE_LOG_TRACE(fmt, ...)           \
    do {                                      \
    } while (0)
#define VG_LITE_RETURN_INV(fmt, ...)          \
    do {                                      \
        return LV_RES_INV;                    \
    }while(0)
#endif /*LV_GPU_NXP_VG_LITE_LOG_TRACES*/

#endif /*LV_USE_GPU_NXP_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VGLITE_UTILS_H*/
