/**
 * @file lv_gpu_nxp_pxp.h
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

#ifndef LV_SRC_LV_GPU_LV_GPU_NXP_PXP_H_
#define LV_SRC_LV_GPU_LV_GPU_NXP_PXP_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_NXP_PXP

#include "../misc/lv_area.h"
#include "../misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/** PXP module instance to use*/
#define LV_GPU_NXP_PXP_ID PXP

/** PXP interrupt line I*/
#define LV_GPU_NXP_PXP_IRQ_ID PXP_IRQn

#ifndef LV_GPU_NXP_PXP_BLIT_SIZE_LIMIT
/** Minimum area (in pixels) for image copy with 100% opacity to be handled by PXP*/
#define LV_GPU_NXP_PXP_BLIT_SIZE_LIMIT 32
#endif

#ifndef LV_GPU_NXP_PXP_BLIT_OPA_SIZE_LIMIT
/** Minimum area (in pixels) for image copy with transparency to be handled by PXP*/
#define LV_GPU_NXP_PXP_BLIT_OPA_SIZE_LIMIT 16
#endif

#ifndef LV_GPU_NXP_PXP_BUFF_SYNC_BLIT_SIZE_LIMIT
/** Minimum invalidated area (in pixels) to be synchronized by PXP during buffer sync */
#define LV_GPU_NXP_PXP_BUFF_SYNC_BLIT_SIZE_LIMIT 32
#endif

#ifndef LV_GPU_NXP_PXP_FILL_SIZE_LIMIT
/** Minimum area (in pixels) to be filled by PXP with 100% opacity*/
#define LV_GPU_NXP_PXP_FILL_SIZE_LIMIT 64
#endif

#ifndef LV_GPU_NXP_PXP_FILL_OPA_SIZE_LIMIT
/** Minimum area (in pixels) to be filled by PXP with transparency*/
#define LV_GPU_NXP_PXP_FILL_OPA_SIZE_LIMIT 32
#endif

/**********************
 *      TYPEDEFS
 **********************/
/**
 * NXP PXP device configuration - call-backs used for
 * interrupt init/wait/deinit.
 */
typedef struct {
    /** Callback for PXP interrupt initialization*/
    lv_res_t (*pxp_interrupt_init)(void);

    /** Callback for PXP interrupt de-initialization*/
    void (*pxp_interrupt_deinit)(void);

    /** Callback that should start PXP and wait for operation complete*/
    void (*pxp_run)(void);
} lv_nxp_pxp_cfg_t;

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Reset and initialize PXP device. This function should be called as a part
 * of display init sequence.
 *
 * @return LV_RES_OK: PXP init ok; LV_RES_INV: init error. See error log for more information.
 */
lv_res_t lv_gpu_nxp_pxp_init(lv_nxp_pxp_cfg_t * cfg);

/**
 * Disable PXP device. Should be called during display deinit sequence.
 */
void lv_gpu_nxp_pxp_deinit(void);

/**
 * Fill area, with optional opacity.
 *
 * @param[in/out] dest_buf destination buffer
 * @param[in] dest_width width (stride) of destination buffer in pixels
 * @param[in] fill_area area to fill
 * @param[in] color color
 * @param[in] opa transparency of the color
 */
void lv_gpu_nxp_pxp_fill(lv_color_t * dest_buf, lv_coord_t dest_width, const lv_area_t * fill_area, lv_color_t color,
                         lv_opa_t opa);

/**
 * @brief BLock Image Transfer - copy rectangular image from src buffer to dst buffer with effects.
 *
 * By default, image is copied directly, with optional opacity configured by \p opa.
 * Color keying can be enabled by calling lv_gpu_nxp_pxp_enable_color_key() before calling this function.
 * Recoloring can be enabled by calling lv_gpu_nxp_pxp_enable_recolor() before calling this function.
 * Note that color keying and recoloring at the same time is not supported and black rectangle is rendered.
 *
 * @param[in/out] dest destination buffer
 * @param[in] dest_width width (stride) of destination buffer in pixels
 * @param[in] src source buffer
 * @param[in] src_with width (stride) of source buffer in pixels
 * @param[in] copy_w width of area to be copied from src to dest
 * @param[in] copy_h height of area to be copied from src to dest
 * @param[in] opa opacity of the result
 */
void lv_gpu_nxp_pxp_blit(lv_color_t * dest, lv_coord_t dest_width, const lv_color_t * src, lv_coord_t src_width,
                         lv_coord_t copy_width, lv_coord_t copy_height, lv_opa_t opa);

/**
 * @brief Enable color keying for subsequent calls to lv_gpu_nxp_pxp_blit()
 *
 * Color key is defined by LV_COLOR_TRANSP symbol in lv_conf.h
 */
void lv_gpu_nxp_pxp_enable_color_key(void);

/**
 * @brief Disable color keying for subsequent calls to lv_gpu_nxp_pxp_blit()
 *
 */
void lv_gpu_nxp_pxp_disable_color_key(void);

/**
 * @brief Enable recolor feature for subsequent calls to lv_gpu_nxp_pxp_blit()
 *
 * @param[in] color recolor value
 * @param[in] opa effect opacity
 */
void lv_gpu_nxp_pxp_enable_recolor(lv_color_t color, lv_opa_t opa);

/**
 * @brief Disable recolor feature for subsequent calls to lv_gpu_nxp_pxp_blit()
 */
void lv_gpu_nxp_pxp_disable_recolor(void);

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_NXP_PXP*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SRC_LV_GPU_LV_GPU_NXP_PXP_H_*/
