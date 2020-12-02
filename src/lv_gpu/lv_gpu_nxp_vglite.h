/**
 * @file lv_gpu_nxp_vglite.h
 *
 */

/**
 * MIT License
 *
 * Copyright (c) 2020 NXP
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

#ifndef LV_SRC_LV_GPU_LV_GPU_NXP_VGLITE_H_
#define LV_SRC_LV_GPU_LV_GPU_NXP_VGLITE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

/** Stride in px required by VG-Lite HW. Don't change this. */
#define LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX 16

#ifndef LV_GPU_NXP_VG_LITE_FILL_SIZE_LIMIT
/** Minimum area (in pixels) to be filled by VG-Lite with 100% opacity */
#define LV_GPU_NXP_VG_LITE_FILL_SIZE_LIMIT 32
#endif

#ifndef LV_GPU_NXP_VG_LITE_FILL_OPA_SIZE_LIMIT
/** Minimum area (in pixels) to be filled by VG-Lite with transparency */
#define LV_GPU_NXP_VG_LITE_FILL_OPA_SIZE_LIMIT 32
#endif

#ifndef LV_GPU_NXP_VG_LITE_BLIT_SIZE_LIMIT
/** Minimum area (in pixels) for image copy with 100% opacity to be handled by VG-Lite */
#define LV_GPU_NXP_VG_LITE_BLIT_SIZE_LIMIT 32
#endif

#ifndef LV_GPU_NXP_VG_LITE_BLIT_OPA_SIZE_LIMIT
/** Minimum area (in pixels) for image copy with transparency to be handled by VG-Lite */
#define LV_GPU_NXP_VG_LITE_BLIT_OPA_SIZE_LIMIT 32
#endif

#ifndef LV_GPU_NXP_VG_LITE_LOG_ERRORS
/** Enable logging of VG-Lite errors (\see LV_LOG_ERROR) */
#define LV_GPU_NXP_VG_LITE_LOG_ERRORS 1
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**
 * BLock Image Transfer descriptor structure
 */
typedef struct {

    const lv_color_t * src;  /**< Source buffer pointer (must be aligned on 32 bytes) */
    lv_area_t src_area;      /**< Area to be copied from source */
    lv_coord_t src_width;    /**< Source buffer width */
    lv_coord_t src_height;   /**< Source buffer height */
    uint32_t src_stride;     /**< Source buffer stride in bytes (must be aligned on 16 px) */

    const lv_color_t * dst;  /**< Destination buffer pointer (must be aligned on 32 bytes) */
    lv_area_t dst_area;      /**< Target area in destination buffer (must be the same as src_area) */
    lv_coord_t dst_width;    /**< Destination buffer width */
    lv_coord_t dst_height;   /**< Destination buffer height */
    uint32_t dst_stride;     /**< Destination buffer stride in bytes (must be aligned on 16 px) */

    lv_opa_t opa;            /**< Opacity - alpha mix (0 = source not copied, 255 = 100% opaque) */

} lv_gpu_nxp_vglite_blit_info_t;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/***
 * Fills rectangular area in buffer.
 * @param[in] dest_buf Destination buffer pointer (must be aligned on 32 bytes)
 * @param[in] dest_width Destination buffer width in pixels ((must be aligned on 16 px)
 * @param[in] dest_height Destination buffer height in pixels
 * @param[in] fill_area Area to be filled
 * @param[in] color Fill color
 * @param[in] opa Opacity (255 = full, 128 = 50% background/50% color, 0 = no fill)
 * @retval LV_RES_OK Fill completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 */
lv_res_t lv_gpu_nxp_vglite_fill(lv_color_t * dest_buf, lv_coord_t dest_width, lv_coord_t dest_height,
                                const lv_area_t * fill_area, lv_color_t color, lv_opa_t opa);

/***
 * BLock Image Transfer.
 * @param[in] blit Description of the transfer
 * @retval LV_RES_OK Transfer complete
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 */
lv_res_t lv_gpu_nxp_vglite_blit(lv_gpu_nxp_vglite_blit_info_t * blit);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_SRC_LV_GPU_LV_GPU_NXP_VGLITE_H_ */
