/**
 * @file lv_test_indev_gesture.h
 *
 */

#ifndef LV_TEST_INDEV_GESTURE_H
#define LV_TEST_INDEV_GESTURE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_TEST && LV_USE_GESTURE_RECOGNITION

#include "../../misc/lv_types.h"
#include "../../indev/lv_indev.h"

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
 * Create a touch (pointer) indevs.
 * They can be controlled via function calls during the test
 */
void lv_test_indev_gesture_create(void);

/**
 * Delete the touch (pointer) indevs.
 */
void lv_test_indev_gesture_delete(void);

/**
 * Get one of the indev created in `lv_test_indev_gesture_create`
 * @param type  type of the indev to get
 * @return      the indev
 */
lv_indev_t * lv_test_indev_get_gesture_indev(lv_indev_type_t type);

/**
 * Set two touch points data for pinch gesture
 * @param point_0 First touch point coordinates
 * @param point_1 Second touch point coordinates
 */
void lv_test_gesture_set_pinch_data(lv_point_t point_0, lv_point_t point_1);

/**
 * Trigger press state of pinch gesture (both touch points pressed)
 */
void lv_test_gesture_pinch_press(void);

/**
 * Trigger release state of pinch gesture (both touch points released)
 */
void lv_test_gesture_pinch_release(void);

/**
 * Simulate a complete pinch gesture operation
 * @param point_begin_0 Starting coordinates of first touch point
 * @param point_begin_1 Starting coordinates of second touch point
 * @param point_end_0   Ending coordinates of first touch point
 * @param point_end_1   Ending coordinates of second touch point
 */
void lv_test_gesture_pinch(lv_point_t point_begin_0, lv_point_t point_begin_1,
                           lv_point_t point_end_0, lv_point_t point_end_1);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEST && LV_USE_GESTURE_RECOGNITION*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_INDEV_GESTURE_H*/




