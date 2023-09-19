/**
 * @file lv_cache.h
 *
 */

#ifndef LV_CACHE_H
#define LV_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stddef.h>
#include "../osal/lv_os.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_CACHE_SRC_TYPE_PTR,
    LV_CACHE_SRC_TYPE_STR,
    _LV_CACHE_SRC_TYPE_LAST,
} lv_cache_src_type_t;

typedef struct {
    /**The image source or other source related to the cache content.*/
    const void * src;

    lv_cache_src_type_t src_type;

    /** Some extra parameters to describe the source. E.g. the current frame of an animation*/
    uint32_t param1;
    uint32_t param2;

    /** The data to cache*/
    const void * data;

    /** Size of data in bytes*/
    uint32_t data_size;

    /** On access to any cache entry, `life` of each cache entry will be incremented by their own `weight` to keep the entry alive longer*/
    uint32_t weight;

    /** The current `life`. Entries with the smallest life will be purged from the cache if a new entry needs to be cached*/
    int32_t life;

    /** Count how many times the cached data is being used.
     * It will be incremented in lv_cache_get_data and decremented in lv_cache_release.
     * A data will dropped from the cache only if its usage_count is zero */
    uint32_t usage_count;

    /** Call `lv_free` on `src` when the entry is removed from the cache */
    uint32_t free_src   : 1;

    /** Call `lv_draw_buf_free` on `data` when the entry is removed from the cache */
    uint32_t free_data   : 1;

    /** The cache entry was larger then the max cache size so only a temporary entry was allocated
     * The entry will be closed and freed in `lv_cache_release` automatically*/
    uint32_t temporary  : 1;

    /**Any user data if needed*/
    void * user_data;
} lv_cache_entry_t;


/**
 * Add a new entry to the cache with the given size.
 * It won't allocate any buffers just free enough space to be a new entry
 * with `size` bytes fits.
 * @param size      the size of the new entry in bytes
 * @return          a handler for the new cache entry
 */
typedef lv_cache_entry_t * (*lv_cache_add_cb)(size_t size);

/**
 * Find a cache entry
 * @param src_ptr   pointer to the source data
 * @param src_type  source type (`LV_CACHE_SRC_TYPE_PTR` or `LV_CACHE_SRC_TYPE_STR`)
 * @param param1    param1, which was set when the cache was added
 * @param param2    param2, which was set when the cache was added
 * @return          the cache entry with given source and parameters or NULL if not found
 */
typedef lv_cache_entry_t * (*lv_cache_find_cb)(const void * src_ptr, lv_cache_src_type_t src_type, uint32_t param1,
                                               uint32_t param2);

/**
 * Invalidate (drop) a cache entry
 * @param entry    the entry to invalidate. (can be retrieved by `lv_cache_find()`)
 */
typedef void (*lv_cache_invalidate_cb)(lv_cache_entry_t * entry);

/**
 * Get the data of a cache entry.
 * It is considered a cached data access so the cache manager can count that
 * this entry was used on more times, and therefore it's more relevant.
 * It also increments entry->usage_count to indicate that the data is being used
 * and cannot be dropped.
 * @param entry     the cache entry whose data should be retrieved
 */
typedef const void * (*lv_cache_get_data_cb)(lv_cache_entry_t * entry);

/**
 * Mark the cache entry as unused. It decrements entry->usage_count.
 * @param entry     the cache entry to invalidate
 */
typedef void (*lv_cache_release_cb)(lv_cache_entry_t * entry);

/**
 * Set maximum cache size in bytes.
 * @param size      the max size in byes
 */
typedef void (*lv_cache_set_max_size_cb)(size_t size);

typedef struct {
    lv_cache_add_cb add_cb;
    lv_cache_find_cb find_cb;
    lv_cache_invalidate_cb invalidate_cb;
    lv_cache_get_data_cb get_data_cb;
    lv_cache_release_cb release_cb;
    lv_cache_set_max_size_cb set_max_size_cb;

    lv_mutex_t mutex;
    size_t max_size;
    uint32_t locked     : 1;    /**< Show the mutex state, used to log unlocked cache access*/
} lv_cache_manager_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the cache module
 */
void _lv_cache_init(void);

/**
 * Add a new entry to the cache with the given size.
 * It won't allocate any buffers just free enough space to be a new entry
 * with `size` bytes fits.
 * @param size      the size of the new entry in bytes
 * @return          a handler for the new cache entry
 */
lv_cache_entry_t * lv_cache_add(size_t size);

/**
 * Find a cache entry with pointer source type
 * @param src_ptr   pointer to the source data
 * @param src_type  source type (`LV_CACHE_SRC_TYPE_PTR` or `LV_CACHE_SRC_TYPE_STR`)
 * @param param1    param1, which was set when the cache was added
 * @param param2    param2, which was set when the cache was added
 * @return          the cache entry with given source and parameters or NULL if not found
 */
lv_cache_entry_t * lv_cache_find(const void * src, lv_cache_src_type_t src_type, uint32_t param1, uint32_t param2);

/**
 * Invalidate (drop) a cache entry
 * @param entry    the entry to invalidate. (can be retrieved by `lv_cache_find()`)
 */
void lv_cache_invalidate(lv_cache_entry_t * entry);

/**
 * Get the data of a cache entry.
 * It is considered a cached data access so the cache manager can count that
 * this entry was used on more times, and therefore it's more relevant.
 * It also increments entry->usage_count to indicate that the data is being used
 * and cannot be dropped.
 * @param entry     the cache entry whose data should be retrieved
 */
const void * lv_cache_get_data(lv_cache_entry_t * entry);

/**
 * Mark the cache entry as unused. It decrements entry->usage_count.
 * @param entry
 */
void lv_cache_release(lv_cache_entry_t * entry);

/**
 * Set maximum cache size in bytes.
 * @param size      the max size in byes
 */
void lv_cache_set_max_size(size_t size);

/**
 * Get the max size of the cache
 * @return      the max size in bytes
 */
size_t lv_cache_get_max_size(void);

/**
 * Lock the mutex of the cache.
 * Needs to be called manually before any cache operation,
 */
void lv_cache_lock(void);

/**
 * Unlock the mutex of the cache.
 * Needs to be called manually after any cache operation,
 */
void lv_cache_unlock(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CACHE_H*/
