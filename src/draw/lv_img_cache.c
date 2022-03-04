/**
 * @file lv_img_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_assert.h"
#include "lv_img_cache.h"
#include "lv_img_decoder.h"
#include "lv_draw_img.h"
#include "../hal/lv_hal_tick.h"
#include "../misc/lv_gc.h"

/*********************
 *      DEFINES
 *********************/
/*Decrement life with this value on every open*/
#define LV_IMG_CACHE_AGING 1

/*Boost life by this factor (multiply time_to_open with this value)*/
#define LV_IMG_CACHE_LIFE_GAIN 1

/*Don't let life to be greater than this limit because it would require a lot of time to
 * "die" from very high values*/
#define LV_IMG_CACHE_LIFE_LIMIT 1000

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_IMG_CACHE_DEF_SIZE
    static bool lv_img_cache_match(const void * src1, const void * src2);
    static void lv_img_cache_close(_lv_img_cache_entry_t * cached_src);
    static _lv_img_cache_entry_t * find_reuse_entry(void);
    static _lv_img_cache_entry_t * find_empty_entry(void);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_IMG_CACHE_DEF_SIZE
    static uint32_t cache_total_size;
    static uint32_t cache_free_size;
    static uint16_t cache_entry_cnt;
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
_lv_img_cache_entry_t * _lv_img_cache_open(const void * src, lv_color_t color, int32_t frame_id)
{
    /*Is the image cached?*/
    _lv_img_cache_entry_t * cached_src = NULL;

#if LV_IMG_CACHE_DEF_SIZE
    if(cache_entry_cnt == 0 || cache_total_size == 0) {
        LV_LOG_WARN("lv_img_cache_open: the cache size is 0");
        return NULL;
    }

    _lv_img_cache_entry_t * cache = LV_GC_ROOT(_lv_img_cache_array);

    /*Decrement all lifes. Make the entries older*/
    uint16_t i;
    for(i = 0; i < cache_entry_cnt; i++) {
        if(cache[i].life > INT32_MIN + LV_IMG_CACHE_AGING) {
            cache[i].life -= LV_IMG_CACHE_AGING;
        }
    }

    for(i = 0; i < cache_entry_cnt; i++) {
        if(color.full == cache[i].dec_dsc.color.full && frame_id == cache[i].dec_dsc.frame_id &&
           lv_img_cache_match(src, cache[i].dec_dsc.src)) {
            /*If opened increment its life.
             *Image difficult to open should live longer to keep avoid frequent their recaching.
             *Therefore increase `life` with `time_to_open`*/
            cached_src = &cache[i];
            cached_src->life += cached_src->dec_dsc.time_to_open * LV_IMG_CACHE_LIFE_GAIN;
            if(cached_src->life > LV_IMG_CACHE_LIFE_LIMIT)
                cached_src->life = LV_IMG_CACHE_LIFE_LIMIT;
            LV_LOG_TRACE("image source found in the cache");
            break;
        }
    }

    /*The image is not cached then cache it now*/
    if(cached_src)
        return cached_src;

    lv_img_header_t header;
    lv_res_t res = lv_img_decoder_get_info(src, &header);

    if(res != LV_RES_OK) {
        LV_LOG_WARN("can't get image info");
        return NULL;
    }

    /*Calculate the memory usage of an image*/
    uint32_t img_req_size = 0;

    if(lv_img_src_get_type(src) == LV_IMG_SRC_FILE) {
        img_req_size = lv_img_buf_get_img_size(header.w, header.h, header.cf);

        if(img_req_size == 0) {
            LV_LOG_WARN("can't get image require memory size, cf = %" PRIu32, header.cf);
            return NULL;
        }
    }

    /*Do not decode image that exceed the limit*/
    if(img_req_size > cache_total_size) {
        LV_LOG_WARN("The memory required for the image (%" PRIu32 " Bytes)"
                    "is greater than the total cache memory size (%" PRIu32 " Bytes)",
                    img_req_size, cache_total_size);
        return NULL;
    }

    cached_src = find_empty_entry();

    /*When the free space is not enough, release the cache until there is enough space*/
    while(img_req_size > cache_free_size) {

        /*Find an entry to reuse. Select the entry with the least life*/
        _lv_img_cache_entry_t * cache_reuse = find_reuse_entry();

        /*Close the decoder to reuse if it was opened (has a valid source)*/
        if(cache_reuse) {
            LV_LOG_INFO("image draw: cache miss, close and reuse an entry");
            lv_img_cache_close(cache_reuse);
            cached_src = cache_reuse;
        }
        else {
            LV_LOG_INFO("image draw: cache miss, cached to an empty entry");
            break;
        }
    };

    /*If the number of cache entries is insufficient, look for the entry that can be reused*/
    if(cached_src == NULL) {
        LV_LOG_INFO("cache entry is full, find reuse entry");
        cached_src = find_reuse_entry();
        lv_img_cache_close(cached_src);
    }

#else
    cached_src = &LV_GC_ROOT(_lv_img_cache_single);
#endif

    /*Open the image and measure the time to open*/
    uint32_t t_start = lv_tick_get();
    lv_res_t open_res = lv_img_decoder_open(&cached_src->dec_dsc, src, color, frame_id);

    if(open_res == LV_RES_INV) {
        LV_LOG_WARN("Image draw cannot open the image resource");
        lv_memset_00(cached_src, sizeof(_lv_img_cache_entry_t));
        cached_src->life = INT32_MIN; /*Make the empty entry very "weak" to force its us*/
        return NULL;
    }
#if LV_IMG_CACHE_DEF_SIZE
    else {
        /*Record the amount of memory occupied*/
        cached_src->mem_cost_size = img_req_size;

        LV_LOG_INFO("cache_free_size(%" PRIu32 ") -= %" PRIu32, cache_free_size, img_req_size);
        cache_free_size -= img_req_size;
    }
#endif

    cached_src->life = 0;

    /*If `time_to_open` was not set in the open function set it here*/
    if(cached_src->dec_dsc.time_to_open == 0) {
        cached_src->dec_dsc.time_to_open = lv_tick_elaps(t_start);
    }

    if(cached_src->dec_dsc.time_to_open == 0)
        cached_src->dec_dsc.time_to_open = 1;

    return cached_src;
}

