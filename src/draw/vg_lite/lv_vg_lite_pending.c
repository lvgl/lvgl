/**
 * @file lv_vg_lite_pending.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vg_lite_pending.h"

#if LV_USE_DRAW_VG_LITE

#include "../../misc/lv_assert.h"
#include "../../misc/lv_array.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_vg_lite_pending_t {
    lv_array_t * arr_act;
    lv_array_t arr_1;
    lv_array_t arr_2;
    lv_vg_lite_pending_free_cb_t free_cb;
    void * user_data;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static inline void lv_vg_lite_pending_array_clear(lv_vg_lite_pending_t * pending, lv_array_t * arr);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_vg_lite_pending_t * lv_vg_lite_pending_create(size_t obj_size, uint32_t capacity_default)
{
    lv_vg_lite_pending_t * pending = lv_malloc_zeroed(sizeof(lv_vg_lite_pending_t));
    LV_ASSERT_MALLOC(pending);
    lv_array_init(&pending->arr_1, capacity_default, obj_size);
    lv_array_init(&pending->arr_2, capacity_default, obj_size);
    pending->arr_act = &pending->arr_1;
    return pending;
}

void lv_vg_lite_pending_destroy(lv_vg_lite_pending_t * pending)
{
    LV_ASSERT_NULL(pending);
    lv_vg_lite_pending_remove_all(pending);
    lv_array_deinit(&pending->arr_1);
    lv_array_deinit(&pending->arr_2);
    lv_memzero(pending, sizeof(lv_vg_lite_pending_t));
    lv_free(pending);
}

void lv_vg_lite_pending_set_free_cb(lv_vg_lite_pending_t * pending, lv_vg_lite_pending_free_cb_t free_cb,
                                    void * user_data)
{
    LV_ASSERT_NULL(pending);
    LV_ASSERT_NULL(free_cb);
    pending->free_cb = free_cb;
    pending->user_data = user_data;
}

void lv_vg_lite_pending_add(lv_vg_lite_pending_t * pending, void * obj)
{
    LV_ASSERT_NULL(pending);
    LV_ASSERT_NULL(obj);
    lv_array_push_back(pending->arr_act, obj);
}

void lv_vg_lite_pending_remove_all(lv_vg_lite_pending_t * pending)
{
    LV_ASSERT_NULL(pending);

    lv_vg_lite_pending_array_clear(pending, &pending->arr_1);
    lv_vg_lite_pending_array_clear(pending, &pending->arr_2);
}

void lv_vg_lite_pending_swap(lv_vg_lite_pending_t * pending)
{
    pending->arr_act = (pending->arr_act == &pending->arr_1) ? &pending->arr_2 : &pending->arr_1;
    lv_vg_lite_pending_array_clear(pending, pending->arr_act);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void lv_vg_lite_pending_array_clear(lv_vg_lite_pending_t * pending, lv_array_t * arr)
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

#endif /*LV_USE_DRAW_VG_LITE*/
