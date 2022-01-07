/**
 * @file lv_fragment.h
 *
 */

#ifndef lv_fragment_H
#define lv_fragment_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_FRAGMENT

#include "../../../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_fragment_manager_t lv_fragment_manager_t;

typedef struct lv_fragment_t lv_fragment_t;
typedef struct lv_fragment_class_t lv_fragment_class_t;

/**
 * Opaque pointer for internal state management
 */
typedef struct manager_stack_t manager_stack_t;

struct lv_fragment_t {
    /**
     * Class of this fragment
     */
    const lv_fragment_class_t * cls;
    /**
     * Manager this fragment instance belongs to
     */
    lv_fragment_manager_t * manager;
    /**
     * lv_obj returned by create_obj_cb
     */
    lv_obj_t * obj;

    /**
     * Private internal states
     */
    manager_stack_t * priv_item;
};

struct lv_fragment_class_t {
    /**
     * Constructor function for fragment class
     * @param self Fragment instance
     * @param args Arguments assigned by fragment manager
     */
    void (*constructor_cb)(struct lv_fragment_t * self, void * args);

    /**
     * Destructor function for fragment class
     * @param self Fragment instance, will be freed after this call
     */
    void (*destructor_cb)(struct lv_fragment_t * self);

    /**
     * Create objects
     * @param self Fragment instance
     * @param container Container of the objects should be created upon
     * @return Created object, NULL if multiple objects has been created
     */
    lv_obj_t * (*create_obj_cb)(struct lv_fragment_t * self, lv_obj_t * container);

    /**
     *
     * @param self Fragment instance
     * @param obj lv_obj returned by create_obj_cb
     */
    void (*obj_created_cb)(struct lv_fragment_t * self, lv_obj_t * obj);

    /**
     * Called before objects in the fragment will be deleted.
     *
     * You can return true and delete the objects by yourself here.
     *
     * @param self Fragment instance
     * @param obj object with this fragment
     * @return true if the fragment will handle deletion on its own
     */
    bool (*obj_will_delete_cb)(struct lv_fragment_t * self, lv_obj_t * obj);

    /**
     * Called when the object created by fragment received `LV_EVENT_DELETE` event
     * @param self Fragment instance
     * @param obj object with this fragment
     */
    void (*obj_deleted_cb)(struct lv_fragment_t * self, lv_obj_t * obj);

    /**
     * Handle event
     * @param self Fragment instance
     * @param which User-defined ID of event
     * @param data1 User-defined data
     * @param data2 User-defined data
     */
    bool (*event_cb)(struct lv_fragment_t * self, int which, void * data1, void * data2);

    /**
     * *REQUIRED*: Allocation size of fragment
     */
    size_t instance_size;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create fragment manager instance
 * @param container Container object for manager to add objects to
 * @param parent Parent fragment if this manager is placed inside another fragment
 * @return Fragment manager instance
 */
lv_fragment_manager_t * lv_fragment_manager_create(lv_obj_t * container, lv_fragment_t * parent);

/**
 * Destroy fragment manager instance
 * @param manager Fragment manager instance
 */
void lv_fragment_manager_del(lv_fragment_manager_t * manager);

/**
 * Remove the top-most fragment for stack
 * @param manager Fragment manager instance
 */
void lv_fragment_manager_pop(lv_fragment_manager_t * manager);

/**
 * Add fragment to the stack
 * @param manager Fragment manager instance
 * @param cls Fragment class
 * @args User-defined argument
 */
void lv_fragment_manager_push(lv_fragment_manager_t * manager, const lv_fragment_class_t * cls, void * args);

/**
 * Replace top-most fragment. Old item in the stack will be removed.
 * @param manager Fragment manager instance
 * @param cls Fragment class
 * @param args Arguments assigned by fragment manager
 */
void lv_fragment_manager_replace(lv_fragment_manager_t * manager, const lv_fragment_class_t * cls,
                                 void * args);

/**
 * Get stack size of this fragment manager
 * @param manager Fragment manager instance
 * @return Stack size of this fragment manager
 */
size_t lv_fragment_manager_get_size(lv_fragment_manager_t * manager);

/**
 * Send event to top-most fragment
 * @param manager Fragment manager instance
 * @param which User-defined ID of event
 * @param data1 User-defined data
 * @param data2 User-defined data
 * @return true if fragment returned true
 */
bool lv_fragment_manager_dispatch_event(lv_fragment_manager_t * manager, int which, void * data1, void * data2);

/**
 * Show lv_msgbox
 * @param manager Fragment manager instance
 * @param cls Fragment class which must return valid lv_msgbox instance
 * @param args Arguments assigned by fragment manager
 */
void lv_fragment_manager_show(lv_fragment_manager_t * manager, const lv_fragment_class_t * cls, void * args);

lv_fragment_t * lv_fragment_manager_get_top(lv_fragment_manager_t * manager);

lv_fragment_t * lv_fragment_manager_get_parent(lv_fragment_manager_t * manager);

/**
 * Create an fragment but not adding to any manager instance.
 *
 * This is useful if you just want to organize your code and manage their lifecycle easier.
 *
 * @param cls Fragment class. This fragment must return non null object.
 * @param container Container of the objects should be created upon
 * @param args Arguments assigned by fragment manager
 * @return Unmanaged fragment instance
 */
lv_fragment_t * lv_fragment_class_create_unmanaged(const lv_fragment_class_t * cls,
                                                   lv_obj_t * container, void * args);

/**
 * Destroy the unmanaged fragment fragment.
 * @param fragment Container of the objects should be created upon
 */
void lv_fragment_class_del_unmanaged(lv_fragment_t * fragment);

/**
 * Asserts the fragment is the top-most one, and pop it
 * @param fragment Fragment instance
 */
void lv_fragment_pop(lv_fragment_t * fragment);

/**
 * Destroy obj in fragment, and recreate them.
 * @param fragment Fragment instance
 */
void lv_fragment_recreate_obj(lv_fragment_t * fragment);
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FRAGMENT*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*lv_fragment_H*/
