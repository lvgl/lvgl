/**
* @file lv_cache.c
*
*/

/*********************
 *      INCLUDES
 *********************/
#include "lv_cache.h"
#include "../../stdlib/lv_sprintf.h"
#include "../lv_assert.h"
#include "lv_cache_entry_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void cache_drop_internal_no_lock(lv_cache_t * cache, const void * key, void * user_data);
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

lv_cache_t * lv_cache_create(const lv_cache_class_t * cache_class,
                             size_t node_size, size_t max_size,
                             lv_cache_ops_t ops)
{
    lv_cache_t * cache = cache_class->alloc_cb();
    LV_ASSERT_MALLOC(cache);

    cache->clz = cache_class;
    cache->node_size = node_size;
    cache->max_size = max_size;
    cache->size = 0;
    cache->ops = ops;

    if(cache->clz->init_cb(cache) == false) {
        LV_LOG_ERROR("Cache init failed");
        lv_free(cache);
        return NULL;
    }

    lv_mutex_init(&cache->lock);

    return cache;
}

void lv_cache_destroy(lv_cache_t * cache, void * user_data)
{
    LV_ASSERT_NULL(cache);

    lv_mutex_lock(&cache->lock);
    cache->clz->destroy_cb(cache, user_data);
    lv_mutex_unlock(&cache->lock);
    lv_mutex_delete(&cache->lock);
    lv_free(cache);
}

lv_cache_entry_t * lv_cache_acquire(lv_cache_t * cache, const void * key, void * user_data)
{
    LV_ASSERT_NULL(cache);
    LV_ASSERT_NULL(key);

    lv_mutex_lock(&cache->lock);
    lv_cache_entry_t * entry = cache->clz->get_cb(cache, key, user_data);
    if(entry != NULL) {
        lv_cache_entry_acquire_data(entry);
    }
    lv_mutex_unlock(&cache->lock);
    return entry;
}
void lv_cache_release(lv_cache_t * cache, lv_cache_entry_t * entry, void * user_data)
{
    LV_ASSERT_NULL(entry);

    lv_mutex_lock(&cache->lock);
    lv_cache_entry_release_data(entry, user_data);

    if(lv_cache_entry_get_ref(entry) == 0 && lv_cache_entry_is_invalid(entry)) {
        cache->ops.free_cb(lv_cache_entry_get_data(entry), user_data);
        lv_cache_entry_delete(entry);
    }
    lv_mutex_unlock(&cache->lock);
}
lv_cache_entry_t * lv_cache_add(lv_cache_t * cache, const void * key, void * user_data)
{
    LV_ASSERT_NULL(cache);
    LV_ASSERT_NULL(key);

    lv_mutex_lock(&cache->lock);
    lv_cache_entry_t * entry = cache->clz->add_cb(cache, key, user_data);
    if(entry != NULL) {
        lv_cache_entry_acquire_data(entry);
    }
    lv_mutex_unlock(&cache->lock);
    return entry;
}
lv_cache_entry_t * lv_cache_acquire_or_create(lv_cache_t * cache, const void * key, void * user_data)
{
    LV_ASSERT_NULL(cache);
    LV_ASSERT_NULL(key);

    lv_mutex_lock(&cache->lock);
    lv_cache_entry_t * entry = cache->clz->get_cb(cache, key, user_data);
    if(entry != NULL) {
        lv_cache_entry_acquire_data(entry);
        lv_mutex_unlock(&cache->lock);
        return entry;
    }
    entry = cache->clz->add_cb(cache, key, user_data);
    if(entry == NULL) {
        lv_mutex_unlock(&cache->lock);
        return NULL;
    }
    bool create_res = cache->ops.create_cb(lv_cache_entry_get_data(entry), user_data);
    if(create_res == false) {
        cache->clz->remove_cb(cache, entry, user_data);
        lv_cache_entry_delete(entry);
        entry = NULL;
    }
    else {
        lv_cache_entry_acquire_data(entry);
    }
    lv_mutex_unlock(&cache->lock);
    return entry;
}
void lv_cache_drop(lv_cache_t * cache, const void * key, void * user_data)
{
    LV_ASSERT_NULL(cache);
    LV_ASSERT_NULL(key);

    lv_mutex_lock(&cache->lock);
    cache_drop_internal_no_lock(cache, key, user_data);
    lv_mutex_unlock(&cache->lock);
}
void lv_cache_drop_all(lv_cache_t * cache, void * user_data)
{
    LV_ASSERT_NULL(cache);

    lv_mutex_lock(&cache->lock);
    cache->clz->drop_all_cb(cache, user_data);
    lv_mutex_unlock(&cache->lock);
}

void lv_cache_set_max_size(lv_cache_t * cache, size_t max_size, void * user_data)
{
    LV_UNUSED(user_data);
    cache->max_size = max_size;
}
size_t lv_cache_get_max_size(lv_cache_t * cache, void * user_data)
{
    LV_UNUSED(user_data);
    return cache->max_size;
}
size_t lv_cache_get_size(lv_cache_t * cache, void * user_data)
{
    LV_UNUSED(user_data);
    return cache->size;
}
size_t lv_cache_get_free_size(lv_cache_t * cache, void * user_data)
{
    LV_UNUSED(user_data);
    return cache->max_size - cache->size;
}
void lv_cache_set_compare_cb(lv_cache_t * cache, lv_cache_compare_cb_t compare_cb, void * user_data)
{
    LV_UNUSED(user_data);
    cache->ops.compare_cb = compare_cb;
}
void lv_cache_set_create_cb(lv_cache_t * cache, lv_cache_create_cb_t alloc_cb, void * user_data)
{
    LV_UNUSED(user_data);
    cache->ops.create_cb = alloc_cb;
}
void lv_cache_set_free_cb(lv_cache_t * cache, lv_cache_free_cb_t free_cb, void * user_data)
{
    LV_UNUSED(user_data);
    cache->ops.free_cb = free_cb;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
static void cache_drop_internal_no_lock(lv_cache_t * cache, const void * key, void * user_data)
{
    lv_cache_entry_t * entry = cache->clz->get_cb(cache, key, user_data);
    if(entry == NULL) {
        return;
    }

    if(lv_cache_entry_get_ref(entry) == 0) {
        cache->clz->remove_cb(cache, entry, user_data);
        cache->ops.free_cb(lv_cache_entry_get_data(entry), user_data);
        lv_cache_entry_delete(entry);
    }
    else {
        lv_cache_entry_set_invalid(entry, true);
        cache->clz->remove_cb(cache, entry, user_data);
    }
}
