/**
 * @file lv_image_cache.c
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

static lv_cache_entry_t * add_cb(size_t size);
static lv_cache_entry_t * find_cb(const void * src, lv_cache_src_type_t src_type, uint32_t param1, uint32_t param2);
static void invalidate_cb(lv_cache_entry_t * entry);
static const void * get_data_cb(lv_cache_entry_t * entry);
static void release_cb(lv_cache_entry_t * entry);
static void set_max_size_cb(size_t new_size);
static bool drop_yougest(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_cache_builtin_init(void)
{
    lv_memzero(&_cache_manager, sizeof(lv_cache_manager_t));
    _cache_manager.add_cb = add_cb;
    _cache_manager.find_cb = find_cb;
    _cache_manager.invalidate_cb = invalidate_cb;
    _cache_manager.get_data_cb = get_data_cb;
    _cache_manager.release_cb = release_cb;
    _cache_manager.set_max_size_cb = set_max_size_cb;

    _lv_ll_init(&dsc.entry_ll, sizeof(lv_cache_entry_t));
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_cache_entry_t * add_cb(size_t size)
{
    size_t max_size = lv_cache_get_max_size();
    /*Can't cache data larger than max size*/

    bool temporary = size > max_size ? true : false;
    if(!temporary) {
        /*Keep dropping items until there is enough space*/
        while(dsc.cur_size + size > _cache_manager.max_size) {
            bool ret = drop_yougest();

            /*No item could be dropped.
             *It can happen because the usage_count of the remaining items are not zero.*/
            if(ret == false) {
                temporary = true;
                break;
            }
        }

    }

    lv_cache_entry_t * e = _lv_ll_ins_head(&dsc.entry_ll);
    lv_memzero(e, sizeof(lv_cache_entry_t));
    e->data_size = size;
    e->weight = 1;
    e->temporary = temporary;

    if(temporary) {
        LV_LOG_USER("cache add temporary: %"LV_PRIu32, (uint32_t)size);
    }
    else {
        LV_LOG_USER("cache add: %"LV_PRIu32, (uint32_t)size);
        dsc.cur_size += size;
    }

    return e;
}

static lv_cache_entry_t * find_cb(const void * src, lv_cache_src_type_t src_type, uint32_t param1, uint32_t param2)
{
    lv_cache_entry_t * e = _lv_ll_get_head(&dsc.entry_ll);
    while(e) {
        if(param1 == e->param1 && param2 == e->param2 && src_type == e->src_type &&
           ((src_type == LV_CACHE_SRC_TYPE_PTR && src == e->src) ||
            (src_type == LV_CACHE_SRC_TYPE_STR && strcmp(src, e->src) == 0))) {
            return e;
        }

        e = _lv_ll_get_next(&dsc.entry_ll, e);
    }

    return NULL;
}

static void invalidate_cb(lv_cache_entry_t * entry)
{
    if(entry == NULL) return;

    dsc.cur_size -= entry->data_size;
    LV_LOG_USER("cache drop %"LV_PRIu32, (uint32_t)entry->data_size);

    if(entry->free_src) lv_free((void *)entry->src);
    if(entry->free_data) lv_draw_buf_free((void *)entry->data);

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
        bool ret = drop_yougest();

        /*No item could be dropped.
         *It can happen because the usage_count of the remaining items are not zero.*/
        if(ret == false) return;
    }
}

static bool drop_yougest(void)
{

    int32_t life_min = INT32_MAX;
    lv_cache_entry_t * e_min = NULL;

    lv_cache_entry_t * e = _lv_ll_get_head(&dsc.entry_ll);
    while(e) {
        if(e->life < life_min && e->usage_count == 0) e_min = e;
        e = _lv_ll_get_next(&dsc.entry_ll, e);
    }

    if(e_min == NULL) {
        return false;
    }

    invalidate_cb(e_min);
    return true;

}
