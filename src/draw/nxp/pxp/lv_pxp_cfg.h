/**
 * @file lv_pxp_cfg.h
 *
 */

/**
 * Copyright 2020-2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_PXP_CFG_H
#define LV_PXP_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_PXP
#if LV_USE_DRAW_PXP || LV_USE_ROTATE_PXP
#include "fsl_cache.h"
#include "fsl_pxp.h"

#if ((LV_DRAW_BUF_ALIGN % 32) != 0)
#error "If PXP is enabled the draw buffers should be aligned to 32-byte boundary, please set LV_DRAW_BUF_ALIGN to a multiple of 32 in lv_conf.h"
#endif

#include "../../../misc/lv_log.h"

/*********************
 *      DEFINES
 *********************/

/** PXP module instance to use*/
#define PXP_ID PXP

/** PXP interrupt line ID*/
#define PXP_IRQ_ID PXP_IRQn

/**********************
 *      TYPEDEFS
 **********************/

/**
 * NXP PXP device configuration.
 */
typedef struct {
    /** Callback for PXP interrupt initialization*/
    void (*pxp_interrupt_init)(void);

    /** Callback for PXP interrupt de-initialization*/
    void (*pxp_interrupt_deinit)(void);

    /** Callback for PXP start*/
    void (*pxp_run)(void);

    /** Callback for waiting of PXP completion*/
    void (*pxp_wait)(void);
} pxp_cfg_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Reset and initialize PXP device. This function should be called as a part
 * of display init sequence.
 */
void lv_pxp_init(void);

/**
 * Disable PXP device. Should be called during display deinit sequence.
 */
void lv_pxp_deinit(void);

/**
 * Reset PXP device.
 */
void lv_pxp_reset(void);

/**
 * Clear cache and start PXP.
 */
void lv_pxp_run(void);

/**
 * Wait for PXP completion.
 */
void lv_pxp_wait(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_PXP || LV_USE_ROTATE_PXP*/
#endif /*LV_USE_PXP*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PXP_CFG_H*/
