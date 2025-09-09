/**
* @file lv_cache_sc_da.c
*
*/
/*********************************************\
*                                            *
*  ┏ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┓           *
*                                            *
*  ┃   Second Chance Cache       ┃           *
*                                            *
*  ┃  ┌───┬───┬───┬───┬───┐      ┃           *
*     │ B │ E │ A │ D │ C │                  *
*  ┃  ├─┬─┼─┬─┼─┬─┼─┬─┼─┬─┤      ┃           *
*     │1│ │0│ │1│ │1│ │0│ │                  *
*  ┃  └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘      ┃           *
*     [0] [1] [2] [3] [4]                    *
*  ┃    ▲   ▲       ▲       ▲    ┃           *
*       │   │       │       │                *
*  ┃    │   │       │   ┌ ─ ┴ ┐  ┃           *
*       │   │       │   │ add │              *
*  ┃    │   │       │   │ new │  ┃           *
*       │   │       │   │here │              *
*  ┃    │   │       │   └ ─ ─ ┘  ┃           *
*       │   │       │                        *
*  ┃    │   │   ┌ ─ ┴ ─ ─ ─ ─ ┐  ┃           *
*       │   │   │ recently    │              *
*  ┃    │   │   │ used bit=1  │  ┃           *
*       │   │   │ (accessed)  │              *
*  ┃    │   │   └ ─ ─ ─ ─ ─ ─ ┘  ┃           *
*       │   │                                *
*  ┃    │   └ ─ ─ victim bit=0   ┃           *
*       │       (will be evicted)            *
*  ┃    └ ─ ─ ─ ─ victim bit=1   ┃           *
*         (gets second chance,               *
*  ┃      bit reset to 0)        ┃           *
*                                            *
*  ┃  Eviction Process:          ┃           *
*     1. Find first bit=0                    *
*  ┃  2. If none, reset all to 0 ┃           *
*     3. Replace first entry                 *
*  ┃                             ┃           *
*  ┗ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┛           *
*                                            *
* ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓ *
* ┃       Buffer Entry Structure           ┃ *
* ┃  ┌────────────┬──────────────────────┐ ┃ *
* ┃  │   DATA     │     ENTRY_DATA       │ ┃ *
* ┃  │ (user type)│ (lv_cache_entry_t)   │ ┃ *
* ┃  └────────────┴──────────────────────┘ ┃ *
* ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛ *
\*********************************************/

/*********************
 *      INCLUDES
 *********************/

#include "lv_cache_sc_da.h"
#include "../lv_cache_entry.h"
#include "../lv_cache_entry_private.h"
#include "../../../stdlib/lv_sprintf.h"
#include "../../../stdlib/lv_string.h"

#include "../../lv_iter.h"
#include "../../lv_assert.h"
#include "../../lv_math.h"
#include "../../lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef uint32_t(get_data_size_cb_t)(const void * data);

typedef struct {
    lv_cache_t cache;
    uint8_t * data;
    size_t capacity;
} lv_cache_sc_da_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * alloc_cb(void);
static bool init_cnt_cb(lv_cache_t * cache);
static void destroy_cb(lv_cache_t * cache, void * user_data);

static lv_cache_entry_t * get_cb(lv_cache_t * cache, const void * key,
                                 void * user_data);
static lv_cache_entry_t * add_cb(lv_cache_t * cache, const void * key,
                                 void * user_data);
static void remove_cb(lv_cache_t * cache, lv_cache_entry_t * entry,
                      void * user_data);
static void drop_cb(lv_cache_t * cache, const void * key, void * user_data);
static void drop_all_cb(lv_cache_t * cache, void * user_data);
static lv_cache_entry_t * get_victim_cb(lv_cache_t * cache, void * user_data);
static lv_cache_reserve_cond_res_t reserve_cond_cb(lv_cache_t * cache,
                                                   const void * key,
                                                   size_t reserved_size,
                                                   void * user_data);

static void * alloc_new_entry(lv_cache_sc_da_t * da, const void * key,
                              void * user_data);

static lv_iter_t * cache_iter_create_cb(lv_cache_t * cache);
static lv_result_t cache_iter_next_cb(void * instance, void * context,
                                      void * elem);

