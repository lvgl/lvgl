/**
 * @file lv_g2d_utils.h
 *
 */

/**
 * Copyright 2024 NXP
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
#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_G2D
#include "../../sw/lv_draw_sw_private.h"

/*********************
 *      DEFINES
 *********************/

#if LV_USE_G2D_ASSERT
#define G2D_ASSERT(expr) LV_ASSERT(expr)
#else
#define G2D_ASSERT(expr)
#endif

#define G2D_ASSERT_MSG(expr, msg)                                    \
    do {                                                             \
        if(!(expr)) {                                                \
            LV_LOG_ERROR(msg);                                       \
            G2D_ASSERT(false);                                       \
        }                                                            \
    } while(0)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

enum g2d_format g2d_get_buf_format(lv_color_format_t cf);

uint32_t g2d_rgba_to_u32(lv_color_t color);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_G2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_G2D_UTILS_H*/