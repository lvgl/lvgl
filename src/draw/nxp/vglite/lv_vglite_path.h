/**
 * @file lv_vglite_path.h
 *
 */

/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_VGLITE_PATH_H
#define LV_VGLITE_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_VGLITE
#include "../../lv_draw.h"
#include "../../lv_draw_triangle.h"

/*********************
 *      DEFINES
 *********************/

/* The optimal Bezier control point offset for radial unit
 * see: https://spencermortensen.com/articles/bezier-circle/
 **/
#define BEZIER_OPTIM_CIRCLE 0.551915024494f

/* Draw lines for control points of Bezier curves */
#define BEZIER_DBG_CONTROL_POINTS   0

/* Path data sizes for different elements */
#define CUBIC_PATH_DATA_SIZE 7 /* 1 opcode, 6 arguments */
#define LINE_PATH_DATA_SIZE 3  /* 1 opcode, 2 arguments */
#define MOVE_PATH_DATA_SIZE 3  /* 1 opcode, 2 arguments */
#define END_PATH_DATA_SIZE 1   /* 1 opcode, 0 arguments */
/* Maximum possible rectangle path size
 * is in the rounded rectangle case:
 * - 1 move for the path start
 * - 4 cubics for the corners
 * - 4 lines for the sides
 * - 1 end for the path end */
#define RECT_PATH_DATA_MAX_SIZE (1 * MOVE_PATH_DATA_SIZE + 4 * CUBIC_PATH_DATA_SIZE + 4 * LINE_PATH_DATA_SIZE + 1 * END_PATH_DATA_SIZE)

/* Maximum possible arc path size
 * is in the rounded arc case:
 * - 1 move for the path start
 * - 16 cubics for the arc (5 inner, 5 outer) and corners (3 per corner)
 * - 1 end for the path end */
#define ARC_PATH_DATA_MAX_SIZE (1 * MOVE_PATH_DATA_SIZE + 16 * CUBIC_PATH_DATA_SIZE + 1 * END_PATH_DATA_SIZE)
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Generates path data for rectangle drawing.
 *
 * @param[in/out] path The path data to initialize
 * @param[in/out] path_size The resulting size of the created path data
 * @param[in] dsc The style descriptor for the rectangle to be drawn
 * @param[in] coords The coordinates of the rectangle to be drawn
 *
 */
void vglite_create_rect_path_data(int32_t * path_data, uint32_t * path_data_size,
                                  int32_t radius,
                                  const lv_area_t * coords);

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

#endif /*LV_VGLITE_PATH_H*/
