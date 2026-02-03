/**
 * @file lv_pending.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_pending.h"
#include "lv_array.h"
#include "lv_assert.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_pending_t {
    lv_array_t * arr_act;
    lv_array_t arr_1;
    lv_array_t arr_2;
    lv_pending_free_cb_t free_cb;
    void * user_data;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static inline void lv_pending_array_clear(lv_pending_t * pending, lv_array_t * arr);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_pending_t * lv_pending_create(size_t obj_size, uint32_t capacity_default)
{
    lv_pending_t * pending = lv_malloc_zeroed(sizeof(lv_pending_t));
    LV_ASSERT_MALLOC(pending);
    lv_array_init(&pending->arr_1, capacity_default, obj_size);
    lv_array_init(&pending->arr_2, capacity_default, obj_size);
    pending->arr_act = &pending->arr_1;
    return pending;
}

void lv_pending_destroy(lv_pending_t * pending)
{
    LV_ASSERT_NULL(pending);
    lv_pending_remove_all(pending);
    lv_array_deinit(&pending->arr_1);
    lv_array_deinit(&pending->arr_2);
    lv_memzero(pending, sizeof(lv_pending_t));
    lv_free(pending);
}

void lv_pending_set_free_cb(lv_pending_t * pending, lv_pending_free_cb_t free_cb,
                            void * user_data)
{
    LV_ASSERT_NULL(pending);
    LV_ASSERT_NULL(free_cb);
    pending->free_cb = free_cb;
    pending->user_data = user_data;
}

void lv_pending_add(lv_pending_t * pending, void * obj)
{
    LV_ASSERT_NULL(pending);
    LV_ASSERT_NULL(obj);
    lv_array_push_back(pending->arr_act, obj);
}

void lv_pending_remove_all(lv_pending_t * pending)
{
    LV_ASSERT_NULL(pending);

    lv_pending_array_clear(pending, &pending->arr_1);
    lv_pending_array_clear(pending, &pending->arr_2);
}

void lv_pending_swap(lv_pending_t * pending)
{
    pending->arr_act = (pending->arr_act == &pending->arr_1) ? &pending->arr_2 : &pending->arr_1;
    lv_pending_array_clear(pending, pending->arr_act);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void lv_pending_array_clear(lv_pending_t * pending, lv_array_t * arr)
{
    LV_ASSERT_NULL(pending->free_cb);

    uint32_t size = lv_array_size(arr);
    if(size == 0) {
        return;
    }

    /* remove all the pending objects */
    for(uint32_t i = 0; i < size; i++) {
        pending->free_cb(lv_array_at(arr, i), pending->user_data);
    }

    lv_array_clear(arr);
}