/**
 * Set the number of images to be cached.
 * More cached images mean more opened image at same time which might mean more memory usage.
 * E.g. if 20 PNG or JPG images are open in the RAM they consume memory while opened in the cache.
 * @param new_entry_cnt number of image to cache
 * @param new_total_size cache total memory size
 */
void lv_img_cache_set_size(uint16_t new_entry_cnt, uint32_t new_total_size)
{
#if LV_IMG_CACHE_DEF_SIZE == 0
    LV_UNUSED(new_entry_cnt);
    LV_UNUSED(new_total_size);
    LV_LOG_WARN("Can't change cache size because it's disabled by LV_IMG_CACHE_DEF_SIZE = 0");
#else
    if(LV_GC_ROOT(_lv_img_cache_array) != NULL) {
        /*Clean the cache before free it*/
        lv_img_cache_invalidate_src(NULL);
        lv_mem_free(LV_GC_ROOT(_lv_img_cache_array));
    }

    /*Reallocate the cache*/
    LV_GC_ROOT(_lv_img_cache_array) = lv_mem_alloc(sizeof(_lv_img_cache_entry_t) * new_entry_cnt);
    LV_ASSERT_MALLOC(LV_GC_ROOT(_lv_img_cache_array));
    if(LV_GC_ROOT(_lv_img_cache_array) == NULL) {
        cache_entry_cnt = 0;
        return;
    }
    cache_entry_cnt = new_entry_cnt;

    /*Clean the cache*/
    lv_memset_00(LV_GC_ROOT(_lv_img_cache_array), cache_entry_cnt * sizeof(_lv_img_cache_entry_t));
    cache_total_size = new_total_size;
    cache_free_size = new_total_size;
#endif
}

/**
 * Invalidate an image source in the cache.
 * Useful if the image source is updated therefore it needs to be cached again.
 * @param src an image source path to a file or pointer to an `lv_img_dsc_t` variable.
 */
void lv_img_cache_invalidate_src(const void * src)
{
    LV_UNUSED(src);
#if LV_IMG_CACHE_DEF_SIZE
    _lv_img_cache_entry_t * cache = LV_GC_ROOT(_lv_img_cache_array);

    uint16_t i;
    for(i = 0; i < cache_entry_cnt; i++) {
        if(src == NULL || lv_img_cache_match(src, cache[i].dec_dsc.src)) {
            if(cache[i].dec_dsc.src != NULL) {
                lv_img_cache_close(&cache[i]);
            }

            lv_memset_00(&cache[i], sizeof(_lv_img_cache_entry_t));
        }
    }
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_IMG_CACHE_DEF_SIZE
static bool lv_img_cache_match(const void * src1, const void * src2)
{
    lv_img_src_t src_type = lv_img_src_get_type(src1);
    if(src_type == LV_IMG_SRC_VARIABLE)
        return src1 == src2;
    if(src_type != LV_IMG_SRC_FILE)
        return false;
    if(lv_img_src_get_type(src2) != LV_IMG_SRC_FILE)
        return false;
    return strcmp(src1, src2) == 0;
}

static void lv_img_cache_close(_lv_img_cache_entry_t * cached_src)
{
    lv_img_decoder_close(&cached_src->dec_dsc);

    LV_LOG_INFO("cache_free_size(%" PRIu32 ") += %" PRIu32, cache_free_size, cached_src->mem_cost_size);
    cache_free_size += cached_src->mem_cost_size;

    /*Mark the entry to be released*/
    lv_memset_00(cached_src, sizeof(_lv_img_cache_entry_t));
}

static _lv_img_cache_entry_t * find_reuse_entry(void)
{
    _lv_img_cache_entry_t * cache = LV_GC_ROOT(_lv_img_cache_array);
    _lv_img_cache_entry_t * cached_src = NULL;

    int32_t life_min = INT32_MAX;

    for(uint16_t i = 0; i < cache_entry_cnt; i++) {
        _lv_img_cache_entry_t * cache_p = &cache[i];
        if(cache_p->dec_dsc.src != NULL) {
            if(cache_p->life < life_min) {
                life_min = cache_p->life;
                cached_src = &cache[i];
            }
        }
    }

    return cached_src;
}

static _lv_img_cache_entry_t * find_empty_entry(void)
{
    _lv_img_cache_entry_t * cache = LV_GC_ROOT(_lv_img_cache_array);
    _lv_img_cache_entry_t * cached_src = NULL;

    for(uint16_t i = 0; i < cache_entry_cnt; i++) {
        _lv_img_cache_entry_t * cache_p = &cache[i];
        if(cache_p->dec_dsc.src == NULL) {
            cached_src = cache_p;
            break;
        }
    }

    return cached_src;
}
#endif
