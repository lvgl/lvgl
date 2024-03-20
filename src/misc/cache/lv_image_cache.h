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
 * Initialize image cache.
 * @return LV_RESULT_OK: initialization succeeded, LV_RESULT_INVALID: failed.
 */
lv_result_t lv_image_cache_init(uint32_t size);

/**
 * Resize image cache.
 * If set to 0, the cache will be disabled.
 * @param new_size  new size of the cache in bytes.
 * @param evict_now true: evict the images should be removed by the eviction policy, false: wait for the next cache cleanup.
 */
void lv_image_cache_resize(uint32_t new_size, bool evict_now);

/**
 * Invalidate image cache. Use NULL to invalidate all images.
 * @param src pointer to an image source.
 */
void lv_image_cache_drop(const void * src);

/**
 * Return true if the image cache is disabled.
 * @return true: disabled, false: enabled.
 */
bool lv_image_cache_is_disabled(void);

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
