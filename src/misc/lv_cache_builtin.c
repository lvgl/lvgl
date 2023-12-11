/**
 * @file lv_cache_builtin.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_cache.h"
#include "../stdlib/lv_string.h"
#include "../core/lv_global.h"
#include "../misc/lv_ll.h"
#include "../osal/lv_os.h"

/*********************
 *      DEFINES
 *********************/
#define _cache_manager LV_GLOBAL_DEFAULT()->cache_manager
#define dsc LV_GLOBAL_DEFAULT()->cache_builtin_dsc

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_cache_entry_t * add_cb(const void * data, size_t data_size, uint32_t data_type, size_t memory_usage);
static lv_cache_entry_t * find_by_data_cb(const void * data, size_t data_size, uint32_t data_type);
static lv_cache_entry_t * find_by_src_cb(lv_cache_entry_t * entry, const void * src, lv_cache_src_type_t src_type);
static void invalidate_cb(lv_cache_entry_t * entry);
static const void * get_data_cb(lv_cache_entry_t * entry);
static void release_cb(lv_cache_entry_t * entry);
static void set_max_size_cb(size_t new_size);
static void empty_cb(void);
static bool drop_youngest(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#if LV_USE_LOG && LV_LOG_TRACE_CACHE
    #define LV_TRACE_CACHE(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define LV_TRACE_CACHE(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_cache_builtin_init(void)
{
    lv_memzero(&dsc, sizeof(lv_cache_builtin_dsc_t));
    _lv_ll_init(&dsc.entry_ll, sizeof(lv_cache_entry_t));

    _cache_manager.add_cb = add_cb;
    _cache_manager.find_by_data_cb = find_by_data_cb;
    _cache_manager.find_by_src_cb = find_by_src_cb;
    _cache_manager.invalidate_cb = invalidate_cb;
    _cache_manager.get_data_cb = get_data_cb;
    _cache_manager.release_cb = release_cb;
    _cache_manager.set_max_size_cb = set_max_size_cb;
    _cache_manager.empty_cb = empty_cb;
}

void _lv_cache_builtin_deinit(void)
{
    lv_cache_entry_t * entry = _lv_ll_get_head(&dsc.entry_ll);
    lv_cache_entry_t * next;
    while(entry) {
        next = _lv_ll_get_next(&dsc.entry_ll, entry);
        invalidate_cb(entry);
        entry = next;
    }

    _lv_ll_clear(&dsc.entry_ll);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_cache_entry_t * add_cb(const void * data, size_t data_size, uint32_t data_type, size_t memory_usage)
{
    size_t max_size = lv_cache_get_max_size();
    /*Can't cache data larger than max size*/

    bool temporary = memory_usage > max_size ? true : false;
    if(!temporary) {
        /*Keep dropping items until there is enough space*/
        while(dsc.cur_size + memory_usage > _cache_manager.max_size) {
            bool ret = drop_youngest();

            /*No item could be dropped.
             *It can happen because the usage_count of the remaining items are not zero.*/
            if(ret == false) {
                temporary = true;
                break;
            }
        }
    }

    lv_cache_entry_t * entry = _lv_ll_ins_head(&dsc.entry_ll);
    lv_memzero(entry, sizeof(lv_cache_entry_t));
    entry->memory_usage = memory_usage;
    entry->weight = 1;
    entry->temporary = temporary;
    entry->data = data;
    entry->data_size = data_size;
    entry->data_type = data_type;

    if(temporary) {
        LV_TRACE_CACHE("Add temporary cache: %lu bytes", (unsigned long)memory_usage);
    }
    else {
        LV_TRACE_CACHE("Add cache: %lu bytes", (unsigned long)memory_usage);
        dsc.cur_size += memory_usage;
    }

    return entry;
}

static lv_cache_entry_t * find_by_data_cb(const void * data, size_t data_size, uint32_t data_type)
{
    lv_cache_entry_t * entry = _lv_ll_get_head(&dsc.entry_ll);
    while(entry) {
        if(entry->data_type == data_type && entry->data_size == data_size) {
            if(entry->compare_cb(entry->data, data, data_size)) {
                return entry;
            }
        }

        entry = _lv_ll_get_next(&dsc.entry_ll, entry);
    }

    return NULL;
}

static lv_cache_entry_t * find_by_src_cb(lv_cache_entry_t * entry, const void * src, lv_cache_src_type_t src_type)
{
    if(entry == NULL) entry = _lv_ll_get_head(&dsc.entry_ll);
    else entry = _lv_ll_get_next(&dsc.entry_ll, entry);

    while(entry) {
        if(src_type == LV_CACHE_SRC_TYPE_POINTER && entry->src == src) return entry;
        if(src_type == LV_CACHE_SRC_TYPE_PATH && lv_strcmp(entry->src, src) == 0) return entry;
        entry = _lv_ll_get_next(&dsc.entry_ll, entry);
    }

    return NULL;
}

static void invalidate_cb(lv_cache_entry_t * entry)
{
    if(entry == NULL) return;

    dsc.cur_size -= entry->memory_usage;
    LV_TRACE_CACHE("Drop cache: %u bytes", (uint32_t)entry->memory_usage);

    if(entry->invalidate_cb) entry->invalidate_cb(entry);

    _lv_ll_remove(&dsc.entry_ll, entry);
    lv_free(entry);
}

static const void * get_data_cb(lv_cache_entry_t * entry)
{
    lv_cache_entry_t * e = _lv_ll_get_head(&dsc.entry_ll);
    while(e) {
        e->life += e->weight;
        e = _lv_ll_get_next(&dsc.entry_ll, e);
    }

    entry->usage_count++;

    return entry->data;
}

static void release_cb(lv_cache_entry_t * entry)
{
    if(entry == NULL) return;

    if(entry->temporary) {
        invalidate_cb(entry);
    }
    else {
        if(entry->usage_count == 0) {
            LV_LOG_ERROR("More lv_cache_release than lv_cache_get_data");
            return;
        }
        entry->usage_count--;
    }
}

static void set_max_size_cb(size_t new_size)
{
    while(dsc.cur_size > new_size) {
        bool ret = drop_youngest();

        /*No item could be dropped.
         *It can happen because the usage_count of the remaining items are not zero.*/
        if(ret == false) return;
    }
}

static void empty_cb(void)
{
    lv_cache_entry_t * entry_to_drop = NULL;
    lv_cache_entry_t * entry = _lv_ll_get_head(&dsc.entry_ll);
    while(entry) {
        entry_to_drop = entry;
        entry = _lv_ll_get_next(&dsc.entry_ll, entry);
        invalidate_cb(entry_to_drop);
    }
}

static bool drop_youngest(void)
{
    int32_t life_min = INT32_MAX;
    lv_cache_entry_t * entry_to_drop = NULL;

    lv_cache_entry_t * entry = _lv_ll_get_head(&dsc.entry_ll);
    while(entry) {
        if(entry->life < life_min && entry->usage_count == 0) entry_to_drop = entry;
        entry = _lv_ll_get_next(&dsc.entry_ll, entry);
    }

    if(entry_to_drop == NULL) {
        return false;
    }

    invalidate_cb(entry_to_drop);
    return true;
}
