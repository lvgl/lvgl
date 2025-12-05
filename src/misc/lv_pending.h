/**
 * @file lv_pending.h
 *
 */

#ifndef LV_PENDING_H
#define LV_PENDING_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_pending_t lv_pending_t;

typedef void (*lv_pending_free_cb_t)(void * obj, void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a pending list
 * @param obj_size the size of the objects in the list
 * @param capacity_default the default capacity of the list
 * @return a pointer to the pending list
 */
lv_pending_t * lv_pending_create(size_t obj_size, uint32_t capacity_default);

/**
 * Destroy a pending list
 * @param pending pointer to the pending list
 */
void lv_pending_destroy(lv_pending_t * pending);

/**
 * Set a free callback for the pending list
 * @param pending pointer to the pending list
 * @param free_cb the free callback
 * @param user_data user data to pass to the free callback
 */
void lv_pending_set_free_cb(lv_pending_t * pending, lv_pending_free_cb_t free_cb,
                            void * user_data);

/**
 * Add an object to the pending list
 * @param pending pointer to the pending list
 * @param obj pointer to the object to add
 */
void lv_pending_add(lv_pending_t * pending, void * obj);

/**
 * Remove all objects from both pending lists
 * @param pending pointer to the pending list
 */
void lv_pending_remove_all(lv_pending_t * pending);

/**
 * Remove all old object references and swap new object references
 * @param pending pointer to the pending list
 */
void lv_pending_swap(lv_pending_t * pending);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PENDING_H*/
