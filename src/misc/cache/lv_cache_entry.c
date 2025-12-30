/**
* @file lv_cache_entry.c
*
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_cache_entry.h"
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

void lv_cache_entry_reset_ref(lv_cache_entry_t * entry)
{
    LV_ASSERT_NULL(entry);
    entry->ref_cnt = 0;
}

void lv_cache_entry_inc_ref(lv_cache_entry_t * entry)
{
    LV_ASSERT_NULL(entry);
    entry->ref_cnt++;
}

void lv_cache_entry_dec_ref(lv_cache_entry_t * entry)
{
    LV_ASSERT_NULL(entry);
    entry->ref_cnt--;
    if(entry->ref_cnt < 0) {
        LV_LOG_WARN("ref_cnt(%" LV_PRIu32 ") < 0", entry->ref_cnt);
        entry->ref_cnt = 0;
    }
}

int32_t lv_cache_entry_get_ref(lv_cache_entry_t * entry)
{
    LV_ASSERT_NULL(entry);
    return entry->ref_cnt;
}

uint32_t lv_cache_entry_get_node_size(lv_cache_entry_t * entry)
{
    return entry->node_size;
}

void lv_cache_entry_set_node_size(lv_cache_entry_t * entry, uint32_t node_size)
{
    LV_ASSERT_NULL(entry);
    entry->node_size = node_size;
}

bool lv_cache_entry_is_invalid(lv_cache_entry_t * entry)
{
    LV_ASSERT_NULL(entry);
    return entry->flags & LV_CACHE_ENTRY_FLAG_INVALID;
}

void * lv_cache_entry_get_data(lv_cache_entry_t * entry)
{
    LV_ASSERT_NULL(entry);
    return (uint8_t *)entry - entry->node_size;
}

void * lv_cache_entry_acquire_data(lv_cache_entry_t * entry)
{
    LV_ASSERT_NULL(entry);

    lv_cache_entry_inc_ref(entry);
    return lv_cache_entry_get_data(entry);
}

void lv_cache_entry_release_data(lv_cache_entry_t * entry, void * user_data)
{
    LV_UNUSED(user_data);

    LV_ASSERT_NULL(entry);
    if(lv_cache_entry_get_ref(entry) == 0) {
        LV_LOG_ERROR("ref_cnt(%" LV_PRIu32 ") == 0", entry->ref_cnt);
        return;
    }

    lv_cache_entry_dec_ref(entry);
}

lv_cache_entry_t * lv_cache_entry_get_entry(void * data, const uint32_t node_size)
{
    LV_ASSERT_NULL(data);
    return (lv_cache_entry_t *)((uint8_t *)data + node_size);
}

void lv_cache_entry_set_cache(lv_cache_entry_t * entry, const lv_cache_t * cache)
{
    LV_ASSERT_NULL(entry);
    entry->cache = cache;
}

const lv_cache_t * lv_cache_entry_get_cache(const lv_cache_entry_t * entry)
{
    LV_ASSERT_NULL(entry);
    return entry->cache;
}

uint32_t lv_cache_entry_get_size(const uint32_t node_size)
{
    return node_size + sizeof(lv_cache_entry_t);
}

lv_cache_entry_t * lv_cache_entry_alloc(const uint32_t node_size, const lv_cache_t * cache)
{
    void * res = lv_malloc_zeroed(lv_cache_entry_get_size(node_size));
    LV_ASSERT_MALLOC(res)
    if(res == NULL) {
        LV_LOG_ERROR("malloc failed");
        return NULL;
    }
    lv_cache_entry_t * entry = (lv_cache_entry_t *)((uint8_t *)res + node_size);
    lv_cache_entry_init(entry, cache, node_size);
    return entry;
}

void lv_cache_entry_init(lv_cache_entry_t * entry, const lv_cache_t * cache, const uint32_t node_size)
{
    LV_ASSERT_NULL(entry);
    LV_ASSERT_NULL(cache);

    entry->cache = cache;
    entry->node_size = node_size;
    entry->ref_cnt = 0;
    entry->flags = 0;
}

void lv_cache_entry_delete(lv_cache_entry_t * entry)
{
    LV_ASSERT_NULL(entry);

    if(entry->flags & LV_CACHE_ENTRY_FLAG_DISABLE_DELETE) {
        return;
    }

    void * data = lv_cache_entry_get_data(entry);
    lv_free(data);
}

void lv_cache_entry_set_flag(lv_cache_entry_t * entry, uint8_t flags)
{
    LV_ASSERT_NULL(entry);
    entry->flags |= flags;
}

void lv_cache_entry_remove_flag(lv_cache_entry_t * entry, uint8_t flags)
{
    LV_ASSERT_NULL(entry);
    entry->flags &= (~flags);
}

bool lv_cache_entry_has_flag(lv_cache_entry_t * entry, uint8_t flags)
{
    LV_ASSERT_NULL(entry);
    return (entry->flags & flags) == flags;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
