/**
* @file lv_image_cache.c
*
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_assert.h"
#include "lv_image_cache.h"
#include "../../core/lv_global.h"
/*********************
 *      DEFINES
 *********************/
#define img_cache_p (LV_GLOBAL_DEFAULT()->img_cache)
#define img_header_cache_p (LV_GLOBAL_DEFAULT()->img_header_cache)
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_image_cache_drop(const void * src)
{
    /*If user invalidate image, the header cache should be invalidated too.*/
    lv_image_header_cache_drop(src);

#if LV_CACHE_DEF_SIZE > 0
    if(src == NULL) {
        lv_cache_drop_all(img_cache_p, NULL);
        return;
    }

    lv_image_cache_data_t search_key = {
        .src = src,
        .src_type = lv_image_src_get_type(src),
    };

    lv_cache_drop(img_cache_p, &search_key, NULL);
#else
    LV_UNUSED(src);
#endif
}

void lv_image_cache_resize(uint32_t new_size, bool evict_now)
{
#if LV_CACHE_DEF_SIZE > 0
    lv_cache_set_max_size(img_cache_p, new_size, NULL);
    if(evict_now) {
        lv_cache_reserve(img_cache_p, new_size, NULL);
    }
#else
    LV_UNUSED(new_size);
    LV_UNUSED(evict_now);
#endif
}

void lv_image_header_cache_drop(const void * src)
{
#if LV_IMAGE_HEADER_CACHE_DEF_CNT > 0
    if(src == NULL) {
        lv_cache_drop_all(img_header_cache_p, NULL);
        return;
    }

    lv_image_header_cache_data_t search_key = {
        .src = src,
        .src_type = lv_image_src_get_type(src),
    };

    lv_cache_drop(img_header_cache_p, &search_key, NULL);
#else
    LV_UNUSED(src);
#endif
}

void lv_image_header_cache_resize(uint32_t new_size, bool evict_now)
{
#if LV_IMAGE_HEADER_CACHE_DEF_CNT > 0
    lv_cache_set_max_size(img_header_cache_p, new_size, NULL);
    if(evict_now) {
        lv_cache_reserve(img_header_cache_p, new_size, NULL);
    }
#else
    LV_UNUSED(new_size);
    LV_UNUSED(evict_now);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
