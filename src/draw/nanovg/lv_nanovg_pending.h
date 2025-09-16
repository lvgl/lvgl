/**
 * @file lv_nanovg_pending.h
 *
 */

#ifndef LV_NANOVG_PENDING_H
#define LV_NANOVG_PENDING_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"

#if LV_USE_DRAW_NANOVG

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_nanovg_pending_t lv_nanovg_pending_t;

typedef void (*lv_nanovg_pending_free_cb_t)(void * obj, void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a pending list
 * @param obj_size the size of the objects in the list
 * @param capacity_default the default capacity of the list
 * @return a pointer to the pending list
 */
lv_nanovg_pending_t * lv_nanovg_pending_create(size_t obj_size, uint32_t capacity_default);

/**
 * Destroy a pending list
 * @param pending pointer to the pending list
 */
void lv_nanovg_pending_destroy(lv_nanovg_pending_t * pending);

/**
 * Set a free callback for the pending list
 * @param pending pointer to the pending list
 * @param free_cb the free callback
 * @param user_data user data to pass to the free callback
 */
void lv_nanovg_pending_set_free_cb(lv_nanovg_pending_t * pending, lv_nanovg_pending_free_cb_t free_cb,
                                   void * user_data);

/**
 * Add an object to the pending list
 * @param pending pointer to the pending list
 * @param obj pointer to the object to add
 */
void lv_nanovg_pending_add(lv_nanovg_pending_t * pending, void * obj);

/**
 * Remove all objects from the active pending list
 * @param pending pointer to the pending list
 */
void lv_nanovg_pending_remove_all(lv_nanovg_pending_t * pending);

/**
 * Remove all old objects reference and swap new objects reference
 * @param pending pointer to the pending list
 */
void lv_nanovg_pending_swap(lv_nanovg_pending_t * pending);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_NANOVG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_NANOVG_PENDING_H*/
