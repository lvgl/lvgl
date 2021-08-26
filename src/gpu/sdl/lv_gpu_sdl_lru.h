#include <SDL2/SDL_mutex.h>
#include <stdint.h>
#include <time.h>

#ifndef LV_LRU_H_
#define LV_LRU_H_

// ------------------------------------------
// errors
// ------------------------------------------
typedef enum {
    LV_LRU_NO_ERROR = 0,
    LV_LRU_MISSING_CACHE,
    LV_LRU_MISSING_KEY,
    LV_LRU_MISSING_VALUE,
    LV_LRU_LOCK_ERROR,
    LV_LRU_VALUE_TOO_LARGE
} lruc_error;


// ------------------------------------------
// types
// ------------------------------------------
typedef void (lv_lru_free_t)(void *v);

typedef struct lruc_item {
    void *value;
    void *key;
    size_t value_length;
    size_t key_length;
    uint64_t access_count;
    struct lruc_item *next;
} lruc_item;

typedef struct {
    lruc_item **items;
    uint64_t access_count;
    uint64_t free_memory;
    uint64_t total_memory;
    uint64_t average_item_length;
    uint32_t hash_table_size;
    time_t seed;
    lv_lru_free_t *value_free;
    lv_lru_free_t *key_free;
    lruc_item *free_items;
    SDL_mutex *mutex;
} lv_lru_t;


// ------------------------------------------
// api
// ------------------------------------------
lv_lru_t *lv_lru_new(uint64_t cache_size, uint32_t average_length, lv_lru_free_t *value_free, lv_lru_free_t *key_free);

lruc_error lv_lru_free(lv_lru_t *cache);

lruc_error lv_lru_set(lv_lru_t *cache, const void *key, size_t key_length, void *value, size_t value_length);

lruc_error lv_lru_get(lv_lru_t *cache, const void *key, size_t key_size, void **value);

lruc_error lv_lru_delete(lv_lru_t *cache, const void *key, size_t key_size);

#endif
