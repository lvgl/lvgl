/**
 * @file lv_vg_lite_pending.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vg_lite_pending.h"

#if LV_USE_DRAW_VG_LITE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_vg_lite_pending_t {
    lv_array_t objs;
    lv_vg_lite_pending_free_cb_t free_cb;
    void * user_data;
};

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

lv_vg_lite_pending_t * lv_vg_lite_pending_create(size_t obj_size, uint32_t capacity_default)
{
    lv_vg_lite_pending_t * pending = lv_malloc_zeroed(sizeof(lv_vg_lite_pending_t));
    LV_ASSERT_MALLOC(pending);
    lv_array_init(&pending->objs, capacity_default, obj_size);
    return pending;
}

void lv_vg_lite_pending_destroy(lv_vg_lite_pending_t * pending)
{
    LV_ASSERT_NULL(pending);
    lv_vg_lite_pending_remove_all(pending);
    lv_array_deinit(&pending->objs);
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
    lv_array_push_back(&pending->objs, obj);
}

void lv_vg_lite_pending_remove_all(lv_vg_lite_pending_t * pending)
{
    LV_ASSERT_NULL(pending);
    LV_ASSERT_NULL(pending->free_cb);

    uint32_t size = lv_array_size(&pending->objs);
    if(size == 0) {
        return;
    }

    /* remove all the pending objects */
    for(uint32_t i = 0; i < size; i++) {
        pending->free_cb(lv_array_at(&pending->objs, i), pending->user_data);
    }

    lv_array_clear(&pending->objs);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/
