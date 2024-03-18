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
#include "lv_cache_private.h"

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
 * Invalidate image cache. Use NULL to invalidate all images.
 * @param src pointer to an image source.
 */
void lv_image_cache_drop(const void * src);

/**
 * Resize image cache.
 * @param new_size  new size of the cache in bytes.
 * @param evict_now true: evict the images should be removed by the eviction policy, false: wait for the next cache cleanup.
 */
void lv_image_cache_resize(uint32_t new_size, bool evict_now);

/**
 * Invalidate image header cache. Use NULL to invalidate all image headers.
 * It's also automatically called when an image is invalidated.
 * @param src pointer to an image source.
 */
void lv_image_header_cache_drop(const void * src);

/**
 * Resize image header cache.
 * @param new_size  new size of the cache in count of image headers.
 * @param evict_now true: evict the image headers should be removed by the eviction policy, false: wait for the next cache cleanup.
 */
void lv_image_header_cache_resize(uint32_t new_size, bool evict_now);

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
