/**
 * @file lv_draw_vglite_arc.h
 *
 */

/**
 * MIT License
 *
 * Copyright 2021, 2022 NXP
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

#ifndef LV_DRAW_VGLITE_ARC_H
#define LV_DRAW_VGLITE_ARC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_GPU_NXP_VG_LITE
#include "lv_gpu_nxp_vglite.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/***
 * Draw arc shape with effects
 * @param draw_ctx drawing context
 * @param dsc the arc description structure (width, rounded ending, opacity)
 * @param center the coordinates of the arc center
 * @param radius the radius of external arc
 * @param start_angle the starting angle in degrees
 * @param end_angle the ending angle in degrees
 */
lv_res_t lv_gpu_nxp_vglite_draw_arc(lv_draw_ctx_t * draw_ctx, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center,
                                    int32_t radius, int32_t start_angle, int32_t end_angle);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_NXP_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_VGLITE_ARC_H*/
