/**
 * @file lv_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_cache.h"
#include "../stdlib/lv_string.h"
#include "../osal/lv_os.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define _cache_manager LV_GLOBAL_DEFAULT()->cache_manager

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
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

void _lv_cache_init(void)
{
    lv_memzero(&_cache_manager, sizeof(lv_cache_manager_t));
    lv_mutex_init(&_cache_manager.mutex);
}

lv_cache_entry_t * lv_cache_add(size_t size)
{
    LV_ASSERT(_cache_manager.locked);
    if(_cache_manager.add_cb == NULL) return NULL;

    return _cache_manager.add_cb(size);
}

lv_cache_entry_t * lv_cache_find(const void * src_ptr, lv_cache_src_type_t src_type, uint32_t param1, uint32_t param2)
{
    LV_ASSERT(_cache_manager.locked);
    if(_cache_manager.find_cb == NULL) return NULL;

    return _cache_manager.find_cb(src_ptr, src_type, param1, param2);
}


void lv_cache_invalidate(lv_cache_entry_t * entry)
{
    LV_ASSERT(_cache_manager.locked);
    if(_cache_manager.invalidate_cb == NULL) return;

    _cache_manager.invalidate_cb(entry);
}

const void * lv_cache_get_data(lv_cache_entry_t * entry)
{
    LV_ASSERT(_cache_manager.locked);
    if(_cache_manager.get_data_cb == NULL) return NULL;

    const void * data = _cache_manager.get_data_cb(entry);
    return data;
}

void lv_cache_release(lv_cache_entry_t * entry)
{
    LV_ASSERT(_cache_manager.locked);
    if(_cache_manager.release_cb == NULL) return;

    _cache_manager.release_cb(entry);
}

void lv_cache_set_max_size(size_t size)
{
    LV_ASSERT(_cache_manager.locked);
    if(_cache_manager.set_max_size_cb == NULL) return;

    _cache_manager.set_max_size_cb(size);
    _cache_manager.max_size = size;
}

size_t lv_cache_get_max_size(void)
{
    return _cache_manager.max_size;
}

void lv_cache_lock(void)
{
    lv_mutex_lock(&_cache_manager.mutex);
    _cache_manager.locked = 1;
}

void lv_cache_unlock(void)
{
    _cache_manager.locked = 0;
    lv_mutex_unlock(&_cache_manager.mutex);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
