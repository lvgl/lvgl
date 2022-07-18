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
void lv_obj_draw_cache_set_enable(lv_obj_t * obj, bool en);

bool lv_obj_draw_cache_get_enable(lv_obj_t * obj);

void lv_obj_draw_cache_invalidate(lv_obj_t * obj);

lv_res_t _lv_obj_draw_cache(lv_event_t * e);

void _lv_obj_draw_cache_free(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_OBJ_DRAW_CACHE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_DRAW_CACHE_H*/
