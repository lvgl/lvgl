/**
 * @file lv_indev.h
 *
 */

#ifndef LV_INDEV_H
#define LV_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "../lv_hal/lv_hal_indev.h"
#include "../lv_core/lv_group.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the display input device subsystem
 */
void _lv_indev_init(void);

/**
 * Called periodically to read the input devices
 * @param task pointer to the task itself
 */
void _lv_indev_read_task(lv_task_t * task);

/**
 * Get the currently processed input device. Can be used in action functions too.
 * @return pointer to the currently processed input device or NULL if no input device processing
 * right now
 */
lv_indev_t * lv_indev_get_act(void);

/**
 * Get the type of an input device
 * @param indev pointer to an input device
 * @return the type of the input device from `lv_hal_indev_type_t` (`LV_INDEV_TYPE_...`)
 */
lv_indev_type_t lv_indev_get_type(const lv_indev_t * indev);

/**
 * Reset one or all input devices
 * @param indev pointer to an input device to reset or NULL to reset all of them
 * @param obj pointer to an object which triggers the reset.
 */
void lv_indev_reset(lv_indev_t * indev, lv_obj_t * obj);

/**
 * Reset the long press state of an input device
 * @param indev_proc pointer to an input device
 */
void lv_indev_reset_long_press(lv_indev_t * indev);

/**
 * Enable or disable an input devices
 * @param indev pointer to an input device
 * @param en true: enable; false: disable
 */
void lv_indev_enable(lv_indev_t * indev, bool en);

/**
 * Set a cursor for a pointer input device (for LV_INPUT_TYPE_POINTER and LV_INPUT_TYPE_BUTTON)
 * @param indev pointer to an input device
 * @param cur_obj pointer to an object to be used as cursor
 */
void lv_indev_set_cursor(lv_indev_t * indev, lv_obj_t * cur_obj);

#if LV_USE_GROUP
/**
 * Set a destination group for a keypad input device (for LV_INDEV_TYPE_KEYPAD)
 * @param indev pointer to an input device
 * @param group point to a group
 */
void lv_indev_set_group(lv_indev_t * indev, lv_group_t * group);
#endif

/**
 * Set the an array of points for LV_INDEV_TYPE_BUTTON.
 * These points will be assigned to the buttons to press a specific point on the screen
 * @param indev pointer to an input device
 * @param group point to a group
 */
void lv_indev_set_button_points(lv_indev_t * indev, const lv_point_t points[]);

/**
 * Get the last point of an input device (for LV_INDEV_TYPE_POINTER and LV_INDEV_TYPE_BUTTON)
 * @param indev pointer to an input device
 * @param point pointer to a point to store the result
 */
void lv_indev_get_point(const lv_indev_t * indev, lv_point_t * point);

/**
* Get the current gesture direct
* @param indev pointer to an input device
* @return current gesture direct
*/
lv_gesture_dir_t lv_indev_get_gesture_dir(const lv_indev_t * indev);

/**
 * Get the last pressed key of an input device (for LV_INDEV_TYPE_KEYPAD)
 * @param indev pointer to an input device
 * @return the last pressed key (0 on error)
 */
uint32_t lv_indev_get_key(const lv_indev_t * indev);

/**
 * Check if there is dragging with an input device or not (for LV_INDEV_TYPE_POINTER and
 * LV_INDEV_TYPE_BUTTON)
 * @param indev pointer to an input device
 * @return true: drag is in progress
 */
bool lv_indev_is_dragging(const lv_indev_t * indev);

/**
 * Get the vector of dragging of an input device (for LV_INDEV_TYPE_POINTER and
 * LV_INDEV_TYPE_BUTTON)
 * @param indev pointer to an input device
 * @param point pointer to a point to store the vector
 */
void lv_indev_get_vect(const lv_indev_t * indev, lv_point_t * point);

/**
 * Manually finish dragging.
 * `LV_SIGNAL_DRAG_END` and `LV_EVENT_DRAG_END` will be sent.
 * @param indev pointer to an input device
 * @return `LV_RES_INV` if the object being dragged was deleted. Else `LV_RES_OK`.
 */
lv_res_t lv_indev_finish_drag(lv_indev_t * indev);

/**
 * Do nothing until the next release
 * @param indev pointer to an input device
 */
void lv_indev_wait_release(lv_indev_t * indev);


/**
 * Gets a pointer to the currently active object in indev proc functions.
 * NULL if no object is currently being handled or if groups aren't used.
 * @return pointer to currently active object
 */
lv_obj_t * lv_indev_get_obj_act(void);

/**
 * Search the most top, clickable object by a point
 * @param obj pointer to a start object, typically the screen
 * @param point pointer to a point for searching the most top child
 * @return pointer to the found object or NULL if there was no suitable object
 */
lv_obj_t * lv_indev_search_obj(lv_obj_t * obj, lv_point_t * point);

/**
 * Get a pointer to the indev read task to
 * modify its parameters with `lv_task_...` functions.
 * @param indev pointer to an inout device
 * @return pointer to the indev read refresher task. (NULL on error)
 */
lv_task_t * lv_indev_get_read_task(lv_disp_t * indev);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_INDEV_H*/
