/**
* @file lv_image_header_cache.h
*
 */

#ifndef LV_IMAGE_HEADER_CACHE_H
#define LV_IMAGE_HEADER_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
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
 * Initialize image header cache.
 * @param  count initial size of the cache in count of image headers.
 * @return LV_RESULT_OK: initialization succeeded, LV_RESULT_INVALID: failed.
 */
lv_result_t lv_image_header_cache_init(uint32_t count);

/**
 * Resize image header cache.
 * If set to 0, the cache is disabled.
 * @param count  new max count of cached image headers.
 * @param evict_now true: evict the image headers should be removed by the eviction policy, false: wait for the next cache cleanup.
 */
void lv_image_header_cache_resize(uint32_t count, bool evict_now);

/**
 * Invalidate image header cache. Use NULL to invalidate all image headers.
 * It's also automatically called when an image is invalidated.
 * @param src pointer to an image source.
 */
void lv_image_header_cache_drop(const void * src);

/**
 * Return true if the image header cache is enabled.
 * @return true: enabled, false: disabled.
 */
bool lv_image_header_cache_is_enabled(void);

/**
 * Create an iterator to iterate over the image header cache.
 * @return an iterator to iterate over the image header cache.
 */
lv_iter_t * lv_image_header_cache_iter_create(void);

/**
 * Dump the content of the image header cache in a human-readable format with cache order.
 */
void lv_image_header_cache_dump(void);

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMAGE_HEADER_CACHE_H*/
