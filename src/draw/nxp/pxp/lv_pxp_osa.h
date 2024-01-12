/**
 * @file lv_pxp_osa.h
 *
 */

/**
 * Copyright 2020, 2022-2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_PXP_OSA_H
#define LV_PXP_OSA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_PXP
#include "lv_pxp_cfg.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * PXP device interrupt handler. Used to check PXP task completion status.
 */
void PXP_IRQHandler(void);

/**
 * Get the PXP default configuration.
 */
pxp_cfg_t * pxp_get_default_cfg(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_PXP*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PXP_OSA_H*/
