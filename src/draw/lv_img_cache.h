/**
 * @file lv_img_cache.h
 *
 */

#ifndef LV_IMG_CACHE_H
#define LV_IMG_CACHE_H

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

/**
 * When loading images from the network it can take a long time to download and decode the image.
 *
 * To avoid repeating this heavy load images can be cached.
 */
typedef struct {
    lv_img_decoder_dsc_t dec_dsc; /**< Image information*/
    void * user_data; /**< Image cache entry user data*/
} _lv_img_cache_entry_t;

typedef struct {
    _lv_img_cache_entry_t * (*open_cb)(const void * src, lv_color_t color, int32_t frame_id);
    void (*set_size_cb)(uint16_t new_entry_cnt);
    void (*invalidate_src_cb)(const void * src);
} lv_img_cache_manager_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the img cache manager
 * @param manager Pointer to the img cache manager
 */
void lv_img_cache_manager_init(lv_img_cache_manager_t * manager);

/**
 * Apply the img cache manager
 * @param manager Pointer to the img cache manager
 */
void lv_img_cache_manager_apply(const lv_img_cache_manager_t * manager);

/**
 * Open an image using the image decoder interface and cache it.
 * The image will be left open meaning if the image decoder open callback allocated memory then it will remain.
 * The image is closed if a new image is opened and the new image takes its place in the cache.
 * @param src source of the image. Path to file or pointer to an `lv_img_dsc_t` variable
 * @param color The color of the image with `LV_IMG_CF_ALPHA_...`
 * @param frame_id the index of the frame. Used only with animated images, set 0 for normal images
 * @return pointer to the cache entry or NULL if can open the image
 */
_lv_img_cache_entry_t * _lv_img_cache_open(const void * src, lv_color_t color, int32_t frame_id);

/**
 * Set the number of images to be cached.
 * More cached images mean more opened image at same time which might mean more memory usage.
 * E.g. if 20 PNG or JPG images are open in the RAM they consume memory while opened in the cache.
 * @param new_entry_cnt number of image to cache
 */
void lv_img_cache_set_size(uint16_t new_entry_cnt);

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
} /*extern "C"*/
#endif

#endif /*LV_IMG_CACHE_H*/
