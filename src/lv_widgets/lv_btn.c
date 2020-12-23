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
#define LV_BTN_INK_VALUE_MAX 256
#define LV_BTN_INK_VALUE_MAX_SHIFT 8

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_btn_design(lv_obj_t * btn, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_btn_signal(lv_obj_t * btn, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static lv_design_cb_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a button object
 * @param par pointer to an object, it will be the parent of the new button
 * @param copy pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
lv_obj_t * lv_btn_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("button create started");

    lv_obj_t * btn;

    btn = lv_cont_create(par, copy);
    LV_ASSERT_MEM(btn);
    if(btn == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(btn);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(btn);

    /*Allocate the extended data*/
    lv_btn_ext_t * ext = lv_obj_allocate_ext_attr(btn, sizeof(lv_btn_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(btn);
        return NULL;
    }

    ext->checkable = 0;

    lv_obj_set_signal_cb(btn, lv_btn_signal);
    lv_obj_set_design_cb(btn, lv_btn_design);

    /*If no copy do the basic initialization*/
    if(copy == NULL) {
        /*Set layout if the button is not a screen*/
        if(par) {
            lv_obj_set_size(btn, LV_DPI, LV_DPI / 3);
            lv_btn_set_layout(btn, LV_LAYOUT_CENTER);
        }

        lv_obj_set_click(btn, true); /*Be sure the button is clickable*/

        lv_theme_apply(btn, LV_THEME_BTN);
    }
    /*Copy 'copy'*/
    else {
        lv_btn_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->checkable             = copy_ext->checkable;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(btn, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

    LV_LOG_INFO("button created");

    return btn;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Enable the toggled states
 * @param btn pointer to a button object
 * @param tgl true: enable toggled states, false: disable
 */
void lv_btn_set_checkable(lv_obj_t * btn, bool tgl)
{
    LV_ASSERT_OBJ(btn, LV_OBJX_NAME);

    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);

    ext->checkable = tgl != false ? 1 : 0;
}

/**
 * Set the state of the button
 * @param btn pointer to a button object
 * @param state the new state of the button (from lv_btn_state_t enum)
 */
void lv_btn_set_state(lv_obj_t * btn, lv_btn_state_t state)
{
    LV_ASSERT_OBJ(btn, LV_OBJX_NAME);

    switch(state) {
        case LV_BTN_STATE_RELEASED:
            lv_obj_clear_state(btn, LV_STATE_PRESSED | LV_STATE_CHECKED | LV_STATE_DISABLED);
            break;
        case LV_BTN_STATE_PRESSED:
            lv_obj_clear_state(btn, LV_STATE_CHECKED | LV_STATE_DISABLED);
            lv_obj_add_state(btn, LV_STATE_PRESSED);
            break;
        case LV_BTN_STATE_CHECKED_RELEASED:
            lv_obj_add_state(btn, LV_STATE_CHECKED);
            lv_obj_clear_state(btn, LV_STATE_PRESSED | LV_STATE_DISABLED);
            break;
        case LV_BTN_STATE_CHECKED_PRESSED:
            lv_obj_add_state(btn, LV_STATE_PRESSED | LV_STATE_CHECKED);
            lv_obj_clear_state(btn, LV_STATE_DISABLED);
            break;
        case LV_BTN_STATE_DISABLED:
            lv_obj_clear_state(btn, LV_STATE_PRESSED | LV_STATE_CHECKED);
            lv_obj_add_state(btn, LV_STATE_DISABLED);
            break;
        case LV_BTN_STATE_CHECKED_DISABLED:
            lv_obj_clear_state(btn, LV_STATE_PRESSED);
            lv_obj_add_state(btn, LV_STATE_DISABLED | LV_STATE_CHECKED);
            break;
    }
}

/**
 * Toggle the state of the button (ON->OFF, OFF->ON)
 * @param btn pointer to a button object
 */
void lv_btn_toggle(lv_obj_t * btn)
{
    LV_ASSERT_OBJ(btn, LV_OBJX_NAME);

    if(lv_obj_get_state(btn, LV_BTN_PART_MAIN) & LV_STATE_CHECKED) {
        lv_obj_clear_state(btn, LV_STATE_CHECKED);
    }
    else {
        lv_obj_add_state(btn, LV_STATE_CHECKED);
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current state of the button
 * @param btn pointer to a button object
 * @return the state of the button (from lv_btn_state_t enum).
 * If the button is in disabled state `LV_BTN_STATE_DISABLED` will be ORed to the other button states.
 */
lv_btn_state_t lv_btn_get_state(const lv_obj_t * btn)
{
    LV_ASSERT_OBJ(btn, LV_OBJX_NAME);

    lv_state_t obj_state = lv_obj_get_state(btn, LV_BTN_PART_MAIN);

    if(obj_state & LV_STATE_DISABLED) {
        if(obj_state & LV_STATE_CHECKED) return LV_BTN_STATE_CHECKED_DISABLED;
        else return LV_BTN_STATE_DISABLED;
    }

    if(obj_state & LV_STATE_CHECKED) {
        if(obj_state & LV_STATE_PRESSED) return LV_BTN_STATE_CHECKED_PRESSED;
        else return LV_BTN_STATE_CHECKED_RELEASED;
    }
    else {
        if(obj_state & LV_STATE_PRESSED) return LV_BTN_STATE_PRESSED;
        else return LV_BTN_STATE_RELEASED;
    }
}

/**
 * Get the toggle enable attribute of the button
 * @param btn pointer to a button object
 * @return true: toggle enabled, false: disabled
 */
bool lv_btn_get_checkable(const lv_obj_t * btn)
{
    LV_ASSERT_OBJ(btn, LV_OBJX_NAME);

    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);

    return ext->checkable != 0 ? true : false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the drop down lists
 * @param btn pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_btn_design(lv_obj_t * btn, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_design(btn, clip_area, mode);
    }
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_design(btn, clip_area, mode);
    }
    else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design(btn, clip_area, mode);
    }

    return LV_DESIGN_RES_OK;
}

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
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    bool tgl           = lv_btn_get_checkable(btn);

    if(sign == LV_SIGNAL_RELEASED) {
        /*If not dragged and it was not long press action then
         *change state and run the action*/
        if(lv_indev_is_dragging(param) == false && tgl) {
            uint32_t toggled = 0;
            if(lv_obj_get_state(btn, LV_BTN_PART_MAIN) & LV_STATE_CHECKED) {
                lv_btn_set_state(btn, LV_BTN_STATE_RELEASED);
                toggled = 0;
            }
            else {
                lv_btn_set_state(btn, LV_BTN_STATE_CHECKED_RELEASED);
                toggled = 1;
            }

            res = lv_event_send(btn, LV_EVENT_VALUE_CHANGED, &toggled);
            if(res != LV_RES_OK) return res;
        }
    }
    else if(sign == LV_SIGNAL_CONTROL) {
#if LV_USE_GROUP
        char c = *((char *)param);
        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            if(lv_btn_get_checkable(btn)) {
                lv_btn_set_state(btn, LV_BTN_STATE_CHECKED_RELEASED);

                uint32_t state = 1;
                res            = lv_event_send(btn, LV_EVENT_VALUE_CHANGED, &state);
                if(res != LV_RES_OK) return res;
            }

        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            if(lv_btn_get_checkable(btn)) {
                lv_btn_set_state(btn, LV_BTN_STATE_RELEASED);

                uint32_t state = 0;
                res            = lv_event_send(btn, LV_EVENT_VALUE_CHANGED, &state);
                if(res != LV_RES_OK) return res;
            }
        }
#endif
    }

    return res;
}

#endif
