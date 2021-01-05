/**
 * @file lv_img_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_misc/lv_debug.h"
#include "lv_img_cache.h"
#include "lv_img_decoder.h"
#include "lv_draw_img.h"
#include "../lv_hal/lv_hal_tick.h"
#include "../lv_misc/lv_gc.h"

#if defined(LV_GC_INCLUDE)
    #include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */
/*********************
 *      DEFINES
 *********************/
/*Decrement life with this value on every open*/
#define LV_IMG_CACHE_AGING 1

/*Boost life by this factor (multiply time_to_open with this value)*/
#define LV_IMG_CACHE_LIFE_GAIN 1

/*Don't let life to be greater than this limit because it would require a lot of time to
 * "die" from very high values */
#define LV_IMG_CACHE_LIFE_LIMIT 1000

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_IMG_CACHE_DEF_SIZE == 0
    static lv_img_cache_entry_t cache_temp;
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_IMG_CACHE_DEF_SIZE
    static uint16_t entry_cnt;
#endif

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
 * @param color color The color of the image with `LV_IMG_CF_ALPHA_...`
 * @return pointer to the cache entry or NULL if can open the image
 */
lv_img_cache_entry_t * _lv_img_cache_open(const void * src, lv_color_t color)
{
    /*Is the image cached?*/
    lv_img_cache_entry_t * cached_src = NULL;

#if LV_IMG_CACHE_DEF_SIZE
    if(entry_cnt == 0) {
        LV_LOG_WARN("lv_img_cache_open: the cache size is 0");
        return NULL;
    }

    lv_img_cache_entry_t * cache = LV_GC_ROOT(_lv_img_cache_array);

    /*Decrement all lifes. Make the entries older*/
    uint16_t i;
    for(i = 0; i < entry_cnt; i++) {
        if(cache[i].life > INT32_MIN + LV_IMG_CACHE_AGING) {
            cache[i].life -= LV_IMG_CACHE_AGING;
        }
    }

    for(i = 0; i < entry_cnt; i++) {
        bool match = false;
        lv_img_src_t src_type = lv_img_src_get_type(cache[i].dec_dsc.src);
        if(src_type == LV_IMG_SRC_VARIABLE) {
            if(cache[i].dec_dsc.src == src && cache[i].dec_dsc.color.full == color.full) match = true;
        }
        else if(src_type == LV_IMG_SRC_FILE) {
            if(strcmp(cache[i].dec_dsc.src, src) == 0) match = true;
        }

        if(match) {
            /* If opened increment its life.
             * Image difficult to open should live longer to keep avoid frequent their recaching.
             * Therefore increase `life` with `time_to_open`*/
            cached_src = &cache[i];
            cached_src->life += cached_src->dec_dsc.time_to_open * LV_IMG_CACHE_LIFE_GAIN;
            if(cached_src->life > LV_IMG_CACHE_LIFE_LIMIT) cached_src->life = LV_IMG_CACHE_LIFE_LIMIT;
            LV_LOG_TRACE("image draw: image found in the cache");
            break;
        }
    }

    /*The image is not cached then cache it now*/
    if(cached_src) return cached_src;

    /*Find an entry to reuse. Select the entry with the least life*/
    cached_src = &cache[0];
    for(i = 1; i < entry_cnt; i++) {
        if(cache[i].life < cached_src->life) {
            cached_src = &cache[i];
        }
    }

    /*Close the decoder to reuse if it was opened (has a valid source)*/
    if(cached_src->dec_dsc.src) {
        lv_img_decoder_close(&cached_src->dec_dsc);
        LV_LOG_INFO("image draw: cache miss, close and reuse an entry");
    }
    else {
        LV_LOG_INFO("image draw: cache miss, cached to an empty entry");
    }

#else
    cached_src = &cache_temp;
#endif
    /*Open the image and measure the time to open*/
    uint32_t t_start;
    t_start                          = lv_tick_get();
    cached_src->dec_dsc.time_to_open = 0;
    lv_res_t open_res                = lv_img_decoder_open(&cached_src->dec_dsc, src, color);
    if(open_res == LV_RES_INV) {
        LV_LOG_WARN("Image draw cannot open the image resource");
        lv_img_decoder_close(&cached_src->dec_dsc);
        _lv_memset_00(&cached_src->dec_dsc, sizeof(lv_img_decoder_dsc_t));
        _lv_memset_00(cached_src, sizeof(lv_img_cache_entry_t));
        cached_src->life = INT32_MIN; /*Make the empty entry very "weak" to force its use  */
        return NULL;
    }

    cached_src->life = 0;

    /*If `time_to_open` was not set in the open function set it here*/
    if(cached_src->dec_dsc.time_to_open == 0) {
        cached_src->dec_dsc.time_to_open = lv_tick_elaps(t_start);
    }

    if(cached_src->dec_dsc.time_to_open == 0) cached_src->dec_dsc.time_to_open = 1;

    return cached_src;
}

/**
 * Set the number of images to be cached.
 * More cached images mean more opened image at same time which might mean more memory usage.
 * E.g. if 20 PNG or JPG images are open in the RAM they consume memory while opened in the cache.
 * @param new_entry_cnt number of image to cache
 */
void lv_img_cache_set_size(uint16_t new_entry_cnt)
{
#if LV_IMG_CACHE_DEF_SIZE == 0
    LV_UNUSED(new_entry_cnt);
    LV_LOG_WARN("Can't change cache size because it's disabled by LV_IMG_CACHE_DEF_SIZE = 0");
#else
    if(LV_GC_ROOT(_lv_img_cache_array) != NULL) {
        /*Clean the cache before free it*/
        lv_img_cache_invalidate_src(NULL);
        lv_mem_free(LV_GC_ROOT(_lv_img_cache_array));
    }

    /*Reallocate the cache*/
    LV_GC_ROOT(_lv_img_cache_array) = lv_mem_alloc(sizeof(lv_img_cache_entry_t) * new_entry_cnt);
    LV_ASSERT_MEM(LV_GC_ROOT(_lv_img_cache_array));
    if(LV_GC_ROOT(_lv_img_cache_array) == NULL) {
        entry_cnt = 0;
        return;
    }
    entry_cnt = new_entry_cnt;

    /*Clean the cache*/
    uint16_t i;
    for(i = 0; i < entry_cnt; i++) {
        _lv_memset_00(&LV_GC_ROOT(_lv_img_cache_array)[i].dec_dsc, sizeof(lv_img_decoder_dsc_t));
        _lv_memset_00(&LV_GC_ROOT(_lv_img_cache_array)[i], sizeof(lv_img_cache_entry_t));
    }
#endif
}

/**
 * Invalidate an image source in the cache.
 * Useful if the image source is updated therefore it needs to be cached again.
 * @param src an image source path to a file or pointer to an `lv_img_dsc_t` variable.
 */
void lv_img_cache_invalidate_src(const void * src)
{
#if LV_IMG_CACHE_DEF_SIZE
    lv_img_cache_entry_t * cache = LV_GC_ROOT(_lv_img_cache_array);

    uint16_t i;
    for(i = 0; i < entry_cnt; i++) {
        if(cache[i].dec_dsc.src == src || src == NULL) {
            if(cache[i].dec_dsc.src != NULL) {
                lv_img_decoder_close(&cache[i].dec_dsc);
            }

            _lv_memset_00(&cache[i].dec_dsc, sizeof(lv_img_decoder_dsc_t));
            _lv_memset_00(&cache[i], sizeof(lv_img_cache_entry_t));
        }
    }
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
