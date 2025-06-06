/**
 * @file lv_draw_eve_display.h
 *
 */

#ifndef LV_DRAW_EVE_DISPLAY_H
#define LV_DRAW_EVE_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_DRAW_EVE

#include "../../draw/eve/lv_draw_eve_target.h"

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
 * Create a display for the EVE draw unit.
 * @param params      Pointer to a struct of display parameters. Can be a temporary variable
 * @param op_cb       A callback that will be called to perform pin and SPI IO operations with the EVE chip
 * @param user_data   use `lv_draw_eve_display_get_user_data` to get this pointer inside the `op_cb`
 * @return            the EVE display
 */
lv_display_t * lv_draw_eve_display_create(const lv_draw_eve_parameters_t * params, lv_draw_eve_operation_cb_t op_cb,
                                          void * user_data);

/**
 * Get the `user_data` parameter that was passed to `lv_draw_eve_display_create`. Useful in the operation callback.
 * @param disp      pointer to the lv_draw_eve display
 * @return          the `user_data` pointer
 */
void * lv_draw_eve_display_get_user_data(lv_display_t * disp);

/**
 * Create a touchscreen indev for the EVE display. This function should be called after the EVE display
 * has been created and it is the default display. Otherwise, set it with `lv_indev_set_display`.
 * @return            the EVE touchscreen indev
 */
lv_indev_t * lv_draw_eve_touch_create(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_EVE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_EVE_DISPLAY_H*/
