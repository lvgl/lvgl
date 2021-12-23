/**
 * @file lv_obj_controller.h
 *
 */

#ifndef LV_OBJ_CONTROLLER_H
#define LV_OBJ_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct lv_controller_manager_t lv_controller_manager_t;

typedef struct lv_obj_controller_t lv_obj_controller_t;
typedef struct lv_obj_controller_class_t lv_obj_controller_class_t;

typedef struct manager_stack_t manager_stack_t;

struct lv_obj_controller_t {
    const lv_obj_controller_class_t *cls;
    lv_controller_manager_t *manager;
    lv_obj_t *obj;

    manager_stack_t *priv_item;
};

struct lv_obj_controller_class_t {
    void (*constructor_cb)(struct lv_obj_controller_t *self, void *args);

    void (*destructor_cb)(struct lv_obj_controller_t *self);

    lv_obj_t *(*create_obj_cb)(struct lv_obj_controller_t *self, lv_obj_t *parent);

    void (*obj_created_cb)(struct lv_obj_controller_t *self, lv_obj_t *view);

    void (*obj_will_delete_cb)(struct lv_obj_controller_t *self, lv_obj_t *view);

    void (*obj_deleted_cb)(struct lv_obj_controller_t *self, lv_obj_t *view);

    bool (*event_cb)(struct lv_obj_controller_t *self, int which, void *data1, void *data2);

    size_t instance_size;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_controller_manager_t *lv_controller_manager_create(lv_obj_t *container, lv_obj_controller_t *parent);

void lv_controller_manager_del(lv_controller_manager_t *manager);

void lv_controller_manager_pop(lv_controller_manager_t *manager);

void lv_controller_manager_push(lv_controller_manager_t *manager, const lv_obj_controller_class_t *cls, void *args);

/**
 * Replace top-most controller. Old item in the stack will be removed.
 * @param manager
 * @param cls
 * @param args
 */
void lv_controller_manager_replace(lv_controller_manager_t *manager, const lv_obj_controller_class_t *cls, void *args);

/**
 * Show lv_msgbox
 * @param manager Controller manager instance
 * @param cls Controller class which must return valid lv_msgbox instance
 * @param args
 */
void lv_controller_manager_show(lv_controller_manager_t *manager, const lv_obj_controller_class_t *cls, void *args);

bool lv_controller_manager_dispatch_event(lv_controller_manager_t *manager, int which, void *data1, void *data2);

void lv_obj_controller_pop(lv_obj_controller_t *controller);

lv_obj_controller_t *lv_controller_manager_top_controller(lv_controller_manager_t *manager);

lv_obj_controller_t *lv_controller_manager_parent(lv_controller_manager_t *manager);

lv_obj_controller_t *lv_controller_create_unmanaged(lv_obj_t *parent, const lv_obj_controller_class_t *cls, void *args);

void lv_controller_recreate_obj(lv_obj_controller_t *controller);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_CONTROLLER_H*/