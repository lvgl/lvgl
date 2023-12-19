/**
* @file lv_cache.h
*
*/

#ifndef LV_CACHE1_H
#define LV_CACHE1_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_cache_entry.h"
#include "lv_cache_private.h"
#include <stdbool.h>
#include <stdlib.h>

#include "_lv_cache_lru_rb.h"

#include "lv_image_cache.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_cache_t * lv_cache_create(const lv_cache_class_t * cache_class,
                             size_t node_size, size_t max_size,
                             lv_cache_ops_t ops);
void lv_cache_destroy(lv_cache_t * cache, void * user_data);

lv_cache_entry_t * lv_cache_acquire(lv_cache_t * cache, const void * key, void * user_data);
lv_cache_entry_t * lv_cache_acquire_or_create(lv_cache_t * cache, const void * key, void * user_data);
lv_cache_entry_t * lv_cache_add(lv_cache_t * cache, const void * key, void * user_data);
void lv_cache_release(lv_cache_t * cache, lv_cache_entry_t * entry, void * user_data);
void lv_cache_drop(lv_cache_t * cache, const void * key, void * user_data);
void lv_cache_drop_all(lv_cache_t * cache, void * user_data);

void lv_cache_set_max_size(lv_cache_t * cache, size_t max_size, void * user_data);
size_t lv_cache_get_max_size(lv_cache_t * cache, void * user_data);
size_t lv_cache_get_size(lv_cache_t * cache, void * user_data);
size_t lv_cache_get_free_size(lv_cache_t * cache, void * user_data);

void   lv_cache_set_compare_cb(lv_cache_t * cache, lv_cache_compare_cb_t compare_cb, void * user_data);
void   lv_cache_set_create_cb(lv_cache_t * cache, lv_cache_create_cb_t alloc_cb, void * user_data);
void   lv_cache_set_free_cb(lv_cache_t * cache, lv_cache_free_cb_t free_cb, void * user_data);
/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CACHE_H*/
