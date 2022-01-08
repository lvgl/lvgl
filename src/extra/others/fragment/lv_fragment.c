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
    lv_mem_free(fragment);
}

void lv_fragment_create_obj(lv_fragment_t * fragment, lv_obj_t * container)
{
    const lv_fragment_class_t * cls = fragment->cls;
    lv_obj_t * obj = cls->create_obj_cb(fragment, container);
    fragment->obj = obj;
    if(cls->obj_created_cb) {
        cls->obj_created_cb(fragment, obj);
    }
}

void lv_fragment_del_obj(lv_fragment_t * fragment)
{
    LV_ASSERT(fragment);
    const lv_fragment_class_t * cls = fragment->cls;
    bool del_handled = false;
    if(cls->obj_will_delete_cb) {
        del_handled = cls->obj_will_delete_cb(fragment, fragment->obj);
    }
    LV_ASSERT_MSG(del_handled || fragment->obj, "Either return created object, or implement obj_will_delete_cb.");
    if(fragment->obj) {
        lv_obj_del(fragment->obj);
    }
    if(cls->obj_deleted_cb) {
        cls->obj_deleted_cb(fragment, fragment->obj);
    }
}

#endif /*LV_USE_FRAGMENT*/
