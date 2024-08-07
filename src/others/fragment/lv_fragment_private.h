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
