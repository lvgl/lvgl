/**
 * @file lv_obj_draw_cache.h
 *
 */

#ifndef LV_OBJ_DRAW_CACHE_H
#define LV_OBJ_DRAW_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"

/*********************
 *      DEFINES
 *********************/
#if LV_USE_OBJ_DRAW_CACHE

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Enable object draw cache function
 * @param obj   pointer to an object
 * @param en    true to enable, false to disable
 */
void lv_obj_draw_cache_set_enable(lv_obj_t * obj, bool en);

void lv_obj_draw_cache_set_img_cf(lv_obj_t * obj, lv_img_cf_t cf);

/**
 * Get object draw cache enable
 * @param obj   pointer to an object
 * @return      return true if enabled
 */
bool lv_obj_draw_cache_get_enable(lv_obj_t * obj);

lv_img_cf_t lv_obj_draw_cache_get_img_cf(lv_obj_t * obj);

const lv_img_dsc_t * lv_obj_draw_cache_get_img_dsc(lv_obj_t * obj);

static inline bool lv_obj_has_draw_cache(lv_obj_t * obj)
{
    return (obj->spec_attr && obj->spec_attr->draw_cache);
}

/**
 * Mark the object as invalid to update cache
 * @param obj    pointer to an object
 */
void lv_obj_draw_cache_invalidate(lv_obj_t * obj);

/**
 * Cache mode drawing processing
 * @param obj      pointer to an object
 * @param draw_ctx pointer to an initialized draw context
 * @return         whether the buffer drawing is successful
 */
lv_res_t _lv_obj_draw_cache(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx);

/**
 * Free the image from the draw cache
 * @param obj     pointer to an object
 */
void _lv_obj_draw_cache_free(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_OBJ_DRAW_CACHE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_DRAW_CACHE_H*/
