/**
 * @file lv_test_indev.h
 *
 */

#ifndef LV_TEST_INDEV_H
#define LV_TEST_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_TEST

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
 * Create a mouse (pointer), keypad, and encoder indevs.
 * They can be controlled via function calls during the test
 */
void lv_test_indev_create_all(void);

/**
 * Get one of the indev created in `lv_test_indev_create_all`
 * @param type  type of the indev to get
 * @return      the indev
 */
lv_indev_t * lv_test_indev_get_indev(lv_indev_type_t type);

/**
 * Move the mouse to the given coordinates.
 * This function doesn't wait, but just changes the state and returns immediately.
 * @param x     the target absolute X coordinate
 * @param y     the target absolute Y coordinate
 */
void lv_test_mouse_move_to(int32_t x, int32_t y);

/**
 * Move the mouse to the center of a widget
 * This function doesn't wait, but just changes the state and returns immediately.
 * @param obj   pointer to an widget
 */
void lv_test_mouse_move_to_obj(lv_obj_t * obj);

/**
 * Move the mouse cursor. Keep the pressed or released state
 * This function doesn't wait, but just changes the state and returns immediately.
 * @param x     the difference in X to move
 * @param y     the difference in Y to move
 */
void lv_test_mouse_move_by(int32_t x, int32_t y);

/**
 *  Make the mouse button pressed.
 *  This function doesn't wait, but just changes the state and returns immediately.
 */
void lv_test_mouse_press(void);

/**
 *  Make the mouse button released.
 *  This function doesn't wait, but just changes the state and returns immediately.
 */
void lv_test_mouse_release(void);

/**
 * Emulate a click on a given point.
 * First set the released state, wait a little, press, wait, and release again.
 * The wait time is 50ms.
 * Internally `lv_timer_handler` is called, meaning all the events will be fired inside this function.
 * @param x     the target absolute X coordinate
 * @param y     the target absolute Y coordinate
 */
void lv_test_mouse_click_at(int32_t x, int32_t y);

/**
 * Emulate a key press.
 * This function doesn't wait, but just changes the state and returns immediately.
 * @param k     the key to press
 */
void lv_test_key_press(uint32_t k);

/**
 * Release the previously press key.
 * This function doesn't wait, but just changes the state and returns immediately.
 * @param k     the key to press
 */
void lv_test_key_release(void);

/**
 * Emulate a key hit.
 * First set the released state, wait a little, press, wait, and release again.
 * The wait time is 50ms.
 * Internally `lv_timer_handler` is called, meaning all the events will be fired inside this function.
 * @param k     the key to hit
 */
void lv_test_key_hit(uint32_t k);


/**
 * Emulate encoder rotation, use positive parameter to rotate to the right
 * and negative to rotate to the left.
 * This function doesn't wait, but just changes the state and returns immediately.
 * @param d     number of encoder ticks to emulate
 */
void lv_test_encoder_add_diff(int32_t d);

/**
 * Emulate an encoder turn a wait 50ms. Use positive parameter to rotate to the right
 * and negative to rotate to the left.
 * Internally `lv_timer_handler` is called, meaning all the events will be fired inside this function.
 * @param d     number of encoder ticks to emulate
 */
void lv_test_encoder_turn(int32_t d);

/**
 * Emulate an encoder press.
 * This function doesn't wait, but just changes the state and returns immediately.
 */
void lv_test_encoder_press(void);

/**
 * Emulate an encoder release.
 * This function doesn't wait, but just changes the state and returns immediately.
 */
void lv_test_encoder_release(void);

/**
 * Emulate am encoder click.
 * First set the released state, wait a little, press, wait, and release again.
 * The wait time is 50ms.
 * Internally `lv_timer_handler` is called, meaning all the events will be fired inside this function.
 */
void lv_test_encoder_click(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEST*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_INDEV_H*/




