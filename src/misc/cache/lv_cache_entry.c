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
#include "_lv_cache_entry_internal.h"
#include "_lv_cache_internal.h"
#include "lv_cache.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_cache_entry_t_ {
    const lv_cache_t_ * cache;
    int32_t ref_cnt;
    uint32_t generation;
    uint32_t node_size;

    bool is_invalid;
    lv_mutex_t lock;
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

void     lv_cache_entry_ref_reset(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    entry->ref_cnt = 0;
}
void     lv_cache_entry_ref_inc(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    entry->ref_cnt++;
}
void     lv_cache_entry_ref_dec(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    entry->ref_cnt--;
    if(entry->ref_cnt < 0) {
        LV_LOG_WARN("lv_cache_entry_ref_dec: ref_cnt(%" LV_PRIu32 ") < 0", entry->ref_cnt);
        entry->ref_cnt = 0;
    }
}
int32_t  lv_cache_entry_ref_get(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    return entry->ref_cnt;
}
uint32_t lv_cache_entry_get_generation(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    return entry->generation;
}
void     lv_cache_entry_inc_generation(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    entry->generation++;
}
void     lv_cache_entry_set_generation(lv_cache_entry_t_ * entry, uint32_t generation)
{
    LV_ASSERT_NULL(entry);
    entry->generation = generation;
}
uint32_t lv_cache_entry_get_node_size(lv_cache_entry_t_ * entry)
{
    return entry->node_size;
}
void     lv_cache_entry_set_node_size(lv_cache_entry_t_ * entry, uint32_t node_size)
{
    entry->node_size = node_size;
}
void     lv_cache_entry_set_invalid(lv_cache_entry_t_ * entry, bool is_invalid)
{
    LV_ASSERT_NULL(entry);
    entry->is_invalid = is_invalid;
}
bool     lv_cache_entry_is_invalid(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    return entry->is_invalid;
}
void  *  lv_cache_entry_get_data(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    return (uint8_t *)entry - entry->node_size;
}
void  * lv_cache_entry_acquire_data(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);

    lv_mutex_lock(&entry->lock);
    lv_cache_entry_ref_inc(entry);
    lv_mutex_unlock(&entry->lock);
    return lv_cache_entry_get_data(entry);
}
void    lv_cache_entry_release_data(lv_cache_entry_t_ * entry, void * user_data)
{
    LV_ASSERT_NULL(entry);
    if(lv_cache_entry_ref_get(entry) == 0) {
        LV_LOG_ERROR("lv_cache_entry_release_data: ref_cnt(%" LV_PRIu32 ") == 0", entry->ref_cnt);
        return;
    }

    lv_mutex_lock(&entry->lock);
    lv_cache_entry_ref_dec(entry);

    if(lv_cache_entry_ref_get(entry) == 0 && lv_cache_entry_is_invalid(entry)) {
        lv_cache_t_ * cache = (lv_cache_t_ *)lv_cache_entry_get_cache(entry);
        lv_cache_drop_(cache, lv_cache_entry_get_data(entry), user_data);
    }
    lv_mutex_unlock(&entry->lock);
}
lv_cache_entry_t_ * lv_cache_entry_get_entry(void * data, const uint32_t node_size)
{
    LV_ASSERT_NULL(data);
    return (lv_cache_entry_t_ *)((uint8_t *)data + node_size);
}
void     lv_cache_entry_set_cache(lv_cache_entry_t_ * entry, const lv_cache_t_ * cache)
{
    LV_ASSERT_NULL(entry);
    entry->cache = cache;
}
const lv_cache_t_ * lv_cache_entry_get_cache(const lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);
    return entry->cache;
}

uint32_t lv_cache_entry_get_size(const uint32_t node_size)
{
    return node_size + sizeof(lv_cache_entry_t_);
}
lv_cache_entry_t_* lv_cache_entry_alloc(const uint32_t node_size, const lv_cache_t_* cache)
{
    void * res = lv_malloc_zeroed(lv_cache_entry_get_size(node_size));
    LV_ASSERT_MALLOC(res)
    if(res == NULL) {
        LV_LOG_ERROR("lv_cache_entry_alloc: malloc failed");
        return NULL;
    }
    lv_cache_entry_t_ * entry = (lv_cache_entry_t_ *)res;
    lv_cache_entry_init(entry, cache, node_size);
    return (lv_cache_entry_t_ *)((uint8_t *)entry + node_size);
}
void lv_cache_entry_init(lv_cache_entry_t_ * entry, const lv_cache_t_ * cache, const uint32_t node_size)
{
    LV_ASSERT_NULL(entry);
    LV_ASSERT_NULL(cache);

    entry->cache = cache;
    entry->node_size = node_size;
    entry->ref_cnt = 0;
    entry->generation = 0;
    entry->is_invalid = false;
    lv_mutex_init(&entry->lock);
}
void lv_cache_entry_delete(lv_cache_entry_t_ * entry)
{
    LV_ASSERT_NULL(entry);

    lv_mutex_delete(&entry->lock);
    void * data = lv_cache_entry_get_data(entry);
    lv_free(data);
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
