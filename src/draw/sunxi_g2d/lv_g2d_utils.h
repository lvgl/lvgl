/**
 * @file lv_g2d_utils.h
 *
 */

/**
 * Copyright 2023，2024 G2D
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_G2D_UTILS_H
#define LV_G2D_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_G2D
#include "g2d_driver.h"
#include "../../misc/lv_color.h"
#include "../../stdlib/lv_string.h"
#include "hal_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    SUNXI_G2D_LIMIT_FILL,
    SUNXI_G2D_LIMIT_OPA_FILL,
    SUNXI_G2D_LIMIT_BLIT,
    SUNXI_G2D_LIMIT_BLEND,
    SUNXI_G2D_LIMIT_SCALE
} sunxi_g2d_limit;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

int32_t sunxifb_g2d_get_limit(sunxi_g2d_limit limit);

g2d_fmt_enh g2d_get_px_format(lv_color_format_t cf);

int lv_g2d_init(void);

void lv_g2d_deinit(void);

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_G2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_G2D_UTILS_H*/
