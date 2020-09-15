/**
 * @file lv_btn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_btn.h"
#if LV_USE_BTN != 0

#include <string.h>
#include "../lv_core/lv_group.h"
#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_btn"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_btn_signal(lv_obj_t * btn, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a button object
 * @param parent pointer to an object, it will be the parent of the new button
 * @param copy DEPRECATED, will be removed in v9.
 *             Pointer to an other button to copy.
 * @return pointer to the created button
 */
lv_obj_t * lv_btn_create(lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_LOG_TRACE("button create started");

    lv_obj_t * btn;

    btn = lv_obj_create(parent, copy);
    LV_ASSERT_MEM(btn);
    if(btn == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(btn);

    lv_obj_set_signal_cb(btn, lv_btn_signal);

    if(copy == NULL) {
        /*Set layout if the button is not a screen*/
        if(parent) {
            lv_obj_set_size(btn, LV_DPI, LV_DPI / 3);
            lv_obj_set_grid(btn, &lv_grid_center);
        }

        lv_theme_apply(btn, LV_THEME_BTN);
    }

    LV_LOG_INFO("button created");

    return btn;
}

/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the button
 * @param btn pointer to a button object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_btn_signal(lv_obj_t * btn, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(btn, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_GET_TYPE) {
        return _lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);
    }

    return res;
}

#endif
