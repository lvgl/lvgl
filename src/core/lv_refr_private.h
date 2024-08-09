/**
 * @file lv_refr_private.h
 *
 */

#ifndef LV_REFR_PRIVATE_H
#define LV_REFR_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_refr.h"

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
 * Initialize the screen refresh subsystem
 */
void lv_refr_init(void);

/**
 * Deinitialize the screen refresh subsystem
 */
void lv_refr_deinit(void);

/**
 * Invalidate an area on display to redraw it
 * @param area_p pointer to area which should be invalidated (NULL: delete the invalidated areas)
 * @param disp pointer to display where the area should be invalidated (NULL can be used if there is
 * only one display)
 */
void lv_inv_area(lv_display_t * disp, const lv_area_t * area_p);

/**
 * Get the display which is being refreshed
 * @return the display being refreshed
 */
lv_display_t * lv_refr_get_disp_refreshing(void);

/**
 * Set the display which is being refreshed
 * @param disp the display being refreshed
 */
void lv_refr_set_disp_refreshing(lv_display_t * disp);

/**
 * Called periodically to handle the refreshing
 * @param timer pointer to the timer itself
 */
void lv_display_refr_timer(lv_timer_t * timer);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_REFR_PRIVATE_H*/
