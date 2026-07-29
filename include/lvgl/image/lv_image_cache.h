/**
 * @file lv_image_cache.h
 *
 */

#ifndef LV_IMAGE_CACHE_H
#define LV_IMAGE_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lv_types.h"

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
 * @note This is the configured quota for the current LVGL global context.
 * @note Valid only after lv_init() and before lv_deinit().
 * @note This getter is not an atomic snapshot. Follow LVGL's thread rules and use
 *       lv_lock() and lv_unlock() when calling it across threads.
 * @return the configured maximum image cache size.
 */
size_t lv_image_cache_get_max_size(void);

/**
 * Get the cache-accounted decoded payload size for currently indexed entries.
 * This is not the resident memory usage of the image cache.
 * @note This getter reads the current LVGL global context.
 * @note Valid only after lv_init() and before lv_deinit().
 * @note This getter is not an atomic snapshot. Follow LVGL's thread rules and use
 *       lv_lock() and lv_unlock() when calling it across threads.
 * @return the cache-accounted decoded payload size in bytes.
 */
size_t lv_image_cache_get_size(void);

/**
 * Get the remaining decoded image cache capacity in bytes.
 * The result is `max(max_size - size, 0)` and is saturated at 0 when a deferred
 * resize temporarily leaves the cache overcommitted.
 * @note This getter reads the current LVGL global context.
 * @note Valid only after lv_init() and before lv_deinit().
 * @note This getter is not an atomic snapshot. Follow LVGL's thread rules and use
 *       lv_lock() and lv_unlock() when calling it across threads.
 * @return the remaining image cache capacity in bytes.
 */
size_t lv_image_cache_get_free_size(void);

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMAGE_CACHE_H*/
