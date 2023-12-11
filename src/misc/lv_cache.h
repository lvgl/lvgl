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
#include <stdbool.h>
#include "../osal/lv_os.h"

/*********************
 *      DEFINES
 *********************/
#define LV_CACHE_DATA_TYPE_NOT_SET  0xFFFFFFFF

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_CACHE_SRC_TYPE_PATH,
    LV_CACHE_SRC_TYPE_POINTER,
} lv_cache_src_type_t;

typedef struct _lv_cache_entry_t {

    /**
     * The data to cache.
     * Can be just a pointer to a byte array, or pointer to a complex structure, or anything else*/
    const void * data;

    /**
     * Size of data in bytes.
     * It's not the size of the cached data, just the size of the structure pointed by `data`
     * E.g. `data` can point to descriptor struct and the size of that struct needs to be stored here.
     * It can be used in the `compary_cb` to compare `data` fields of the entries with a requested one*/
    uint32_t data_size;

    /**An integer ID to tell the type of the cached data.
     * If set to `LV_CACHE_DATA_TYPE_NOT_SET` `lv_cache_find_by_data` cannot be used*/
    uint32_t data_type;

    /**
     * The source from which the cache is created.
     * It's can or cannot be the same as data, or different, or NULL.
     * It's used to find the cache entries which are relted to same source.
     * E.g. the same image, font, etc.  */
    const void * src;

    lv_cache_src_type_t src_type;

    /**Arbitrary parameters to better identify the source*/
    int32_t param1;
    int32_t param2;

    /** Memory in bytes used by data. */
    uint32_t memory_usage;

    /**
     * Called to compare the data of cache entries.
     * Before calling this function LVGL checks that `data_size` of both entries are the same.
     * This callback look into `data` and check all the pointers and their content on any level.
     * @param data1      first data to compare
     * @param data2      second data to compare
     * @param data_size  size of data
     * @return           true: `data1` and `data2` are the same
     */
    bool (*compare_cb)(const void * data1, const void * data2, size_t data_size);

    /**
     * Called when the entry is invalidated to free its data
     * @param e     the cache entry to free
     */
    void (*invalidate_cb)(struct _lv_cache_entry_t * e);

    /** User processing tag*/
    uint32_t process_state;

    /** On access to any cache entry, `life` of each cache entry will be incremented by their own `weight` to keep the entry alive longer*/
    uint32_t weight;

    /** The current `life`. Entries with the smallest life will be purged from the cache if a new entry needs to be cached*/
    int32_t life;

    /** Count how many times the cached data is being used.
     * It will be incremented in lv_cache_get_data and decremented in lv_cache_release.
     * A data will dropped from the cache only if its usage_count is zero */
    uint32_t usage_count;

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
 * @param data          data the cache, can be a complex structure too
 * @param data_size     the size of data (if it's a struct then the size of the struct)
 * @param data_type     type of data to identify the kind of this cache entry
 * @param memory_usage  the size of memory used by this entry (`data` might contain pointers so it's size of all buffers related to entry)
 * @return              a handler for the new cache entry
 */
typedef lv_cache_entry_t * (*lv_cache_add_cb)(const void * data, size_t data_size, uint32_t data_type,
                                              size_t memory_usage);

/**
 * Find a cache entry based on its data
 * @param data      the data to find
 * @param data_size size of data
 * @param data_type ID for the data type
 * @return          the cache entry with given `data` or NULL if not found
 */
typedef lv_cache_entry_t * (*lv_cache_find_by_data_cb)(const void * data, size_t data_size, uint32_t data_type);

/**
 * Get the next entry which has the given source and parameters
 * @param prev_entry    pointer to the previous entry from which the nest should be found. NULL means to start from the beginning.
 * @param src           a pointer or a string
 * @param src_type      element of lv_cache_src_type_t
 * @return              the cache entry with given source or NULL if not found
 */
typedef lv_cache_entry_t * (*lv_cache_find_by_src_cb)(lv_cache_entry_t * entry, const void * src,
                                                      lv_cache_src_type_t src_type);

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

/**
 * Empty the cache.
 */
typedef void (*lv_cache_empty_cb)(void);

typedef struct {
    lv_cache_add_cb add_cb;
    lv_cache_find_by_data_cb find_by_data_cb;
    lv_cache_find_by_src_cb find_by_src_cb;
    lv_cache_invalidate_cb invalidate_cb;
    lv_cache_get_data_cb get_data_cb;
    lv_cache_release_cb release_cb;
    lv_cache_set_max_size_cb set_max_size_cb;
    lv_cache_empty_cb empty_cb;

    lv_mutex_t mutex;
    size_t max_size;
    uint32_t locked     : 1;    /**< Show the mutex state, used to log unlocked cache access*/
    uint32_t last_data_type;
} lv_cache_manager_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the cache module
 */
void _lv_cache_init(void);

/**
 * Deinitialize the cache module
 */
void _lv_cache_deinit(void);

/**
 * Set new cache manager
 * @param manager   the new cache manager with callback functions set
 */
void lv_cache_set_manager(lv_cache_manager_t * manager);

/**
 * Add a new entry to the cache with the given size.
 * It won't allocate any buffers just free enough space to be a new entry
 * with `size` bytes fits.
 * @param data          data the cache, can be a complex structure too
 * @param data_size     the size of data (if it's a struct then the size of the struct)
 * @param data_type     type of data to identify the kind of this cache entry
 * @param memory_usage  the size of memory used by this entry (`data` might contain pointers so it's size of all buffers related to entry)
 * @return              a handler for the new cache entry
 */
lv_cache_entry_t * lv_cache_add(const void * data, size_t data_size, uint32_t data_type, size_t memory_usage);

/**
 * Find a cache entry based on its data
 * @param data      the data to find
 * @param data_size size of data
 * @param data_type ID of data type
 * @return          the cache entry with given `data` or NULL if not found
 */
lv_cache_entry_t * lv_cache_find_by_data(const void * data, size_t data_size, uint32_t data_type);

/**
 * Get the next entry which has the given source and parameters
 * @param prev_entry    pointer to the previous entry from which the nest should be found. NULL means to start from the beginning.
 * @param src           a pointer or a string
 * @param src_type      element of lv_cache_src_type_t
 * @return              the cache entry with given source or NULL if not found
 */
lv_cache_entry_t * lv_cache_find_by_src(lv_cache_entry_t * entry, const void * src, lv_cache_src_type_t src_type);

/**
 * Invalidate (drop) a cache entry. It will call the entry's `invalidate_cb` to free the resources
 * @param entry    the entry to invalidate. (can be retrieved by `lv_cache_find()`)
 */
void lv_cache_invalidate(lv_cache_entry_t * entry);

/**
 * Invalidate all cache entries with a given source
 * @param src           a pointer or a string
 * @param src_type      element of lv_cache_src_type_t
 */
void lv_cache_invalidate_by_src(const void * src, lv_cache_src_type_t src_type);

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

/**
 * Register a data type which can be used as `entry->data_type`.
 * @return      the registered unique data type ID.
 */
uint32_t lv_cache_register_data_type(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CACHE_H*/
