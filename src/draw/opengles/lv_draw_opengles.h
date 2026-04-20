/**
 * @file lv_draw_opengles.h
 *
 */

#ifndef LV_DRAW_OPENGLES_H
#define LV_DRAW_OPENGLES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_DRAW_OPENGLES

#include "../lv_draw.h"
#include "../../misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_opengles_init(void);
void lv_draw_opengles_deinit(void);

/**
 * Clear a region of the GPU texture that backs the given layer.
 * Needed because `lv_draw_buf_clear` operates on the CPU draw buffer,
 * which is only a dummy placeholder when the layer is rendered to a
 * GL texture. Without this, a transparent screen background leaves
 * stale pixels on the texture between frames.
 *
 * @param layer layer whose backing texture must be cleared
 * @param area  area (in layer/screen coordinates) to clear
 */
void lv_draw_opengles_clear_layer_area(lv_layer_t * layer, const lv_area_t * area);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_OPENGLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_OPENGLES_H*/
