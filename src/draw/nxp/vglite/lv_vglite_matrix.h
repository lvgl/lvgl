/**
 * @file lv_vglite_matrix.h
 *
 */

/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_VGLITE_MATRIX_H
#define LV_VGLITE_MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_VGLITE

#include "../../lv_draw_image.h"
#include "../../lv_draw.h"

#include "vg_lite.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

vg_lite_matrix_t * vglite_get_matrix(void);

/**
 * Creates matrix that translates to origin of given destination area.
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 *
 */
void vglite_set_translation_matrix(const lv_area_t * dest_area);

/**
 * Creates matrix that translates to origin of given destination area with transformation (scale or rotate).
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] dsc Image descriptor
 *
 */
void vglite_set_transformation_matrix(const lv_area_t * dest_area, const lv_draw_image_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VGLITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VGLITE_MATRIX_H*/
