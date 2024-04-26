/**
 * @file lv_lru.h
 *
 */

#ifndef LV_LRU_H
#define LV_LRU_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lv_conf_internal.h"

#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_LRU_OK = 0,
    LV_LRU_MISSING_CACHE,
    LV_LRU_MISSING_KEY,
    LV_LRU_MISSING_VALUE,
    LV_LRU_LOCK_ERROR,
    LV_LRU_VALUE_TOO_LARGE
} lv_lru_res_t;

typedef void (*lv_lru_free_cb_t)(void * v);

typedef struct lv_lru_item_t lv_lru_item_t;

typedef struct lv_lru_t {
    lv_lru_item_t ** items;
    uint64_t access_count;
    size_t free_memory;
    size_t total_memory;
    size_t average_item_length;
    size_t hash_table_size;
    uint32_t seed;
    lv_lru_free_cb_t value_free;
    lv_lru_free_cb_t key_free;
    lv_lru_item_t * free_items;
} lv_lru_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_lru_t * lv_lru_create(size_t cache_size, size_t average_length, lv_lru_free_cb_t value_free,
                         lv_lru_free_cb_t key_free);

void lv_lru_delete(lv_lru_t * cache);

lv_lru_res_t lv_lru_set(lv_lru_t * cache, const void * key, size_t key_length, void * value, size_t value_length);

lv_lru_res_t lv_lru_get(lv_lru_t * cache, const void * key, size_t key_size, void ** value);

lv_lru_res_t lv_lru_remove(lv_lru_t * cache, const void * key, size_t key_size);

/**
 * remove the least recently used item
 *
 * @todo we can optimise this by finding the n lru items, where n = required_space / average_length
 */
void lv_lru_remove_lru_item(lv_lru_t * cache);
/**********************
 *      MACROS
 **********************/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LRU_H*/
