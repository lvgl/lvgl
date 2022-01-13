/**
 * @file lv_fragment_manager.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_fragment.h"

#if LV_USE_FRAGMENT

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_fragment_manager_t {
    lv_fragment_t * parent;
    lv_fragment_managed_states_t * top;
};


/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_fragment_managed_states_t * item_new(const lv_fragment_class_t * cls, lv_fragment_manager_t * manager);

static void item_create_obj(lv_fragment_managed_states_t * item, const lv_obj_class_t * check_type);

static void item_del_obj(lv_fragment_managed_states_t * item);

static void item_del_fragment(lv_fragment_managed_states_t * item);

static lv_fragment_managed_states_t * fragment_attach(lv_fragment_manager_t * manager, lv_fragment_t * fragment,
                                                      lv_obj_t * const * container);

static void cb_states_obj_destroyed(lv_event_t * event);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_fragment_manager_t * lv_fragment_manager_create(lv_fragment_t * parent)
{
    lv_fragment_manager_t * instance = lv_mem_alloc(sizeof(lv_fragment_manager_t));
    lv_memset_00(instance, sizeof(lv_fragment_manager_t));
    instance->parent = parent;
    return instance;
}

void lv_fragment_manager_del(lv_fragment_manager_t * manager)
{
    LV_ASSERT_NULL(manager);
    lv_fragment_managed_states_t * top = manager->top;
    while(top) {
        LV_ASSERT(top->cls);
        item_del_obj(top);
        item_del_fragment(top);
        lv_fragment_managed_states_t * prev = top->prev;
        lv_mem_free(top);
        top = prev;
    }
    lv_mem_free(manager);
}

void lv_fragment_manager_add(lv_fragment_manager_t * manager, lv_fragment_t * fragment, lv_obj_t * const * container)
{
    lv_fragment_managed_states_t * item = fragment_attach(manager, fragment, container);
    item_create_obj(item, NULL);
}

void lv_fragment_manager_remove(lv_fragment_manager_t * manager, lv_fragment_t * fragment)
{
    LV_ASSERT(manager);
    LV_ASSERT(fragment);
    LV_ASSERT_NULL(fragment->managed);
    LV_ASSERT(fragment->managed->manager == manager);
    lv_fragment_managed_states_t * states = fragment->managed;
    lv_fragment_managed_states_t * prev = NULL;
    if(states->in_stack) {
        lv_fragment_managed_states_t * next;
        for(next = manager->top; next; next = next->prev) {
            if(next->prev == states) {
                break;
            }
        }
        prev = states->prev;
        if(next) {
            next->prev = prev;
        }
        else {
            manager->top = prev;
        }
    }
    item_del_obj(states);
    item_del_fragment(states);
    lv_mem_free(states);
    if(prev) {
        item_create_obj(prev, NULL);
    }
}

void lv_fragment_manager_push(lv_fragment_manager_t * manager, lv_fragment_t * fragment, lv_obj_t * const * container)
{
    lv_fragment_managed_states_t * top = manager->top;
    if(top != NULL) {
        item_del_obj(top);
    }
    lv_fragment_managed_states_t * states = fragment_attach(manager, fragment, container);
    states->prev = top;
    states->in_stack = true;
    manager->top = states;
    item_create_obj(states, NULL);
}

bool lv_fragment_manager_pop(lv_fragment_manager_t * manager)
{
    lv_fragment_t * top = lv_fragment_manager_get_top(manager);
    if(top == NULL) return false;
    lv_fragment_manager_remove(manager, top);
    return true;
}

void lv_fragment_manager_replace(lv_fragment_manager_t * manager, lv_fragment_t * fragment,
                                 lv_obj_t * const * container)
{
    lv_fragment_t * top = lv_fragment_manager_get_top(manager);
    if(top != NULL) {
        lv_fragment_manager_remove(manager, top);
    }
    lv_fragment_manager_push(manager, fragment, container);
}

void lv_fragment_manager_recreate_obj(lv_fragment_manager_t * manager, lv_fragment_t * fragment)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(fragment);
    LV_ASSERT_NULL(fragment->managed);
    LV_ASSERT(fragment->managed->manager == manager);
    item_del_obj(fragment->managed);
    item_create_obj(fragment->managed, NULL);
}

bool lv_fragment_manager_dispatch_event(lv_fragment_manager_t * manager, int code, void * userdata)
{
    LV_ASSERT(manager);
    lv_fragment_managed_states_t * top = manager->top;
    if(!top) return false;
    lv_fragment_t * instance = top->instance;
    if(!instance) return false;
    if(lv_fragment_manager_dispatch_event(instance->child_manager, code, userdata)) return true;
    if(!top->cls->event_cb) return false;
    return top->cls->event_cb(instance, code, userdata);
}

size_t lv_fragment_manager_get_size(lv_fragment_manager_t * manager)
{
    size_t size = 0;
    lv_fragment_managed_states_t * cur;
    for(cur = manager->top; cur; cur = cur->prev) {
        size++;
    }
    return size;
}

lv_fragment_t * lv_fragment_manager_get_top(lv_fragment_manager_t * manager)
{
    LV_ASSERT(manager);
    lv_fragment_managed_states_t * top = manager->top;
    if(!top)return NULL;
    return top->instance;
}

lv_fragment_t * lv_fragment_manager_get_parent_fragment(lv_fragment_manager_t * manager)
{
    LV_ASSERT(manager);
    return manager->parent;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_fragment_managed_states_t * item_new(const lv_fragment_class_t * cls, lv_fragment_manager_t * manager)
{
    LV_ASSERT(cls->instance_size);
    lv_fragment_managed_states_t * item = lv_mem_alloc(sizeof(lv_fragment_managed_states_t));
    lv_memset_00(item, sizeof(lv_fragment_managed_states_t));
    item->cls = cls;
    item->manager = manager;
    return item;
}

static void item_create_obj(lv_fragment_managed_states_t * item, const lv_obj_class_t * check_type)
{
    LV_ASSERT(item->instance);
    item->destroying_obj = false;
    lv_fragment_create_obj(item->instance, item->container ? *item->container : NULL);
    if(check_type) {
        LV_ASSERT(lv_obj_has_class(item->instance->obj, check_type));
    }
    item->obj_created = true;
    if(item->instance->obj) {
        lv_obj_add_event_cb(item->instance->obj, cb_states_obj_destroyed, LV_EVENT_DELETE, item);
    }
    else if(*item->container) {
        lv_obj_add_event_cb(*item->container, cb_states_obj_destroyed, LV_EVENT_DELETE, item);
    }
}

static void item_del_obj(lv_fragment_managed_states_t * item)
{
    if(!item->obj_created) return;
    item->destroying_obj = true;
    lv_fragment_t * fragment = item->instance;
    if(fragment->obj) {
        lv_fragment_del_obj(fragment);
    }
    else {
        LV_ASSERT(item->container && *item->container);
        lv_obj_remove_event_cb_with_user_data(*item->container, cb_states_obj_destroyed, item);
        bool delete_handled = false;
        const lv_fragment_class_t * cls = fragment->cls;
        if(cls->obj_will_delete_cb) {
            delete_handled = cls->obj_will_delete_cb(fragment, NULL);
        }
        if(!delete_handled) {
            lv_obj_clean(*item->container);
        }
        if(cls->obj_deleted_cb) {
            cls->obj_deleted_cb(fragment, NULL);
        }
    }
    item->obj_created = false;
}

/**
 * Detach, then destroy fragment
 * @param item fragment states
 */
static void item_del_fragment(lv_fragment_managed_states_t * item)
{
    lv_fragment_t * instance = item->instance;
    if(instance->cls->detached_cb) {
        instance->cls->detached_cb(instance);
    }
    instance->managed = NULL;
    lv_fragment_del(instance);
    item->instance = NULL;
}


static lv_fragment_managed_states_t * fragment_attach(lv_fragment_manager_t * manager, lv_fragment_t * fragment,
                                                      lv_obj_t * const * container)
{
    LV_ASSERT(manager);
    LV_ASSERT(fragment);
    LV_ASSERT(fragment->managed == NULL);
    lv_fragment_managed_states_t * item = item_new(fragment->cls, manager);
    item->container = container;
    item->instance = fragment;
    fragment->managed = item;
    if(fragment->cls->attached_cb) {
        fragment->cls->attached_cb(fragment);
    }
    return item;
}

static void cb_states_obj_destroyed(lv_event_t * event)
{
    lv_fragment_managed_states_t * states = lv_event_get_user_data(event);
    states->obj_created = false;
}

#endif /*LV_USE_FRAGMENT*/