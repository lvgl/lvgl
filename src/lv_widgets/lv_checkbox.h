/**
 * @file lv_cb.h
 *
 */

#ifndef LV_CHECKBOX_H
#define LV_CHECKBOX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_CHECKBOX != 0

/*Testing of dependencies*/
#if LV_USE_BTN == 0
#error "lv_cb: lv_btn is required. Enable it in lv_conf.h (LV_USE_BTN  1) "
#endif

#if LV_USE_LABEL == 0
#error "lv_cb: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "lv_btn.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of check box*/
typedef struct {
    lv_btn_ext_t bg_btn; /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * bullet; /*Pointer to button*/
    lv_obj_t * label;  /*Pointer to label*/
} lv_checkbox_ext_t;

/** Checkbox styles. */
enum {
    LV_CHECKBOX_PART_BG = LV_BTN_PART_MAIN,  /**< Style of object background. */
    _LV_CHECKBOX_PART_VIRTUAL_LAST,
    LV_CHECKBOX_PART_BULLET = _LV_BTN_PART_REAL_LAST, /**< Style of box (released). */
    _LV_CHECKBOX_PART_REAL_LAST
};
typedef uint8_t lv_checkbox_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a check box objects
 * @param par pointer to an object, it will be the parent of the new check box
 * @param copy pointer to a check box object, if not NULL then the new object will be copied from it
 * @return pointer to the created check box
 */
lv_obj_t * lv_checkbox_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a check box. `txt` will be copied and may be deallocated
 * after this function returns.
 * @param cb pointer to a check box
 * @param txt the text of the check box. NULL to refresh with the current text.
 */
void lv_checkbox_set_text(lv_obj_t * cb, const char * txt);

/**
 * Set the text of a check box. `txt` must not be deallocated during the life
 * of this checkbox.
 * @param cb pointer to a check box
 * @param txt the text of the check box. NULL to refresh with the current text.
 */
void lv_checkbox_set_text_static(lv_obj_t * cb, const char * txt);

/**
 * Set the state of the check box
 * @param cb pointer to a check box object
 * @param checked true: make the check box checked; false: make it unchecked
 */
void lv_checkbox_set_checked(lv_obj_t * cb, bool checked);

/**
 * Make the check box inactive (disabled)
 * @param cb pointer to a check box object
 */
void lv_checkbox_set_disabled(lv_obj_t * cb);

/**
 * Set the state of a check box
 * @param cb pointer to a check box object
 * @param state the new state of the check box (from lv_btn_state_t enum)
 */
void lv_checkbox_set_state(lv_obj_t * cb, lv_btn_state_t state);
/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a check box
 * @param cb pointer to check box object
 * @return pointer to the text of the check box
 */
const char * lv_checkbox_get_text(const lv_obj_t * cb);

/**
 * Get the current state of the check box
 * @param cb pointer to a check box object
 * @return true: checked; false: not checked
 */
static inline bool lv_checkbox_is_checked(const lv_obj_t * cb)
{
    return lv_btn_get_state(cb) == LV_BTN_STATE_RELEASED ? false : true;
}

/**
 * Get whether the check box is inactive or not.
 * @param cb pointer to a check box object
 * @return true: inactive; false: not inactive
 */
static inline bool lv_checkbox_is_inactive(const lv_obj_t * cb)
{
    return lv_btn_get_state(cb) == LV_BTN_STATE_DISABLED ? true : false;
}

/**
 * Get the current state of a check box
 * @param cb pointer to a check box object
 * @return the state of the check box (from lv_btn_state_t enum)
 */
static inline lv_btn_state_t lv_checkbox_get_state(const lv_obj_t * cb)
{
    return lv_btn_get_state(cb);
}

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CHECKBOX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_CHECKBOX_H*/
