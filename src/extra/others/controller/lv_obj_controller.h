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
#include "../../../lv_conf_internal.h"

#if LV_USE_OBJ_CONTROLLER

#include "../../../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_controller_manager_t lv_controller_manager_t;

typedef struct lv_obj_controller_t lv_obj_controller_t;
typedef struct lv_obj_controller_class_t lv_obj_controller_class_t;

/**
 * Opaque pointer for internal state management
 */
typedef struct manager_stack_t manager_stack_t;

struct lv_obj_controller_t {
    /**
     * Class of this controller
     */
    const lv_obj_controller_class_t * cls;
    /**
     * Manager this controller instance belongs to
     */
    lv_controller_manager_t * manager;
    /**
     * lv_obj returned by create_obj_cb
     */
    lv_obj_t * obj;

    /**
     * Private internal states
     */
    manager_stack_t * priv_item;
};

struct lv_obj_controller_class_t {
    /**
     * Constructor function for controller class
     * @param self Controller instance
     * @param args Arguments assigned by controller manager
     */
    void (*constructor_cb)(struct lv_obj_controller_t * self, void * args);

    /**
     * Destructor function for controller class
     * @param self Controller instance, will be freed after this call
     */
    void (*destructor_cb)(struct lv_obj_controller_t * self);

    /**
     * Create objects
     * @param self Controller instance
     * @param container Container of the objects should be created upon
     * @return Created object, NULL if multiple objects has been created
     */
    lv_obj_t * (*create_obj_cb)(struct lv_obj_controller_t * self, lv_obj_t * container);

    /**
     *
     * @param self Controller instance
     * @param obj lv_obj returned by create_obj_cb
     */
    void (*obj_created_cb)(struct lv_obj_controller_t * self, lv_obj_t * obj);

    /**
     * Called before objects in the controller will be deleted.
     *
     * You can return true and delete the objects by yourself here.
     *
     * @param self Controller instance
     * @param obj object with this controller
     * @return true if the controller will handle deletion on its own
     */
    bool (*obj_will_delete_cb)(struct lv_obj_controller_t * self, lv_obj_t * obj);

    /**
     * Called when the object created by controller received `LV_EVENT_DELETE` event
     * @param self Controller instance
     * @param obj object with this controller
     */
    void (*obj_deleted_cb)(struct lv_obj_controller_t * self, lv_obj_t * obj);

    /**
     * Handle event
     * @param self Controller instance
     * @param which User-defined ID of event
     * @param data1 User-defined data
     * @param data2 User-defined data
     */
    bool (*event_cb)(struct lv_obj_controller_t * self, int which, void * data1, void * data2);

    /**
     * *REQUIRED*: Allocation size of controller
     */
    size_t instance_size;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create controller manager instance
 * @param container Container object for manager to add objects to
 * @param parent Parent obj_controller if this manager is placed inside another obj_controller
 * @return Controller manager instance
 */
lv_controller_manager_t * lv_controller_manager_create(lv_obj_t * container, lv_obj_controller_t * parent);

/**
 * Destroy controller manager instance
 * @param manager Controller manager instance
 */
void lv_controller_manager_del(lv_controller_manager_t * manager);

/**
 * Remove the top-most controller for stack
 * @param manager Controller manager instance
 */
void lv_controller_manager_pop(lv_controller_manager_t * manager);

/**
 * Add controller to the stack
 * @param manager Controller manager instance
 * @param cls Controller class
 * @args User-defined argument
 */
void lv_controller_manager_push(lv_controller_manager_t * manager, const lv_obj_controller_class_t * cls, void * args);

/**
 * Replace top-most controller. Old item in the stack will be removed.
 * @param manager Controller manager instance
 * @param cls Controller class
 * @param args Arguments assigned by controller manager
 */
void lv_controller_manager_replace(lv_controller_manager_t * manager, const lv_obj_controller_class_t * cls,
                                   void * args);

/**
 * Get stack size of this controller manager
 * @param manager Controller manager instance
 * @return Stack size of this controller manager
 */
size_t lv_controller_manager_size(lv_controller_manager_t * manager);

/**
 * Send event to top-most controller
 * @param manager Controller manager instance
 * @param which User-defined ID of event
 * @param data1 User-defined data
 * @param data2 User-defined data
 * @return true if controller returned true
 */
bool lv_controller_manager_dispatch_event(lv_controller_manager_t * manager, int which, void * data1, void * data2);

/**
 * Show lv_msgbox
 * @param manager Controller manager instance
 * @param cls Controller class which must return valid lv_msgbox instance
 * @param args Arguments assigned by controller manager
 */
void lv_controller_manager_show(lv_controller_manager_t * manager, const lv_obj_controller_class_t * cls, void * args);

lv_obj_controller_t * lv_controller_manager_top_controller(lv_controller_manager_t * manager);

lv_obj_controller_t * lv_controller_manager_get_parent(lv_controller_manager_t * manager);

/**
 * Create an obj_controller but not adding to any manager instance.
 *
 * This is useful if you just want to organize your code and manage their lifecycle easier.
 *
 * @param cls Controller class. This controller must return non null object.
 * @param container Container of the objects should be created upon
 * @param args Arguments assigned by controller manager
 * @return Unmanaged controller instance
 */
lv_obj_controller_t * lv_obj_controller_class_create_unmanaged(const lv_obj_controller_class_t * cls,
                                                               lv_obj_t * container, void * args);

/**
 * Destroy the unmanaged controller instance.
 * @param controller Container of the objects should be created upon
 */
void lv_obj_controller_class_del_unmanaged(lv_obj_controller_t * controller);

/**
 * Asserts the controller is the top-most one, and pop it
 * @param controller Controller instance
 */
void lv_obj_controller_pop(lv_obj_controller_t * controller);

/**
 * Destroy obj in controller, and recreate them.
 * @param controller Controller instance
 */
void lv_obj_controller_recreate_obj(lv_obj_controller_t * controller);
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OBJ_CONTROLLER*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_CONTROLLER_H*/
