/**
 * @file lv_fragment.c
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

typedef struct manager_stack_t {
    const lv_fragment_class_t * cls;
    lv_fragment_t * instance;
    bool obj_created;
    bool destroying_obj;
    bool dialog;
    struct manager_stack_t * prev;
} manager_stack_t;

struct _lv_fragment_manager_t {
    lv_obj_t * container;
    lv_fragment_t * parent;
    manager_stack_t * top;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static manager_stack_t * item_new(const lv_fragment_class_t * cls);

static lv_fragment_t * item_create_fragment(lv_fragment_manager_t * manager, manager_stack_t * item,
                                            void * args);

static void item_create_obj(lv_fragment_manager_t * manager, manager_stack_t * item, lv_obj_t * parent,
                            const lv_obj_class_t * check_type);

static void item_destroy_obj(lv_fragment_manager_t * manager, manager_stack_t * item);

static void item_destroy_fragment(manager_stack_t * item);

static void obj_cb_delete(lv_event_t * event);

static void fragment_destroy_obj(lv_fragment_t * fragment);

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
    LV_ASSERT(manager);
    manager_stack_t * top = manager->top;
    while(top) {
        LV_ASSERT(top->cls);
        item_destroy_obj(manager, top);
        item_destroy_fragment(top);
        struct manager_stack_t * prev = top->prev;
        lv_mem_free(top);
        top = prev;
    }
    lv_mem_free(manager);
}

void lv_fragment_manager_push(lv_fragment_manager_t * manager, const lv_fragment_class_t * cls, void * args)
{
    LV_ASSERT(manager);
    LV_ASSERT(cls);
    manager_stack_t * item = item_new(cls);
    lv_obj_t * parent = manager->container;
    item_create_fragment(manager, item, args);
    /* Destroy object of previous screen */
    if(manager->top) {
        item_destroy_obj(manager, manager->top);
    }
    item_create_obj(manager, item, parent, NULL);
    manager_stack_t * top = manager->top;
    item->prev = top;
    manager->top = item;
}

void lv_fragment_manager_replace(lv_fragment_manager_t * manager, const lv_fragment_class_t * cls,
                                 void * args)
{
    LV_ASSERT(manager);
    LV_ASSERT(cls);
    manager_stack_t * top = item_new(cls);
    item_create_fragment(manager, top, args);
    manager_stack_t * old = manager->top;
    if(old) {
        item_destroy_obj(manager, old);
        item_destroy_fragment(old);
        lv_mem_free(old);
    }
    manager->top = top;
    item_create_obj(manager, top, manager->container, NULL);
}

size_t lv_fragment_manager_size(lv_fragment_manager_t * manager)
{
    size_t size = 0;
    for(manager_stack_t * cur = manager->top; cur; cur = cur->prev) {
        size++;
    }
    return size;
}

void lv_fragment_manager_pop(lv_fragment_manager_t * manager)
{
    LV_ASSERT(manager);
    manager_stack_t * top = manager->top;
    if(!top) return;
    manager_stack_t * prev = top->prev;
    bool dialog = top->dialog;
    if(!dialog && prev) {
        item_create_fragment(manager, prev, NULL);
    }
    item_destroy_obj(manager, top);
    item_destroy_fragment(top);
    lv_mem_free(top);
    if(!dialog && prev) {
        item_create_obj(manager, prev, manager->container, NULL);
    }
    manager->top = prev;
}

bool lv_fragment_manager_dispatch_event(lv_fragment_manager_t * manager, int which, void * data1, void * data2)
{
    LV_ASSERT(manager);
    manager_stack_t * top = manager->top;
    if(!top) return false;
    lv_fragment_t * instance = top->instance;
    if(!instance || !top->cls->event_cb) return false;
    return top->cls->event_cb(instance, which, data1, data2);
}

#if LV_USE_MSGBOX

void lv_fragment_manager_show(lv_fragment_manager_t * manager, const lv_fragment_class_t * cls, void * args)
{
    LV_ASSERT(manager);
    LV_ASSERT(cls);
    manager_stack_t * item = item_new(cls);
    item_create_fragment(manager, item, args);
    item_create_obj(manager, item, NULL, &lv_msgbox_class);
    item->dialog = true;
    manager_stack_t * top = manager->top;
    item->prev = top;
    manager->top = item;
}

#endif

lv_fragment_t * lv_fragment_manager_get_top(lv_fragment_manager_t * manager)
{
    LV_ASSERT(manager);
    manager_stack_t * top = manager->top;
    if(!top)return NULL;
    return top->instance;
}

lv_fragment_t * lv_fragment_manager_get_parent(lv_fragment_manager_t * manager)
{
    LV_ASSERT(manager);
    return manager->parent;
}

