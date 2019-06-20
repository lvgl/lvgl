/**
 * @file lv_img_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_img_cache.h"
#include "../lv_hal/lv_hal_tick.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_img_cache_t * img_cache;
static uint32_t life_time = LV_IMG_CACHE_DEF_LIFE_TIME;
static uint16_t slot_num;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Open an image using the image decoder interface and cache it.
 * The image will be left open meaning if the image decoder open callback allocated memory then it will remain.
 * The image is closed if a new image is opened and the new image takes its place in the cache.
 * @param src source of the image. Path to file or pointer to an `lv_img_dsc_t` variable
 * @param style style of the image
 * @return pointer to the cache entry or NULL if can open the image
 */
lv_img_cache_t * lv_img_cache_open(const void * src, const lv_style_t * style)
{
    if(slot_num == 0) {
        LV_LOG_WARN("lv_img_cache_open: the cache size is 0");
        return NULL;
    }

    /*Check if the image is already cached*/
    lv_img_cache_t * cached_dsc = NULL;
    lv_img_cache_t * cache_dsc_reuse = NULL;
    uint16_t i;
    for(i = 0; i < slot_num; i++) {
        if(img_cache[i].dsc.src == src) {
            cached_dsc = &img_cache[i];
            LV_LOG_TRACE("image draw: image found in the cache");
            break;
        }
        /*Meanwhile check for a reusable slot in the cache. It will be required if `src` is not cached*/
        else {

            /*If there is no idea for `cache_dsc_reuse` then let's use this*/
            if(cache_dsc_reuse == NULL) {
                cache_dsc_reuse = &img_cache[i];
            }

            /*There won't be better option then an empty slot so keep it.*/
            if(cache_dsc_reuse->dsc.src == NULL) continue;

            /*If its an empty slot then its the best choice to use*/
            if(img_cache[i].dsc.src == NULL) {
                cache_dsc_reuse = &img_cache[i];
            }
            /*If this image wasn't used for while then reuse it*/
            else if(lv_tick_elaps(img_cache[i].timestamp) > life_time) {
                cache_dsc_reuse = &img_cache[i];
            }
            /* Drop the image which is the fastest to reopen */
            else if(img_cache[i].open_duration < cache_dsc_reuse->open_duration) {
                    cache_dsc_reuse = &img_cache[i];
            }
        }
    }

    /*The image is not cached then cache it now*/
    if(cached_dsc == NULL) {
        /*Close the slot to reuse if it was opened (has a valid source)*/
        if(cache_dsc_reuse->dsc.src) {
            lv_img_decoder_close(&cache_dsc_reuse->dsc);
            LV_LOG_INFO("image draw: cache miss, close and reuse a slot");
        } else {
            LV_LOG_INFO("image draw: cache miss, cached in empty slot");
        }

        /*Open the image and measure the time to open*/
        uint32_t t_start;
        t_start = lv_tick_get();
        const uint8_t * img_data = lv_img_decoder_open(&cache_dsc_reuse->dsc, src, style);
        if(img_data == LV_IMG_DECODER_OPEN_FAIL) {
            LV_LOG_WARN("Image draw cannot open the image resource");
            lv_img_decoder_close(&cache_dsc_reuse->dsc);
            memset(&cache_dsc_reuse->dsc, 0, sizeof(lv_img_decoder_dsc_t));
            memset(&cache_dsc_reuse, 0, sizeof(lv_img_cache_t));
            return NULL;
        }

        cached_dsc = cache_dsc_reuse;
        cached_dsc->img_data = img_data;
        cached_dsc->open_duration = lv_tick_elaps(t_start);
    }

    cached_dsc->timestamp = lv_tick_get();

    return cached_dsc;
}

/**
 * Set the number of images to be cached.
 * More cached images mean more opened image at same time which might mean more memory usage.
 * E.g. if 20 PNG or JPG images are open in the RAM they consume memory while opened in the cache.
 * @param new_slot_num number of image to cache
 */
void lv_img_cache_set_size(uint16_t new_slot_num)
{
    if(img_cache != NULL) {
        /*Clean the cache before free it*/
        lv_img_cache_invalidate_src(NULL);
        lv_mem_free(img_cache);
    }

    /*Reallocate the cache*/
    img_cache = lv_mem_alloc(sizeof(lv_img_cache_t) * new_slot_num);
    lv_mem_assert(img_cache);
    if(img_cache == NULL) {
        slot_num = 0;
        return;
    }
    slot_num = new_slot_num;

    /*Clean the cache*/
    uint16_t i;
    for(i = 0; i < slot_num; i++) {
        memset(&img_cache[i].dsc, 0, sizeof(lv_img_decoder_dsc_t));
        memset(&img_cache[i], 0, sizeof(lv_img_cache_t));
    }
}


/**
 * Set a life time for the cache entries.
 * After this time a cached image is considered unused and it's more probable the it will be replaced by a new image.
 * @param new_life_time the new life time in milliseconds
 */
void lv_img_cache_set_life_time(uint32_t new_life_time)
{
    life_time = new_life_time;
}

/**
 * Invalidate an image source in the cache.
 * Useful if the image source is updated therefore it needs to be cached again.
 * @param src an image source path to a file or pointer to an `lv_img_dsc_t` variable.
 */
void lv_img_cache_invalidate_src(const void * src)
{
    uint16_t i;
    for(i = 0; i < slot_num; i++) {
        if(img_cache[i].dsc.src == src || src == NULL) {
            if(img_cache[i].dsc.src != NULL) {
                lv_img_decoder_close(&img_cache[i].dsc);
            }

            memset(&img_cache[i].dsc, 0, sizeof(lv_img_decoder_dsc_t));
            memset(&img_cache[i], 0, sizeof(lv_img_cache_t));
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
