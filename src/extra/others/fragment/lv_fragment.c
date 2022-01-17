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
 *  STATIC PROTOTYPES
 **********************/

static void cb_delete_assertion(lv_event_t * event);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_fragment_t * lv_fragment_create(const lv_fragment_class_t * cls, void * args)
{
    LV_ASSERT_NULL(cls);
    LV_ASSERT_NULL(cls->create_obj_cb);
    LV_ASSERT(cls->instance_size > 0);
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
    LV_ASSERT_NULL(fragment);
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
    lv_fragment_managed_states_t * states = fragment->managed;
    if(states) {
        states->destroying_obj = false;
    }
    const lv_fragment_class_t * cls = fragment->cls;
    lv_obj_t * obj = cls->create_obj_cb(fragment, container);
    LV_ASSERT_NULL(obj);
    lv_obj_add_event_cb(obj, cb_delete_assertion, LV_EVENT_DELETE, NULL);
    fragment->obj = obj;
    lv_fragment_manager_create_obj(fragment->child_manager);
    if(states) {
        states->obj_created = true;
    }
    if(cls->obj_created_cb) {
        cls->obj_created_cb(fragment, obj);
    }
    return obj;
}

void lv_fragment_del_obj(lv_fragment_t * fragment)
{
    LV_ASSERT_NULL(fragment);
    lv_fragment_manager_del_obj(fragment->child_manager);
    lv_fragment_managed_states_t * states = fragment->managed;
    if(states) {
        if(!states->obj_created) return;
        states->destroying_obj = true;
    }
    LV_ASSERT_NULL(fragment->obj);
    const lv_fragment_class_t * cls = fragment->cls;
    bool cb_removed = lv_obj_remove_event_cb(fragment->obj, cb_delete_assertion);
    LV_ASSERT(cb_removed);
    if(cls->obj_will_delete_cb) {
        cls->obj_will_delete_cb(fragment, fragment->obj);
    }
    lv_obj_del(fragment->obj);
    if(cls->obj_deleted_cb) {
        cls->obj_deleted_cb(fragment, fragment->obj);
    }
    if(states) {
        states->obj_created = false;
    }
    fragment->obj = NULL;
}

void lv_fragment_recreate_obj(lv_fragment_t * fragment)
{
    LV_ASSERT_NULL(fragment);
    LV_ASSERT_NULL(fragment->managed);
    lv_fragment_manager_recreate_obj(fragment->managed->manager, fragment);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void cb_delete_assertion(lv_event_t * event)
{
    LV_UNUSED(event);
    LV_ASSERT_MSG(0, "Please delete objects with lv_fragment_destroy_obj");
}

#endif /*LV_USE_FRAGMENT*/
