/**
 * @file lv_api_map_v9_2.h
 *
 */

#ifndef LV_API_MAP_V9_2_H
#define LV_API_MAP_V9_2_H

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

/**********************
 *      MACROS
 **********************/

#define lv_display_set_draw_buffers     lv_display_set_render_draw_buffers
#define lv_display_set_buffers          lv_display_set_render_buffers

/**********************
 * DEPRECATED FUNCTIONS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_API_MAP_V9_2_H*/
