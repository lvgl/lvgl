/**
 * @file lv_indev_proc.h
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
void lv_indev_init(void);

/**
 * Get the currently processed input device. Can be used in action functions too.
 * @return pointer to the currently processed input device or NULL if no input device processing right now
 */
lv_indev_t * lv_indev_get_act(void);

/**
 * Reset one or all input devices
 * @param indev pointer to an input device to reset or NULL to reset all of them
 */
void lv_indev_reset(lv_indev_t * indev);

/**
 * Reset the long press state of an input device
 * @param indev_proc pointer to an input device
 */
void lv_indev_reset_lpr(lv_indev_t * indev);

/**
 * Enable input devices device by type
 * @param type Input device type
 * @param enable true: enable this type; false: disable this type
 */
void lv_indev_enable(lv_hal_indev_type_t type, bool enable);

/**
 * Set a cursor for a pointer input device
 * @param indev pointer to an input device (type: 'LV_INDEV_TYPE_POINTER')
 * @param cur_obj pointer to an object to be used as cursor
 */
void lv_indev_set_cursor(lv_indev_t *indev, lv_obj_t *cur_obj);

#if USE_LV_GROUP
/**
 * Set a destination group for a keypad input device
 * @param indev pointer to an input device (type: 'LV_INDEV_TYPE_KEYPAD')
 * @param group point to a group
 */
void lv_indev_set_group(lv_indev_t *indev, lv_group_t *group);
#endif
/**
 * Get the last point of an input device
 * @param indev pointer to an input device
 * @param point pointer to a point to store the result
 */
void lv_indev_get_point(lv_indev_t * indev, lv_point_t * point);
/**
 * Check if there is dragging with an input device or not
 * @param indev pointer to an input device
 * @return true: drag is in progress
 */
bool lv_indev_is_dragging(lv_indev_t * indev);

/**
 * Get the vector of dragging of an input device
 * @param indev pointer to an input device
 * @param point pointer to a point to store the vector
 */
void lv_indev_get_vect(lv_indev_t * indev, lv_point_t * point);

/**
 * Get elapsed time since last press
 * @param indev pointer to an input device (NULL to get the overall smallest inactivity)
 * @return Elapsed ticks (milliseconds) since last press
 */
uint32_t lv_indev_get_inactive_time(lv_indev_t * indev);

/**
 * Do nothing until the next release
 * @param indev pointer to an input device
 */
void lv_indev_wait_release(lv_indev_t * indev);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_INDEV_H*/
