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
#include "_lv_cache_internal.h"
#include <stdbool.h>
#include <stdlib.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_cache_t_ * lv_cache_create_(const lv_cache_class_t * cache_class,
                               size_t node_size, size_t max_size,
                               lv_cache_compare_cb_t compare_cb,
                               lv_cache_create_cb_t create_cb,
                               lv_cache_free_cb_t free_cb);
void   lv_cache_destroy_(lv_cache_t_ * cache, void * user_data);

lv_cache_entry_t_ * lv_cache_get_(lv_cache_t_ * cache, const void * key, void * user_data);
lv_cache_entry_t_ * lv_cache_get_or_create_(lv_cache_t_ * cache, const void * key, void * user_data);
void   lv_cache_remove_(lv_cache_t_* cache, lv_cache_entry_t_* entry, void * user_data);
void   lv_cache_drop_(lv_cache_t_ * cache, const void * key, void * user_data);
void   lv_cache_drop_all_(lv_cache_t_ * cache, void * user_data);
void   lv_cache_set_max_size_(lv_cache_t_ * cache, size_t max_size, void * user_data);
size_t lv_cache_get_max_size_(lv_cache_t_ * cache, void * user_data);
size_t lv_cache_get_size_(lv_cache_t_ * cache, void * user_data);
size_t lv_cache_get_free_size_(lv_cache_t_ * cache, void * user_data);

void   lv_cache_set_compare_cb(lv_cache_t_ * cache, lv_cache_compare_cb_t compare_cb, void * user_data);
void   lv_cache_set_create_cb(lv_cache_t_ * cache, lv_cache_create_cb_t alloc_cb, void * user_data);
void   lv_cache_set_free_cb(lv_cache_t_ * cache, lv_cache_free_cb_t free_cb, void * user_data);
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
