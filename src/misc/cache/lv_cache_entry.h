/**
* @file lv_cache_entry.h
*
 */

#ifndef LV_CACHE_ENTRY_H
#define LV_CACHE_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../osal/lv_os.h"
#include "../lv_types.h"
#include "lv_cache_private.h"
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

/**
 * Get the size of a cache entry.
 * @param node_size     The size of the node in the cache.
 * @return              The size of the cache entry.
 */
uint32_t lv_cache_entry_get_size(const uint32_t node_size);

/**
 * Get the reference count of a cache entry.
 * @param entry        The cache entry to get the reference count of.
 * @return             The reference count of the cache entry.
 */
int32_t  lv_cache_entry_get_ref(lv_cache_entry_t * entry);

/**
 * Get the node size of a cache entry. Which is the same size with @lv_cache_entry_get_size's node_size parameter.
 * @param entry        The cache entry to get the node size of.
 * @return             The node size of the cache entry.
 */
uint32_t lv_cache_entry_get_node_size(lv_cache_entry_t * entry);

/**
 * Check if a cache entry is invalid.
 * @param entry        The cache entry to check.
 * @return             True: the cache entry is invalid. False: the cache entry is valid.
 */
bool     lv_cache_entry_is_invalid(lv_cache_entry_t * entry);

/**
 * Get the data of a cache entry.
 * @param entry        The cache entry to get the data of.
 * @return             The pointer to the data of the cache entry.
 */
void  *  lv_cache_entry_get_data(lv_cache_entry_t * entry);

/**
 * Get the cache instance of a cache entry.
 * @param entry        The cache entry to get the cache instance of.
 * @return             The pointer to the cache instance of the cache entry.
 */
const lv_cache_t * lv_cache_entry_get_cache(const lv_cache_entry_t * entry);

/**
 * Get the cache entry of a data. The data should be allocated by the cache instance.
 * @param data         The data to get the cache entry of.
 * @param node_size    The size of the node in the cache.
 * @return             The pointer to the cache entry of the data.
 */
lv_cache_entry_t * lv_cache_entry_get_entry(void * data, const uint32_t node_size);

/**
 * Allocate a cache entry.
 * @param node_size    The size of the node in the cache.
 * @param cache        The cache instance to allocate the cache entry from.
 * @return             The pointer to the allocated cache entry.
 */
lv_cache_entry_t * lv_cache_entry_alloc(const uint32_t node_size, const lv_cache_t * cache);

/**
 * Initialize a cache entry.
 * @param entry        The cache entry to initialize.
 * @param cache        The cache instance to allocate the cache entry from.
 * @param node_size    The size of the node in the cache.
 */
void lv_cache_entry_init(lv_cache_entry_t * entry, const lv_cache_t * cache, const uint32_t node_size);

/**
 * Deallocate a cache entry. And the data of the cache entry will be freed.
 * @param entry        The cache entry to deallocate.
 */
void lv_cache_entry_delete(lv_cache_entry_t * entry);
/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CACHE_ENTRY_H*/
