/**
 * @file lv_obj_controller.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_obj_controller.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct manager_stack_t {
    const lv_obj_controller_class_t *cls;
    lv_obj_controller_t *controller;
    bool obj_created;
    bool destroying_obj;
    bool dialog;
    struct manager_stack_t *prev;
} manager_stack_t;

struct lv_controller_manager_t {
    lv_obj_t *container;
    lv_obj_controller_t *parent;
    manager_stack_t *top;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static manager_stack_t *item_new(const lv_obj_controller_class_t *cls);

static lv_obj_controller_t *item_create_controller(lv_controller_manager_t *manager, manager_stack_t *item, void *args);

static void item_create_obj(lv_controller_manager_t *manager, manager_stack_t *item, lv_obj_t *parent,
                            const lv_obj_class_t *check_type);

static void item_destroy_obj(lv_controller_manager_t *manager, manager_stack_t *item);

static void item_destroy_controller(manager_stack_t *item);

static void obj_cb_delete(lv_event_t *event);

static void controller_destroy_obj(lv_obj_controller_t *controller);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/



/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_controller_manager_t *lv_controller_manager_create(lv_obj_t *container, lv_obj_controller_t *parent) {
    LV_ASSERT(container);
    lv_controller_manager_t *instance = lv_mem_alloc(sizeof(lv_controller_manager_t));
    instance->parent = parent;
    instance->container = container;
    instance->top = NULL;
    return instance;
}

void lv_controller_manager_del(lv_controller_manager_t *manager) {
    LV_ASSERT(manager);
    manager_stack_t *top = manager->top;
    while (top) {
        LV_ASSERT(top->cls);
        item_destroy_obj(manager, top);
        item_destroy_controller(top);
        struct manager_stack_t *prev = top->prev;
        lv_mem_free(top);
        top = prev;
    }
    lv_mem_free(manager);
}

void lv_controller_manager_push(lv_controller_manager_t *manager, const lv_obj_controller_class_t *cls, void *args) {
    LV_ASSERT(manager);
    LV_ASSERT(cls);
    manager_stack_t *item = item_new(cls);
    lv_obj_t *parent = manager->container;
    item_create_controller(manager, item, args);
    /* Destroy object of previous screen */
    if (manager->top) {
        item_destroy_obj(manager, manager->top);
    }
    item_create_obj(manager, item, parent, NULL);
    manager_stack_t *top = manager->top;
    item->prev = top;
    manager->top = item;
}

void lv_controller_manager_replace(lv_controller_manager_t *manager, const lv_obj_controller_class_t *cls, void *args) {
    LV_ASSERT(manager);
    LV_ASSERT(cls);
    manager_stack_t *top = item_new(cls);
    item_create_controller(manager, top, args);
    manager_stack_t *old = manager->top;
    if (old) {
        item_destroy_obj(manager, old);
        item_destroy_controller(old);
        lv_mem_free(old);
    }
    manager->top = top;
    item_create_obj(manager, top, manager->container, NULL);
}

void lv_controller_manager_show(lv_controller_manager_t *manager, const lv_obj_controller_class_t *cls, void *args) {
    LV_ASSERT(manager);
    LV_ASSERT(cls);
    manager_stack_t *item = item_new(cls);
    item_create_controller(manager, item, args);
    item_create_obj(manager, item, NULL, &lv_msgbox_class);
    item->dialog = true;
    manager_stack_t *top = manager->top;
    item->prev = top;
    manager->top = item;
}

void lv_controller_manager_pop(lv_controller_manager_t *manager) {
    LV_ASSERT(manager);
    manager_stack_t *top = manager->top;
    if (!top) return;
    manager_stack_t *prev = top->prev;
    bool dialog = top->dialog;
    if (!dialog && prev) {
        item_create_controller(manager, prev, NULL);
    }
    item_destroy_obj(manager, top);
    item_destroy_controller(top);
    lv_mem_free(top);
    if (!dialog && prev) {
        item_create_obj(manager, prev, manager->container, NULL);
    }
    manager->top = prev;
}

bool lv_controller_manager_dispatch_event(lv_controller_manager_t *manager, int which, void *data1, void *data2) {
    LV_ASSERT(manager);
    manager_stack_t *top = manager->top;
    if (!top) return false;
    lv_obj_controller_t *controller = top->controller;
    if (!controller || !top->cls->event_cb) return false;
    return top->cls->event_cb(controller, which, data1, data2);
}

void lv_obj_controller_pop(lv_obj_controller_t *controller) {
    LV_ASSERT(controller);
    lv_controller_manager_t *manager = controller->manager;
    LV_ASSERT(manager);
    LV_ASSERT(manager->top->controller == controller);
    lv_controller_manager_pop(manager);
}

lv_obj_controller_t *lv_controller_manager_top_controller(lv_controller_manager_t *manager) {
    LV_ASSERT(manager);
    manager_stack_t *top = manager->top;
    if (!top)return NULL;
    return top->controller;
}

lv_obj_controller_t *lv_controller_manager_parent(lv_controller_manager_t *manager) {
    LV_ASSERT(manager);
    return manager->parent;
}

