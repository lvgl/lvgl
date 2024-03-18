/**
 * @file lv_api_map_v9_0.h
 *
 */

#ifndef LV_API_MAP_V9_0_H
#define LV_API_MAP_V9_0_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/
#define lv_image_set_align               lv_image_set_inner_align
#define lv_image_get_align               lv_image_get_inner_align

#ifndef LV_DRAW_LAYER_SIMPLE_BUF_SIZE
#define LV_DRAW_LAYER_SIMPLE_BUF_SIZE    LV_DRAW_SW_LAYER_SIMPLE_BUF_SIZE
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 * DEPRECATED FUNCTIONS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_API_MAP_V9_0_H*/
