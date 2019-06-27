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
 * @param disp pointer to display which active screen should be get. (NULL to use the default
 * screen)
 * @return pointer to the active screen object (loaded by 'lv_scr_load()')
 */
lv_obj_t * lv_disp_get_scr_act(lv_disp_t * disp);

/**
 * Make a screen active
 * @param scr pointer to a screen
 */
void lv_disp_load_scr(lv_obj_t * scr);

/**
 * Return with the top layer. (Same on every screen and it is above the normal screen layer)
 * @param disp pointer to display which top layer should be get. (NULL to use the default screen)
 * @return pointer to the top layer object  (transparent screen sized lv_obj)
 */
lv_obj_t * lv_disp_get_layer_top(lv_disp_t * disp);

/**
 * Return with the sys. layer. (Same on every screen and it is above the normal screen and the top
 * layer)
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

/**
 * Get a pointer to the screen refresher task to
 * modify its parameters with `lv_task_...` functions.
 * @param disp pointer to a display
 * @return pointer to the display refresher task. (NULL on error)
 */
lv_task_t * lv_disp_get_refr_task(lv_disp_t * disp);

/**
 * Get elapsed time since last user activity on a display (e.g. click)
 * @param disp pointer to an display (NULL to get the overall smallest inactivity)
 * @return elapsed ticks (milliseconds) since the last activity
 */
uint32_t lv_disp_get_inactive_time(const lv_disp_t * disp);

/**
 * Manually trigger an activity on a display
 * @param disp pointer to an display (NULL to use the default display)
 */
void lv_disp_trig_activity(lv_disp_t * disp);

/*------------------------------------------------
 * To improve backward compatibility
 * Recommended only if you have one display
 *------------------------------------------------*/

/**
 * Get the active screen of the default display
 * @return pointer to the active screen
 */
static inline lv_obj_t * lv_scr_act(void)
{
    return lv_disp_get_scr_act(lv_disp_get_default());
}

/**
 * Get the top layer  of the default display
 * @return pointer to the top layer
 */
static inline lv_obj_t * lv_layer_top(void)
{
    return lv_disp_get_layer_top(lv_disp_get_default());
}

/**
 * Get the active screen of the deafult display
 * @return  pointer to the sys layer
 */
static inline lv_obj_t * lv_layer_sys(void)
{
    return lv_disp_get_layer_sys(lv_disp_get_default());
}

static inline void lv_scr_load(lv_obj_t * scr)
{
    lv_disp_load_scr(scr);
}

/**********************
 *      MACROS
 **********************/

/*------------------------------------------------
 * To improve backward compatibility
 * Recommended only if you have one display
 *------------------------------------------------*/

#ifndef LV_HOR_RES
#define LV_HOR_RES lv_disp_get_hor_res(lv_disp_get_default())
#endif

#ifndef LV_VER_RES
#define LV_VER_RES lv_disp_get_ver_res(lv_disp_get_default())
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEMPL_H*/