lv_fragment_t * lv_fragment_class_create_unmanaged(const lv_fragment_class_t * cls,
                                                   lv_obj_t * container, void * args)
{
    LV_ASSERT(cls);
    LV_ASSERT(cls->instance_size);
    LV_ASSERT(cls->create_obj_cb);
    lv_fragment_t * instance = lv_mem_alloc(cls->instance_size);
    lv_memset_00(instance, cls->instance_size);
    instance->cls = cls;
    if(cls->constructor_cb) {
        cls->constructor_cb(instance, args);
    }
    lv_obj_t * obj = cls->create_obj_cb(instance, container);
    LV_ASSERT(obj);
    instance->obj = obj;
    if(cls->obj_created_cb) {
        cls->obj_created_cb(instance, obj);
    }
    return instance;
}

void lv_fragment_class_del_unmanaged(lv_fragment_t * fragment)
{
    LV_ASSERT(fragment);
    LV_ASSERT(fragment->obj);
    LV_ASSERT(!fragment->manager);
    const lv_fragment_class_t * cls = fragment->cls;
    if(cls->obj_will_delete_cb) {
        cls->obj_will_delete_cb(fragment, fragment->obj);
    }
    lv_obj_del(fragment->obj);
    if(cls->obj_deleted_cb) {
        cls->obj_deleted_cb(fragment, fragment->obj);
    }
    if(cls->destructor_cb) {
        cls->destructor_cb(fragment);
    }
    lv_mem_free(fragment);
}

void lv_fragment_pop(lv_fragment_t * fragment)
{
    LV_ASSERT(fragment);
    lv_fragment_manager_t * manager = fragment->manager;
    LV_ASSERT(manager);
    LV_ASSERT(manager->top->instance == fragment);
    lv_fragment_manager_pop(manager);
}

void lv_fragment_recreate_obj(lv_fragment_t * fragment)
{
    LV_ASSERT(fragment);
    // Disable CB first
    lv_obj_remove_event_cb(fragment->obj, obj_cb_delete);
    fragment_destroy_obj(fragment);

    const lv_fragment_class_t * cls = fragment->cls;
    lv_obj_t * obj = cls->create_obj_cb(fragment, fragment->manager->container);
    fragment->obj = obj;
    if(cls->obj_created_cb) {
        cls->obj_created_cb(fragment, obj);
    }
    if(obj) {
        lv_obj_add_event_cb(obj, obj_cb_delete, LV_EVENT_DELETE, fragment->priv_item);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static manager_stack_t * item_new(const lv_fragment_class_t * cls)
{
    LV_ASSERT(cls->instance_size);
    manager_stack_t * item = lv_mem_alloc(sizeof(manager_stack_t));
    lv_memset_00(item, sizeof(manager_stack_t));
    item->cls = cls;
    return item;
}

static lv_fragment_t * item_create_fragment(lv_fragment_manager_t * manager, manager_stack_t * item,
                                            void * args)
{
    if(item->instance) return item->instance;
    const lv_fragment_class_t * cls = item->cls;
    LV_ASSERT(cls->instance_size);
    lv_fragment_t * instance = lv_mem_alloc(cls->instance_size);
    lv_memset_00(instance, cls->instance_size);
    instance->cls = cls;
    instance->manager = manager;
    instance->priv_item = item;
    if(cls->constructor_cb) {
        cls->constructor_cb(instance, args);
    }
    item->instance = instance;
    return instance;
}

static void item_create_obj(lv_fragment_manager_t * manager, manager_stack_t * item, lv_obj_t * parent,
                            const lv_obj_class_t * check_type)
{
    LV_ASSERT(item->instance);
    const lv_fragment_class_t * cls = item->cls;
    LV_ASSERT(cls->create_obj_cb);
    lv_fragment_t * instance = item->instance;
    lv_obj_t * obj = cls->create_obj_cb(instance, parent);
    if(check_type) {
        LV_ASSERT(lv_obj_has_class(obj, check_type));
    }
    instance->obj = obj;
    item->obj_created = true;
    if(cls->obj_created_cb) {
        cls->obj_created_cb(instance, obj);
    }
    if(obj) {
        lv_obj_add_event_cb(obj, obj_cb_delete, LV_EVENT_DELETE, item);
    }
}

static void item_destroy_obj(lv_fragment_manager_t * manager, manager_stack_t * item)
{
    if(!item->obj_created) return;
    item->destroying_obj = true;
    lv_fragment_t * instance = item->instance;
    fragment_destroy_obj(instance);
    item->obj_created = false;
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

static void item_destroy_fragment(manager_stack_t * item)
{
    const lv_fragment_class_t * cls = item->cls;
    if(cls->destructor_cb) {
        cls->destructor_cb(item->instance);
    }
    lv_mem_free(item->instance);
    item->instance = NULL;
}

static void obj_cb_delete(lv_event_t * event)
{
    manager_stack_t * item = lv_event_get_user_data(event);
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
        manager_stack_t * prev = item->prev;
        item_destroy_fragment(item);
        lv_mem_free(item);
        manager->top = prev;
    }
}

#endif /*LV_USE_FRAGMENT*/
