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

#include "../lvgl_public.h"

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
 * @param area_p pointer to area which should be invalidated
 * @param disp pointer to display where the area should be invalidated (NULL can be used if there is
 * only one display)
 * @return LV_RESULT_OK: the area is invalidated; LV_RESULT_INVALID: the area wasn't invalidated.
 */
lv_result_t lv_inv_area(lv_display_t * disp, const lv_area_t * area_p);

/**
 * Drop every invalidated area of a display, so nothing is redrawn because of them.
 * It also drops the redraw requests the Widgets have made but the refresh has not turned
 * into areas yet, so the display really is left with nothing to redraw.
 * @param disp pointer to a display (NULL can be used if there is only one display)
 */
void lv_refr_clear_invalid_areas(lv_display_t * disp);

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
 * Search the most top object which fully covers an area
 * @param area_p pointer to an area
 * @param obj the first object to start the searching (typically a screen)
 * @return
 */
lv_obj_t * lv_refr_get_top_obj(const lv_area_t * area_p, lv_obj_t * obj);

/**
 * Render an object to a layer
 * @param layer target drawing layer
 * @param obj   object to render
 */
void lv_obj_refr(lv_layer_t * layer, lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_REFR_PRIVATE_H*/
