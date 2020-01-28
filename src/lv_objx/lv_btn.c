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
#include "../lv_core/lv_debug.h"
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

    ext->toggle = 0;

    lv_obj_set_signal_cb(btn, lv_btn_signal);
    lv_obj_set_design_cb(btn, lv_btn_design);

    /*If no copy do the basic initialization*/
    if(copy == NULL) {
        /*Set layout if the button is not a screen*/
        if(par) lv_btn_set_layout(btn, LV_LAYOUT_CENTER);

        lv_obj_set_click(btn, true); /*Be sure the button is clickable*/

        lv_theme_apply(btn, LV_THEME_BTN);
    }
    /*Copy 'copy'*/
    else {
        lv_btn_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->toggle             = copy_ext->toggle;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(btn);
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
void lv_btn_set_toggle(lv_obj_t * btn, bool tgl)
{
    LV_ASSERT_OBJ(btn, LV_OBJX_NAME);

    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);

    ext->toggle = tgl != false ? 1 : 0;
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
        case LV_BTN_STATE_REL:
            lv_obj_clear_state(btn, LV_OBJ_STATE_PRESSED | LV_OBJ_STATE_CHECKED);
            break;
        case LV_BTN_STATE_PR:
            lv_obj_clear_state(btn, LV_OBJ_STATE_CHECKED);
            lv_obj_add_state(btn, LV_OBJ_STATE_PRESSED);
            break;
        case LV_BTN_STATE_TGL_REL:
            lv_obj_add_state(btn, LV_OBJ_STATE_CHECKED);
            lv_obj_clear_state(btn, LV_OBJ_STATE_PRESSED);
            break;
        case LV_BTN_STATE_TGL_PR:
            lv_obj_add_state(btn, LV_OBJ_STATE_PRESSED | LV_OBJ_STATE_CHECKED);
            break;
        case LV_BTN_STATE_INA:
            lv_obj_add_state(btn, LV_OBJ_STATE_DISABLED);
            break;
    }

//    /*Make the state change happen immediately, without transition*/
//    btn->prev_state = btn->state;
}

/**
 * Toggle the state of the button (ON->OFF, OFF->ON)
 * @param btn pointer to a button object
 */
