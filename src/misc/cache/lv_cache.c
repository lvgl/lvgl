/**
* @file lv_cache.c
*
*/

/*********************
 *      INCLUDES
 *********************/
#include "lv_cache.h"
#include "../lv_assert.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_cache_entry_t {
    lv_cache_t_ * cache;
    uint32_t ref_cnt;
    void * data;
};
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

lv_cache_t_ * lv_cache_create_(const lv_cache_class_t * cache_class,
                              size_t node_size, size_t max_size,
                              lv_cache_compare_cb_t compare_cb,
                              lv_cache_create_cb_t create_cb,
                              lv_cache_free_cb_t free_cb)
{
    lv_cache_t_ * cache = cache_class->alloc_cb();
    LV_ASSERT_MALLOC(cache);

    cache->clz = cache_class;
    cache->node_size = node_size;
    cache->max_size = max_size;
    cache->size = 0;
    cache->compare_cb = compare_cb;
    cache->create_cb = create_cb;
    cache->free_cb = free_cb;

    cache->clz->init_cb(cache);

    return cache;
}

void   lv_cache_destroy_(lv_cache_t_ * cache, void * user_data)
{
    LV_ASSERT_NULL(cache);
    cache->clz->destroy_cb(cache, user_data);
}

void * lv_cache_get_or_create_(lv_cache_t_ * cache, const void * key, void * user_data)
{
    return cache->clz->get_or_create_cb(cache, key, user_data);
}
void   lv_cache_drop_(lv_cache_t_ * cache, const void * key, void * user_data)
{
    cache->clz->drop_cb(cache, key, user_data);
}
void   lv_cache_drop_all_(lv_cache_t_ * cache, void * user_data)
{
    cache->clz->drop_all_cb(cache, user_data);
}
void   lv_cache_set_max_size_(lv_cache_t_ * cache, size_t max_size, void * user_data)
{
    LV_UNUSED(user_data);
    cache->max_size = max_size;
}
size_t lv_cache_get_max_size_(lv_cache_t_ * cache, void * user_data)
{
    LV_UNUSED(user_data);
    return cache->max_size;
}
size_t lv_cache_get_size_(lv_cache_t_ * cache, void * user_data)
{
    LV_UNUSED(user_data);
    return cache->size;
}
size_t lv_cache_get_free_size_(lv_cache_t_ * cache, void * user_data)
{
    LV_UNUSED(user_data);
    return cache->max_size - cache->size;
}
void   lv_cache_set_compare_cb(lv_cache_t_ * cache, lv_cache_compare_cb_t compare_cb, void * user_data)
{
    LV_UNUSED(user_data);
    cache->compare_cb = compare_cb;
}
void   lv_cache_set_create_cb(lv_cache_t_ * cache, lv_cache_create_cb_t alloc_cb, void * user_data)
{
    LV_UNUSED(user_data);
    cache->create_cb = alloc_cb;
}
void   lv_cache_set_free_cb(lv_cache_t_ * cache, lv_cache_free_cb_t free_cb, void * user_data)
{
    LV_UNUSED(user_data);
    cache->free_cb = free_cb;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