static lv_cache_entry_t * get_possible_victim(lv_cache_sc_da_t * da,
                                              size_t index);

static inline void set_second_chance(lv_cache_entry_t * entry, bool value);
static inline bool has_second_chance(lv_cache_entry_t * entry);
static inline void get_entry(lv_cache_sc_da_t * da, size_t index,
                             void ** cache_data, lv_cache_entry_t ** cache_entry);

/**********************
 *  GLOBAL VARIABLES
 **********************/
const lv_cache_class_t lv_cache_class_sc_da = {
    .alloc_cb = alloc_cb,
    .init_cb = init_cnt_cb,
    .destroy_cb = destroy_cb,

    .get_cb = get_cb,
    .add_cb = add_cb,
    .remove_cb = remove_cb,
    .drop_cb = drop_cb,
    .drop_all_cb = drop_all_cb,
    .get_victim_cb = get_victim_cb,
    .reserve_cond_cb = reserve_cond_cb,
    .iter_create_cb = cache_iter_create_cb,
};

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#define LV_CACHE_ENTRY_SIZE lv_cache_entry_get_size(0)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void get_entry(lv_cache_sc_da_t * da, size_t index,
                             void ** cache_data, lv_cache_entry_t ** cache_entry)
{
    const size_t node_size = lv_cache_entry_get_size(da->cache.node_size);
    *cache_data = da->data + (index * node_size);
    *cache_entry =
        lv_cache_entry_get_entry(*cache_data, da->cache.node_size);
}
static inline void set_second_chance(lv_cache_entry_t * entry, bool value)
{
    if(value) {
        lv_cache_entry_set_flag(entry, LV_CACHE_ENTRY_FLAG_CLASS_CUSTOM);
    }
    else {
        lv_cache_entry_remove_flag(entry, LV_CACHE_ENTRY_FLAG_CLASS_CUSTOM);
    }
}

static inline bool has_second_chance(lv_cache_entry_t * entry)
{
    return lv_cache_entry_has_flag(entry, LV_CACHE_ENTRY_FLAG_CLASS_CUSTOM);
}

static void * alloc_new_entry(lv_cache_sc_da_t * da, const void * key,
                              void * user_data)
{
    LV_UNUSED(user_data);

    LV_ASSERT_NULL(da);
    LV_ASSERT_NULL(key);

    if(da == NULL || key == NULL) {
        return NULL;
    }
    const size_t node_size = lv_cache_entry_get_size(da->cache.node_size);

    if(da->capacity == da->cache.size) {
        if(da->capacity == da->cache.max_size) {
            LV_LOG_ERROR(
                "Reached maximum size of cache. Unable to allocate a new entry");
            return NULL;
        }

        const size_t new_capacity =
            LV_MIN(da->capacity == 0 ? 1 : da->capacity * 2,
                   da->cache.max_size);

        uint8_t * new_data = (uint8_t *)lv_realloc(
                                 da->data, new_capacity * node_size);

        if(!new_data) {
            LV_LOG_ERROR("Failed to allocate new data for cache");
            return NULL;
        }

        da->data = new_data;
        da->capacity = new_capacity;
    }
    void * last_da_entry;
    lv_cache_entry_t * last_cache_entry;

    get_entry(da, da->cache.size, &last_da_entry, &last_cache_entry);

    lv_memcpy(last_da_entry, key, da->cache.node_size);
    lv_cache_entry_init(last_cache_entry, &da->cache, da->cache.node_size);
    lv_cache_entry_set_flag(last_cache_entry, LV_CACHE_ENTRY_FLAG_DISABLE_DELETE);

    /*New entries start with their second chance set*/
    set_second_chance(last_cache_entry, true);
    return last_cache_entry;
}

static void * alloc_cb(void)
{
    return lv_calloc(1, sizeof(lv_cache_sc_da_t));
}

static bool init_cnt_cb(lv_cache_t * cache)
{
    LV_ASSERT_NULL(cache);
    lv_cache_sc_da_t * da = (lv_cache_sc_da_t *)cache;
    return da->cache.node_size > 0 && da->cache.ops.compare_cb &&
           da->cache.ops.free_cb;
}

