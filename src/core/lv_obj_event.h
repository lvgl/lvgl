/**
 * @file lv_obj_event.h
 *
 */

#ifndef LV_OBJ_EVENT_H
#define LV_OBJ_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "../misc/lv_event.h"
#include "../indev/lv_indev.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Cover check results.*/
typedef enum {
    LV_COVER_RES_COVER      = 0,
    LV_COVER_RES_NOT_COVER  = 1,
    LV_COVER_RES_MASKED     = 2,
} lv_cover_res_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Send an event to the object
 * @param obj           pointer to an object
 * @param event_code    the type of the event from `lv_event_t`
 * @param param         arbitrary data depending on the widget type and the event. (Usually `NULL`)
 * @return LV_RESULT_OK: `obj` was not deleted in the event; LV_RESULT_INVALID: `obj` was deleted in the event_code
 */
lv_result_t lv_obj_send_event(lv_obj_t * obj, lv_event_code_t event_code, void * param);

/**
 * Used by the widgets internally to call the ancestor widget types's event handler
 * @param class_p   pointer to the class of the widget (NOT the ancestor class)
 * @param e         pointer to the event descriptor
 * @return          LV_RESULT_OK: the target object was not deleted in the event; LV_RESULT_INVALID: it was deleted in the event_code
 */
lv_result_t lv_obj_event_base(const lv_obj_class_t * class_p, lv_event_t * e);

/**
 * Get the current target of the event. It's the object which event handler being called.
 * If the event is not bubbled it's the same as "original" target.
 * @param e     pointer to the event descriptor
 * @return      the target of the event_code
 */
lv_obj_t * lv_event_get_current_target_obj(lv_event_t * e);

/**
 * Get the object originally targeted by the event. It's the same even if the event is bubbled.
 * @param e     pointer to the event descriptor
 * @return      pointer to the original target of the event_code
 */
lv_obj_t * lv_event_get_target_obj(lv_event_t * e);

/**
 * Add an event handler function for an object.
 * Used by the user to react on event which happens with the object.
 * An object can have multiple event handler. They will be called in the same order as they were added.
 * @param obj       pointer to an object
 * @param filter    an event code (e.g. `LV_EVENT_CLICKED`) on which the event should be called. `LV_EVENT_ALL` can be used to receive all the events.
 * @param event_cb  the new event function
 * @param           user_data custom data will be available in `event_cb`
 * @return          handler to the event. It can be used in `lv_obj_remove_event_dsc`.
 */
lv_event_dsc_t * lv_obj_add_event_cb(lv_obj_t * obj, lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data);

uint32_t lv_obj_get_event_count(lv_obj_t * obj);

lv_event_dsc_t * lv_obj_get_event_dsc(lv_obj_t * obj, uint32_t index);

bool lv_obj_remove_event(lv_obj_t * obj, uint32_t index);

bool lv_obj_remove_event_cb(lv_obj_t * obj, lv_event_cb_t event_cb);

bool lv_obj_remove_event_dsc(lv_obj_t * obj, lv_event_dsc_t * dsc);

/**
 * Remove an event_cb with user_data
 * @param obj           pointer to a obj
 * @param event_cb      the event_cb of the event to remove
 * @param user_data     user_data
 * @return              the count of the event removed
 */
uint32_t lv_obj_remove_event_cb_with_user_data(lv_obj_t * obj, lv_event_cb_t event_cb, void * user_data);

/**
 * Get the input device passed as parameter to indev related events.
 * @param e     pointer to an event
 * @return      the indev that triggered the event or NULL if called on a not indev related event
 */
lv_indev_t * lv_event_get_indev(lv_event_t * e);

/**
 * Get the draw context which should be the first parameter of the draw functions.
 * Namely: `LV_EVENT_DRAW_MAIN/POST`, `LV_EVENT_DRAW_MAIN/POST_BEGIN`, `LV_EVENT_DRAW_MAIN/POST_END`
 * @param e     pointer to an event
 * @return      pointer to a draw context or NULL if called on an unrelated event
 */
lv_layer_t * lv_event_get_layer(lv_event_t * e);

/**
 * Get the old area of the object before its size was changed. Can be used in `LV_EVENT_SIZE_CHANGED`
 * @param e     pointer to an event
 * @return      the old absolute area of the object or NULL if called on an unrelated event
 */
const lv_area_t * lv_event_get_old_size(lv_event_t * e);

/**
 * Get the key passed as parameter to an event. Can be used in `LV_EVENT_KEY`
 * @param e     pointer to an event
 * @return      the triggering key or NULL if called on an unrelated event
 */
uint32_t lv_event_get_key(lv_event_t * e);

/**
 * Get the signed rotary encoder diff. passed as parameter to an event. Can be used in `LV_EVENT_ROTARY`
 * @param e     pointer to an event
 * @return      the triggering key or NULL if called on an unrelated event
 */
int32_t lv_event_get_rotary_diff(lv_event_t * e);

/**
 * Get the animation descriptor of a scrolling. Can be used in `LV_EVENT_SCROLL_BEGIN`
 * @param e     pointer to an event
 * @return      the animation that will scroll the object. (can be modified as required)
 */
lv_anim_t * lv_event_get_scroll_anim(lv_event_t * e);

/**
 * Set the new extra draw size. Can be used in `LV_EVENT_REFR_EXT_DRAW_SIZE`
 * @param e     pointer to an event
 * @param size  The new extra draw size
 */
void lv_event_set_ext_draw_size(lv_event_t * e, int32_t size);

/**
 * Get a pointer to an `lv_point_t` variable in which the self size should be saved (width in `point->x` and height `point->y`).
 * Can be used in `LV_EVENT_GET_SELF_SIZE`
 * @param e     pointer to an event
 * @return      pointer to `lv_point_t` or NULL if called on an unrelated event
 */
lv_point_t * lv_event_get_self_size_info(lv_event_t * e);

/**
 * Get a pointer to an `lv_hit_test_info_t` variable in which the hit test result should be saved. Can be used in `LV_EVENT_HIT_TEST`
 * @param e     pointer to an event
 * @return      pointer to `lv_hit_test_info_t` or NULL if called on an unrelated event
 */
lv_hit_test_info_t * lv_event_get_hit_test_info(lv_event_t * e);

/**
 * Get a pointer to an area which should be examined whether the object fully covers it or not.
 * Can be used in `LV_EVENT_HIT_TEST`
 * @param e     pointer to an event
 * @return      an area with absolute coordinates to check
 */
const lv_area_t * lv_event_get_cover_area(lv_event_t * e);

/**
 * Set the result of cover checking. Can be used in `LV_EVENT_COVER_CHECK`
 * @param e     pointer to an event
 * @param res   an element of ::lv_cover_check_info_t
 */
void lv_event_set_cover_res(lv_event_t * e, lv_cover_res_t res);

/**
 * Get the draw task which was just added.
 * Can be used in `LV_EVENT_DRAW_TASK_ADDED event`
 * @param e     pointer to an event
 * @return      the added draw task
 */
lv_draw_task_t * lv_event_get_draw_task(lv_event_t * e);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_EVENT_H*/
