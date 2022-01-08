/**
 * @file lv_fragment_manager.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_fragment.h"

#if LV_USE_FRAGMENT

#include "../../../extra/widgets/msgbox/lv_msgbox.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct internal_states_t {
    const lv_fragment_class_t * cls;
    lv_fragment_t * instance;
    bool obj_created;
    bool destroying_obj;
    bool dialog;
    bool in_stack;
    struct internal_states_t * prev;
};

struct _lv_fragment_manager_t {
    lv_obj_t * container;
    lv_fragment_t * parent;
    internal_states_t * top;
};


/**********************
 *  STATIC PROTOTYPES
 **********************/

static internal_states_t * item_new(const lv_fragment_class_t * cls);

static void item_create_obj(internal_states_t * item, lv_obj_t * parent, const lv_obj_class_t * check_type);

static void item_del_obj(internal_states_t * item);

static void item_del_fragment(internal_states_t * item);

#if LV_USE_MSGBOX

    static void cb_msgbox_delete(lv_event_t * event);

#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_fragment_manager_t * lv_fragment_manager_create(lv_obj_t * container, lv_fragment_t * parent)
{
    LV_ASSERT(container);
    lv_fragment_manager_t * instance = lv_mem_alloc(sizeof(lv_fragment_manager_t));
    instance->parent = parent;
    instance->container = container;
    instance->top = NULL;
    return instance;
}

void lv_fragment_manager_del(lv_fragment_manager_t * manager)
{
    LV_ASSERT_NULL(manager);
    internal_states_t * top = manager->top;
    while(top) {
        LV_ASSERT(top->cls);
        item_del_obj(top);
        item_del_fragment(top);
        struct internal_states_t * prev = top->prev;
        lv_mem_free(top);
        top = prev;
    }
    lv_mem_free(manager);
}

void lv_fragment_manager_add(lv_fragment_manager_t * manager, lv_fragment_t * fragment)
{
    LV_ASSERT(manager);
    LV_ASSERT(fragment);
    LV_ASSERT(fragment->_states == NULL);
    LV_ASSERT(fragment->manager == NULL);
    internal_states_t * item = item_new(fragment->cls);
    lv_obj_t * parent = manager->container;
    item->instance = fragment;
    fragment->manager = manager;
    fragment->_states = item;
    if(fragment->cls->attached_cb) {
        fragment->cls->attached_cb(fragment);
    }
    /* Destroy object of previous screen */
    if(manager->top) {
        item_del_obj(manager->top);
    }
    item_create_obj(item, parent, NULL);
}

void lv_fragment_manager_remove(lv_fragment_manager_t * manager, lv_fragment_t * fragment)
{
    LV_ASSERT(manager);
    LV_ASSERT(fragment);
    LV_ASSERT(fragment->manager == manager);
    LV_ASSERT_NULL(fragment->_states);
    internal_states_t * states = fragment->_states;
    internal_states_t * prev = NULL;
    if(states->in_stack) {
        internal_states_t * next;
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
            /*Removing top states*/
            manager->top = prev;
        }
    }
    item_del_obj(states);
    item_del_fragment(states);
    lv_mem_free(states);
    if(prev) {
        item_create_obj(prev, manager->container, NULL);
    }
}

void lv_fragment_manager_push(lv_fragment_manager_t * manager, lv_fragment_t * fragment)
{
    lv_fragment_manager_add(manager, fragment);
    internal_states_t * states = fragment->_states;
    internal_states_t * top = manager->top;
    states->prev = top;
    states->in_stack = true;
    manager->top = states;
}

void lv_fragment_manager_pop(lv_fragment_manager_t * manager)
{
    lv_fragment_manager_remove(manager, lv_fragment_manager_get_top(manager));
}

void lv_fragment_manager_replace(lv_fragment_manager_t * manager, lv_fragment_t * fragment)
{
    lv_fragment_manager_pop(manager);
    lv_fragment_manager_push(manager, fragment);
}

size_t lv_fragment_manager_get_size(lv_fragment_manager_t * manager)
{
    size_t size = 0;
    internal_states_t * cur;
    for(cur = manager->top; cur; cur = cur->prev) {
        size++;
    }
    return size;
}

bool lv_fragment_manager_dispatch_event(lv_fragment_manager_t * manager, int which, void * data1, void * data2)
{
    LV_ASSERT(manager);
    internal_states_t * top = manager->top;
    if(!top) return false;
    lv_fragment_t * instance = top->instance;
    if(!instance || !top->cls->event_cb) return false;
    return top->cls->event_cb(instance, which, data1, data2);
}

#if LV_USE_MSGBOX