static void destroy_cb(lv_cache_t * cache, void * user_data)
{
    LV_UNUSED(user_data);

    lv_cache_sc_da_t * da = (lv_cache_sc_da_t *)cache;

    LV_ASSERT_NULL(da);

    if(da == NULL) {
        return;
    }

    cache->clz->drop_all_cb(cache, user_data);
    cache->size = 0;
    lv_free(da->data);
    da->data = NULL;
    da->capacity = 0;
}

static lv_cache_entry_t * get_cb(lv_cache_t * cache, const void * key,
                                 void * user_data)
{
    LV_UNUSED(user_data);

    lv_cache_sc_da_t * da = (lv_cache_sc_da_t *)cache;

    LV_ASSERT_NULL(da);
    LV_ASSERT_NULL(key);

    if(da == NULL || key == NULL) {
        return NULL;
    }

    /* Linear search */
    lv_cache_entry_t * cache_entry = NULL;
    for(size_t i = 0; i < da->cache.size; ++i) {
        void * curr_da_entry;
        lv_cache_entry_t * curr_cache_entry;
        get_entry(da, i, &curr_da_entry, &curr_cache_entry);

        if(da->cache.ops.compare_cb(curr_da_entry, key) == 0) {
            cache_entry = curr_cache_entry;
            /*When an entry is used, we set it's second chance to true again*/
            set_second_chance(cache_entry, true);
            break;
        }
    }
    return cache_entry;
}

static lv_cache_entry_t * add_cb(lv_cache_t * cache, const void * key,
                                 void * user_data)
{
    LV_UNUSED(user_data);

    lv_cache_sc_da_t * da = (lv_cache_sc_da_t *)cache;

    LV_ASSERT_NULL(da);
    LV_ASSERT_NULL(key);

    if(da == NULL || key == NULL) {
        return NULL;
    }

    lv_cache_entry_t * entry = alloc_new_entry(da, key, user_data);
    if(entry == NULL) {
        return NULL;
    }

    cache->size += 1;

    return entry;
}

static void remove_cb(lv_cache_t * cache, lv_cache_entry_t * entry,
                      void * user_data)
{
    LV_UNUSED(user_data);

    lv_cache_sc_da_t * da = (lv_cache_sc_da_t *)cache;

    LV_ASSERT_NULL(da);
    LV_ASSERT_NULL(entry);

    if(da == NULL || entry == NULL) {
        return;
    }

    const size_t entry_size = lv_cache_entry_get_size(da->cache.node_size);
    uint8_t * da_entry_to_remove = (uint8_t *)lv_cache_entry_get_data(entry);

    void * last_da_entry;
    lv_cache_entry_t * last_cache_entry;
    get_entry(da, cache->size - 1, &last_da_entry, &last_cache_entry);

    if(da_entry_to_remove != last_da_entry) {
        lv_memcpy(da_entry_to_remove, last_da_entry, entry_size);
    }
    cache->size -= 1;
}

static void drop_cb(lv_cache_t * cache, const void * key, void * user_data)
{
    lv_cache_sc_da_t * da = (lv_cache_sc_da_t *)cache;

    LV_ASSERT_NULL(da);
    LV_ASSERT_NULL(key);

    if(da == NULL || key == NULL) {
        return;
    }
    lv_cache_entry_t * entry = cache->clz->get_cb(cache, key, user_data);
    if(!entry) {
        return;
    }

    const size_t entry_size = lv_cache_entry_get_size(da->cache.node_size);
    uint8_t * da_entry_to_remove = (uint8_t *)lv_cache_entry_get_data(entry);

    void * last_da_entry;
    lv_cache_entry_t * last_cache_entry;
    get_entry(da, cache->size - 1, &last_da_entry, &last_cache_entry);

    if(da_entry_to_remove != last_da_entry) {
        lv_memcpy(da_entry_to_remove, last_da_entry, entry_size);
    }
    cache->ops.free_cb(da_entry_to_remove, user_data);
    cache->size -= 1;
}

