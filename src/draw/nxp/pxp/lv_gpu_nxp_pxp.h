/**
 * @file lv_gpu_nxp_pxp.h
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

#ifndef LV_GPU_NXP_PXP_H
#define LV_GPU_NXP_PXP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GPU_NXP_PXP
#include "fsl_cache.h"
#include "fsl_pxp.h"

#include "../../../misc/lv_log.h"

/*********************
 *      DEFINES
 *********************/

/** PXP module instance to use*/
#define LV_GPU_NXP_PXP_ID PXP

/** PXP interrupt line ID*/
#define LV_GPU_NXP_PXP_IRQ_ID PXP_IRQn

#ifndef LV_GPU_NXP_PXP_LOG_ERRORS
/** Enable logging of PXP errors (\see LV_LOG_ERROR)*/
#define LV_GPU_NXP_PXP_LOG_ERRORS 1
#endif

#ifndef LV_GPU_NXP_PXP_LOG_TRACES
/** Enable logging of PXP errors (\see LV_LOG_ERROR)*/
#define LV_GPU_NXP_PXP_LOG_TRACES 0
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
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Reset and initialize PXP device. This function should be called as a part
 * of display init sequence.
 *
 * @retval LV_RES_OK PXP init completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_PXP_LOG_ERRORS)
 */
lv_res_t lv_gpu_nxp_pxp_init(void);

/**
 * Disable PXP device. Should be called during display deinit sequence.
 */
void lv_gpu_nxp_pxp_deinit(void);

/**
 * Start PXP job and wait for completion.
 */
void lv_gpu_nxp_pxp_run(void);

/**********************
 *      MACROS
 **********************/

#define PXP_COND_STOP(cond, txt)              \
    do {                                      \
        if (cond) {                           \
            LV_LOG_ERROR("%s. STOP!", txt);   \
            for ( ; ; );                      \
        }                                     \
    } while(0)

#if LV_GPU_NXP_PXP_LOG_ERRORS
#define PXP_RETURN_INV(fmt, ...)              \
    do {                                      \
        LV_LOG_ERROR(fmt, ##__VA_ARGS__);     \
        return LV_RES_INV;                    \
    } while (0)
#else
#define PXP_RETURN_INV(fmt, ...)              \
    do {                                      \
        return LV_RES_INV;                    \
    }while(0)
#endif /*LV_GPU_NXP_PXP_LOG_ERRORS*/

#if LV_GPU_NXP_PXP_LOG_TRACES
#define PXP_LOG_TRACE(fmt, ...)               \
    do {                                      \
        LV_LOG_ERROR(fmt, ##__VA_ARGS__);     \
    } while (0)
#else
#define PXP_LOG_TRACE(fmt, ...)               \
    do {                                      \
    } while (0)
#endif /*LV_GPU_NXP_PXP_LOG_TRACES*/

#endif /*LV_USE_GPU_NXP_PXP*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GPU_NXP_PXP_H*/
