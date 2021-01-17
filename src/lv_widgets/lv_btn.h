/**
 * @file lv_btn.h
 *
 */

#ifndef LV_BTN_H
#define LV_BTN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_BTN != 0

/*Testing of dependencies*/
#if LV_USE_CONT == 0
#error "lv_btn: lv_cont is required. Enable it in lv_conf.h (LV_USE_CONT 1)"
#endif

#include "lv_cont.h"
#include "../lv_core/lv_indev.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Possible states of a button.
 * It can be used not only by buttons but other button-like objects too*/
enum {
    LV_BTN_STATE_RELEASED,
    LV_BTN_STATE_PRESSED,
    LV_BTN_STATE_DISABLED,
    LV_BTN_STATE_CHECKED_RELEASED,
    LV_BTN_STATE_CHECKED_PRESSED,
    LV_BTN_STATE_CHECKED_DISABLED,
    _LV_BTN_STATE_LAST, /* Number of states*/
};
typedef uint8_t lv_btn_state_t;

/** Extended data of button*/
typedef struct {
    /** Ext. of ancestor*/
    lv_cont_ext_t cont;

    /** 1: Toggle enabled*/
    uint8_t checkable : 1;
} lv_btn_ext_t;

/**Styles*/
enum {
    LV_BTN_PART_MAIN = LV_OBJ_PART_MAIN,
    _LV_BTN_PART_VIRTUAL_LAST,
    _LV_BTN_PART_REAL_LAST = _LV_OBJ_PART_REAL_LAST,
};
typedef uint8_t lv_btn_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a button object
 * @param par pointer to an object, it will be the parent of the new button
 * @param copy pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
lv_obj_t * lv_btn_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Enable the toggled states. On release the button will change from/to toggled state.
 * @param btn pointer to a button object
 * @param tgl true: enable toggled states, false: disable
 */
void lv_btn_set_checkable(lv_obj_t * btn, bool tgl);

/**
 * Set the state of the button
 * @param btn pointer to a button object
 * @param state the new state of the button (from lv_btn_state_t enum)
 */
void lv_btn_set_state(lv_obj_t * btn, lv_btn_state_t state);

/**
 * Toggle the state of the button (ON->OFF, OFF->ON)
 * @param btn pointer to a button object
 */
void lv_btn_toggle(lv_obj_t * btn);

/**
 * Set the layout on a button
 * @param btn pointer to a button object
 * @param layout a layout from 'lv_cont_layout_t'
 */
static inline void lv_btn_set_layout(lv_obj_t * btn, lv_layout_t layout)
{
    lv_cont_set_layout(btn, layout);
}

/**
 * Set the fit policy in all 4 directions separately.
 * It tells how to change the button size automatically.
 * @param btn pointer to a button object
 * @param left left fit policy from `lv_fit_t`
 * @param right right fit policy from `lv_fit_t`
 * @param top top fit policy from `lv_fit_t`
 * @param bottom bottom fit policy from `lv_fit_t`
 */
static inline void lv_btn_set_fit4(lv_obj_t * btn, lv_fit_t left, lv_fit_t right, lv_fit_t top, lv_fit_t bottom)
{
    lv_cont_set_fit4(btn, left, right, top, bottom);
}

/**
 * Set the fit policy horizontally and vertically separately.
 * It tells how to change the button size automatically.
 * @param btn pointer to a button object
 * @param hor horizontal fit policy from `lv_fit_t`
 * @param ver vertical fit policy from `lv_fit_t`
 */
static inline void lv_btn_set_fit2(lv_obj_t * btn, lv_fit_t hor, lv_fit_t ver)
{
    lv_cont_set_fit2(btn, hor, ver);
}

/**
 * Set the fit policy in all 4 direction at once.
 * It tells how to change the button size automatically.
 * @param btn pointer to a button object
 * @param fit fit policy from `lv_fit_t`
 */
static inline void lv_btn_set_fit(lv_obj_t * btn, lv_fit_t fit)
{
    lv_cont_set_fit(btn, fit);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current state of the button
 * @param btn pointer to a button object
 * @return the state of the button (from lv_btn_state_t enum)
 * If the button is in disabled state `LV_BTN_STATE_DISABLED` will be ORed to the other button states.
 */
lv_btn_state_t lv_btn_get_state(const lv_obj_t * btn);

/**
 * Get the toggle enable attribute of the button
 * @param btn pointer to a button object
 * @return true: checkable enabled, false: disabled
 */
bool lv_btn_get_checkable(const lv_obj_t * btn);

/**
 * Get the layout of a button
 * @param btn pointer to button object
 * @return the layout from 'lv_cont_layout_t'
 */
static inline lv_layout_t lv_btn_get_layout(const lv_obj_t * btn)
{
    return lv_cont_get_layout(btn);
}

/**
 * Get the left fit mode
 * @param btn pointer to a button object
 * @return an element of `lv_fit_t`
 */
static inline lv_fit_t lv_btn_get_fit_left(const lv_obj_t * btn)
{
    return lv_cont_get_fit_left(btn);
}

/**
 * Get the right fit mode
 * @param btn pointer to a button object
 * @return an element of `lv_fit_t`
 */
static inline lv_fit_t lv_btn_get_fit_right(const lv_obj_t * btn)
{
    return lv_cont_get_fit_right(btn);
}

/**
 * Get the top fit mode
 * @param btn pointer to a button object
 * @return an element of `lv_fit_t`
 */
static inline lv_fit_t lv_btn_get_fit_top(const lv_obj_t * btn)
{
    return lv_cont_get_fit_top(btn);
}

/**
 * Get the bottom fit mode
 * @param btn pointer to a button object
 * @return an element of `lv_fit_t`
 */
static inline lv_fit_t lv_btn_get_fit_bottom(const lv_obj_t * btn)
{
    return lv_cont_get_fit_bottom(btn);
}

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BUTTON*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BTN_H*/
