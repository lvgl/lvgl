/**
 * @file lv_img_cache.h
 *
 */

#ifndef LV_CAHCE_H
#define LV_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_img_decoder.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    lv_img_decoder_dsc_t dsc;
    const uint8_t * img_data;

    /* How much time did it take to open the image?
     * When out of cache close the images which are faster to reopen*/
    uint32_t open_duration;

    /*Time stamp when the image was last used*/
    uint32_t timestamp;
}lv_img_cache_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Open an image using the image decoder interface and cache it.
 * The image will be left open meaning if the image decoder open callback allocated memory then it will remain.
 * The image is closed if a new image is opened and the new image takes its place in the cache.
 * @param src source of the image. Path to file or pointer to an `lv_img_dsc_t` variable
 * @param style style of the image
 * @return pointer to the cache entry or NULL if can open the image
 */
lv_img_cache_t * lv_img_cache_open(const void * src, const lv_style_t * style);

/**
 * Set the number of images to be cached.
 * More cached images mean more opened image at same time which might mean more memory usage.
 * E.g. if 20 PNG or JPG images are open in the RAM they consume memory while opened in the cache.
 * @param new_slot_num number of image to cache
 */
void lv_img_cache_set_size(uint16_t new_slot_num);

/**
 * Set a life time for the cache entries.
 * After this time a cached image is considered unused and it's more probable the it will be replaced by a new image.
 * @param new_life_time the new life time in milliseconds
 */
void lv_img_cache_set_life_time(uint32_t new_life_time);

/**
 * Invalidate an image source in the cache.
 * Useful if the image source is updated therefore it needs to be cached again.
 * @param src an image source path to a file or pointer to an `lv_img_dsc_t` variable.
 */
void lv_img_cache_invalidate_src(const void * src);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_CACHE_H*/
