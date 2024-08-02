/**
 * @file lv_spinbox.h
 *
 */

#ifndef LV_SPINBOX_H
#define LV_SPINBOX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../textarea/lv_textarea.h"

#if LV_USE_SPINBOX

/*Testing of dependencies*/
#if LV_USE_TEXTAREA == 0
#error "lv_spinbox: lv_ta is required. Enable it in lv_conf.h (LV_USE_TEXTAREA  1) "
#endif

/*********************
 *      DEFINES
 *********************/
#define LV_SPINBOX_MAX_DIGIT_COUNT 10

/**********************
 *      TYPEDEFS
 **********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_spinbox_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a spinbox object
 * @param parent    pointer to an object, it will be the parent of the new spinbox
 * @return          pointer to the created spinbox
 */
lv_obj_t * lv_spinbox_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set spinbox value
 * @param obj   pointer to spinbox
 * @param v     value to be set
 */
void lv_spinbox_set_value(lv_obj_t * obj, int32_t v);

/**
 * Set spinbox rollover function
 * @param obj       pointer to spinbox
 * @param rollover  true or false to enable or disable (default)
 */
void lv_spinbox_set_rollover(lv_obj_t * obj, bool rollover);

/**
 * Set spinbox digit format (digit count and decimal format)
 * @param obj           pointer to spinbox
 * @param digit_count   number of digit excluding the decimal separator and the sign
 * @param sep_pos       number of digit before the decimal point. If 0, decimal point is not
 * shown
 */
void lv_spinbox_set_digit_format(lv_obj_t * obj, uint32_t digit_count, uint32_t sep_pos);

/**
 * Set spinbox step
 * @param obj   pointer to spinbox
 * @param step  steps on increment/decrement. Can be 1, 10, 100, 1000, etc the digit that will change.
 */
void lv_spinbox_set_step(lv_obj_t * obj, uint32_t step);

/**
 * Set spinbox value range
 * @param obj       pointer to spinbox
 * @param range_min maximum value, inclusive
 * @param range_max minimum value, inclusive
 */
void lv_spinbox_set_range(lv_obj_t * obj, int32_t range_min, int32_t range_max);

/**
 * Set cursor position to a specific digit for edition
 * @param obj   pointer to spinbox
 * @param pos   selected position in spinbox
 */
void lv_spinbox_set_cursor_pos(lv_obj_t * obj, uint32_t pos);

/**
 * Set direction of digit step when clicking an encoder button while in editing mode
 * @param obj           pointer to spinbox
 * @param direction     the direction (LV_DIR_RIGHT or LV_DIR_LEFT)
 */
void lv_spinbox_set_digit_step_direction(lv_obj_t * obj, lv_dir_t direction);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get spinbox rollover function status
 * @param obj   pointer to spinbox
 */
bool lv_spinbox_get_rollover(lv_obj_t * obj);

/**
 * Get the spinbox numeral value (user has to convert to float according to its digit format)
 * @param obj   pointer to spinbox
 * @return      value integer value of the spinbox
 */
int32_t lv_spinbox_get_value(lv_obj_t * obj);

/**
 * Get the spinbox step value (user has to convert to float according to its digit format)
 * @param obj   pointer to spinbox
 * @return      value integer step value of the spinbox
 */
int32_t lv_spinbox_get_step(lv_obj_t * obj);

/*=====================
 * Other functions
 *====================*/

/**
 * Select next lower digit for edition by dividing the step by 10
 * @param obj   pointer to spinbox
 */
void lv_spinbox_step_next(lv_obj_t * obj);

/**
 * Select next higher digit for edition by multiplying the step by 10
 * @param obj   pointer to spinbox
 */
void lv_spinbox_step_prev(lv_obj_t * obj);

/**
 * Increment spinbox value by one step
 * @param obj   pointer to spinbox
 */
void lv_spinbox_increment(lv_obj_t * obj);

/**
 * Decrement spinbox value by one step
 * @param obj   pointer to spinbox
 */
void lv_spinbox_decrement(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SPINBOX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif
#endif /*LV_SPINBOX_H*/
