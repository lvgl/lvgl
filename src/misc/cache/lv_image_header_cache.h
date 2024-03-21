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

#if LV_IMAGE_HEADER_CACHE_DEF_CNT > 0

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
 * @return LV_RESULT_OK: initialization succeeded, LV_RESULT_INVALID: failed.
 */
lv_result_t lv_image_header_cache_init(void);

/**
 * Resize image header cache.
 * @param new_size  new size of the cache in count of image headers.
 * @param evict_now true: evict the image headers should be removed by the eviction policy, false: wait for the next cache cleanup.
 */
void lv_image_header_cache_resize(uint32_t new_size, bool evict_now);

#endif /*LV_IMAGE_HEADER_CACHE_DEF_CNT > 0*/

/**
 * Invalidate image header cache. Use NULL to invalidate all image headers.
 * It's also automatically called when an image is invalidated.
 * @param src pointer to an image source.
 */
void lv_image_header_cache_drop(const void * src);

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
