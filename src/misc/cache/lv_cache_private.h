/**
* @file lv_cache_private.h
*
*/

#ifndef LV_CACHE_PRIVATE_H
#define LV_CACHE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_types.h"
#include "../../osal/lv_os.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * The result of the cache reserve condition callback
 */
typedef enum {
    LV_CACHE_RESERVE_COND_OK,          /**< The condition is met and no entries need to be evicted */
    LV_CACHE_RESERVE_COND_TOO_LARGE,   /**< The condition is not met and the reserve size is too large */
    LV_CACHE_RESERVE_COND_NEED_VICTIM, /**< The condition is not met and a victim is needed to be evicted */
    LV_CACHE_RESERVE_COND_ERROR        /**< An error occurred while checking the condition */
} lv_cache_reserve_cond_res_t;

struct _lv_cache_ops_t;
struct _lv_cache_t;
struct _lv_cache_class_t;
struct _lv_cache_entry_t;

typedef struct _lv_cache_ops_t lv_cache_ops_t;
typedef struct _lv_cache_class_t lv_cache_class_t;

typedef int8_t lv_cache_compare_res_t;
typedef bool (*lv_cache_create_cb_t)(void * node, void * user_data);
typedef void (*lv_cache_free_cb_t)(void * node, void * user_data);
typedef lv_cache_compare_res_t (*lv_cache_compare_cb_t)(const void * a, const void * b);

/**
 * The cache instance allocation function, used by the cache class to allocate memory for cache instances.
 * @return It should return a pointer to the allocated instance.
 */
typedef void * (*lv_cache_alloc_cb_t)(void);

/**
 * The cache instance initialization function, used by the cache class to initialize the cache instance.
 * @return It should return true if the initialization is successful, false otherwise.
 */
typedef bool (*lv_cache_init_cb_t)(lv_cache_t * cache);

/**
 * The cache instance destruction function, used by the cache class to destroy the cache instance.
 */
typedef void (*lv_cache_destroy_cb_t)(lv_cache_t * cache, void * user_data);

/**
 * The cache get function, used by the cache class to get a cache entry by its key.
 * @return `NULL` if the key is not found.
 */
typedef lv_cache_entry_t * (*lv_cache_get_cb_t)(lv_cache_t * cache, const void * key, void * user_data);

/**
 * The cache add function, used by the cache class to add a cache entry with a given key.
 * This function only cares about how to add the entry, it doesn't check if the entry already exists and doesn't care about is it a victim or not.
 * @return the added cache entry, or NULL if the entry is not added.
 */
typedef lv_cache_entry_t * (*lv_cache_add_cb_t)(lv_cache_t * cache, const void * key, void * user_data);

/**
 * The cache remove function, used by the cache class to remove a cache entry from the cache but doesn't free the memory..
 * This function only cares about how to remove the entry, it doesn't care about is it a victim or not.
 */
typedef void (*lv_cache_remove_cb_t)(lv_cache_t * cache, lv_cache_entry_t * entry, void * user_data);

/**
 * The cache drop function, used by the cache class to remove a cache entry from the cache and free the memory.
 */
typedef void (*lv_cache_drop_cb_t)(lv_cache_t * cache, const void * key, void * user_data);

/**
 * The cache drop all function, used by the cache class to remove all cache entries from the cache and free the memory.
 */
typedef void (*lv_cache_drop_all_cb_t)(lv_cache_t * cache, void * user_data);

/**
 * The cache get victim function, used by the cache class to get a victim entry to be evicted.
 */
typedef lv_cache_entry_t * (*lv_cache_get_victim_cb)(lv_cache_t * cache, void * user_data);

/**
 * The cache reserve condition function, used by the cache class to check if a new entry can be added to the cache without exceeding its maximum size.
 * See lv_cache_reserve_cond_res_t for the possible results.
 */
typedef lv_cache_reserve_cond_res_t (*lv_cache_reserve_cond_cb)(lv_cache_t * cache, const void * key, size_t size,
                                                                void * user_data);

/**
 * The cache iterator creation function, used by the cache class to create an iterator for the cache.
 * @return A pointer to the created iterator, or NULL if the iterator cannot be created.
 */
typedef lv_iter_t * (*lv_cache_iter_create_cb)(lv_cache_t * cache);

/**
 * The cache operations struct
 */
struct _lv_cache_ops_t {
    lv_cache_compare_cb_t compare_cb;    /**< Compare function for keys */
    lv_cache_create_cb_t create_cb;      /**< Create function for nodes */
    lv_cache_free_cb_t free_cb;          /**< Free function for nodes */
};

/**
 * The cache entry struct
 */
struct _lv_cache_t {
    const lv_cache_class_t * clz;     /**< Cache class. There are two built-in classes:
                                       * - lv_cache_class_lru_rb_count for LRU-based cache with count-based eviction policy.
                                       * - lv_cache_class_lru_rb_size for LRU-based cache with size-based eviction policy. */

    uint32_t node_size;               /**< Size of a node */

    uint32_t max_size;                /**< Maximum size of the cache */
    uint32_t size;                    /**< Current size of the cache */

    lv_cache_ops_t ops;               /**< Cache operations struct _lv_cache_ops_t */

    lv_mutex_t lock;                  /**< Cache lock used to protect the cache in multithreading environments */

    const char * name;                /**< Name of the cache */
};

/**
 * Cache class struct for building custom cache classes
 *
 * Examples:
 * - lv_cache_class_lru_rb_count for LRU-based cache with count-based eviction policy.
 * - lv_cache_class_lru_rb_size for LRU-based cache with size-based eviction policy.
 */
struct _lv_cache_class_t {
    lv_cache_alloc_cb_t alloc_cb;                 /**< The allocation function for cache entries */
    lv_cache_init_cb_t init_cb;                   /**< The initialization function for cache entries */
    lv_cache_destroy_cb_t destroy_cb;             /**< The destruction function for cache entries */

    lv_cache_get_cb_t get_cb;                     /**< The get function for cache entries */
    lv_cache_add_cb_t add_cb;                     /**< The add function for cache entries */
    lv_cache_remove_cb_t remove_cb;               /**< The remove function for cache entries */
    lv_cache_drop_cb_t drop_cb;                   /**< The drop function for cache entries */
    lv_cache_drop_all_cb_t drop_all_cb;           /**< The drop all function for cache entries */
    lv_cache_get_victim_cb get_victim_cb;         /**< The get victim function for cache entries */
    lv_cache_reserve_cond_cb reserve_cond_cb;     /**< The reserve condition function for cache entries */

    lv_cache_iter_create_cb iter_create_cb;       /**< The iterator creation function for cache entries */
};

/*-----------------
 * Cache entry slot
 *----------------*/

struct _lv_cache_slot_size_t;

typedef struct _lv_cache_slot_size_t lv_cache_slot_size_t;

/**
 * Cache entry slot struct
 *
 * To add new fields to the cache entry, add them to a new struct and add it to the first
 * field of the cache data struct.  And this one is a size slot for the cache entry.
 */
struct _lv_cache_slot_size_t {
    size_t size;
};
/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CACHE_PRIVATE_H*/