lv_obj_controller_t *lv_controller_create_unmanaged(lv_obj_t *parent, const lv_obj_controller_class_t *cls,
                                                    void *args) {
    LV_ASSERT(cls);
    LV_ASSERT(cls->instance_size);
    LV_ASSERT(cls->create_obj_cb);
    lv_obj_controller_t *controller = lv_mem_alloc(cls->instance_size);
    lv_memset_00(controller, cls->instance_size);
    controller->cls = cls;
    if (cls->constructor_cb) {
        cls->constructor_cb(controller, args);
    }
    lv_obj_t *obj = cls->create_obj_cb(controller, parent);
    controller->obj = obj;
    if (cls->obj_created_cb) {
        cls->obj_created_cb(controller, obj);
    }
    return controller;
}

void lv_controller_recreate_obj(lv_obj_controller_t *controller) {
    LV_ASSERT(controller);
    // Disable CB first
    lv_obj_remove_event_cb(controller->obj, obj_cb_delete);
    controller_destroy_obj(controller);

    const lv_obj_controller_class_t *cls = controller->cls;
    lv_obj_t *obj = cls->create_obj_cb(controller, controller->manager->container);
    controller->obj = obj;
    if (cls->obj_created_cb) {
        cls->obj_created_cb(controller, obj);
    }
    if (obj) {
        lv_obj_add_event_cb(obj, obj_cb_delete, LV_EVENT_DELETE, controller->priv_item);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static manager_stack_t *item_new(const lv_obj_controller_class_t *cls) {
    LV_ASSERT(cls->instance_size);
    manager_stack_t *item = lv_mem_alloc(sizeof(manager_stack_t));
    lv_memset_00(item, sizeof(manager_stack_t));
    item->cls = cls;
    return item;
}

static lv_obj_controller_t *item_create_controller(lv_controller_manager_t *manager, manager_stack_t *item,
                                                   void *args) {
    if (item->controller) return item->controller;
    const lv_obj_controller_class_t *cls = item->cls;
    LV_ASSERT(cls->instance_size);
    lv_obj_controller_t *controller = lv_mem_alloc(cls->instance_size);
    lv_memset_00(controller, cls->instance_size);
    controller->cls = cls;
    controller->manager = manager;
    controller->priv_item = item;
    if (cls->constructor_cb) {
        cls->constructor_cb(controller, args);
    }
    item->controller = controller;
    return controller;
}

static void item_create_obj(lv_controller_manager_t *manager, manager_stack_t *item, lv_obj_t *parent,
                            const lv_obj_class_t *check_type) {
    LV_ASSERT(item->controller);
    const lv_obj_controller_class_t *cls = item->cls;
    LV_ASSERT(cls->create_obj_cb);
    lv_obj_controller_t *controller = item->controller;
    lv_obj_t *obj = cls->create_obj_cb(controller, parent);
    if (check_type) {
        LV_ASSERT(lv_obj_has_class(obj, check_type));
    }
    controller->obj = obj;
    item->obj_created = true;
    if (cls->obj_created_cb) {
        cls->obj_created_cb(controller, obj);
    }
    if (obj) {
        lv_obj_add_event_cb(obj, obj_cb_delete, LV_EVENT_DELETE, item);
    }
}

static void item_destroy_obj(lv_controller_manager_t *manager, manager_stack_t *item) {
    if (!item->obj_created) return;
    item->destroying_obj = true;
    lv_obj_controller_t *controller = item->controller;
    controller_destroy_obj(controller);
    item->obj_created = false;
}

static void controller_destroy_obj(lv_obj_controller_t *controller) {
    const lv_obj_controller_class_t *cls = controller->cls;
    lv_obj_t *obj = controller->obj;
    if (obj) {
        if (cls->obj_will_delete_cb) {
            cls->obj_will_delete_cb(controller, obj);
        }
        lv_obj_del(obj);
    } else {
        LV_ASSERT(controller->manager);
        LV_ASSERT(controller->manager->container);
        if (cls->obj_will_delete_cb) {
            cls->obj_will_delete_cb(controller, NULL);
        }
        lv_obj_clean(controller->manager->container);
        if (cls->obj_deleted_cb) {
            cls->obj_deleted_cb(controller, NULL);
        }
    }
    controller->obj = NULL;
}

static void item_destroy_controller(manager_stack_t *item) {
    const lv_obj_controller_class_t *cls = item->cls;
    if (cls->destructor_cb) {
        cls->destructor_cb(item->controller);
    }
    lv_mem_free(item->controller);
    item->controller = NULL;
}

static void obj_cb_delete(lv_event_t *event) {
    manager_stack_t *item = lv_event_get_user_data(event);
    lv_obj_controller_t *controller = item->controller;
    lv_controller_manager_t *manager = controller->manager;
    const lv_obj_controller_class_t *cls = controller->cls;
    if (event->target != controller->obj) return;
    if (cls->obj_deleted_cb) {
        cls->obj_deleted_cb(controller, event->target);
    }
    item->obj_created = false;
    controller->obj = NULL;
    if (!item->destroying_obj) {
        manager_stack_t *prev = item->prev;
        item_destroy_controller(item);
        lv_mem_free(item);
        manager->top = prev;
    }
}
