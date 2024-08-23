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
#include "lv_cache_entry.h"
#include "lv_cache_private.h"
#include "../lv_types.h"

#include "lv_cache_lru_rb.h"

#include "lv_image_cache.h"
#include "lv_image_header_cache.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a cache object with the given parameters.
 * @param cache_class   The class of the cache. Currently only support one two builtin classes:
 *                        - lv_cache_class_lru_rb_count for LRU-based cache with count-based eviction policy.
 *                        - lv_cache_class_lru_rb_size for LRU-based cache with size-based eviction policy.
 * @param node_size     The node size is the size of the data stored in the cache..
 * @param max_size      The max size is the maximum amount of memory or count that the cache can hold.
 *                        - lv_cache_class_lru_rb_count: max_size is the maximum count of nodes in the cache.
 *                        - lv_cache_class_lru_rb_size: max_size is the maximum size of the cache in bytes.
 * @param ops           A set of operations that can be performed on the cache. See lv_cache_ops_t for details.
 * @return              Returns a pointer to the created cache object on success, `NULL` on error.
 */
lv_cache_t * lv_cache_create(const lv_cache_class_t * cache_class,
                             size_t node_size, size_t max_size,
                             lv_cache_ops_t ops);

/**
 * Destroy a cache object.
 * @param cache         The cache object pointer to destroy.
 * @param user_data     A user data pointer that will be passed to the free callback.
 */
void lv_cache_destroy(lv_cache_t * cache, void * user_data);

/**
 * Acquire a cache entry with the given key. If entry not in cache, it will return `NULL` (not found).
 * If the entry is found, it's priority will be changed by the cache's policy. And the `lv_cache_entry_t::ref_cnt` will be incremented.
 * @param cache         The cache object pointer to acquire the entry.
 * @param key           The key of the entry to acquire.
 * @param user_data     A user data pointer that will be passed to the create callback.
 * @return              Returns a pointer to the acquired cache entry on success with `lv_cache_entry_t::ref_cnt` incremented, `NULL` on error.
 */
lv_cache_entry_t * lv_cache_acquire(lv_cache_t * cache, const void * key, void * user_data);

/**
 * Acquire a cache entry with the given key. If the entry is not in the cache, it will create a new entry with the given key.
 * If the entry is found, it's priority will be changed by the cache's policy. And the `lv_cache_entry_t::ref_cnt` will be incremented.
 * If you want to use this API to simplify the code, you should provide a `lv_cache_ops_t::create_cb` that creates a new entry with the given key.
 * This API is a combination of lv_cache_acquire() and lv_cache_add(). The effect is the same as calling lv_cache_acquire() and lv_cache_add() separately.
 * And the internal impact on cache is also consistent with these two APIs.
 * @param cache         The cache object pointer to acquire the entry.
 * @param key           The key of the entry to acquire or create.
 * @param user_data     A user data pointer that will be passed to the create callback.
 * @return              Returns a pointer to the acquired or created cache entry on success with `lv_cache_entry_t::ref_cnt` incremented, `NULL` on error.
 */
lv_cache_entry_t * lv_cache_acquire_or_create(lv_cache_t * cache, const void * key, void * user_data);

/**
 * Add a new cache entry with the given key and data. If the cache is full, the cache's policy will be used to evict an entry.
 * @param cache         The cache object pointer to add the entry.
 * @param key           The key of the entry to add.
 * @param user_data     A user data pointer that will be passed to the create callback.
 * @return              Returns a pointer to the added cache entry on success with `lv_cache_entry_t::ref_cnt` incremented, `NULL` on error.
 */
lv_cache_entry_t * lv_cache_add(lv_cache_t * cache, const void * key, void * user_data);

/**
 * Release a cache entry. The `lv_cache_entry_t::ref_cnt` will be decremented. If the `lv_cache_entry_t::ref_cnt` is zero, it will issue an error.
 * If the entry passed to this function is the last reference to the data and the entry is marked as invalid, the cache's policy will be used to evict the entry.
 * @param cache         The cache object pointer to release the entry.
 * @param entry         The cache entry pointer to release.
 * @param user_data     A user data pointer that will be passed to the free callback.
 */
void lv_cache_release(lv_cache_t * cache, lv_cache_entry_t * entry, void * user_data);

/**
 * Reserve a certain amount of memory/count in the cache. This function is useful when you want to reserve a certain amount of memory/count in advance,
 * for example, when you know that you will need it later.
 * When the current cache size is max than the reserved size, the function will evict entries until the reserved size is reached.
 * @param cache         The cache object pointer to reserve.
 * @param reserved_size The amount of memory/count to reserve.
 * @param user_data     A user data pointer that will be passed to the free callback.
 */