void lv_btn_toggle(lv_obj_t * btn)
{
    LV_ASSERT_OBJ(btn, LV_OBJX_NAME);



    if(lv_obj_get_state(btn, LV_BTN_PART_MAIN) & LV_OBJ_STATE_CHECKED) {
        lv_obj_clear_state(btn, LV_OBJ_STATE_CHECKED);
    } else {
        lv_obj_add_state(btn, LV_OBJ_STATE_CHECKED);
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current state of the button
 * @param btn pointer to a button object
 * @return the state of the button (from lv_btn_state_t enum)
 */
lv_btn_state_t lv_btn_get_state(const lv_obj_t * btn)
{
    LV_ASSERT_OBJ(btn, LV_OBJX_NAME);

    lv_obj_state_t state = lv_obj_get_state(btn, LV_BTN_PART_MAIN);

    if(state & LV_OBJ_STATE_CHECKED) {
        if(state & LV_OBJ_STATE_PRESSED) return LV_BTN_STATE_TGL_PR;
        else return LV_BTN_STATE_TGL_REL;
    } else {
        if(state & LV_OBJ_STATE_PRESSED) return LV_BTN_STATE_PR;
        else return LV_BTN_STATE_REL;
    }

}

/**
 * Get the toggle enable attribute of the button
 * @param btn pointer to a button object
 * @return true: toggle enabled, false: disabled
 */
bool lv_btn_get_toggle(const lv_obj_t * btn)
{
    LV_ASSERT_OBJ(btn, LV_OBJX_NAME);

    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);

    return ext->toggle != 0 ? true : false;
}

lv_style_list_t * lv_btn_get_style(lv_obj_t * cont, uint8_t type)
{
    lv_style_list_t * style_dsc_p;
    switch(type) {
    case LV_BTN_PART_MAIN:
        style_dsc_p = &cont->style_list;
        break;
    default:
        style_dsc_p = NULL;
    }

    return style_dsc_p;
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
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_design(btn, clip_area, mode);
    } else if(mode == LV_DESIGN_DRAW_POST) {
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

    bool tgl           = lv_btn_get_toggle(btn);
    lv_btn_state_t state = lv_btn_get_state(btn);
    if(sign == LV_SIGNAL_PRESSED) {
        /*Refresh the state*/
        if(state == LV_BTN_STATE_REL) {
            lv_btn_set_state(btn, LV_BTN_STATE_PR);
        } else if(state == LV_BTN_STATE_TGL_REL) {
            lv_btn_set_state(btn, LV_BTN_STATE_TGL_PR);
        }
    } else if(sign == LV_SIGNAL_PRESS_LOST) {
        /*Refresh the state*/
        if(state == LV_BTN_STATE_PR)
            lv_btn_set_state(btn, LV_BTN_STATE_REL);
        else if(state == LV_BTN_STATE_TGL_PR)
            lv_btn_set_state(btn, LV_BTN_STATE_TGL_REL);
    } else if(sign == LV_SIGNAL_PRESSING) {
        /*When the button begins to drag revert pressed states to released*/
        if(lv_indev_is_dragging(param) != false) {
            if(state == LV_BTN_STATE_PR)
                lv_btn_set_state(btn, LV_BTN_STATE_REL);
            else if(state == LV_BTN_STATE_TGL_PR)
                lv_btn_set_state(btn, LV_BTN_STATE_TGL_REL);
        }
    } else if(sign == LV_SIGNAL_RELEASED) {
        /*If not dragged and it was not long press action then
         *change state and run the action*/
        if(lv_indev_is_dragging(param) == false) {
            uint32_t toggled = 0;
            if(state == LV_BTN_STATE_PR && tgl == false) {
                lv_btn_set_state(btn, LV_BTN_STATE_REL);
                toggled = 0;
            } else if(state == LV_BTN_STATE_TGL_PR && tgl == false) {
                lv_btn_set_state(btn, LV_BTN_STATE_TGL_REL);
                toggled = 1;
            } else if(state == LV_BTN_STATE_PR && tgl == true) {
                lv_btn_set_state(btn, LV_BTN_STATE_TGL_REL);
                toggled = 1;
            } else if(state == LV_BTN_STATE_TGL_PR && tgl == true) {
                lv_btn_set_state(btn, LV_BTN_STATE_REL);
                toggled = 0;
            }

            if(tgl) {
                res = lv_event_send(btn, LV_EVENT_VALUE_CHANGED, &toggled);
                if(res != LV_RES_OK) return res;
            }
        } else { /*If dragged change back the state*/
            if(state == LV_BTN_STATE_PR) {
                lv_btn_set_state(btn, LV_BTN_STATE_REL);
            } else if(state == LV_BTN_STATE_TGL_PR) {
                lv_btn_set_state(btn, LV_BTN_STATE_TGL_REL);
            }
        }
    } else if(sign == LV_SIGNAL_CONTROL) {
        char c = *((char *)param);
        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            if(lv_btn_get_toggle(btn)) {
                lv_btn_set_state(btn, LV_BTN_STATE_TGL_REL);

                uint32_t state = 1;
                res            = lv_event_send(btn, LV_EVENT_VALUE_CHANGED, &state);
                if(res != LV_RES_OK) return res;
            }

        } else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            if(lv_btn_get_toggle(btn)) {
                lv_btn_set_state(btn, LV_BTN_STATE_REL);

                uint32_t state = 0;
                res            = lv_event_send(btn, LV_EVENT_VALUE_CHANGED, &state);
                if(res != LV_RES_OK) return res;
            }
        }
    }

    return res;
}

#endif
