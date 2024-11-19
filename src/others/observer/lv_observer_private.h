/**
 * @file lv_observer_private.h
 *
 */

#ifndef LV_OBSERVER_PRIVATE_H
#define LV_OBSERVER_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_observer.h"

#if LV_USE_OBSERVER

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * The observer object: a descriptor returned when subscribing LVGL widgets to subjects
 */
struct _lv_observer_t {
    lv_subject_t * subject;             /**< The observed value */
    lv_observer_cb_t cb;                /**< Callback that should be called when the value changes*/
    void * target;                      /**< A target for the observer, e.g. a widget or style*/
    void * user_data;                   /**< Additional parameter supplied when subscribing*/
    uint32_t auto_free_user_data : 1;   /**< Automatically free user data when the observer is removed */
    uint32_t notified : 1;              /**< Mark if this observer was already notified*/
    uint32_t for_obj : 1;               /**< `target` is an `lv_obj_t *`*/
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OBSERVER */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBSERVER_PRIVATE_H*/
