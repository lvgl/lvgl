/**
 * @file lv_lru.c
 *
 * @see https://github.com/willcannings/C-LRU-Cache
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_lru.h"
#include "lv_math.h"
#include "lv_mem.h"
#include "lv_assert.h"
#include "lv_log.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_lru_item_t {
    void * value;
    void * key;
    size_t value_length;
    size_t key_length;
    uint64_t access_count;
    struct _lv_lru_item_t * next;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * MurmurHash2
 * @author Austin Appleby
 * @see http://sites.google.com/site/murmurhash/
 */
static uint32_t lv_lru_hash(lv_lru_t * cache, const void * key, uint32_t key_length);

/** compare a key against an existing item's key */
static int lv_lru_cmp_keys(lv_lru_item_t * item, const void * key, uint32_t key_length);

/** remove an item and push it to the free items queue */
static void lv_lru_remove_item(lv_lru_t * cache, lv_lru_item_t * prev, lv_lru_item_t * item, uint32_t hash_index);

/** pop an existing item off the free queue, or create a new one */
static lv_lru_item_t * lv_lru_pop_or_create_item(lv_lru_t * cache);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/* error helpers */
#define error_for(conditions, error)  if(conditions) {return error;}
#define test_for_missing_cache()      error_for(!cache, LV_LRU_MISSING_CACHE)
#define test_for_missing_key()        error_for(!key, LV_LRU_MISSING_KEY)
#define test_for_missing_value()      error_for(!value || value_length == 0, LV_LRU_MISSING_VALUE)
#define test_for_value_too_large()    error_for(value_length > cache->total_memory, LV_LRU_VALUE_TOO_LARGE)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_lru_t * lv_lru_create(size_t cache_size, size_t average_length, lv_lru_free_t * value_free,
                         lv_lru_free_t * key_free)
{
    // create the cache
    lv_lru_t * cache = (lv_lru_t *) lv_malloc(sizeof(lv_lru_t));
    lv_memzero(cache, sizeof(lv_lru_t));
    if(!cache) {
        LV_LOG_WARN("LRU Cache unable to create cache object");
        return NULL;
    }
    cache->hash_table_size = cache_size / average_length;
    cache->average_item_length = average_length;
    cache->free_memory = cache_size;
    cache->total_memory = cache_size;
    cache->seed = lv_rand(1, UINT32_MAX);
    cache->value_free = value_free ? value_free : lv_free;
    cache->key_free = key_free ? key_free : lv_free;

    // size the hash table to a guestimate of the number of slots required (assuming a perfect hash)
    cache->items = (lv_lru_item_t **) lv_malloc(sizeof(lv_lru_item_t *) * cache->hash_table_size);
    lv_memzero(cache->items, sizeof(lv_lru_item_t *) * cache->hash_table_size);
    if(!cache->items) {
        LV_LOG_WARN("LRU Cache unable to create cache hash table");
        lv_free(cache);
        return NULL;
    }
    return cache;
}


void lv_lru_del(lv_lru_t * cache)
{
    LV_ASSERT_NULL(cache);

    // free each of the cached items, and the hash table
    lv_lru_item_t * item = NULL, *next = NULL;
    uint32_t i = 0;
    if(cache->items) {
        for(; i < cache->hash_table_size; i++) {
            item = cache->items[i];
            while(item) {
                next = (lv_lru_item_t *) item->next;
                cache->value_free(item->value);
                cache->key_free(item->key);
                cache->free_memory += item->value_length;
                lv_free(item);
                item = next;
            }
        }
        lv_free(cache->items);
    }

    if(cache->free_items) {
        item = cache->free_items;
        while(item) {
            next = (lv_lru_item_t *) item->next;
            lv_free(item);
            item = next;
        }
    }

    // free the cache
    lv_free(cache);
}


lv_lru_res_t lv_lru_set(lv_lru_t * cache, const void * key, size_t key_length, void * value, size_t value_length)
{
    test_for_missing_cache();
    test_for_missing_key();
    test_for_missing_value();
    test_for_value_too_large();

    // see if the key already exists
    uint32_t hash_index = lv_lru_hash(cache, key, key_length);
    int required = 0;
    lv_lru_item_t * item = NULL, *prev = NULL;
    item = cache->items[hash_index];

    while(item && lv_lru_cmp_keys(item, key, key_length)) {
        prev = item;
        item = (lv_lru_item_t *) item->next;
    }

    if(item) {
        // update the value and value_lengths
        required = (int)(value_length - item->value_length);
        cache->value_free(item->value);
        item->value = value;
        item->value_length = value_length;

    }
    else {
        // insert a new item
        item = lv_lru_pop_or_create_item(cache);
        item->value = value;
        item->key = lv_malloc(key_length);
        memcpy(item->key, key, key_length);
        item->value_length = value_length;
        item->key_length = key_length;
        required = (int) value_length;

        if(prev)
            prev->next = item;
        else
            cache->items[hash_index] = item;
    }
    item->access_count = ++cache->access_count;

    // remove as many items as necessary to free enough space
    if(required > 0 && (size_t) required > cache->free_memory) {
        while(cache->free_memory < (size_t) required)
            lv_lru_remove_lru_item(cache);
    }
    cache->free_memory -= required;
    return LV_LRU_OK;
}


