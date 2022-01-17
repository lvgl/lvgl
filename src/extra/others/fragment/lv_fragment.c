/**
 * @file lv_fragment.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_fragment.h"

#if LV_USE_FRAGMENT

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_fragment_t * lv_fragment_create(const lv_fragment_class_t * cls, void * args)
{
    LV_ASSERT_NULL(cls);
    LV_ASSERT(cls->instance_size);
    LV_ASSERT_NULL(cls->create_obj_cb);
    lv_fragment_t * instance = lv_mem_alloc(cls->instance_size);
    lv_memset_00(instance, cls->instance_size);
    instance->cls = cls;
    instance->child_manager = lv_fragment_manager_create(instance);
    if(cls->constructor_cb) {
        cls->constructor_cb(instance, args);
    }
    return instance;
}

void lv_fragment_del(lv_fragment_t * fragment)
{
    LV_ASSERT(fragment);
    /* Objects will leak if this function called before objects deleted */
    const lv_fragment_class_t * cls = fragment->cls;
    if(cls->destructor_cb) {
        cls->destructor_cb(fragment);
    }
    lv_fragment_manager_del(fragment->child_manager);
    lv_mem_free(fragment);
}

void lv_fragment_remove_self(lv_fragment_t * fragment)
{
    LV_ASSERT_NULL(fragment);
    LV_ASSERT_NULL(fragment->managed);
    lv_fragment_manager_remove(fragment->managed->manager, fragment);
}

lv_obj_t * const * lv_fragment_get_container(lv_fragment_t * fragment)
{
    LV_ASSERT_NULL(fragment);
    LV_ASSERT_NULL(fragment->managed);
    return fragment->managed->container;
}

lv_fragment_t * lv_fragment_get_parent(lv_fragment_t * fragment)
{
    LV_ASSERT_NULL(fragment);
    LV_ASSERT_NULL(fragment->managed);
    return lv_fragment_manager_get_parent_fragment(fragment->managed->manager);
}

lv_obj_t * lv_fragment_create_obj(lv_fragment_t * fragment, lv_obj_t * container)
{
    const lv_fragment_class_t * cls = fragment->cls;
    lv_obj_t * obj = cls->create_obj_cb(fragment, container);
    fragment->obj = obj;
    if(fragment->managed) {
        fragment->managed->obj_created = true;
    }
    if(cls->obj_created_cb) {
        cls->obj_created_cb(fragment, obj);
    }
    return obj;
}

void lv_fragment_del_obj(lv_fragment_t * fragment)
{
    LV_ASSERT(fragment);
    const lv_fragment_class_t * cls = fragment->cls;
    bool del_handled = false;
    if(cls->obj_will_delete_cb) {
        del_handled = cls->obj_will_delete_cb(fragment, fragment->obj);
    }
    if(!del_handled && fragment->obj != NULL) {
        lv_obj_del(fragment->obj);
    }
    if(cls->obj_deleted_cb) {
        cls->obj_deleted_cb(fragment, fragment->obj);
    }
    fragment->obj = NULL;
}

void lv_fragment_recreate_obj(lv_fragment_t * fragment)
{
    LV_ASSERT_NULL(fragment);
    LV_ASSERT_NULL(fragment->managed);
    lv_fragment_manager_recreate_obj(fragment->managed->manager, fragment);
}

#endif /*LV_USE_FRAGMENT*/
