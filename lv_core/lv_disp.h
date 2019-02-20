/**
 * @file lv_disp.h
 *
 */

#ifndef LV_DISP_H
#define LV_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_hal/lv_hal.h"
#include "lv_obj.h"

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
 * Return with a pointer to the active screen
 * @param disp pointer to display which active screen should be get. (NULL to use the default screen)
 * @return pointer to the active screen object (loaded by 'lv_scr_load()')
 */
lv_obj_t * lv_disp_get_scr_act(lv_disp_t * disp);

/**
 * Make a screen active
 * @param scr pointer to a screen
 */
void lv_disp_set_scr_act(lv_obj_t * scr);

/**
 * Return with the top layer. (Same on every screen and it is above the normal screen layer)
 * @param disp pointer to display which top layer should be get. (NULL to use the default screen)
 * @return pointer to the top layer object  (transparent screen sized lv_obj)
 */
lv_obj_t * lv_disp_get_layer_top(lv_disp_t * disp);

/**
 * Return with the sys. layer. (Same on every screen and it is above the normal screen and the top layer)
 * @param disp pointer to display which sys. layer  should be get. (NULL to use the default screen)
 * @return pointer to the sys layer object  (transparent screen sized lv_obj)
 */
lv_obj_t * lv_disp_get_layer_sys(lv_disp_t * disp);

/**
 * Assign a screen to a display.
 * @param disp pointer to a display where to assign the screen
 * @param scr pointer to a screen object to assign
 */
void lv_disp_assign_screen(lv_disp_t * disp, lv_obj_t * scr);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEMPL_H*/