lv_lru_res_t lv_lru_get(lv_lru_t * cache, const void * key, size_t key_size, void ** value)
{
    test_for_missing_cache();
    test_for_missing_key();

    // loop until we find the item, or hit the end of a chain
    uint32_t hash_index = lv_lru_hash(cache, key, key_size);
    lv_lru_item_t * item = cache->items[hash_index];

    while(item && lv_lru_cmp_keys(item, key, key_size))
        item = (lv_lru_item_t *) item->next;

    if(item) {
        *value = item->value;
        item->access_count = ++cache->access_count;
    }
    else {
        *value = NULL;
    }

    return LV_LRU_OK;
}

lv_lru_res_t lv_lru_remove(lv_lru_t * cache, const void * key, size_t key_size)
{
    test_for_missing_cache();
    test_for_missing_key();

    // loop until we find the item, or hit the end of a chain
    lv_lru_item_t * item = NULL, *prev = NULL;
    uint32_t hash_index = lv_lru_hash(cache, key, key_size);
    item = cache->items[hash_index];

    while(item && lv_lru_cmp_keys(item, key, key_size)) {
        prev = item;
        item = (lv_lru_item_t *) item->next;
    }

    if(item) {
        lv_lru_remove_item(cache, prev, item, hash_index);
    }

    return LV_LRU_OK;
}

void lv_lru_remove_lru_item(lv_lru_t * cache)
{
    lv_lru_item_t * min_item = NULL, *min_prev = NULL;
    lv_lru_item_t * item = NULL, *prev = NULL;
    uint32_t i = 0, min_index = -1;
    uint64_t min_access_count = -1;

    for(; i < cache->hash_table_size; i++) {
        item = cache->items[i];
        prev = NULL;

        while(item) {
            if(item->access_count < min_access_count || (int64_t) min_access_count == -1) {
                min_access_count = item->access_count;
                min_item = item;
                min_prev = prev;
                min_index = i;
            }
            prev = item;
            item = item->next;
        }
    }

    if(min_item) {
        lv_lru_remove_item(cache, min_prev, min_item, min_index);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint32_t lv_lru_hash(lv_lru_t * cache, const void * key, uint32_t key_length)
{
    uint32_t m = 0x5bd1e995;
    uint32_t r = 24;
    uint32_t h = cache->seed ^ key_length;
    char * data = (char *) key;

    while(key_length >= 4) {
        uint32_t k = *(uint32_t *) data;
        k *= m;
        k ^= k >> r;
        k *= m;
        h *= m;
        h ^= k;
        data += 4;
        key_length -= 4;
    }

    if(key_length >= 3) {
        h ^= data[2] << 16;
    }
    if(key_length >= 2) {
        h ^= data[1] << 8;
    }
    if(key_length >= 1) {
        h ^= data[0];
        h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h % cache->hash_table_size;
}

static int lv_lru_cmp_keys(lv_lru_item_t * item, const void * key, uint32_t key_length)
{
    if(key_length != item->key_length) {
        return 1;
    }
    else {
        return memcmp(key, item->key, key_length);
    }
}

static void lv_lru_remove_item(lv_lru_t * cache, lv_lru_item_t * prev, lv_lru_item_t * item, uint32_t hash_index)
{
    if(prev) {
        prev->next = item->next;
    }
    else {
        cache->items[hash_index] = (lv_lru_item_t *) item->next;
    }

    // free memory and update the free memory counter
    cache->free_memory += item->value_length;
    cache->value_free(item->value);
    cache->key_free(item->key);

    // push the item to the free items queue
    lv_memzero(item, sizeof(lv_lru_item_t));
    item->next = cache->free_items;
    cache->free_items = item;
}

static lv_lru_item_t * lv_lru_pop_or_create_item(lv_lru_t * cache)
{
    lv_lru_item_t * item = NULL;

    if(cache->free_items) {
        item = cache->free_items;
        cache->free_items = item->next;
        lv_memzero(item, sizeof(lv_lru_item_t));
    }
    else {
        item = (lv_lru_item_t *) lv_malloc(sizeof(lv_lru_item_t));
        lv_memzero(item, sizeof(lv_lru_item_t));
    }

    return item;
}
