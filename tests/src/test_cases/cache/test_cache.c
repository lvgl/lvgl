#if LV_BUILD_TEST

#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "lv_test_helpers.h"

#include "unity/unity.h"

static uint32_t MEM_SIZE = 0;

// Cache size in bytes
#define CACHE_SIZE_BYTES 1000

lv_cache_t * cache;

typedef struct _test_data {
    lv_cache_slot_size_t slot;

    int32_t key1;
    int32_t key2;

    void * data; // malloced data
} test_data;

static lv_cache_compare_res_t compare_cb(const test_data * lhs, const test_data * rhs)
{
    if(lhs->key1 != rhs->key1) {
        return lhs->key1 > rhs->key1 ? 1 : -1;
    }
    if(lhs->key2 != rhs->key2) {
        return lhs->key2 > rhs->key2 ? 1 : -1;
    }
    return 0;
}

static void free_cb(test_data * node, void * user_data)
{
    LV_UNUSED(user_data);
    lv_free(node->data);
}

void setUp(void)
{
    /* Function run before every test */
    MEM_SIZE = lv_test_get_free_mem();

    lv_cache_ops_t ops = {
        .compare_cb = (lv_cache_compare_cb_t) compare_cb,
        .create_cb = NULL,
        .free_cb = (lv_cache_free_cb_t)free_cb,
    };
    cache = lv_cache_create(&lv_cache_class_lru_rb_size, sizeof(test_data), CACHE_SIZE_BYTES, ops);
}

void tearDown(void)
{
    /* Function run after every test */
    lv_cache_destroy(cache, NULL);
    cache = NULL;

    TEST_ASSERT_MEM_LEAK_LESS_THAN(MEM_SIZE, 32);
}

void test_cache_1(void)
{

    void * record_data_ptr = NULL;

    // create many node unless cache is full
    uint32_t curr_mem_size = 8;
    uint32_t curr_total_mem_size = 0;
    while(curr_total_mem_size < CACHE_SIZE_BYTES) {
        test_data search_key = {
            .slot.size = curr_mem_size,

            .key1 = (int32_t)curr_mem_size,
            .key2 = (int32_t)curr_mem_size + 1
        };

        // acquire cache first
        lv_cache_entry_t * entry = lv_cache_acquire(cache, &search_key, NULL);
        if(entry != NULL) {
            continue;
        }

        // if cache miss then add cache
        entry = lv_cache_add(cache, &search_key, NULL);
        TEST_ASSERT_NOT_NULL(entry);

        test_data * data = lv_cache_entry_get_data(entry);
        TEST_ASSERT_NOT_NULL(data);

        data->data = lv_malloc(data->slot.size);

        // record data ptr when {key1 = 32, key2 = 33}.
        if(search_key.key1 == 32 && search_key.key2 == 33) {
            record_data_ptr = data->data;
        }

        lv_cache_release(cache, entry, NULL);

        curr_total_mem_size += curr_mem_size;
        curr_mem_size *= 2;

        TEST_PRINTF("cache free: %d, allocated: %d", lv_cache_get_free_size(cache, NULL), curr_mem_size);
    }

    /*
     * allocated = 8 + 16 + 32 + 64 + 128 + 256 - 8 - 16 + 512 = 992
     * free = 1000 - 992 = 8
     * The last node size should be 1024, but the cache's max size is 1000. So new entry will be allocated failed and
     * the loop will break.
     * */
    TEST_ASSERT_EQUAL(8, lv_cache_get_free_size(cache, NULL));

    /*
     * Search entry {key1 = 32, key2 = 33}
     */
    test_data search_key32 = {
        .key1 = 32,
        .key2 = 33
    };
    lv_cache_entry_t * entry_key32 = lv_cache_acquire(cache, &search_key32, NULL);

    test_data * cached_data_key32 = lv_cache_entry_get_data(entry_key32);
    TEST_ASSERT_EQUAL(record_data_ptr, cached_data_key32->data);

    /*
     * Now drop the cache {key1 = 32, key2 = 33}. However, this entry is acquired once without release, so `drop`
     * will not release the memory allocated by this entry.
     */
    size_t mem_curr_free = lv_test_get_free_mem();
    lv_cache_drop(cache, &search_key32, NULL);
    /*
     * Though it doesn't release the data, the entry and other structure has been freed.
     * lv_rb_note_t (4 ptr + 1 int32 may align to 8 bit on 64 bit machine) + lv_ll (2 ptr + node_size).
     * Also, the def heap has some other aligned attributes. It'll also affect the final result.
     */
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_curr_free,
                                   sizeof(lv_rb_node_t)
                                   + sizeof(void *) + (sizeof(lv_ll_node_t *) + sizeof(lv_ll_node_t *))
                                   + 32); // the last 32 is an error in memory allocating
    mem_curr_free = lv_test_get_free_mem();
    lv_cache_release(cache, entry_key32, NULL);
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_curr_free,
                                   lv_cache_entry_get_size(sizeof(test_data)) + sizeof(void *)
                                   + 32
                                   + 32);

    // Now the freed cache size should be 8 + 32 = 40
    TEST_ASSERT_EQUAL(40, lv_cache_get_free_size(cache, NULL));
}

#endif
