/**
 * @file lv_fragment_private.h
 *
 */

#ifndef LV_FRAGMENT_PRIVATE_H
#define LV_FRAGMENT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

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

struct lv_fragment_t {
    /**
     * Class of this fragment
     */
    const lv_fragment_class_t * cls;
    /**
     * Managed fragment states. If not null, then this fragment is managed.
     *
     * @warning Don't modify values inside this struct!
     */
    lv_fragment_managed_states_t * managed;
    /**
     * Child fragment manager
     */
    lv_fragment_manager_t * child_manager;
    /**
     * lv_obj returned by create_obj_cb
     */
    lv_obj_t * obj;

};

struct lv_fragment_class_t {
    /**
     * Constructor function for fragment class
     * @param self Fragment instance
     * @param args Arguments assigned by fragment manager
     */
    void (*constructor_cb)(lv_fragment_t * self, void * args);

    /**
     * Destructor function for fragment class
     * @param self Fragment instance, will be freed after this call
     */
    void (*destructor_cb)(lv_fragment_t * self);

    /**
     * Fragment attached to manager
     * @param self Fragment instance
     */
    void (*attached_cb)(lv_fragment_t * self);

    /**
     * Fragment detached from manager
     * @param self Fragment instance
     */
    void (*detached_cb)(lv_fragment_t * self);

    /**
     * Create objects
     * @param self Fragment instance
     * @param container Container of the objects should be created upon
     * @return Created object, NULL if multiple objects has been created
     */
    lv_obj_t * (*create_obj_cb)(lv_fragment_t * self, lv_obj_t * container);

    /**
     *
     * @param self Fragment instance
     * @param obj lv_obj returned by create_obj_cb
     */
    void (*obj_created_cb)(lv_fragment_t * self, lv_obj_t * obj);

    /**
     * Called before objects in the fragment will be deleted.
     *
     * @param self Fragment instance
     * @param obj object with this fragment
     */
    void (*obj_will_delete_cb)(lv_fragment_t * self, lv_obj_t * obj);

    /**
     * Called when the object created by fragment received `LV_EVENT_DELETE` event
     * @param self Fragment instance
     * @param obj object with this fragment
     */
    void (*obj_deleted_cb)(lv_fragment_t * self, lv_obj_t * obj);

    /**
     * Handle event
     * @param self Fragment instance
     * @param which User-defined ID of event
     * @param data1 User-defined data
     * @param data2 User-defined data
     */
    bool (*event_cb)(lv_fragment_t * self, int code, void * userdata);

    /**
     * *REQUIRED*: Allocation size of fragment
     */
    size_t instance_size;
};

/**
 * Fragment states
 */
struct lv_fragment_managed_states_t  {
    /**
     * Class of the fragment
     */
    const lv_fragment_class_t * cls;
    /**
     * Manager the fragment attached to
     */
    lv_fragment_manager_t * manager;
    /**
     * Container object the fragment adding view to
     */
    lv_obj_t * const * container;
    /**
     * Fragment instance
     */
    lv_fragment_t * instance;
    /**
     * true between `create_obj_cb` and `obj_deleted_cb`
     */
    bool obj_created;
    /**
     * true before `lv_fragment_delete_obj` is called. Don't touch any object if this is true
     */
    bool destroying_obj;
    /**
     * true if this fragment is in navigation stack that can be popped
     */
    bool in_stack;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_FRAGMENT */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FRAGMENT_PRIVATE_H*/
