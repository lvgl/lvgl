/**
 * @file lv_vg_lite_stroke.h
 *
 */

#ifndef LV_VG_LITE_STROKE_H
#define LV_VG_LITE_STROKE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_vg_lite_utils.h"

#if LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_vg_lite_path_t;

struct _lv_draw_vg_lite_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the stroke module
 * @param unit pointer to the unit
 */
void lv_vg_lite_stroke_init(struct _lv_draw_vg_lite_unit_t * unit, uint32_t cache_cnt);

/**
 * Deinitialize the stroke module
 * @param unit pointer to the unit
 */
void lv_vg_lite_stroke_deinit(struct _lv_draw_vg_lite_unit_t * unit);

/**
 * Get the stroke cache entry
 * @param unit pointer to the unit
 * @param path pointer to the path
 * @param dsc pointer to the stroke descriptor
 * @return pointer to the stroke cache entry
 */
lv_cache_entry_t * lv_vg_lite_stroke_get(struct _lv_draw_vg_lite_unit_t * unit,
                                         struct _lv_vg_lite_path_t * path,
                                         const lv_vector_stroke_dsc_t * dsc);

/**
 * Get the path of a stroke
 * @param cache_entry pointer to the stroke cache entry
 * @return pointer to the path
 */
struct _lv_vg_lite_path_t * lv_vg_lite_stroke_get_path(lv_cache_entry_t * cache_entry);

/**
 * Drop the stroke cache entry
 * @param unit pointer to the unit
 * @param stroke pointer to the stroke
 */
void lv_vg_lite_stroke_drop(struct _lv_draw_vg_lite_unit_t * unit, lv_cache_entry_t * cache_entry);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VG_LITE_STROKE_H*/
