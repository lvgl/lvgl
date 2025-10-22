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

#if LV_USE_G2D
#if LV_USE_DRAW_G2D || LV_USE_ROTATE_G2D
#include "../../sw/lv_draw_sw_private.h"
#include "g2d.h"
#include "g2dExt.h"

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

typedef enum g2d_format g2d_format_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

g2d_format_t g2d_get_buf_format(lv_color_format_t cf);

uint32_t g2d_rgba_to_u32(lv_color_t color);

int32_t g2d_get_buf_fd(const lv_draw_buf_t * draw_buf);

void g2d_set_handle(void * handle);

void * g2d_get_handle(void);

#if LV_USE_ROTATE_G2D
void g2d_rotate(lv_draw_buf_t * buf1, lv_draw_buf_t * buf2, int32_t width, int32_t height, uint32_t rotation,
                lv_color_format_t cf);
#endif
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_G2D || LV_USE_ROTATE_G2D*/
#endif /*LV_USE_G2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_G2D_UTILS_H*/