void lv_fragment_manager_show(lv_fragment_manager_t * manager, lv_fragment_t * fragment)
{
    LV_ASSERT(manager);
    LV_ASSERT(fragment);
    const lv_fragment_class_t * cls = fragment->cls;
    internal_states_t * item = item_new(cls);
    item_create_obj(item, NULL, &lv_msgbox_class);
    item->dialog = true;
    internal_states_t * top = manager->top;
    item->prev = top;
    manager->top = item;
}

#endif

lv_fragment_t * lv_fragment_manager_get_top(lv_fragment_manager_t * manager)
{
    LV_ASSERT(manager);
    internal_states_t * top = manager->top;
    if(!top)return NULL;
    return top->instance;
}

lv_fragment_t * lv_fragment_manager_get_parent(lv_fragment_manager_t * manager)
{
    LV_ASSERT(manager);
    return manager->parent;
}

void lv_fragment_manager_recreate_obj(lv_fragment_manager_t * manager, lv_fragment_t * fragment)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(fragment);
    // Disable CB first
    if(fragment->_states->dialog) {
        lv_obj_remove_event_cb(fragment->obj, cb_msgbox_delete);
    }
    lv_fragment_del_obj(fragment);
    lv_fragment_create_obj(fragment, fragment->manager->container);

    const lv_fragment_class_t * cls = fragment->cls;
    lv_obj_t * obj = cls->create_obj_cb(fragment, fragment->manager->container);
    fragment->obj = obj;
    if(cls->obj_created_cb) {
        cls->obj_created_cb(fragment, obj);
    }
    if(fragment->_states->dialog) {
        lv_obj_add_event_cb(obj, cb_msgbox_delete, LV_EVENT_DELETE, fragment->_states);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static internal_states_t * item_new(const lv_fragment_class_t * cls)
{
    LV_ASSERT(cls->instance_size);
    internal_states_t * item = lv_mem_alloc(sizeof(internal_states_t));
    lv_memset_00(item, sizeof(internal_states_t));
    item->cls = cls;
    return item;
}

static void item_create_obj(internal_states_t * item, lv_obj_t * parent, const lv_obj_class_t * check_type)
{
    LV_ASSERT(item->instance);
    lv_fragment_create_obj(item->instance, parent);
    if(check_type) {
        LV_ASSERT(lv_obj_has_class(item->instance->obj, check_type));
    }
    item->obj_created = true;
}

static void item_del_obj(internal_states_t * item)
{
    if(!item->obj_created) return;
    item->destroying_obj = true;
    lv_fragment_t * instance = item->instance;
    lv_fragment_del_obj(instance);
    item->obj_created = false;
}

static void item_del_fragment(internal_states_t * item)
{
    lv_fragment_t * instance = item->instance;
    if(instance->cls->detached_cb) {
        instance->cls->detached_cb(instance);
    }
    instance->manager = NULL;
    instance->_states = NULL;
    lv_fragment_del(instance);
}

static void fragment_destroy_obj(lv_fragment_t * fragment)
{
    const lv_fragment_class_t * cls = fragment->cls;
    lv_obj_t * obj = fragment->obj;
    if(obj) {
        bool delete_handled = false;
        if(cls->obj_will_delete_cb) {
            delete_handled = cls->obj_will_delete_cb(fragment, obj);
        }
        if(!delete_handled) {
            lv_obj_del(obj);
        }
        else if(cls->obj_deleted_cb) {
            cls->obj_deleted_cb(fragment, NULL);
        }
    }
    else {
        LV_ASSERT(fragment->manager);
        LV_ASSERT(fragment->manager->container);
        bool delete_handled = false;
        if(cls->obj_will_delete_cb) {
            delete_handled = cls->obj_will_delete_cb(fragment, NULL);
        }
        if(!delete_handled) {
            lv_obj_clean(fragment->manager->container);
        }
        if(cls->obj_deleted_cb) {
            cls->obj_deleted_cb(fragment, NULL);
        }
    }
    fragment->obj = NULL;
}

#if LV_USE_MSGBOX
static void cb_msgbox_delete(lv_event_t * event)
{
    internal_states_t * item = lv_event_get_user_data(event);
    lv_fragment_t * instance = item->instance;
    lv_fragment_manager_t * manager = instance->manager;
    const lv_fragment_class_t * cls = instance->cls;
    if(event->target != instance->obj) return;
    if(cls->obj_deleted_cb) {
        cls->obj_deleted_cb(instance, event->target);
    }
    item->obj_created = false;
    instance->obj = NULL;
    if(!item->destroying_obj) {
        internal_states_t * prev = item->prev;
        item_del_fragment(item);
        lv_mem_free(item);
        manager->top = prev;
    }
}
#endif /*LV_USE_MSGBOX*/

#endif /*LV_USE_FRAGMENT*/