void lv_cache_reserve(lv_cache_t * cache, uint32_t reserved_size, void * user_data);

/**
 * Drop a cache entry with the given key. If the entry is not in the cache, nothing will happen to it.
 * If the entry is found, it will be removed from the cache and its data will be freed when the last reference to it is released.
 * @note The data will not be freed immediately but when the last reference to it is released. But this entry will not be found by lv_cache_acquire().
 *       If you want cache a same key again, you should use lv_cache_add() or lv_cache_acquire_or_create().
 * @param cache         The cache object pointer to drop the entry.
 * @param key           The key of the entry to drop.
 * @param user_data     A user data pointer that will be passed to the free callback.
 */
void lv_cache_drop(lv_cache_t * cache, const void * key, void * user_data);

/**
 * Drop all cache entries. All entries will be removed from the cache and their data will be freed when the last reference to them is released.
 * @note If some entries are still referenced by other objects, it will issue an error. And this case shouldn't happen in normal cases..
 * @param cache         The cache object pointer to drop all entries.
 * @param user_data     A user data pointer that will be passed to the free callback.
 */
void lv_cache_drop_all(lv_cache_t * cache, void * user_data);

/**
 * Evict one entry from the cache. The eviction policy will be used to select the entry to evict.
 * @param cache         The cache object pointer to evict an entry.
 * @param user_data     A user data pointer that will be passed to the free callback.
 * @return              Returns true if an entry is evicted, false if no entry is evicted.
 */
bool lv_cache_evict_one(lv_cache_t * cache, void * user_data);

/**
 * Set the maximum size of the cache.
 * If the current cache size is greater than the new maximum size, the cache's policy will be used to evict entries until the new maximum size is reached.
 * If set to 0, the cache will be disabled.
 * @note But this behavior will happen only new entries are added to the cache.
 * @param cache         The cache object pointer to set the maximum size.
 * @param max_size      The new maximum size of the cache.
 * @param user_data     A user data pointer that will be passed to the free callback.
 */
void   lv_cache_set_max_size(lv_cache_t * cache, size_t max_size, void * user_data);

/**
 * Get the maximum size of the cache.
 * @param cache         The cache object pointer to get the maximum size.
 * @param user_data     A user data pointer that will be passed to the free callback.
 * @return              Returns the maximum size of the cache.
 */
size_t lv_cache_get_max_size(lv_cache_t * cache, void * user_data);

/**
 * Get the current size of the cache.
 * @param cache         The cache object pointer to get the current size.
 * @param user_data     A user data pointer that will be passed to the free callback.
 * @return              Returns the current size of the cache.
 */
size_t lv_cache_get_size(lv_cache_t * cache, void * user_data);

/**
 * Get the free size of the cache.
 * @param cache         The cache object pointer to get the free size.
 * @param user_data     A user data pointer that will be passed to the free callback.
 * @return              Returns the free size of the cache.
 */
size_t lv_cache_get_free_size(lv_cache_t * cache, void * user_data);

/**
 * Return true if the cache is enabled.
 * Disabled cache means that when the max_size of the cache is 0. In this case, all cache operations will be no-op.
 * @param cache         The cache object pointer to check if it's disabled.
 * @return              Returns true if the cache is enabled, false otherwise.
 */
bool lv_cache_is_enabled(lv_cache_t * cache);

/**
 * Set the compare callback of the cache.
 * @param cache         The cache object pointer to set the compare callback.
 * @param compare_cb    The compare callback to set.
 * @param user_data     A user data pointer.
 */
void   lv_cache_set_compare_cb(lv_cache_t * cache, lv_cache_compare_cb_t compare_cb, void * user_data);

/**
 * Set the create callback of the cache.
 * @param cache         The cache object pointer to set the create callback.
 * @param alloc_cb      The create callback to set.
 * @param user_data     A user data pointer.
 */
void   lv_cache_set_create_cb(lv_cache_t * cache, lv_cache_create_cb_t alloc_cb, void * user_data);

/**
 * Set the free callback of the cache.
 * @param cache         The cache object pointer to set the free callback.
 * @param free_cb       The free callback to set.
 * @param user_data     A user data pointer.
 */
void   lv_cache_set_free_cb(lv_cache_t * cache, lv_cache_free_cb_t free_cb, void * user_data);

/**
 * Give a name for a cache object. Only the pointer of the string is saved.
 * @param cache         The cache object pointer to set the name.
 * @param name          The name of the cache.
 */
void lv_cache_set_name(lv_cache_t * cache, const char * name);

/**
 * Get the name of a cache object.
 * @param cache         The cache object pointer to get the name.
 * @return              Returns the name of the cache.
 */
const char * lv_cache_get_name(lv_cache_t * cache);

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_CACHE_H */
