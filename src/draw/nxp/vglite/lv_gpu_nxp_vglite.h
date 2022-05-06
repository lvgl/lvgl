/**
 * @file lv_gpu_nxp_vglite.h
 *
 */

/**
 * MIT License
 *
 * Copyright 2020-2022 NXP
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

#ifndef LV_GPU_NXP_VGLITE_H
#define LV_GPU_NXP_VGLITE_H

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
#include "fsl_debug_console.h"

/*********************
 *      DEFINES
 *********************/

/** Use this symbol as limit to disable feature (value has to be larger than supported resolution) */
#define LV_GPU_NXP_VG_LITE_FEATURE_DISABLED (1920*1080+1)

/** Stride in px required by VG-Lite HW. Don't change this. */
#define LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX 16U

#ifndef LV_GPU_NXP_VG_LITE_LOG_ERRORS
/** Enable logging of VG-Lite errors (\see LV_LOG_ERROR)*/
#define LV_GPU_NXP_VG_LITE_LOG_ERRORS 1
#endif

#ifndef LV_GPU_NXP_VG_LITE_LOG_TRACES
/** Enable logging of VG-Lite errors (\see LV_LOG_ERROR)*/
#define LV_GPU_NXP_VG_LITE_LOG_TRACES 0
#endif

/* Draw rectangles around BLIT tiles */
#define BLIT_DBG_AREAS   0

/* Print detailed info to SDK console (NOT to LVGL log system) */
#define BLIT_DBG_VERBOSE 0

/* Verbose debug print */
#if BLIT_DBG_VERBOSE
#define PRINT_BLT PRINTF
#else
#define PRINT_BLT(...)
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
 * Fills vg_lite_buffer_t structure according given parameters.
 *
 * @param[in/out] vgbuf Buffer structure to be filled
 * @param[in] width Width of buffer in pixels
 * @param[in] height Height of buffer in pixels
 * @param[in] stride Stride of the buffer in bytes
 * @param[in] ptr Pointer to the buffer (must be aligned according VG-Lite requirements)
 * @param[in] source Boolean to check if this is a source buffer
 */
lv_res_t lv_vglite_init_buf(vg_lite_buffer_t * vgbuf, uint32_t width, uint32_t height, uint32_t stride,
                            const lv_color_t * ptr, bool source);

#if BLIT_DBG_AREAS
/**
 * Draw a simple rectangle, 1 px line width.
 *
 * @param dest_buf Destination buffer
 * @param dest_width Destination buffer width (must be aligned on 16px)
 * @param dest_height Destination buffer height
 * @param fill_area Rectangle coordinates
 * @param color Rectangle color
 */
void lv_vglite_dbg_draw_rectangle(lv_color_t * dest_buf, lv_coord_t dest_width, lv_coord_t dest_height,
                                  lv_area_t * fill_area, lv_color_t color);
#endif

/**
 * Clean & invalidate cache.
 */
void lv_vglite_invalidate_cache(void);

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
            LV_LOG_ERROR(fmt, ##__VA_ARGS__); \
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
        LV_LOG_ERROR(fmt, ##__VA_ARGS__);     \
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

#endif /*LV_GPU_NXP_VGLITE_H*/
