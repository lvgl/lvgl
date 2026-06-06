/**
 * @file lv_sifli_epic_utils.h
 *
 */

/**
 * Copyright 2024 SiFli Technologies
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_SIFLI_EPIC_UTILS_H
#define LV_SIFLI_EPIC_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_SIFLI_EPIC
#include "../../sw/lv_draw_sw.h"
#include "../../lv_draw_private.h"
#include "../../../stdlib/lv_string.h"
#include "lv_sifli_epic_cfg.h"

/*********************
 *      DEFINES
 *********************/

#if LV_USE_SIFLI_EPIC_ASSERT
#define EPIC_ASSERT(expr) LV_ASSERT(expr)
#else
#define EPIC_ASSERT(expr)
#endif

#define EPIC_ASSERT_MSG(expr, msg)                                   \
    do {                                                             \
        if(!(expr)) {                                                \
            LV_LOG_ERROR(msg);                                       \
            EPIC_ASSERT(false);                                      \
        }                                                            \
    } while(0)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Convert LVGL color format to EPIC color format.
 * @param cf LVGL color format
 * @return EPIC color format constant
 */
uint32_t lv_img_cf_to_epic_cf(lv_color_format_t cf);

/**
 * Convert LVGL color to EPIC color structure.
 * @param color LVGL color
 * @param opa Opacity value
 * @return EPIC color structure
 */
EPIC_ColorDef lv_color_to_epic_color(lv_color_t color, lv_opa_t opa);

/**
 * Setup background and output layers for EPIC operation.
 * This function configures the EPIC layers based on the draw task.
 *
 * @param epic_bg_layer Pointer to background layer configuration
 * @param epic_output_layer Pointer to output layer configuration
 * @param draw_task Draw task containing layer information
 * @param coords Coordinates to draw
 * @return 0 on success, non-zero if fully clipped
 */
uint32_t lv_epic_setup_layers(EPIC_LayerConfigTypeDef *epic_bg_layer,
EPIC_LayerConfigTypeDef *epic_output_layer,
lv_draw_task_t * draw_task,
const lv_area_t * coords);

/**
 * Check if color format is supported by EPIC.
 * @param cf LVGL color format
 * @param flags Image flags
 * @return true if supported, false otherwise
 */
bool lv_epic_cf_supported(lv_color_format_t cf, uint32_t flags);

/**
 * Convert a stride in bytes to a width in pixels for a given color format.
 * @param stride Byte stride
 * @param cf LVGL color format
 * @return Width in pixels
 */
uint16_t lv_epic_stride_to_width(uint32_t stride, lv_color_format_t cf);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SIFLI_EPIC*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SIFLI_EPIC_UTILS_H*/