static void drop_all_cb(lv_cache_t * cache, void * user_data)
{
    lv_cache_sc_da_t * da = (lv_cache_sc_da_t *)cache;

    LV_ASSERT_NULL(da);

    if(da == NULL) {
        return;
    }
    uint32_t used_cnt = 0;
    for(size_t i = 0; i < cache->size; ++i) {
        void * da_entry;
        lv_cache_entry_t * cache_entry;
        get_entry(da, i, &da_entry, &cache_entry);
        const int32_t refs = lv_cache_entry_get_ref(cache_entry);

        if(refs > 0) {
            LV_LOG_WARN(
                "entry (%zu) is still referenced (%" LV_PRId32
                ")",
                i, refs);
            used_cnt++;
            continue;
        }
        da->cache.ops.free_cb(da_entry, user_data);
    }
    if(used_cnt > 0) {
        LV_LOG_WARN("%" LV_PRId32 " entries are still referenced",
                    used_cnt);
    }

    cache->size = 0;
}

static lv_cache_entry_t * get_possible_victim(lv_cache_sc_da_t * da, size_t index)
{
    LV_ASSERT(index < da->cache.size);

    void * da_entry;
    lv_cache_entry_t * cache_entry;
    get_entry(da, index, &da_entry, &cache_entry);

    const uint8_t sec_chance = has_second_chance(cache_entry);
    const int32_t refs = lv_cache_entry_get_ref(cache_entry);

    if(sec_chance == 0 && refs == 0) {
        return cache_entry;
    }
    if(sec_chance == 0 && refs > 0) {
        LV_LOG_INFO(
            "Entry %zu should be evicted but it's still referenced %" LV_PRId32
            " times\n",
            index, refs);
        return NULL;
    }

    /*Remove its second chance*/
    set_second_chance(cache_entry, false);
    return NULL;
}

static lv_cache_entry_t * get_victim_cb(lv_cache_t * cache, void * user_data)
{
    LV_UNUSED(user_data);

    lv_cache_sc_da_t * da = (lv_cache_sc_da_t *)cache;

    LV_ASSERT_NULL(da);
    /*
         * We iterate twice to handle the complexity introduced by reference counting
         * in the second chance algorithm:
         *
         * First iteration: Clear second chance bits and look for victims (entries with
         * sec_chance=0 AND refs=0). Some entries may have sec_chance=0 but refs>0,
         * making them unavailable for eviction despite being marked for removal.
         *
         * Second iteration: Now that all second chance bits are cleared from the first
         * pass, we can find entries that are truly available for eviction (refs=0).
         * We can't assume the first entry will be the victim after the first round
         * because reference counts may prevent eviction of otherwise eligible entries.
         *
         * This ensures we give all entries a proper second chance while respecting
         * active references that prevent immediate eviction.
         */
    for(size_t i = 0; i < 2; ++i) {
        for(size_t j = 0; j < da->cache.size; ++j) {
            lv_cache_entry_t * victim = get_possible_victim(da, j);
            if(victim) {
                return victim;
            }
        }
    }
    return NULL;
}

static lv_cache_reserve_cond_res_t reserve_cond_cb(lv_cache_t * cache,
                                                   const void * key,
                                                   size_t reserved_size,
                                                   void * user_data)
{
    LV_UNUSED(user_data);
    LV_UNUSED(key);

    lv_cache_sc_da_t * da = (lv_cache_sc_da_t *)cache;

    LV_ASSERT_NULL(da);

    if(da == NULL) {
        return LV_CACHE_RESERVE_COND_ERROR;
    }

    return cache->size + reserved_size + 1 > da->cache.max_size ?
           LV_CACHE_RESERVE_COND_NEED_VICTIM :
           LV_CACHE_RESERVE_COND_OK;
}

static lv_iter_t * cache_iter_create_cb(lv_cache_t * cache)
{
    return lv_iter_create(cache, lv_cache_entry_get_size(cache->node_size),
                          0, cache_iter_next_cb);
}

static lv_result_t cache_iter_next_cb(void * instance, void * context, void * elem)
{
    lv_cache_sc_da_t * da = (lv_cache_sc_da_t *)instance;
    uint8_t ** da_entry = context;
    LV_ASSERT_NULL(da_entry);
    const size_t entry_size = lv_cache_entry_get_size(da->cache.node_size);

    if(*da_entry == NULL) {
        *da_entry = da->data;
    }
    else {
        *da_entry += entry_size;
    }

    if(*da_entry == NULL) {
        return LV_RESULT_INVALID;
    }

    lv_memcpy(elem, da_entry, entry_size);

    return LV_RESULT_OK;
}
