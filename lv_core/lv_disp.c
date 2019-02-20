/**
 * @file lv_disp.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_disp.h"

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
 * Return with a pointer to the active screen
 * @param disp pointer to display which active screen should be get. (NULL to use the default screen)
 * @return pointer to the active screen object (loaded by 'lv_scr_load()')
 */
lv_obj_t * lv_disp_get_scr_act(lv_disp_t * disp)
{
    if(!disp) disp = lv_disp_get_default();
    if(!disp) {
        LV_LOG_WARN("lv_scr_act: no display registered to get its top layer");
        return NULL;
    }

    return disp->act_scr;
}

/**
 * Make a screen active
 * @param scr pointer to a screen
 */
void lv_disp_set_scr_act(lv_obj_t * scr)
{
    lv_disp_t * d = lv_obj_get_disp(scr);

    d->act_scr = scr;

    lv_obj_invalidate(scr);
}

/**
 * Return with the top layer. (Same on every screen and it is above the normal screen layer)
 * @param disp pointer to display which top layer should be get. (NULL to use the default screen)
 * @return pointer to the top layer object  (transparent screen sized lv_obj)
 */
lv_obj_t * lv_disp_get_layer_top(lv_disp_t * disp)
{
    if(!disp) disp = lv_disp_get_default();
    if(!disp) {
        LV_LOG_WARN("lv_layer_top: no display registered to get its top layer");
        return NULL;
    }

    return disp->top_layer;
}

/**
 * Return with the sys. layer. (Same on every screen and it is above the normal screen and the top layer)
 * @param disp pointer to display which sys. layer  should be get. (NULL to use the default screen)
 * @return pointer to the sys layer object  (transparent screen sized lv_obj)
 */
lv_obj_t * lv_disp_get_layer_sys(lv_disp_t * disp)
{
    if(!disp) disp = lv_disp_get_default();
    if(!disp) {
        LV_LOG_WARN("lv_layer_sys: no display registered to get its top layer");
        return NULL;
    }

    return disp->sys_layer;
}

/**
 * Assign a screen to a display.
 * @param disp pointer to a display where to assign the screen
 * @param scr pointer to a screen object to assign
 */
void lv_disp_assign_screen(lv_disp_t * disp, lv_obj_t * scr)
{
    if(lv_obj_get_parent(scr) != NULL) {
        LV_LOG_WARN("lv_disp_assign_screen: try to assign a non-screen object");
        return;
    }

    lv_disp_t * old_disp = lv_obj_get_disp(scr);

    if(old_disp == disp) return;

    lv_ll_chg_list(&old_disp->scr_ll, &disp->scr_ll, scr);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
