/**
 * @file lv_refr.h
 *
 */

#ifndef LV_REFR_H
#define LV_REFR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "../display/lv_display.h"
#include "../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Redraw the invalidated areas now.
 * Normally the redrawing is periodically executed in `lv_timer_handler` but a long blocking process
 * can prevent the call of `lv_timer_handler`. In this case if the GUI is updated in the process
 * (e.g. progress bar) this function can be called when the screen should be updated.
 * @param disp pointer to display to refresh. NULL to refresh all displays.
 */
void lv_refr_now(lv_display_t * disp);

/**
 * Redrawn on object and all its children using the passed draw context
 * @param layer pointer to a layer where to draw.
 * @param obj   the start object from the redraw should start
 */
void lv_obj_redraw(lv_layer_t * layer, lv_obj_t * obj);

/**********************
 *   STATIC FUNCTIONS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_REFR_H*/
